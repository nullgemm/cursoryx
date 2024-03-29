#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>

#if defined(CURSORYX_ERROR_LOG_MANUAL) || defined(CURSORYX_ERROR_LOG_THROW)
	#include <stdio.h>
#endif

#ifdef CURSORYX_ERROR_ABORT
	#include <stdlib.h>
#endif

void cursoryx_error_init(
	struct cursoryx* context)
{
#ifndef CURSORYX_ERROR_SKIP
	char** log = context->error_messages;

	log[CURSORYX_ERROR_OK] =
		"out-of-bound error message";
	log[CURSORYX_ERROR_NULL] =
		"null pointer";
	log[CURSORYX_ERROR_ALLOC] =
		"failed malloc";
	log[CURSORYX_ERROR_BOUNDS] =
		"out-of-bounds index";
	log[CURSORYX_ERROR_DOMAIN] =
		"invalid domain";
	log[CURSORYX_ERROR_FD] =
		"invalid file descriptor";

	log[CURSORYX_ERROR_POSIX_MMAP] =
		"could not mmap shared Wayland memory";
	log[CURSORYX_ERROR_POSIX_MUNMAP] =
		"could not munmap shared Wayland memory";
	log[CURSORYX_ERROR_POSIX_MEMCPY] =
		"could not memcpy into shared Wayland memory";

	log[CURSORYX_ERROR_X11_CURSOR_CREATE] =
		"could not create X11 cursor";
	log[CURSORYX_ERROR_X11_CURSOR_DESTROY] =
		"could not destroy X11 cursor";
	log[CURSORYX_ERROR_X11_CURSOR_SET] =
		"could not set X11 cursor";
	log[CURSORYX_ERROR_X11_PIXMAP_CREATE] =
		"could not create X11 pixmap";
	log[CURSORYX_ERROR_X11_GC_CREATE] =
		"could not create X11 pixmap graphics context";
	log[CURSORYX_ERROR_X11_IMG_CREATE] =
		"could not create X11 image";
	log[CURSORYX_ERROR_X11_IMG_PUT] =
		"could not prepare X11 image";
	log[CURSORYX_ERROR_X11_FORMATS_GET] =
		"could not get X11 formats";
	log[CURSORYX_ERROR_X11_FORMATS_STANDARD] =
		"could not get standard X11 format";
	log[CURSORYX_ERROR_X11_PICTURE_CREATE] =
		"could not create X11 picture";
	log[CURSORYX_ERROR_X11_CUSTOM_CREATE] =
		"could not create custom X11 cursor";
	log[CURSORYX_ERROR_X11_CUSTOM_DESTROY] =
		"could not destroy custom X11 cursor";

	log[CURSORYX_ERROR_MACOS_OBJ_NIL] =
		"could not get a valid objective-c object";

	log[CURSORYX_ERROR_WIN_CURSOR_LOAD] =
		"could not load win32 cursor";
	log[CURSORYX_ERROR_WIN_BMP_MASK_CREATE] =
		"could not create win32 bitmap mask";
	log[CURSORYX_ERROR_WIN_BMP_COLOR_CREATE] =
		"could not create win32 bitmap color";
	log[CURSORYX_ERROR_WIN_ICON_CREATE] =
		"could not create win32 cursor";
	log[CURSORYX_ERROR_WIN_OBJECT_DELETE] =
		"could not delete win32 object";
	log[CURSORYX_ERROR_WIN_ACTIVE_GET] =
		"couldn't get a win32 active window handle";

	log[CURSORYX_ERROR_WAYLAND_REQUEST] =
		"could not perform Wayland request";
	log[CURSORYX_ERROR_WAYLAND_LISTENER_ADD] =
		"could not add Wayland listener";
	log[CURSORYX_ERROR_WAYLAND_SURFACE_CREATE] =
		"could not create Wayland surface";
	log[CURSORYX_ERROR_WAYLAND_THEME_LOAD] =
		"could not load Wayland cursor theme";
	log[CURSORYX_ERROR_WAYLAND_CURSOR_GET] =
		"could not get Wayland cursor";
	log[CURSORYX_ERROR_WAYLAND_POINTER_UNAVAILABLE] =
		"could not register Wayland mouse pointer";
	log[CURSORYX_ERROR_WAYLAND_THEME_UNAVAILABLE] =
		"could not register Wayland cursor theme";
	log[CURSORYX_ERROR_WAYLAND_SURFACE_UNAVAILABLE] =
		"could not register Wayland surface";
	log[CURSORYX_ERROR_WAYLAND_CURSOR_INVALID] =
		"got an invalid Wayland cursor";
	log[CURSORYX_ERROR_WAYLAND_BUFFER_CREATE] =
		"could not create Wayland buffer";
	log[CURSORYX_ERROR_WAYLAND_SURFACE_FRAME_GET] =
		"could not get Wayland surface frame";
	log[CURSORYX_ERROR_WAYLAND_TIME_GET] =
		"could not get time";
	log[CURSORYX_ERROR_WAYLAND_BUFFER_GET] =
		"could not get Wayland buffer from cursor";
#endif
}

void cursoryx_error_log(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
#ifndef CURSORYX_ERROR_SKIP
	#ifdef CURSORYX_ERROR_LOG_MANUAL
		#ifdef CURSORYX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				error->file,
				error->line);
		#endif

		if (error->code < CURSORYX_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif
#endif
}

const char* cursoryx_error_get_msg(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	if (error->code < CURSORYX_ERROR_COUNT)
	{
		return context->error_messages[error->code];
	}
	else
	{
		return context->error_messages[0];
	}
}

enum cursoryx_error cursoryx_error_get_code(
	struct cursoryx_error_info* error)
{
	return error->code;
}

const char* cursoryx_error_get_file(
	struct cursoryx_error_info* error)
{
	return error->file;
}

unsigned cursoryx_error_get_line(
	struct cursoryx_error_info* error)
{
	return error->line;
}

void cursoryx_error_ok(
	struct cursoryx_error_info* error)
{
	error->code = CURSORYX_ERROR_OK;
	error->file = "";
	error->line = 0;
}

void cursoryx_error_throw_extra(
	struct cursoryx* context,
	struct cursoryx_error_info* error,
	enum cursoryx_error code,
	const char* file,
	unsigned line)
{
#ifndef CURSORYX_ERROR_SKIP
	error->code = code;
	error->file = file;
	error->line = line;

	#ifdef CURSORYX_ERROR_LOG_THROW
		#ifdef CURSORYX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		if (error->code < CURSORYX_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif

	#ifdef CURSORYX_ERROR_ABORT
		abort();
	#endif
#endif
}
