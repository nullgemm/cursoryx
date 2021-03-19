#ifndef H_CURSORYX_X11
#define H_CURSORYX_X11

#include <stdbool.h>
#include <stdint.h>

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>

struct cursoryx_data_x11
{
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
};

struct cursoryx_x11
{
	xcb_cursor_context_t* context;
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
};

#endif
