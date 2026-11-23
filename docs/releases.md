# Open P4Studio releases

`Version.txt` versions the open-source distribution independently of the SDE
component version in `install/share/VERSION`. As in P4C, the monthly workflow
opens a version-bump PR on the first day of the month if `main` has changed
since the last tag. It uses the P4.org release App (`RELEASE_APP_ID` and
`RELEASE_APP_PRIVATE_KEY` must be available to this repository).

Merging a change to `Version.txt` on `main` starts publication, including
the initial `0.1.0` release. The workflow builds the Ubuntu 24.04 x86-64 SDE
image with the P4-14 and P4-16 example programs, then runs the full installed
PTF suite (`ci/run-model-tests.py --all`) against that image. Each program runs
on its first available architecture. Any failed program blocks publication;
the test step prints the failed program's output in the Actions log. On success
it pushes `ghcr.io/p4lang/open-p4studio:v<VERSION>` and creates a GitHub
Release with the image digest and these downloadable assets:

* `tofino-model-v<VERSION>-ubuntu24.04-amd64.tar.gz`: the simulator and its
  matching `libcli`. Install `libpcap0.8t64` on Ubuntu 24.04, extract the
  archive, and keep `bin/` and `lib/` together. The simulator needs raw socket
  privileges. Use the image if you also need `bf_switchd`, PTF or SDE configs.
* The `.sha256` file: run `sha256sum -c <archive>.sha256` beside the archive.

Other pushes to `main` publish the development image as `:latest`. Downstream
CI should pin the digest from the GitHub Release. To retry interrupted
publication, rerun the failed workflow at its original commit.

## Test a P4C change with the released SDE

The following example runs the image's `tna_counter` PTF suite against output
from **your** Tofino compiler. Use the example source from the same Studio
release as the image. Run on a Linux host with Docker and at least 196 free
2 MiB huge pages reserved for a model run.

```bash
# Set IMAGE to ghcr.io/p4lang/open-p4studio@sha256:<digest from the release>.
# Set P4C to the path to the compiler driver under test.
out=$(mktemp -d)
"$P4C" --std p4-16 --target tofino --arch tna \
  --bf-rt-schema "$out/bf-rt.json" -o "$out" \
  pkgsrc/p4-examples/p4_16_programs/tna_counter/tna_counter.p4

docker run --rm --init --cap-add NET_ADMIN --cap-add SYS_ADMIN \
  --security-opt apparmor=unconfined \
  --mount "type=bind,src=$out,dst=/open-p4studio/install/share/tofinopd/tna_counter,readonly" \
  "$IMAGE" ./ci/run-test.py tna_counter
```

The mount replaces the entire example pipeline, including the BF-RT schema,
context and device binary. The image supplies the runtime and the packet tests.
