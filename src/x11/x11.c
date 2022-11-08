#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_x11.h"
#include "x11/x11.h"

#include <stdlib.h>
#include <xcb/render.h>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_renderutil.h>
#include <xcb/xfixes.h>

static char* cursoryx_names_x11[] =
{
	[CURSORYX_ARROW]      = "default",
	[CURSORYX_HAND]       = "pointer",
	[CURSORYX_CROSSHAIR]  = "crosshair",
	[CURSORYX_TEXT]       = "text",
	[CURSORYX_UNABLE]     = "not-allowed",
	[CURSORYX_BUSY]       = "wait",
	[CURSORYX_SIZE_ALL]   = "all-scroll",
	[CURSORYX_SIZE_NE_SW] = "nesw-resize",
	[CURSORYX_SIZE_NW_SE] = "nwse-resize",
	[CURSORYX_SIZE_N_S]   = "row-resize",
	[CURSORYX_SIZE_W_E]   = "col-resize",
};

void cursoryx_x11_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = malloc(sizeof (struct x11_backend));

	if (backend == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return;
	}

	struct x11_backend zero = {0};
	*backend = zero;

	context->backend_data = backend;

	// the backend is responsible for the custom cursor list
	// so we must set the beginning pointer to NULL here
	// and free all the list in cursoryx_x11_clean
	context->custom_list = NULL;

	cursoryx_error_ok(error);
}

void cursoryx_x11_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct cursoryx_x11_data* window_data = data;

	backend->cursor = NULL;
	backend->conn = window_data->conn;
	backend->window = window_data->window;
	backend->screen = window_data->screen;

	// TODO report available features
	xcb_generic_error_t* error_xcb = NULL;
	const xcb_query_extension_reply_t* ext_reply = NULL;

	// test if the render extension v1.4 is available
	ext_reply =
		xcb_get_extension_data(
			backend->conn,
			&xcb_render_id);

	if (ext_reply->present != 0)
	{
		xcb_render_query_version_cookie_t cookie =
			xcb_render_query_version(
				backend->conn,
				1,
				4);

		xcb_render_query_version_reply_t* reply =
			xcb_render_query_version_reply(
				backend->conn,
				cookie,
				&error_xcb);

		backend->xrender = (error_xcb == NULL);

		free(reply);
	}
	else
	{
		backend->xrender = false;
	}

	// test if the xfixes extension v4.0 is available
	ext_reply =
		xcb_get_extension_data(
			backend->conn,
			&xcb_xfixes_id);

	if (ext_reply->present != 0)
	{
		xcb_xfixes_query_version_cookie_t cookie =
			xcb_xfixes_query_version(
				backend->conn,
				4,
				0);

		xcb_xfixes_query_version_reply_t* reply =
			xcb_xfixes_query_version_reply(
				backend->conn,
				cookie,
				&error_xcb);

		backend->xfixes = (error_xcb == NULL);

		free(reply);
	}
	else
	{
		backend->xfixes = false;
	}

	// create cursor context
	int error_posix =
		xcb_cursor_context_new(
			backend->conn,
			backend->screen,
			&(backend->cursor));

	if (error_posix < 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_CURSOR_CREATE);
		return;
	}

	cursoryx_error_ok(error);
}

void cursoryx_x11_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	if (context->cursor == cursor)
	{
		return;
	}

	if (cursor == CURSORYX_NONE)
	{
		if (backend->xfixes == true)
		{
			xcb_xfixes_hide_cursor(
				backend->conn,
				backend->window);
		}
	}
	else
	{
		if (context->cursor == CURSORYX_NONE)
		{
			if (backend->xfixes == true)
			{
				xcb_xfixes_show_cursor(
					backend->conn,
					backend->window);
			}
		}

		xcb_cursor_t id =
			xcb_cursor_load_cursor(
				backend->cursor,
				cursoryx_names_x11[cursor]);

		xcb_void_cookie_t cookie =
			xcb_change_window_attributes(
				backend->conn,
				backend->window,
				XCB_CW_CURSOR,
				&id);

		xcb_generic_error_t* error_xcb =
			xcb_request_check(
				backend->conn,
				cookie);

		xcb_free_cursor(
			backend->conn,
			id);

		if (error_xcb != NULL)
		{
			cursoryx_error_throw(
				context,
				error,
				CURSORYX_ERROR_X11_CURSOR_SET);

			return;
		}
	}

	context->cursor = cursor;

	cursoryx_error_ok(error);
}

void cursoryx_x11_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_custom_backend* custom_backend = custom->backend;
	xcb_generic_error_t* error_xcb = NULL;

	xcb_void_cookie_t cookie =
		xcb_change_window_attributes(
			backend->conn,
			backend->window,
			XCB_CW_CURSOR,
			&(custom_backend->cursor));

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_X11_CURSOR_SET);
	}

	cursoryx_error_ok(error);
}

