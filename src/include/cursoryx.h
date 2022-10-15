#ifndef H_CURSORYX
#define H_CURSORYX

struct cursoryx;

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

struct cursoryx_config_backend
{
	void* data;

	void (*init)(
		struct cursoryx* context);

	void (*start)(
		struct cursoryx* context,
		void* data);

	void (*set)(
		struct cursoryx* context,
		enum cursoryx_cursor cursor);

	void (*stop)(
		struct cursoryx* context);

	void (*clean)(
		struct cursoryx* context);
};

struct cursoryx* cursoryx_init(
	struct cursoryx_config_backend* config);

void cursoryx_start(
	struct cursoryx* context,
	void* data);

void cursoryx_set(
	struct cursoryx* context,
	enum cursoryx_cursor cursor);

void cursoryx_stop(
	struct cursoryx* context);

void cursoryx_clean(
	struct cursoryx* context);

#endif
