#ifndef CURSORYX_H
#define CURSORYX_H

#include <stdbool.h>
#include <stdint.h>

#ifdef CURSORYX_WAYLAND
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#endif

#ifdef CURSORYX_X11
#include <xcb/xcb_cursor.h>
#endif

#ifdef CURSORYX_WIN
#include <windows.h>
#endif

#ifdef CURSORYX_QUARTZ
#include <objc/runtime.h>
#endif

#ifdef CURSORYX_WAYLAND
struct cursoryx_wayland
{
	struct wl_compositor* compositor;
	struct wl_pointer* pointer;
	struct wl_shm* shm;
};
#endif

#ifdef CURSORYX_X11
struct cursoryx_x11
{
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
};
#endif

#ifdef CURSORYX_WIN
struct cursoryx_win
{
	HWND handle;
};
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

#ifdef CURSORYX_WAYLAND
	uint32_t time;
	int size;

	// wayland
	struct wl_cursor_theme* theme;
	struct wl_surface* surface;
	struct wl_cursor* cursor;
	struct wl_buffer* buffer;

	// user-supplied
	struct wl_pointer* pointer;
	struct wl_shm* shm;
#endif

#ifdef CURSORYX_X11
	xcb_cursor_context_t* context;
	xcb_connection_t* conn;
	xcb_window_t window;
	xcb_screen_t* screen;
#endif

#ifdef CURSORYX_WIN
	HWND handle;
#endif

#ifdef CURSORYX_QUARTZ
	id current_obj;
#endif
};

bool cursoryx_start(
	struct cursoryx* cursoryx,
	void* data);

void cursoryx_set(
	struct cursoryx* cursoryx,
	enum cursoryx_cursor cursor);

void cursoryx_stop(
	struct cursoryx* cursoryx);

#endif
