# Cursoryx
Cursoryx is a portable cursor setting library for X11, Wayland, Windows
and macOS. It is lightweight and self-contained, making it suitable for use
with custom windowing code or low-level abstractions libraries like
[Globox](https://github.com/nullgemm/globox).

## Available cursors
Windows, macOS and Linux cursor availability matrix
(excluding X11 because it does not have a standard)
```
|Windows       |macOS         |libwayland-cursors|
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
Sources:
 - [Windows system cursors reference](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setsystemcursor)
 - [macOS usable cursors reference](https://developer.apple.com/documentation/appkit/nscursor)
 - [X11 cursors reference](https://gitlab.freedesktop.org/xorg/lib/libx11/-/blob/master/include/X11/cursorfont.h)
 - [Wayland cursors reference](https://gitlab.freedesktop.org/wayland/wayland/-/blob/master/cursor/cursor-data.h)

Cursoryx cursors and their equivalents on the target platforms.
```
|Cursoryx      |Windows       |macOS         |X11           |Wayland       |
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

## Building
The build system for Cursoryx consists in bash scripts generating ninja scripts.
The first step is to generate the ninja script; then, this ninja script must
be ran to build the actual binary.

### General steps for the library
You will need 2 modules to be able to use one of Cursoryx' backend:
 - The Cursoryx core, providing code common across platforms.
   This is what implements the main interface of the library,
   to which you will bind the backend of your choice at run time.
 - The Cursoryx backend module, holding all code specific to a platform.
   This is what is actually executed when the interface is used,
   after you bind all the backend's functions to the main library.

All these components are generated using the scripts found in `make/lib`.
They take arguments: execute them alone to get some help about that.

The scripts named after executable file formats generate ninja scripts
to compile the Cursoryx core module. An example use for Linux would be:
```
./make/lib/elf.sh development
```

The scripts named after platforms will generate ninja files to compile backends:
```
./make/lib/x11.sh development
```

All ninja scripts are generated in `make/output`. To compile, simply execute
them using the original `ninja` or the faster `samurai` implementation.
```
ninja -f ./make/output/lib_elf.ninja
ninja -f ./make/output/lib_x11.ninja
```

All the binaries we build are automatically copied in a new `cursoryx_bin` folder
suffixed with the latest tag on the repository, like this: `cursoryx_bin_v0.0.0`.
To copy the library headers here and make the build ready to use and distribute,
we run the core and platform ninja scripts again, adding the `headers` argument:
```
ninja -f ./make/output/lib_elf.ninja headers
ninja -f ./make/output/lib_x11.ninja headers
```

### X11 support
Cursoryx was built using the modern libxcb X11 library instead of libX11.
Make sure all its components are installed before you start compiling.

### Windows support
Our build system relies on the MinGW toolchain to build Windows binaries.

#### Compiling from Windows
To compile the Windows module under Windows, we recommend using the MinGW
toolchain provided by the [MSYS2](https://www.msys2.org) building platform.

For increased comfort we also recommend using Microsoft's "Windows Terminal":
it is available for download outside of the Microsoft Store on the project's
[GitHub](https://github.com/microsoft/terminal/latest).
It is possible to use MSYS2 from the new Windows Terminal with a custom profile:
to do this click the downwards arrow next to the tabs in the terminal window:
this will open the settings menu, from which new profiles can be created.
Create a new empty profile and paste the following command in the field for the
executable path - make sure the arguments are here otherwise it won't work.
```
C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64
```
You can also set the icon path to the following
```
C:/msys64/ucrt64.ico
```
Once your profile is configured, you can open new tabs using it from the main
window, or set it as the default profile from the settings menu.

Whether you decide to go for the Windows terminal or the included MinTTY,
make sure you are using MSYS2 with its UCRT64 environment, this is important.
When your setup is ready, install a basic MinGW toolchain in MSYS2, like this:
```
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain git ninja unzip
```

You can then proceed to the general build steps.

#### Cross-compiling from Linux
To cross-compile the Windows module under Linux, simply install MinGW and build.
Wine is fully supported, so you will be able to test the examples as expected,
but remember Wine is not Windows and does not support transparency or blur.

### macOS support
Our build system relies on the Xcode toolchain to build macOS binaries.

#### Compiling from macOS
To compile the macOS module under macOS, we recommend that you don't download
the entirety of Xcode, as it would be overkill. Instead, simply get the
command-line tools using your favorite installation method.

If you do not want to register an Apple account, it is possible to get the bare
macOS SDK from Apple's "Software Update" servers using some of the scripts in
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk).

If you did not get Xcode, you also have to install git independently
```
brew install git
```

You can then proceed to the general build steps.

#### Cross-compiling from Linux
To cross-compile the macOS module under Linux, we recommed using OSXcross.
The toolchain can be deployed easily without an Apple account by cloning
[instant_macos_sdk](https://github.com/nullgemm/instant_macos_sdk)
and following the instructions in the readme.

You can then proceed to the general build steps.

### All-in-one helper script
For convenience, a helper script can be found in `make/scripts/build.sh`.
It will automatically build the library and an example in a single command line.
To use it, you must supply all the following arguments:
 - a build type
 - a backend name
 - a build toolchain type

For instance:
```
./make/scripts/build.sh development x11 native
```

## Windowing setup
### X11
This backend's initialization data must include the following XCB structures:
 - The XCB connection pointer
 - The XCB root window
 - The XCB window

### Wayland
This backend's initialization data must include the following callbacks:
 - A registry handler callback Cursoryx can call to register its own callback
   to be executed during the `wl_registry` globals enumeration
 - A capabilities handler callback Cursoryx can call to register its own callback
   to be executed during the `wl_seat` capabilities enumeration

## Library usage
### Setting it up
Include the general Cursoryx header and the backend header:
```
#include "cursoryx.h"
#include "cursoryx_x11.h"
```

Initialize configuration structures:
```
struct cursoryx_error_info error = {0};
struct cursoryx_config_backend config = {0};
```

Bind backend implementation:
```
cursoryx_prepare_init_x11(&config);
```

Initialize Cursoryx:
```
struct cursoryx* cursoryx = cursoryx_init(&config, &error);
```

Start Cursoryx with the appropriate backend data:
```
struct cursoryx_x11_data backend_data =
{
    .conn = x11_conn,
    .window = x11_window,
    .root = x11_root_window,
};

cursoryx_start(cursoryx, &backend_data, &error);
```

### Cleaning it up
Stop Cursoryx
```
cursoryx_stop(cursoryx, &error);
```

Perform cleanup
```
cursoryx_clean(cursoryx, &error);
```

### Managing cursors
Create and set custom cursors:
```
struct cursoryx_custom_config setup =
{
    .image = rgba_buffer,
    .width = 32,
    .height = 32,
    .x = 0,
    .y = 0,
};

struct cursoryx_custom* custom = cursoryx_custom_create(cursoryx, &setup, &error);
cursoryx_custom_set(cursoryx, custom, &error);
cursoryx_custom_destroy(cursoryx, custom, &error);
```

Set native cursors:
```
cursoryx_set(cursoryx, CURSORYX_BUSY, &error_cursoryx);
```

Get debug info:
```
const char* code_name = cursoryx_get_event_code_name(cursoryx, info.event_code, &error);
const char* state_name = cursoryx_get_event_state_name(cursoryx, info.event_state, &error);
cursoryx_error_get_code(&error);
cursoryx_error_log(cursoryx, &error);
```

## Testing
### CI
The `ci` folder contains dockerfiles and scripts to generate testing images
and can be used locally, but a `concourse_pipeline.yml` file is also available
here and should be usable with a few modifications in case you want a more
user-friendly experience. Our own Concourse instance will not be made public
since it runs on a home server (mostly for economic reasons).

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
run the batch script inside (make sure your working directory is `default/windows`).

The registry editor might prompt you for authorization; this is normal since
the script automatically adds the two entries of the cursor scheme in the registry.

The mouse settings window will be opened after the installation is done.
Select the `Pointers` tab, choose the `microstorm` cursor scheme and `Apply`.

### macOS
A macOS version of the theme is available in `default/macos` for use with
[MouseCape](https://github.com/alexzielenski/Mousecape).
