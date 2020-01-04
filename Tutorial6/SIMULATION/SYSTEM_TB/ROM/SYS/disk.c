#include "../config.h"
//////////////////////////////////////////////////////////////////////////
FATFS fatfs;
FRESULT rc;
uint32_t val;
DIR dir;
FILINFO fno;
//////////////////////////////////////////////////////////////////////////
char dir_string[256];
//////////////////////////////////////////////////////////////////////////
bool load_file_system(void) {

	memset(dir_string, 0, sizeof(dir_string));
	dir_string[0] = '/';

	rc = pf_mount(&fatfs);
	if (rc) {
		return false;
	}

	rc = pf_opendir(&dir, dir_string);
	if (rc) {
		return false;
	}

	return true;

}
//////////////////////////////////////////////////////////////////////////
bool open_dir(char* filedir) {

	rc = pf_opendir(&dir, filedir);
	if (rc) {
		dir_string[0] = '/';
		dir_string[1] = 0;
		return false;
	}
	
	memset(dir_string, 0, sizeof(dir_string));
	strncpy(dir_string, filedir, strlen(filedir));

	return true;

}
//////////////////////////////////////////////////////////////////////////
void dir_to_screen(void) {

	rc = pf_opendir(&dir, dir_string);
	if (rc) {
		return;
	}
	for (uint32_t i = 0; i < 60; i++) {
		rc = pf_readdir(&dir, &fno);
		if (rc != FR_OK || fno.fname[0] == 0) {
			return;
		}
		if (fno.fattrib & AM_DIR) {
			snprintf(buf, 256, "<DIR> %s", fno.fname);
			outputstring(buf, true);
		}
		else {
			snprintf(buf, 256, "      %s", fno.fname);
			outputstring(buf, true);
		}
	}
	return;
}
//////////////////////////////////////////////////////////////////////////

