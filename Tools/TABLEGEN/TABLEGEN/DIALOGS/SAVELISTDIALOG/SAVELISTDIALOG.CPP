#include "GRNMPCH.H"
#include "SAVELISTDIALOG.H"
//////////////////////////////////////////////////////////////////////////
CSaveListDialogsHandler::CSaveListDialogsHandler(VOID){

}
//////////////////////////////////////////////////////////////////////////
CSaveListDialogsHandler::~CSaveListDialogsHandler(VOID){

}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnFileOk(IFileDialog* pfd){
	return S_OK;    // allow the dialog to close
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnFolderChanging(IFileDialog* pfd,IShellItem* psiFolder){
	return S_OK;    // allow the change
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnFolderChange(IFileDialog* pfd){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnSelectionChange(IFileDialog* pfd){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnShareViolation(IFileDialog* pfd,IShellItem* psi,FDE_SHAREVIOLATION_RESPONSE* pResponse){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnTypeChange(IFileDialog* pfd){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnOverwrite(IFileDialog* pfd,IShellItem* psi,FDE_OVERWRITE_RESPONSE* pResponse){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// IFileDialogControlEvents methods
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnItemSelected(IFileDialogCustomize* pfdc,DWORD dwIDCtl,DWORD dwIDItem){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnButtonClicked(IFileDialogCustomize* pfdc,DWORD dwIDCtl){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnCheckButtonToggled(IFileDialogCustomize* pfdc,DWORD dwIDCtl,BOOL bChecked){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSaveListDialogsHandler::OnControlActivating(IFileDialogCustomize* pfdc,DWORD dwIDCtl){
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
BOOL SaveFileListWithIFileOpenDlgEx(CString& filenamepath,BOOL binary, BOOL verilog){

	HRESULT hr;
	CComPtr<IFileOpenDialog> pDlg;
	vector<CString> vecsFilterParts;
	vector<COMDLG_FILTERSPEC> vecFilters;
	DWORD dwFlags=0;
	//////////////////////////////////////////////////////////////////////////
	CString strTitle;
	CString strOKButton;
	CString strDefaultExtention;
	CString strFileNameLable;
	//////////////////////////////////////////////////////////////////////////
	if(binary==FALSE)
	{
		if (verilog == TRUE) {
			strTitle.LoadString(IDS_SAVE_LIST_FILE_VERILOG);
			strDefaultExtention.LoadString(IDS_SAVE_LIST_FILE_DEFAULT_VERILOG);
		}
		else {
			strTitle.LoadString(IDS_SAVE_LIST_FILE_ASM);
			strDefaultExtention.LoadString(IDS_SAVE_LIST_FILE_DEFAULT_ASM);
		}
	}else{
		strTitle.LoadString(IDS_SAVE_LIST_FILE_BIN);
		strDefaultExtention.LoadString(IDS_SAVE_LIST_FILE_DEFAULT_BIN);
	}
	strOKButton.LoadString(IDS_SAVE_LIST_FILE_OK_BUTTON);
	strFileNameLable.LoadString(IDS_SAVE_LIST_FILE_FILENAME_LABLE);
	//////////////////////////////////////////////////////////////////////////
	hr=pDlg.CoCreateInstance(__uuidof(FileOpenDialog));

	if(FAILED(hr)){
		return FALSE;
	}
	static const GUID guidFileSave=SAVEFILE_LIST_FILE_GUID;

	hr=pDlg->SetClientGuid(guidFileSave);

	if(binary==FALSE)
	{
		if (verilog == TRUE) {
			if (BuildFilterSpecList(IDS_VERILOG_FILE_FILTERS, vecsFilterParts, vecFilters))
			{
				hr = pDlg->SetFileTypes((UINT)vecFilters.size(), &vecFilters[0]);
			}
		}
		else {
			if (BuildFilterSpecList(IDS_ASM_FILE_FILTERS, vecsFilterParts, vecFilters))
			{
				hr = pDlg->SetFileTypes((UINT)vecFilters.size(), &vecFilters[0]);
			}
		}
	}else{
		if(BuildFilterSpecList(IDS_BIN_FILE_FILTERS,vecsFilterParts,vecFilters))
		{
			hr = pDlg->SetFileTypes((UINT)vecFilters.size(),&vecFilters[0]);
		}

	}
	// Set some other properties of the dialog. It's not the end of the world if
	// any of these calls fail.
	pDlg->SetTitle(strTitle);
	pDlg->SetOkButtonLabel(strOKButton);
	pDlg->SetFileNameLabel(strFileNameLable);
	pDlg->SetFileName(filenamepath);
	pDlg->SetDefaultExtension(strDefaultExtention);

// 	hr=pDlg->GetOptions(&dwFlags);
	pDlg->SetOptions(/*dwFlags|*/FOS_SHAREAWARE|FOS_NOREADONLYRETURN|FOS_FORCEPREVIEWPANEON|FOS_FORCEFILESYSTEM);

	// Set up our event listener.
	CComObjectStackEx<CSaveListDialogsHandler> cbk;
	CComQIPtr<IFileDialogEvents> pEvents=cbk.GetUnknown();
	DWORD dwCookie;
	bool bAdvised;

	hr=pDlg->Advise(pEvents,&dwCookie);

	bAdvised=SUCCEEDED(hr);

	CComQIPtr<IFileDialogCustomize> pfdc=pDlg;

	if(pfdc){

	}

	hr=pDlg->Show(NULL);

	if(bAdvised){
		pDlg->Unadvise(dwCookie);
	}

	if(SUCCEEDED(hr)){
		CComPtr<IShellItem> pItem;
		hr=pDlg->GetResult(&pItem);
		if(SUCCEEDED(hr)){
			CString sPath;
			if(PathFromShellItem(pItem,sPath)){

				filenamepath=sPath;
			}
		}
		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