struct cursoryx_custom* cursoryx_x11_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	xcb_generic_error_t* error_xcb = NULL;

	// allocate custom context
	struct cursoryx_custom* custom =
		malloc(sizeof (struct cursoryx_custom));

	if (custom == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	// allocate custom backend (x11) context
	struct x11_custom_backend* custom_backend =
		malloc(sizeof (struct x11_custom_backend));

	if (custom_backend == NULL)
	{
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	custom->backend = custom_backend;

	// generate X11 ids
	xcb_gc_t gc = xcb_generate_id(backend->conn);
	xcb_pixmap_t pixmap = xcb_generate_id(backend->conn);
	custom_backend->cursor = xcb_generate_id(backend->conn);
	xcb_render_picture_t picture = xcb_generate_id(backend->conn);

	// create pixmap
	xcb_void_cookie_t cookie =
		xcb_create_pixmap(
			backend->conn,
			32,
			pixmap,
			backend->screen->root,
			config->width,
			config->height);

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_PIXMAP_CREATE);
		return NULL;
	}

	// create graphics context
	cookie =
		xcb_create_gc(
			backend->conn,
			gc,
			pixmap,
			0,
			NULL);

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		xcb_free_pixmap(backend->conn, pixmap);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_GC_CREATE);
		return NULL;
	}

	// create image
	xcb_image_t* img =
		xcb_image_create_native(
			backend->conn,
			config->width,
			config->height,
			XCB_IMAGE_FORMAT_Z_PIXMAP,
			32,
			NULL,
			4 * config->width * config->height,
			(uint8_t*) config->image);

	if (img == NULL)
	{
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_IMG_CREATE);
		return NULL;
	}

	// prepare image
	cookie =
		xcb_image_put(
			backend->conn,
			pixmap,
			gc,
			img,
			0,
			0,
			0);

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		xcb_image_destroy(img);
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_IMG_PUT);
		return NULL;
	}

	// get picture formats
	xcb_render_query_pict_formats_cookie_t cookie_formats =
		xcb_render_query_pict_formats(
			backend->conn);

	xcb_render_query_pict_formats_reply_t* reply_formats =
		xcb_render_query_pict_formats_reply(
			backend->conn,
			cookie_formats,
			&error_xcb);

	if (error_xcb != NULL)
	{
		xcb_image_destroy(img);
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_FORMATS_GET);
		return NULL;
	}

	// find the ARGB32 format in the list
	xcb_render_pictforminfo_t* format =
		xcb_render_util_find_standard_format(
			reply_formats,
			XCB_PICT_STANDARD_ARGB_32);

	if (format == NULL)
	{
		xcb_image_destroy(img);
		free(reply_formats);
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_FORMATS_STANDARD);
		return NULL;
	}

	// create a picture from the pixmap...
	cookie =
		xcb_render_create_picture(
			backend->conn,
			picture,
			pixmap,
			format->id,
			0,
			NULL);

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		xcb_image_destroy(img);
		free(reply_formats);
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_PICTURE_CREATE);
		return NULL;
	}

	// create the cursor
	cookie =
		xcb_render_create_cursor(
			backend->conn,
			custom_backend->cursor,
			picture,
			config->x,
			config->y);

	error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		xcb_render_free_picture(backend->conn, picture);
		xcb_image_destroy(img);
		free(reply_formats);
		xcb_free_pixmap(backend->conn, pixmap);
		xcb_free_gc(backend->conn, gc);
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_X11_CUSTOM_CREATE);
		return NULL;
	}

	// add the newly created cursor to the linked list
	if (context->custom_list != NULL)
	{
		context->custom_list->prev = custom;
	}

	custom->prev = NULL;
	custom->next = context->custom_list;
	context->custom_list = custom;

	// free temporary resources
	xcb_render_free_picture(backend->conn, picture);
	xcb_image_destroy(img);
	free(reply_formats);
	xcb_free_pixmap(backend->conn, pixmap);
	xcb_free_gc(backend->conn, gc);

	// all good
	cursoryx_error_ok(error);
	return custom;
}

void cursoryx_x11_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;
	struct x11_custom_backend* custom_backend = custom->backend;

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

	// free x11 resources
	xcb_void_cookie_t cookie =
		xcb_free_cursor(
			backend->conn,
			custom_backend->cursor);

	xcb_generic_error_t* error_xcb =
		xcb_request_check(
			backend->conn,
			cookie);

	if (error_xcb != NULL)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_X11_CURSOR_DESTROY);

		return;
	}

	// free allocations
	free(custom_backend);
	free(custom);

	cursoryx_error_ok(error);
}

void cursoryx_x11_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	xcb_cursor_context_free(backend->cursor);
	cursoryx_error_ok(error);
}

void cursoryx_x11_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct x11_backend* backend = context->backend_data;

	// free custom cursor contexts
	struct x11_custom_backend* custom_backend;
	struct cursoryx_custom* custom = context->custom_list;
	struct cursoryx_custom* next;

	while (custom != NULL)
	{
		custom_backend = custom->backend;
		next = custom->next;

		xcb_free_cursor(backend->conn, custom_backend->cursor);
		free(custom_backend);
		free(custom);

		custom = next;
	}

	// free backend context
	free(backend);

	cursoryx_error_ok(error);
}

void cursoryx_prepare_init_x11(
	struct cursoryx_config_backend* config)
{
	config->data = NULL;
	config->init = cursoryx_x11_init;
	config->start = cursoryx_x11_start;
	config->set = cursoryx_x11_set;
	config->custom_set = cursoryx_x11_custom_set;
	config->custom_create = cursoryx_x11_custom_create;
	config->custom_destroy = cursoryx_x11_custom_destroy;
	config->stop = cursoryx_x11_stop;
	config->clean = cursoryx_x11_clean;
}
