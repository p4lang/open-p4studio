#!/usr/bin/env python3

import subprocess
import sys
import os
import signal
import time
import argparse
import threading
import logging
import tempfile
import shutil
import re
from pathlib import Path
from typing import Optional, List, Dict, Tuple, IO, Any

# --- Configuration ---
TESTS_TIMEOUT_SECONDS = 10800
STARTUP_WAIT_SECONDS = 1
TERM_WAIT_SECONDS = 2
DEFAULT_LOG_LEVEL = logging.INFO
APP_NAME = "Tofino P4 PTF Test Runner"
TEMP_PREFIX = f"{APP_NAME.replace(' ', '_')}_"
NETNS_PREFIX = "p4t"  # Short prefix for network namespace names
NETNS_MAX_LEN = 15  # Common practical limit for interface names derived from netns

# --- Globals ---
background_processes: List[subprocess.Popen] = []
_cleanup_running = False
final_exit_code = 0
output_threads: List[threading.Thread] = []
logger = logging.getLogger(APP_NAME)


def setup_logging(level: int = DEFAULT_LOG_LEVEL) -> None:
    """Configures the global logger."""
    log_formatter = logging.Formatter(
        "%(asctime)s - %(levelname)s - [%(threadName)s] - %(message)s"
    )
    log_handler = logging.StreamHandler(sys.stdout)
    log_handler.setFormatter(log_formatter)
    if not logger.handlers:
        logger.addHandler(log_handler)
    logger.setLevel(level)
    logger.propagate = False
    logger.debug("Logging initialized at level %s", logging.getLevelName(level))


def get_augmented_env(sde_install_path: Path, sde_path: Path) -> Dict[str, str]:
    """Creates environment with SDE_INSTALL and SDE set."""
    if not sde_install_path.is_dir():
        raise ValueError("SDE_INSTALL path not valid: %s", sde_install_path)
    if not sde_path.is_dir():
        raise ValueError("SDE path not valid: %s", sde_path)
    env = os.environ.copy()
    env["SDE_INSTALL"] = str(sde_install_path)
    env["SDE"] = str(sde_path)
    logger.debug("Using SDE_INSTALL=%s", env["SDE_INSTALL"])
    logger.debug("Using SDE=%s", env["SDE"])
    return env


def prefix_output(
    pipe: Optional[IO[bytes]],
    prefix: str,
    encoding: str = "utf-8",
    errors: str = "replace",
) -> None:
    """Reads output from a pipe, adds a prefix, and logs to INFO."""
    if not pipe:
        logger.warning("No pipe provided for prefix '%s'", prefix)
        return
    thread_name = threading.current_thread().name
    logger.debug("Output thread '%s' starting for prefix '%s'", thread_name, prefix)
    try:
        for line_bytes in iter(pipe.readline, b""):
            line_str = line_bytes.decode(encoding, errors=errors).rstrip()
            logger.info("%s: %s", prefix, line_str)
    except ValueError:
        logger.warning("Pipe closed unexpectedly for prefix '%s'", prefix)
    except Exception as e:
        logger.error("Error reading output for '%s': %s", prefix, e, exc_info=False)
    finally:
        logger.debug(
            "Output thread '%s' finishing for prefix '%s'", thread_name, prefix
        )
        if pipe and not pipe.closed:
            try:
                pipe.close()
            except Exception:
                pass


