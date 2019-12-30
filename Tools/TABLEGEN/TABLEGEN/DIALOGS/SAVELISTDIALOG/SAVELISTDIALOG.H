#pragma once
#ifndef __GENERALFILEDIALOG_H__
#define __GENERALFILEDIALOG_H__
//////////////////////////////////////////////////////////////////////////
#include "../../RESOURCEDEF.H"
#include "../../HTL/HTLFILE.H"
//////////////////////////////////////////////////////////////////////////
#define SAVEFILE_LIST_FILE_GUID {0x613CDDDD,0x7D53,0x41FA,{0x94,0x6E,0x2D,0x80,0xB2,0x57,0xA2,0x22}}
//////////////////////////////////////////////////////////////////////////
class CSaveListDialogsHandler:
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CSaveListDialogsHandler>,
public IFileDialogEvents,
public IFileDialogControlEvents
{

public:

	CSaveListDialogsHandler(VOID);
	~CSaveListDialogsHandler(VOID);

	BEGIN_COM_MAP(CSaveListDialogsHandler)
		COM_INTERFACE_ENTRY(IFileDialogEvents)
		COM_INTERFACE_ENTRY(IFileDialogControlEvents)
	END_COM_MAP()

	// IFileDialogEvents
	STDMETHODIMP OnFileOk(IFileDialog* pfd);
	STDMETHODIMP OnFolderChanging(IFileDialog* pfd,IShellItem* psiFolder);
	STDMETHODIMP OnFolderChange(IFileDialog* pfd);
	STDMETHODIMP OnSelectionChange(IFileDialog* pfd);
	STDMETHODIMP OnShareViolation(IFileDialog* pfd,IShellItem* psi,FDE_SHAREVIOLATION_RESPONSE* pResponse);
	STDMETHODIMP OnTypeChange(IFileDialog* pfd);
	STDMETHODIMP OnOverwrite(IFileDialog* pfd,IShellItem* psi,FDE_OVERWRITE_RESPONSE* pResponse);

	// IFileDialogControlEvents
	STDMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc,DWORD dwIDCtl,DWORD dwIDItem);
	STDMETHODIMP OnButtonClicked(IFileDialogCustomize* pfdc,DWORD dwIDCtl);
	STDMETHODIMP OnCheckButtonToggled(IFileDialogCustomize* pfdc,DWORD dwIDCtl,BOOL bChecked);
	STDMETHODIMP OnControlActivating(IFileDialogCustomize* pfdc,DWORD dwIDCtl);

};
//////////////////////////////////////////////////////////////////////////
BOOL SaveFileListWithIFileOpenDlgEx(CString& filenamepath,BOOL binary,BOOL verilog);
//////////////////////////////////////////////////////////////////////////











#endif//__GENERALFILEDIALOG_H__