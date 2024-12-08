FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install all the toolchain dependencies for devcontainer
RUN apt-get update && apt-get install -y  \
    gdb gcc g++ cmake \
    nlohmann-json3-dev libboost-all-dev doctest-dev\
    && apt-get autoremove \
    && rm -rf /var/lib/apt/lists/* \
