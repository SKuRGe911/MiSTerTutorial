#pragma once
#ifndef __OUTPUTPATHDIALOG_H__
#define __OUTPUTPATHDIALOG_H__
//////////////////////////////////////////////////////////////////////////
#include "../../RESOURCEDEF.H"
#include "../../HTL/HTLFILE.H"
//////////////////////////////////////////////////////////////////////////
#define SELECT_OUTPUT_PATH_GUID { 0xddd142aa,0xdda1,0x4845,{0x85,0xef,0x9a,0x5c,0x9d,0x86,0x6a,0xbd} }
//////////////////////////////////////////////////////////////////////////
class CSelectOutputPathDialogsHandler:
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CSelectOutputPathDialogsHandler>,
public IFileDialogEvents,
public IFileDialogControlEvents
{

public:

	//////////////////////////////////////////////////////////////////////////
	CSelectOutputPathDialogsHandler(VOID);
	~CSelectOutputPathDialogsHandler(VOID);
	//////////////////////////////////////////////////////////////////////////
	BEGIN_COM_MAP(CSelectOutputPathDialogsHandler)
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
BOOL SelectOutputPathWithIFileOpenDialogEx(CString& OutputPath);
//////////////////////////////////////////////////////////////////////////
#endif//__OUTPUTPATHDIALOG_H__