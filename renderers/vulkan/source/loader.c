#include <vulkan/api/instance.h>

#define VK_LOAD_FUNC(func)										\
	static PFN_##func func = NULL;								\
	if (func == NULL) {											\
		func = (void *)vkGetInstanceProcAddr(instance, #func);	\
	} (void)(func)

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	const VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT * const pCreateInfo,
	const VkAllocationCallbacks * const pAllocator,
	VkDebugUtilsMessengerEXT * const pMessenger)
{
	VK_LOAD_FUNC(vkCreateDebugUtilsMessengerEXT);
	return vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	const VkInstance instance,
	const VkDebugUtilsMessengerEXT messenger,
	const VkAllocationCallbacks * const pAllocator)
{
	VK_LOAD_FUNC(vkDestroyDebugUtilsMessengerEXT);
	return vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
