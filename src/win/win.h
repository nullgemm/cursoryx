#ifndef H_CURSORYX_INTERNAL_WIN
#define H_CURSORYX_INTERNAL_WIN

#include "cursoryx.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>

struct win_backend
{
	void* data;
};

union win_cursor
{
	HCURSOR handle;
	LONG integer;
};

struct win_custom_backend
{
	union win_cursor cursor;
};

void cursoryx_win_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_win_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error);

void cursoryx_win_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error);

void cursoryx_win_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

struct cursoryx_custom* cursoryx_win_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error);

void cursoryx_win_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

void cursoryx_win_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_win_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

#endif
