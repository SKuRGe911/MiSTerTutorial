#include "../config.h"
//////////////////////////////////////////////////////////////////////////
uint32_t ready_to_swap = 0;
uint32_t auto_swap = 0;
uint32_t current_buffer = 0;
uint32_t mode_0_buffer_a = 0x00000000;
uint32_t mode_0_buffer_b = 0x00000000;
//////////////////////////////////////////////////////////////////////////
void set_video_mode(uint32_t mode) {
	WRITEIO(CPU_IO_VIDEO_CTRL, mode&0x01F);
}
//////////////////////////////////////////////////////////////////////////
void set_video_mode_0_addr_a(uint32_t addr) {
	mode_0_buffer_a = addr;
	WRITEIO(CPU_IO_VIDEO_ADDR_A, addr);
}
//////////////////////////////////////////////////////////////////////////
void set_video_mode_0_addr_b(uint32_t addr) {
	mode_0_buffer_b = addr;
	WRITEIO(CPU_IO_VIDEO_ADDR_B, addr);
}
//////////////////////////////////////////////////////////////////////////
void swap_buffers_now(void) {
	WRITEIO(CPU_IO_VIDEO_SWAP, VIDEO_MODE_0_BUFFER_SWAP_NOW);
	current_buffer = current_buffer ? 0 : 1;
}
//////////////////////////////////////////////////////////////////////////
void set_video_swap_ready(void) {
	ready_to_swap = 1;
}
//////////////////////////////////////////////////////////////////////////
void set_video_auto_swap_on(void) {
	auto_swap = 1;
}
//////////////////////////////////////////////////////////////////////////
void set_video_auto_swap_off(void) {
	auto_swap = 0;
}
//////////////////////////////////////////////////////////////////////////
void write_video_mode_0_mem(uint32_t pos, uint32_t data) {
	if (current_buffer == 0) {
		WRITESDRAM(mode_0_buffer_b + pos, data);
	}else if(current_buffer == 1){
		WRITESDRAM(mode_0_buffer_a + pos, data);
	}
}
//////////////////////////////////////////////////////////////////////////
void black_video_mode_0_mem(void) {
	if (current_buffer == 0) {
		for (uint32_t x = 0; x < (640 * 480) / 2; x++) {
			WRITESDRAM(mode_0_buffer_b + (x << 2), 0);
		}
	}else if (current_buffer == 1) {
		for (uint32_t x = 0; x < (640 * 480) / 2; x++) {
			WRITESDRAM(mode_0_buffer_a + (x << 2), 0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void white_video_mode_0_mem(void) {
	if (current_buffer == 0) {
		for (uint32_t x = 0; x < (640 * 480) / 2; x++) {
			WRITESDRAM(mode_0_buffer_b + (x << 2) , -1);
		}
	}else if (current_buffer == 1) {
		for (uint32_t x = 0; x < (640 * 480) / 2; x++) {
			WRITESDRAM(mode_0_buffer_a + (x << 2), -1);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
