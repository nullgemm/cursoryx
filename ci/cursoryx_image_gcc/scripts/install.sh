#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# samurai					building the binaries
# gcc						compiling the code
# musl-dev					standard C library
# libxcb-dev				libxcb
# xcb-util-cursor-dev		libxcb cursor utils

apk add --no-cache \
	git \
	bash \
	samurai \
	gcc \
	musl-dev \
	libxcb-dev \
	xcb-util-cursor-dev
