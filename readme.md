# Cursoryx
Cursoryx is a portable cursor management library. Like
[globox](https://github.com/cylgom/globox)
and
[willis](https://github.com/cylgom/willis)
it aims at implementing the subset of cursor-relevant features available on
Mac OS, Windows, X11 and Wayland.

## Available cursors
Windows, Mac OS and Linux cursor availability matrix
(excluding X11 because it has everything imaginable)
```
|Windows       |Mac OS        |libwayland-cursors|
|              |              |(minimal set)     |
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
|Size (N/S)    |Size (N/S)    |Size (N)      |Size (N/S)    |Size (N)      |
|Size (W/E)    |Size (W/E)    |Size (W)      |Size (W/E)    |Size (W)      |
|Text          |Text          |Text          |Text          |Text          |
```

[Windows system cursors reference](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setsystemcursor)
[Mac OS usable cursors reference](https://developer.apple.com/documentation/appkit/nscursor)
[X11 cursors reference](https://gitlab.freedesktop.org/xorg/lib/libx11/-/blob/master/include/X11/cursorfont.h)
[Wayland cursors reference](https://gitlab.freedesktop.org/wayland/wayland/-/blob/master/cursor/cursor-data.h)

## Usage
