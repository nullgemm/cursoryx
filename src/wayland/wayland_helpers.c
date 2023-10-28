#define _XOPEN_SOURCE 700
#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_wayland.h"
#include "wayland/wayland.h"
#include "wayland/wayland_helpers.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-cursor.h>

// registry handler
void wayland_helpers_registry_handler(
	void* data,
	struct wl_registry* registry,
	uint32_t name,
	const char* interface,
	uint32_t version)
{
	struct cursoryx* context = data;
	struct wayland_backend* backend = context->backend_data;
	struct cursoryx_error_info error;

	if (strcmp(interface, wl_compositor_interface.name) == 0)
	{
		backend->compositor =
			wl_registry_bind(
				registry,
				name,
				&wl_compositor_interface,
				4);

		if (backend->compositor == NULL)
		{
			cursoryx_error_throw(
				context,
				&error,
				CURSORYX_ERROR_WAYLAND_REQUEST);
			return;
		}

		backend->surface =
			wl_compositor_create_surface(
				backend->compositor);

		if (backend->surface == NULL)
		{
			cursoryx_error_throw(
				context,
				&error,
				CURSORYX_ERROR_WAYLAND_SURFACE_CREATE);

			return;
		}
	}
	else if (strcmp(interface, wl_shm_interface.name) == 0)
	{
		backend->shm =
			wl_registry_bind(
				registry,
				name,
				&wl_shm_interface,
				1);

		if (backend->shm == NULL)
		{
			cursoryx_error_throw(
				context,
				&error,
				CURSORYX_ERROR_WAYLAND_REQUEST);

			return;
		}

		backend->theme =
			wl_cursor_theme_load(
				NULL,
				backend->default_theme_size,
				backend->shm);

		if (backend->theme == NULL)
		{
			cursoryx_error_throw(
				context,
				&error,
				CURSORYX_ERROR_WAYLAND_THEME_LOAD);

			return;
		}

		backend->cursor =
			wl_cursor_theme_get_cursor(
				backend->theme,
				"left_ptr");

		if (backend->cursor == NULL)
		{
			cursoryx_error_throw(
				context,
				&error,
				CURSORYX_ERROR_WAYLAND_CURSOR_GET);

			return;
		}
	}
}

// capabilities handler
void wayland_helpers_capabilities_handler(
	void* data,
	struct wl_seat* seat,
	uint32_t capabilities)
{
	struct cursoryx* context = data;
	struct wayland_backend* backend = context->backend_data;

	bool pointer = (capabilities & WL_SEAT_CAPABILITY_POINTER) != 0;

	if ((pointer == true) && (backend->pointer == NULL))
	{
		backend->pointer = wl_seat_get_pointer(seat);
	}
	else if ((pointer == false) && (backend->pointer != NULL))
	{
		wl_pointer_release(backend->pointer);
		backend->pointer = NULL;
	}
}

// cursor animation
void wayland_helpers_animate(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	// if the current cursor is a custom user-set one we don't have to animate
	if (context->cursor == CURSORYX_COUNT)
	{
		cursoryx_error_ok(error);
		return;
	}

	// if the current cursor is a system one, we have to support animations
	struct wayland_backend* backend = context->backend_data;

	// get time
	struct timespec time;
	int ok = clock_gettime(CLOCK_REALTIME, &time);

	if (ok == -1)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_TIME_GET);

		return;
	}

	// convert to milliseconds
	backend->time = (time.tv_sec * 1000) + (time.tv_nsec / 1000000);

	// get image
	unsigned int frame = wl_cursor_frame(backend->cursor, backend->time);

	backend->buffer =
		wl_cursor_image_get_buffer(
			backend->cursor->images[frame]);

	if (backend->buffer == NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WAYLAND_BUFFER_GET);

		return;
	}

	// set cursor
	wl_pointer_set_cursor(
		backend->pointer,
		0,
		backend->surface,
		backend->cursor->images[frame]->hotspot_x,
		backend->cursor->images[frame]->hotspot_y);

	wl_surface_attach(backend->surface, backend->buffer, 0, 0);
	wl_surface_damage_buffer(backend->surface, 0, 0, INT32_MAX, INT32_MAX);
	wl_surface_commit(backend->surface);

	cursoryx_error_ok(error);
}

