#include <vulkan/api/helper.h>
#include <defines.h>
#include <asserts.h>
#include <string.h>

STATIC_ASSERT(sizeof(VkSemaphore) == sizeof(uint64_t));
STATIC_ASSERT(sizeof(VkFence) == sizeof(uint64_t));

void vkFlagsToStr(const VkFlags flags, const VkFlags max, const VkFlagsToStr flagsToStr,
				  const char ** restrict strArray)
{
	for (VkFlags i = 1; i < max; i <<= 1) {
		if (flags & i) {
			*strArray++ = flagsToStr(i);
		}
	}
}

uint_fast8_t vkGetIdByFlags(const VkFlags * const restrict flags, const uint_fast8_t length,
							const uint_fast8_t step, const VkFlags required, const VkFlags conflicts)
{
	uint_fast8_t id = UINT8_MAX;

	for (uint_fast8_t i = 0; i < length; i++) {
		const VkFlags flag = GET_BY_OFFSET(flags, i * step);

		if (flag & conflicts) {
			continue;
		}

		if (RK_CHECK_FLAGS(flag, required)) {
			id = i;
			break;
		}
	}

	return id;
}

void vkProcFlags(const VkFlagInfo * restrict flagsInfo, const VkFlags * const restrict flags,
				 const uint_fast8_t length, const uint_fast8_t step)
{
	while (flagsInfo->value != NULL) {
		*flagsInfo->value = vkGetIdByFlags(flags, length, step, flagsInfo->required,
										   flagsInfo->conflicts);
		flagsInfo++;
	}
}

void vkMergePresentQueue(const VkPresentInfo * restrict presentInfo, const uint_fast8_t id) {
	while (presentInfo->dstValue != NULL) {
		if (*presentInfo->srcValue == id) {
			*presentInfo->srcValue = UINT8_MAX;
			*presentInfo->dstValue = id;
		} else {
			*presentInfo->dstValue = UINT8_MAX;
		}

		presentInfo++;
	}
}

uint_fast8_t vkSelectIdByPriority(const uint_fast8_t * const restrict priorities, const uint_fast8_t length) {
	for (uint_fast8_t i = 0; i < length; i++) {
		const uint_fast8_t priority = priorities[i];

		if (priority != UINT8_MAX) {
			return priority;
		}
	}

	return UINT8_MAX;
}

void vkSelectValueByPriority(const void * const array, const uint_fast8_t arrayLength,
							 const void * const priorities, const uint_fast8_t prioritiesCount,
							 void * const result, const uint_fast8_t size)
{
	for (uint_fast8_t i = 0; i < prioritiesCount; i++) {
		const void * const priority = (char *)priorities + i * size;

		for (uint_fast8_t j = 0; j < arrayLength; j++) {
			const void * const value = (char *)array + j * size;
			const int cmpResult = memcmp(priority, value, size);

			if (cmpResult == RK_RESULT_SUCCESS) {
				memcpy(result, value, size);
				return;
			}
		}
	}
}

void vkSwapHandlers(uint64_t * const restrict array, const uint_fast8_t index1, const uint_fast8_t index2) {
	const uint64_t handler1 = array[index1];
	const uint64_t handler2 = array[index2];
	array[index1] = handler2;
	array[index2] = handler1;
}
