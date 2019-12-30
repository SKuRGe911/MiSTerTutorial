#include "GRNMPCH.H"
#include "APPWINDOW.H"
//////////////////////////////////////////////////////////////////////////
DWORD TABLE_TYPE_CHANGED= ::RegisterWindowMessageW(L"TABLE_TYPE_CHANGED");
//////////////////////////////////////////////////////////////////////////
CAppWindow::CAppWindow(VOID):
	m_SingleInstance(),
	OutputFilePath(),
	baseClass(OutputFilePath,TableType,Binary, Verilog, WordSize, Endian,(LPCTSTR)NULL,NULL,NULL,true,false),
	Page0(OutputFilePath,TableType,Binary, Verilog, WordSize, Endian,(LPCTSTR)IDS_WINDOW_TITLE_0,true,true),
	Page1(OutputFilePath,TableType,Binary, Verilog, WordSize, Endian,(LPCTSTR)IDS_WINDOW_TITLE_1,true,true),
	Page2(TableType,(LPCTSTR)IDS_WINDOW_TITLE_2,true,true),
	Page3(OutputFilePath,(LPCTSTR)IDS_WINDOW_TITLE_3,true,true),
	Page4(OutputFilePath,(LPCTSTR)IDS_WINDOW_TITLE_4,true,true),
	Page5(OutputFilePath,(LPCTSTR)IDS_WINDOW_TITLE_5,true,true)
{

}
//////////////////////////////////////////////////////////////////////////
CAppWindow::~CAppWindow(VOID)
{

}
//////////////////////////////////////////////////////////////////////////
BOOL CAppWindow::Initialize(int nCmdShow)
{

	//////////////////////////////////////////////////////////////////////////
	HWND retwin=m_SingleInstance.Find(NULL, GRNM_SINGLEINSTANCE_STRING);
	if(retwin!=NULL){
		::SetActiveWindow(retwin);
		::SetForegroundWindow(retwin);
		::ShowWindow(retwin,SW_SHOW);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	m_SingleInstance.Mark(baseClass::m_hWnd, GRNM_SINGLEINSTANCE_STRING);
	//////////////////////////////////////////////////////////////////////////
	baseClass::SetHeader((LPCTSTR)IDR_BLUEGERANIUM);
	baseClass::SetWatermark((LPCTSTR)IDR_GERANIUM_BANNER);
	//////////////////////////////////////////////////////////////////////////
	{
		CString ReadyToConvert;
		ReadyToConvert.LoadString(IDS_GERANIUM_CONVERT_MESSAGE);
		Page1.SetHeaderTitle(ReadyToConvert);
		ReadyToConvert.LoadString(IDS_GERANIUM_CONVERT_MESSAGE_1);
		Page1.SetHeaderSubTitle(ReadyToConvert);
	}
	//////////////////////////////////////////////////////////////////////////
	baseClass::AddPage(Page0);
	DWORD itempos= _AppSettings.GetUserProfileDword(GRNM_REGISTRY_FOLDER_APP, GRNM_REGISTRY_KEY_TABLE_TYPE_ID, 0);
	if (itempos == 0) {
		baseClass::AddPage(Page1);
		TableType = 0;
	}else if (itempos == 1) {
		baseClass::AddPage(Page1);
		TableType = 1;
	}else if (itempos == 2) {
		baseClass::AddPage(Page1);
		TableType = 2;
	}
	else if (itempos == 3) {
		baseClass::AddPage(Page3);
		TableType = 3;
	}
	else if (itempos == 4) {
		baseClass::AddPage(Page4);
		TableType = 4;
	}
	else if (itempos == 5) {
		baseClass::AddPage(Page5);
		TableType = 5;
	}
	else if (itempos == 6) {
		baseClass::AddPage(Page1);
		TableType = 5;
	}
	else {
		baseClass::AddPage(Page2);
		TableType = -1;
	}
	//////////////////////////////////////////////////////////////////////////
	Page0.EnableDoubleBuffering();
	Page1.EnableDoubleBuffering();
	Page2.EnableDoubleBuffering();
	Page3.EnableDoubleBuffering();
	Page4.EnableDoubleBuffering();
	Page5.EnableDoubleBuffering();
	//////////////////////////////////////////////////////////////////////////
	if(baseClass::Create(NULL)==NULL){
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	CRect winpos;
	winpos.left=_AppSettings.GetUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINPOS_Y,0);
	winpos.top=_AppSettings.GetUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINPOS_Y,0);
	winpos.right=_AppSettings.GetUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINSIZ_X,100);
	winpos.bottom=_AppSettings.GetUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINSIZ_Y,100);
	//////////////////////////////////////////////////////////////////////////
	baseClass::SetWindowPos(NULL,&winpos,SWP_SHOWWINDOW);
	baseClass::ShowWindow(nCmdShow);
	baseClass::SetActiveWindow();
	::SetForegroundWindow(baseClass::m_hWnd);
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
BOOL CAppWindow::UnInitialize(VOID)
{

	//////////////////////////////////////////////////////////////////////////
	int nRet=0;
	if(baseClass::IsWindow()){
		//////////////////////////////////////////////////////////////////////////
		//STORE WINDOW POSITION													//
		//////////////////////////////////////////////////////////////////////////
		CRect winpos;
		baseClass::GetWindowRect(&winpos);
		_AppSettings.WriteUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINPOS_X,winpos.left);
		_AppSettings.WriteUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINPOS_Y,winpos.top);
		_AppSettings.WriteUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINSIZ_X,winpos.right);
		_AppSettings.WriteUserProfileDword(GRNM_REGISTRY_FOLDER_WINPOS,GRNM_REGISTRY_KEY_WINSIZ_Y,winpos.bottom);
		//////////////////////////////////////////////////////////////////////////
		_AppSettings.WriteUserProfileDword(GRNM_REGISTRY_FOLDER_APP, GRNM_REGISTRY_KEY_TABLE_TYPE_ID, m_TableType);
		//////////////////////////////////////////////////////////////////////////
		nRet= baseClass::GetResult();
		if((nRet==1)){	//FINNISH WAS PRESSED AND BUILDNOW SELECTED//
		}
		baseClass::DestroyWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	m_SingleInstance.Clear();
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
LRESULT CAppWindow::OnTableTypeChanged(UINT, WPARAM, LPARAM, BOOL& bHandled) {
	bHandled = TRUE;
	
	switch (TableType) {
		case 0:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page1);
			break;
		}
		case 1:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page1);
			break;
		}
		case 2:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page1);
			break;
		}
		case 3:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page3);
			break;
		}
		case 4:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page4);
			break;
		}
		case 5:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page5);
			break;
		}
		case 6:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page1);
			break;
		}
		default:
		{
			baseClass::RemovePage(1);
			baseClass::AddPage(Page2);
			break;

		}
	}
	return FALSE;

}
//////////////////////////////////////////////////////////////////////////
CMainPage::CMainPage(CString& outputpath, UINT64& tabletype,BOOL& binary, BOOL& verilog, BOOL& wordsize, BOOL& endian, ATL::_U_STRINGorID title,UINT uStartPage,HWND hWndParent,bool EnableDoubleBuffering,bool IsWizard):
	baseClass(title,uStartPage,hWndParent,EnableDoubleBuffering,IsWizard),
	HasCentered(false),
	m_TableType(tabletype),
	m_FileOutputPath(outputpath),
	m_Binary(binary),
	m_Verilog(verilog),
	m_16Bit(wordsize),
	m_endian(endian)
{

}
//////////////////////////////////////////////////////////////////////////
CMainPage::~CMainPage(VOID)
{

}
//////////////////////////////////////////////////////////////////////////
VOID  CMainPage::OnShowWindow(BOOL,INT)
{
	CenterWindow(GetDesktopWindow());
	SetMsgHandled(FALSE);
}
//////////////////////////////////////////////////////////////////////////





