PLATFORM_DIR	:= $(PLATFORMS_DIR)/windows
TARGET			:= $(BUILD_DIR)/$(CONFIG_PROJECT_NAME).exe
VPATH			:= $(VPATH) $(PLATFORM_DIR)/source

## Compiler Flags ##

CFLAGS	:= $(CFLAGS) -flto -I $(PLATFORM_DIR)/api
LDFLAGS	:= $(LDFLAGS) --lto

ifdef CONFIG_RENDERER_VULKAN
CONFIG_VULKAN_SDK_PATH	:= $(patsubst "%",%, $(CONFIG_VULKAN_SDK_PATH))
CFLAGS					:= $(CFLAGS) -I $(CONFIG_VULKAN_SDK_PATH)/Include
LDFLAGS					:= $(LDFLAGS) -L $(CONFIG_VULKAN_SDK_PATH)/Lib
LDFLAGS					:= $(LDFLAGS) -lvulkan-1
$(BUILD_DIR)/shader.o:	$(VULKAN_SPV)
endif

## Sources ##

SOURCES := window.c
OBJECTS := $(OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

## Targets ##

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@ gcc $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	$(info Linking $@)

$(BUILD_DIR)/%.o: %.c
	@ mkdir -p $(@D)
	@ gcc $(CFLAGS) $< -o $@
	$(info Compilling $<)
