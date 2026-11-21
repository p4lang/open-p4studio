FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        bridge-utils ethtool iproute2 libedit2 libexpat1 libpcap0.8t64 libpython3.12t64 \
        procps python3 python3-setuptools sudo \
    && rm -rf /var/lib/apt/lists/*
