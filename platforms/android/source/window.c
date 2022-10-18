#include <asserts.h>
#include <window.h>
#include <config.h>
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#endif

void rkWindowCreate(void) {

}

void rkWindowGetEvents(void) {

}

void rkWindowDestroy(void) {

}

#ifdef CONFIG_RENDERER_VULKAN
const char * vkGetWsiSurfaceExtension(void) {
	return VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
}

void vkCreateSurfaceWrap(void) {
	const VkAndroidSurfaceCreateInfoKHR surfaceInfo = {
		.sType		= VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
		.pNext		= NULL,
		.flags		= 0,
		.window		= NULL,
	};

	const VkResult result = vkCreateAndroidSurfaceKHR(instance, &surfaceInfo,
													  allocationCallbacks, &surface);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Android surface created!");
}
#endif
