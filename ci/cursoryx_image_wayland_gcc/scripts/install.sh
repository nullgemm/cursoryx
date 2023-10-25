#!/bin/sh

# git						cloning the repo
# bash						running the build scripts
# samurai					building the binaries
# gcc						compiling the code
# musl-dev					standard C library
# wayland                   wayland libs for client, cursor, egl
# wayland-dev               wayland headers

apk add --no-cache \
	git \
	bash \
	samurai \
	gcc \
	musl-dev \
	wayland \
	wayland-dev
