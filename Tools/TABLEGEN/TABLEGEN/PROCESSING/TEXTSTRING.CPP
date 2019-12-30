#include "GRNMPCH.H"
#include "PROCESSING.H"
//////////////////////////////////////////////////////////////////////////
const char aciilookup[128] = {
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,
	0x20,		// SPACE
	0x21,		// !
	0x22,		// "
	0x23,		// #
	0x24,		// $
	0x25,		// %
	0x26,		// &
	0x27,		// '
	0x28,		// (
	0x29,		// )
	0x2A,		// *
	0x2B,		// +
	0x2C,		// ,
	0x2D,		// -
	0x2E,		// .
	0x2F,		// /
	0x30,		// 0
	0x31,		// 1
	0x32,		// 2
	0x33,		// 3
	0x34,		// 4
	0x35,		// 5
	0x36,		// 6
	0x37,		// 7
	0x38,		// 8
	0x39,		// 9
	0x3A,		// :
	0x3B,		// ;
	0x3C,		// <
	0x3D,		// =
	0x3E,		// >
	0x3F,		// ?
	0x20,		// @
	0x41,		// A
	0x42,		// B
	0x43,		// C
	0x44,		// D
	0x45,		// E
	0x46,		// F
	0x47,		// G
	0x48,		// H
	0x49,		// I
	0x4A,		// J
	0x4B,		// K
	0x4C,		// L
	0x4D,		// M
	0x4E,		// N
	0x4F,		// O
	0x50,		// P
	0x51,		// Q
	0x52,		// R
	0x53,		// S
	0x54,		// T
	0x55,		// U
	0x56,		// V
	0x57,		// W
	0x58,		// X
	0x59,		// Y
	0x5A,		// Z
	0x20,		// [
	0x20,		/* \ */
	0x20,		// [
	0x20,		// ^
	0x20,		// _
	0x27,		// '
	0x01,		// a
	0x02,		// b
	0x03,		// c
	0x04,		// d
	0x05,		// e
	0x06,		// f
	0x07,		// g
	0x08,		// h
	0x09,		// i
	0x0A,		// j
	0x0B,		// k
	0x0C,		// l
	0x0D,		// m
	0x0E,		// n
	0x0F,		// o
	0x10,		// p
	0x11,		// q
	0x12,		// r
	0x13,		// s
	0x14,		// t
	0x15,		// u
	0x16,		// v
	0x17,		// w
	0x18,		// x
	0x19,		// y
	0x1A,		// z
	0x20,		// {
	0x20,		// |
	0x20,		// }
	0x20,		// ~
	0x20		// DEL
};
//////////////////////////////////////////////////////////////////////////
BOOL ProcTextString(CString outnamepath, CString& str, DWORD& characters_per_line) {

	//////////////////////////////////////////////////////////////////////////
	CFile outfile;
	//////////////////////////////////////////////////////////////////////////
	if (outfile.Create(outnamepath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL) == FALSE) {
		MessageBoxW(NULL, L"COULD NOT OPEN OUTPUT FILE", L"GENERATE ERROR!", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	CStringA outputstring;
	outputstring.Format("%S", str);
	//////////////////////////////////////////////////////////////////////////
	DWORD asciilen = outputstring.GetLength();
	char *ascii = outputstring.GetBuffer(asciilen);
	//////////////////////////////////////////////////////////////////////////
	DWORD charcounter = 0;
	DWORD linecounter = 0;
	DWORD count=0;
	CStringA write_string="";
	write_string.Format("\tDB\t");
	for (DWORD j = 0; j < asciilen; j++) {
		char c = aciilookup[*ascii];
		if (charcounter == characters_per_line) {
			if (*ascii == '\n' || *ascii == '\r') {
				ascii++;
				asciilen++;
			}
			else if (*ascii == '\0') {
				goto end;
			}
			else {
				charcounter = 0;
				linecounter++;
				if (linecounter >= 24) {
					goto end;
				}
			}
		}
		else {
			charcounter++;
			if (*ascii == '\n'|| *ascii =='\r') {
				c = 0x20;
				asciilen++;
			}
			else {
				ascii++;
			}
			write_string.Format("%s%c$%02X", write_string, count == 0 ? '\t' : ',', (BYTE)c);
			count++;
			if (count >= 16) {
				count = 0;
				write_string.Format("%s\n", write_string);
				outfile.Write(write_string.GetBuffer(write_string.GetLength()), write_string.GetLength());
				write_string.ReleaseBuffer();
				write_string.Format("\tDB\t");
			}

		}

	}
	//////////////////////////////////////////////////////////////////////////

end:

	//////////////////////////////////////////////////////////////////////////
	if (count < 16) {
		write_string.Format("%s\n", write_string);
		outfile.Write(write_string.GetBuffer(write_string.GetLength()), write_string.GetLength());
		write_string.ReleaseBuffer();
	}
	//////////////////////////////////////////////////////////////////////////
	outputstring.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	outfile.Close();
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
