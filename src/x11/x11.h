#ifndef H_CURSORYX_X11_PRIVATE
#define H_CURSORYX_X11_PRIVATE

#include "cursoryx.h"

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>

struct x11_backend
{
	xcb_cursor_context_t* cursor;
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
};

void cursoryx_x11_init(
	struct cursoryx* context);

void cursoryx_x11_start(
	struct cursoryx* context,
	void* data);

void cursoryx_x11_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor);

void cursoryx_x11_stop(
	struct cursoryx* context);

void cursoryx_x11_clean(
	struct cursoryx* context);

#endif
