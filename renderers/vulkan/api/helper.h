#pragma once

#include <vulkan/vulkan.h>

#define VK_FLAG_INFO(value, required, conflicts) \
	{ value, required, conflicts }
#define VK_PRESENT_INFO(src, dst) \
	{ src, dst }

typedef const char * (* VkFlagsToStr)(const VkFlags flags);

typedef struct {
	uint_fast8_t * value;
	VkFlags required;
	VkFlags conflicts;
} VkFlagInfo;

typedef struct {
	uint_fast8_t * srcValue;
	uint_fast8_t * dstValue;
} VkPresentInfo;

void vkFlagsToStr(const VkFlags flags, const VkFlags max, const VkFlagsToStr flagsToStr,
				  const char ** restrict strArray);

uint_fast8_t vkGetIdByFlags(const VkFlags * const restrict flags, const uint_fast8_t length,
							const uint_fast8_t step, const VkFlags required, const VkFlags conflicts);
void vkProcFlags(const VkFlagInfo * restrict flagsInfo, const VkFlags * const restrict flags,
				 const uint_fast8_t length, const uint_fast8_t step);

void vkMergePresentQueue(const VkPresentInfo * restrict presentInfo, const uint_fast8_t id);
uint_fast8_t vkSelectIdByPriority(const uint_fast8_t * const restrict priorities, const uint_fast8_t length);

void vkSelectValueByPriority(const void * const array, const uint_fast8_t arrayLength,
							 const void * const priorities, const uint_fast8_t prioritiesCount,
							 void * const result, const uint_fast8_t size);
void vkSwapHandlers(uint64_t * const restrict array, const uint_fast8_t index1, const uint_fast8_t index2);
