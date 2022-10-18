RENDERER_DIR	:= $(RENDERERS_DIR)/vulkan
VPATH			:= $(VPATH) $(RENDERER_DIR)/source
SHADER_DIR		:= $(RENDERER_DIR)/shader

## Shaders ##

VULKAN_SHADERS	:= main.vert.glsl main.frag.glsl
VULKAN_SPV		:= $(patsubst %.glsl, $(BUILD_DIR)/%.spv.h, $(VULKAN_SHADERS))

## Sources ##

SOURCES	:= renderer_vulkan.c
SOURCES	:= $(SOURCES) instance.c
SOURCES	:= $(SOURCES) debug_utils.c
SOURCES	:= $(SOURCES) helper.c
SOURCES	:= $(SOURCES) loader.c
SOURCES	:= $(SOURCES) surface.c
SOURCES	:= $(SOURCES) physical_device.c
SOURCES	:= $(SOURCES) device.c
SOURCES	:= $(SOURCES) shader.c
SOURCES	:= $(SOURCES) swapchain.c
SOURCES	:= $(SOURCES) image_view.c
SOURCES	:= $(SOURCES) render_pass.c
SOURCES	:= $(SOURCES) pipeline_layout.c
SOURCES	:= $(SOURCES) pipeline_cache.c
SOURCES	:= $(SOURCES) pipeline.c
SOURCES	:= $(SOURCES) buffer.c
SOURCES	:= $(SOURCES) command_pool.c
SOURCES	:= $(SOURCES) command_buffer.c
SOURCES	:= $(SOURCES) framebuffer.c
SOURCES	:= $(SOURCES) semaphore.c
SOURCES	:= $(SOURCES) fence.c
SOURCES	:= $(SOURCES) queue.c
OBJECTS := $(OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

## Targets ##

$(BUILD_DIR)/%.spv.h: $(SHADER_DIR)/%.glsl
	@ mkdir -p $(@D)
	@ glslangValidator --vn $(subst .,_, $(@F)) -V $< -o $@
	$(info Validating $<)
