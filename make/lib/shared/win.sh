#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../../..

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
folder_ninja="build"
folder_objects="$folder_ninja/shared"
folder_cursoryx="cursoryx_bin_$tag"
folder_library="$folder_cursoryx/lib/cursoryx"
mkdir -p "$folder_objects"

# list link flags (order matters)
link+=("-lshcore")
link+=("-lgdi32")
link+=("-ldwmapi")

# list objs (order matters)
obj+=("$folder_objects/cursoryx_win.o")
obj+=("$folder_objects/cursoryx_pe.o")

# parse soname
soname="$folder_library/win/cursoryx_win.dll"

# extract objects from static archives
ar --output "$folder_objects" -x "$folder_library/win/cursoryx_win.a"
ar --output "$folder_objects" -x "$folder_library/cursoryx_pe.a"

# build shared object
x86_64-w64-mingw32-gcc -shared -o $soname "${obj[@]}" "${link[@]}"
