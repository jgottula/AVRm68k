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
OUT:=$(PROJNAME).out
HEX:=$(PROJNAME).hex
BIN:=$(PROJNAME).bin
LST:=$(PROJNAME).lst
DUMP:=$(PROJNAME).dump

.PHONY: all avr program asm clean

all: avr

avr: $(OUT) $(HEX) $(BIN) $(DUMP) $(LST)


program: $(HEX) $(TEST_HEX)
	-sudo avrdude $(AVRDUDEFLAGS) -U flash:w:$(HEX)
clean:
	-rm -rf *.out *.hex *.bin *.lst *.dump


asm: $(SOURCES) $(HEADERS) Makefile
	avr-gcc $(CFLAGS) $(INCLUDE) $(LINK) -S $(SOURCES)


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