# Uses terminate_process from user's reference (no process group kill)
def terminate_process(proc: subprocess.Popen, name: str) -> None:
    """Gracefully terminates a process using Popen object, resorting to SIGKILL."""
    global final_exit_code
    pid = proc.pid
    if proc.poll() is None:
        logger.info("Stopping %s (PID: %d)...", name, pid)
        try:
            proc.terminate()
            try:
                proc.wait(timeout=TERM_WAIT_SECONDS)
                logger.info("%s (PID: %d) terminated gracefully.", name, pid)
            except subprocess.TimeoutExpired:
                if proc.poll() is None:
                    logger.warning("%s (PID: %d) sending SIGKILL...", name, pid)
                    proc.kill()
                    proc.wait(timeout=1)
                    logger.info("%s (PID: %d) killed.", name, pid)
                else:
                    logger.info("%s (PID: %d) terminated after SIGTERM.", name, pid)
            except Exception as e:
                logger.error("Error waiting for %s after SIGTERM: %s", name, e)
                # Fallthrough
        except ProcessLookupError:
            logger.warning("Process %s (PID: %d) not found during SIGTERM.", name, pid)
        except Exception as e:
            logger.error(
                "Error sending SIGTERM to %s (PID: %d): %s", name, pid, e
            )  # Fallthrough

        if proc.poll() is None:
            logger.warning("Force killing %s (PID: %d).", name, pid)
            try:
                proc.kill()
                proc.wait(timeout=1)
                logger.info("%s (PID: %d) killed.", name, pid)
            except Exception as kill_e:
                logger.error(
                    "Failed fallback SIGKILL %s (PID: %d): %s", name, pid, kill_e
                )
    else:
        logger.debug(
            "%s (PID: %d) already stopped (rc: %s).", name, pid, proc.returncode
        )


def run_command_sync(
    cmd: List[str],
    env: Dict[str, str],
    cwd: Path,
    namespace: Optional[str] = None,
    check: bool = True,
    shell: bool = False,
) -> bool:
    """Runs a command synchronously in cwd (optionally namespace), logs, returns success."""
    global final_exit_code
    original_cmd_str = " ".join(cmd)
    exec_cmd = list(cmd)
    if namespace:
        ns_prefix = ["sudo", "-E", "ip", "netns", "exec", namespace]
        exec_cmd = ns_prefix + exec_cmd
        logger.debug(
            "Running command in netns '%s' (cwd: %s): %s",
            namespace,
            cwd,
            " ".join(exec_cmd),
        )
    else:
        logger.debug("Running command synchronously in %s: %s", cwd, " ".join(exec_cmd))
    try:
        result = subprocess.run(
            exec_cmd,
            check=check,
            capture_output=True,
            text=True,
            env=env,
            cwd=cwd,
            shell=shell,
        )
        if result.stdout:
            logger.info(
                "Command [%s] stdout:\n%s",
                original_cmd_str.split()[0],
                result.stdout.strip(),
            )
        if result.stderr:
            logger.warning(
                "Command [%s] stderr:\n%s",
                original_cmd_str.split()[0],
                result.stderr.strip(),
            )
        logger.debug("Command [%s] finished successfully.", original_cmd_str.split()[0])
        return True
    except subprocess.CalledProcessError as e:
        logger.critical("Command [%s] failed (rc: %d).", " ".join(e.cmd), e.returncode)
        if e.stderr:
            logger.error("Stderr:\n%s", e.stderr.strip())
        if e.stdout:
            logger.error("Stdout:\n%s", e.stdout.strip())
        if final_exit_code == 0:
            final_exit_code = e.returncode or 1
        return False
    except FileNotFoundError:
        logger.critical("Cannot execute. '%s' not found.", exec_cmd[0])
        final_exit_code = 127
        return False
    except Exception as e:
        logger.critical(
            "Unexpected error running [%s]: %s", " ".join(exec_cmd), e, exc_info=True
        )
        final_exit_code = 1
        return False


