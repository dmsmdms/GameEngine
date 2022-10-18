#include <options.h>
#include <defines.h>
#include <config.h>
#include <window.h>
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/vulkan.h>
#endif

static const RkWsiCallback windowCreate[] = {
#ifdef CONFIG_WSI_XCB
	RK_VAL(RK_WSI_TYPE_XCB, rkXcbWindowCreate),
#endif
#ifdef CONFIG_WSI_WAYLAND
	RK_VAL(RK_WSI_TYPE_WAYLAND, rkWaylandWindowCreate),
#endif
};

static const RkWsiCallback windowGetEvents[] = {
#ifdef CONFIG_WSI_XCB
	RK_VAL(RK_WSI_TYPE_XCB, rkXcbWindowGetEvents),
#endif
#ifdef CONFIG_WSI_WAYLAND
	RK_VAL(RK_WSI_TYPE_WAYLAND, rkWaylandWindowGetEvents),
#endif
};

static const RkWsiCallback windowDestroy[] = {
#ifdef CONFIG_WSI_XCB
	RK_VAL(RK_WSI_TYPE_XCB, rkXcbWindowDestroy),
#endif
#ifdef CONFIG_WSI_WAYLAND
	RK_VAL(RK_WSI_TYPE_WAYLAND, rkWaylandWindowDestroy),
#endif
};

#ifdef CONFIG_RENDERER_VULKAN
static const char * const vkWsiSurfaceExtensions[] = {
#ifdef CONFIG_WSI_XCB
	RK_VAL(RK_WSI_TYPE_XCB, VK_KHR_XCB_SURFACE_EXTENSION_NAME),
#endif
#ifdef CONFIG_WSI_WAYLAND
	RK_VAL(RK_WSI_TYPE_WAYLAND, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME),
#endif
};

static const RkWsiCallback vkSurfaceCreateCallbacks[] = {
	#ifdef CONFIG_WSI_XCB
		RK_VAL(RK_WSI_TYPE_XCB, vkXcbCreateSurfaceWrap),
	#endif
	#ifdef CONFIG_WSI_WAYLAND
		RK_VAL(RK_WSI_TYPE_WAYLAND, vkWaylandCreateSurfaceWrap),
	#endif
};
#endif

static void rkWindowCallback(void) {
	// Do nothing //
}

RkWsiCallback rkWindowGetEvents = rkWindowCallback;
RkWsiCallback rkWindowDestroy = rkWindowCallback;
#ifdef CONFIG_RENDERER_VULKAN
RkWsiCallback vkCreateSurfaceWrap = rkWindowCallback;
#endif

void rkWindowCreate(void) {
	const RkWsiType type = cmdOptions.wsiType;
	const RkWsiCallback create = windowCreate[type];
	rkWindowGetEvents = windowGetEvents[type];
	rkWindowDestroy = windowDestroy[type];
#ifdef CONFIG_RENDERER_VULKAN
	vkCreateSurfaceWrap = vkSurfaceCreateCallbacks[type];
#endif
	create();
}

#ifdef CONFIG_RENDERER_VULKAN
const char * vkGetWsiSurfaceExtension(void) {
	const RkWsiType type = cmdOptions.wsiType;
	return vkWsiSurfaceExtensions[type];
}
#endif
