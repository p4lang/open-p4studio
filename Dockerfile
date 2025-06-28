FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    sudo python3 python3-pip build-essential cmake ca-certificates libssl-dev \
    && apt-get clean

# Create workspace
WORKDIR /open-p4studio
COPY . .

RUN ./p4studio/p4studio profile apply --jobs $(nproc) ./p4studio/profiles/docker.yaml

# Set environment variables
ENV SDE=/open-p4studio
ENV SDE_INSTALL=/open-p4studio/install

# Create the symlink
RUN ln -s $SDE_INSTALL/bin/p4c $SDE_INSTALL/bin/bf-p4c || true

CMD ["/bin/bash"]
