# README

This is a sample YAML configuration used with p4studio profile mode builds.

```yaml
global-options:
  asan: false
  asic: false
  bf-python: true
  bfsys-debug-mode: false
  coverage: false
  cpuveth: true
  force32: false
  lto: false
  profiler: false
  tcmalloc: true
  p4ppflags:
  p4flags: "-g --verbose 1"
  extra-cppflags:
  kdir: /path/to/kdir
features:
  bf-diags:
    thrift-diags: true
  bf-platforms:
    accton-diags: false
    bsp: true
    bsp-path: /path/to/bsp
    newport: false
    newport-diags: false
    tclonly: false
  drivers:
    bfrt: true
    bfrt-generic-flags: true
    grpc: true
    p4rt: false
    pi: false
    thrift-driver: true
    kernel-modules: true
  p4-examples:
    - tna_exact_match
  switch:
    sai: true
    thrift-switch: true
architectures:
  - tofino
  - tofino2
```
