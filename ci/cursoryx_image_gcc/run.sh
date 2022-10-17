#!/bin/bash

docker run --name cursoryx_container_gcc cursoryx_image_gcc "$@" &> log
