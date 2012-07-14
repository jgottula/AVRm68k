PROJNAME:=prog
CFLAGS:=-Wa,-mcpu=68030,-mno-68851,-mno-68881,-mno-68882,-mno-float
ASFLAGS:=-mcpu=68030 -mno-68851 -mno-68881 -mno-68882 -mno-float
OBJCOPYFLAGS:=-R .flash
AVRDUDEFLAGS:=-c usbasp -p m1284p -B 460800
C_SOURCES:=$(wildcard *.c)
C_HEADERS:=$(wildcard *.h)
AS_SOURCES:=$(wildcard *.s)
GLOBAL_OUT:=../out/$(PROJNAME)
PROG1_OUT:=$(GLOBAL_OUT)/prog1.out
PROG1_HEX:=$(GLOBAL_OUT)/prog1.hex
PROG1_BIN:=$(GLOBAL_OUT)/prog1.bin
PROG1_DUMP:=$(GLOBAL_OUT)/prog1.dump
PROG2_ASM:=$(GLOBAL_OUT)/prog2.s
PROG2_OUT:=$(GLOBAL_OUT)/prog2.out
PROG2_HEX:=$(GLOBAL_OUT)/prog2.hex
PROG2_BIN:=$(GLOBAL_OUT)/prog2.bin
PROG2_DUMP:=$(GLOBAL_OUT)/prop2.dump

.PHONY: all prog1 prog2 load1 load2 clean


all: prog1 prog2

prog1: $(PROG1_OUT) $(PROG1_HEX) $(PROG1_BIN) $(PROG1_DUMP)

prog2: $(PROG2_ASM)

program: $(HEX) $(TEST_HEX)
	-sudo avrdude $(AVRDUDEFLAGS) -U flash:w:$(HEX)

clean:
	-find $(GLOBAL_OUT) -type f -delete


$(PROG1_OUT): prog1.s Makefile
	m68k-elf-as $(ASFLAGS) -o $(PROG1_OUT) prog1.s

$(PROG1_HEX): $(PROG1_OUT)
	m68k-elf-objcopy -O ihex $(OBJCOPYFLAGS) $(PROG1_OUT) $@

$(PROG1_BIN): $(PROG1_OUT)
	m68k-elf-objcopy -O binary $(OBJCOPYFLAGS) $(PROG1_OUT) $@

$(PROG1_DUMP): $(PROG1_OUT)
	m68k-elf-objdump -s -d -M att $(PROG1_OUT) >$@


#$(PROG2_OUT): prog2.c Makefile
#	m68k-elf-gcc $(CFLAGS) -o $(PROG2_OUT) prog2.c
#
#$(PROG2_HEX): $(PROG2_OUT)
#	m68k-elf-objcopy -O ihex $(OBJCOPYFLAGS) $(PROG2_OUT) $@
#
#$(PROG2_BIN): $(PROG2_OUT)
#	m68k-elf-objcopy -O binary $(OBJCOPYFLAGS) $(PROG2_OUT) $@
#
#$(PROG2_DUMP): $(PROG2_OUT)
#	m68k-elf-objdump -s -d -M att $(PROG2_OUT) >$@

$(PROG2_ASM): prog2.c Makefile
	m68k-elf-gcc $(CFLAGS) -o $(PROG2_ASM) -S prog2.c