def start_background_process(
    cmd: List[str],
    log_prefix: str,
    env: Dict[str, str],
    cwd: Path,
    namespace: Optional[str] = None,
) -> Optional[subprocess.Popen]:
    """Starts a process in background in cwd (optionally namespace), returns Popen or None."""
    global background_processes, output_threads, final_exit_code
    exec_cmd = list(cmd)
    if namespace:
        ns_prefix = ["sudo", "-E", "ip", "netns", "exec", namespace]
        exec_cmd = ns_prefix + exec_cmd
        logger.info(
            "Starting background process [%s] in netns '%s' (cwd: %s): %s",
            log_prefix,
            namespace,
            cwd,
            " ".join(exec_cmd),
        )
    else:
        logger.info(
            "Starting background process [%s] in %s: %s",
            log_prefix,
            cwd,
            " ".join(exec_cmd),
        )
    try:
        proc = subprocess.Popen(
            exec_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=False,
            env=env,
            cwd=cwd,
        )
        background_processes.append(proc)
        thread = threading.Thread(
            target=prefix_output,
            args=(proc.stdout, log_prefix),
            daemon=True,
            name=f"{log_prefix}OutputThread",
        )
        output_threads.append(thread)
        thread.start()
        time.sleep(0.5)  # Brief check
        if proc.poll() is not None:
            logger.error(
                "Background process [%s] failed/exited immediately (rc: %s). Check logs.",
                log_prefix,
                proc.returncode,
            )
            if final_exit_code == 0:
                final_exit_code = proc.returncode or 1
            return None
        logger.info(
            "Background process [%s] started successfully (PID: %d).",
            log_prefix,
            proc.pid,
        )
        return proc
    except FileNotFoundError:
        logger.critical("Cannot execute. '%s' not found.", exec_cmd[0])
        final_exit_code = 127
        return None
    except Exception as e:
        logger.critical(
            "Failed to start background process [%s]: %s", log_prefix, e, exc_info=True
        )
        final_exit_code = 1
        return None


def check_process_liveness(proc: Optional[subprocess.Popen], name: str) -> bool:
    """Checks if a process is still running."""
    global final_exit_code
    if proc is None:
        logger.debug("Cannot check liveness for non-started process [%s].", name)
        return False
    rc = proc.poll()
    if rc is not None:
        logger.error(
            "Process [%s] (PID: %d) is not alive (rc: %s).", name, proc.pid, rc
        )
        final_exit_code = rc or 1
        return False
    else:
        logger.debug("Process [%s] (PID: %d) is alive.", name, proc.pid)
        return True


def run_tests(
    cmd: List[str],
    timeout: int,
    env: Dict[str, str],
    cwd: Path,
    namespace: Optional[str] = None,
) -> Optional[int]:
    """Runs the test process in cwd (optionally namespace), returns exit code."""
    global output_threads, final_exit_code
    log_prefix = "tests"
    exec_cmd = list(cmd)
    if namespace:
        ns_prefix = ["sudo", "-E", "ip", "netns", "exec", namespace]
        exec_cmd = ns_prefix + exec_cmd
        logger.info(
            "Starting tests process in netns '%s' (cwd: %s): %s (Timeout: %ds)",
            namespace,
            cwd,
            " ".join(exec_cmd),
            timeout,
        )
    else:
        logger.info(
            "Starting tests process in %s: %s (Timeout: %ds)",
            cwd,
            " ".join(exec_cmd),
            timeout,
        )
    proc: Optional[subprocess.Popen] = None
    try:
        proc = subprocess.Popen(
            exec_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=False,
            env=env,
            cwd=cwd,
        )
        thread = threading.Thread(
            target=prefix_output,
            args=(proc.stdout, log_prefix),
            daemon=True,
            name=f"{log_prefix.capitalize()}OutputThread",
        )
        output_threads.append(thread)
        thread.start()
        logger.info("Waiting for tests process (PID: %d) to complete...", proc.pid)
        tests_exit_code = proc.wait(timeout=timeout)
        if tests_exit_code == 0:
            logger.info("Tests completed successfully.")
        else:
            logger.error("Tests failed with exit code %s.", tests_exit_code)
        return tests_exit_code
    except subprocess.TimeoutExpired:
        logger.error("Tests timed out after %d seconds.", timeout)
        if proc:
            terminate_process(proc, "Timed-out tests")
        return 124
    except FileNotFoundError:
        logger.critical("Cannot execute. '%s' not found.", exec_cmd[0])
        final_exit_code = 127
        return 127
    except Exception as e:
        logger.critical(
            "An unexpected error occurred running tests: %s", e, exc_info=True
        )
        if proc and proc.poll() is None:
            terminate_process(proc, "Tests (after error)")
        return 1
    finally:
        if proc and proc.stdout and not proc.stdout.closed:
            try:
                proc.stdout.close()
            except Exception:
                pass


