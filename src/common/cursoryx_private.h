#ifndef H_CURSORYX_PRIVATE
#define H_CURSORYX_PRIVATE

#include <stdint.h>
#include "include/cursoryx.h"
#include "common/cursoryx_error.h"

struct cursoryx_custom
{
	void* backend;
	struct cursoryx_custom* prev;
	struct cursoryx_custom* next;
};

struct cursoryx
{
	enum cursoryx_cursor cursor;
	char* error_messages[CURSORYX_ERROR_COUNT];
	void* backend_data;
	struct cursoryx_config_backend backend_callbacks;
	struct cursoryx_custom* custom_list;
};

#endif
