"""Delimit runner stages so the suite can replay a failure from the program log."""

from pathlib import Path


_BEGIN = "=== stage: "
_FAILED = "=== stage failed ==="
PTF_STAGE = "PTF tests"


def begin_stage(name: str) -> None:
    print(f"{_BEGIN}{name}", flush=True)


def fail_stage() -> None:
    print(_FAILED, flush=True)


def print_failed_stage(log: Path, *, cases_reported: bool) -> None:
    # Remember offsets rather than buffering a potentially large daemon log.
    stage = "runner setup"
    start = 0
    with log.open(errors="replace") as stream:
        while line := stream.readline():
            if line.startswith(_BEGIN):
                stage = line[len(_BEGIN):].rstrip()
                start = stream.tell()
            elif line.rstrip() == _FAILED:
                break
        if cases_reported and stage == PTF_STAGE:
            return  # The xUnit report has the output for each failed case.
        print(f"  Failed stage: {stage}")
        stream.seek(start)
        for line in stream:
            if line.rstrip() == _FAILED:
                break
            print(f"    {line}", end="")