def cleanup() -> None:
    """Cleans up background processes."""
    global _cleanup_running, background_processes
    if _cleanup_running:
        return
    _cleanup_running = True
    logger.info(">>> Initiating cleanup of background processes...")
    for proc in reversed(list(background_processes)):  # Use list copy
        name = f"process {proc.pid}"
        try:
            if proc.args and isinstance(proc.args, (list, tuple)) and proc.args:
                name = os.path.basename(proc.args[-1])  # Try last arg
        except Exception:
            pass
        terminate_process(proc, name)
    background_processes.clear()
    logger.info(">>> Background process cleanup finished.")
    _cleanup_running = False


def signal_handler(signum: int, frame: Any) -> None:
    """Handles termination signals."""
    global final_exit_code, _cleanup_running
    if _cleanup_running:
        logger.warning("Signal %d during cleanup.", signum)
        return
    try:
        signame = signal.Signals(signum).name
    except ValueError:
        signame = f"Signal {signum}"
    logger.warning(">>> Received %s. Initiating cleanup and exit...", signame)
    if final_exit_code == 0:
        final_exit_code = 128 + signum
    # Let finally block handle cleanup order and exit


def join_output_threads(timeout: int = 2) -> None:
    """Waits for output threads to finish."""
    global output_threads
    logger.debug("Joining output threads (timeout: %ds)...", timeout)
    start_time = time.monotonic()
    threads_to_join = list(output_threads)
    for thread in threads_to_join:
        if thread.is_alive():
            elapsed = time.monotonic() - start_time
            remaining_timeout = max(0, timeout - elapsed)
            logger.debug(
                "Joining thread: %s (rem timeout: %.1fs)",
                thread.name,
                remaining_timeout,
            )
            try:
                thread.join(timeout=remaining_timeout)
            except Exception as e:
                logger.warning("Error joining thread %s: %s", thread.name, e)
            if thread.is_alive():
                logger.warning("Output thread %s timed out.", thread.name)
        else:
            logger.debug("Output thread %s already finished.", thread.name)
    output_threads.clear()
    logger.debug("Finished joining output threads.")


def parse_arguments() -> Tuple[argparse.Namespace, Path, Path]:
    """Parses args, validates SDE paths, returns args, sde_install_path, sde_path."""
    parser = argparse.ArgumentParser(
        description=f"{APP_NAME}: Run Tofino model, switchd, and P4 tests."
    )
    parser.add_argument("p4_program", help="Name of the P4 program.")
    parser.add_argument(
        "--sde-install",
        default=os.getenv("SDE_INSTALL"),
        help="Path to SDE install dir.",
    )
    parser.add_argument("--sde", default=os.getenv("SDE"), help="Path to SDE dir.")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable debug logging."
    )
    parser.add_argument(
        "--keep-temp-dir",
        action="store_true",
        help="Do not delete the temporary directory.",
    )
    args = parser.parse_args()
    log_level = logging.DEBUG if args.verbose else DEFAULT_LOG_LEVEL
    setup_logging(log_level)
    if not args.sde_install:
        logger.critical("SDE_INSTALL not specified.")
        sys.exit(1)
    sde_install_path = Path(args.sde_install).resolve()
    if not sde_install_path.is_dir():
        logger.critical("SDE_INSTALL dir not found: %s", sde_install_path)
        sys.exit(1)
    if not args.sde:
        logger.critical("SDE not specified.")
        sys.exit(1)
    sde_path = Path(args.sde).resolve()
    if not sde_path.is_dir():
        logger.critical("SDE dir not found: %s", sde_path)
        sys.exit(1)
    logger.info("Using P4 Program: %s", args.p4_program)
    logger.info("Using SDE Install Path: %s", sde_install_path)
    logger.info("Using SDE Path: %s", sde_path)
    if args.keep_temp_dir:
        logger.info("Will keep temporary directory.")
    return args, sde_install_path, sde_path


