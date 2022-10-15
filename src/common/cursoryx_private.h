#ifndef H_CURSORYX_PRIVATE
#define H_CURSORYX_PRIVATE

#include "include/cursoryx.h"

struct cursoryx
{
	enum cursoryx_cursor cursor;
	void* backend_data;
	struct cursoryx_config_backend backend_callbacks;
};

#endif
