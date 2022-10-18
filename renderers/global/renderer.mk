RENDERER_DIR	:= $(RENDERERS_DIR)/global
VPATH			:= $(VPATH) $(RENDERER_DIR)/source

SOURCES := renderer.c
SOURCES := $(SOURCES) vertex.c
OBJECTS := $(OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))
