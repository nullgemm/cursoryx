#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "appkit/macos.h"
#include "appkit/macos_helpers.h"

#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <objc/message.h>

// Here are all the cursors actually supported by Apple, as seen in WebKit
// The spinning pinwheel is not available here either of course since Apple
// really does not want us to use it directly in applications.
// https://github.com/WebKit/webkit/blob/main/Source/WebCore/platform/mac/CursorMac.mm
// https://github.com/WebKit/WebKit/blob/main/Source/WebCore/PAL/pal/spi/mac/HIServicesSPI.h
enum appkit_private_cursors
{
	kCoreCursorFirstCursor = 0,
	kCoreCursorArrow = kCoreCursorFirstCursor,
	kCoreCursorIBeam,
	kCoreCursorMakeAlias,
	kCoreCursorNotAllowed,
	kCoreCursorBusyButClickable,
	kCoreCursorCopy,
	kCoreCursorScreenShotSelection = 7,
	kCoreCursorScreenShotSelectionToClip,
	kCoreCursorScreenShotWindow,
	kCoreCursorScreenShotWindowToClip,
	kCoreCursorClosedHand,
	kCoreCursorOpenHand,
	kCoreCursorPointingHand,
	kCoreCursorCountingUpHand,
	kCoreCursorCountingDownHand,
	kCoreCursorCountingUpAndDownHand,
	kCoreCursorResizeLeft,
	kCoreCursorResizeRight,
	kCoreCursorResizeLeftRight,
	kCoreCursorCross,
	kCoreCursorResizeUp,
	kCoreCursorResizeDown,
	kCoreCursorResizeUpDown,
	kCoreCursorContextualMenu,
	kCoreCursorPoof,
	kCoreCursorIBeamVertical,
	kCoreCursorWindowResizeEast,
	kCoreCursorWindowResizeEastWest,
	kCoreCursorWindowResizeNorthEast,
	kCoreCursorWindowResizeNorthEastSouthWest,
	kCoreCursorWindowResizeNorth,
	kCoreCursorWindowResizeNorthSouth,
	kCoreCursorWindowResizeNorthWest,
	kCoreCursorWindowResizeNorthWestSouthEast,
	kCoreCursorWindowResizeSouthEast,
	kCoreCursorWindowResizeSouth,
	kCoreCursorWindowResizeSouthWest,
	kCoreCursorWindowResizeWest,
	kCoreCursorWindowMove,
	kCoreCursorHelp,
	kCoreCursorCell,
	kCoreCursorZoomIn,
	kCoreCursorZoomOut,
	kCoreCursorLastCursor = kCoreCursorZoomOut,
};

// here is the Cursoryx to AppKit private cursors conversion table
static long long cursoryx_appkit_private[] =
{
	[CURSORYX_ARROW]      = kCoreCursorArrow,
	[CURSORYX_HAND]       = kCoreCursorPointingHand,
	[CURSORYX_CROSSHAIR]  = kCoreCursorCross,
	[CURSORYX_TEXT]       = kCoreCursorIBeam,
	[CURSORYX_UNABLE]     = kCoreCursorNotAllowed,
	[CURSORYX_BUSY]       = kCoreCursorBusyButClickable,
	[CURSORYX_SIZE_ALL]   = kCoreCursorWindowMove,
	[CURSORYX_SIZE_NE_SW] = kCoreCursorWindowResizeNorthEastSouthWest,
	[CURSORYX_SIZE_NW_SE] = kCoreCursorWindowResizeNorthWestSouthEast,
	[CURSORYX_SIZE_N_S]   = kCoreCursorWindowResizeNorthSouth,
	[CURSORYX_SIZE_W_E]   = kCoreCursorWindowResizeEastWest,
};

static NSInteger coreCursorType(id self, SEL _cmd)
{
	long long* cursorType = 0;

	object_getInstanceVariable(
		self,
		"cursorType",
		(void**) &cursorType);

	return *cursorType;
}

void appkit_helpers_private_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;

	// allocate private cursor hack class
	Class nscursoryx_class =
		objc_allocateClassPair(
			[NSCursor class],
			"NSCursoryx",
			0);

	SEL coreCursorTypeSel =
		NSSelectorFromString(
			@"_coreCursorType");

	class_addMethod(
		nscursoryx_class,
		coreCursorTypeSel,
		(IMP) coreCursorType,
		method_getTypeEncoding(
			class_getInstanceMethod(
				[NSCursor class],
				coreCursorTypeSel)));

	class_addIvar(
		nscursoryx_class,
		"cursorType",
		sizeof (long long),
		sizeof (void*),
		"q");

	objc_registerClassPair(nscursoryx_class);
	backend->nscursoryx_class = nscursoryx_class;
	cursoryx_error_ok(error);
}

void appkit_helpers_private_set(
	struct cursoryx* context,
	long long cursor,
	struct cursoryx_error_info* error)
{
	struct appkit_backend* backend = context->backend_data;
	id nscursoryx_obj = [backend->nscursoryx_class new];

	backend->type = cursoryx_appkit_private[cursor];
	object_setInstanceVariable(nscursoryx_obj, "cursorType", &(backend->type));

	SEL setSel = NSSelectorFromString(@"set");
	((void(*)(id, SEL)) objc_msgSend)(nscursoryx_obj, setSel);

	backend->current = nscursoryx_obj;
	cursoryx_error_ok(error);
}
