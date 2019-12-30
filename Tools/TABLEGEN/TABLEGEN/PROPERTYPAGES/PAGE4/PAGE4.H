#pragma once
#ifndef __PAGE4_H__
#define __PAGE4_H__
//////////////////////////////////////////////////////////////////////////
#include "../../HTL/HTLRESIZE.H"
#include "../../RESOURCEDEF.H"
//////////////////////////////////////////////////////////////////////////
#define GRNM_LOOKUP_REGISTRY_SECTION									L"LOOKUP"
#define GRNM_LOOKUP_TOTAL_ENTRIES_REGISTRY_KEY							L"LAST TOTAL ENTRIES"
#define GRNM_LOOKUP_START_VALUE_REGISTRY_KEY							L"LAST START VALUE"
#define GRNM_LOOKUP_END_VALUE_REGISTRY_KEY								L"LAST END VALUE"
#define GRNM_LOOKUP_INCREMENT_VALUE_REGISTRY_KEY						L"INCREMENT VALUE"
#define GRNM_LOOKUP_TOTAL_ENTRTIES_DEFAULT								L"8"
#define GRNM_LOOKUP_START_VALUE_DEFAULT									L"0"
#define GRNM_LOOKUP_END_VALUE_DEFAULT									L"0"
#define GRNM_LOOKUP_INCREMENT_VALUE_DEFAULT								L"1"
//////////////////////////////////////////////////////////////////////////
extern DWORD TABLE_TYPE_CHANGED;
//////////////////////////////////////////////////////////////////////////
class CPropertyPage4 :
	public CResizablePropertyPageImpl<CPropertyPage4>
{

protected:

	//////////////////////////////////////////////////////////////////////////
	CEdit																m_TotalEntries;
	CEdit																m_StartValue;
	CEdit																m_EndValue;
	CEdit																m_IncrementValue;
	//////////////////////////////////////////////////////////////////////////
	CString&															m_FileName;
	//////////////////////////////////////////////////////////////////////////
	signed short int													m_TotalEntriesDW;
	signed short int													m_StartValueDW;
	signed short int													m_EndValueDW;
	signed short int													m_IncrementValueDW;
	//////////////////////////////////////////////////////////////////////////

public:

	//////////////////////////////////////////////////////////////////////////
	typedef CPropertyPage4												thisClass;
	typedef CResizablePropertyPageImpl<CPropertyPage4>					baseClass;
	typedef CResizablePropertyPageImpl									baseClassNoParent;
	//////////////////////////////////////////////////////////////////////////
	enum { IDD = IDD_PAGE4 };
	//////////////////////////////////////////////////////////////////////////
	CPropertyPage4(CString& filename, ATL::_U_STRINGorID title = (LPCTSTR)NULL, bool IsExterior = false, bool EnableDoubleBuffer = false);
	~CPropertyPage4(VOID);
	//////////////////////////////////////////////////////////////////////////
	BOOL OnSetActive(VOID);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	INT_PTR OnWizardFinish(VOID);
	BOOL OnQueryCancel(VOID);
	//////////////////////////////////////////////////////////////////////////
	LRESULT OnEnChangeEditTotalEntries(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditStartValue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditEndValue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditIncrement(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//////////////////////////////////////////////////////////////////////////
	BEGIN_MSG_MAP_EX(baseClassNoParent)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER(IDC_EDIT_TOTAL_ENTRIES, EN_CHANGE, OnEnChangeEditTotalEntries)
		COMMAND_HANDLER(IDC_EDIT_START_VALUE, EN_CHANGE, OnEnChangeEditStartValue)
		COMMAND_HANDLER(IDC_EDIT_END_VALUE, EN_CHANGE, OnEnChangeEditEndValue)
		COMMAND_HANDLER(IDC_EDIT_INCREMENT, EN_CHANGE, OnEnChangeEditIncrement)
		CHAIN_MSG_MAP(baseClass)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	//////////////////////////////////////////////////////////////////////////
	BEGIN_DLGRESIZE_MAP_EX(thisClass)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_STATIC_TOTAL_ENTRIES, NULL)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_START_VALUE, NULL)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_END_VALUE, NULL)
		DLGRESIZE_CONTROL_EX(IDC_STATIC_INCREMENT, NULL)
		//////////////////////////////////////////////////////////////////////////
		DLGRESIZE_CONTROL_EX(IDC_EDIT_TOTAL_ENTRIES, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_EDIT_START_VALUE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_EDIT_END_VALUE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL_EX(IDC_EDIT_INCREMENT, DLSZ_SIZE_X)
		//////////////////////////////////////////////////////////////////////////
	END_DLGRESIZE_MAP_EX()
	//////////////////////////////////////////////////////////////////////////

};
//////////////////////////////////////////////////////////////////////////
#endif//__PAGE4_H__