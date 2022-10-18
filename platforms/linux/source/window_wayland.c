#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>
#include <stdbool.h>
#include <asserts.h>
#include <options.h>
#include <defines.h>
#include <config.h>
#include <logger.h>
#include <window.h>
#include <string.h>
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#endif

static struct wl_display * display = NULL;
static struct wl_registry * registry = NULL;
static struct wl_compositor * compositor = NULL;
static struct xdg_wm_base * shell = NULL;
static struct wl_surface * wl_surface = NULL;
static struct xdg_surface * xdg_surface = NULL;
static struct xdg_toplevel * xdg_toplevel = NULL;

static void rkWaylandGlobalRegistryHandler(UNUSED void * const data,
										   struct wl_registry * const wl_registry,
										   const uint32_t name,
										   const char * const interface,
										   const uint32_t version)
{
	if (strcmp(interface, "wl_compositor") == RK_RESULT_SUCCESS) {
		compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
	} else if (strcmp(interface, "xdg_wm_base") == RK_RESULT_SUCCESS) {
		shell = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, version);
	}
}

static void rkWaylandXdgWmBasePingHandler(UNUSED void * const data,
										  struct xdg_wm_base * const shell,
										  const uint32_t serial)
{
	xdg_wm_base_pong(shell, serial);
}

static void rkWaylandXdgSurfaceConfigure(UNUSED void * const data,
										 struct xdg_surface * const xdg_surface,
										 const uint32_t serial)
{
	xdg_surface_ack_configure(xdg_surface, serial);
}

static void rkWaylandXdgToplevelConfigure(UNUSED void * const data,
										  UNUSED struct xdg_toplevel * const xdg_toplevel,
										  UNUSED const int32_t width,
										  UNUSED const int32_t height,
										  UNUSED struct wl_array * const states)
{
	// Do nothing //
}

static void rkWaylandXdgToplevelClose(UNUSED void * const data,
									  UNUSED struct xdg_toplevel * const xdg_toplevel)
{
	appIsRunning = false;
}

static const struct wl_registry_listener registryListener = {
	.global			= rkWaylandGlobalRegistryHandler,
	.global_remove	= NULL,
};

static const struct xdg_wm_base_listener xdgWmBaseListener = {
	.ping = rkWaylandXdgWmBasePingHandler,
};

static const struct xdg_surface_listener xdgSurfaceListener = {
	.configure = rkWaylandXdgSurfaceConfigure,
};

static const struct xdg_toplevel_listener xdgToplevelListener = {
	.configure	= rkWaylandXdgToplevelConfigure,
	.close		= rkWaylandXdgToplevelClose,
};

void rkWaylandWindowCreate(void) {
	display = wl_display_connect(NULL);
	RK_ASSERT(display == NULL, NULL);

	registry = wl_display_get_registry(display);
	RK_ASSERT(registry == NULL, NULL);

	wl_registry_add_listener(registry, &registryListener, NULL);
	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	RK_ASSERT(compositor == NULL, NULL);
	RK_ASSERT(shell == NULL, NULL);

	wl_surface = wl_compositor_create_surface(compositor);
	RK_ASSERT(wl_surface == NULL, NULL);

	xdg_surface = xdg_wm_base_get_xdg_surface(shell, wl_surface);
	RK_ASSERT(xdg_surface == NULL, NULL);

	xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	RK_ASSERT(xdg_toplevel == NULL, NULL);

	xdg_wm_base_add_listener(shell, &xdgWmBaseListener, NULL);
	xdg_surface_add_listener(xdg_surface, &xdgSurfaceListener, NULL);
	xdg_toplevel_add_listener(xdg_toplevel, &xdgToplevelListener, NULL);
	xdg_toplevel_set_title(xdg_toplevel, CONFIG_PROJECT_NAME);
	wl_surface_commit(wl_surface);

	rkLog(RK_LOG_LEVEL_INFO, "Wayland surface created! (width: %hu, height: %hu, fullscreen: %hu)",
		  (uint16_t)cmdOptions.width, (uint16_t)cmdOptions.height, (uint16_t)cmdOptions.fullscreen);
}

void rkWaylandWindowGetEvents(void) {
	wl_display_dispatch(display);
}

void rkWaylandWindowDestroy(void) {
	if (display != NULL) {
		if (xdg_toplevel != NULL) {
			xdg_toplevel_destroy(xdg_toplevel);
			xdg_toplevel = NULL;
		}

		if (xdg_surface != NULL) {
			xdg_surface_destroy(xdg_surface);
			xdg_surface = NULL;
		}

		if (wl_surface != NULL) {
			wl_surface_destroy(wl_surface);
			wl_surface = NULL;
		}

		if (shell != NULL) {
			xdg_wm_base_destroy(shell);
			shell = NULL;
		}

		if (compositor != NULL) {
			wl_compositor_destroy(compositor);
			compositor = NULL;
		}

		if (registry != NULL) {
			wl_registry_destroy(registry);
			registry = NULL;
		}

		wl_display_disconnect(display);
		display = NULL;

		rkLog(RK_LOG_LEVEL_INFO, "Wayland surface destroyed!");
	}
}

#ifdef CONFIG_RENDERER_VULKAN
void vkWaylandCreateSurfaceWrap(void) {
	const VkWaylandSurfaceCreateInfoKHR surfaceInfo = {
		.sType		= VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
		.pNext		= NULL,
		.flags		= 0,
		.display	= display,
		.surface	= wl_surface,
	};

	const VkResult result = vkCreateWaylandSurfaceKHR(instance, &surfaceInfo,
													  allocationCallbacks, &surface);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Wayland surface created!");
}
#endif
