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
OUT:=out/$(PROJNAME)/$(PROJNAME).out
HEX:=out/$(PROJNAME)/$(PROJNAME).hex
BIN:=out/$(PROJNAME)/$(PROJNAME).bin
LST:=out/$(PROJNAME)/$(PROJNAME).lst
DUMP:=out/$(PROJNAME)/$(PROJNAME).dump

.PHONY: all avr asm program clean

all: avr asm

avr: $(OUT) $(HEX) $(BIN) $(DUMP) $(LST)


program: $(HEX) $(TEST_HEX)
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
