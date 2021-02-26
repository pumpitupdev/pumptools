# Reference: https://gitlab.com/nvidia/container-images/samples/-/raw/master/opengl/ubuntu16.04/glxgears/Dockerfile
# Use a rather old version of ubuntu to ensure compatibility regarding libc
FROM nvidia/opengl:1.2-glvnd-runtime-ubuntu16.04

LABEL description="Runtime environment for pump games with tools"

ENV DEBIAN_FRONTEND=noninteractive

ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES},display

# Install build dependencies, multilib to get 32-bit versions
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y gcc-multilib
RUN apt-get install -y mesa-utils
RUN apt-get install -y libglu1-mesa:i386
RUN apt-get install -y libusb-0.1-4:i386
RUN apt-get install -y libconfig++9v5:i386
RUN apt-get install -y libxcursor1:i386
RUN apt-get install -y libxinerama1:i386
RUN apt-get install -y libxi6:i386
RUN apt-get install -y libxrandr2:i386
RUN apt-get install -y libxxf86vm1:i386
RUN apt-get install -y libx11-6:i386
RUN apt-get install -y libasound2:i386
RUN apt-get install -y libfreetype6:i386
RUN apt-get install -y libusb-1.0-0:i386
RUN apt-get install -y zlib1g:i386
RUN apt-get install -y libcurl4-gnutls-dev:i386
RUN apt-get install -y lib32tinfo5
RUN apt-get install -y lib32ncurses5
RUN apt-get install -y xorg
RUN apt-get install -y xinit
RUN apt-get install -y x11-xserver-utils
RUN apt-get install -y xserver-xorg-core
RUN apt-get install -y alsa-base
RUN apt-get install -y alsa-utils
RUN apt-get install -y alsa-tools
RUN apt-get install -y gdb
RUN apt-get install -y gdbserver
RUN apt-get install -y strace

RUN rm -rf /var/lib/apt/lists/*

RUN mkdir /piu

WORKDIR /piu

ENTRYPOINT [ "xterm" ]

# TODO need a custom entry point that takes the game folder as an argument to run the run.sh script because we need the xsession etc