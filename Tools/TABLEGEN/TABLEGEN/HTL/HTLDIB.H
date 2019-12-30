#ifndef __ATLDIB_H__
#define __ATLDIB_H__

/////////////////////////////////////////////////////////////////////////////
// DIB (Device Independant Bitmap) class
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CDib

class CDib
{
public:
   LPVOID m_hDib;
   BITMAPINFOHEADER m_bi;
   DWORD m_dwLineWidth;
   DWORD m_dwPixelWidth;
   WORD m_nColors;

   CDib() : m_hDib(NULL), m_nColors(0), m_dwLineWidth(0UL), m_dwPixelWidth(0UL)
   {
      ::ZeroMemory(&m_bi, sizeof(m_bi));
   }
   ~CDib()
   {
      if( !IsEmpty() ) DeleteObject();
   }
   CDib& operator=(const CDib& src)
   {
      src.CopyTo(this);
      return *this;
   }
   void DeleteObject()
   {
      ATLASSERT(!IsEmpty());
      free(m_hDib);
      m_hDib = NULL;
   }
   BOOL IsEmpty() const
   {
      return m_hDib==NULL;
   }
   BOOL CopyTo(CDib *pDst) const
   {
      ATLASSERT(pDst);
      if( !pDst->IsEmpty() ) pDst->DeleteObject();
      DWORD dwSize = GetByteSize();
      pDst->m_hDib = malloc(dwSize); // Allocate memory block to store our bitmap
      ATLASSERT(pDst->m_hDib);
      if( pDst->m_hDib==NULL ) return FALSE;
      ::CopyMemory(&pDst->m_bi, &m_bi, sizeof(m_bi));
      ::CopyMemory(pDst->m_hDib, m_hDib, dwSize);
      pDst->m_dwLineWidth = m_dwLineWidth;
      pDst->m_dwPixelWidth = m_dwPixelWidth;
      pDst->m_nColors = m_nColors;
      return TRUE;
   }
   BOOL LoadBitmap(LPCTSTR lpszPathName)
   {
      ATLASSERT(!::IsBadStringPtr(lpszPathName,-1));
      HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL, lpszPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
      ATLASSERT(hBitmap);
      if( hBitmap==NULL ) return FALSE;
      BOOL res = Create(hBitmap);
      ::DeleteObject(hBitmap);
      return res;
   }
   BOOL LoadBitmap(UINT nRes)
   {
      HBITMAP hBitmap = (HBITMAP) ::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(nRes), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
      ATLASSERT(hBitmap);
      if( hBitmap==NULL ) return FALSE;
      BOOL res = Create(hBitmap);
      ::DeleteObject(hBitmap);
      return res;
   }
   BOOL Create(HBITMAP hBitmap)
   {
      ATLASSERT(::GetObjectType(hBitmap)==OBJ_BITMAP);
      BITMAP bm;
      ::GetObject(hBitmap, sizeof(BITMAP), &bm);
      if( Create(bm.bmWidth, bm.bmHeight, 24)==FALSE ) return FALSE;
      CWindowDC dc(HWND_DESKTOP); // Need a DC; any DC will do!
      int lines = ::GetDIBits(dc, hBitmap, 0, bm.bmHeight, GetBits(), (BITMAPINFO*) &m_bi, DIB_RGB_COLORS);
      ATLASSERT(lines==bm.bmHeight);
      if( lines != bm.bmHeight ) return FALSE;
      return TRUE;
   }
   BOOL Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount)
   {
      #define WIDTHBYTES(bits)  (((bits) + 31) / 32 * 4)
      if( !IsEmpty() ) DeleteObject();

      // Make sure bits per pixel is valid
      if( wBitCount<=1 ) wBitCount = 1;
      else if( wBitCount<=4 ) wBitCount = 4;
      else if( wBitCount<=8 ) wBitCount = 8;
      else wBitCount = 24;
      switch( wBitCount ){
      case 1:
         m_nColors = 2;
         break;
      case 4:
         m_nColors = 16;
         break;
      case 8:
         m_nColors = 256;
         break;
      default:
         // A 24/32 bit DIB has no color table
         m_nColors = 0;
      }
      m_dwLineWidth = WIDTHBYTES(wBitCount * dwWidth);
      m_dwPixelWidth = wBitCount / 8;

      // Initialize BITMAPINFOHEADER
      m_bi.biSize = sizeof(BITMAPINFOHEADER);
      m_bi.biWidth = dwWidth;         // Fill in width from parameter
      m_bi.biHeight = dwHeight;       // Fill in height from parameter
      m_bi.biPlanes = 1;              // Must be 1
      m_bi.biBitCount = wBitCount;
      m_bi.biCompression = BI_RGB;    
      m_bi.biSizeImage = m_dwLineWidth * dwHeight;
      m_bi.biXPelsPerMeter = 0;
      m_bi.biYPelsPerMeter = 0;
      m_bi.biClrUsed = 0;
      m_bi.biClrImportant = 0;

      // Calculate size of memory block required to store the DIB.  This
      // block should be big enough to hold the BITMAPINFOHEADER, the color
      // table, and the bits
      m_hDib = malloc(GetByteSize()); // Allocate memory block to store our bitmap
      if( m_hDib==NULL ) return FALSE;

      // Use our bitmap info structure to fill in first part of
      // our DIB with the BITMAPINFOHEADER
      LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER) m_hDib;
      *lpbi = m_bi;
      return TRUE;
   }
   DWORD GetByteSize() const
   {
      return m_bi.biSize + m_bi.biSizeImage + GetPaletteSize();
   }
   inline LPBYTE GetBits() const
   {
      ATLASSERT(!IsEmpty());
      return( (BYTE*) m_hDib + ( *(LPDWORD) m_hDib ) + GetPaletteSize() ); 
   }
   inline DWORD GetWidth() const
   {
      ATLASSERT(!IsEmpty());
      return m_bi.biWidth;
   }
   inline DWORD GetHeight() const
   {
      ATLASSERT(!IsEmpty());
      return m_bi.biHeight;
   }
   inline DWORD GetLineWidth() const
   {
      ATLASSERT(!IsEmpty());
      return m_dwLineWidth;
   }
   inline DWORD GetPixelWidth() const
   {
      ATLASSERT(!IsEmpty());
      return m_dwPixelWidth;
   }
   WORD GetColorCount() const
   {
      return m_nColors;
   }
   WORD GetPaletteSize() const
   {
      WORD wNumColors = GetColorCount();
      if( m_bi.biSize==sizeof(BITMAPCOREHEADER) )
         return (WORD) (wNumColors * sizeof(RGBTRIPLE));
      else
         return (WORD) (wNumColors * sizeof(RGBQUAD));
   }
   WORD GetBitCount() const
   {
      ATLASSERT(!IsEmpty());
      return m_bi.biBitCount;
   }

   COLORREF GetPixel(int x, int y) const
   {
      ATLASSERT(!IsEmpty());
      if( x<0 || x>=m_bi.biWidth || y<0 || y>=m_bi.biHeight ) return RGB(0,0,0);
      LPBYTE lp = GetBits() + (y * m_dwLineWidth) + (x * m_dwPixelWidth);
      return RGB(lp[0], lp[1], lp[2]);
   }
   void SetPixel(int x, int y, COLORREF rgb)
   {
      ATLASSERT(!IsEmpty());
      if( x<0 || x>=m_bi.biWidth || y<0 || y>=m_bi.biHeight ) return;
      LPBYTE lp = GetBits() + (y * m_dwLineWidth) + (x * m_dwPixelWidth);
      *lp++ = GetRValue(rgb);
      *lp++ = GetGValue(rgb);
      *lp++ = GetBValue(rgb);
   }

   BOOL Draw(HDC hDC, long xoffset, long yoffset) const
   {
      if( (m_hDib==NULL) || (hDC==NULL) ) return FALSE;
      ::SetStretchBltMode(hDC,COLORONCOLOR);   
      ::SetDIBitsToDevice(hDC, xoffset, yoffset,
         m_bi.biWidth, m_bi.biHeight, 0, 0, 
         0, m_bi.biHeight, GetBits(),
         (BITMAPINFO*) m_hDib, DIB_RGB_COLORS);
      return TRUE;
   }
   BOOL Stretch(HDC hDC, long xoffset, long yoffset, long xsize, long ysize) const
   {
      if( (m_hDib == NULL) || (hDC == NULL) ) return FALSE;
      ::SetStretchBltMode(hDC,COLORONCOLOR);   
      ::StretchDIBits(hDC, xoffset, yoffset,
         xsize, ysize, 0, 0, m_bi.biWidth, m_bi.biHeight,
         GetBits(), (BITMAPINFO*) m_hDib, DIB_RGB_COLORS, SRCCOPY);
      return TRUE;
   }
   BOOL Stretch(HDC hDC, RECT rc) const
   {
      return Stretch(hDC, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
   }

};

typedef CDib CDib24;


#endif // __ATLDIB_H__
