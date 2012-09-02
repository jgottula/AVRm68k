#include <stdint.h>
#include <stdnoreturn.h>
#include "debug.h"
#include "dram.h"
#include "intr.h"
#include "io.h"

noreturn void main(void)
{
	intrInit();
	
	for ( ;; );
}
