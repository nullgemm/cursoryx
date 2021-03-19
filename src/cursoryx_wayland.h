#ifndef H_CURSORYX_WAYLAND
#define H_CURSORYX_WAYLAND

#include "xdg-shell-client-protocol.h"

#include <stdint.h>
#include <wayland-client.h>

struct cursoryx_data_wayland
{
	struct wl_compositor* compositor;
	struct wl_pointer* pointer;
	struct wl_shm* shm;
};

struct cursoryx_wayland
{
	uint32_t time;
	int size;

	struct wl_cursor_theme* theme;
	struct wl_surface* surface;
	struct wl_cursor* cursor;
	struct wl_buffer* buffer;

	struct wl_pointer* pointer;
	struct wl_shm* shm;
};

#endif
