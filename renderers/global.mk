CFLAGS := $(CFLAGS) -I $(RENDERERS_DIR)

include $(RENDERERS_DIR)/global/renderer.mk

ifdef CONFIG_RENDERER_VULKAN
include $(RENDERERS_DIR)/vulkan/renderer.mk
endif
