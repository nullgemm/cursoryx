#define _POSIX_C_SOURCE 200809L

#include "cursoryx.h"
#include "cursoryx_windows.h"

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

// Windows cursor names sparse LUT
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

// windows being windows
union alias_hcursor_integer
{
	HCURSOR cursor;
	LONG integer;
};

bool cursoryx_start(
	struct cursoryx* cursoryx,
	void* data)
{
	struct cursoryx_data_windows* win = data;

	cursoryx->cursoryx_windows.handle = win->handle;
	cursoryx->current = CURSORYX_COUNT;

	return true;
}

void cursoryx_set(
	struct cursoryx* cursoryx,
	enum cursoryx_cursor cursor)
{
	if (cursoryx->current == cursor)
	{
		return;
	}

	if (cursor == CURSORYX_NONE)
	{
		ShowCursor(FALSE);
	}
	else
	{
		if (cursoryx->current == CURSORYX_NONE)
		{
			ShowCursor(TRUE);
		}

		union alias_hcursor_integer new;

		new.cursor = 
			LoadCursor(
				NULL,
				cursoryx_names_win[cursor]);

		SetClassLong(
			cursoryx->cursoryx_windows.handle,
			GCLP_HCURSOR,
			new.integer);
	}

	cursoryx->current = cursor;
}

void cursoryx_stop(
	struct cursoryx* cursoryx)
{

}
