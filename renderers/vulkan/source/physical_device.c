#include <vulkan/api/physical_device.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#include <vulkan/api/helper.h>
#include <asserts.h>
#include <defines.h>

VkDeviceInfo mainDeviceInfo = {
	.physicalDevice = VK_NULL_HANDLE,
};

static void vkGetMemoryInfo(VkDeviceInfo * const restrict info) {
	VkPhysicalDeviceMemoryProperties * const restrict memoryProperties = &info->memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(info->physicalDevice, memoryProperties);

	const VkFlagInfo memoryFlagsInfo[] = {
		VK_FLAG_INFO(&info->memoryInfo.localId, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
		VK_FLAG_INFO(&info->memoryInfo.hostId, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
		VK_FLAG_INFO(&info->memoryInfo.localHostId, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0),
		VK_FLAG_INFO(NULL, 0, 0),
	};

	vkProcFlags(memoryFlagsInfo, &memoryProperties->memoryTypes->propertyFlags,
				memoryProperties->memoryTypeCount, sizeof(memoryProperties->memoryTypes[0]));
	// TODO: add fully support discrete GPU
	// info->memoryInfo.hasLocal = (info->memoryInfo.localId != UINT8_MAX ? VK_TRUE : VK_FALSE);
	info->memoryInfo.hasLocal = VK_FALSE;

	vkLog(VULKAN_TAG_DEBUG,
		  "Device %s memory info:\n"
		  "\tlocalId=%u\n"
		  "\thostId=%u\n"
		  "\tlocalHostId=%u",
		  info->properties.deviceName,
		  (unsigned)info->memoryInfo.localId,
		  (unsigned)info->memoryInfo.hostId,
		  (unsigned)info->memoryInfo.localHostId);
}

static void vkGetQueueFamilyInfo(VkDeviceInfo * const restrict info) {
	uint32_t queueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(info->physicalDevice, &queueFamiliesCount, NULL);

	VkQueueFamilyProperties queueFamilyProperties[queueFamiliesCount];
	vkGetPhysicalDeviceQueueFamilyProperties(info->physicalDevice, &queueFamiliesCount, queueFamilyProperties);

	uint_fast8_t renderId;
	uint_fast8_t computeId;
	uint_fast8_t presentId;
	uint_fast8_t transferId;

	const VkFlagInfo queueFlagsInfo[] = {
		VK_FLAG_INFO(&renderId, VK_QUEUE_GRAPHICS_BIT, 0),
		VK_FLAG_INFO(&computeId, VK_QUEUE_COMPUTE_BIT, 0),
		VK_FLAG_INFO(&presentId, 0, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT),
		VK_FLAG_INFO(&transferId, VK_QUEUE_TRANSFER_BIT, 0),
		VK_FLAG_INFO(&info->queueFamilyInfo.renderId, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT |
			VK_QUEUE_TRANSFER_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.computeId, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT |
			VK_QUEUE_TRANSFER_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.transferId, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT |
			VK_QUEUE_COMPUTE_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.renderComputeId, VK_QUEUE_GRAPHICS_BIT |
			VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.renderTransferId, VK_QUEUE_GRAPHICS_BIT |
			VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.computeTransferId, VK_QUEUE_COMPUTE_BIT |
			VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT),
		VK_FLAG_INFO(&info->queueFamilyInfo.renderComputeTransferId, VK_QUEUE_GRAPHICS_BIT |
			VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 0),
		VK_FLAG_INFO(NULL, 0, 0),
	};

	vkProcFlags(queueFlagsInfo, &queueFamilyProperties->queueFlags, queueFamiliesCount,
				sizeof(queueFamilyProperties[0]));
	info->queueFamilyInfo.hasRender = (renderId != UINT8_MAX ? VK_TRUE : VK_FALSE);
	info->queueFamilyInfo.hasCompute = (computeId != UINT8_MAX ? VK_TRUE : VK_FALSE);
	info->queueFamilyInfo.hasTransfer = (transferId != UINT8_MAX ? VK_TRUE : VK_FALSE);

	const VkPresentInfo queuePresentInfo[] = {
		VK_PRESENT_INFO(&presentId, &info->queueFamilyInfo.presentId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.renderId, &info->queueFamilyInfo.renderPresentId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.computeId, &info->queueFamilyInfo.computePresentId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.transferId, &info->queueFamilyInfo.presentTransferId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.renderComputeId, &info->queueFamilyInfo.renderComputePresentId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.renderTransferId, &info->queueFamilyInfo.renderPresentTransferId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.computeTransferId, &info->queueFamilyInfo.computePresentTransferId),
		VK_PRESENT_INFO(&info->queueFamilyInfo.renderComputeTransferId,
			&info->queueFamilyInfo.renderComputePresentTransferId),
		VK_PRESENT_INFO(NULL, NULL),
	};

	for (uint_fast8_t id = 0; id < queueFamiliesCount; id++) {
		VkBool32 hasSupport = VK_FALSE;
		const VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(info->physicalDevice, id, surface, &hasSupport);
		RK_VULKAN_ASSERT(result, NULL);

		if (hasSupport) {
			vkMergePresentQueue(queuePresentInfo, id);
			info->queueFamilyInfo.hasPresent = VK_TRUE;
		}
	}

	vkLog(VULKAN_TAG_DEBUG,
		  "Device %s queue family info:\n"
		  "\trenderId=%u\n"
		  "\tcomputeId=%u\n"
		  "\tpresentId=%u\n"
		  "\ttransferId=%u\n"
		  "\trenderComputeId=%u\n"
		  "\trenderPresentId=%u\n"
		  "\trenderTransferId=%u\n"
		  "\tcomputePresentId=%u\n"
		  "\tcomputeTransferId=%u\n"
		  "\tpresentTransferId=%u\n"
		  "\trenderComputePresentId=%u\n"
		  "\trenderComputeTransferId=%u\n"
		  "\trenderPresentTransferId=%u\n"
		  "\tcomputePresentTransferId=%u\n"
		  "\trenderComputePresentTransferId=%u\n"
		  "\thasRender=%u\n"
		  "\thasCompute=%u\n"
		  "\thasPresent=%u\n"
		  "\thasTransfer=%u",
		  info->properties.deviceName,
		  (unsigned)info->queueFamilyInfo.renderId,
		  (unsigned)info->queueFamilyInfo.computeId,
		  (unsigned)info->queueFamilyInfo.presentId,
		  (unsigned)info->queueFamilyInfo.transferId,
		  (unsigned)info->queueFamilyInfo.renderComputeId,
		  (unsigned)info->queueFamilyInfo.renderPresentId,
		  (unsigned)info->queueFamilyInfo.renderTransferId,
		  (unsigned)info->queueFamilyInfo.computePresentId,
		  (unsigned)info->queueFamilyInfo.computeTransferId,
		  (unsigned)info->queueFamilyInfo.presentTransferId,
		  (unsigned)info->queueFamilyInfo.renderComputePresentId,
		  (unsigned)info->queueFamilyInfo.renderComputeTransferId,
		  (unsigned)info->queueFamilyInfo.renderPresentTransferId,
		  (unsigned)info->queueFamilyInfo.computePresentTransferId,
		  (unsigned)info->queueFamilyInfo.renderComputePresentTransferId,
		  (unsigned)info->queueFamilyInfo.hasRender,
		  (unsigned)info->queueFamilyInfo.hasCompute,
		  (unsigned)info->queueFamilyInfo.hasPresent,
		  (unsigned)info->queueFamilyInfo.hasTransfer);
}

static void vkSelectMainDevice(const VkPhysicalDevice * const restrict devices, const uint_fast8_t devicesCount) {
	for (uint_fast8_t i = 0; i < devicesCount; i++) {
		VkDeviceInfo info = { .physicalDevice = devices[i] };
		vkGetPhysicalDeviceProperties(info.physicalDevice, &info.properties);

		vkGetMemoryInfo(&info);
		vkGetQueueFamilyInfo(&info);

		do {
			if (info.queueFamilyInfo.hasRender && info.queueFamilyInfo.hasPresent) {
				if (info.memoryInfo.hasLocal && info.queueFamilyInfo.hasTransfer == VK_FALSE) {
					break;
				}

				vkLog(VULKAN_TAG_INFO, "Selected device %s! (id=%u)",
					  info.properties.deviceName, (unsigned)i);
				mainDeviceInfo = info;
				return;
			}
		} while (VK_FALSE);

		vkLog(VULKAN_TAG_DEBUG, "Skipped device %s! (id=%u)",
			  info.properties.deviceName, (unsigned)i);
	}

	rkExit(RK_RESULT_VULKAN_DEVICE_NOT_FOUND, NULL);
}

void vkEnumeratePhysicalDevicesWrap(void) {
	uint32_t devicesCount;
	VkResult result = vkEnumeratePhysicalDevices(instance, &devicesCount, NULL);
	RK_VULKAN_ASSERT(result, NULL);

	VkPhysicalDevice devices[devicesCount];
	result = vkEnumeratePhysicalDevices(instance, &devicesCount, devices);
	RK_VULKAN_ASSERT(result, NULL);

	vkSelectMainDevice(devices, devicesCount);
}
