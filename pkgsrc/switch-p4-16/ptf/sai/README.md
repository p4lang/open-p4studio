## How to run SAI PTF wiht tofino2 



### Start model 


```bash

    ./tools/run_tofino_model.sh --arch tofino2 -p switch -c install/share/p4/targets/tofino2/switch-sai-t2na-cpu-veth.conf.in -f submodules/bf-switch/ptf/tofino_2ports.json

```


### Start Driver 


```bash

    ./tools/run_switchd.sh --arch tofino2 -p switch -c install/share/p4/targets/tofino2/switch-sai-t2na-cpu-veth.conf.in

```


### Run Tests


```bash

    ./tools/run_p4_tests.sh --arch tofino2 -p switch -t submodules/bf-switch/ptf/sai/ --test-params "port_map_file='./submodules/bf-switch/ptf/sai/default_interface_to_front_map.ini'" --target asic-model -f submodules/bf-switch/ptf/tofino_2ports.json -s sail3
```
