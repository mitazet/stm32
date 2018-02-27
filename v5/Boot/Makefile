# Minimal makefile for STM32F3 Neucleo board.
TARGET := arm-none-eabi-

# devtools
CC := $(TARGET)gcc
LD := $(TARGET)ld
AS := $(TARGET)as
OBJCOPY := $(TARGET)-objcopy
STFLASH := st-flash
PORT := /dev/ttyACM0

CFLAGS = -g3 -mcpu=cortex-m4 -mthumb -nostdlib -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c99
INCLUDE = -I./include

OBJDIR = obj

SOURCES = $(shell find * -name "*.c")
SRCDIR = $(dir $(SOURCES))
BINDIR = $(addprefix $(OBJDIR)/, $(SRCDIR))

ASMS = $(shell find * -name "*.s")
ASMDIR = $(dir $(ASMS))
ASMBIN = $(addprefix $(OBJDIR)/, $(ASMDIR))

OBJECTS = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SOURCES))) $(addprefix $(OBJDIR)/, $(patsubst %.s, %.o, $(ASMS))) 
DEPENDS = $(OBJECTS:.o=.d)

ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif

MEMORYMAP = STM32F303K8.ld
OBJECTNAME = bootloader

default :
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@[ -d "$(BINDIR)" ] || mkdir -p $(BINDIR)
	@[ -d "$(ASMBIN)" ] || mkdir -p $(ASMBIN)
	@make all --no-print-directory

all: $(OBJECTS) $(OBJECTNAME).elf $(OBJECTNAME).bin

$(OBJECTNAME).bin: $(OBJECTNAME).elf
	$(OBJCOPY) $(OBJECTNAME).elf -I ihex -O binary $(OBJECTNAME).bin

$(OBJECTNAME).elf: $(OBJECTS)
	$(LD) -T $(MEMORYMAP) $(OBJECTS) -o $(OBJECTNAME).elf

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: %.s
	$(AS) -g $< -o $@

.PHONY: clean flash

st-flash: $(OBJECTNAME).bin
	$(STFLASH) write $(OBJECTNAME).bin 0x8000000

clean:
	rm -rf $(OBJDIR) *.elf *.bin
