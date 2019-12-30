#ifndef __ATLDIBFILTERS_H__
#define __ATLDIBFILTERS_H__

/////////////////////////////////////////////////////////////////////////////
// 24 bit DIB image filters
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

// Include my DIB class
#include "atldib.h"


/////////////////////////////////////////////////////////////////////////////
// 24 bit image filters

class CFillImageFilter
{
public:
   static BOOL Process(CDib24* pDst, COLORREF clr, LPRECT prc = NULL)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      RECT rcTemp;
      if( prc==NULL ) {
         ::SetRect(&rcTemp, 0,0, pDst->GetWidth()-1, pDst->GetHeight()-1);
         prc = &rcTemp;
      }
      if( (DWORD)prc->right > pDst->GetWidth() ) return FALSE;
      if( (DWORD)prc->top > pDst->GetHeight() ) return FALSE;
      LPBYTE pStream = pDst->GetBits() + (pDst->GetLineWidth() * prc->top) + (pDst->GetPixelWidth() * prc->left);
      DWORD cx = min( (DWORD)(prc->right-prc->left), pDst->GetWidth() );
      DWORD cy = min( (DWORD)(prc->bottom-prc->top), pDst->GetHeight() );
      BYTE r = GetRValue(clr);
      BYTE g = GetGValue(clr);
      BYTE b = GetBValue(clr);
      for( DWORD y=0; y<cy; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<cx; x++ ) {
            *p++ = b;
            *p++ = g;
            *p++ = r;
         }
         pStream += pDst->GetLineWidth();
      }
      return TRUE;
   }
};

class CStrippleImageFilter
{
public:
   static BOOL Process(CDib24* pDst, COLORREF clr, DWORD divisor, LPRECT prc = NULL)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      RECT rcTemp;
      if( prc==NULL ) {
         ::SetRect(&rcTemp, 0,0, pDst->GetWidth()-1, pDst->GetHeight()-1);
         prc = &rcTemp;
      }
      LPBYTE pStream = pDst->GetBits() + (pDst->GetLineWidth() * prc->top) + (pDst->GetPixelWidth() * prc->left);
      DWORD nPixelWidth = pDst->GetPixelWidth();
      DWORD nLineWidth = pDst->GetLineWidth();
      DWORD cx = min( (DWORD)(prc->right-prc->left), pDst->GetWidth() );
      DWORD cy = min( (DWORD)(prc->bottom-prc->top), pDst->GetHeight() );
      BYTE r = GetRValue(clr);
      BYTE g = GetGValue(clr);
      BYTE b = GetBValue(clr);
      for( DWORD y=0; y<cy; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<cx; x++ ) {
            if( (x % divisor)==0 && (y % divisor)==0 ) {
               *p = b;
               *p = g;
               *p = r;
            }
            p += nPixelWidth;
         }
         pStream += nLineWidth;
      }
      return TRUE;
   }
};

class CGreyScaleImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pStream = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<nWidth; x++ ) {
            BYTE c = (BYTE) (*(p+0)/3 + *(p+1)/3 + *(p+2)/3);
            *p++ = c;
            *p++ = c;
            *p++ = c;
         }
         pStream += nLine;
      }
      return TRUE;
   }
};

class CBrightenImageFilter
{
public:
   static BOOL Process(CDib24* pDst, short percent)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pStream = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      percent += 100;
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<nWidth; x++ ) {
            for( int z=0; z<3; z++ ) {
               WORD c = *p;
               c = (WORD) (c * percent / 100);
               *p++ = (BYTE) max(0, min(c, 255));
            }
         }
         pStream += nLine;
      }
      return TRUE;
   }
};

class CFadeImageFilter
{
public:
   static BOOL Process(CDib24* pDst, CDib24* pSrc, short percent)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pSrcBits = pSrc->GetBits();
      LPBYTE pDstBits = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      _ASSERTE(pSrc->GetWidth()==pDst->GetWidth());
      _ASSERTE(pSrc->GetHeight()==pDst->GetHeight());
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE pS = pSrcBits;
         LPBYTE pD = pDstBits;
         for( DWORD x=0; x<nWidth; x++ ) {
            for( int z=0; z<3; z++ ) {
               long c = *pS++ - *pD;
               c = (c * percent / 100);
               *pD++ = (BYTE)(*pD + c);
            }
         }
         pSrcBits += nLine;
         pDstBits += nLine;
      }
      return TRUE;
   }
};

