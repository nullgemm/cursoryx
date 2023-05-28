#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_appkit.h"
#include "appkit/macos.h"
#include "appkit/macos_helpers.h"

#include <stdlib.h>

#import <Appkit/Appkit.h>
#import <CoreGraphics/CoreGraphics.h> // CGDisplayHideCursor, CGDisplayShowCursor

// Here is the Cursoryx to AppKit public cursors conversion table.
// It is not in use by default, since we currently rely on a hack and use
// Apple's private system cursors to provide a more complete set to the user.
// To disable this hack and use only public cursors, define the contant below.
#ifdef CURSORYX_APPKIT_DISABLE_HACKS
static char* cursoryx_appkit_public[] =
{
	[CURSORYX_ARROW]      = "arrowCursor",
	[CURSORYX_HAND]       = "pointingHandCursor",
	[CURSORYX_CROSSHAIR]  = "crosshairCursor",
	[CURSORYX_TEXT]       = "IBeamCursor",
	[CURSORYX_UNABLE]     = "operationNotAllowedCursor",
	[CURSORYX_BUSY]       = "arrowCursor",
	[CURSORYX_SIZE_ALL]   = "closedHandCursor",
	[CURSORYX_SIZE_NE_SW] = "arrowCursor",
	[CURSORYX_SIZE_NW_SE] = "arrowCursor",
	[CURSORYX_SIZE_N_S]   = "resizeUpDownCursor",
	[CURSORYX_SIZE_W_E]   = "resizeLeftRightCursor",
};
#endif

void cursoryx_appkit_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = malloc(sizeof (struct appkit_backend));

	if (backend == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return;
	}

	struct appkit_backend zero = {0};
	*backend = zero;

	context->backend_data = backend;

#ifndef CURSORYX_APPKIT_DISABLE_HACKS
	appkit_helpers_private_init(context, error);
#endif

	// the backend is responsible for the custom cursor list
	// so we must set the beginning pointer to NULL here
	// and free all the list in cursoryx_appkit_clean
	context->custom_list = NULL;

	cursoryx_error_ok(error);
}

void cursoryx_appkit_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;
	struct cursoryx_appkit_data* window_data = data;

	cursoryx_error_ok(error);
}

void cursoryx_appkit_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;

	switch (cursor)
	{
		case CURSORYX_NONE:
		{
			CGDisplayHideCursor(kCGDirectMainDisplay);
			break;
		}
		default:
		{
			if (context->cursor == CURSORYX_NONE)
			{
				CGDisplayShowCursor(kCGDirectMainDisplay);
			}

#ifdef CURSORYX_APPKIT_DISABLE_HACKS
			SEL selector = sel_registerName(cursoryx_appkit_public[cursor]);
			NSCursor* nscursor = [NSCursor performSelector:selector];
			[nscursor set];
#else
			appkit_helpers_private_set(context, cursor, error);
#endif

			break;
		}
	}

	context->cursor = cursor;
	cursoryx_error_ok(error);
}

void cursoryx_appkit_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;
	struct appkit_custom_backend* custom_backend = custom->backend;

	[custom_backend->cursor set];

	cursoryx_error_ok(error);
}

struct cursoryx_custom* cursoryx_appkit_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;

	// allocate custom context
	struct cursoryx_custom* custom =
		malloc(sizeof (struct cursoryx_custom));

	if (custom == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	// allocate custom backend (appkit) context
	struct appkit_custom_backend* custom_backend =
		malloc(sizeof (struct appkit_custom_backend));

	if (custom_backend == NULL)
	{
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	custom->backend = custom_backend;

	// create colorspace to convert buffer automatically
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

	if (colorspace == Nil)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_MACOS_OBJ_NIL);
		return NULL;
	}

	// create bitmap from buffer
	CGContextRef bitmap =
		CGBitmapContextCreate(
			config->image,
			config->width,
			config->height,
			8,
			config->width * 4,
			colorspace,
			kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);

	if (bitmap == Nil)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_MACOS_OBJ_NIL);
		return NULL;
	}

	// create image from bitmap
	CGImageRef image = CGBitmapContextCreateImage(bitmap);

	if (image == Nil)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_MACOS_OBJ_NIL);
		return NULL;
	}

	// save custom cursor
	NSSize size = NSMakeSize(config->width, config->height);
	NSPoint hotspot = NSMakePoint(config->x, config->y);
	NSImage* cursor = [[NSImage alloc] initWithCGImage: image size: size];
	custom_backend->cursor = [[NSCursor alloc] initWithImage: cursor hotSpot: hotspot];

	// update custom cursor list
	custom->prev = NULL;
	custom->next = context->custom_list;
	context->custom_list = custom;

	// all good
	cursoryx_error_ok(error);
	return custom;
}

void cursoryx_appkit_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;
	struct appkit_custom_backend* custom_backend = custom->backend;

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

	// free allocations
	free(custom_backend);
	free(custom);

	cursoryx_error_ok(error);
}

void cursoryx_appkit_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;

	cursoryx_error_ok(error);
}

void cursoryx_appkit_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;

	// free custom cursor contexts
	struct appkit_custom_backend* custom_backend;
	struct cursoryx_custom* custom = context->custom_list;
	struct cursoryx_custom* next;

	while (custom != NULL)
	{
		custom_backend = custom->backend;
		next = custom->next;

		free(custom_backend);
		free(custom);

		custom = next;
	}

	// free backend context
	free(backend);

	cursoryx_error_ok(error);
}

void cursoryx_prepare_init_appkit(
	struct cursoryx_config_backend* config)
{
	config->data = NULL;
	config->init = cursoryx_appkit_init;
	config->start = cursoryx_appkit_start;
	config->set = cursoryx_appkit_set;
	config->custom_set = cursoryx_appkit_custom_set;
	config->custom_create = cursoryx_appkit_custom_create;
	config->custom_destroy = cursoryx_appkit_custom_destroy;
	config->stop = cursoryx_appkit_stop;
	config->clean = cursoryx_appkit_clean;
}
