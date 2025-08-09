#!/bin/bash

ar -x cursoryx_wayland.a
ar -x ../cursoryx_elf.a
gcc -shared -o cursoryx_wayland.so *.o -lwayland-client -lwayland-cursor -lxkbcommon -lpthread
