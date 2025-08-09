#!/bin/bash

ar -x cursoryx_x11.a
ar -x ../cursoryx_elf.a
gcc -shared -o cursoryx_x11.so *.o -lxcb-errors -lxcb-shm -lxcb -lxcb-cursor -lxcb-image -lxcb-randr -lxcb-render -lxcb-render-util -lxcb-sync -lxcb-xfixes -lxcb-xinput -lxcb-xkb -lxcb-xrm -lxkbcommon -lxkbcommon-x11 -lpthread

