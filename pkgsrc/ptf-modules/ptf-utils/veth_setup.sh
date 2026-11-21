#!/usr/bin/env bash
noOfVeths=64
if [ $# -eq 1 ]; then
    noOfVeths=$1
fi
echo "No of Veths is $noOfVeths"
let "vethpairs=$noOfVeths/2"
last=`expr $vethpairs - 1`
veths=`seq 0 1 $last`
echo "Adding CPU veth"
veths+=" 125"

set -e

for i in $veths; do
    intf0="veth$(($i*2))"
    intf1="veth$(($i*2+1))"
    if ! ip link show $intf0 &> /dev/null; then
        ip link add name $intf0 type veth peer name $intf1 &> /dev/null
    fi
    # Prevent IPv6 control packets on test ports even when sysctls are read-only.
    ip link set dev $intf0 addrgenmode none mtu 10240 up
    ip link set dev $intf1 addrgenmode none mtu 10240 up
    in_docker=$(cat /proc/1/cgroup| egrep -i docker || true)

    if [ -z "$in_docker" ]; then
        TOE_OPTIONS="rx tx sg tso ufo gso gro lro rxvlan txvlan rxhash"
        for TOE_OPTION in $TOE_OPTIONS; do
           /sbin/ethtool --offload $intf0 "$TOE_OPTION" off &> /dev/null || true
           /sbin/ethtool --offload $intf1 "$TOE_OPTION" off &> /dev/null || true
        done
    fi
    # do not fail if ipv6 is disabled system-wide
    sysctl net.ipv6.conf.$intf0.disable_ipv6=1 &> /dev/null || true
    sysctl net.ipv6.conf.$intf1.disable_ipv6=1 &> /dev/null || true
    echo "Added: $intf0,$intf1"
done
