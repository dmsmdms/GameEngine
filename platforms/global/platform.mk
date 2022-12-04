PLATFORM_DIR	:= $(PLATFORMS_DIR)/global
VPATH			:= $(VPATH) $(PLATFORM_DIR)/source
VPATH			:= $(VPATH) $(PLATFORM_DIR)/test
CFLAGS			:= $(CFLAGS) -I $(PLATFORM_DIR)/api

SOURCES := filesystem.c
SOURCES := $(SOURCES) asserts.c
SOURCES := $(SOURCES) vector-math.c
SOURCES := $(SOURCES) simple-math.c

ifdef ARCH_x86
SOURCES := $(SOURCES) vector-math-x86.c
endif

PC_SOURCES	:= main.c
PC_SOURCES	:= $(PC_SOURCES) options.c
PC_SOURCES	:= $(PC_SOURCES) logger.c

ifdef CONFIG_PLATFORM_LINUX
SOURCES := $(SOURCES) $(PC_SOURCES)
endif

ifdef CONFIG_PLATFORM_WINDOWS
SOURCES := $(SOURCES) $(PC_SOURCES)
endif

OBJECTS := $(OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

ifdef CONFIG_TESTS_SUPPORT
TEST_SOURCES := tests.c
TEST_SOURCES := $(TEST_SOURCES) math-tests.c
TEST_SOURCES := $(TEST_SOURCES) asserts.c
TEST_SOURCES := $(TEST_SOURCES) logger.c
TEST_SOURCES := $(TEST_SOURCES) options.c
TEST_SOURCES := $(TEST_SOURCES) vector-math.c

ifdef ARCH_x86
TEST_SOURCES := $(TEST_SOURCES) vector-math-x86.c
endif

TEST_OBJECTS := $(TEST_OBJECTS) $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_SOURCES))
endif