def check_prerequisites(root_dir: Path, sde_install_path: Path) -> Dict[str, Path]:
    """Checks required scripts, returns dict of absolute paths."""
    scripts = {
        "veth_setup": sde_install_path / "bin" / "veth_setup.sh",
        "run_tofino_model": root_dir / "run_tofino_model.sh",
        "run_switchd": root_dir / "run_switchd.sh",
        "run_p4_tests": root_dir / "run_p4_tests.sh",
    }
    all_ok = True
    resolved_scripts = {}
    for name, script_path in scripts.items():
        abs_path = script_path.resolve()
        resolved_scripts[name] = abs_path
        if not abs_path.is_file():
            logger.critical("Script not found: %s", abs_path)
            all_ok = False
        elif name != "veth_setup" and not os.access(abs_path, os.X_OK):
            logger.critical("Script not executable: %s", abs_path)
            all_ok = False
    if not all_ok:
        raise FileNotFoundError("Prerequisite scripts missing/not executable.")
    logger.debug("All prerequisite scripts found.")
    return resolved_scripts


def generate_unique_netns_name(
    temp_dir_name: str, prefix: str = NETNS_PREFIX, max_len: int = NETNS_MAX_LEN
) -> str:
    """
    Creates a sanitized, unique network namespace name derived from a unique temp dir name.
    """
    # Extract the unique part generated by mkdtemp (usually random chars at the end)
    parts = temp_dir_name.split("_")
    unique_suffix = (
        parts[-1] if len(parts) > 1 and parts[-1] else temp_dir_name[-8:]
    )  # Fallback: last 8 chars if no suffix found

    # Sanitize the unique suffix itself (remove non-alphanumeric)
    sanitized_suffix = re.sub(r"[^a-zA-Z0-9]+", "", unique_suffix)
    if not sanitized_suffix:  # Handle cases where suffix becomes empty
        sanitized_suffix = "xxxx"  # Placeholder

    # Combine prefix and sanitized suffix, respecting max_len
    available_len_for_suffix = max_len - len(prefix) - 1  # Account for prefix + '_'
    if available_len_for_suffix < 1:
        return prefix[:max_len]  # Prefix itself is too long

    truncated_suffix = sanitized_suffix[:available_len_for_suffix]
    netns_name = f"{prefix}_{truncated_suffix}"

    logger.debug(
        "Generated netns name '%s' from temp dir '%s'", netns_name, temp_dir_name
    )
    return netns_name


def create_netns(name: str, env: Dict[str, str], cwd: Path) -> bool:
    """Creates a network namespace."""
    logger.info("Creating network namespace: %s", name)
    cmd = ["sudo", "ip", "netns", "add", name]
    return run_command_sync(cmd, env=env, cwd=cwd, check=True)


def delete_netns(name: str, env: Dict[str, str], cwd: Path) -> None:

    # ip netns pids net0 | xargs kill
    cmd = ["sudo", "ip", "netns", "pids", name, "|", "xargs", "kill"]
    if not run_command_sync(
        cmd, env=env, cwd=cwd, check=False
    ):  # No namespace, don't check exit code strictly
        logger.warning(
            "Failed to kill proces in network namespace '%s' (they may already be gone).",
            name,
        )

    """Delete the namespace and with it all the process running in it."""
    logger.info("Deleting network namespace: %s", name)
    cmd = ["sudo", "ip", "netns", "delete", name]
    if not run_command_sync(
        cmd, env=env, cwd=cwd, check=False
    ):  # No namespace, don't check exit code strictly
        logger.warning(
            "Failed to delete network namespace '%s' (may already be gone).", name
        )
    else:
        logger.debug("Successfully deleted network namespace '%s'.", name)


