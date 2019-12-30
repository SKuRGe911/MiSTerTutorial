#include "GRNMPCH.H"
#include "../PROCESSING/PROCESSING.H"
//////////////////////////////////////////////////////////////////////////
BOOL ProcLookupTable(CString outnamepath, signed short int totalvalues, signed short int startvalue, signed short int endvalue, signed short int incrementvalue) {

	//////////////////////////////////////////////////////////////////////////
	CFile outfile;
	//////////////////////////////////////////////////////////////////////////
	if (outfile.Create(outnamepath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL) == FALSE) {
		MessageBoxW(NULL, L"COULD NOT OPEN OUTPUT FILE", L"GENERATE ERROR!", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	CStringA text;
	//////////////////////////////////////////////////////////////////////////
	DWORD count = 0;
	BYTE item = startvalue;
	text.Format("\tDB\t");
	for (DWORD j = 0; j < totalvalues; j++) {
		text.Format("%s%c$%02X", text, count == 0 ? '\t' : ',', item);
		count++;
		item = item + incrementvalue;
		if (count >= 16) {
			count = 0;
			text.Format("%s\n", text);
			outfile.Write(text.GetBuffer(text.GetLength()), text.GetLength());
			text.ReleaseBuffer();
			text.Format("\tDB\t");
		}
	}
	if (count < 16) {
		text.Format("%s\n", text);
		outfile.Write(text.GetBuffer(text.GetLength()), text.GetLength());
		text.ReleaseBuffer();
	}
	//////////////////////////////////////////////////////////////////////////
	outfile.Close();
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////