#include "GRNMPCH.H"
#include "PAGE0.H"
#include "../../DIALOGS/SAVELISTDIALOG/SAVELISTDIALOG.H"
//////////////////////////////////////////////////////////////////////////
using namespace HTL;
//////////////////////////////////////////////////////////////////////////
CPropertyPage0::CPropertyPage0(CString& outputfile,UINT64& id,BOOL& binary, BOOL& wordsize,BOOL& verilog,BOOL& endian,ATL::_U_STRINGorID title,bool IsExterior,bool EnableDoubleBuffer):
CResizablePropertyPageImpl<CPropertyPage0>(title,IsExterior,EnableDoubleBuffer),
DlgTitleFont(NULL),
TitleText(NULL),
DescText(NULL),
m_TableTypeListBox(NULL),
m_SelectOutputFile(NULL),
m_BinarySelectButton(NULL),
m_16BitSelectButton(NULL),
m_EndianBitSelectButton(NULL),
m_TableTypeId(id),
m_OutputFileName(outputfile),
m_Binary(binary),
m_Verilog(verilog),
m_16bit(wordsize),
m_endian(endian)
{

}
//////////////////////////////////////////////////////////////////////////
CPropertyPage0::~CPropertyPage0(VOID)
{

}
//////////////////////////////////////////////////////////////////////////
BOOL CPropertyPage0::OnSetActive(VOID)
{

	//////////////////////////////////////////////////////////////////////////
	GetPropertySheet().SetWizardButtons(PSWIZB_NEXT);
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnInitDialog(HWND hWnd,LPARAM lParam)
{

	//////////////////////////////////////////////////////////////////////////
	//SETUP THE TOP PART OF THE WINDOWS DISPLAY								//
	//////////////////////////////////////////////////////////////////////////
	CString txt;
	//////////////////////////////////////////////////////////////////////////
	TitleText.Attach(GetDlgItem(IDC_LBL_TITLE));
	txt.LoadStringW(IDS_GERANIUM_CONVERT_MESSAGE);
	TitleText.SetWindowTextW(txt);
	TitleText.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	DescText.Attach(GetDlgItem(IDC_LBL_WELCOME));
	txt.LoadStringW(IDS_GERANIUM_CONVERT_MESSAGE_1);
	DescText.SetWindowTextW(txt);
	DescText.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	CFontHandle DlgFont=GetFont();
	CClientDC dcScreen(NULL);
	//////////////////////////////////////////////////////////////////////////
	LOGFONT LogFont={0};
	DlgFont.GetLogFont(&LogFont);
	SecureHelper::strcpyW_x(LogFont.lfFaceName,32,L"verdana bold");
	//////////////////////////////////////////////////////////////////////////
	LogFont.lfWeight=FW_BOLD;
	LogFont.lfHeight=-::MulDiv(12,dcScreen.GetDeviceCaps(LOGPIXELSY),72);
	//////////////////////////////////////////////////////////////////////////
	DlgTitleFont.CreateFontIndirect(&LogFont);
	TitleText.SetFont(DlgTitleFont);
	//////////////////////////////////////////////////////////////////////////
	m_BinarySelectButton.Attach(GetDlgItem(IDC_BINARY_CHECK));
	txt.LoadString(IDS_TEXT_BINARY_BUTTON);
	m_BinarySelectButton.SetWindowText(txt);
	m_Binary = (UINT64)_AppSettings.GetUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION,GRNM_OUTPUT_BINARY_REGISTRY_KEY,GRNM_OUTPUT_BINARY_DEFAULT);
	m_BinarySelectButton.SetCheck(m_Binary);
	m_BinarySelectButton.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	m_VerilogSelectButton.Attach(GetDlgItem(IDC_VERILOG_CHECK));
	txt.LoadString(IDS_TEXT_VERILOG_BUTTON);
	m_VerilogSelectButton.SetWindowText(txt);
	m_Verilog = (UINT64)_AppSettings.GetUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_VERILOG_REGISTRY_KEY, GRNM_OUTPUT_VERILOG_DEFAULT);
	m_VerilogSelectButton.SetCheck(m_Verilog);
	m_VerilogSelectButton.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	m_16BitSelectButton.Attach(GetDlgItem(IDC_WORDSIZE_CHECK));
	txt.LoadString(IDS_TEXT_WORDSIZE_BUTTON);
	m_16BitSelectButton.SetWindowText(txt);
	m_16bit = (UINT64)_AppSettings.GetUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_WORDSIZE_REGISTRY_KEY, GRNM_OUTPUT_WORDSIZE_DEFAULT);
	m_16BitSelectButton.SetCheck(m_16bit);
	m_16BitSelectButton.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	m_EndianBitSelectButton.Attach(GetDlgItem(IDC_ENDIAN_CHECK));
	txt.LoadString(IDS_TEXT_ENDIAN_BUTTON);
	m_EndianBitSelectButton.SetWindowText(txt);
	m_endian = (UINT64)_AppSettings.GetUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_ENDIAN_REGISTRY_KEY, GRNM_OUTPUT_ENDIAN_DEFAULT);
	m_EndianBitSelectButton.SetCheck(m_endian);
	m_EndianBitSelectButton.Invalidate(TRUE);
	//////////////////////////////////////////////////////////////////////////
	m_TableTypeListBox.Attach(GetDlgItem(IDC_COMBO_TABLE_TYPE));
	txt.LoadString(IDS_TEXT_LIST_BOX_BITMAP_IMAGE);
	int a=m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0+0));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_SIN_WAVE);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0+1));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_BITMAP_IMAGE);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0 + 2));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_BITMAP_TESTER);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0 + 3));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_LOOK_UP_TABLE);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0 + 4));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_ASCII_TEXT_CONVERSION);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0 + 5));
	//////////////////////////////////////////////////////////////////////////
	txt.LoadString(IDS_TEXT_LIST_BOX_VERILOG_ROM);
	a = m_TableTypeListBox.AddString(txt);
	m_TableTypeListBox.SetItemDataPtr(a, (LPVOID)((UINT64)0 + 6));
	//////////////////////////////////////////////////////////////////////////
	UINT64 ListSelItem = (UINT64)_AppSettings.GetUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_TYPE_REGISTRY_KEY, GRNM_OUTPUT_TYPE_DEFAULT);
	for (DWORD item = 0; item < 7; item++) {
		LPVOID item_pointer = m_TableTypeListBox.GetItemDataPtr(item);
		if (item_pointer == (LPVOID)ListSelItem) {
			m_TableTypeListBox.SetCurSel(item);
			m_TableTypeListBox.Invalidate(TRUE);
			m_TableTypeId = ListSelItem;
			GetPropertySheet().PostMessage(TABLE_TYPE_CHANGED, 0, 0);
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CString LastPath = _AppSettings.GetUserProfileString(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_PATH_REGISTRY_KEY, GRNM_OUTPUT_PATH_DEFAULT);
	m_SelectOutputFile.Attach(GetDlgItem(IDC_BUTTON_FILE_SELECT));
	m_OutputFileNameEdit.Attach(GetDlgItem(IDC_EDIT_FILE_NAME_OUTPUT));
	m_OutputFileNameEdit.SetWindowText(LastPath);
	m_OutputFileNameEdit.Invalidate(TRUE);
	m_OutputFileName = LastPath;
	//////////////////////////////////////////////////////////////////////////
	SetMsgHandled(FALSE);
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
BOOL CPropertyPage0::OnQueryCancel(VOID)
{

	//////////////////////////////////////////////////////////////////////////
	return FALSE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnCbnSelchangeComboTableType(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	int a=m_TableTypeListBox.GetCurSel();
	//////////////////////////////////////////////////////////////////////////
	LPVOID i = m_TableTypeListBox.GetItemDataPtr(a);
	//////////////////////////////////////////////////////////////////////////
	m_TableTypeId = (UINT64)i;
	_AppSettings.WriteUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_TYPE_REGISTRY_KEY,(DWORD) m_TableTypeId);
	//////////////////////////////////////////////////////////////////////////
	GetPropertySheet().PostMessage(TABLE_TYPE_CHANGED, 0, 0);
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnBnClickedButtonFileSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	CString FileName;
	//////////////////////////////////////////////////////////////////////////
	if (SaveFileListWithIFileOpenDlgEx(FileName,m_Binary,m_Verilog) == TRUE) {
		//////////////////////////////////////////////////////////////////////////
		_AppSettings.WriteUserProfileString(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_PATH_REGISTRY_KEY, FileName);
		m_OutputFileName = FileName;
		m_OutputFileNameEdit.SetWindowText(m_OutputFileName);
		m_OutputFileNameEdit.Invalidate(TRUE);
		//////////////////////////////////////////////////////////////////////////
	}
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnBnClickedBinaryCheck(WORD /*wNotifyCode*/,WORD /*wID*/,HWND /*hWndCtl*/,BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	if(m_Binary==0){
		m_Binary = 1;
	}else{
		m_Binary = 0;
	}
	m_BinarySelectButton.SetCheck(m_Binary);
	m_BinarySelectButton.Invalidate(TRUE);
	_AppSettings.WriteUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION,GRNM_OUTPUT_BINARY_REGISTRY_KEY,(DWORD)m_Binary);
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnBnClickedVerilogCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	if (m_Verilog == 0) {
		m_Verilog = 1;
	}
	else {
		m_Verilog = 0;
	}
	m_VerilogSelectButton.SetCheck(m_Verilog);
	m_VerilogSelectButton.Invalidate(TRUE);
	_AppSettings.WriteUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_VERILOG_REGISTRY_KEY, (DWORD)m_Verilog);
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnBnClickedWordSizeCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	if (m_16bit == 0) {
		m_16bit = 1;
	}
	else {
		m_16bit = 0;
	}
	m_16BitSelectButton.SetCheck(m_16bit);
	m_16BitSelectButton.Invalidate(TRUE);
	_AppSettings.WriteUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_WORDSIZE_REGISTRY_KEY, (DWORD)m_16bit);
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
LRESULT CPropertyPage0::OnBnClickedEndianCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//////////////////////////////////////////////////////////////////////////
	bHandled = TRUE;
	//////////////////////////////////////////////////////////////////////////
	if (m_endian == 0) {
		m_endian = 1;
	}
	else {
		m_endian = 0;
	}
	m_EndianBitSelectButton.SetCheck(m_endian);
	m_EndianBitSelectButton.Invalidate(TRUE);
	_AppSettings.WriteUserProfileDword(GRNM_OUTPUT_REGISTRY_SECTION, GRNM_OUTPUT_ENDIAN_REGISTRY_KEY, (DWORD)m_endian);
	//////////////////////////////////////////////////////////////////////////
	return 0;
	//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
