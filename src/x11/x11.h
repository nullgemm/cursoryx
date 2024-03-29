#ifndef H_CURSORYX_INTERNAL_X11
#define H_CURSORYX_INTERNAL_X11

#include "cursoryx.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>

struct x11_backend
{
	xcb_cursor_context_t* cursor;
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
	bool xrender;
	bool xfixes;
};

struct x11_custom_backend
{
	xcb_cursor_t cursor;
};

void cursoryx_x11_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_x11_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error);

void cursoryx_x11_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error);

void cursoryx_x11_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

struct cursoryx_custom* cursoryx_x11_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error);

void cursoryx_x11_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

void cursoryx_x11_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_x11_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

#endif
