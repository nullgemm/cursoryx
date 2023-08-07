#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_win.sh release win
docker run --name cursoryx_container_win_mingw cursoryx_image_win_mingw "$@" &> log
