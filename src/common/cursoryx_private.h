#ifndef H_CURSORYX_PRIVATE
#define H_CURSORYX_PRIVATE

#include "include/cursoryx.h"
#include "common/cursoryx_error.h"

struct cursoryx
{
	enum cursoryx_cursor cursor;
	char** error_messages;
	void* backend_data;
	struct cursoryx_config_backend backend_callbacks;
};

#endif
