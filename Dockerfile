FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    sudo python3 python3-pip build-essential cmake ca-certificates libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Create workspace
WORKDIR /open-p4studio
COPY . .

# The model profile does not need kernel headers or the launcher's kernel
# compiler installation step. The Python entry point applies the same profile.
RUN python3 p4studio profile apply ./p4studio/profiles/docker.yaml \
    && ln -sfn p4c install/bin/bf-p4c \
    && rm -rf build

# Set environment variables
ENV SDE=/open-p4studio
ENV SDE_INSTALL=/open-p4studio/install

CMD ["/bin/bash"]
