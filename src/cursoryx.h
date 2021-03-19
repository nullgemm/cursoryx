#ifndef H_CURSORYX
#define H_CURSORYX

#include <stdbool.h>

#if defined(CURSORYX_X11)
	#include "cursoryx_x11.h"
#elif defined(CURSORYX_WAYLAND)
	#include "cursoryx_wayland.h"
#elif defined(CURSORYX_WINDOWS)
	#include "cursoryx_windows.h"
#elif defined(CURSORYX_MACOS)
	#include "cursoryx_macos.h"
#endif

enum cursoryx_cursor
{
	CURSORYX_ARROW = 0,
	CURSORYX_HAND,

	CURSORYX_CROSSHAIR,
	CURSORYX_TEXT,

	CURSORYX_UNABLE,
	CURSORYX_BUSY,

	CURSORYX_SIZE_ALL,   /*  plus shape  +  */
	CURSORYX_SIZE_NE_SW, /*  diagonal    \  */
	CURSORYX_SIZE_NW_SE, /*  diagonal    /  */
	CURSORYX_SIZE_N_S,   /*  vertical    |  */
	CURSORYX_SIZE_W_E,   /*  horizontal  -  */

	CURSORYX_NONE,
	CURSORYX_COUNT,
};

struct cursoryx
{
	enum cursoryx_cursor current;

	#if defined(CURSORYX_X11)
	struct cursoryx_x11 cursoryx_x11;
	#elif defined(CURSORYX_WAYLAND)
	struct cursoryx_wayland cursoryx_wayland;
	#elif defined(CURSORYX_WINDOWS)
	struct cursoryx_windows cursoryx_windows;
	#elif defined(CURSORYX_MACOS)
	struct cursoryx_macos cursoryx_macos;
	#endif
};

bool cursoryx_start(struct cursoryx* cursoryx, void* data);
void cursoryx_set(struct cursoryx* cursoryx, enum cursoryx_cursor cursor);
void cursoryx_stop(struct cursoryx* cursoryx);

#endif
