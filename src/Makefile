MCU_BASENAME:=mcu
MCU_MCU:=atmega1284p
MCU_FREQ_KHZ:=20000
MCU_CFLAGS:=-mmcu=$(MCU_MCU) -DF_CPU=$(MCU_FREQ_KHZ)000UL
MCU_AVRDUDEFLAGS:=-c usbasp -p m1284p -B 460800

DMA_BASENAME:=dma
DMA_MCU:=atmega324a
DMA_FREQ_KHZ:=20000
DMA_CFLAGS:=-mmcu=$(DMA_MCU) -DF_CPU=$(DMA_FREQ_KHZ)000UL
DMA_AVRDUDEFLAGS:=-c usbasp -p m324a -B 460800

PROG1_BASENAME:=prog1
PROG1_ASFLAGS:=
PROG1_AVRDUDEFLAGS:=

PROG2_BASENAME:=prog2
PROG2_CFLAGS:=

COMMON_CFLAGS=-std=gnu11 -Wall -Wextra -Wno-unused-function -O2 -flto -ffreestanding -fwhole-program -fuse-linker-plugin -mrelax -DDEBUG
COMMON_OBJCOPYFLAGS:=-R .eeprom

INCLUDE:=
LINK:=

MCU_SRCDIR:=mcu
DMA_SRCDIR:=dma

MCU_EXE:=$(MCU_SRCDIR)/$(MCU_BASENAME).out
MCU_BIN:=$(MCU_SRCDIR)/$(MCU_BASENAME).bin
MCU_HEX=$(MCU_SRCDIR)/$(MCU_BASENAME).hex
MCU_MAP:=$(MCU_SRCDIR)/$(MCU_BASENAME).map
MCU_DUMP:=$(MCU_SRCDIR)/$(MCU_BASENAME).dump

DMA_EXE:=$(DMA_SRCDIR)/$(DMA_BASENAME).out
DMA_BIN:=$(DMA_SRCDIR)/$(DMA_BASENAME).bin
DMA_HEX=$(DMA_SRCDIR)/$(DMA_BASENAME).hex
DMA_MAP:=$(DMA_SRCDIR)/$(DMA_BASENAME).map
DMA_DUMP:=$(DMA_SRCDIR)/$(DMA_BASENAME).dump

# TODO: PROG dirs


.PHONY: all mcu dma prog1 prog2 lst map dump loadmcu loaddma loadprog1 clean

# default rule
all: mcu dma prog1 prog2 lst map dump

mcu: $(MCU_EXE) $(MCU_BIN) $(MCU_HEX)

dma: $(DMA_EXE) $(DMA_BIN) $(DMA_HEX)

prog1: $(PROG1_EXE) $(PROG1_BIN) $(PROG1_HEX)

prog2: $(PROG2_EXE) $(PROG2_BIN) $(PROG2_HEX)

lst: $(patsubst %.c,%.lst,$(wildcard $(MCU_SRCDIR)/*.c)) $(patsubst %.c,%.lst,$(wildcard $(DMA_SRCDIR)/*.c))

map: $(MCU_MAP) $(DMA_MAP)

dump: $(MCU_DUMP) $(DMA_DUMP)

loadmcu: $(MCU_HEX)
	-sudo avrdude $(MCU_AVRDUDEFLAGS) -U flash:w:$^

loaddma: $(DMA_HEX)
	# ...

loadprog1: $(PROG1_HEX)
	# ...

clean:
	-rm -rfv $(MCU_SRCDIR)/{*.o,*.lst,$(MCU_BASENAME).{out,bin,hex,map,dump}}
	-rm -rfv $(DMA_SRCDIR)/{*.o,*.lst,$(DMA_BASENAME).{out,bin,hex,map,dump}}


$(MCU_SRCDIR)/%.o: $(MCU_SRCDIR)/%.c $(wildcard $(MCU_SRCDIR)/*.h) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MCU_CFLAGS) -c -o $@ $<

$(MCU_SRCDIR)/%.o: $(MCU_SRCDIR)/%.s $(wildcard $(MCU_SRCDIR)/*.h) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MCU_CFLAGS) -c -o $@ $<

$(MCU_SRCDIR)/%.lst: $(MCU_SRCDIR)/%.c $(wildcard $(MCU_SRCDIR)/*.h) Makefile
	avr-gcc $(COMMON_CFLAGS) $(MCU_CFLAGS) -c -O0 -g -w -Wa,-adhln -o /dev/null $< >$@

$(MCU_EXE): $(patsubst %.c,%.o,$(wildcard $(MCU_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(MCU_SRCDIR)/*.s))
	avr-gcc $(COMMON_CFLAGS) $(MCU_CFLAGS) -o $@ $^

$(MCU_BIN): $(MCU_EXE)
	avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(MCU_HEX): $(MCU_EXE)
	avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(MCU_MAP): $(MCU_EXE)
	avr-nm -ns $^ >$@

$(MCU_DUMP): $(MCU_EXE)
	avr-objdump -s -d $^ >$@


# TODO: DMA targets
$(DMA_EXE): $(patsubst %.c,%.o,$(wildcard $(DMA_SRCDIR)/*.c)) $(patsubst %.s,%.o,$(wildcard $(DMA_SRCDIR)/*.s))
	#avr-gcc $(COMMON_CFLAGS) $(DMA_CFLAGS) -o $@ $^

$(DMA_BIN): $(DMA_EXE)
	#avr-objcopy -O binary $(COMMON_OBJCOPYFLAGS) $^ $@

$(DMA_HEX): $(DMA_EXE)
	#avr-objcopy -O ihex $(COMMON_OBJCOPYFLAGS) $^ $@

$(DMA_MAP): $(DMA_EXE)
	#avr-nm -ns $^ >$@

$(DMA_DUMP): $(DMA_EXE)
	#avr-objdump -s -d $^ >$@


# TODO: PROG targets
