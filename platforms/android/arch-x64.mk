ANDROID_X64_BUILD_DIR	:= $(BUILD_DIR)/x64
ANDROID_X64_OBJECTS		:= $(patsubst $(BUILD_DIR)/%.o, $(ANDROID_X64_BUILD_DIR)/%.o, $(OBJECTS))
ANDROID_X64_CC			:= x86_64-linux-android$(CONFIG_ANDROID_SDK_VERSION)-clang

ifdef CONFIG_RENDERER_VULKAN
$(ANDROID_X64_BUILD_DIR)/shader.o: $(VULKAN_SPV)
endif

$(ANDROID_X64_BUILD_DIR)/%.o: %.c
	@ mkdir -p $(@D)
	@ $(ANDROID_X64_CC) $(CFLAGS) $< -o $@
	$(info Compilling $< (x64))
