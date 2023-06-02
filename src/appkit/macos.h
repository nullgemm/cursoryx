#ifndef H_CURSORYX_INTERNAL_APPKIT
#define H_CURSORYX_INTERNAL_APPKIT

#include "cursoryx.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>

#import <AppKit/AppKit.h>

struct appkit_backend
{
	Class nscursoryx_class;
	long long type;
	id current;
};

struct appkit_custom_backend
{
	NSCursor* cursor;
};

void cursoryx_appkit_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_appkit_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error);

void cursoryx_appkit_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error);

void cursoryx_appkit_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

struct cursoryx_custom* cursoryx_appkit_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error);

void cursoryx_appkit_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

void cursoryx_appkit_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_appkit_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

#endif
