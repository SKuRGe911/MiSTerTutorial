#pragma once
//////////////////////////////////////////////////////////////////////////
BOOL ProcBitmap(CString outnamepath, CString innamepath);
BOOL ProcVerilog(CString outnamepath, CString innamepath, BOOL binary, BOOL verilog,BOOL wordsize,BOOL endian);
BOOL ProcSineWave(CString outnamepath, DWORD maxvalue,DWORD midvalue);
BOOL ProcCosineWave(CString outnamepath, DWORD maxvalue, DWORD midvalue);
BOOL ProcLookupTable(CString outnamepath, signed short int totalvalues, signed short int startvalue, signed short int endvalue, signed short int incrementvalue);
BOOL ProcTextString(CString outnamepath, CString& str,DWORD& characters_per_line);
//////////////////////////////////////////////////////////////////////////
