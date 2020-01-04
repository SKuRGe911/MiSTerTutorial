#include "../config.h"
//////////////////////////////////////////////////////////////////////////
char cmd[256];
uint32_t cmdpos = 0;
char st[256];
uint32_t sz = 0;
//////////////////////////////////////////////////////////////////////////
void toggle_console(void) {
	uint32_t c = (uint32_t)READIO(CPU_IO_CONSOLE_CTRL);
	if ((c & CONSOLE_ON) == CONSOLE_ON) {
		c = c & ~CONSOLE_ON;
	}
	else {
		c = c | CONSOLE_ON;
	}
	WRITEIO(CPU_IO_CONSOLE_CTRL, c);
}
//////////////////////////////////////////////////////////////////////////
void console_on(void) {
	uint32_t c = (uint32_t)READIO(CPU_IO_CONSOLE_CTRL);
	c = c | CONSOLE_ON;
	WRITEIO(CPU_IO_CONSOLE_CTRL, c);
}
//////////////////////////////////////////////////////////////////////////
void console_off(void) {
	uint32_t c = (uint32_t)READIO(CPU_IO_CONSOLE_CTRL);
	c = c & ~CONSOLE_ON;
	WRITEIO(CPU_IO_CONSOLE_CTRL, c);
}
//////////////////////////////////////////////////////////////////////////
void cmd_add_char(char ch) {
	if (ch == 255) {
		// ESCAPE KEY
		toggle_console();
		return;
	}
	if (ch == 17) {
		// UP ARROW KEY
		return;
	}
	if (ch == 18) {
		// LEFT ARROW KEY
		return;
	}
	if (ch == 19) {
		// DOWN ARROW KEY
		return;
	}
	if (ch == 20) {
		// RIGHT ARROW KEY
		return;
	}
	if (ch == 10) {
		// ENTER KEY
		cmd[cmdpos] = 0;
		for (size_t a = (cmdpos % CHAR_HORIZONTAL_COUNT) + 1; a <= CHAR_HORIZONTAL_COUNT; a++) {
			WRITECHAR(0);
		}
		cmd_enter();
		cmdpos = 0;
		return;
	}
	outputchar(ch);
	cmd[cmdpos] = ch;
	cmdpos++;
}
//////////////////////////////////////////////////////////////////////////
void cmd_enter(void) {

	if (strncmp(cmd, "HELP", 4) == 0) {
		display_help_on_console();
		return;
	}
	else if (strncmp(cmd, "CLS", 3) == 0) {
		cleartextbuffer();
		return;
	}
	else if (strncmp(cmd, "DIR", 3) == 0) {
		dir_to_screen();
		return;
	}
	else if (strncmp(cmd, "CD ", 3) == 0) {
		char* pathname = (&cmd[3]);
		sz = strlen(pathname);
		strncpy(st, pathname, sz);
		st[sz] = 0;
		bool rc = open_dir(st);
		if (!rc) {
			outputstring("OPEN DIRECTORY ERROR!",true);
			return;
		}
		else {
			outputstring("OPEN DIRECTORY SUCCESS", true);
		}
		return;
	}else if (strncmp(cmd, "STATUS", 6) == 0) {
		uint32_t s = READIO(CPU_IO_STATUS);
		outputstring("STATUS - ",false);
		print_bin_val(s);
		outputstring(" ", true);
	}else {
		return;
	}

}
//////////////////////////////////////////////////////////////////////////

