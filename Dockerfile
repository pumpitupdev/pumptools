# Use a rather old version of ubuntu to ensure compatibility regarding libc
FROM ubuntu:16.04

LABEL description="Build environment for pumptools"

# Install build dependencies, multilib to get 32-bit versions
RUN dpkg --add-architecture i386
RUN apt-get update && apt-get install -y \
    g++-multilib \
    gcc-multilib \
    zip \
    cmake \
    make \
    git \
    libc6-dev-i386 \
    libusb-1.0-0-dev:i386 \
    libusb-dev:i386 \
    libasound2-dev:i386 \
    libconfig++-dev:i386 \
    libx11-dev:i386 \
    libcurl4-gnutls-dev:i386 \
    libsodium-dev:i386

# Copy files for building to container
RUN mkdir /pumptools
WORKDIR /pumptools

COPY CHANGELOG.md CHANGELOG.md
COPY CMakeLists.txt CMakeLists.txt
COPY CONTRIBUTING.md CONTRIBUTING.md
COPY LICENSE LICENSE
COPY Makefile Makefile
COPY Package.mk Package.mk
COPY README.md README.md
COPY dist dist
COPY doc doc
COPY cmake cmake
COPY src src
# .git folder required or building fails when version is generated
COPY .git .git

# Building
RUN make build package