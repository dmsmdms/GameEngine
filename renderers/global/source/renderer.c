#include <global/api/renderer.h>
#include <options.h>
#include <defines.h>

static const RkRendererCallback rendererCreate[] = {
#ifdef CONFIG_RENDERER_VULKAN
	RK_VAL(RK_RENDERER_TYPE_VULKAN, rkVulkanRendererCreate),
#endif
};

static const RkRendererCallback rendererDestroy[] = {
#ifdef CONFIG_RENDERER_VULKAN
	RK_VAL(RK_RENDERER_TYPE_VULKAN, rkVulkanRendererDestroy),
#endif
};

static const RkRendererCallback rendererDraw[] = {
#ifdef CONFIG_RENDERER_VULKAN
	RK_VAL(RK_RENDERER_TYPE_VULKAN, rkVulkanRendererDraw),
#endif
};

static void rkRendererCallback(void) {
	// Do nothing //
}

RkRendererCallback rkRendererDestroy = rkRendererCallback;
RkRendererCallback rkRendererDraw = rkRendererCallback;

void rkRendererCreate(void) {
	const RkRendererType type = cmdOptions.rendererType;
	const RkRendererCallback create = rendererCreate[type];
	rkRendererDestroy = rendererDestroy[type];
	rkRendererDraw = rendererDraw[type];
	create();
}
