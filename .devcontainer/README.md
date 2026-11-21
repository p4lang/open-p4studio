# Run model tests with the native SDE build

Build and install the SDE and the P4 program on an x86-64 Ubuntu 24.04 host first.
Open the checkout at its physical path (`pwd -P`) so that absolute paths from
the build also resolve inside the container. The container mounts the checkout,
including `install/`, read-only at that same path.

In VS Code, select **Dev Containers: Reopen in Container**. From the container
terminal, run:

```bash
./ci/run-test.py tna_counter
```

The runner creates a network namespace and virtual Ethernet interfaces inside
the container. It writes test output under `/tmp`. Rebuild on the host as
usual; the next test run sees the updated installation.