class CColorizeImageFilter
{
public:
   static BOOL Process(CDib24* pDst, COLORREF clr)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pStream = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      //
      float r = GetRValue(clr);
      float g = GetGValue(clr);
      float b = GetBValue(clr);
      //
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<nWidth; x++ ) {
            float mid = (*(p+0)*20.0f + *(p+1)*50.0f + *(p+2)*30.0f) / 100.f;
            *p++ = (BYTE) (b * mid);
            *p++ = (BYTE) (g * mid);
            *p++ = (BYTE) (r * mid);
         }
         pStream += nLine;
      }
      return TRUE;
   }
};

class CNegativeImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pStream = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<nWidth; x++ ) {
            *p++ = (BYTE) (255-*p);
            *p++ = (BYTE) (255-*p);
            *p++ = (BYTE) (255-*p);
         }
         pStream += nLine;
      }
      return TRUE;
   }
};

class CCopyImageFilter
{
public:
   static BOOL Process(CDib24* pDst, CDib24* pSrc, POINT& ptPos)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pSrcBits = pSrc->GetBits();
      LPBYTE pDstBits = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      //
      pDstBits += (pDst->GetLineWidth()*ptPos.y) + (ptPos.x*pDst->GetPixelWidth());
      DWORD cx = min( nWidth - ptPos.x, pSrc->GetWidth() );
      DWORD cy = min( nHeight - ptPos.y, pSrc->GetHeight() );
      //
      for( DWORD y=0; y<cy; y++ ) {
         ::CopyMemory(pDstBits, pSrcBits, cx*3);
         pSrcBits += pSrc->GetLineWidth();
         pDstBits += pDst->GetLineWidth();
      }
      return TRUE;
   }
};

class CCopyMaskedImageFilter
{
public:
   static BOOL Process(CDib24* pDst, CDib24* pSrc, POINT& ptPos, COLORREF clrBack)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pSrcBits = pSrc->GetBits();
      LPBYTE pDstBits = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      //
      pDstBits += (pDst->GetLineWidth()*ptPos.y) + (ptPos.x*pDst->GetPixelWidth());
      DWORD cx = min( nWidth - ptPos.x, pSrc->GetWidth() );
      DWORD cy = min( nHeight - ptPos.y, pSrc->GetHeight() );
      //
      BYTE r = GetRValue(clrBack);
      BYTE g = GetGValue(clrBack);
      BYTE b = GetBValue(clrBack);
      //
      for( DWORD y=0; y<cy; y++ ) {
         LPBYTE pS = pSrcBits;
         LPBYTE pD = pDstBits;
         for( DWORD x=0; x<cx; x++ ) {
            if( *(pS+0)==r &&
                *(pS+1)==g &&
                *(pS+2)==b ) 
            {
               pS += 3;
               pD += 3;
            }
            else {
               *pD++ = *pS++;
               *pD++ = *pS++;
               *pD++ = *pS++;
            }
         }
         pSrcBits += pSrc->GetLineWidth();
         pDstBits += pDst->GetLineWidth();
      }
      return TRUE;
   }
};

class CReplaceColourFilter
{
public:
   static BOOL Process(CDib24* pDst, COLORREF clrSearch, COLORREF clrReplacement)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      LPBYTE pDstBits = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      //
      BYTE rSrc = GetRValue(clrSearch);
      BYTE gSrc = GetGValue(clrSearch);
      BYTE bSrc = GetBValue(clrSearch);
      BYTE rRepl = GetRValue(clrReplacement);
      BYTE gRepl = GetGValue(clrReplacement);
      BYTE bRepl = GetBValue(clrReplacement);
      //
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE pD = pDstBits;
         for( DWORD x=0; x<nWidth; x++ ) {
            if( *(pD+0)==bSrc &&
                *(pD+1)==gSrc &&
                *(pD+2)==rSrc ) 
            {
               *pD++ = bRepl;
               *pD++ = gRepl;
               *pD++ = rRepl;
            }
            else {
               pD += 3;
            }
         }
         pDstBits += pDst->GetLineWidth();
      }
      return TRUE;
   }
};

class CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst, float* matrix)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      _ASSERTE(!::IsBadReadPtr(matrix, sizeof(float)*9));
      // Create a copy of the image
      CDib24 dibTemp;
      if( pDst->CopyTo(&dibTemp)==FALSE ) return FALSE;
      // Get pointers
      LPBYTE pSrcBits = pDst->GetBits();
      LPBYTE pDstBits = dibTemp.GetBits();
      _ASSERTE(pSrcBits);
      _ASSERTE(pDstBits);
      int nWidth = pDst->GetWidth();
      int nHeight = pDst->GetHeight();
      DWORD nLineWidth = pDst->GetLineWidth();
      DWORD nPixelWidth = pDst->GetPixelWidth();
      _ASSERTE(nLineWidth==dibTemp.GetLineWidth());
      //
      pSrcBits += nLineWidth + nPixelWidth;
      pDstBits += nLineWidth + nPixelWidth;
      //
      float sum;
      for( int y=1; y<nHeight-1; y++ ) {
         LPBYTE p = pDstBits;
         LPBYTE s = pSrcBits;
         for( int x=1; x<nWidth-1; x++ ) {
            for( int z=0; z<3; z++ ) {
               //
               //         0 3 6
               // color = 1 4 7
               //         2 5 8
               //
               sum = 0.0f;
               sum += matrix[0] * (*(s-nLineWidth-nPixelWidth));
               sum += matrix[1] * (*(s-nPixelWidth));
               sum += matrix[2] * (*(s+nLineWidth-nPixelWidth));
               sum += matrix[3] * (*(s-nLineWidth));
               sum += matrix[4] * (*s);
               sum += matrix[5] * (*(s+nLineWidth));
               sum += matrix[6] * (*(s-nLineWidth+nPixelWidth));
               sum += matrix[7] * (*(s+nPixelWidth));
               sum += matrix[8] * (*(s+nLineWidth+nPixelWidth));
               int i = (int) sum;
               *p++ = (BYTE) max(0,min(i,255));
               s++;
            }
         }
         pDstBits += nLineWidth;
         pSrcBits += nLineWidth;
      }
      dibTemp.CopyTo(pDst);
      return TRUE;
   }
};

class CBlurImageFilter : public CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      const float ninth = 1.0f / 9.0f;
      float matrix[] = {
        ninth, ninth, ninth,
        ninth, ninth, ninth,
        ninth, ninth, ninth
      };
      return CMatrixImageFilter::Process(pDst, matrix);
   }
};

class CEdgeImageFilter : public CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      float matrix[] = {
        0.0f, -1.0f, 0.0f,
        -1.0f, 4.0f, -1.0f,
        0.0f, -1.0f, 0.0f
      };
      return CMatrixImageFilter::Process(pDst, matrix);
   }
};

class CSharpenImageFilter : public CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      float matrix[] = {
        0.0f, -1.0f, 0.0f,
        -1.0f, 5.0f, -1.0f,
        0.0f, -1.0f, 0.0f
      };
      return CMatrixImageFilter::Process(pDst, matrix);
   }
};

class CGaussImageFilter : public CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      float matrix[] = {
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
        2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f
      };
      return CMatrixImageFilter::Process(pDst, matrix);
   }
};

class CInvertGaussImageFilter : public CMatrixImageFilter
{
public:
   static BOOL Process(CDib24* pDst)
   {
      float matrix[] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -2.0f, -1.0f,
        -1.0f, -1.0f, -1.0f
      };
      return CMatrixImageFilter::Process(pDst, matrix);
   }
};

class CPosterizeImageFilter
{
public:
   static BOOL Process(CDib24* pDst, float threshold)
   {
      _ASSERTE(pDst->GetBitCount()==24);
      _ASSERTE(threshold>0.0f && threshold<1.0f);
      LPBYTE pStream = pDst->GetBits();
      DWORD nWidth = pDst->GetWidth();
      DWORD nHeight = pDst->GetHeight();
      DWORD nLine = pDst->GetLineWidth();
      for( DWORD y=0; y<nHeight; y++ ) {
         LPBYTE p = pStream;
         for( DWORD x=0; x<nWidth; x++ ) {
            for( int z=0; z<3; z++ ) {
               float value = *p / 256.0f;
               if( value > 0) {
                  if( value > threshold ){
                     if( value > 2.0f * threshold ) {
                        if( value > 3.0f * threshold ) {
                           *p = (BYTE) (4.0f * threshold);
                        }
                        else {
                           *p = (BYTE) (3.0f * threshold * 256.0f);
                        }
                     }
                     else {
                        *p = (BYTE) (2.0f * threshold * 256.0f);
                     }
                  }
                  else {
                     *p = (BYTE) (threshold * 256.0f);
                  }
               }
               else {
                  *p = 0;
               }               
               p++;
            }
         }
         pStream += nLine;
      }
      return TRUE;
   }
};


/////////////////////////////////////////////////////////////////////////////
// RGB and HSL conversions

// Code from ing.davide.pizzolato@libero.it - available
// at www.codeproject.com

#pragma warning( disable : 4244 ) // conversion .. loss of data

#define  HSLMAX   240   /* H,L, and S vary over 0-HSLMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HSLMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HSLMAX must each fit in a byte. */
#define UNDEFINED_HUE (HSLMAX*2/3)

inline RGBQUAD RGB2RGBQUAD(COLORREF cr)
{
   RGBQUAD c;
   c.rgbRed = GetRValue(cr);   // Get R, G, and B out of DWORD 
   c.rgbGreen = GetGValue(cr);
   c.rgbBlue = GetBValue(cr);
   c.rgbReserved = 0;
   return c;
}

