ANDROID_X86_BUILD_DIR	:= $(BUILD_DIR)/x86
ANDROID_X86_OBJECTS		:= $(patsubst $(BUILD_DIR)/%.o, $(ANDROID_X86_BUILD_DIR)/%.o, $(OBJECTS))
ANDROID_X86_CC			:= i686-linux-android$(CONFIG_ANDROID_SDK_VERSION)-clang

ifdef CONFIG_RENDERER_VULKAN
$(ANDROID_X86_BUILD_DIR)/shader.o: $(VULKAN_SPV)
endif

$(ANDROID_X86_BUILD_DIR)/%.o: %.c
	@ mkdir -p $(@D)
	@ $(ANDROID_X86_CC) $(CFLAGS) $< -o $@
	$(info Compilling $< (x86))
