#pragma once
#ifndef __HTLREGISTRY_H__
#define __HTLREGISTRY_H__
//////////////////////////////////////////////////////////////////////////
namespace HTL{
//////////////////////////////////////////////////////////////////////////
class CAppSettings{

public:

	// Must be called once to set the name of the key under HKLM\\Software
	// or HKCU\\Software.
	// Normally this will be of the form "[company name]\\[app name]"
	void SetRegKey(LPCTSTR pszKey){
		m_strRegKeyName=_T("Software\\");
		m_strRegKeyName+=pszKey;
	}

	CString GetUserProfileString(LPCTSTR lpszSection,LPCTSTR lpszEntry,LPCTSTR lpszDefault=NULL){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_CURRENT_USER,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwcbNeeded=0;
			if(key.QueryValue((LPTSTR)NULL,lpszEntry,&dwcbNeeded)==ERROR_SUCCESS){
				CString strVal;
				LPTSTR pszVal=strVal.GetBuffer(dwcbNeeded);

				if(key.QueryValue(pszVal,lpszEntry,&dwcbNeeded)==ERROR_SUCCESS){
					strVal.ReleaseBuffer();
					key.Close();
					return strVal;
				} else{
					strVal.ReleaseBuffer(0);
				}
			}
		}

		key.Close();
		return CString(lpszDefault);
	}

	CString GetMachineProfileString(LPCTSTR lpszSection,LPCTSTR lpszEntry,LPCTSTR lpszDefault=NULL){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_LOCAL_MACHINE,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwcbNeeded=0;
			if(key.QueryValue((LPTSTR)NULL,lpszEntry,&dwcbNeeded)==ERROR_SUCCESS){
				CString strVal;
				LPTSTR pszVal=strVal.GetBuffer(dwcbNeeded);

				if(key.QueryValue(pszVal,lpszEntry,&dwcbNeeded)==ERROR_SUCCESS){
					strVal.ReleaseBuffer();
					key.Close();
					return strVal;
				} else{
					strVal.ReleaseBuffer(0);
				}
			}
		}

		key.Close();
		return CString(lpszDefault);
	}

	DWORD GetUserProfileDword(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwDefault=0){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_CURRENT_USER,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwVal;
			if(key.QueryValue(dwVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return dwVal;
			}
		}

		key.Close();
		return dwDefault;;
	}

	DWORD GetMachineProfileDword(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwDefault=0){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_LOCAL_MACHINE,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwVal;
			if(key.QueryValue(dwVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return dwVal;
			}
		}

		key.Close();
		return dwDefault;;
	}

	template <class T> BOOL GetMachineProfileStruct(LPCTSTR lpszSection,LPCTSTR lpszEntry,T& obj){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_LOCAL_MACHINE,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwcbSize=sizeof(T);
			if(::RegQueryValueEx(key,lpszEntry,NULL,NULL,(LPBYTE)&obj,&dwcbSize)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	template <class T> BOOL GetUserProfileStruct(LPCTSTR lpszSection,LPCTSTR lpszEntry,T& obj){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Open(HKEY_CURRENT_USER,strKey,KEY_READ)==ERROR_SUCCESS){
			DWORD dwcbSize=sizeof(T);
			if(::RegQueryValueEx(key,lpszEntry,NULL,NULL,(LPBYTE)&obj,&dwcbSize)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	BOOL WriteMachineProfileString(LPCTSTR lpszSection,LPCTSTR lpszEntry,LPCTSTR lpszVal){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_LOCAL_MACHINE,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(key.SetValue(lpszVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	template <class T> BOOL WriteMachineProfileStruct(LPCTSTR lpszSection,LPCTSTR lpszEntry,T& obj){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_LOCAL_MACHINE,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(::RegSetValueEx(key,lpszEntry,0,REG_BINARY,(LPBYTE)&obj,sizeof(T))==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	template <class T> BOOL WriteUserProfileStruct(LPCTSTR lpszSection,LPCTSTR lpszEntry,T& obj){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_CURRENT_USER,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(::RegSetValueEx(key,lpszEntry,0,REG_BINARY,(LPBYTE)&obj,sizeof(T))==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	BOOL WriteMachineProfileDword(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwVal){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_LOCAL_MACHINE,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(key.SetValue(dwVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	BOOL WriteUserProfileString(LPCTSTR lpszSection,LPCTSTR lpszEntry,LPCTSTR lpszVal){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_CURRENT_USER,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(key.SetValue(lpszVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

	BOOL WriteUserProfileDword(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwVal){
		CString strKey=CatKeyAndSubkeyNames(lpszSection);
		CRegKey key;

		if(key.Create(HKEY_CURRENT_USER,strKey,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE)==ERROR_SUCCESS){
			if(key.SetValue(dwVal,lpszEntry)==ERROR_SUCCESS){
				key.Close();
				return TRUE;
			}
		}

		key.Close();
		return FALSE;
	}

private:
	CString CatKeyAndSubkeyNames(LPCTSTR lpszSection){
		ATLASSERT(!m_strRegKeyName.IsEmpty());
		CString strKey=m_strRegKeyName;

		if(lpszSection){
			strKey+=_T("\\");
			strKey+=lpszSection;
		}

		return strKey;
	}

	CString m_strRegKeyName;
};
//////////////////////////////////////////////////////////////////////////
};//namespace HTL
//////////////////////////////////////////////////////////////////////////
#endif//__HTLREGISTRY_H__
