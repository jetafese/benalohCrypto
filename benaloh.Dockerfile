FROM seahorn/seahorn-llvm14:nightly

## install required pacakges
USER root

## Install latest cmake
RUN apt -y remove --purge cmake
RUN apt -y update
RUN apt -y install wget python3-pip
RUN python3 -m pip install --upgrade pip
RUN pip3 install cmake --upgrade
RUN apt install -y libmlir-14-dev mlir-14-tools


# Assume that docker-build is ran in the top-level directory
WORKDIR /opt
COPY . benalohCrypto

WORKDIR /opt
RUN git clone https://github.com/GaloisInc/cryptol.git
WORKDIR /opt/cryptol
RUN git submodule update --init
RUN ./cry build

# run 
