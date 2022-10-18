#pragma once

#include <vulkan/vulkan.h>

extern VkSwapchainKHR swapchain;
extern VkImage swapchainImages[];
extern uint_fast8_t swapchainImagesCount;
extern uint_fast8_t swapchainImageIndex;

void vkCreateSwapchainWrap(void);
void vkDestroySwapchainWrap(void);
void vkGetSwapchainImagesWrap(void);
void vkAcquireNextImageWrap(void);
