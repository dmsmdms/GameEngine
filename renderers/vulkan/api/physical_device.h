#pragma once

#include <vulkan/vulkan.h>

typedef struct {
	uint_fast8_t localId;
	uint_fast8_t hostId;
	uint_fast8_t localHostId;
	uint_fast8_t hasLocal : 1;
} VkMemoryInfo;

typedef struct {
	uint_fast8_t renderId;
	uint_fast8_t computeId;
	uint_fast8_t presentId;
	uint_fast8_t transferId;
	uint_fast8_t renderComputeId;
	uint_fast8_t renderPresentId;
	uint_fast8_t renderTransferId;
	uint_fast8_t computePresentId;
	uint_fast8_t computeTransferId;
	uint_fast8_t presentTransferId;
	uint_fast8_t renderComputePresentId;
	uint_fast8_t renderComputeTransferId;
	uint_fast8_t renderPresentTransferId;
	uint_fast8_t computePresentTransferId;
	uint_fast8_t renderComputePresentTransferId;
	uint_fast8_t hasRender : 1;
	uint_fast8_t hasCompute : 1;
	uint_fast8_t hasPresent : 1;
	uint_fast8_t hasTransfer : 1;
} VkQueueFamilyInfo;

typedef struct {
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	VkQueueFamilyInfo queueFamilyInfo;
	VkMemoryInfo memoryInfo;
} VkDeviceInfo;

extern VkDeviceInfo mainDeviceInfo;

void vkEnumeratePhysicalDevicesWrap(void);
