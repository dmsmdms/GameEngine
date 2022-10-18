#pragma once

#include <autoconf.h>

void rkWindowCreate(void);
void rkWindowGetEvents(void);
void rkWindowDestroy(void);

#ifdef CONFIG_RENDERER_VULKAN
const char * vkGetWsiSurfaceExtension(void);
void vkCreateSurfaceWrap(void);
#endif
