#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <autoconf.h>
#include <options.h>
#include <asserts.h>
#include <window.h>

static const VkApplicationInfo appInfo = {
	.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO,
	.pNext				= NULL,
	.pApplicationName	= CONFIG_PROJECT_NAME,
	.applicationVersion	= VK_MAKE_VERSION(CONFIG_PROJECT_VERSION_MAJOR,
		CONFIG_PROJECT_VERSION_MINOR, CONFIG_PROJECT_VERSION_PATCH),
	.pEngineName		= CONFIG_ENGINE_NAME,
	.engineVersion		= VK_MAKE_VERSION(CONFIG_ENGINE_VERSION_MAJOR,
		CONFIG_ENGINE_VERSION_MINOR, CONFIG_ENGINE_VERSION_PATCH),
	.apiVersion			= VK_API_VERSION_1_0,
};

VkInstance instance = VK_NULL_HANDLE;

void vkCreateInstanceWrap(void) {
	const char * layers[] = { NULL };
	uint_fast8_t layersCount = 0;

	const char * extensions[CONFIG_VULKAN_INSTANCE_EXTENSIONS_MAX] = { NULL };
	uint_fast8_t extensionsCount = 0;

	extensions[extensionsCount++] = vkGetWsiSurfaceExtension();
	extensions[extensionsCount++] = VK_KHR_SURFACE_EXTENSION_NAME;

	if (cmdOptions.vkValidation != VK_FALSE) {
		layers[layersCount++] = cmdOptions.vkValidationLayer;
		extensions[extensionsCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	}

	const VkInstanceCreateInfo info = {
		.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext						= vkDebugUtilsMessengerCreateInfoGet(),
		.flags						= 0,
		.pApplicationInfo			= &appInfo,
		.enabledLayerCount			= layersCount,
		.ppEnabledLayerNames		= layers,
		.enabledExtensionCount		= extensionsCount,
		.ppEnabledExtensionNames	= extensions,
	};

	const VkResult result = vkCreateInstance(&info, allocationCallbacks, &instance);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Instance created!");
}

void vkDestroyInstanceWrap(void) {
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Instance destroyed!");
		instance = VK_NULL_HANDLE;
	}
}
