#!/bin/bash

tag=$(git tag --sort v:refname | tail -n 1)
release=cursoryx_bin_"$tag"

docker cp cursoryx_container_gcc:/cursoryx/$release .
