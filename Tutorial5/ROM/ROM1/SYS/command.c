#include "../geranium.h"

#define PICSIZE (0x0EA600)

char cmd[256];
BYTE cmdpos = 0;
uint32_t image[PICSIZE/4];

void cmd_add_char(BYTE ch) {
	if (ch == 10) {
		cmd[cmdpos] = 0;
		cmd_enter();
		cmdpos = 0;
		return;
	}
	outputchar(ch);
	cmd[cmdpos] = ch;
	cmdpos++;
	if (cmdpos == 255) {
		cmdpos = 0;
	}
}

void cmd_enter(void) {
	FRESULT rc;
	UINT bytes_read;

	if (strncmp(cmd, "DIR", 3) == 0) {
		dir_to_screen();
		incline();
	}else if (strncmp(cmd, "LOAD ", 5) == 0) {
		incline();
		char* filename = (&cmd[5]);
		uint32_t sz = strlen(filename);
		char st[256];
		strncpy(st, filename, sz);
		st[sz] = 0;
		snprintf_(buf, 256, "LOADING: %s", st);
		outputstring(buf);
		incline();
		rc = pf_open(st);
		if (rc) {
			outputstring("OPEN ERROR!");
			incline();
		}
		bytes_read = PICSIZE * 4;
		rc = pf_read((BYTE*)image, bytes_read, &bytes_read);
		if (rc) {
			outputstring("READ ERROR!");
			incline();
		}
		outputstring("LOADED SUCCESS");
		incline();
	}
	else if (strncmp(cmd, "CLS", 3) == 0) {
		cleartextbuffer();
	}
	else if (strncmp(cmd, "SHOW ", 5) == 0) {
		incline();
		char* value = (&cmd[5]);
		uint32_t sz = getcharcount(value);
		char st[256];
		strncpy(st, value, sz);
		st[sz] = 0;
		uint32_t x=atol(st);
		write_video_mem(image,x);
		outputstring("SHOW DONE!");
		incline();

	}
	else if (strncmp(cmd, "SWAPOFF", 7) == 0) {
		incline();
		disable_swap_manual();
	}
	else if (strncmp(cmd, "SWAPON", 6) == 0) {
		incline();
		enable_swap_manual();
	}else if (strncmp(cmd, "SWAP", 4) == 0) {
		incline();
		swap_video_buffers();
	}else{
		incline();
	}

}