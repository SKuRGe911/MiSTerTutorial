#pragma once
#ifndef __PAGE1_H__
#define __PAGE1_H__
//////////////////////////////////////////////////////////////////////////
#include "../../HTL/HTLRESIZE.H"
#include "../../RESOURCEDEF.H"
//////////////////////////////////////////////////////////////////////////
#define TABLEGEN_REGISTRY_FILE											L"GRNM TABLEGEN"
#define TABLEGEN_REGISTRY_KEY_X_SIZE_KEY								L"CURRENT X SIZE"
#define TABLEGEN_REGISTRY_KEY_X_SIZE_KEY_DEFAULT						L"255"
#define TABLEGEN_REGISTRY_KEY_Y_SIZE_KEY								L"CURRENT Y SIZE"
#define TABLEGEN_REGISTRY_KEY_Y_SIZE_KEY_DEFAULT						L"255"
#define TABLEGEN_REGISTRY_KEY_PATH_KEY									L"CURRENT PATH"
#define TABLEGEN_REGISTRY_KEY_PATH_KEY_DEFAULT							L"OUTPUT.ASM"
#define TABLEGEN_REGISTRY_SINE_MAX_VALUE_KEY							L"SINE MAX VALUE"
#define TABLEGEN_REGISTRY_SINE_MAX_VALUE_KEY_DEFAULT					L"255"
#define TABLEGEN_REGISTRY_SINE_MID_VALUE_KEY							L"SINE MID VALUE"
#define TABLEGEN_REGISTRY_SINE_MID_VALUE_KEY_DEFAULT					L"127"
#define TABLEGEN_REGISTRY_KEY_VERILOG_BIN_PATH_KEY						L"CURRENT VERILOG BINARY PATH"
#define TABLEGEN_REGISTRY_KEY_VERILOG_BIN_PATH_KEY_DEFAULT				L"INPUT.BIN"
//////////////////////////////////////////////////////////////////////////
#define GRNMHEADERLIST_REGISTRY_SECTION									L"GRNM HEADER LIST"
#define GRNMHEADERLIST_REGISTRY_KEY										L"HEADER FILES"
#define GRNMHEADERLIST_DEFAULT											L"*.h"
//////////////////////////////////////////////////////////////////////////
class CPropertyPage1:
public CResizablePropertyPageImpl<CPropertyPage1>
{

private:

	//////////////////////////////////////////////////////////////////////////
	CStatic																	m_InfoStatic;
	//////////////////////////////////////////////////////////////////////////
	CEdit																	m_A_text;
	CStatic																	m_A_text_lable;
	CButton																	m_A_button;
	CEdit																	m_B_text;
	CStatic																	m_B_text_lable;
	CButton																	m_B_button;
	CEdit																	m_C_text;
	CStatic																	m_C_text_lable;
	CButton																	m_C_button;
	CEdit																	m_D_text;
	CStatic																	m_D_text_lable;
	CButton																	m_D_button;
	//////////////////////////////////////////////////////////////////////////
	UINT64&																	m_TableTypeId;
	CString&																m_OutputFileName;
	CString																	m_InputFileName;
	BOOL&																	m_Binary;
	BOOL&																	m_Verilog;
	BOOL&																	m_16Bit;
	BOOL&																	m_endian;
	BOOL																	m_Initialized;
	//////////////////////////////////////////////////////////////////////////
	DWORD																	m_Sine_max_value;
	DWORD																	m_Sine_mid_value;
	//////////////////////////////////////////////////////////////////////////

public:

	//////////////////////////////////////////////////////////////////////////
	typedef CPropertyPage1												thisClass;
	typedef CResizablePropertyPageImpl<CPropertyPage1>					baseClass;
	typedef CResizablePropertyPageImpl									baseClassNoParent;
	//////////////////////////////////////////////////////////////////////////
	enum { IDD=IDD_PAGE1 };
	//////////////////////////////////////////////////////////////////////////
	CPropertyPage1(CString& outputfile, UINT64& id,BOOL& binary,BOOL& verilog,BOOL& wordsize, BOOL& endian,ATL::_U_STRINGorID title=(LPCTSTR)NULL,bool IsExterior=false,bool EnableDoubleBuffer=false);
	~CPropertyPage1(VOID);
	//////////////////////////////////////////////////////////////////////////
	int OnSetActive(VOID);
	BOOL OnQueryCancel(VOID);
	INT_PTR OnWizardFinish(VOID);
	int OnWizardNext(VOID);
	//////////////////////////////////////////////////////////////////////////
	LRESULT OnEnChangeEditA(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditB(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditC(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditD(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonA(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonB(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonC(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonD(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//////////////////////////////////////////////////////////////////////////
	BEGIN_MSG_MAP_EX(baseClassNoParent)
		COMMAND_HANDLER(IDC_EDIT_A, EN_CHANGE, OnEnChangeEditA)
		COMMAND_HANDLER(IDC_EDIT_B, EN_CHANGE, OnEnChangeEditB)
		COMMAND_HANDLER(IDC_EDIT_C, EN_CHANGE, OnEnChangeEditC)
		COMMAND_HANDLER(IDC_EDIT_D, EN_CHANGE, OnEnChangeEditD)
		COMMAND_HANDLER(IDC_BUTTON_A, BN_CLICKED, OnBnClickedButtonA)
		COMMAND_HANDLER(IDC_BUTTON_B, BN_CLICKED, OnBnClickedButtonB)
		COMMAND_HANDLER(IDC_BUTTON_C, BN_CLICKED, OnBnClickedButtonC)
		COMMAND_HANDLER(IDC_BUTTON_D, BN_CLICKED, OnBnClickedButtonD)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(baseClass)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	//////////////////////////////////////////////////////////////////////////
	BEGIN_DLGRESIZE_MAP_EX(thisClass)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_EDIT_A, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_BOX_A_TEXT, NULL)
		DLGRESIZE_CONTROL_EX(IDC_BUTTON_A, DLSZ_MOVE_X)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_EDIT_B, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_BOX_B_TEXT, NULL)
		DLGRESIZE_CONTROL_EX(IDC_BUTTON_B, DLSZ_MOVE_X)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_EDIT_C, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_BOX_C_TEXT, NULL)
		DLGRESIZE_CONTROL_EX(IDC_BUTTON_C, DLSZ_MOVE_X)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_EDIT_D, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_BOX_D_TEXT, NULL)
		DLGRESIZE_CONTROL_EX(IDC_BUTTON_D, DLSZ_MOVE_X)
		//////////////////////////////////////////////////////////////////////////
	END_DLGRESIZE_MAP_EX()
	//////////////////////////////////////////////////////////////////////////
	LRESULT OnInitDialog(HWND hWnd,LPARAM lParam);
	//////////////////////////////////////////////////////////////////////////
	BOOL SetType(VOID);
	//////////////////////////////////////////////////////////////////////////

};
//////////////////////////////////////////////////////////////////////////
#endif//__PAGE1_H__