#ifndef H_CURSORYX_X11
#define H_CURSORYX_X11

#include "cursoryx.h"

#include <xcb/xcb.h>

struct cursoryx_x11_data
{
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
};

#if !defined(CURSORYX_SHARED)
void cursoryx_prepare_init_x11(
	struct cursoryx_config_backend* config);
#endif

#endif
