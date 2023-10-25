#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_wayland.sh release wayland
docker run --name cursoryx_container_wayland_gcc cursoryx_image_wayland_gcc "$@" &> log
