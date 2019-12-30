#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
	*((uint32_t*)GERANIUM_IO_ADDRESS + (160 * y + x)) = color;
}
//////////////////////////////////////////////////////////////////////////
void draw_line(uint32_t x, uint32_t y, uint32_t x2, uint32_t y2, uint32_t color) {
	uint32_t longest, shortest, numerator, i;
	uint32_t dx1 = (x < x2) ? 1 : -1;
	uint32_t dy1 = (y < y2) ? 1 : -1;
	uint32_t dx2, dy2;

	longest = abs(x2 - x);
	shortest = abs(y2 - y);
	if (longest < shortest) {
		longest = abs(y2 - y);
		shortest = abs(x2 - x);
		dx2 = 0;
		dy2 = dy1;
	}
	else {
		dx2 = dx1;
		dy2 = 0;
	}

	numerator = longest / 2;
	for (i = 0; i <= longest; i++) {
		put_pixel(x, y, color);
		if (numerator >= longest - shortest) {
			numerator += shortest;
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else {
			numerator += shortest;
			x += dx2;
			y += dy2;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void cpu_wait(void) {
	uint32_t z=0,x=0;
	for (uint32_t a = 0; a < 200000000; a++) {
		for (z = 0; z < 200000000; z++) {
			x = a * z;
			if (x == 0) {
				continue;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
int main(void)
{	
	int32_t i;
	uint32_t color = 0;
	interupt_on = true;

	while (1) {
		for (i = 0; i < 120; i++) {
			cpu_wait();
			draw_line(0, 0, 159, i, color++);
		}
		for (i = 159; i >= 0; i--) {
			cpu_wait();
			draw_line(0, 0, i, 119, color++);
		}
		for (i = 0; i < 160; i++) {
			cpu_wait();
			draw_line(0, 119, i, 0, color++);
		}
		for (i = 0; i < 120; i++) {
			cpu_wait();
			draw_line(0, 119, 159, i, color++);
		}
		for (i = 119; i >= 0; i--) {
			cpu_wait();
			draw_line(159, 119, 0, i, color++);
		}
		for (i = 0; i < 160; i++) {
			cpu_wait();
			draw_line(159, 119, i, 0, color++);
		}
		for (i = 159; i >= 0; i--) {
			cpu_wait();
			draw_line(159, 0, i, 119, color++);
		}
		for (i = 119; i >= 0; i--) {
			cpu_wait();
			draw_line(159, 0, 0, i, color++);
		}
	}

	return 0;

}


