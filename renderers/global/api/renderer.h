#pragma once

#include <autoconf.h>

typedef void (* RkRendererCallback)(void);

#ifdef CONFIG_RENDERER_VULKAN
void rkVulkanRendererCreate(void);
void rkVulkanRendererDestroy(void);
void rkVulkanRendererDraw(void);
#endif

void rkRendererCreate(void);
extern RkRendererCallback rkRendererDestroy;
extern RkRendererCallback rkRendererDraw;
