#include "../config.h"
//////////////////////////////////////////////////////////////////////////
uint32_t error_no = 0;
//////////////////////////////////////////////////////////////////////////
void seterrorno(uint32_t e) {
	error_no = e;
}
//////////////////////////////////////////////////////////////////////////
uint32_t geterrorno(void) {
	return error_no;
}
//////////////////////////////////////////////////////////////////////////
void die(FRESULT rc) {
	snprintf(buf, 256, "[Fail rc=%u]", rc);
	outputstring(buf, true);
	for (;;);
}
//////////////////////////////////////////////////////////////////////////
void print_last_error(void) {

	switch (error_no) {
	case ERROR_GRNM_NONE:
	{
		outputstring("NO ERROR", true);
		break;
	}
	case ERROR_GRNM_FILE_SYSTEM:
	{
		outputstring("FILE SYSTEM ERROR", true);
		break;
	}
	case ERROR_GRNM_FILE_IO:
	{
		outputstring("FILE IO ERROR", true);
		break;
	}
	case ERROR_GRNM_GRAPHICS_IO:
	{
		outputstring("GRAPHICS IO ERROR", true);
		break;
	}
	case ERROR_GRNM_MEMORY:
	{
		outputstring("MEMORY ERROR", true);
		break;
	}
	default:
	{
		outputstring("UNKNOWN ERROR", true);
		break;
	}
	}
}
//////////////////////////////////////////////////////////////////////////
