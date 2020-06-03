#include "cursoryx.h"

#include <stdbool.h>
#include <stdint.h>

#include <objc/runtime.h>
#include <objc/message.h>

// CGDisplayHideCursor, CGDisplayShowCursor
#include <CoreGraphics/CoreGraphics.h>

id (*cursoryx_msg_id)(id, SEL) =
	(id (*)(id, SEL)) objc_msgSend;

// Quartz cursor names sparse LUT
static char* cursoryx_names_quartz[] =
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

bool cursoryx_start(
	struct cursoryx* cursoryx,
	void* data)
{
	cursoryx->current = CURSORYX_COUNT;

	return true;
}

void cursoryx_set(
	struct cursoryx* cursoryx,
	enum cursoryx_cursor cursor)
{
	if (cursor == CURSORYX_NONE)
	{
		if (cursoryx->current == CURSORYX_NONE)
		{
			return;
		}

		CGDisplayHideCursor(kCGDirectMainDisplay);
	}
	else
	{
		if (cursoryx->current == CURSORYX_NONE)
		{
			CGDisplayShowCursor(kCGDirectMainDisplay);
		}

		cursoryx->current_obj = cursoryx_msg_id(
			(id) objc_getClass("NSCursor"),
			sel_getUid(cursoryx_names_quartz[cursor]));

		cursoryx_msg_id(
			cursoryx->current_obj,
			sel_getUid("set"));
	}

	cursoryx->current = cursor;
}

void cursoryx_stop(
	struct cursoryx* cursoryx)
{

}
