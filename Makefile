PROJNAME:=avrm68k
MCU:=atmega1284p
CPU_KHZ:=20000
AVRDUDE_MCU:=m1284p
AVRDUDE_BAUD:=460800

CFLAGS:=-mmcu=$(MCU) -std=gnu11 -Wall -Wextra -Wno-unused-function -O2 -flto \
	-ffreestanding -fwhole-program -fuse-linker-plugin -mrelax -DDEBUG \
	-DF_CPU=$(CPU_KHZ)000UL
OBJCOPYFLAGS:=-R .eeprom
AVRDUDEFLAGS:=-c usbasp -p $(AVRDUDE_MCU) -B $(AVRDUDE_BAUD)
SOURCES:=$(wildcard *.c)
HEADERS:=$(wildcard *.h)
INCLUDE:=
LINK:=
GLOBAL_OUT:=out/$(PROJNAME)
OUT:=$(GLOBAL_OUT)/$(PROJNAME).out
HEX:=$(GLOBAL_OUT)/$(PROJNAME).hex
BIN:=$(GLOBAL_OUT)/$(PROJNAME).bin
LST:=$(GLOBAL_OUT)/$(PROJNAME).lst
DUMP:=$(GLOBAL_OUT)/$(PROJNAME).dump


.PHONY: all avr asm prog load clean


all: avr asm

avr: $(OUT) $(HEX) $(BIN) $(DUMP) $(LST)

prog:
	make -C prog all

load: $(HEX) $(TEST_HEX)
	-sudo avrdude $(AVRDUDEFLAGS) -U flash:w:$(HEX)

clean:
	-find out/ -type f -print -delete


asm: $(SOURCES) $(HEADERS) Makefile
	cd out/asm && find ../.. -maxdepth 1 -type f -iname '*.c' -print0 | \
		xargs -0 avr-gcc $(CFLAGS) $(INCLUDE) $(LINK) -S


$(OUT): $(SOURCES) $(HEADERS) Makefile
	avr-gcc $(CFLAGS) $(INCLUDE) $(LINK) -o $@ $(SOURCES)

$(HEX): $(OUT)
	avr-objcopy -O ihex $(OBJCOPYFLAGS) $(OUT) $@

$(BIN): $(OUT)
	avr-objcopy -O binary $(OBJCOPYFLAGS) $(OUT) $@

$(DUMP): $(OUT)
	avr-objdump -s -d $(OUT) >$@

$(LST): $(OUT)
	avr-nm -ns $(OUT) >$@
