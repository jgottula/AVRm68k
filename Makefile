PROJNAME:=avrm68k
CFLAGS:=-mmcu=atmega324a -std=c11 -Wall -Wextra -Wno-unused-function -Os -flto \
	-ffreestanding -fwhole-program -fuse-linker-plugin -DF_CPU=20000000UL \
	-DDEBUG
M68K_ASFLAGS:=-mcpu=68030 -mno-68851 -mno-68881 -mno-68882 -mno-float
M68K_CFLAGS:=-S -Wa,-mcpu=68030,-mno-68851,-mno-68881,-mno-68882,-mno-float
OBJCOPYFLAGS:=-R .eeprom
M68K_OBJCOPYFLAGS:=-R .flash
AVRDUDEFLAGS:=-c usbasp -p m324a -D
SOURCES:=$(wildcard *.c)
HEADERS:=$(wildcard *.h)
TEST1_SOURCES:=programs/test1.s
TEST2_SOURCES:=programs/test2.c
INCLUDE:=
LINK:=
OUT:=$(PROJNAME).out
HEX:=$(PROJNAME).hex
BIN:=$(PROJNAME).bin
LST:=$(PROJNAME).lst
DUMP:=$(PROJNAME).dump
TEST1_OUT:=programs/test1.out
TEST1_HEX:=programs/test1.hex
TEST1_BIN:=programs/test1.bin
TEST1_DUMP:=programs/test1.dump
TEST2_ASM:=programs/test2.s

.PHONY: all avr test1 test2 program progtest clean asm

all: avr test1 test2

avr: $(OUT) $(HEX) $(BIN) $(DUMP) $(LST)
test1: $(TEST1_OUT) $(TEST1_HEX) $(TEST1_BIN) $(TEST1_DUMP)
test2: $(TEST2_ASM)


clean:
	-rm -rf *.out *.hex *.bin *.lst *.dump
program: $(HEX) $(TEST_HEX)
	sudo avrdude $(AVRDUDEFLAGS) -U flash:w:$(HEX)
progtest: $(TEST1_HEX) Makefile
	sudo avrdude $(AVRDUDEFLAGS) -U eeprom:w:$(TEST1_HEX)


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


$(TEST1_OUT): $(TEST1_SOURCES) Makefile
	m68k-elf-as $(M68K_ASFLAGS) -o $(TEST1_OUT) $(TEST1_SOURCES)

$(TEST1_HEX): $(TEST1_OUT)
	m68k-elf-objcopy -O ihex $(M68K_OBJCOPYFLAGS) $(TEST1_OUT) $@

$(TEST1_BIN): $(TEST1_OUT)
	m68k-elf-objcopy -O binary $(M68K_OBJCOPYFLAGS) $(TEST1_OUT) $@

$(TEST1_DUMP): $(TEST1_OUT)
	m68k-elf-objdump -s -d -M att $(TEST1_OUT) >$@


$(TEST2_ASM): $(TEST2_SOURCES) Makefile
	m68k-elf-gcc $(M68K_CFLAGS) -o $(TEST2_ASM) $(TEST2_SOURCES)