inline COLORREF RGBQUAD2RGB(RGBQUAD c)
{
   return RGB(c.rgbRed,c.rgbGreen,c.rgbBlue);
}

inline RGBQUAD RGBtoHSL(RGBQUAD lRGBColor)
{
   BYTE H,L,S;                  // output HSL values
   BYTE cMax,cMin;              // max and min RGB values
   WORD Rdelta,Gdelta,Bdelta;   // intermediate value: % of spread from max

   BYTE R = lRGBColor.rgbRed;   // get R, G, and B out of DWORD
   BYTE G = lRGBColor.rgbGreen;
   BYTE B = lRGBColor.rgbBlue;

   cMax = max( max(R,G), B);    // calculate lightness
   cMin = min( min(R,G), B);
   L = (((cMax+cMin)*HSLMAX)+RGBMAX)/(2*RGBMAX);

   if (cMax==cMin){          // r=g=b --> achromatic case
      S = 0;                 // saturation
      H = UNDEFINED_HUE;     // hue
   } 
   else {                    // chromatic case
      if (L <= (HSLMAX/2))   // saturation
         S = (((cMax-cMin)*HSLMAX)+((cMax+cMin)/2))/(cMax+cMin);
      else
         S = (((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2))/(2*RGBMAX-cMax-cMin);
      /* hue */
      Rdelta = ( ((cMax-R)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Gdelta = ( ((cMax-G)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Bdelta = ( ((cMax-B)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);

      if (R == cMax)
         H = Bdelta - Gdelta;
      else if (G == cMax)
         H = (HSLMAX/3) + Rdelta - Bdelta;
      else /* B == cMax */
         H = ((2*HSLMAX)/3) + Gdelta - Rdelta;

      if (H < 0) H += HSLMAX;
      if (H > HSLMAX) H -= HSLMAX;
   }
   RGBQUAD hsl={L,S,H,0};
   return hsl;
}

inline WORD HueToRGB(WORD n1, WORD n2, WORD hue)
{ 
   // Range check: note values passed add/subtract thirds of range
   if (hue < 0) hue += HSLMAX;
   if (hue > HSLMAX) hue -= HSLMAX;

   // Return r,g, or b value from this tridrant
   if (hue < (HSLMAX/6))
      return (WORD)( n1 + (((n2-n1)*hue+(HSLMAX/12))/(HSLMAX/6)) );
   if (hue < (HSLMAX/2))
      return ( n2 );
   if (hue < ((HSLMAX*2)/3))
      return (WORD)( n1 + (((n2-n1)*(((HSLMAX*2)/3)-hue)+(HSLMAX/12))/(HSLMAX/6))); 
   else
      return n1;
} 

inline RGBQUAD HSLtoRGB(RGBQUAD lHSLColor)
{ 
   BYTE R,G,B;                 // RGB component values
   WORD Magic1,Magic2;         // calculated magic numbers (really!)

   WORD hue = lHSLColor.rgbRed;     // get H, S, and L out of DWORD
   WORD sat = lHSLColor.rgbGreen;
   WORD lum = lHSLColor.rgbBlue;

   if (sat == 0) {            /* achromatic case */
      R = G = B = (lum*RGBMAX)/HSLMAX;
   } 
   else {                     /* chromatic case */
      // Set up magic numbers
      if (lum <= (HSLMAX/2))
         Magic2 = (WORD)((lum*(HSLMAX + sat) + (HSLMAX/2))/HSLMAX);
      else
         Magic2 = (WORD)(lum + sat - ((lum*sat) + (HSLMAX/2))/HSLMAX);
      Magic1 = 2*lum-Magic2;

      // get RGB, change units from HSLMAX to RGBMAX
      R = (HueToRGB(Magic1,Magic2,(WORD)(hue+(HSLMAX/3)))*RGBMAX+(HSLMAX/2))/HSLMAX; 
      G = (HueToRGB(Magic1,Magic2,hue)*RGBMAX + (HSLMAX/2)) / HSLMAX;
      B = (HueToRGB(Magic1,Magic2,(WORD)(hue-(HSLMAX/3)))*RGBMAX+(HSLMAX/2))/HSLMAX;
   }
   RGBQUAD rgb={ B, G, R, 0 };
   return rgb;
}

inline RGBQUAD HSLtoRGB(COLORREF cHSLColor)
{
   return HSLtoRGB(RGB2RGBQUAD(cHSLColor));
}

#pragma warning( default : 4244 ) // conversion .. loss of data


#endif // __ATLDIBFILTERS_H__

