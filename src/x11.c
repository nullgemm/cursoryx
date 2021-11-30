#define _POSIX_C_SOURCE 200809L

#include "cursoryx.h"
#include "cursoryx_x11.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xfixes.h>

// X cursor names sparse LUT
static char* cursoryx_names_x11[] =
{
	[CURSORYX_ARROW]      = "default",
	[CURSORYX_HAND]       = "pointer",
	[CURSORYX_CROSSHAIR]  = "crosshair",
	[CURSORYX_TEXT]       = "text",
	[CURSORYX_UNABLE]     = "not-allowed",
	[CURSORYX_BUSY]       = "wait",
	[CURSORYX_SIZE_ALL]   = "all-scroll",
	[CURSORYX_SIZE_NE_SW] = "nesw-resize",
	[CURSORYX_SIZE_NW_SE] = "nwse-resize",
	[CURSORYX_SIZE_N_S]   = "row-resize",
	[CURSORYX_SIZE_W_E]   = "col-resize",
};

bool cursoryx_start(
	struct cursoryx* cursoryx,
	void* data)
{
	int ok;
	struct cursoryx_data_x11* x11 = data;

	cursoryx->current = CURSORYX_COUNT;
	cursoryx->cursoryx_x11.conn = x11->conn;
	cursoryx->cursoryx_x11.window = x11->window;
	cursoryx->cursoryx_x11.screen = x11->screen;

	ok = xcb_cursor_context_new(
		cursoryx->cursoryx_x11.conn,
		cursoryx->cursoryx_x11.screen,
		&(cursoryx->cursoryx_x11.context));

	if (ok < 0)
	{
		return false;
	}

	return true;
}

void cursoryx_set(
	struct cursoryx* cursoryx,
	enum cursoryx_cursor cursor)
{
	if (cursoryx->current == cursor)
	{
		return;
	}

	if (cursor == CURSORYX_NONE)
	{
		xcb_xfixes_query_version(
			cursoryx->cursoryx_x11.conn,
			4,
			0);

		xcb_xfixes_hide_cursor(
			cursoryx->cursoryx_x11.conn,
			cursoryx->cursoryx_x11.window);
	}
	else
	{
		if (cursoryx->current == CURSORYX_NONE)
		{
			xcb_xfixes_show_cursor(
				cursoryx->cursoryx_x11.conn,
				cursoryx->cursoryx_x11.window);
		}

		xcb_cursor_t id =
			xcb_cursor_load_cursor(
				cursoryx->cursoryx_x11.context,
				cursoryx_names_x11[cursor]);

		xcb_change_window_attributes(
			cursoryx->cursoryx_x11.conn,
			cursoryx->cursoryx_x11.window,
			XCB_CW_CURSOR,
			&id);

		xcb_free_cursor(
			cursoryx->cursoryx_x11.conn,
			id);
	}

	cursoryx->current = cursor;
}

void cursoryx_stop(
	struct cursoryx* cursoryx)
{
	xcb_cursor_context_free(cursoryx->cursoryx_x11.context);
}
