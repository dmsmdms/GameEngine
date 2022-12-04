#include <asserts.h>
#include <options.h>
#include <xcb/xcb.h>
#include <stdbool.h>
#include <defines.h>
#include <config.h>
#include <stdlib.h>
#include <logger.h>
#include <window.h>
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#endif

#define RK_ATOM_HELPER(str, exists) \
	atomHelper(str, ARRAY_LEN(str) - 1, exists)

static xcb_connection_t * connection = NULL;
static xcb_atom_t atom_delete_window = 0;
static xcb_window_t window = 0;

static xcb_intern_atom_reply_t * atomHelper(const char * const str, const uint16_t length,
											const uint8_t exists)
{
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, exists, length, str);
	return xcb_intern_atom_reply(connection, cookie, NULL);
}

void rkXcbWindowCreate(void) {
	int screen_id;
	connection = xcb_connect(NULL, &screen_id);
	RK_ASSERT(connection == NULL, NULL);
	RK_XCB_ASSERT(connection, NULL);

	const xcb_setup_t * const setup = xcb_get_setup(connection);
	xcb_screen_iterator_t screen_iterator;

	for (screen_iterator = xcb_setup_roots_iterator(setup); screen_id > 0; --screen_id) {
		xcb_screen_next(&screen_iterator);
	}

	const xcb_screen_t * const restrict screen = screen_iterator.data;
	window = xcb_generate_id(connection);

	const uint32_t value_mask = XCB_CW_EVENT_MASK;
	const uint32_t value_list[] = {
		XCB_EVENT_MASK_NO_EVENT,
	};

	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
		cmdOptions.width, cmdOptions.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual, value_mask, value_list);

	xcb_intern_atom_reply_t * const restrict protocols = RK_ATOM_HELPER("WM_PROTOCOLS", true);
	xcb_intern_atom_reply_t * const restrict delete_window = RK_ATOM_HELPER("WM_DELETE_WINDOW", false);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, protocols->atom, 4,
		sizeof(delete_window->atom) * 8, 1, &delete_window->atom);
	atom_delete_window = delete_window->atom;

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING, sizeof(char) * 8, ARRAY_LEN(CONFIG_PROJECT_NAME), CONFIG_PROJECT_NAME);

	free(delete_window);
	free(protocols);

	const char wm_class[] = CONFIG_PROJECT_NAME "\0" CONFIG_PROJECT_NAME;
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS,
		XCB_ATOM_STRING, sizeof(char) * 8, sizeof(wm_class), wm_class);

	if (cmdOptions.fullscreen != false) {
		xcb_intern_atom_reply_t * const restrict wm_state = RK_ATOM_HELPER("_NET_WM_STATE", false);
		xcb_intern_atom_reply_t * const restrict wm_fullscreen = RK_ATOM_HELPER("_NET_WM_STATE_FULLSCREEN", false);

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, wm_state->atom,
			XCB_ATOM_ATOM, sizeof(wm_fullscreen->atom) * 8, 1, &wm_fullscreen->atom);

		cmdOptions.width = screen->width_in_pixels;
		cmdOptions.height = screen->height_in_pixels;

		free(wm_fullscreen);
		free(wm_state);
	}

	xcb_map_window(connection, window);
	xcb_flush(connection);

	rkLog(RK_LOG_LEVEL_INFO, "Xcb window created! (width: %hu, height: %hu, fullscreen: %hu)",
		  (uint16_t)cmdOptions.width, (uint16_t)cmdOptions.height, (uint16_t)cmdOptions.fullscreen);
}

void rkXcbWindowGetEvents(void) {
	for (xcb_generic_event_t * restrict event; (event = xcb_poll_for_event(connection)); free(event)) {
		switch (event->response_type & ~0x80) {
		case XCB_CLIENT_MESSAGE:
			if (((xcb_client_message_event_t *)(event))->data.data32[0] == atom_delete_window) {
				appIsRunning = false;
			} break;
		}
	}
}

void rkXcbWindowDestroy(void) {
	if (connection != NULL) {
		if (window != 0) {
			xcb_destroy_window(connection, window);
			window = 0;
		}

		xcb_disconnect(connection);
		connection = NULL;

		rkLog(RK_LOG_LEVEL_INFO, "Xcb window destroyed!");
	}
}

#ifdef CONFIG_RENDERER_VULKAN
void vkXcbCreateSurfaceWrap(void) {
	const VkXcbSurfaceCreateInfoKHR surfaceInfo = {
		.sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		.pNext		= NULL,
		.flags		= 0,
		.connection	= connection,
		.window		= window,
	};

	const VkResult result = vkCreateXcbSurfaceKHR(instance, &surfaceInfo,
												  allocationCallbacks, &surface);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Xcb surface created!");
}
#endif
