#pragma once
#ifndef __GROWABLEARRAY_H__
#define __GROWABLEARRAY_H__
//////////////////////////////////////////////////////////////////////////
namespace HTL
{
	//////////////////////////////////////////////////////////////////////////
	//A GROWABLE ARRAY														//
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	class CGrowableArray
	{
	private:

		//////////////////////////////////////////////////////////////////////////
		typedef CGrowableArray												thisClass;
		typedef TYPE														typeClass;
		//////////////////////////////////////////////////////////////////////////

	protected:

		//////////////////////////////////////////////////////////////////////////
		TYPE*																m_pData;		// the actual array of data
		UINT64																m_nSize;        // # of elements (upperBound - 1)
		UINT64																m_nMaxSize;     // max allocated
																							//////////////////////////////////////////////////////////////////////////
		HRESULT SetSizeInternal(UINT64 nNewMaxSize);								// This version doesn't call ctor or dtor.
																					//////////////////////////////////////////////////////////////////////////

	public:

		//////////////////////////////////////////////////////////////////////////
		CGrowableArray(VOID):
			m_pData(NULL),
			m_nSize(0),
			m_nMaxSize(0)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		CGrowableArray(CONST CGrowableArray<TYPE>& a)
		{
			//////////////////////////////////////////////////////////////////////////
			for(UINT64 i=0;i<a.m_nSize;i++) {
				Add(a.m_pData[i]);
			}
			//////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////
		~CGrowableArray(VOID)
		{
			//////////////////////////////////////////////////////////////////////////
			RemoveAll();
			//////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////
		CONST TYPE& operator[](UINT64 nIndex) CONST
		{
			//////////////////////////////////////////////////////////////////////////
			return GetAt(nIndex);
			//////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////
		TYPE& operator[](UINT64 nIndex)
		{
			//////////////////////////////////////////////////////////////////////////
			return GetAt(nIndex);
			//////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////
		CGrowableArray& operator=(CONST CGrowableArray<TYPE>& a)
		{
			//////////////////////////////////////////////////////////////////////////
			if(this==&a) {
				return *this;
			}
			//////////////////////////////////////////////////////////////////////////
			RemoveAll();
			//////////////////////////////////////////////////////////////////////////
			for(UINT64 i=0;i<a.m_nSize;i++) {
				Add(a.m_pData[i]);
			}
			//////////////////////////////////////////////////////////////////////////
			return *this;
			//////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////
		HRESULT SetSize(UINT64 nNewMaxSize);
		HRESULT Add(CONST TYPE& value);
		HRESULT Insert(UINT64 nIndex,CONST TYPE& value);
		HRESULT SetAt(UINT64 nIndex,CONST TYPE& value);
		//////////////////////////////////////////////////////////////////////////
		TYPE& GetAt(UINT64 nIndex) CONST
		{
			ATLASSERT(nIndex>=0&&nIndex<m_nSize);
			return m_pData[nIndex];
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 GetSize(VOID) CONST
		{
			return m_nSize;
		}
		//////////////////////////////////////////////////////////////////////////
		TYPE* GetData(VOID)
		{
			return m_pData;
		}
		//////////////////////////////////////////////////////////////////////////
		BOOL Contains(CONST TYPE& value)
		{
			return (-1!=IndexOf(value));
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 IndexOf(CONST TYPE& value)
		{
			return (m_nSize>0)?IndexOf(value,0,m_nSize):-1;
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 IndexOf(CONST TYPE& value,UINT64 iStart)
		{
			return IndexOf(value,iStart,m_nSize-iStart);
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 IndexOf(CONST TYPE& value,UINT64 nIndex,UINT64 nNumElements);
		//////////////////////////////////////////////////////////////////////////
		UINT64 LastIndexOf(CONST TYPE& value)
		{
			return (m_nSize>0)?LastIndexOf(value,m_nSize-1,m_nSize):-1;
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 LastIndexOf(CONST TYPE& value,UINT64 nIndex)
		{
			return LastIndexOf(value,nIndex,nIndex+1);
		}
		//////////////////////////////////////////////////////////////////////////
		UINT64 LastIndexOf(CONST TYPE& value,UINT64 nIndex,UINT64 nNumElements);
		//////////////////////////////////////////////////////////////////////////
		HRESULT Remove(UINT64 nIndex);
		//////////////////////////////////////////////////////////////////////////
		VOID RemoveAll(VOID)
		{
			SetSize(0);
		}
		//////////////////////////////////////////////////////////////////////////
		VOID Reset(VOID)
		{
			m_nSize=0;
		}
		//////////////////////////////////////////////////////////////////////////

	};
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::SetSizeInternal(UINT64 nNewMaxSize)
	{
		//////////////////////////////////////////////////////////////////////////
		if(nNewMaxSize<0||(nNewMaxSize>INT_MAX/sizeof(TYPE)))
		{
			ATLASSERT(FALSE);
			return E_INVALIDARG;
		}
		//////////////////////////////////////////////////////////////////////////
		if(nNewMaxSize==0)
		{
			// Shrink to 0 size & cleanup
			if(m_pData)
			{
				free(m_pData);
				m_pData=NULL;
			}

			m_nMaxSize=0;
			m_nSize=0;
		} else if(m_pData==NULL||nNewMaxSize>m_nMaxSize){

			// Grow array
			UINT64 nGrowBy=(m_nMaxSize==0)?16:m_nMaxSize;

			// Limit nGrowBy to keep m_nMaxSize less than INT_MAX
			if((UINT)m_nMaxSize+(UINT)nGrowBy>(UINT)INT_MAX)
				nGrowBy=INT_MAX-m_nMaxSize;

			nNewMaxSize=__max(nNewMaxSize,m_nMaxSize+nGrowBy);

			// Verify that (nNewMaxSize * sizeof(TYPE)) is not greater than UINT_MAX or the realloc will overrun
			if(sizeof(TYPE)>UINT_MAX/(UINT)nNewMaxSize)
				return E_INVALIDARG;

			TYPE* pDataNew=(TYPE*)realloc(m_pData,nNewMaxSize*sizeof(TYPE));
			if(pDataNew==NULL)
				return E_OUTOFMEMORY;

			m_pData=pDataNew;
			m_nMaxSize=nNewMaxSize;
		}

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::SetSize(UINT64 nNewMaxSize)
	{
		UINT64 nOldSize=m_nSize;

		if(nOldSize>nNewMaxSize)
		{
			ATLASSERT(m_pData);
			if(m_pData)
			{
				// Removing elements. Call dtor.

				for(UINT64 i=nNewMaxSize; i<nOldSize; ++i)
					m_pData[i].~TYPE();
			}
		}

		// Adjust buffer.  Note that there's no need to check for error
		// since if it happens, nOldSize == nNewMaxSize will be TRUE.)
		HRESULT hr=SetSizeInternal(nNewMaxSize);

		if(nOldSize<nNewMaxSize)
		{
			ATLASSERT(m_pData);
			if(m_pData)
			{
				// Adding elements. Call ctor.

				for(UINT64 i=nOldSize; i<nNewMaxSize; ++i)
					::new (&m_pData[i]) TYPE;
			}
		}

		return hr;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::Add(CONST TYPE& value)
	{
		HRESULT hr;
		if(FAILED(hr=SetSizeInternal(m_nSize+1)))
			return hr;

		ATLASSERT(m_pData!=NULL);

		// Construct the new element
		::new (&m_pData[m_nSize]) TYPE;

		// Assign
		m_pData[m_nSize]=value;
		++m_nSize;

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::Insert(UINT64 nIndex,CONST TYPE& value)
	{
		HRESULT hr;

		// Validate index
		if(nIndex < 0||
		   nIndex > m_nSize)
		{
			ATLASSERT(FALSE);
			return E_INVALIDARG;
		}

		// Prepare the buffer
		if(FAILED(hr=SetSizeInternal(m_nSize+1)))
			return hr;

		// Shift the array
		MoveMemory(&m_pData[nIndex+1],&m_pData[nIndex],sizeof(TYPE) * (m_nSize-nIndex));

		// Construct the new element
		::new (&m_pData[nIndex]) TYPE;

		// Set the value and increase the size
		m_pData[nIndex]=value;
		++m_nSize;

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::SetAt(UINT64 nIndex,CONST TYPE& value)
	{
		// Validate arguments
		if(nIndex<0||
		   nIndex>=m_nSize)
		{
			ATLASSERT(FALSE);
			return E_INVALIDARG;
		}

		m_pData[nIndex]=value;
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Searches for the specified value and returns the index of the first occurrence
	// within the section of the data array that extends from iStart and contains the 
	// specified number of elements. Returns -1 if value is not found within the given 
	// section.
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	UINT64 CGrowableArray<TYPE>::IndexOf(CONST TYPE& value,UINT64 iStart,UINT64 nNumElements)
	{
		// Validate arguments
		if(iStart<0||
		   iStart>=m_nSize||
		   nNumElements<0||
		   iStart+nNumElements>m_nSize)
		{
			ATLASSERT(FALSE);
			return -1;
		}

		// Search
		for(UINT64 i=iStart; i<(iStart+nNumElements); i++)
		{
			if(value==m_pData[i])
				return i;
		}

		// Not found
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////
	// Searches for the specified value and returns the index of the last occurrence
	// within the section of the data array that contains the specified number of elements
	// and ends at iEnd. Returns -1 if value is not found within the given section.
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	UINT64 CGrowableArray<TYPE>::LastIndexOf(CONST TYPE& value,UINT64 iEnd,UINT64 nNumElements)
	{
		// Validate arguments
		if(iEnd<0||
		   iEnd>=m_nSize||
		   nNumElements<0||
		   iEnd-nNumElements<0)
		{
			ATLASSERT(FALSE);
			return -1;
		}

		// Search
		for(UINT64 i=iEnd; i>(iEnd-nNumElements); i--)
		{
			if(value==m_pData[i])
				return i;
		}

		// Not found
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TYPE>
	HRESULT CGrowableArray<TYPE>::Remove(UINT64 nIndex)
	{
		if(nIndex<0||
		   nIndex>=m_nSize)
		{
			ATLASSERT(FALSE);
			return E_INVALIDARG;
		}

		// Destruct the element to be removed
		m_pData[nIndex].~TYPE();

		// Compact the array and decrease the size
		MoveMemory(&m_pData[nIndex],&m_pData[nIndex+1],sizeof(TYPE) * (m_nSize-(nIndex+1)));
		--m_nSize;

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////

}//NAMESPACE HTL
 //////////////////////////////////////////////////////////////////////////
#endif//__GROWABLEARRAY_H__
