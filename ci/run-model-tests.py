#!/usr/bin/env python3
"""Run PTF programs concurrently, each with its own model and network namespace."""

import argparse
import asyncio
import os
import signal
import sys
import tempfile
from pathlib import Path


def positive(value):
    number = int(value)
    if number < 1:
        raise argparse.ArgumentTypeError("must be at least 1")
    return number


async def run_suite(args, cases, log_dir):
    slots = asyncio.Semaphore(args.jobs)
    runner = Path(__file__).with_name("run-test.py")

    async def run(program):
        async with slots:
            log = log_dir / f"{program}.log"
            print(f"START {program}", flush=True)
            with log.open("w") as output:
                proc = await asyncio.create_subprocess_exec(
                    sys.executable, str(runner), "--sde", str(args.sde),
                    "--sde-install", str(args.sde_install), program,
                    stdout=output, stderr=asyncio.subprocess.STDOUT,
                    start_new_session=True,
                )
                try:
                    status = await proc.wait()
                finally:
                    if proc.returncode is None:
                        proc.terminate()  # Let run-test.py clean up its namespace.
                        await proc.wait()
            print(f"{'FAIL' if status else 'PASS'} {program} ({log})", flush=True)
            return status

    # Cancelling gather stops queued jobs and lets active runners finish cleanup.
    task = asyncio.gather(*(run(program) for program in cases))
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
    failed = [name for name, status in zip(cases, statuses) if status]
    print(f"{len(cases) - len(failed)}/{len(cases)} programs passed")
    if failed:
        print("Failed:", ", ".join(failed))
    return int(bool(failed))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("programs", metavar="PROGRAM", nargs="+")
    parser.add_argument("-j", "--jobs", type=positive, default=len(os.sched_getaffinity(0)),
                        help="maximum concurrent models (default: available CPUs)")
    parser.add_argument("--sde", type=Path, default=os.environ.get("SDE") or Path(__file__).resolve().parents[1])
    parser.add_argument("--sde-install", type=Path, default=os.environ.get("SDE_INSTALL"))
    args = parser.parse_args()
    args.sde = args.sde.resolve()
    args.sde_install = (args.sde_install or args.sde / "install").resolve()
    for name in args.programs:
        if not name or Path(name).name != name or name in (".", ".."):
            parser.error(f"invalid program name: {name!r}")
    cases = list(dict.fromkeys(args.programs))
    log_dir = Path(tempfile.mkdtemp(prefix="p4studio-suite-"))
    print(f"Running {len(cases)} programs with {min(args.jobs, len(cases))} workers. Logs: {log_dir}", flush=True)
    return asyncio.run(run_suite(args, cases, log_dir))


if __name__ == "__main__":
    sys.exit(main())
