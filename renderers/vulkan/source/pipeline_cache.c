#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/physical_device.h>
#include <vulkan/api/pipeline_cache.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/device.h>
#include <filesystem.h>
#include <asserts.h>
#include <string.h>

static const RkFilePermissionFlags permissions = RK_FILE_PERMISSION_NONE
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_READ_OWNER
	| RK_FILE_PERMISSION_READ_OWNER
#endif
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_WRITE_OWNER
	| RK_FILE_PERMISSION_WRITE_OWNER
#endif
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_READ_GROUP
	| RK_FILE_PERMISSION_READ_GROUP
#endif
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_WRITE_GROUP
	| RK_FILE_PERMISSION_WRITE_GROUP
#endif
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_READ_OTHER
	| RK_FILE_PERMISSION_READ_OTHER
#endif
#ifdef CONFIG_VULKAN_PIPELINE_CACHE_WRITE_OTHER
	| RK_FILE_PERMISSION_WRITE_OTHER
#endif
;

VkPipelineCache mainPipelineCache = VK_NULL_HANDLE;

static void vkGetPipelineCacheDataWrap(void) {
	size_t size;
	VkResult result = vkGetPipelineCacheData(mainDevice, mainPipelineCache, &size, NULL);
	RK_VULKAN_ASSERT(result, NULL);

	char buffer[size];
	result = vkGetPipelineCacheData(mainDevice, mainPipelineCache, &size, buffer);
	RK_VULKAN_ASSERT(result, NULL);

	const char * const path = CONFIG_VULKAN_PIPELINE_CACHE_PATH;
	rkCreateFileSimple(path, buffer, size, permissions);
	vkLog(VULKAN_TAG_DEBUG, "Write pipeline cache file %s!", path);
}

void vkCreatePipelineCacheWrap(void) {
	uint_fast32_t size = 0;
	const char * const path = CONFIG_VULKAN_PIPELINE_CACHE_PATH;
	const RkResult fileResult = rkGetFileSizeByPath(path, &size);

	char buffer[size];

	if (fileResult == RK_RESULT_SUCCESS) {
		size = rkReadFileSimple(path, buffer, size);
		vkLog(VULKAN_TAG_DEBUG, "Read pipeline cache file %s!", path);

		const VkPipelineCacheHeaderVersionOne * const restrict header = (void *)buffer;
		const int result = memcmp(header->pipelineCacheUUID, mainDeviceInfo.properties.pipelineCacheUUID,
								  sizeof(header->pipelineCacheUUID));
		if (result != RK_RESULT_SUCCESS) {
			vkLog(VULKAN_TAG_DEBUG, "Undefined pipelineCacheUUID!");
			size = 0;
		}
	} else {
		vkLog(VULKAN_TAG_DEBUG, "Not found pipeline cache file %s!", path);
	}

	const VkPipelineCacheCreateInfo info = {
		.sType				= VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.pNext				= NULL,
		.flags				= 0,
		.initialDataSize	= size,
		.pInitialData		= (void *)buffer,
	};

	const VkResult result = vkCreatePipelineCache(mainDevice, &info, allocationCallbacks, &mainPipelineCache);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Main pipeline cache created!");
}

void vkDestroyPipelineCacheWrap(void) {
	if (mainPipelineCache != VK_NULL_HANDLE) {
		vkGetPipelineCacheDataWrap();
		vkDestroyPipelineCache(mainDevice, mainPipelineCache, allocationCallbacks);
		vkLog(VULKAN_TAG_INFO, "Main pipeline cache destroyed!");
		mainPipelineCache = VK_NULL_HANDLE;
	}
}
