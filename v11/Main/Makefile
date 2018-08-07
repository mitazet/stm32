# Minimal makefile for STM32F3 Neucleo board.
TARGET := arm-none-eabi-

# devtools
CC := $(TARGET)gcc
CXX:= $(TARGET)g++
LD := $(TARGET)ld
AS := $(TARGET)as
OBJCOPY := $(TARGET)objcopy
STFLASH := st-flash
PORT := /dev/ttyACM0

BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -not -path "*/test/*" -or -name "*.c" -or -name "*.s")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_MAKE := $(shell find $(SRC_DIRS) -name "Makefile" -path "*/test/*")
TEST_DIR := $(dir $(TEST_MAKE))

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

CFLAGS := $(INC_FLAGS) -g3 -mcpu=cortex-m4 -mthumb -nostdlib -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c99
CXXFLAGS := $(INC_FLAGS) -g3 -mcpu=cortex-m4 -mthumb -nostdlib -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c++11

MEMORYMAP := STM32F303K8.ld
LDFLAGS := -T $(SRC_DIRS)/$(MEMORYMAP)

OBJECTNAME = STM

default :
	@make all --no-print-directory

all: test $(BUILD_DIR)/$(OBJECTNAME).bin

$(BUILD_DIR)/$(OBJECTNAME).bin: $(BUILD_DIR)/$(OBJECTNAME).elf
	$(OBJCOPY) $(BUILD_DIR)/$(OBJECTNAME).elf -I ihex -O binary $(BUILD_DIR)/$(OBJECTNAME).bin

$(BUILD_DIR)/$(OBJECTNAME).elf: $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) -g $< -o $@

test : 
	@for subdir in $(TEST_DIR) ; do \
		make -C $$subdir ;\
	done

.PHONY: clean flash

st-flash: $(OBJECTNAME).bin
	$(STFLASH) write $(OBJECTNAME).bin 0x8000000

clean:
	rm -rf $(BUILD_DIR)
	@for subdir in $(TEST_DIR) ; do \
		make clean -C $$subdir ;\
	done

-include $(DEPS)

MKDIR_P ?= mkdir -p
