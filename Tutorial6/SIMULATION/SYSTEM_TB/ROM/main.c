#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
#define GERANIUM_VIDEO_RAM_A						0x01000000
#define GERANIUM_VIDEO_RAM_B						0x01800000
//////////////////////////////////////////////////////////////////////////
int main(void)
{	

	uint32_t s = 2000;
	uint32_t* buf = (uint32_t*)malloc(sizeof(uint32_t) * s);

	for (uint32_t a = 0; a < s / 4; a++) {
		buf[a] = a;
	}

	for (uint32_t a = 0; a < s / 4; a++) {
		WRITESDRAM(GERANIUM_VIDEO_RAM_A + (a << 2), a);
		WRITESDRAM(GERANIUM_VIDEO_RAM_B + (a << 2), a);
	}

	while (1) {
		for (uint32_t x = 0; x < 20000; x++) {

		}
		continue;
	}

	return 0;

}


