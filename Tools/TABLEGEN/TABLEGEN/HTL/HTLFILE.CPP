#include "GRNMPCH.H"
#include "../RESOURCEDEF.H"
#include "../HTL/HTLFILE.H"
//////////////////////////////////////////////////////////////////////////
namespace HTL{

	/////////////////////////////////////////////////////////////////////////////
	// Global functions

	// Convert a pipe-separated list of filter strings into a double-null-terminted
	// string, the format you need to store in an OPENFILENAME.
	LPCTSTR PrepFilterString(CString& sFilters){
		LPTSTR p=sFilters.GetBuffer(0);
		LPCTSTR pRet=p;

		while('\0'!=*p){
			if(*p=='|')
				*p++='\0';
			else
				p=CharNext(p);
		}

		return pRet;
	}

	// Return the file system path for a given IShellItem.
	bool PathFromShellItem(IShellItem* pItem,CString& sPath){
		HRESULT hr;
		LPOLESTR pwsz=NULL;

		hr=pItem->GetDisplayName(SIGDN_FILESYSPATH,&pwsz);

		if(FAILED(hr))
			return false;

		sPath=pwsz;
		CoTaskMemFree(pwsz);
		return true;
	}

	// Convert a pipe-separated list of filter strings into a vector of
	// COMDLG_FILTERSPEC. The vector<CString> is needed to actually hold the strings
	// that the COMDLG_FILTERSPEC structs will point to.
	bool BuildFilterSpecList(_U_STRINGorID szFilterList,vector<CString>& vecsFilterParts,vector<COMDLG_FILTERSPEC>& vecFilters){
		CString sFilterList=szFilterList.m_lpstr;
		CString sToken;
		int nIdx=0;

		// Split the passed-in filter list on pipe characters (MFC-style)
		for(;;){
			sToken=sFilterList.Tokenize(_T("|"),nIdx);

			if(sToken.IsEmpty())
				break;

			vecsFilterParts.push_back(sToken);
		}

		// There should be an even number of tokens in the string
		if(vecsFilterParts.size()&1){
			ATLASSERT(0);
			vecsFilterParts.pop_back();
		}

		// Use each pair of tokens for a COMDLG_FILTERSPEC struct.
		for(vector<CString>::size_type i=0; i<vecsFilterParts.size(); i+=2){
			COMDLG_FILTERSPEC fs={vecsFilterParts[i],vecsFilterParts[i+1]};
			vecFilters.push_back(fs);
		}

		return !vecFilters.empty();
	}


};//namespace HTL