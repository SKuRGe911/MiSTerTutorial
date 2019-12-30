#pragma once
#ifndef __ASSETSFILEDIALOG_H__
#define __ASSETSFILEDIALOG_H__
//////////////////////////////////////////////////////////////////////////
#include "../../RESOURCEDEF.H"
#include "../../HTL/HTLFILE.H"
//////////////////////////////////////////////////////////////////////////
#define LOADFILE_LIST_FILE_GUID { 0xaaa142ae,0xadae,0x4845,{0x85,0xef,0x9a,0x5c,0x9d,0x86,0x6a,0xbd} }
//////////////////////////////////////////////////////////////////////////
class CLoadListDialogsHandler:
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CLoadListDialogsHandler>,
public IFileDialogEvents,
public IFileDialogControlEvents
{

public:

	//////////////////////////////////////////////////////////////////////////
	CLoadListDialogsHandler(VOID);
	~CLoadListDialogsHandler(VOID);
	//////////////////////////////////////////////////////////////////////////
	BEGIN_COM_MAP(CLoadListDialogsHandler)
		COM_INTERFACE_ENTRY(IFileDialogEvents)
		COM_INTERFACE_ENTRY(IFileDialogControlEvents)
	END_COM_MAP()
	//////////////////////////////////////////////////////////////////////////
	// IFileDialogEvents
	//////////////////////////////////////////////////////////////////////////
	STDMETHODIMP OnFileOk(IFileDialog* pfd);
	STDMETHODIMP OnFolderChanging(IFileDialog* pfd,IShellItem* psiFolder);
	STDMETHODIMP OnFolderChange(IFileDialog* pfd);
	STDMETHODIMP OnSelectionChange(IFileDialog* pfd);
	STDMETHODIMP OnShareViolation(IFileDialog* pfd,IShellItem* psi,FDE_SHAREVIOLATION_RESPONSE* pResponse);
	STDMETHODIMP OnTypeChange(IFileDialog* pfd);
	STDMETHODIMP OnOverwrite(IFileDialog* pfd,IShellItem* psi,FDE_OVERWRITE_RESPONSE* pResponse);
	//////////////////////////////////////////////////////////////////////////
	// IFileDialogControlEvents
	//////////////////////////////////////////////////////////////////////////
	STDMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc,DWORD dwIDCtl,DWORD dwIDItem);
	STDMETHODIMP OnButtonClicked(IFileDialogCustomize* pfdc,DWORD dwIDCtl);
	STDMETHODIMP OnCheckButtonToggled(IFileDialogCustomize* pfdc,DWORD dwIDCtl,BOOL bChecked);
	STDMETHODIMP OnControlActivating(IFileDialogCustomize* pfdc,DWORD dwIDCtl);
	//////////////////////////////////////////////////////////////////////////

};
//////////////////////////////////////////////////////////////////////////
BOOL LoadFileListWithIFileOpenDlgEx(CString& filenamepath);
//////////////////////////////////////////////////////////////////////////















//////////////////////////////////////////////////////////////////////////
#endif//__ASSETSFILEDIALOG_H__