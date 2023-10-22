#ifndef H_CURSORYX_INTERNAL_WAYLAND
#define H_CURSORYX_INTERNAL_WAYLAND

#include "cursoryx.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

struct wayland_backend
{
	// default cursor theme size
	int default_theme_size;
	// last animation update time
	uint32_t time;

	// interfaces
	struct wl_compositor* compositor;
	struct wl_shm* shm;

	// core structures
	struct wl_surface* surface;
	struct wl_pointer* pointer;

	// default cursor data
	struct wl_cursor_theme* theme;
	struct wl_cursor* cursor;
	struct wl_buffer* buffer;

	// listener
	struct wl_callback_listener listener_surface_frame;
};

struct wayland_custom_backend
{
	struct wl_buffer* buffer;
	size_t buffer_len;
	uint32_t* argb;

	int hotspot_x;
	int hotspot_y;
};

void cursoryx_wayland_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_wayland_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error);

void cursoryx_wayland_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error);

void cursoryx_wayland_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

struct cursoryx_custom* cursoryx_wayland_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error);

void cursoryx_wayland_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

void cursoryx_wayland_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_wayland_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

#endif
