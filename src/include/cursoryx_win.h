#ifndef H_CURSORYX_WIN
#define H_CURSORYX_WIN

#include "cursoryx.h"

struct cursoryx_win_data
{
	void* data;
};

#if !defined(CURSORYX_SHARED)
void cursoryx_prepare_init_win(
	struct cursoryx_config_backend* config);
#endif

#endif
