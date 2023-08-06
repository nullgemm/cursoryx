#ifndef H_CURSORYX_WIN
#define H_CURSORYX_WIN

#include "cursoryx.h"

#include <windows.h>

struct cursoryx_win_data
{
	void* data;
};

void cursoryx_prepare_init_win(
	struct cursoryx_config_backend* config);

#endif
