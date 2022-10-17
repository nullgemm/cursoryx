#include "include/cursoryx.h"
#include "common/cursoryx_private.h"
#include "common/cursoryx_error.h"

#include <stdbool.h>

#if defined(CURSORYX_ERROR_LOG_MANUAL) || defined(CURSORYX_ERROR_LOG_THROW)
	#include <stdio.h>
#endif

#ifdef CURSORYX_ERROR_ABORT
	#include <stdlib.h>
#endif

void cursoryx_error_init(
	struct cursoryx* context)
{
#ifndef CURSORYX_ERROR_SKIP
	char** log = context->error_messages;

	log[CURSORYX_ERROR_OK] =
		"out-of-bound error message";
	log[CURSORYX_ERROR_NULL] =
		"null pointer";
	log[CURSORYX_ERROR_ALLOC] =
		"failed malloc";
	log[CURSORYX_ERROR_BOUNDS] =
		"out-of-bounds index";
	log[CURSORYX_ERROR_DOMAIN] =
		"invalid domain";
	log[CURSORYX_ERROR_FD] =
		"invalid file descriptor";

	log[CURSORYX_ERROR_X11_CREATE_CURSOR] =
		"could not create X11 cursor";
#endif
}

void cursoryx_error_log(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
#ifndef CURSORYX_ERROR_SKIP
	#ifdef CURSORYX_ERROR_LOG_MANUAL
		#ifdef CURSORYX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				error->file,
				error->line);
		#endif

		if (error->code < CURSORYX_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif
#endif
}

const char* cursoryx_error_get_msg(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	if (error->code < CURSORYX_ERROR_COUNT)
	{
		return context->error_messages[error->code];
	}
	else
	{
		return context->error_messages[0];
	}
}

enum cursoryx_error cursoryx_error_get_code(
	struct cursoryx_error_info* error)
{
	return error->code;
}

const char* cursoryx_error_get_file(
	struct cursoryx_error_info* error)
{
	return error->file;
}

unsigned cursoryx_error_get_line(
	struct cursoryx_error_info* error)
{
	return error->line;
}

void cursoryx_error_ok(
	struct cursoryx_error_info* error)
{
	error->code = CURSORYX_ERROR_OK;
	error->file = "";
	error->line = 0;
}

void cursoryx_error_throw_extra(
	struct cursoryx* context,
	struct cursoryx_error_info* error,
	enum cursoryx_error code,
	const char* file,
	unsigned line)
{
#ifndef CURSORYX_ERROR_SKIP
	error->code = code;
	error->file = file;
	error->line = line;

	#ifdef CURSORYX_ERROR_LOG_THROW
		#ifdef CURSORYX_ERROR_LOG_DEBUG
			fprintf(
				stderr,
				"error in %s line %u: ",
				file,
				line);
		#endif

		if (error->code < CURSORYX_ERROR_COUNT)
		{
			fprintf(stderr, "%s\n", context->error_messages[error->code]);
		}
		else
		{
			fprintf(stderr, "%s\n", context->error_messages[0]);
		}
	#endif

	#ifdef CURSORYX_ERROR_ABORT
		abort();
	#endif
#endif
}
