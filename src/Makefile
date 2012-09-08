# AVRm68k
# (c) 2012 Justin Gottula
# The source code of this project is distributed under the terms of the
# simplified BSD license. See the LICENSE file for details.

# project makefile

EMU_BASENAME:=emulator
EMU_MCU:=atmega1284p
EMU_FREQ_KHZ:=20000
EMU_CFLAGS:=-mmcu=$(EMU_MCU) -DF_CPU=$(EMU_FREQ_KHZ)000UL
EMU_AVRDUDEFLAGS:=-c usbasp -p m1284p -B 460800

MMU_BASENAME:=mmu
MMU_MCU:=atmega324a
MMU_FREQ_KHZ:=20000
MMU_CFLAGS:=-mmcu=$(MMU_MCU) -DF_CPU=$(MMU_FREQ_KHZ)000UL
MMU_AVRDUDEFLAGS:=-c usbasp -p m324a -B 460800

PROG1_BASENAME:=prog1
PROG1_ASFLAGS:=-mcpu=68030 -mno-68851 -mno-68881 -mno-68882 -mno-float
PROG2_BASENAME:=prog2
PROG2_CFLAGS:=-c -O0 -Wa,-mcpu=68030,-mno-68851,-mno-68881,-mno-68882,-mno-float
PROG_AVRDUDEFLAGS:=-R .flash
PROG_OBJDUMPFLAGS:=-s -d -M att

COMMON_CFLAGS=-std=gnu11 -Wall -Wextra -Wno-unused-function -O2 -flto -ffreestanding -fwhole-program -fuse-linker-plugin -mrelax -DDEBUG
COMMON_LST_CFLAGS=-c -O0 -g -w -Wa,-adhln -o /dev/null
COMMON_OBJCOPYFLAGS:=-R .eeprom
COMMON_OBJDUMPFLAGS:=-s -d


COMMON_SRCDIR:=common
EMU_SRCDIR:=emulator
MMU_SRCDIR:=mmu
PROG_SRCDIR:=prog


EMU_EXE:=$(EMU_SRCDIR)/$(EMU_BASENAME).out
EMU_BIN:=$(EMU_SRCDIR)/$(EMU_BASENAME).bin
EMU_HEX=$(EMU_SRCDIR)/$(EMU_BASENAME).hex
EMU_MAP:=$(EMU_SRCDIR)/$(EMU_BASENAME).map
EMU_DUMP:=$(EMU_SRCDIR)/$(EMU_BASENAME).dump
EMU_PCH:=$(EMU_SRCDIR)/all.h
EMU_PCH_EXT:=$(EMU_PCH).gch

MMU_EXE:=$(MMU_SRCDIR)/$(MMU_BASENAME).out
MMU_BIN:=$(MMU_SRCDIR)/$(MMU_BASENAME).bin
MMU_HEX=$(MMU_SRCDIR)/$(MMU_BASENAME).hex
MMU_MAP:=$(MMU_SRCDIR)/$(MMU_BASENAME).map
MMU_DUMP:=$(MMU_SRCDIR)/$(MMU_BASENAME).dump
MMU_PCH:=$(MMU_SRCDIR)/all.h
MMU_PCH_EXT:=$(MMU_PCH).gch

PROG1_EXE=$(PROG_SRCDIR)/$(PROG1_BASENAME).out
PROG1_BIN=$(PROG_SRCDIR)/$(PROG1_BASENAME).bin
PROG1_HEX=$(PROG_SRCDIR)/$(PROG1_BASENAME).hex
PROG1_MAP=$(PROG_SRCDIR)/$(PROG1_BASENAME).map
PROG1_DUMP=$(PROG_SRCDIR)/$(PROG1_BASENAME).dump

PROG2_EXE=$(PROG_SRCDIR)/$(PROG2_BASENAME).out
PROG2_BIN=$(PROG_SRCDIR)/$(PROG2_BASENAME).bin
PROG2_HEX=$(PROG_SRCDIR)/$(PROG2_BASENAME).hex
PROG2_MAP=$(PROG_SRCDIR)/$(PROG2_BASENAME).map
PROG2_DUMP=$(PROG_SRCDIR)/$(PROG2_BASENAME).dump


