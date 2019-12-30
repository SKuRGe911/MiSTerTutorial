#include "GRNMPCH.H"
#include "HTLSINGLE.H"
//////////////////////////////////////////////////////////////////////////
CWindowMarker::CWindowMarker():
m_hWnd(NULL),
m_cName(){
}
//////////////////////////////////////////////////////////////////////////
CWindowMarker::~CWindowMarker(){
	Clear();
}
//////////////////////////////////////////////////////////////////////////
void CWindowMarker::Mark(HWND hWnd_,LPCTSTR pName_){
	Clear();
	::SetProp(hWnd_,pName_,(HANDLE)TRUE);
	m_hWnd=hWnd_;
	m_cName=pName_;
}
//////////////////////////////////////////////////////////////////////////
void CWindowMarker::Clear(){
	if(m_hWnd){
		::RemoveProp(m_hWnd,m_cName);
		m_hWnd=NULL;
		m_cName.Empty();
	}
}
//////////////////////////////////////////////////////////////////////////
struct SFindProcParms{
	HWND hWnd;
	CString cName;
};
//////////////////////////////////////////////////////////////////////////
HWND CWindowMarker::Find(HWND hWndTop_,LPCTSTR pName_){
	SFindProcParms sParms;
	sParms.hWnd=NULL;
	sParms.cName=pName_;
	::EnumWindows(FindProc,reinterpret_cast<LPARAM>(&sParms));
	return sParms.hWnd;
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CWindowMarker::FindProc(HWND hWnd_,LPARAM lParam_){
	SFindProcParms *pParms=reinterpret_cast<SFindProcParms *>(lParam_);
	if(::GetProp(hWnd_,pParms->cName)){
		pParms->hWnd=hWnd_;
		return FALSE;
	} else{
		return TRUE;
	}
}
//////////////////////////////////////////////////////////////////////////
