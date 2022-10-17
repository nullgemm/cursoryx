#!/bin/sh

git clone https://github.com/nullgemm/cursoryx.git
cd ./cursoryx || exit

# TODO remove
git checkout next

# test build
./make/scripts/build.sh