# evaluated when used
COMMON_HEADERS=$(wildcard $(COMMON_SRCDIR)/*.h)
EMU_HEADERS=$(wildcard $(EMU_SRCDIR)/*.h)
MMU_HEADERS=$(wildcard $(MMU_SRCDIR)/*.h)
PROG1_SOURCE=$(wildcard $(PROG_SRCDIR)/*.s)
PROG2_SOURCE=$(wildcard $(PROG_SRCDIR)/*.c)
EMU_OBJECTS=$(patsubst %.c,%.o,$(wildcard $(EMU_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(EMU_SRCDIR)/*.s))
MMU_OBJECTS=$(patsubst %.c,%.o,$(wildcard $(MMU_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(MMU_SRCDIR)/*.s))
EMU_CLEAN=$(EMU_EXE) $(EMU_BIN) $(EMU_HEX) $(EMU_MAP) $(EMU_DUMP) $(wildcard $(EMU_SRCDIR)/*.o) $(wildcard $(EMU_SRCDIR)/*.lst) $(wildcard $(EMU_SRCDIR)/*.gch)
MMU_CLEAN=$(MMU_EXE) $(MMU_BIN) $(MMU_HEX) $(MMU_MAP) $(MMU_DUMP) $(wildcard $(MMU_SRCDIR)/*.o) $(wildcard $(EMU_SRCDIR)/*.lst) $(wildcard $(EMU_SRCDIR)/*.gch)
PROG_CLEAN=$(PROG1_EXE) $(PROG1_BIN) $(PROG1_HEX) $(PROG1_MAP) $(PROG1_DUMP) $(PROG2_EXE) $(PROG2_BIN) $(PROG2_HEX) $(PROG2_MAP) $(PROG2_DUMP)


.PHONY: all emulator mmu prog prog1 prog2 pch lst map dump loademu loadmmu loadprog1 loadprog2 clean

# default rule
all: emulator mmu prog lst map dump

emulator: $(EMU_EXE) $(EMU_BIN) $(EMU_HEX)
mmu: $(MMU_EXE) $(MMU_BIN) $(MMU_HEX)

prog: prog1 prog2

prog1: $(PROG1_EXE) $(PROG1_BIN) $(PROG1_HEX)
prog2: $(PROG2_EXE) $(PROG2_BIN) $(PROG2_HEX)

pch: $(EMU_PCH_EXT) $(MMU_PCH_EXT)

lst: $(patsubst %.c,%.lst,$(wildcard $(EMU_SRCDIR)/*.c)) $(patsubst %.c,%.lst,$(wildcard $(MMU_SRCDIR)/*.c))

map: $(EMU_MAP) $(MMU_MAP) $(PROG1_MAP) $(PROG2_MAP)

dump: $(EMU_DUMP) $(MMU_DUMP) $(PROG1_DUMP) $(PROG2_DUMP)

loademu: $(EMU_HEX)
	-sudo avrdude $(EMU_AVRDUDEFLAGS) -U flash:w:$^
loadmmu: $(MMU_HEX)
	-sudo avrdude $(MMU_AVRDUDEFLAGS) -U flash:w:$^

loadprog1: $(PROG1_HEX)
	-sudo avrdude $(PROG_AVRDUDEFLAGS) -U eeprom:w:$^
loadprog2: $(PROG2_HEX)
	-sudo avrdude $(PROG_AVRDUDEFLAGS) -U eeprom:w:$^

clean:
	rm -f $(EMU_CLEAN)
	rm -f $(MMU_CLEAN)
	rm -f $(PROG_CLEAN)


$(EMU_SRCDIR)/%.o: $(EMU_SRCDIR)/%.c $(EMU_PCH_EXT) $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -c -o $@ $<

$(EMU_SRCDIR)/%.o: $(EMU_SRCDIR)/%.s $(EMU_PCH_EXT) $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -c -o $@ $<

$(EMU_SRCDIR)/%.lst: $(EMU_SRCDIR)/%.c $(EMU_PCH_EXT) $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) $(COMMON_LST_CFLAGS) $< >$@


$(MMU_SRCDIR)/%.o: $(MMU_SRCDIR)/%.c $(MMU_PCH_EXT) $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -c -o $@ $<

$(MMU_SRCDIR)/%.o: $(MMU_SRCDIR)/%.s $(MMU_PCH_EXT) $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -c -o $@ $<

$(MMU_SRCDIR)/%.lst: $(MMU_SRCDIR)/%.c $(MMU_PCH_EXT) $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) $(COMMON_LST_CFLAGS) $< >$@


$(EMU_PCH_EXT): $(EMU_PCH) $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) $(EMU_PCH)

$(EMU_EXE): $(EMU_OBJECTS)
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -o $@ $^

$(EMU_BIN): $(EMU_EXE)
	avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(EMU_HEX): $(EMU_EXE)
	avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(EMU_MAP): $(EMU_EXE)
	avr-nm -ns $^ >$@

$(EMU_DUMP): $(EMU_EXE)
	avr-objdump $(COMMON_OBJDUMPFLAGS) $^ >$@


$(MMU_PCH_EXT): $(MMU_PCH) $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) $(MMU_PCH)

$(MMU_EXE): $(MMU_OBJECTS)
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -o $@ $^

$(MMU_BIN): $(MMU_EXE)
	avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(MMU_HEX): $(MMU_EXE)
	avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(MMU_MAP): $(MMU_EXE)
	avr-nm -ns $^ >$@

$(MMU_DUMP): $(MMU_EXE)
	avr-objdump $(COMMON_OBJDUMPFLAGS) $^ >$@


$(PROG1_EXE): $(PROG1_SOURCE)
	m68k-elf-as $(PROG1_ASFLAGS) -o $@ $<

$(PROG1_BIN): $(PROG1_EXE)
	m68k-elf-objcopy -O binary $(PROG_OBJCOPYFLAGS) $^ $@

$(PROG1_HEX): $(PROG1_EXE)
	m68k-elf-objcopy -O ihex $(PROG_OBJCOPYFLAGS) $^ $@

$(PROG1_MAP): $(PROG1_EXE)
	m68k-elf-nm -ns $^ >$@

$(PROG1_DUMP): $(PROG1_EXE)
	m68k-elf-objdump $(PROG_OBJDUMPFLAGS) $^ >$@


$(PROG2_EXE): $(PROG2_SOURCE)
	m68k-elf-gcc $(PROG2_CFLAGS) -o $@ $<

$(PROG2_BIN): $(PROG2_EXE)
	m68k-elf-objcopy -O binary $(PROG_OBJCOPYFLAGS) $^ $@

$(PROG2_HEX): $(PROG2_EXE)
	m68k-elf-objcopy -O ihex $(PROG_OBJCOPYFLAGS) $^ $@

$(PROG2_MAP): $(PROG2_EXE)
	m68k-elf-nm -ns $^ >$@

$(PROG2_DUMP): $(PROG2_EXE)
	m68k-elf-objdump $(PROG_OBJDUMPFLAGS) $^ >$@
