#define _POSIX_C_SOURCE 200809L

#include "cursoryx.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <wayland-client.h>
#include <wayland-cursor.h>
#include "xdg-shell-client-protocol.h"

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
	int cursors_size,
	void* data)
{
	struct cursoryx_wayland* wayland = data;

	cursoryx->size = cursors_size;

	cursoryx->theme =
		wl_cursor_theme_load(
			NULL,
			cursors_size,
			wayland->shm);

	if (cursoryx->theme == NULL)
	{
		return false;
	}

	cursoryx->cursor =
		wl_cursor_theme_get_cursor(
			cursoryx->theme,
			"left_ptr");

	if (cursoryx->cursor == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->theme);

		return false;
	}

	cursoryx->surface = wl_compositor_create_surface(wayland->compositor);

	if (cursoryx->surface == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->theme);

		return false;
	}

	cursoryx->pointer = wayland->pointer;

	if (cursoryx->pointer == NULL)
	{
		wl_cursor_theme_destroy(cursoryx->theme);

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
		cursoryx->cursor =
			wl_cursor_theme_get_cursor(
				cursoryx->theme,
				cursoryx_names_wayland[cursor]);
	}
	else
	{
		wl_pointer_set_cursor(
			cursoryx->pointer,
			0,
			NULL,
			0,
			0);

		return;
	}
	
	if ((cursoryx->cursor == NULL)
		|| (cursoryx->cursor->image_count < 1))
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

	// convert to milliseconds
	cursoryx->time = (time.tv_sec * 1000) + ((time.tv_nsec * 67) >> 26);

	// get image
	unsigned int frame = wl_cursor_frame(cursoryx->cursor, cursoryx->time);

	cursoryx->buffer =
		wl_cursor_image_get_buffer(
			cursoryx->cursor->images[frame]);

	if (cursoryx->buffer == NULL)
	{
		return;
	}

	// set cursor
	wl_pointer_set_cursor(
		cursoryx->pointer,
		0,
		cursoryx->surface,
		cursoryx->cursor->images[frame]->hotspot_x,
		cursoryx->cursor->images[frame]->hotspot_y);

	wl_surface_attach(cursoryx->surface, cursoryx->buffer, 0, 0);
	wl_surface_damage(cursoryx->surface, 0, 0, cursoryx->size, cursoryx->size);
	wl_surface_commit(cursoryx->surface);
}

void cursoryx_stop(
	struct cursoryx* cursoryx)
{
	wl_cursor_theme_destroy(cursoryx->theme);
	wl_surface_destroy(cursoryx->surface);
}
