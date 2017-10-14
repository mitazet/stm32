# Minimal makefile for STM32F3 Neucleo board.
TARGET := arm-none-eabi

# Find the devtools
LIBGCC := /usr/lib/gcc/$(TARGET)/4.8/
CC := $(shell { which $(TARGET)-gcc || which $(TARGET)-gcc-x86; } 2>/dev/null)
LD := $(shell { which $(TARGET)-ld || which $(TARGET)-ld-x86; } 2>/dev/null)
AS := $(shell { which $(TARGET)-as || which $(TARGET)-as-x86; } 2>/dev/null)
OBJCOPY := $(shell { which $(TARGET)-objcopy || which $(TARGET)-objcopy-x86; } 2>/dev/null)
STM32FLASH := $(shell { which stm32flash || which stm32flash-x86; } 2>/dev/null)
STFLASH := $(shell { which st-flash || which st-flash-x86; } 2>/dev/null)
PORT := $(shell { ls /dev/ttyACM0 || ls /dev/ports/usb0; } 2>/dev/null)

# If hey is not available, this resolves to #, and makes the hey commands
# invisible to the shell. Dirty, I know.
HEY := $(shell { which hey || echo \\\# ; } 2>/dev/null)

CFLAGS = -g3 -mcpu=cortex-m4 -mthumb -nostdlib -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c99
INCLUDE := -I./cmsis -I./system -I./printf
OBJDIR := ./obj
OBJECTS = $(OBJDIR)/startup.o $(OBJDIR)/main.o $(OBJDIR)/printf.o
MEMORYMAP = STM32F303K8.ld
OBJECTNAME = STM

$(OBJECTNAME).bin: $(OBJECTNAME).out
	$(OBJCOPY) $(OBJECTNAME).out -I ihex -O binary $(OBJECTNAME).bin

$(OBJECTNAME).out: $(OBJECTS)
	$(LD) -T $(MEMORYMAP) $(OBJECTS) -L$(LIBGCC) -o $(OBJECTNAME).out

$(OBJDIR)/startup.o:  ./startup/startup_stm32f303x8.s
	-mkdir -p $(OBJDIR)
	$(AS) -g $< -o $@

$(OBJDIR)/main.o: ./main.c
	-mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/printf.o:   ./printf/printf.c
	-mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean flash

stm32flash: $(OBJECTNAME).bin
	$(HEY) SerialConnect delete port
	$(STM32FLASH) -v -w $^ $(PORT)
	$(HEY) SerialConnect set port to $(basename $(PORT))

st-flash: $(OBJECTNAME).bin
	$(STFLASH) write $(OBJECTNAME).bin 0x8000000

clean:
	rm -f $(OBJECTS) *.out *.bin
