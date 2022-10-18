#pragma once

#include <autoconf.h>

typedef void (* RkWsiCallback)(void);

#ifdef CONFIG_WSI_XCB
void rkXcbWindowCreate(void);
void rkXcbWindowGetEvents(void);
void rkXcbWindowDestroy(void);
#ifdef CONFIG_RENDERER_VULKAN
void vkXcbCreateSurfaceWrap(void);
#endif
#endif

#ifdef CONFIG_WSI_WAYLAND
void rkWaylandWindowCreate(void);
void rkWaylandWindowGetEvents(void);
void rkWaylandWindowDestroy(void);
#ifdef CONFIG_RENDERER_VULKAN
void vkWaylandCreateSurfaceWrap(void);
#endif
#endif

#ifdef CONFIG_RENDERER_VULKAN
const char * vkGetWsiSurfaceExtension(void);
extern RkWsiCallback vkCreateSurfaceWrap;
#endif

void rkWindowCreate(void);
extern RkWsiCallback rkWindowGetEvents;
extern RkWsiCallback rkWindowDestroy;
