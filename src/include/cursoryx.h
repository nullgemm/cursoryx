#ifndef H_CURSORYX
#define H_CURSORYX

#include <stdint.h>
#include <stddef.h>

struct cursoryx;
struct cursoryx_custom;

enum cursoryx_cursor
{
	CURSORYX_ARROW = 0,
	CURSORYX_HAND,

	CURSORYX_CROSSHAIR,
	CURSORYX_TEXT,

	CURSORYX_UNABLE,
	CURSORYX_BUSY,

	CURSORYX_SIZE_ALL,   /*  plus shape  +  */
	CURSORYX_SIZE_NE_SW, /*  diagonal    \  */
	CURSORYX_SIZE_NW_SE, /*  diagonal    /  */
	CURSORYX_SIZE_N_S,   /*  vertical    |  */
	CURSORYX_SIZE_W_E,   /*  horizontal  -  */

	CURSORYX_NONE,
	CURSORYX_COUNT,
};

enum cursoryx_error
{
	CURSORYX_ERROR_OK = 0,
	CURSORYX_ERROR_NULL,
	CURSORYX_ERROR_ALLOC,
	CURSORYX_ERROR_BOUNDS,
	CURSORYX_ERROR_DOMAIN,
	CURSORYX_ERROR_FD,

	CURSORYX_ERROR_X11_CURSOR_CREATE,
	CURSORYX_ERROR_X11_CURSOR_DESTROY,
	CURSORYX_ERROR_X11_CURSOR_SET,
	CURSORYX_ERROR_X11_PIXMAP_CREATE,
	CURSORYX_ERROR_X11_GC_CREATE,
	CURSORYX_ERROR_X11_IMG_CREATE,
	CURSORYX_ERROR_X11_IMG_PUT,
	CURSORYX_ERROR_X11_FORMATS_GET,
	CURSORYX_ERROR_X11_FORMATS_STANDARD,
	CURSORYX_ERROR_X11_PICTURE_CREATE,
	CURSORYX_ERROR_X11_CUSTOM_CREATE,
	CURSORYX_ERROR_X11_CUSTOM_DESTROY,

	CURSORYX_ERROR_WIN_CURSOR_LOAD,
	CURSORYX_ERROR_WIN_CURSOR_SET,
	CURSORYX_ERROR_WIN_BMP_MASK_CREATE,
	CURSORYX_ERROR_WIN_BMP_COLOR_CREATE,
	CURSORYX_ERROR_WIN_ICON_CREATE,
	CURSORYX_ERROR_WIN_OBJECT_DELETE,

	CURSORYX_ERROR_MACOS_OBJ_NIL,

	CURSORYX_ERROR_COUNT,
};

struct cursoryx_error_info
{
	enum cursoryx_error code;
	const char* file;
	unsigned line;
};

struct cursoryx_custom_config
{
	uint32_t* image;
	size_t width;
	size_t height;
	size_t x;
	size_t y;
};

struct cursoryx_config_backend
{
	void* data;

	void (*init)(
		struct cursoryx* context,
		struct cursoryx_error_info* error);

	void (*start)(
		struct cursoryx* context,
		void* data,
		struct cursoryx_error_info* error);

	void (*set)(
		struct cursoryx* context,
		enum cursoryx_cursor cursor,
		struct cursoryx_error_info* error);

	void (*custom_set)(
		struct cursoryx* context,
		struct cursoryx_custom* custom,
		struct cursoryx_error_info* error);

	struct cursoryx_custom* (*custom_create)(
		struct cursoryx* context,
		struct cursoryx_custom_config* config,
		struct cursoryx_error_info* error);

	void (*custom_destroy)(
		struct cursoryx* context,
		struct cursoryx_custom* custom,
		struct cursoryx_error_info* error);

	void (*stop)(
		struct cursoryx* context,
		struct cursoryx_error_info* error);

	void (*clean)(
		struct cursoryx* context,
		struct cursoryx_error_info* error);
};

struct cursoryx* cursoryx_init(
	struct cursoryx_config_backend* config,
	struct cursoryx_error_info* error);

void cursoryx_start(
	struct cursoryx* context,
	void* data,
	struct cursoryx_error_info* error);

void cursoryx_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor,
	struct cursoryx_error_info* error);

void cursoryx_custom_set(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

struct cursoryx_custom* cursoryx_custom_create(
	struct cursoryx* context,
	struct cursoryx_custom_config* config,
	struct cursoryx_error_info* error);

void cursoryx_custom_destroy(
	struct cursoryx* context,
	struct cursoryx_custom* custom,
	struct cursoryx_error_info* error);

void cursoryx_stop(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_clean(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

void cursoryx_error_log(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

const char* cursoryx_error_get_msg(
	struct cursoryx* context,
	struct cursoryx_error_info* error);

enum cursoryx_error cursoryx_error_get_code(
	struct cursoryx_error_info* error);

const char* cursoryx_error_get_file(
	struct cursoryx_error_info* error);

unsigned cursoryx_error_get_line(
	struct cursoryx_error_info* error);

void cursoryx_error_ok(
	struct cursoryx_error_info* error);

#endif
