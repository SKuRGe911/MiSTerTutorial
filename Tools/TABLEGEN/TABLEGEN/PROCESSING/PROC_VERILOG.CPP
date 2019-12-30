#include "GRNMPCH.H"
#include "../PROCESSING/PROCESSING.H"
//////////////////////////////////////////////////////////////////////////
BOOL ProcVerilog(CString outnamepath, CString innamepath, BOOL binary, BOOL verilog, BOOL wordsize,BOOL endian)
{

	//////////////////////////////////////////////////////////////////////////
	vector<unsigned char> buffer;
	CStringA OutputString;
	CFile outfile;
	CFile infile;
	DWORD len;
	//////////////////////////////////////////////////////////////////////////
	if (binary == TRUE) {
		//////////////////////////////////////////////////////////////////////////
		if (outfile.Create(outnamepath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL) == FALSE)
		{
			MessageBoxW(NULL, L"COULD NOT OPEN OUTPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		if (infile.Open(innamepath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL) == FALSE)
		{
			MessageBoxW(NULL, L"COULD NOT OPEN INPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
			outfile.Close();
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		len = infile.GetSize();
		buffer.resize(len);
		if (infile.Read(&buffer[0], len) == FALSE)
		{
			MessageBoxW(NULL, L"COULD NOT READ INPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
			outfile.Close();
			infile.Close();
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		DWORD w = 0;
		for (DWORD j = 0; j < len; j=j+4) {
			if (endian == TRUE) {
				w = (((DWORD)buffer[j]) << 24) | (((DWORD)buffer[j + 1]) << 16) | (((DWORD)buffer[j + 2]) << 8) | (((DWORD)buffer[j + 3]));
			}
			else {
				w = (((DWORD)buffer[j]) ) | (((DWORD)buffer[j + 1]) << 8) | (((DWORD)buffer[j + 2]) << 16) | (((DWORD)buffer[j + 3]) << 24);
			}
			outfile.Write(&w, sizeof(w));
		}
		outfile.Close();
		infile.Close();
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	if (verilog == FALSE)
	{
		MessageBoxW(NULL, L"NOT A VERILOG OUTPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	if (outfile.Create(outnamepath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL) == FALSE)
	{
		MessageBoxW(NULL, L"COULD NOT OPEN OUTPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	if (infile.Open(innamepath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL) == FALSE)
	{
		MessageBoxW(NULL, L"COULD NOT OPEN INPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
		outfile.Close();
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	len = infile.GetSize();
	buffer.resize(len);
	if (infile.Read(&buffer[0], len) == FALSE)
	{
		MessageBoxW(NULL, L"COULD NOT READ INPUT FILE", L"CONVERT ERROR!", MB_OK | MB_ICONERROR);
		outfile.Close();
		infile.Close();
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	if (wordsize == TRUE) {
		OutputString.Format("//VERILOG ROM\n\nmodule\ndata_io_rom\n(\n\tinput\t\twire\t[31:0]\taddress,\n\tinput\t\twire\t\t\t\tclock,\n\toutput\treg\t[31:0]\tq=32'd0\n);\n\nalways @(posedge clock) begin\n\tcase(address)\n");
	}
	else {
		OutputString.Format("//VERILOG ROM\n\nmodule\ndata_io_rom\n(\n\tinput\t\twire\t[31:0]\taddress,\n\tinput\t\twire\t\t\t\tclock,\n\toutput\treg\t[15:0]\tq=16'd0\n);\n\nalways @(posedge clock) begin\n\tcase(address)\n");
	}
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()), OutputString.GetLength());
	OutputString.ReleaseBuffer();
	if (wordsize == TRUE) {
		for (DWORD j = 0; j < len; j = j + 4) {
			if (endian == TRUE) {
				OutputString.Format("\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t32'h%02X%02X%02X%02X;\n\t\tend\n", j, (unsigned char)buffer[j + 3], (unsigned char)buffer[j + 2], (unsigned char)buffer[j + 1], (unsigned char)buffer[j]);
			}
			else {
				OutputString.Format("\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t32'h%02X%02X%02X%02X;\n\t\tend\n", j, (unsigned char)buffer[j], (unsigned char)buffer[j + 1], (unsigned char)buffer[j + 2], (unsigned char)buffer[j + 3]);
			}
			outfile.Write(OutputString.GetBuffer(OutputString.GetLength()), OutputString.GetLength());
			OutputString.ReleaseBuffer();
		}
	}
	else {
		for (DWORD j = 0; j < len; j = j + 4) {
			if (endian == TRUE) {
				OutputString.Format("\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t16'h%02X%02X;\n\t\tend\n", j, (unsigned char)buffer[j + 1], (unsigned char)buffer[j + 0]);
				OutputString.Format("%s\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t16'h%02X%02X;\n\t\tend\n",OutputString, j+2, (unsigned char)buffer[j + 3], (unsigned char)buffer[j + 2]);
			}
			else {
				OutputString.Format("\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t16'h%02X%02X;\n\t\tend\n", j, (unsigned char)buffer[j + 0], (unsigned char)buffer[j + 1]);
				OutputString.Format("%s\t\t32'h%08X: begin\n\t\t\t\tq\t\t\t<=\t\t\t16'h%02X%02X;\n\t\tend\n",OutputString, j+2, (unsigned char)buffer[j + 2], (unsigned char)buffer[j + 3]);
			}
			outfile.Write(OutputString.GetBuffer(OutputString.GetLength()), OutputString.GetLength());
			OutputString.ReleaseBuffer();
		}
	}
	if (wordsize == TRUE) {
		OutputString.Format("\t\tdefault: begin\n\t\t\t\tq\t\t\t<=\t\t\t32'h00000000;\n\t\tend\n");
	}
	else {
		OutputString.Format("\t\tdefault: begin\n\t\t\t\tq\t\t\t<=\t\t\t16'h0000;\n\t\tend\n");
	}
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()), OutputString.GetLength());
	OutputString.ReleaseBuffer();
	OutputString.Format("\tendcase\nend\n\nendmodule\n\0");
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()), OutputString.GetLength());
	OutputString.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	outfile.Close();
	infile.Close();
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
