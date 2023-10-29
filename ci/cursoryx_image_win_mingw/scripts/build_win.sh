#!/bin/sh

git clone https://github.com/nullgemm/cursoryx.git
cd ./cursoryx || exit

# test build
./make/scripts/build.sh "$@"
