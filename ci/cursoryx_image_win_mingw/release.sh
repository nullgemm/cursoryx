#!/bin/bash

echo "please clean docker"

sudo rm -rf cursoryx_bin_v* log
sudo docker rm cursoryx_container_win_mingw
sudo docker rmi cursoryx_image_win_mingw
sudo docker rmi alpine:edge

sudo ./build.sh
sudo ./run.sh /scripts/build_win.sh release win native
sudo ./artifact.sh

sudo chown -R $(id -un):$(id -gn) cursoryx_bin_v*
