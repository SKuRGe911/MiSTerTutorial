#pragma once
#ifndef __WTLRESIZE_H__
#define __WTLRESIZE_H__

#ifndef _WTL_NEW_PAGE_NOTIFY_HANDLERS 
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS 
#endif  // _WTL_NEW_PAGE_NOTIFY_HANDLERS 

#ifndef __ATLCRACK_H__
#include <atlcrack.h>
#endif

namespace WTL
{

	#define BEGIN_DLGRESIZE_MAP_EX(thisClass) \
	void StaticResizeMap() {

	#define DLGRESIZE_CONTROL_EX(ControlId, ResizeFlags) \
		InitialMap.Add(_AtlDlgResizeMapEx(ControlId, ResizeFlags));

	#define END_DLGRESIZE_MAP_EX() \
		InitialMap.Add(_AtlDlgResizeMapEx(-1, 0)); \
	}

	template <class T>
	class CDialogResizeDynamic: public CDialogResize<T> {
		private:
		bool IsInitialized;
		public:

			/* Same class as _AtlDlgResizeMap only with ctor, should be added in WTL some day */

		struct ATL_NO_VTABLE _AtlDlgResizeMapEx {
			int m_nCtlID;
			DWORD m_dwResizeFlags;
			_AtlDlgResizeMapEx(int ControlId,DWORD ResizeFlags): m_nCtlID(ControlId),m_dwResizeFlags(ResizeFlags){
			}
		};

		ATL::CSimpleArray<_AtlDlgResizeMapEx> InitialMap;

		/* Import all entries from static resize map here */

		CDialogResizeDynamic(): CDialogResize<T>(),IsInitialized(false){
			T* pT=static_cast<T*>(this);

			InitialMap.RemoveAll();
			pT->StaticResizeMap();
		}

		const _AtlDlgResizeMap* GetDlgResizeMap(){
			return (_AtlDlgResizeMap*)InitialMap.GetData(); // force conversion
		}

		void DlgResize_Init(bool bAddGripper=true,bool bUseMinTrackSize=true,DWORD dwForceStyle=WS_CLIPCHILDREN){
			CDialogResize<T>::DlgResize_Init(bAddGripper,bUseMinTrackSize,dwForceStyle);
			IsInitialized=true;
		}

		void ResetResizeMap(){
			InitialMap.RemoveAll();
		}

		// Groups are not allowed if DialogResize is already initialized

		void AddToResizeMap(int ControlId,DWORD ResizeFlags){
			T* pT=static_cast<T*>(this);
			if(IsInitialized){
				ATLASSERT(ControlId!=-1);
				ATL::CWindow ctl=pT->GetDlgItem(ControlId);
				ATLASSERT(ctl.IsWindow());

				RECT rectCtl={0};
				ctl.GetWindowRect(&rectCtl);
				::MapWindowPoints(NULL,pT->m_hWnd,(LPPOINT)&rectCtl,2);

				_AtlDlgResizeData data={ControlId,ResizeFlags,{rectCtl.left,rectCtl.top,rectCtl.right,rectCtl.bottom}};
				m_arrData.Add(data);
			} else{
				if(InitialMap.GetSize()==0){
					InitialMap.Add(_AtlDlgResizeMapEx(ControlId,ResizeFlags));
				} else{
					InitialMap.SetAtIndex(InitialMap.GetSize()-1,_AtlDlgResizeMapEx(ControlId,ResizeFlags));
				}
				InitialMap.Add(_AtlDlgResizeMapEx(-1,0));
			}
		}

		void RemoveFromResizeMap(int ControlId){
			if(IsInitialized){
				ATLASSERT(ControlId!=-1);
				for(int i=0; i<m_arrData.GetSize(); i++){
					if(m_arrData[i].m_nCtlID==ControlId){
						m_arrData.RemoveAt(i);
						break;
					}
				}
			} else{
				for(int i=0; i<InitialMap.GetSize(); i++){
					if(InitialMap[i].m_nCtlID==ControlId){
						InitialMap.RemoveAt(i);
						break;
					}
				}
			}
		}

