PROJNAME:=avrm68k
CFLAGS:=-mmcu=atmega1284p -std=c11 -Wall -Wextra -Wno-unused-function -Os \
	-flto -ffreestanding -fwhole-program -fuse-linker-plugin \
	-DF_CPU=20000000UL -DDEBUG
OBJCOPYFLAGS:=-R .eeprom
AVRDUDEFLAGS:=-c usbasp -p m1284p -B 460800
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

.PHONY: all avr asm load clean

all: avr asm

avr: $(OUT) $(HEX) $(BIN) $(DUMP) $(LST)


load: $(HEX) $(TEST_HEX)
	-sudo avrdude $(AVRDUDEFLAGS) -U flash:w:$(HEX)
clean:
	-find out/ -type f -delete


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
