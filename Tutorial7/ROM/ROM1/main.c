#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
int main(void)
{	
	outputstring("[GERANIUM 2020]",true);
	output_system_message("ESC Key Toggles Console, HELP for List of Commands");
	outputstring(" ", true);

	while (1) {
		for (uint32_t x = 0; x < 20000; x++) {

		}
		continue;
	}

	return 0;

}


