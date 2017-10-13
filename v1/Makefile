# Minimal makefile for STM32F3 Neucleo board.
TARGET := arm-none-eabi

# Find the devtools
CPATH := /usr/bin/$(TARGET)
LIBGCC := /usr/lib/gcc/$(TARGET)/4.8/
CC := $(shell { which $(TARGET)-gcc || which $(TARGET)-gcc-x86; } 2>/dev/null)
LD := $(shell { which $(TARGET)-ld || which $(TARGET)-ld-x86; } 2>/dev/null)
AS := $(shell { which $(TARGET)-as || which $(TARGET)-as-x86; } 2>/dev/null)
OBJCOPY := $(shell { which $(TARGET)-objcopy || which $(TARGET)-objcopy-x86; } 2>/dev/null)
STM32FLASH := $(shell { which stm32flash || which stm32flash-x86; } 2>/dev/null)
STFLASH := $(shell { which st-flash || which st-flash-x86; } 2>/dev/null)
PORT := $(shell { ls /dev/ttyS0 || ls /dev/ports/usb0; } 2>/dev/null)

# If hey is not available, this resolves to #, and makes the hey commands
# invisible to the shell. Dirty, I know.
HEY := $(shell { which hey || echo \\\# ; } 2>/dev/null)

CFLAGS = -g3 -mcpu=cortex-m4 -mthumb -nostdlib -Isrc -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c99
OBJECTS = startup.o main.o printf.o
MEMORYMAP = STM32F303K8Tx_FLASH.ld
OBJECTNAME = STM

$(OBJECTNAME).bin: $(OBJECTNAME).out
	$(OBJCOPY) $(OBJECTNAME).out -I ihex -O binary $(OBJECTNAME).bin

$(OBJECTNAME).out: $(OBJECTS)
	$(LD) -T $(MEMORYMAP) $(OBJECTS) -L$(LIBGCC) -o $(OBJECTNAME).out

startup.o:  ./src/startup_stm32f303x8.s
	$(AS) -g $< -o $@

main.o: ./src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

printf.o:   ./src/printf.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean flash

stm32flash: $(OBJECTNAME).bin
	$(HEY) SerialConnect delete port
	$(STM32FLASH) -v -w $^ $(PORT)
	$(HEY) SerialConnect set port to $(basename $(PORT))

st-flash: $(OBJECTNAME).bin
	$(STFLASH) write $(OBJCTNAME).bin 0x8000000

clean:
	rm *.o *.out *.bin
