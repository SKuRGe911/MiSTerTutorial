#pragma once
#ifndef __PAGE0_H__
#define __PAGE0_H__
//////////////////////////////////////////////////////////////////////////
#include "../../HTL/HTLRESIZE.H"
#include "../../RESOURCEDEF.H"
//////////////////////////////////////////////////////////////////////////
#define GRNM_OUTPUT_REGISTRY_SECTION									L"LAST OUTPUT"
#define GRNM_OUTPUT_TYPE_REGISTRY_KEY									L"LAST TYPE"
#define GRNM_OUTPUT_PATH_REGISTRY_KEY									L"LAST PATH"
#define GRNM_OUTPUT_BINARY_REGISTRY_KEY									L"LAST BINARY"
#define GRNM_OUTPUT_VERILOG_REGISTRY_KEY								L"LAST VERILOG"
#define GRNM_OUTPUT_WORDSIZE_REGISTRY_KEY								L"LAST WORDSIZE"
#define GRNM_OUTPUT_ENDIAN_REGISTRY_KEY									L"LAST ENDIAN"
#define GRNM_OUTPUT_TYPE_DEFAULT										1
#define GRNM_OUTPUT_PATH_DEFAULT										L""
#define GRNM_OUTPUT_BINARY_DEFAULT										0
#define GRNM_OUTPUT_VERILOG_DEFAULT										0
#define GRNM_OUTPUT_WORDSIZE_DEFAULT									0
#define GRNM_OUTPUT_ENDIAN_DEFAULT										0
//////////////////////////////////////////////////////////////////////////
extern DWORD TABLE_TYPE_CHANGED;
//////////////////////////////////////////////////////////////////////////
class CPropertyPage0:
public CResizablePropertyPageImpl<CPropertyPage0>
{

public:

	//////////////////////////////////////////////////////////////////////////
	CFont																DlgTitleFont;
	//////////////////////////////////////////////////////////////////////////
	CStatic																TitleText;
	CStatic																DescText;
	//////////////////////////////////////////////////////////////////////////
	CButton																m_SelectOutputFile;
	CEdit																m_OutputFileNameEdit;
	CComboBox															m_TableTypeListBox;
	CButton																m_BinarySelectButton;
	CButton																m_VerilogSelectButton;
	CButton																m_16BitSelectButton;
	CButton																m_EndianBitSelectButton;
	//////////////////////////////////////////////////////////////////////////
	UINT64&																m_TableTypeId;
	CString&															m_OutputFileName;
	BOOL&																m_Binary;
	BOOL&																m_Verilog;
	BOOL&																m_16bit;
	BOOL&																m_endian;
	//////////////////////////////////////////////////////////////////////////

public:

	//////////////////////////////////////////////////////////////////////////
	typedef CPropertyPage0												thisClass;
	typedef CResizablePropertyPageImpl<CPropertyPage0>					baseClass;
	typedef CResizablePropertyPageImpl									baseClassNoParent;
	//////////////////////////////////////////////////////////////////////////
	enum { IDD=IDD_PAGE0 };
	//////////////////////////////////////////////////////////////////////////
	CPropertyPage0(CString& outputfile, UINT64& id,BOOL& binary, BOOL& verilog,BOOL& endian,BOOL& wordsize, ATL::_U_STRINGorID title=(LPCTSTR)NULL,bool IsExterior=false,bool EnableDoubleBuffer=false);
	~CPropertyPage0(VOID);
	//////////////////////////////////////////////////////////////////////////
	BOOL OnSetActive(VOID);
	LRESULT OnInitDialog(HWND hWnd,LPARAM lParam);
	BOOL OnQueryCancel(VOID);
	//////////////////////////////////////////////////////////////////////////
	LRESULT OnCbnSelchangeComboTableType(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFileSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBinaryCheck(WORD /*wNotifyCode*/,WORD /*wID*/,HWND /*hWndCtl*/,BOOL& /*bHandled*/);
	LRESULT OnBnClickedVerilogCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedWordSizeCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnBnClickedEndianCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//////////////////////////////////////////////////////////////////////////
	BEGIN_MSG_MAP_EX(baseClassNoParent)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_FILE_SELECT, BN_CLICKED, OnBnClickedButtonFileSelect)
		COMMAND_HANDLER(IDC_COMBO_TABLE_TYPE, CBN_SELCHANGE, OnCbnSelchangeComboTableType)
		COMMAND_HANDLER(IDC_BINARY_CHECK,BN_CLICKED,OnBnClickedBinaryCheck)
		COMMAND_HANDLER(IDC_VERILOG_CHECK, BN_CLICKED, OnBnClickedVerilogCheck)
		COMMAND_HANDLER(IDC_WORDSIZE_CHECK, BN_CLICKED, OnBnClickedWordSizeCheck)
		COMMAND_HANDLER(IDC_ENDIAN_CHECK, BN_CLICKED, OnBnClickedEndianCheck)
		CHAIN_MSG_MAP(baseClass)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	//////////////////////////////////////////////////////////////////////////
	BEGIN_DLGRESIZE_MAP_EX(thisClass)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_LBL_TITLE,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_LBL_WELCOME,DLSZ_SIZE_X)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_COMBO_TABLE_TYPE,DLSZ_MOVE_Y|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_BUTTON_FILE_SELECT,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL_EX(IDC_EDIT_FILE_NAME_OUTPUT,DLSZ_MOVE_Y|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_BINARY_CHECK,DLSZ_MOVE_Y|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_VERILOG_CHECK, DLSZ_MOVE_Y | DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_ENDIAN_CHECK, DLSZ_MOVE_Y | DLSZ_SIZE_X | DLSZ_MOVE_X)
		//////////////////////////////////////////////////////////////////////////
	END_DLGRESIZE_MAP_EX()
	//////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
#endif//__PAGE0_H__