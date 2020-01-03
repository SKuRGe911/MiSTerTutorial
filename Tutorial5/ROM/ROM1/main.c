#include "config.h"
//////////////////////////////////////////////////////////////////////////
#ifndef SHIFT_COUNTER_BITS
#error SHIFT_COUNTER_BITS must be defined as 4 (for simulation) or 18 (for hardware bitstreams)!
#endif
//////////////////////////////////////////////////////////////////////////
int main(void)
{	
	outputstring("[GERANIUM 2020]",true);
	outputstring("ESC Key Toggles Console, HELP for List of Commands", true);
	toggle_console();

	if (load_file_system() == false) {
		seterrorno(ERROR_GRNM_FILE_SYSTEM);
		goto endloop;
	}

	outputstring("FILE SYSTEM LOADED", true);
	
	uint32_t video_image_size = sizeof(uint32_t) * 160 * 120;
	uint32_t* video_image_ptr = (uint32_t*)malloc(video_image_size);
	OPEN_FILE("IMAGE1.ROM");
	READ_FILE(video_image_ptr, video_image_size);

	outputstring("IMAGE LOADED", true);

	for (uint32_t a = 0; a < 160 * 120; a++) {
		WRITEOTHER(a, video_image_ptr[a]);
	}

	outputstring("IMAGE DISPLAYED", true);
	
	interupt_on = true;

	goto mainloop;

	endloop:

	if (geterrorno() != 0) {
		
		outputstring("-=-FATAL ERROR - END EXECUTION-=-", true);
		
		print_last_error();
		
		while (1) {
			if (restart_loop == true) {
				restart_loop = false;
				outputstring("RESTARTING FROM ERROR", true);
				
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


