#ifndef H_CURSORYX_ERROR
#define H_CURSORYX_ERROR

#include "include/cursoryx.h"

#define cursoryx_error_throw(context, error, code) \
	cursoryx_error_throw_extra(\
		context,\
		error,\
		code,\
		CURSORYX_ERROR_FILE,\
		CURSORYX_ERROR_LINE)
#define CURSORYX_ERROR_FILE __FILE__
#define CURSORYX_ERROR_LINE __LINE__

void cursoryx_error_throw_extra(
	struct cursoryx* context,
	struct cursoryx_error_info* error,
	enum cursoryx_error code,
	const char* file,
	unsigned line);

void cursoryx_error_init(
	struct cursoryx* context);

#endif
