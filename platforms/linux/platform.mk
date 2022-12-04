PLATFORM_DIR	:= $(PLATFORMS_DIR)/linux
TARGET			:= $(BUILD_DIR)/$(CONFIG_PROJECT_NAME)
VPATH			:= $(VPATH) $(PLATFORM_DIR)/source
VPATH			:= $(VPATH) $(BUILD_DIR)

ifdef CONFIG_TESTS_SUPPORT
TEST_TARGET	:= $(BUILD_DIR)/tests
endif

## Compiler Flags ##

CFLAGS	:= $(CFLAGS) -flto -I $(PLATFORM_DIR)/api
LDFLAGS	:= $(LDFLAGS) --lto

ifdef CONFIG_RENDERER_VULKAN
LDFLAGS					:= $(LDFLAGS) -lvulkan
$(BUILD_DIR)/shader.o:	$(VULKAN_SPV)
endif

## Sources ##

SOURCES := window.c

ifdef CONFIG_WSI_XCB
SOURCES := $(SOURCES) window_xcb.c
LDFLAGS	:= $(LDFLAGS) -lxcb
endif

ifdef CONFIG_WSI_WAYLAND
CONFIG_WAYLAND_XDG_DIR	:= $(patsubst "%",%, $(CONFIG_WAYLAND_XDG_DIR))
VPATH					:= $(VPATH) $(CONFIG_WAYLAND_XDG_DIR)

WAYLAND_XML		:= xdg-shell.xml
WAYLAND_HEADERS	:= $(patsubst %.xml, $(BUILD_DIR)/%-client-protocol.h, $(WAYLAND_XML))
WAYLAND_SOURCE	:= $(patsubst %.xml, %-protocol.c, $(WAYLAND_XML))
.PRECIOUS		:  $(addprefix $(BUILD_DIR)/, $(WAYLAND_SOURCE))

SOURCES	:= $(SOURCES) $(WAYLAND_SOURCE)
SOURCES	:= $(SOURCES) window_wayland.c
LDFLAGS	:= $(LDFLAGS) -lwayland-client

$(BUILD_DIR)/window_wayland.o: $(WAYLAND_HEADERS)
$(BUILD_DIR)/%-protocol.o: $(WAYLAND_HEADERS)

$(BUILD_DIR)/%-client-protocol.h: %.xml
	@ mkdir -p $(@D)
	@ wayland-scanner client-header < $< > $@
	$(info Generating $@)

$(BUILD_DIR)/%-protocol.c: %.xml
	@ mkdir -p $(@D)
	@ wayland-scanner private-code < $< > $@
	$(info Generating $@)
endif

OBJECTS := $(OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

## Targets ##

all: $(TARGET)

$(TARGET): $(OBJECTS) $(TEST_TARGET)
	@ gcc $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	$(info Linking $@)

$(BUILD_DIR)/%.o: %.c
	@ mkdir -p $(@D)
	@ gcc $(CFLAGS) $< -o $@
	$(info Compilling $<)

ifdef CONFIG_TESTS_SUPPORT
$(TEST_TARGET): $(TEST_OBJECTS)
	@ gcc $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)
	@ $(TEST_TARGET) --log-info=on --log-debug=on
endif
