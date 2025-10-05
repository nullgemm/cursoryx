#ifndef H_CURSORYX_WAYLAND
#define H_CURSORYX_WAYLAND

#include "cursoryx.h"

#include <stdbool.h>
#include <stdint.h>

struct cursoryx_wayland_data
{
	bool (*add_capabilities_handler)(
		void* data,
		void (*capabilities_handler)(
			void* data,
			void* seat,
			uint32_t capabilities),
		void* capabilities_handler_data);

	void* add_capabilities_handler_data;

	bool (*add_registry_handler)(
		void* data,
		void (*registry_handler)(
			void* data,
			void* registry,
			uint32_t name,
			const char* interface,
			uint32_t version),
		void* registry_handler_data);

	void* add_registry_handler_data;
};

#if !defined(CURSORYX_SHARED)
void cursoryx_prepare_init_wayland(
	struct cursoryx_config_backend* config);
#endif

#endif
