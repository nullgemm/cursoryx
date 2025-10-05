#ifndef H_CURSORYX_APPKIT
#define H_CURSORYX_APPKIT

#include "cursoryx.h"

struct cursoryx_appkit_data
{
	void* data;
};

#if !defined(CURSORYX_SHARED)
void cursoryx_prepare_init_appkit(
	struct cursoryx_config_backend* config);
#endif

#endif