		void ChangeResizeMapEntry(int ControlId,DWORD ResizeFlags){
			if(IsInitialized){
				ATLASSERT(ControlId!=-1);
				for(int i=0; i<m_arrData.GetSize(); i++){
					if(m_arrData[i].m_nCtlID==ControlId){
						m_arrData[i].m_dwResizeFlags=ResizeFlags;
						break;
					}
				}
			} else{
				for(int i=0; i<InitialMap.GetSize(); i++){
					if(InitialMap[i].m_nCtlID==ControlId){
						InitialMap[i].m_dwResizeFlags=ResizeFlags;
						break;
					}
				}
			}
		}
		#if 0
		void AddToResizeMapWithGroup(_AtlDlgResizeMapEx Map[]){

		}
		#endif
	};

	/* -------------------------------------------------------------------------------- */

	template  <class T,class TBase=CPropertySheetWindow>
	class ATL_NO_VTABLE CResizablePropertySheetImpl
		: public CPropertySheetImpl <T,TBase>,
		public CDialogResizeDynamic <T>,
		public CMessageFilter
	{
		private:
		bool EnableDoubleBuffering;
		bool m_IsWizard;

		enum {
			ID_PSHEET_OK=IDOK,
			ID_PSHEET_APPLY=ID_APPLY_NOW,
			ID_PSHEET_CANCEL=IDCANCEL,
			ID_PSHEET_HELP=IDHELP,
			ID_PSHEET_TAB=ATL_IDC_TAB_CONTROL,
			ID_PSHEET_PREV=ID_WIZBACK,
			ID_PSHEET_NEXT=ID_WIZNEXT,
			ID_PSHEET_FINISH=ID_WIZFINISH,
			ID_PSHEET_BOTTOMFRAME=ID_WIZFINISH+1,
			ID_PSHEET_TOPFRAME=ID_WIZFINISH+2,
		};

		public:

		CResizablePropertySheetImpl(ATL::_U_STRINGorID title=(LPCTSTR)NULL,UINT uStartPage=0,
			HWND hWndParent=NULL,bool EnableDoubleBuffering=false,bool IsWizard=false)
			: CPropertySheetImpl <T,TBase>(title,uStartPage,hWndParent){
			this->EnableDoubleBuffering=EnableDoubleBuffering;

			if(IsWizard){
				SetWizardMode(true);
			}
		}

		static int CALLBACK PropSheetCallback(HWND hWnd,UINT uMsg,LPARAM lParam){
			if(uMsg==PSCB_PRECREATE){
				LPDLGTEMPLATE Template=(LPDLGTEMPLATE)lParam;
				// remove dialog border styles
				Template->style&=~DS_MODALFRAME;

				// add child window and clipping styles
				Template->style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_OVERLAPPEDWINDOW /*| WS_ICONIC*/;
				return 0;
			}
			return CPropertySheetImpl<T>::PropSheetCallback(hWnd,uMsg,lParam);
		}

		void OnSheetInitialized(){
			_Module.GetMessageLoop()->AddMessageFilter(this);
		}

		virtual void OnFinalMessage(HWND hWnd){
			CMessageLoop* pLoop=_Module.GetMessageLoop();
			ATLASSERT(pLoop!=NULL);
			pLoop->RemoveMessageFilter(this);
		}

		void SetWizardMode(bool IsWizard){
			ResetResizeMap();
			if(IsWizard){
				AddToResizeMap(ID_PSHEET_PREV,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_NEXT,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_FINISH,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_CANCEL,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_HELP,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_TOPFRAME,DLSZ_SIZE_X);
				AddToResizeMap(ID_PSHEET_BOTTOMFRAME,DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_BOTTOMFRAME,DLSZ_SIZE_X);
			} else{
				AddToResizeMap(ID_PSHEET_OK,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_APPLY,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_CANCEL,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_HELP,DLSZ_MOVE_X|DLSZ_MOVE_Y);
				AddToResizeMap(ID_PSHEET_TAB,DLSZ_SIZE_X|DLSZ_SIZE_Y);
			}
			this->m_IsWizard=IsWizard;
			CPropertySheetImpl<T>::SetWizardMode();
		}

		void SetHeader(LPCTSTR szbmHeader){
			SetWizardMode(true);
			CPropertySheetImpl<T>::SetHeader(szbmHeader);
		}

		void SetHeader(HBITMAP hbmHeader){
			SetWizardMode(true);
			return CPropertySheetImpl<T>::SetHeader(hbmHeader);
		}

		void SetWatermark(LPCTSTR szbmWatermark,HPALETTE hplWatermark=NULL){
			SetWizardMode(true);
			CPropertySheetImpl<T>::SetWatermark(szbmWatermark,hplWatermark);
		}

		void SetWatermark(HBITMAP hbmWatermark,HPALETTE hplWatermark=NULL){
			SetWizardMode(true);
			return CPropertySheetImpl<T> ::SetWatermark(hbmWatermark,hplWatermark);
		}

		// This is required to handle modeless Dialog messages correctly 
		virtual BOOL PreTranslateMessage(LPMSG pMsg){
			if(IsDialogMessage(pMsg)){
				if(!::IsWindow(m_hWnd)||GetActivePage()==NULL){
					// Not really clean code, but better than creating an 
					// extra property sheet Message Loop class 
					PostQuitMessage(0);
				}
				return TRUE;
			}
			return FALSE;
		}

		void OnSize(WPARAM wParam,CSize Size){
			// Resize Property Sheet controls manually first 
			BOOL Handled=FALSE;
			CDialogResize<T>::OnSize(WM_SIZE,0,MAKELONG(Size.cx,Size.cy),Handled);
			UpdatePropertyPage(GetActivePage());
			SetMsgHandled(FALSE);
		}

		LRESULT OnSetActive(LPNMHDR Hdr){
			UpdatePropertyPage((HWND)((LPPSHNOTIFY)Hdr)->lParam);
			SetMsgHandled(FALSE);
			return TRUE;
		}

		LRESULT OnWmShowWindow(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandled){
			DlgResize_Init();
			#if _WIN32_WINNT >= 0x0501
			if(EnableDoubleBuffering){
				SetWindowLongPtr(GWL_EXSTYLE,GetWindowLongPtr(GWL_EXSTYLE)|WS_EX_COMPOSITED);
			}
			#endif
			bHandled=FALSE;
			return S_OK;
		}

		LRESULT OnEraseBackground(HDC hDc){

			/* Helps prevent flickering during resizing */

			if(m_IsWizard && GetActivePage()){
				RECT rcTop,rcWnd,rcPage;

				GetDlgItem(ID_PSHEET_TOPFRAME).GetWindowRect(&rcTop);
				ScreenToClient(&rcTop);

				CWindow(GetActivePage()).GetWindowRect(&rcPage);
				ScreenToClient(&rcPage);

				ExcludeClipRect(hDc,rcPage.left,rcPage.top,rcPage.right,rcPage.bottom);
				ExcludeClipRect(hDc,rcTop.left,0,rcTop.right,rcTop.bottom);

				GetClientRect(&rcWnd);
				FillRect(hDc,&rcWnd,GetSysColorBrush(COLOR_BTNFACE));

				SelectClipRgn(hDc,0);
				SetMsgHandled(TRUE);
			} else{
				SetMsgHandled(FALSE);
			}
			return TRUE;
		}

		BEGIN_MSG_MAP_EX(CResizablePropertySheetImpl)
			MESSAGE_HANDLER(WM_SHOWWINDOW,OnWmShowWindow)
			MSG_WM_SIZE(OnSize);
			MSG_WM_ERASEBKGND(OnEraseBackground);
			NOTIFY_CODE_HANDLER_EX(PSN_SETACTIVE,OnSetActive)
			CHAIN_MSG_MAP(CPropertySheetImpl<T>)
			CHAIN_MSG_MAP(CDialogResize<T>)
		END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP_EX(CResizablePropertySheetImpl)
			DLGRESIZE_CONTROL_EX(ID_PSHEET_OK,DLSZ_MOVE_X|DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL_EX(ID_PSHEET_APPLY,DLSZ_MOVE_X|DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL_EX(ID_PSHEET_CANCEL,DLSZ_MOVE_X|DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL_EX(ID_PSHEET_HELP,DLSZ_MOVE_X|DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL_EX(ID_PSHEET_TAB,DLSZ_SIZE_X|DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP_EX()
	private:

		void UpdatePropertyPage(HWND hWnd){
			// Adjust property page size 
			CPropertyPageWindow Wnd=hWnd;
			CSize PageMargin;

			if(Wnd){
				RECT rc,rctf,rcbf;
				GetClientRect(&rc);

				int Width=0;
				int Height=0;
				int Top=0;
				int Left=0;

				if(m_IsWizard){
					::GetWindowRect(GetDlgItem(ID_PSHEET_TOPFRAME),&rctf);
					::GetWindowRect(GetDlgItem(ID_PSHEET_BOTTOMFRAME),&rcbf);
					ScreenToClient(&rctf);
					ScreenToClient(&rcbf);

					PageMargin.cx=11;
					PageMargin.cy=11;

					if(::GetProp(hWnd,_T("IsExterior"))){
						Left=0;
						Top=0;
						Width=rc.right;
						Height=rcbf.top;
					} else{
						Top=rctf.top+PageMargin.cy;
						Left=PageMargin.cx;
						Width=rc.right-Left-PageMargin.cx;
						Height=rcbf.top-Top-PageMargin.cy;
					}
				} else{
					RECT rci;
					CTabCtrl Tab=GetTabControl();
					Tab.GetItemRect(HwndToIndex(Wnd),&rci);
					Tab.GetWindowRect(&rc);
					ScreenToClient(&rc);

					PageMargin.cx=4;
					PageMargin.cy=4;

					Top=rc.top+rci.bottom+PageMargin.cy;
					Left=rc.left+PageMargin.cx;
					Width=rc.right-PageMargin.cx-Left;
					Height=rc.bottom-PageMargin.cy-Top;
				}
				Wnd.SetWindowPos(NULL,Left,Top,Width,Height,SWP_NOACTIVATE|SWP_NOZORDER);
			}
		}
	};

	template  <class T,class TBase=CPropertyPageWindow>
	class ATL_NO_VTABLE CResizablePropertyPageImpl:
		public CPropertyPageImpl<T,TBase>,
		public CDialogResizeDynamic<T>
	{
		private:
		bool IsDoubleBufferEnabled;
		public:
		CResizablePropertyPageImpl(ATL::_U_STRINGorID title=(LPCTSTR)NULL,
			bool IsExterior=false,bool bDoubleBuffer=false)
			: CPropertyPageImpl<T>(title){
			if(IsExterior) m_psp.dwFlags|=PSP_HIDEHEADER;
			IsDoubleBufferEnabled=bDoubleBuffer;
		}

		LRESULT OnInitDialog(HWND hWnd,LPARAM lParam){
			if(m_psp.dwFlags & PSP_HIDEHEADER){
				SetProp(m_hWnd,_T("IsExterior"),(HANDLE)1);
			}
			#if _WIN32_WINNT >= 0x0501
			if(IsDoubleBufferEnabled){
				SetWindowLongPtr(GWL_EXSTYLE,GetWindowLongPtr(GWL_EXSTYLE)|WS_EX_COMPOSITED);
			}
			#endif
			DlgResize_Init(false,false);
			SetMsgHandled(FALSE);
			return FALSE;
		}

		void OnDestroy(){
			RemoveProp(m_hWnd,_T("IsExterior"));
			SetMsgHandled(false);
		}

		#if _WIN32_WINNT >= 0x0501
		void EnableDoubleBuffering(){
			if(m_hWnd){
				SetWindowLongPtr(GWL_EXSTYLE,GetWindowLongPtr(GWL_EXSTYLE)|WS_EX_COMPOSITED);
			} else{
				IsDoubleBufferEnabled=true;
			}
		}

		void DisableDoubleBuffering(){
			if(m_hWnd){
				SetWindowLongPtr(GWL_EXSTYLE,GetWindowLongPtr(GWL_EXSTYLE) & ~WS_EX_COMPOSITED);
			} else{
				IsDoubleBufferEnabled=false;
			}
		}
		#endif

		void AddPageFlags(UINT Flags){
			m_psp.dwFlags|=Flags;
		}

		void RemovePageFlags(UINT Flags){
			m_psp.dwFlags&=~Flags;
		}

		BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_DESTROY(OnDestroy);
			// forward WM_NOTIFY message to parent (Property Sheet) manually 
		if(uMsg==WM_NOTIFY){
			// since lParam in LPPSHNOTIFY struct isn't used, we'll use it 
			if(((LPNMHDR)lParam)->code==PSN_SETACTIVE){
				((LPPSHNOTIFY)lParam)->lParam=(LPARAM)m_hWnd;
				::SendMessage(GetParent(),uMsg,wParam,lParam);
			}
		}
		CHAIN_MSG_MAP(CPropertyPageImpl<T>)
			CHAIN_MSG_MAP(CDialogResize<T>)
			END_MSG_MAP()
	};


}; // namespace WTL 

#endif  // __ATL_RESIZABLE_PROPERTY_SHEET_H__ 

