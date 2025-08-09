#ifndef H_CURSORYX_INTERNAL_WAYLAND_HELPERS
#define H_CURSORYX_INTERNAL_WAYLAND_HELPERS

#include "cursoryx.h"
#include "common/cursoryx_error.h"
#include "wayland/wayland.h"

#include <stdint.h>
#include <wayland-client.h>

// registry handler
void wayland_helpers_registry_handler(
	void* data,
	void* registry,
	uint32_t name,
	const char* interface,
	uint32_t version);

// capabilities handler
void wayland_helpers_capabilities_handler(
	void* data,
	void* seat,
	uint32_t capabilities);

// cursor animation
void wayland_helpers_animate(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

// frame callback
void wayland_helpers_surface_frame_done(
	void* data,
	struct wl_callback* callback,
	uint32_t time);

// buffer creation
uint32_t* wayland_helpers_buffer_create(
	struct cursoryx* context,
	struct wayland_custom_backend* custom_backend,
	unsigned width,
	unsigned height,
	struct cursoryx_error_info* error);

// buffer destruction
void wayland_helpers_buffer_destroy(
	struct cursoryx* context,
	struct wayland_custom_backend* custom_backend,
	struct cursoryx_error_info* error);

#endif
