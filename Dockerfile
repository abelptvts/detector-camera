FROM debian:buster

RUN dpkg --add-architecture armhf && \
    apt-get update && \
	apt-get install -y git \
	    build-essential \
	    cmake \
	    gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf \
	    unzip wget curl \
	    libopencv-videoio-dev:armhf \
	    libssl-dev:armhf
