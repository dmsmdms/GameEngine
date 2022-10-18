ANDROID_ARMV7_BUILD_DIR	:= $(BUILD_DIR)/ARMv7
ANDROID_ARMV7_OBJECTS	:= $(patsubst $(BUILD_DIR)/%.o, $(ANDROID_ARMV7_BUILD_DIR)/%.o, $(OBJECTS))
ANDROID_ARMV7_CC		:= armv7a-linux-androideabi$(CONFIG_ANDROID_SDK_VERSION)-clang

ifdef CONFIG_RENDERER_VULKAN
$(ANDROID_ARMV7_BUILD_DIR)/shader.o: $(VULKAN_SPV)
endif

$(ANDROID_ARMV7_BUILD_DIR)/%.o: %.c
	@ mkdir -p $(@D)
	@ $(ANDROID_ARMV7_CC) $(CFLAGS) $< -o $@
	$(info Compilling $< (ARMv7))
