/* precompiled header */


/* standard library */
#include <stdbool.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>

/* avr platform headers */
#include <avr/cpufunc.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

/* avr utility headers */
#include <util/atomic.h>
#include <util/delay.h>
#include <util/delay_basic.h>

/* project headers */
#include "../common/io.h"
#include "bitwise.h"
#include "boot.h"
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "io.h"
#include "m68k.h"
#include "m68k_ea.h"
#include "m68k_instr.h"
#include "m68k_mem.h"
#include "m68k_mmu.h"
#include "sd.h"
#include "spi.h"
#include "sram.h"
#include "test.h"
#include "uart.h"
