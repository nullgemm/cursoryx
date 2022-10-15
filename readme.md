# Cursoryx
Cursoryx is a portable cursor management library. Like
[globox](https://github.com/nullgemm/globox)
and
[willis](https://github.com/nullgemm/willis)
it aims at implementing the subset of cursor-relevant features available on
Mac OS, Windows, X11 and Wayland.

## Available cursors
Windows, Mac OS and Linux cursor availability matrix
(excluding X11 because it does not have a standard)
```
|Windows       |Mac OS        |libwayland-cursors|
|(shared set)  |(AppKit set)  |(bundled mini-set)|
|--------------|--------------|------------------|
|Arrow         |Arrow         |Arrow             |
|              |Arrow (Copy)  |                  |
|              |Arrow (Menu)  |                  |
|              |Arrow (Delete)|                  |
|Unable        |Arrow (Unable)|                  |
|Arrow (Help)  |              |                  |
|Arrow (Busy)  |              |                  |
|Busy          |              |Busy              |
|Crosshair     |Crosshair     |                  |
|Hand          |Hand          |Hand              |
|              |Hand (Open)   |                  |
|              |Hand (Closed) |Hand (Closed)     |
|Size (N/S/W/E)|              |                  |
|Size (N-E/S-W)|              |                  |
|Size (N-W/S-E)|              |                  |
|Size (N/S)    |Size (N/S)    |                  |
|Size (W/E)    |Size (W/E)    |                  |
|              |              |Size (N-E)        |
|              |              |Size (S-W)        |
|              |              |Size (N-W)        |
|              |              |Size (S-E)        |
|Size (N)      |Size (N)      |Size (N)          |
|              |Size (S)      |Size (S)          |
|              |Size (W)      |Size (W)          |
|              |Size (E)      |Size (E)          |
|Text          |Text          |Text              |
|              |Text (Vert.)  |                  |
|              |Link          |                  |
```

Cursoryx cursor and their equivalents on the target platforms.
```
|Cursoryx      |Windows       |Mac OS        |X11           |Wayland       |
|--------------|--------------|--------------|--------------|--------------|
|Arrow         |Arrow         |Arrow         |Arrow         |Arrow         |
|Unable        |Unable        |Arrow (Unable)|Unable        |Arrow         |
|Busy          |Busy          |Arrow         |Busy          |Busy          |
|Hand          |Hand          |Hand          |Hand          |Hand          |
|Crosshair     |Crosshair     |Crosshair     |Crosshair     |Arrow         |
|Size (N/S/W/E)|Size (N/S/W/E)|Hand (Closed) |Size (N/S/W/E)|Hand (Closed) |
|Size (N-E/S-W)|Size (N-E/S-W)|Arrow         |Size (N-E/S-W)|Size (N-E)    |
|Size (N-W/S-E)|Size (N-W/S-E)|Arrow         |Size (N-W/S-E)|Size (N-W)    |
|Size (N/S)    |Size (N/S)    |Size (N/S)    |Size (N/S)    |Size (N)      |
|Size (W/E)    |Size (W/E)    |Size (W/E)    |Size (W/E)    |Size (W)      |
|Text          |Text          |Text          |Text          |Text          |
|None          |None (Window) |None (Screen) |None (Window) |None (Window) |
```

[Windows system cursors reference](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setsystemcursor)
 | [Mac OS usable cursors reference](https://developer.apple.com/documentation/appkit/nscursor)
 | [X11 cursors reference](https://gitlab.freedesktop.org/xorg/lib/libx11/-/blob/master/include/X11/cursorfont.h)
 | [Wayland cursors reference](https://gitlab.freedesktop.org/wayland/wayland/-/blob/master/cursor/cursor-data.h)

## Usage
See `cursoryx.h` for the API and
[globox](https://github.com/nullgemm/globox)
for an implementation example.

## Microstorm
The `default` folder contains all the sources and tools required to generate
"microstorm", a simple, flat, shadow-less cursor theme for X11 (and Wayland).
It only provides the 11 Cursoryx cursors but comes with all the necessary
aliases. This cursor theme was designed to look good with both gamma-aware
and linear blending.

### Linux
To generate a release, simply
```
cd default/linux/source/kit
./build.sh
```

You can then copy the `microstorm` folder in `/usr/share/icons/`, and create
symlinks in `/usr/share/icons/default` and `~/.local/share/icons/default`.

### Windows
To install the cursor theme under Windows, open the `default/windows` folder and
run the batch script inside (make sure your working directory `default/windows`.

The registry editor might prompt you for authorization; this is normal since
the script automatically adds the two entries of the cursor scheme in the registry.

The mouse settings window will be opened after the installation is done.
Select the `Pointers` tab, choose the `microstorm` cursor scheme and `Apply`.

### macOS
A macOS version of the theme is available in `default/macos` for use with
[MouseCape](https://github.com/alexzielenski/Mousecape).

## Greetings
Unlucky Morpheus for the cool music
