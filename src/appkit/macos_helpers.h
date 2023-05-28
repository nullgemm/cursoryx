#ifndef H_CURSORYX_INTERNAL_APPKIT_HELPERS
#define H_CURSORYX_INTERNAL_APPKIT_HELPERS

#include "include/cursoryx.h"
#include "appkit/macos.h"

void appkit_helpers_private_init(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void appkit_helpers_private_set(
	struct cursoryx* context,
	long long cursor,
	struct cursoryx_error_info* error);

#endif
