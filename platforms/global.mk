get-prop = $(patsubst $(strip $(2))=%,%, $(shell grep $(2) $(1)))

## Platform makefiles ##

include $(PLATFORMS_DIR)/global/platform.mk

ifdef CONFIG_PLATFORM_LINUX
include $(PLATFORMS_DIR)/linux/platform.mk
endif

ifdef CONFIG_PLATFORM_WINDOWS
include $(PLATFORMS_DIR)/windows/platform.mk
endif

ifdef CONFIG_PLATFORM_ANDROID
include $(PLATFORMS_DIR)/android/platform.mk
endif
