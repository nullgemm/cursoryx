#include "include/cursoryx.h"
#include "common/cursoryx_private.h"

#include <stdlib.h>

struct cursoryx* cursoryx_init(
	struct cursoryx_config_backend* config,
	struct cursoryx_error_info* error)
{
	struct cursoryx* context = malloc(sizeof (struct cursoryx));

	if (context == NULL)
	{
		return NULL;
	}

	struct cursoryx zero = {0};
	*context = zero;

	cursoryx_error_init(context);

	context->cursor = CURSORYX_COUNT;
	context->backend_data = NULL;
	context->backend_callbacks = *config;
	context->backend_callbacks.init(context, error);

	return context;
}

void cursoryx_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.start(context, data, error);
}

void cursoryx_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.set(context, cursor, error);
}

void cursoryx_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.custom_set(context, custom, error);
}

struct cursoryx_custom* cursoryx_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error)
{
	return context->backend_callbacks.custom_create(context, config, error);
}

void cursoryx_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.custom_destroy(context, custom, error);
}

void cursoryx_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.stop(context, error);
}

void cursoryx_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error)
{
	context->backend_callbacks.clean(context, error);
	free(context);
}
