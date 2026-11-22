#!/usr/bin/env python3
"""Run PTF programs concurrently, each with its own model and network namespace."""

import argparse
import asyncio
import os
import signal
import sys
import tempfile
from pathlib import Path
from xml.etree import ElementTree

from model_test_discovery import ARCHITECTURES, discover_programs
from model_test_log import print_failed_stage


RUNNER = Path(__file__).with_name("run-test.py")


def positive(value: str) -> int:
    number = int(value)
    if number < 1:
        raise argparse.ArgumentTypeError("must be at least 1")
    return number


def print_section(label: str, text: str) -> None:
    if text:
        print(f"  {label}:")
        for line in text.splitlines():
            print(f"    {line}")


def report_cases(program: str, reports: Path) -> bool:
    """Print each result with its own failure detail. Return whether any case failed."""
    count = 0
    failed = False
    for report in sorted(reports.glob("*.xml")):
        for case in ElementTree.parse(report).iter("testcase"):
            count += 1
            result = "PASS"
            problem = case.find("error")
            if problem is None:
                problem = case.find("failure")
            if problem is not None:
                result = "FAIL"
                failed = True
            elif case.find("skipped") is not None:
                result = "SKIP"
            print(f"{result} {program}: {case.attrib['classname']}.{case.attrib['name']}", flush=True)
            if problem is not None:
                detail = problem.text or problem.get("message") or problem.get("type") or ""
                print_section("Failure", detail)
                print_section("stdout", case.findtext("system-out") or "")
                print_section("stderr", case.findtext("system-err") or "")
    if not count:
        raise ValueError(f"no test cases reported in {reports}")
    return failed


async def run_program(program: str, arch: str, args: argparse.Namespace, log_dir: Path) -> int:
    """Delegate one model's lifecycle to run-test.py and capture its output."""
    log = log_dir / f"{program}.log"
    reports = log_dir / program
    command = [
        sys.executable, str(RUNNER), "--sde", str(args.sde),
        "--sde-install", str(args.sde_install), "--arch", arch,
        "--xunit-dir", str(reports), program,
    ]
    if args.use_knet and program == "knet_mgr_test":
        command.append("--use-knet")
    print(f"START {program}", flush=True)
    with log.open("w") as output:
        process = await asyncio.create_subprocess_exec(
            *command,
            stdout=output,
            stderr=asyncio.subprocess.STDOUT,
            start_new_session=True,
        )
        try:
            status = await process.wait()
        finally:
            if process.returncode is None:
                process.terminate()  # Let run-test.py clean up its namespace.
                await process.wait()
    failed_cases = False
    report_error = None
    try:
        failed_cases = report_cases(program, reports)
        if failed_cases:
            status = status or 1
    except (OSError, ElementTree.ParseError, KeyError, ValueError) as error:
        report_error = str(error)
        status = status or 1
    print(f"{'FAIL' if status else 'PASS'} program {program}", flush=True)
    if status:
        if report_error:
            print(f"  {report_error}")
        print_failed_stage(log, cases_reported=failed_cases)
    return status


async def run_suite(args: argparse.Namespace, programs: dict[str, str]) -> int:
    workers = min(args.jobs, len(programs))
    slots = asyncio.Semaphore(workers)
    if args.log_dir:
        args.log_dir.mkdir(parents=True, exist_ok=True)
    log_dir = Path(tempfile.mkdtemp(prefix="p4studio-suite-", dir=args.log_dir)).resolve()
    print(f"Running {len(programs)} programs with {workers} workers. Logs: {log_dir}", flush=True)

    async def run_when_ready(name: str, arch: str) -> int:
        async with slots:
            return await run_program(name, arch, args, log_dir)

    # Cancelling gather stops queued jobs and lets active runners finish cleanup.
    jobs = [run_when_ready(name, arch) for name, arch in programs.items()]
    task = asyncio.gather(*jobs)
    loop = asyncio.get_running_loop()
    for sig in (signal.SIGINT, signal.SIGTERM):
        loop.add_signal_handler(sig, task.cancel)
    try:
        statuses = await task
    except asyncio.CancelledError:
        return 1
    finally:
        for sig in (signal.SIGINT, signal.SIGTERM):
            loop.remove_signal_handler(sig)
    failed = [name for name, status in zip(programs, statuses) if status]
    summary = f"{len(programs) - len(failed)}/{len(programs)} programs passed"
    print(summary)
    if failed:
        print("Failed:", ", ".join(failed))
    if args.summary:
        with args.summary.open("a") as output:
            print(f"### Model tests: {summary}\n", file=output)
            print("| Program | Result |\n| --- | --- |", file=output)
            for name, status in zip(programs, statuses):
                print(f"| `{name}` | {'FAIL' if status else 'PASS'} |", file=output)
    return int(bool(failed))


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("programs", metavar="PROGRAM", nargs="*")
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--all", action="store_true", help="run all installed PTF example programs")
    mode.add_argument("--list", action="store_true", help="list discovered programs and architectures")
    parser.add_argument("--use-knet", action="store_true",
                        help="Give knet_mgr_test exclusive use of the loaded bf_knet module.")
    parser.add_argument(
        "--arch", choices=ARCHITECTURES,
        help="use this model architecture (discovery prefers tofino by default)",
    )
    parser.add_argument(
        "-j", "--jobs", type=positive, default=len(os.sched_getaffinity(0)),
        help="maximum concurrent models (default: available CPUs)",
    )
    parser.add_argument("--sde", type=Path, default=os.environ.get("SDE"))
    parser.add_argument("--sde-install", type=Path, default=os.environ.get("SDE_INSTALL"))
    parser.add_argument("--log-dir", type=Path, help="Parent directory for this run's logs and xUnit reports")
    parser.add_argument("--summary", type=Path, help="Append a Markdown summary to this file")
    args = parser.parse_args()
    args.sde = (args.sde or Path(__file__).resolve().parents[1]).resolve()
    args.sde_install = (args.sde_install or args.sde / "install").resolve()
    for name in args.programs:
        if not name or Path(name).name != name or name in (".", ".."):
            parser.error(f"invalid program name: {name!r}")
    if args.programs and (args.all or args.list):
        parser.error("choose either named programs or discovery")
    if not (args.programs or args.all or args.list):
        parser.error("specify PROGRAM names, --all, or --list")
    return args


def main() -> int:
    args = parse_args()
    if args.programs:
        programs = dict.fromkeys(args.programs, args.arch or "tofino")
    else:
        architectures = (args.arch,) if args.arch else ARCHITECTURES
        try:
            programs = discover_programs(args.sde, args.sde_install, architectures)
        except OSError as error:
            print(f"{Path(__file__).name}: {error}", file=sys.stderr)
            return 2
        if not programs:
            print(f"no installed PTF programs found in {args.sde_install}", file=sys.stderr)
            return 2

    if args.list:
        for name, arch in sorted(programs.items()):
            print(f"{name}\t{arch}")
        return 0
    return asyncio.run(run_suite(args, programs))


if __name__ == "__main__":
    sys.exit(main())
