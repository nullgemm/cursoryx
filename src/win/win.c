#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "include/cursoryx_win.h"
#include "win/win.h"

#include <stdlib.h>
#include <windows.h>

static void* cursoryx_names_win[] =
{
	[CURSORYX_ARROW]      = IDC_ARROW,
	[CURSORYX_HAND]       = IDC_HAND,
	[CURSORYX_CROSSHAIR]  = IDC_CROSS,
	[CURSORYX_TEXT]       = IDC_IBEAM,
	[CURSORYX_UNABLE]     = IDC_NO,
	[CURSORYX_BUSY]       = IDC_WAIT,
	[CURSORYX_SIZE_ALL]   = IDC_SIZEALL,
	[CURSORYX_SIZE_NE_SW] = IDC_SIZENESW,
	[CURSORYX_SIZE_NW_SE] = IDC_SIZENWSE,
	[CURSORYX_SIZE_N_S]   = IDC_SIZENS,
	[CURSORYX_SIZE_W_E]   = IDC_SIZEWE,
};

void cursoryx_win_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = malloc(sizeof (struct win_backend));

	if (backend == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return;
	}

	struct win_backend zero = {0};
	*backend = zero;

	context->backend_data = backend;

	// the backend is responsible for the custom cursor list
	// so we must set the beginning pointer to NULL here
	// and free all the list in cursoryx_win_clean
	context->custom_list = NULL;

	cursoryx_error_ok(error);
}

void cursoryx_win_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;
	struct cursoryx_win_data* window_data = data;

	backend->win = window_data->win;
	backend->device_context = window_data->device_context;

	cursoryx_error_ok(error);
}

void cursoryx_win_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;

	if (context->cursor == cursor)
	{
		cursoryx_error_ok(error);
		return;
	}

	if (cursor == CURSORYX_NONE)
	{
		ShowCursor(FALSE);
	}
	else
	{
		if (context->cursor == CURSORYX_NONE)
		{
			ShowCursor(TRUE);
		}

		union win_cursor win_cursor;

		win_cursor.handle = LoadCursorW(NULL, cursoryx_names_win[cursor]);

		if (win_cursor.handle == NULL)
		{
			cursoryx_error_throw(
				context,
				error,
				CURSORYX_ERROR_WIN_CURSOR_LOAD);

			return;
		}

		DWORD ok = SetClassLongW(backend->win, GCLP_HCURSOR, win_cursor.integer);

		if (ok == 0)
		{
			cursoryx_error_throw(
				context,
				error,
				CURSORYX_ERROR_WIN_CURSOR_SET);

			return;
		}
	}

	context->cursor = cursor;

	cursoryx_error_ok(error);
}

void cursoryx_win_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;
	struct win_custom_backend* custom_backend = custom->backend;

	DWORD ok =
		SetClassLongW(
			backend->win,
			GCLP_HCURSOR,
			custom_backend->cursor.integer);

	if (ok == 0)
	{
		cursoryx_error_throw(
			context,
			error,
			CURSORYX_ERROR_WIN_CURSOR_SET);

		return;
	}

	context->cursor = CURSORYX_COUNT;

	cursoryx_error_ok(error);
}

struct cursoryx_custom* cursoryx_win_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;

	// allocate custom context
	struct cursoryx_custom* custom =
		malloc(sizeof (struct cursoryx_custom));

	if (custom == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	// allocate custom backend (win) context
	struct win_custom_backend* custom_backend =
		malloc(sizeof (struct win_custom_backend));

	if (custom_backend == NULL)
	{
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	custom->backend = custom_backend;

	// create custom cursor
	// create bitmap structure
	BITMAP pixmap =
	{
		.bmType = 0,
		.bmWidth = config->width,
		.bmHeight = config->height,
		.bmWidthBytes = config->width * 4,
		.bmPlanes = 1,
		.bmBitsPixel = 32,
		.bmBits = config->image,
	};

	// allocate mask buffer
	size_t buf_len = (sizeof (uint32_t)) * config->width * config->height;
	uint32_t* buf = malloc(buf_len);

	if (buf == NULL)
	{
		free(custom_backend);
		free(custom);
		cursoryx_error_throw(context, error, CURSORYX_ERROR_ALLOC);
		return NULL;
	}

	// fill mask buffer and create mask
	memset(buf, 0xffffffff, buf_len);
	HBITMAP mask = CreateBitmap(config->width, config->height, 1, 32, buf);

	if (mask == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WIN_BMP_MASK_CREATE);
		free(buf);
		free(custom_backend);
		free(custom);
		return NULL;
	}

	// create cursor bitmap
	HBITMAP color = CreateBitmapIndirect(&pixmap);

	if (color == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WIN_BMP_COLOR_CREATE);
		free(buf);
		free(custom_backend);
		free(custom);
		return NULL;
	}

	// create cursor info structure
	ICONINFO info =
	{
		.fIcon = FALSE,
		.xHotspot = config->x,
		.yHotspot = config->y,
		.hbmMask = mask,
		.hbmColor = color,
	};

	// create cursor
	custom_backend->cursor.handle = CreateIconIndirect(&info);
	free(buf);

	if (custom_backend->cursor.handle == NULL)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WIN_ICON_CREATE);
		free(custom_backend);
		free(custom);
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
	int ok;

	ok = DeleteObject(mask);

	if (ok == 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WIN_OBJECT_DELETE);
		free(custom_backend);
		free(custom);
		return NULL;
	}

	ok = DeleteObject(color);

	if (ok == 0)
	{
		cursoryx_error_throw(context, error, CURSORYX_ERROR_WIN_OBJECT_DELETE);
		free(custom_backend);
		free(custom);
		return NULL;
	}

	// all good
	cursoryx_error_ok(error);
	return custom;
}

void cursoryx_win_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;
	struct win_custom_backend* custom_backend = custom->backend;

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

void cursoryx_win_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;

	cursoryx_error_ok(error);
}

void cursoryx_win_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct win_backend* backend = context->backend_data;

	// free custom cursor contexts
	struct win_custom_backend* custom_backend;
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

void cursoryx_prepare_init_win(
	struct cursoryx_config_backend* config)
{
	config->data = NULL;
	config->init = cursoryx_win_init;
	config->start = cursoryx_win_start;
	config->set = cursoryx_win_set;
	config->custom_set = cursoryx_win_custom_set;
	config->custom_create = cursoryx_win_custom_create;
	config->custom_destroy = cursoryx_win_custom_destroy;
	config->stop = cursoryx_win_stop;
	config->clean = cursoryx_win_clean;
}
