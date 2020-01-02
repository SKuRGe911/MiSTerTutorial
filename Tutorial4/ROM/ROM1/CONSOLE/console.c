#include "../config.h"
//////////////////////////////////////////////////////////////////////////
char buf[256];
uint32_t pos = 0;
//////////////////////////////////////////////////////////////////////////
void console_print_line(char* string_to_output) {
	outputstring(string_to_output, true);
}
//////////////////////////////////////////////////////////////////////////
void outputstring(char* string_to_output,bool toend) {
	uint32_t out;
	uint32_t sz = strlen(string_to_output);
	for (size_t a = 0; a <= sz; a++) {
		out = string_to_output[a];
		WRITECHAR(out);
	}
	if (toend) {
		for (size_t a = (sz % CHAR_HORIZONTAL_COUNT) + 1; a < CHAR_HORIZONTAL_COUNT; a++) {
			WRITECHAR(0);
		}
	}
	return;
}
//////////////////////////////////////////////////////////////////////////
void outputchar(char c) {
	uint32_t out = (uint32_t)c;
	WRITECHAR(out);
}
//////////////////////////////////////////////////////////////////////////
void cleartextbuffer(void) {
	WRITECHAR_CLEAR_SCREEN();
}
//////////////////////////////////////////////////////////////////////////
void clear_line(void) {
	WRITECHAR_CLEAR_LINNE();
}
//////////////////////////////////////////////////////////////////////////
char* get_bin(uint32_t pos, uint32_t num) {
	static char one[2] = { '1',0 };
	static char zero[2] = { '0',0 };
	if (((num >> pos) & 1) == 1) {
		return one;
	}
	return zero;
}
//////////////////////////////////////////////////////////////////////////
void print_bin_val(uint32_t val) {

	snprintf(buf, 256, "%s%s%s%s%s%s%s%s", (char*)get_bin(31, val), (char*)get_bin(30, val), (char*)get_bin(29, val), (char*)get_bin(28, val), (char*)get_bin(27, val), (char*)get_bin(26, val), (char*)get_bin(25, val), (char*)get_bin(24, val));
	snprintf(buf, 256, "%s%s%s%s%s%s%s%s%s", (char*)buf, (char*)get_bin(23, val), (char*)get_bin(22, val), (char*)get_bin(21, val), (char*)get_bin(20, val), (char*)get_bin(19, val), (char*)get_bin(18, val), (char*)get_bin(17, val), (char*)get_bin(16, val));
	snprintf(buf, 256, "%s%s%s%s%s%s%s%s%s", (char*)buf, (char*)get_bin(15, val), (char*)get_bin(14, val), (char*)get_bin(13, val), (char*)get_bin(12, val), (char*)get_bin(11, val), (char*)get_bin(10, val), (char*)get_bin(9, val), (char*)get_bin(8, val));
	snprintf(buf, 256, "%s%s%s%s%s%s%s%s%s", (char*)buf, (char*)get_bin(7, val), (char*)get_bin(6, val), (char*)get_bin(5, val), (char*)get_bin(4, val), (char*)get_bin(3, val), (char*)get_bin(2, val), (char*)get_bin(1, val), (char*)get_bin(0, val));
	outputstring(buf, false);

}
//////////////////////////////////////////////////////////////////////////
void print_hex_val(uint32_t val) {

	snprintf(buf, 256, "0x0%08x", ((uint16_t)val));
	outputstring(buf, false);

}
//////////////////////////////////////////////////////////////////////////
uint32_t getcharcount(char* str) {
	uint32_t count = 0;
	while (str[count] != ' ' && str[count] != 0) {
		count++;
	}
	return count;
}
//////////////////////////////////////////////////////////////////////////
