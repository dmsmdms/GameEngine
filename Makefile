.PHONY			:  all adb-install adb-run adb-uninstall defconfig menuconfig savedefconfig clean
.DEFAULT_GOAL	:= all

## Project ##

ifndef PROJECT
$(error Please export PROJECT=path/to/your/project.mk)
endif

ifneq ($(PROJECT), $(wildcard $(PROJECT)))
$(error Project $(PROJECT) not found)
endif

include $(PROJECT)

## Defines ##

PROJECT_DIR			:= $(patsubst %/,%, $(dir $(PROJECT)))
PROJECT_DIR_FULL	:= $(abspath $(PROJECT_DIR))
BUILD_DIR			:= $(PROJECT_DIR)/build
BUILD_DIR_FULL		:= $(abspath $(BUILD_DIR))
PLATFORMS_DIR		:= platforms
RENDERERS_DIR		:= renderers

## Build OS ##

ifneq ($(OS), Windows_NT)
export OS := $(shell uname -s)
endif

ifeq ($(OS), Linux)
OS_LINUX := y
else ifeq ($(OS), Windows_NT)
OS_WINDOWS := y
else
$(error $(OS) is not supported)
endif

## KConfig ##

export KCONFIG_ALLCONFIG	:= $(PROJECT_DIR_FULL)/project.config
export KCONFIG_CONFIG		:= $(BUILD_DIR_FULL)/config
export KCONFIG_AUTOHEADER	:= $(BUILD_DIR_FULL)/autoconf.h
export KCONFIG_ROOT_DIR		:= $(shell pwd)

KCONFIG_INCLUDE_DIR		:= $(BUILD_DIR)/include
KCONFIG_GENERATED_DIR	:= $(KCONFIG_INCLUDE_DIR)/generated
KCONFIG_CONFIG_DIR		:= $(KCONFIG_INCLUDE_DIR)/config
KCONFIG_FILE_FULL		:= $(abspath KConfig)

include $(wildcard $(KCONFIG_CONFIG))

## Project Defines ##

CONFIG_PROJECT_NAME	:= $(patsubst "%",%, $(CONFIG_PROJECT_NAME))
CONFIG_ENGINE_NAME	:= $(patsubst "%",%, $(CONFIG_ENGINE_NAME))
CFLAGS				:= -c -Wall -Werror -MD -I $(BUILD_DIR)

ifdef CONFIG_DEBUG_BUILD
CFLAGS := $(CFLAGS) -g -O0
else
CFLAGS := $(CFLAGS) -O3
endif

ifdef CONFIG_NATIVE_BUILD
CFLAGS := $(CFLAGS) -march=native
endif

## Header Dependencies ##

ifeq ($(BUILD_DIR), $(wildcard $(BUILD_DIR)))
include $(shell find $(BUILD_DIR) -name *.d)
endif

## Build Scripts ##

include $(RENDERERS_DIR)/global.mk
include $(PLATFORMS_DIR)/global.mk

## Targets ##

defconfig: $(KCONFIG_ALLCONFIG)
	@ mkdir -p $(BUILD_DIR)
	@ mkdir -p $(KCONFIG_GENERATED_DIR)
	@ mkdir -p $(KCONFIG_CONFIG_DIR)
	@ rm -f $(KCONFIG_AUTOHEADER)
	@ cd $(BUILD_DIR) && kconfig-conf --alldefconfig $(KCONFIG_FILE_FULL)
	@ cd $(BUILD_DIR) && kconfig-conf --silentoldconfig $(KCONFIG_FILE_FULL)
	@ rm -rf $(KCONFIG_INCLUDE_DIR)

menuconfig: $(KCONFIG_CONFIG)
	@ mkdir -p $(BUILD_DIR)
	@ mkdir -p $(KCONFIG_GENERATED_DIR)
	@ mkdir -p $(KCONFIG_CONFIG_DIR)
	@ rm -f $(KCONFIG_AUTOHEADER)
	@ cd $(BUILD_DIR) && kconfig-mconf $(KCONFIG_FILE_FULL)
	@ cd $(BUILD_DIR) && kconfig-conf --silentoldconfig $(KCONFIG_FILE_FULL)
	@ rm -rf $(KCONFIG_INCLUDE_DIR)

savedefconfig: $(KCONFIG_CONFIG)
	@ cd $(BUILD_DIR) && kconfig-conf --savedefconfig $(KCONFIG_ALLCONFIG) $(KCONFIG_FILE_FULL)

clean:
	@ rm -rf $(BUILD_DIR)