// frame callback
void wayland_helpers_surface_frame_done(
	void* data,
	struct wl_callback* callback,
	uint32_t time)
{
	struct cursoryx* context = data;
	struct wayland_backend* backend = context->backend_data;
	struct cursoryx_error_info error;

	if (callback != NULL)
	{
		// destroy the current frame callback
		// this is where the surface_frame from last time is destroyed
		wl_callback_destroy(callback);
	}

	// register a new frame callback
	// valgrind false positive, surface_frame is destroyed in the next callback
	struct wl_callback* surface_frame = wl_surface_frame(backend->surface);

	if (surface_frame == NULL)
	{
		cursoryx_error_throw(
			context,
			&error,
			CURSORYX_ERROR_WAYLAND_SURFACE_FRAME_GET);

		return;
	}

	// set surface frame callback
	int error_posix =
		wl_callback_add_listener(
			surface_frame,
			&(backend->listener_surface_frame),
			context);

	if (error_posix == -1)
	{
		cursoryx_error_throw(
			context,
			&error,
			CURSORYX_ERROR_WAYLAND_LISTENER_ADD);

		return;
	}

	// render cursor
	wayland_helpers_animate(context, &error);
}

// buffer creation
uint32_t* wayland_helpers_buffer_create(
	struct cursoryx* context,
	struct wayland_custom_backend* custom_backend,
	unsigned width,
	unsigned height,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	custom_backend->buffer_len = 4 * width * height;

	// create shm - code by sircmpwn
	int retries = 100;
	int fd;

	struct timespec time;
	uint64_t random;
	int i;

	do
	{
		char name[] = "/wl_shm-XXXXXX";

		clock_gettime(CLOCK_REALTIME, &time);
		random = time.tv_nsec;

		for (i = 0; i < 6; ++i)
		{
			name[(sizeof (name)) - 7 + i] =
				'A'
				+ (random & 15)
				+ ((random & 16) * 2);

			random >>= 5;
		}

		fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);

		--retries;

		if (fd >= 0)
		{
			shm_unlink(name);

			break;
		}
	}
	while ((retries > 0) && (errno == EEXIST));

	if (fd < 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_FD);
		return NULL;
	}

	// allocate shm
	int error_posix;

	do
	{
		error_posix = ftruncate(fd, custom_backend->buffer_len);
	}
	while ((error_posix < 0) && (errno == EINTR));

	if (error_posix < 0)
	{
		close(fd);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_FD);
		return NULL;
	}

	// mmap
	uint32_t* argb =
		mmap(
			NULL,
			custom_backend->buffer_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0);

	if (argb == MAP_FAILED)
	{
		close(fd);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_POSIX_MMAP);
		return NULL;
	}

	// create memory pool
	struct wl_shm_pool* software_pool =
		wl_shm_create_pool(
			backend->shm,
			fd,
			custom_backend->buffer_len);

	if (software_pool == NULL)
	{
		munmap(argb, custom_backend->buffer_len);
		close(fd);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// create buffer
	custom_backend->buffer =
		wl_shm_pool_create_buffer(
			software_pool,
			0,
			width,
			height,
			width * 4,
			WL_SHM_FORMAT_ARGB8888);

	if (custom_backend->buffer == NULL)
	{
		wl_shm_pool_destroy(software_pool);
		munmap(argb, custom_backend->buffer_len);
		close(fd);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WAYLAND_REQUEST);
		return NULL;
	}

	// clean resources
	wl_shm_pool_destroy(software_pool);
	close(fd);

	// all good
	cursoryx_error_ok(error);
	return argb;
}

// buffer destruction
void wayland_helpers_buffer_destroy(
	struct cursoryx* context,
	struct wayland_custom_backend* custom_backend,
	struct cursoryx_error_info* error)
{
	struct wayland_backend* backend = context->backend_data;
	int error_posix = munmap(custom_backend->argb, custom_backend->buffer_len);

	if (error_posix < 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_POSIX_MUNMAP);
		return;
	}

	cursoryx_error_ok(error);
}
