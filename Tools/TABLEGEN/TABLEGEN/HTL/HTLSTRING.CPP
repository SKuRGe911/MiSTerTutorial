#include "GRNMPCH.H"
#include "HTLSTRING.H"
//////////////////////////////////////////////////////////////////////////
std::string ws2s(const std::wstring& s)
{
	int slength=(int)s.length()+1;
	int len=WideCharToMultiByte(CP_ACP,0,s.c_str(),slength,0,0,0,0)-1;
	std::string r(len,'\0');
	WideCharToMultiByte(CP_ACP,0,s.c_str(),slength,&r[0],len,0,0);
	return r;
}


std::wstring s2ws(const std::string &s)
{
	int slength=(int)s.length()+1;
	int len=MultiByteToWideChar(CP_ACP,0,s.c_str(),slength,0,0)-1;
	std::wstring r(len,'\0');
	MultiByteToWideChar(CP_ACP,0,s.c_str(),slength,&r[0],len);
	return r;
}
