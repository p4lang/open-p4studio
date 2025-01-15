Running PTF on HW
=================

PTF is an open-source packet testing framework used extensively in Intel for data-plane testing. PTF works by constructing data packets and sending them over physical ports and then verifying the packets with an expected packet on the received port.

This guide explains how to setup a simple topology to run PTF from a server connected to a DUT.

The examples provided in ptf/examples assume the server connects to a DUT with 4 connections.

```
        |             |
    eth0|-------------|if0
        |             |
    eth1|-------------|if1
DUT ----|             |---- Server
    eth2|-------------|if2
        |             |
    eth3|-------------|if3
        |             |
```
On the server, if0-if3 are linux interfaces for the physical ports.

On the DUT, eth0-eth3 are front panel ports. The scripts assume the ports are 25G connections broken out over a single physical port.

DUT Front ports
---------------
The switch SDK counts physical ports starting at index 0 and goes up to the max value. The SDK starts off assuming there are no ports created. It requires an explicit port create API invocation.

Assuming eth0-eth3 are 400G ports, that would mean the front panel index would be 0, 8, 16, 24.

If they were 25G links broken out over a single link, then the front panel index for port creation would be 0, 1, 2, 3.

Software
--------
The built software artifacts are typically copied to both the server and DUT. The PTF server communicates with the DUT over a thrift connection. The API invocation for configuring the DUT happens over the thrift connection. The actual data packets get sent over the physical ports.

DUT
---
Start the driver application by invoking **"./tools/run_switchd.sh -p switch"**. This basically means the bf_switchd process uses the switch program as the data-plane. If the driver is built with thrift support, a thrift connection is open on port 9091.

After this, the DUT is ready to accept configuration from the server. At this point, the PTF commands can be either executed from the DUT itself, or from a remote server.

Local execution is pretty straight forward and typically done for cases where configuration is the only requirement while traffic is sent/received from an external traffic generator.
```
./tools/run_p4_tests.sh -p switch \
                        -t switch.p4-16/ptf/examples/ \
                        -s <your_own_test>
```

PTF SERVER
----------
The PTF server configuration is a tad more complex the first time it's done. The PTF application needs to be aware of the port mapping between the server and DUT. For that reason, the PTF test script requires a JSON file with the port mapping. A typical port mapping for the above topology, again assuming 4 400G connections would be
```json
{
    "PortToIf": [
        {
            "device_port":0,
            "if":"if0"
        },
        {
            "device_port":8,
            "if":"if1"
        },
       {
            "device_port":16,
            "if":"if2"
        },
        {
            "device_port":24,
            "if":"if3"
        }
    ]
}
```
A typical PTF test would do something like below,
```python
pkt = simple_tcp_packet(eth_dst='00:77:66:55:44:33', ip_dst='10.10.10.1', ip_ttl=64)
exp_pkt = simple_tcp_packet(eth_src='00:77:66:55:44:33', ip_dst='10.10.10.1', ip_ttl=63)
send_packet(self, 0, str(pkt))   # Tx on if0
verify_packet(self, exp_pkt, 8)  # Rx on if1
```
The PTF process is now invoked using
```
./tools/run_p4_tests.sh -p switch \
                        -t switch.p4-16/ptf/api/ \
                        --ip 10.232.10.143 \
                        --arch tofino2 \
                        --target hw \
                        --no-veth \
                        --no-status-srv \
                        -f ~/ports.json \
                        -s switch_l2.L2VlanTest \
                        --test-params="port_speed='25g';skip_ports_check=False"

       -p - Testing the "switch" data-plane
       -t - Location of tests
       -s - Test to run, run L2VLanTest from switch_l2.py file
   --arch - Tofino or Tofino2
 --target - Indicate PTF that tests are running on HW
     --ip - DUT mgmt IP for thrift connection
--no-veth - Skip veth setup (for model)
--no-status-srv - Fail test if no connection found to DUT
--test-params - Optional test parameters:
                port_speed - e.g. 25g, sets ports speed
                skip_ports_check - if True, ports state check will be skipped
```

PTF Tests
---------
The switch.p4-16 packages has an extensive set of tests to verify the data-plane functionality for all switch features. All tests are present under the ptf/api directory and a great source to explore configuration steps for switch API objects.

The API tests have a helper module which eases pre-configuration steps like connecting to DUT, basic port configuration from the JSON file and other helper routines. All tests in ptf/api inherit the ApiHelper class to use these helper routines. Some important helpers explained below
```
configure()
    Add ports from ports JSON file described above accessible via self.port0, self.port1, etc. These are not port indices but the actual port handles returned by the SDK upon port create. Execute further operations on the ports using these handles. If --test-params option has been used, ports speed is set based on provided value and/or ports state check
    is skipped or performed.
    Access the port indices using the self.devports[<>] objects. These are internal to PTF.
    Add 4 vlan accessible via self.vlan10, self.vlan20, self.vlan30, self.vlan40
    Add a default router MAC entry for routed tests
    Add a VRF accessible via self.vrf10
cleanup()
    Reverse of above routine except for the ports which are not deleted. Further PTF tests will use the ports added above
attribute_get()
    Get an attribute of an object given an object handle
attribute_set()
    Set an attribute of an object given an object handle and attribute value
```

A much smaller set of examples are present at ptf/examples. These are primarily for customer to get used to the PTF programming model before attempting larger tests.

The PTF tests use the Python wrapper APIs which are auto-generated from the object model. View them either in the build directory under **build/api/gen-py/api_adapter.py** or in the install directory under **install/lib/python2.7/site-packages/bf_switcht_api_thrift/api_adapter.py**.
