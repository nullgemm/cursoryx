#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_x11.h"
#include "x11/x11.h"

#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xfixes.h>

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

void cursoryx_x11_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = malloc(sizeof (struct x11_backend));

	if (backend == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return;
	}

	struct x11_backend zero = {0};
	*backend = zero;

	context->backend_data = backend;

	cursoryx_error_ok(error);
}

void cursoryx_x11_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct cursoryx_x11_data* window_data = data;

	backend->cursor = NULL;
	backend->conn = window_data->conn;
	backend->window = window_data->window;
	backend->screen = window_data->screen;

	int error_xcb = xcb_cursor_context_new(
		backend->conn,
		backend->screen,
		&(backend->cursor));

	if (error_xcb < 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_CREATE_CURSOR);
		return;
	}

	cursoryx_error_ok(error);
}

void cursoryx_x11_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	if (context->cursor == cursor)
	{
		return;
	}

	if (cursor == CURSORYX_NONE)
	{
		xcb_xfixes_query_version(
			backend->conn,
			4,
			0);

		xcb_xfixes_hide_cursor(
			backend->conn,
			backend->window);
	}
	else
	{
		if (context->cursor == CURSORYX_NONE)
		{
			xcb_xfixes_show_cursor(
				backend->conn,
				backend->window);
		}

		xcb_cursor_t id =
			xcb_cursor_load_cursor(
				backend->cursor,
				cursoryx_names_x11[cursor]);

		xcb_change_window_attributes(
			backend->conn,
			backend->window,
			XCB_CW_CURSOR,
			&id);

		xcb_free_cursor(
			backend->conn,
			id);
	}

	context->cursor = cursor;

	cursoryx_error_ok(error);
}

void cursoryx_x11_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	xcb_cursor_context_free(backend->cursor);
	cursoryx_error_ok(error);
}

void cursoryx_x11_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	free(backend);
	cursoryx_error_ok(error);
}

void cursoryx_prepare_init_x11(
	struct cursoryx_config_backend* config)
{
	config->data = NULL;
	config->init = cursoryx_x11_init;
	config->start = cursoryx_x11_start;
	config->set = cursoryx_x11_set;
	config->stop = cursoryx_x11_stop;
	config->clean = cursoryx_x11_clean;
}
