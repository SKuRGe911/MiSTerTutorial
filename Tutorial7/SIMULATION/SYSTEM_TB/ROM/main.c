#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
int main(void)
{	
	set_video_mode(VIDEO_MODE_0);
	set_video_mode_0_addr_a(VIDEO_MODE_0_RAM_A);
	set_video_mode_0_addr_b(VIDEO_MODE_0_RAM_B);
	swap_buffers_now();

	interupt_on = true;

	set_video_auto_swap_on();

	while (1) {
		for (uint32_t x = 0; x < 20000; x++) {

		}
		set_video_swap_ready();
		continue;
	}

	return 0;

}


