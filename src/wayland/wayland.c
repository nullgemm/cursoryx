#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_wayland.h"
#include "wayland/wayland.h"
#include "wayland/wayland_helpers.h"

#include <stdlib.h>
#include <string.h>
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

void cursoryx_wayland_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = malloc(sizeof (struct wayland_backend));

	if (backend == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return;
	}

	struct wayland_backend zero = {0};
	*backend = zero;

	context->backend_data = backend;

	// set default values when possible
	backend->default_theme_size = 32; // TODO autmatically configure this
	backend->time = 0;

	// frame callback listener
	struct wl_callback_listener listener_surface_frame =
	{
		.done = cursoryx_wayland_helpers_surface_frame_done,
	};  

	backend->listener_surface_frame = listener_surface_frame;

	// the backend is responsible for the custom cursor list
	// so we must set the beginning pointer to NULL here
	// and free all the list in cursoryx_wayland_clean
	context->custom_list = NULL;

	cursoryx_error_ok(error);
}

void cursoryx_wayland_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	struct cursoryx_wayland_data* window_data = data;

	window_data->add_registry_handler(
		window_data->add_registry_handler_data,
		cursoryx_wayland_helpers_registry_handler,
		context);

	window_data->add_capabilities_handler(
		window_data->add_capabilities_handler_data,
		cursoryx_wayland_helpers_capabilities_handler,
		context);

	cursoryx_error_ok(error);
}

void cursoryx_wayland_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	context->cursor = cursor;

	// check we have everything we need
	if (backend->pointer == NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_POINTER_UNAVAILABLE);

		return;
	}

	if (backend->theme == NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_THEME_UNAVAILABLE);

		return;
	}

	if (backend->surface == NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_SURFACE_UNAVAILABLE);

		return;
	}

	// hide cursor and exit if requested
	if (cursor == CURSORYX_NONE)
	{
		wl_pointer_set_cursor(
			backend->pointer,
			0,
			NULL,
			0,
			0);

		cursoryx_error_ok(error);

		return;
	}

	// get cursor from theme
	backend->cursor =
		wl_cursor_theme_get_cursor(
			backend->theme,
			cursoryx_names_wayland[cursor]);

	if ((backend->cursor == NULL) || (backend->cursor->image_count < 1))
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_CURSOR_INVALID);

		return;
	}

	// create cursor frame
	struct wl_callback* surface_frame =
		wl_surface_frame(
			backend->surface);

	if (surface_frame == NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_SURFACE_FRAME_GET);

		return;
	}

	// set cursor frame callback
	int error_posix =
		wl_callback_add_listener(
			surface_frame,
			&(backend->listener_surface_frame),
			context);

	if (error_posix == -1)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_LISTENER_ADD);

		return;
	}

	// animate cursor
	cursoryx_wayland_helpers_animate(context, error);

	// error always set
}

void cursoryx_wayland_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	struct wayland_custom_backend* custom_backend = custom->backend;

	wl_pointer_set_cursor(
		backend->pointer,
		0,
		backend->surface,
		custom_backend->hotspot_x,
		custom_backend->hotspot_y);

	wl_surface_attach(backend->surface, custom_backend->buffer, 0, 0);
	wl_surface_damage_buffer(backend->surface, 0, 0, INT32_MAX, INT32_MAX);
	wl_surface_commit(backend->surface);

	context->cursor = CURSORYX_COUNT;

	cursoryx_error_ok(error);
}

struct cursoryx_custom* cursoryx_wayland_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;

	// allocate custom context
	struct cursoryx_custom* custom =
		malloc(sizeof (struct cursoryx_custom));

	if (custom == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	// allocate custom backend (Wayland) context
	struct wayland_custom_backend* custom_backend =
		malloc(sizeof (struct wayland_custom_backend));

	if (custom_backend == NULL)
	{
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	custom->backend = custom_backend;

	// create buffer
	custom_backend->argb =
		cursoryx_wayland_helpers_buffer_create(
			context,
			custom_backend,
			config->width,
			config->height,
			error);

	if (cursoryx_error_get_code(error) != CURSORYX_ERROR_OK)
	{
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WAYLAND_BUFFER_CREATE);
		return NULL;
	}

	// copy image into buffer
	void* error_mem =
		memcpy(
			custom_backend->argb,
			config->image,
			config->width * config->height * 4);

	if (error_mem == NULL)
	{
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_POSIX_MEMCPY);
		return NULL;
	}

	// copy hotspot coordinates
	custom_backend->hotspot_x = config->x;
	custom_backend->hotspot_y = config->y;

	// add the newly created cursor to the linked list
	if (context->custom_list != NULL)
	{
		context->custom_list->prev = custom;
	}

	custom->prev = NULL;
	custom->next = context->custom_list;
	context->custom_list = custom;

	// all good
	cursoryx_error_ok(error);
	return custom;
}

void cursoryx_wayland_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	struct wayland_custom_backend* custom_backend = custom->backend;

	// re-wire the linked list
	if (custom->prev != NULL)
	{
		custom->prev->next = custom->next;
	}
	else
	{
		context->custom_list = custom->next;
	}

	if (custom->next != NULL)
	{
		custom->next->prev = custom->prev;
	}

	// free Wayland resources
	cursoryx_wayland_helpers_buffer_destroy(context, custom_backend, error);
	wl_buffer_destroy(custom_backend->buffer);

	// free allocations
	free(custom_backend);
	free(custom);

	cursoryx_error_ok(error);
}

void cursoryx_wayland_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;

	if (backend->theme != NULL)
	{
		wl_cursor_theme_destroy(backend->theme);
	}

	if (backend->shm != NULL)
	{
		wl_shm_destroy(backend->shm);
	}

	if (backend->surface != NULL)
	{
		wl_surface_destroy(backend->surface);
	}

	if (backend->compositor != NULL)
	{
		wl_compositor_destroy(backend->compositor);
	}

	cursoryx_error_ok(error);
}

void cursoryx_wayland_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;

	// free custom cursor contexts
	struct wayland_custom_backend* custom_backend;
	struct cursoryx_custom* custom = context->custom_list;
	struct cursoryx_custom* next;

	while (custom != NULL)
	{
		custom_backend = custom->backend;
		next = custom->next;

		cursoryx_wayland_helpers_buffer_destroy(context, custom_backend, error);
		wl_buffer_destroy(custom_backend->buffer);
		free(custom_backend);
		free(custom);

		custom = next;
	}

	// free backend context
	free(backend);

	cursoryx_error_ok(error);
}

void cursoryx_prepare_init_wayland(
	struct cursoryx_config_backend* config)
{
	config->data = NULL;
	config->init = cursoryx_wayland_init;
	config->start = cursoryx_wayland_start;
	config->set = cursoryx_wayland_set;
	config->custom_set = cursoryx_wayland_custom_set;
	config->custom_create = cursoryx_wayland_custom_create;
	config->custom_destroy = cursoryx_wayland_custom_destroy;
	config->stop = cursoryx_wayland_stop;
	config->clean = cursoryx_wayland_clean;
}
