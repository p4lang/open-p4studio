"""Find PTF example programs that have an installed model pipeline."""

import json
import sys
from collections.abc import Iterator, Sequence
from pathlib import Path


ARCHITECTURES = ("tofino", "tofino2", "tofino2m", "tofino3")


def example_directories(sde: Path) -> Iterator[Path]:
    examples = sde / "pkgsrc/p4-examples"
    for group in ("ptf-tests", "p4_16_programs"):
        for directory in sorted((examples / group).iterdir()):
            if any(module.name != "__init__.py" for module in directory.glob("*.py")):
                yield directory


def config_name(directory: Path) -> str:
    override = directory / "custom_conf_file"
    if override.is_file():
        return override.read_text().strip()
    return f"{directory.name}.conf"


def pipeline_binaries(config: Path) -> Iterator[str]:
    data = json.loads(config.read_text())

    # Legacy PD configs name the binary directly on the program.
    for program in data.get("p4_program_list", []):
        if program.get("tofino-bin"):
            yield program["tofino-bin"]

    # BF-RT configs name one binary per pipeline.
    for device in data.get("p4_devices", []):
        for program in device["p4_programs"]:
            for pipeline in program["p4_pipelines"]:
                yield pipeline["config"]


def discover_programs(sde: Path, install: Path, architectures: Sequence[str]) -> dict[str, str]:
    """Choose the first installed architecture for each example that has tests."""
    targets = install / "share/p4/targets"
    programs = {}
    for directory in example_directories(sde):
        name = config_name(directory)
        for arch in architectures:
            config = targets / arch / name
            if not config.is_file():
                continue
            # CMake can generate configs even for architectures it did not build.
            binaries = list(pipeline_binaries(config))
            if binaries and all((install / path).is_file() for path in binaries):
                programs[directory.name] = arch
                break
        else:
            print(
                f"Skipping {directory.name}: no installed pipeline for {name} on "
                f"{', '.join(architectures)}",
                file=sys.stderr,
            )
    return programs
