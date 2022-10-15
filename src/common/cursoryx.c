#include "include/cursoryx.h"
#include "common/cursoryx_private.h"

#include <stdlib.h>

struct cursoryx* cursoryx_init(
	struct cursoryx_config_backend* config)
{
	struct cursoryx* context = malloc(sizeof (struct cursoryx));

	if (context == NULL)
	{
		return NULL;
	}

	struct cursoryx zero = {0};
	*context = zero;

	context->cursor = CURSORYX_COUNT;
	context->backend_data = NULL;
	context->backend_callbacks = *config;
	context->backend_callbacks.init(context);

	return context;
}

void cursoryx_start(
	struct cursoryx* context,
	void* data)
{
	context->backend_callbacks.start(context, data);
}

void cursoryx_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor)
{
	context->backend_callbacks.set(context, cursor);
}

void cursoryx_stop(
	struct cursoryx* context)
{
	context->backend_callbacks.stop(context);
}

void cursoryx_clean(
	struct cursoryx* context)
{
	context->backend_callbacks.clean(context);
	free(context);
}
