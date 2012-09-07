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
PROG1_ASFLAGS:=
PROG1_AVRDUDEFLAGS:=

PROG2_BASENAME:=prog2
PROG2_CFLAGS:=

COMMON_CFLAGS=-std=gnu11 -Wall -Wextra -Wno-unused-function -O2 -flto -ffreestanding -fwhole-program -fuse-linker-plugin -mrelax -DDEBUG
COMMON_LST_CFLAGS=-c -O0 -g -w -Wa,-adhln -o /dev/null
COMMON_OBJCOPYFLAGS:=-R .eeprom

INCLUDE:=
LINK:=

COMMON_SRCDIR:=common
EMU_SRCDIR:=emulator
MMU_SRCDIR:=mmu

EMU_EXE:=$(EMU_SRCDIR)/$(EMU_BASENAME).out
EMU_BIN:=$(EMU_SRCDIR)/$(EMU_BASENAME).bin
EMU_HEX=$(EMU_SRCDIR)/$(EMU_BASENAME).hex
EMU_MAP:=$(EMU_SRCDIR)/$(EMU_BASENAME).map
EMU_DUMP:=$(EMU_SRCDIR)/$(EMU_BASENAME).dump

MMU_EXE:=$(MMU_SRCDIR)/$(MMU_BASENAME).out
MMU_BIN:=$(MMU_SRCDIR)/$(MMU_BASENAME).bin
MMU_HEX=$(MMU_SRCDIR)/$(MMU_BASENAME).hex
MMU_MAP:=$(MMU_SRCDIR)/$(MMU_BASENAME).map
MMU_DUMP:=$(MMU_SRCDIR)/$(MMU_BASENAME).dump

# TODO: PROG dirs


# evaluated when used
COMMON_HEADERS=$(wildcard $(COMMON_SRCDIR)/*.h)
EMU_HEADERS=$(wildcard $(EMU_SRCDIR)/*.h)
EMU_OBJECTS=$(patsubst %.c,%.o,$(wildcard $(EMU_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(EMU_SRCDIR)/*.s))
MMU_HEADERS=$(wildcard $(MMU_SRCDIR)/*.h)
MMU_OBJECTS=$(patsubst %.c,%.o,$(wildcard $(MMU_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(MMU_SRCDIR)/*.s))
EMU_CLEAN=$(EMU_EXE) $(EMU_BIN) $(EMU_HEX) $(EMU_MAP) $(EMU_DUMP) $(wildcard $(EMU_SRCDIR)/*.o)
MMU_CLEAN=$(MMU_EXE) $(MMU_BIN) $(MMU_HEX) $(MMU_MAP) $(MMU_DUMP) $(wildcard $(MMU_SRCDIR)/*.o)


.PHONY: all emulator mmu prog1 prog2 lst map dump loademu loadmmu loadprog1 clean

# default rule
all: emulator mmu prog1 prog2 lst map dump

emulator: $(EMU_EXE) $(EMU_BIN) $(EMU_HEX)

mmu: $(MMU_EXE) $(MMU_BIN) $(MMU_HEX)

prog1: $(PROG1_EXE) $(PROG1_BIN) $(PROG1_HEX)

prog2: $(PROG2_EXE) $(PROG2_BIN) $(PROG2_HEX)

lst: $(patsubst %.c,%.lst,$(wildcard $(EMU_SRCDIR)/*.c)) $(patsubst %.c,%.lst,$(wildcard $(MMU_SRCDIR)/*.c))

map: $(EMU_MAP) $(MMU_MAP)

dump: $(EMU_DUMP) $(MMU_DUMP)

loademu: $(EMU_HEX)
	-sudo avrdude $(EMU_AVRDUDEFLAGS) -U flash:w:$^

loadmmu: $(MMU_HEX)
	# ...

loadprog1: $(PROG1_HEX)
	# ...

clean:
	rm -f $(EMU_CLEAN)
	rm -f $(MMU_CLEAN)


$(EMU_SRCDIR)/%.o: $(EMU_SRCDIR)/%.c $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -c -o $@ $<

$(EMU_SRCDIR)/%.o: $(EMU_SRCDIR)/%.s $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -c -o $@ $<

$(EMU_SRCDIR)/%.lst: $(EMU_SRCDIR)/%.c $(EMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) $(COMMON_LST_CFLAGS) $< >$@


$(MMU_SRCDIR)/%.o: $(MMU_SRCDIR)/%.c $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -c -o $@ $<

$(MMU_SRCDIR)/%.o: $(MMU_SRCDIR)/%.s $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -c -o $@ $<

$(MMU_SRCDIR)/%.lst: $(MMU_SRCDIR)/%.c $(MMU_HEADERS) $(COMMON_HEADERS) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) $(COMMON_LST_CFLAGS) $< >$@


$(EMU_EXE): $(EMU_OBJECTS)
	avr-gcc $(COMMON_CFLAGS) $(EMU_CFLAGS) -o $@ $^

$(EMU_BIN): $(EMU_EXE)
	avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(EMU_HEX): $(EMU_EXE)
	avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(EMU_MAP): $(EMU_EXE)
	avr-nm -ns $^ >$@

$(EMU_DUMP): $(EMU_EXE)
	avr-objdump -s -d $^ >$@


$(MMU_EXE): $(MMU_OBJECTS)
	avr-gcc $(COMMON_CFLAGS) $(MMU_CFLAGS) -o $@ $^

$(MMU_BIN): $(MMU_EXE)
	avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(MMU_HEX): $(MMU_EXE)
	avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(MMU_MAP): $(MMU_EXE)
	avr-nm -ns $^ >$@

$(MMU_DUMP): $(MMU_EXE)
	avr-objdump -s -d $^ >$@


# TODO: PROG targets
