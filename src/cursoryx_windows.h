#ifndef H_CURSORYX_WINDOWS
#define H_CURSORYX_WINDOWS

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

struct cursoryx_data_windows
{
	HWND handle;
};

struct cursoryx_windows
{
	HWND handle;
};

#endif
