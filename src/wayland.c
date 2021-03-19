#define _POSIX_C_SOURCE 200809L

#include "cursoryx.h"
#include "cursoryx_wayland.h"
#include "xdg-shell-client-protocol.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <wayland-client.h>
#include <wayland-cursor.h>

// X cursor names sparse LUT for wayland
// we can't reuse X11's LUT because we try to use the default theme provided
// by libwayland-cursors in order to better integrate the application visually
static char* cursoryx_names_wayland[] =
{
	[CURSORYX_ARROW]      = "left_ptr",
	[CURSORYX_HAND]       = "hand1",
	[CURSORYX_CROSSHAIR]  = "left_ptr",
	[CURSORYX_TEXT]       = "xterm",
	[CURSORYX_UNABLE]     = "left_ptr",
	[CURSORYX_BUSY]       = "watch",
	[CURSORYX_SIZE_ALL]   = "grabbing",
	[CURSORYX_SIZE_NE_SW] = "top_right_corner",
	[CURSORYX_SIZE_NW_SE] = "top_left_corner",
	[CURSORYX_SIZE_N_S]   = "top_side",
	[CURSORYX_SIZE_W_E]   = "left_side",
};

bool cursoryx_start(
	struct cursoryx* cursoryx,
	void* data)
{
	struct cursoryx_data_wayland* wayland = data;

	cursoryx->cursoryx_wayland.size = 32;

	cursoryx->cursoryx_wayland.theme =
		wl_cursor_theme_load(
			NULL,
			cursoryx->cursoryx_wayland.size,
			wayland->shm);

	if (cursoryx->cursoryx_wayland.theme == NULL)
	{
		return false;
	}

	cursoryx->cursoryx_wayland.cursor =
		wl_cursor_theme_get_cursor(
			cursoryx->cursoryx_wayland.theme,
			"left_ptr");

	if (cursoryx->cursoryx_wayland.cursor == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->cursoryx_wayland.theme);

		return false;
	}

	cursoryx->cursoryx_wayland.surface = wl_compositor_create_surface(wayland->compositor);

	if (cursoryx->cursoryx_wayland.surface == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->cursoryx_wayland.theme);

		return false;
	}

	cursoryx->cursoryx_wayland.pointer = wayland->pointer;

	if (cursoryx->cursoryx_wayland.pointer == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->cursoryx_wayland.theme);

		return false;
	}

	return true;
}

void cursoryx_set(
	struct cursoryx* cursoryx,
	enum cursoryx_cursor cursor)
{
	if (cursor != CURSORYX_NONE)
	{
		cursoryx->cursoryx_wayland.cursor =
			wl_cursor_theme_get_cursor(
				cursoryx->cursoryx_wayland.theme,
				cursoryx_names_wayland[cursor]);
	}
	else
	{
		wl_pointer_set_cursor(
			cursoryx->cursoryx_wayland.pointer,
			0,
			NULL,
			0,
			0);

		return;
	}
	
	if ((cursoryx->cursoryx_wayland.cursor == NULL)
		|| (cursoryx->cursoryx_wayland.cursor->image_count < 1))
	{
		return;
	}

	// get time
	struct timespec time;

	int ok = clock_gettime(CLOCK_REALTIME, &time);

	if (ok == -1)
	{
		return;
	}

	// convert to milliseconds (approx.)
	cursoryx->cursoryx_wayland.time = (time.tv_sec * 1000) + ((time.tv_nsec * 67) >> 26);

	// get image
	unsigned int frame = wl_cursor_frame(cursoryx->cursoryx_wayland.cursor, cursoryx->cursoryx_wayland.time);

	cursoryx->cursoryx_wayland.buffer =
		wl_cursor_image_get_buffer(
			cursoryx->cursoryx_wayland.cursor->images[frame]);

	if (cursoryx->cursoryx_wayland.buffer == NULL)
	{
		return;
	}

	// set cursor
	wl_pointer_set_cursor(
		cursoryx->cursoryx_wayland.pointer,
		0,
		cursoryx->cursoryx_wayland.surface,
		cursoryx->cursoryx_wayland.cursor->images[frame]->hotspot_x,
		cursoryx->cursoryx_wayland.cursor->images[frame]->hotspot_y);

	wl_surface_attach(cursoryx->cursoryx_wayland.surface, cursoryx->cursoryx_wayland.buffer, 0, 0);
	wl_surface_damage(cursoryx->cursoryx_wayland.surface, 0, 0, cursoryx->cursoryx_wayland.size, cursoryx->cursoryx_wayland.size);
	wl_surface_commit(cursoryx->cursoryx_wayland.surface);
}

void cursoryx_stop(
	struct cursoryx* cursoryx)
{
	wl_cursor_theme_destroy(cursoryx->cursoryx_wayland.theme);
	wl_surface_destroy(cursoryx->cursoryx_wayland.surface);
}
