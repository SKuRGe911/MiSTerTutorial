#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
int main(void)
{	
	console_on();
	outputstring("[GERANIUM 2020]",true);
	output_system_message("ESC Key Toggles Console, HELP for List of Commands");
	outputstring(" ", true);

	if (load_file_system() == false) {
		seterrorno(ERROR_GRNM_FILE_SYSTEM);
		goto endloop;
	}

	output_system_message("FILE SYSTEM LOADED");
	
	uint32_t video_image_size = sizeof(uint16_t) * 640 * 480;
	uint32_t* video_image_ptr = (uint32_t*)pvalloc(video_image_size);
	uint32_t video_output_ptr = (uint32_t)0x01000000;

	OPEN_FILE("VASH16.ROM");
	READ_FILE(video_image_ptr, video_image_size);

	output_system_message("IMAGE LOADED");

	for (uint32_t a = 0; a < video_image_size/2; a++) {
		WRITESDRAM(video_output_ptr + (a << 2), video_image_ptr[a]);
	}

	output_system_message("IMAGE DISPLAYED");
	
	interupt_on = true;

	goto mainloop;

	endloop:

	if (geterrorno() != 0) {
		
		output_system_message("-=-FATAL ERROR - END EXECUTION-=-");
		
		print_last_error();
		
		while (1) {
			if (restart_loop == true) {
				restart_loop = false;
				output_system_message("RESTARTING FROM ERROR");
				
				goto mainloop;
			}
			continue;
		}
	}
	

mainloop:

	while (1) {
		for (uint32_t x = 0; x < 20000; x++) {

		}
		continue;
	}

	return 0;

}


