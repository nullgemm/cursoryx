#!/bin/sh

git clone https://github.com/nullgemm/cursoryx.git
cd ./cursoryx || exit

# test build
export PATH=/scripts/sdk/target/bin/:/bin:/sbin:/usr/bin:/usr/sbin
export LD_LIBRARY_PATH=/scripts/sdk/target/lib/

./make/scripts/build.sh "$@"