def main() -> None:
    global final_exit_code, background_processes, output_threads

    args, sde_install_path, sde_path = parse_arguments()

    try:
        this_dir = Path(__file__).parent.resolve(strict=True)
        root_dir = (this_dir / "..").resolve(strict=True)
        logger.debug("Script directory: %s", this_dir)
        logger.debug("Project root directory: %s", root_dir)
    except Exception as e:
        logger.critical("Could not determine script/root directory: %s", e)
        sys.exit(1)

    model_proc: Optional[subprocess.Popen] = None
    switchd_proc: Optional[subprocess.Popen] = None
    temp_dir_path: Optional[Path] = None
    netns_name: Optional[str] = None

    try:
        signal.signal(signal.SIGINT, signal_handler)
        signal.signal(signal.SIGTERM, signal_handler)
        logger.debug("Registered signal handlers.")
    except ValueError:
        logger.warning("Could not register signal handlers.")
    except Exception as e:
        logger.error("Failed to register signal handlers: %s", e)

    # Main try block to ensure finally block runs for cleanup
    try:
        # --- Create Temporary Directory ---
        try:
            # Use the global TEMP_PREFIX
            temp_dir_path_str = tempfile.mkdtemp(prefix=TEMP_PREFIX)
            temp_dir_path = Path(temp_dir_path_str)
            logger.info("Created temporary directory: %s", temp_dir_path)
            # --- Generate Unique Network Namespace Name ---
            netns_name = generate_unique_netns_name(
                temp_dir_path.name
            )  # Use new helper
        except Exception as e:
            logger.critical("Failed to create temporary directory: %s", e)
            raise SystemExit(1)

        # --- Prepare Environment & Scripts ---
        augmented_env = get_augmented_env(sde_install_path, sde_path)
        scripts = check_prerequisites(root_dir, sde_install_path)  # Gets absolute paths

        # --- Create Network Namespace ---
        if not create_netns(netns_name, augmented_env, temp_dir_path):
            logger.critical(
                "Failed to create network namespace '%s'. Aborting.", netns_name
            )
            raise SystemExit(final_exit_code or 1)  # Trigger cleanup

        # --- Run veth_setup (in namespace) ---
        logger.info(
            "Setting up virtual ethernet interfaces in netns '%s'...", netns_name
        )
        ip_cmd = ["ip", "link", "set", "dev", "lo", "up"]
        if not run_command_sync(
            ip_cmd, augmented_env, cwd=temp_dir_path, namespace=netns_name
        ):
            raise SystemExit(final_exit_code or 1)
        logger.info("lo interface setup complete in netns '%s'.", netns_name)

        # --- Run veth_setup (in namespace) ---
        logger.info(
            "Setting up virtual ethernet interfaces in netns '%s'...", netns_name
        )
        veth_cmd = ["sudo", str(scripts["veth_setup"]), "128"]
        if not run_command_sync(
            veth_cmd, augmented_env, cwd=temp_dir_path, namespace=netns_name
        ):
            raise SystemExit(final_exit_code or 1)
        logger.info("veth_setup complete in netns '%s'.", netns_name)

        # --- Start Background Processes (in namespace) ---
        model_cmd = [
            str(scripts["run_tofino_model"]),
            "-p",
            args.p4_program,
            "--arch",
            "tofino",
            "-q",
        ]
        model_proc = start_background_process(
            model_cmd, "model", augmented_env, cwd=temp_dir_path, namespace=netns_name
        )
        if not model_proc:
            raise SystemExit(final_exit_code or 1)

        switchd_cmd = [
            str(scripts["run_switchd"]),
            "-p",
            args.p4_program,
            "--arch",
            "tofino",
        ]
        switchd_proc = start_background_process(
            switchd_cmd,
            "switchd",
            augmented_env,
            cwd=temp_dir_path,
            namespace=netns_name,
        )
        if not switchd_proc:
            raise SystemExit(final_exit_code or 1)

        # --- Wait and Check Liveness ---
        logger.info(
            "Waiting %ds for model/switchd (in netns '%s') to initialize...",
            STARTUP_WAIT_SECONDS,
            netns_name,
        )
        time.sleep(STARTUP_WAIT_SECONDS)

        model_alive = check_process_liveness(model_proc, "model")
        switchd_alive = check_process_liveness(switchd_proc, "switchd")
        if not model_alive or not switchd_alive:
            logger.critical("Background services died before tests could start.")
            raise SystemExit(final_exit_code or 1)

        # --- Run Tests (in namespace) ---
        logger.info("Starting P4 tests in netns '%s'...", netns_name)
        tests_cmd = [
            str(scripts["run_p4_tests"]),
            "-p",
            args.p4_program,
            "--arch",
            "tofino",
        ]
        tests_exit_code = run_tests(
            tests_cmd,
            TESTS_TIMEOUT_SECONDS,
            augmented_env,
            cwd=temp_dir_path,
            namespace=netns_name,
        )

        if (
            final_exit_code == 0
            and tests_exit_code is not None
            and tests_exit_code != 0
        ):
            logger.info(
                "Tests failed/timed out, setting final exit code to %d", tests_exit_code
            )
            final_exit_code = tests_exit_code

        # --- Final Liveness Check ---
        logger.info("Checking background process status after tests.")
        model_alive_after = check_process_liveness(model_proc, "model")
        switchd_alive_after = check_process_liveness(switchd_proc, "switchd")
        if (not model_alive_after or not switchd_alive_after) and final_exit_code == 0:
            logger.error("Background process died prematurely. Setting failure.")
            final_exit_code = 1

    except FileNotFoundError as e:
        logger.critical("Prerequisite check failed: %s", e)
        final_exit_code = 1
    except ValueError as e:
        logger.critical("Path validation failed: %s", e)
        final_exit_code = 1
    except SystemExit as e:
        logger.info("Exiting (code: %s).", e.code)
        final_exit_code = e.code if isinstance(e.code, int) else 1
    except Exception as e:
        logger.critical("--- UNEXPECTED SCRIPT ERROR ---")
        logger.critical("Error: %s", e, exc_info=True)
        final_exit_code = 1
    finally:
        # --- Cleanup Order: Processes -> Namespace -> Temp Dir ---
        cleanup()
        join_output_threads()

        # Delete network namespace (if name was generated)
        if netns_name:
            try:
                # Need env and cwd. Ensure they exist if main try block failed early.
                current_env = locals().get("augmented_env")
                current_cwd = locals().get("temp_dir_path")
                if not current_env:
                    current_env = get_augmented_env(
                        sde_install_path, sde_path
                    )  # Recreate env if needed
                if not current_cwd:
                    current_cwd = Path(
                        tempfile.gettempdir()
                    )  # Fallback cwd for delete cmd

                delete_netns(netns_name, current_env, current_cwd)
            except Exception as e:
                logger.error("Error during netns deletion logic: %s", e)

        # Conditional Temp Dir Cleanup
        if temp_dir_path and temp_dir_path.exists():
            if final_exit_code == 0 and not args.keep_temp_dir:
                logger.info(
                    "Tests passed. Removing temporary directory: %s", temp_dir_path
                )
                try:
                    shutil.rmtree(temp_dir_path)
                except Exception as e:
                    logger.warning(
                        "Failed to remove temporary directory %s: %s", temp_dir_path, e
                    )
            elif final_exit_code != 0:
                logger.warning(
                    "Tests failed (rc %d). Keeping temporary directory: %s",
                    final_exit_code,
                    temp_dir_path,
                )
            else:
                logger.info(
                    "Keeping temporary directory as requested: %s", temp_dir_path
                )

        logger.info("Script finished. Final exit code: %d", final_exit_code)


if __name__ == "__main__":
    main()
    sys.exit(final_exit_code)
