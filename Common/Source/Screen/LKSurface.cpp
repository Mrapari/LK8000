/*
 * LK8000 Tactical Flight Computer -  WWW.LK8000.IT
 * Released under GNU/GPL License v.2
 * See CREDITS.TXT file for authors and copyrights
 *
 * File:   LKSurface.cpp
 * Author: Bruno de Lacheisserie
 * 
 * Created on 17 octobre 2014
 */

#include <tchar.h>
#include <cassert>
#include <math.h>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include "LKSurface.h"
#include "MathFunctions.h"
#include "utils/2dpclip.h"
#include "utils/array_adaptor.h"

#ifndef WIN32
#warning "TODO: need to implement"
#endif

#ifdef WIN32

LKSurface::LKSurface() : _OutputDC(), _AttribDC(), _TempDC() {

}

void LKSurface::SetAttribDC(HDC hDC) {
    _AttribDC = hDC;
}

void LKSurface::SetOutputDC(HDC hDC) {
    _OutputDC = hDC;
}

void LKSurface::ReleaseAttribDC() {
    _AttribDC = NULL;
}

void LKSurface::Release() {
    if(_OutputDC) {
        ::DeleteDC(Detach());
    }
}

void LKSurface::ReleaseTempDC() {
    if (_TempDC) {
        ::DeleteDC(_TempDC);
        _TempDC = NULL;
    }
}

HDC LKSurface::GetTempDC() {
    if (!_TempDC) {
        _TempDC = ::CreateCompatibleDC(GetAttribDC());
    }
    return _TempDC;
}

bool LKSurface::Attach(HDC hDC) {
    assert(NULL == _OutputDC); // only attach once.
    assert(NULL == _AttribDC);
    if (NULL == hDC) {
        return false;
    }
    _OutputDC = hDC;

    return true;
}

HDC LKSurface::Detach() {
    HDC hDC = _OutputDC;
    ReleaseAttribDC();
    ReleaseTempDC();
    _OutputDC = NULL;
    return hDC;
}
#endif

LKSurface::~LKSurface() {
    Release();
}

LKColor LKSurface::SetTextColor(const LKColor& Color) {
#ifdef WIN32
    return LKColor(::SetTextColor(*this, Color));
#else
    return LKColor();
#endif
}

LKColor LKSurface::SetBkColor(const LKColor& Color) {
#ifdef WIN32
    return LKColor(::SetBkColor(*this, Color));
#else
    return LKColor();
#endif
}

int LKSurface::SetBkMode(int mode) {
#ifdef WIN32
    return ::SetBkMode(*this, mode);
#else
    return 0;
#endif
}

LKFont LKSurface::SelectObject(const LKFont& Font) {
#ifdef WIN32
    LKFont OldFont((HFONT)::SelectObject(*this, (HFONT) Font));
    return OldFont;
#else
    return LKFont();
#endif
}

LKBrush LKSurface::SelectObject(const LKBrush& Brush) {
#ifdef WIN32
    LKBrush OldBrush((HBRUSH)::SelectObject(*this, (HBRUSH) Brush));
    return OldBrush;
#else
    return LKBrush();
#endif
}

LKPen LKSurface::SelectObject(const LKPen& Pen) {
#ifdef WIN32
    LKPen OldPen((HPEN)::SelectObject(*this, (HPEN) Pen));
    return OldPen;
#else
    return LKPen();
#endif
}

void LKSurface::DrawMaskedBitmap(const int x, const int y, const int cx, const int cy, const LKBitmap& Bitmap, const int cxSrc, const int cySrc) {
#ifdef WIN32
    HGDIOBJ old = ::SelectObject(GetTempDC(), (HBITMAP) Bitmap);

    if (cxSrc != cx || cySrc != cy) {
        ::StretchBlt(*this, x, y, cx, cy, GetTempDC(), 0, 0, cxSrc, cySrc, SRCPAINT);
        ::StretchBlt(*this, x, y, cx, cy, GetTempDC(), cxSrc, 0, cxSrc, cySrc, SRCAND);
    } else {
        ::BitBlt(*this, x, y, cx, cy, GetTempDC(), 0, 0, SRCPAINT);
        ::BitBlt(*this, x, y, cx, cy, GetTempDC(), cxSrc, 0, SRCAND);
    }

    ::SelectObject(GetTempDC(), old);
#endif
}

void LKSurface::DrawBitmap(const int x, const int y, const int cx, const int cy, const LKBitmap& Bitmap, const int cxSrc, const int cySrc) {
#ifdef WIN32
    HGDIOBJ old = ::SelectObject(GetTempDC(), (HBITMAP) Bitmap);

    if (cxSrc != cx || cySrc != cy) {
        ::StretchBlt(*this, x, y, cx, cy, GetTempDC(), 0, 0, cxSrc, cySrc, SRCPAINT);
    } else {
        ::BitBlt(*this, x, y, cx, cy, GetTempDC(), 0, 0, SRCPAINT);
    }

    ::SelectObject(GetTempDC(), old);
#endif    
}

void LKSurface::DrawBitmap(const int x, const int y, const int cx, const int cy, const LKBitmap& Bitmap) {
#ifdef WIN32
    HGDIOBJ old = ::SelectObject(GetTempDC(), (HBITMAP) Bitmap);
    ::BitBlt(*this, x, y, cx, cy, GetTempDC(), 0, 0, SRCPAINT);
    ::SelectObject(GetTempDC(), old);
#endif    
}

void LKSurface::Polygon(const POINT *apt, int cpt) {
#ifdef WIN32
    ::Polygon(*this, apt, cpt);
#endif    
}

void LKSurface::Polygon(const POINT *apt, int cpt, const RECT& ClipRect) {
    if(cpt>=3) {
        std::vector<POINT> Clipped;
        Clipped.reserve(cpt);
        LKGeom::ClipPolygon(ClipRect, const_array_adaptor<POINT>(apt, cpt), Clipped);
        if(Clipped.size() >= 3) {
            Polygon(Clipped.data(), Clipped.size());
        }
    }
}


void LKSurface::Polyline(const POINT *apt, int cpt) {
#ifdef WIN32
    ::Polyline(*this, apt, cpt);
#endif    
}

void LKSurface::Polyline(const POINT *apt, int cpt, const RECT& ClipRect) {
    if(cpt >= 2) {
        POINT Line[2];
        const POINT * It = apt;

        LKGeom::clipper<POINT> Clipper((POINT) {ClipRect.left, ClipRect.top}, (POINT) {ClipRect.right, ClipRect.bottom});
        
        Line[0] = *(It++);
        while ( It < apt+cpt) {
            Line[1] = *(It++);
            if(Clipper.ClipLine(Line[0], Line[1])) {
                Polyline(Line, 2);
            }
            Line[0] = Line[1];
        }
    }
}

void LKSurface::DrawDashPoly(const int width, const LKColor& color, const POINT *pt, const unsigned npoints, const RECT& rc) {
    for (unsigned Segment = 1; Segment < npoints; Segment++) {
        DrawDashLine(width, pt[Segment - 1], pt[Segment], color, rc);
    }
}

void LKSurface::Blackness(const int x, const int y, const int cx, const int cy) {
#ifdef WIN32
    ::BitBlt(*this, x, y, cx, cy, NULL, 0, 0, BLACKNESS);
#endif
}

void LKSurface::Whiteness(const int x, const int y, const int cx, const int cy) {
#ifdef WIN32
    ::BitBlt(*this, x, y, cx, cy, NULL, 0, 0, WHITENESS);
#endif
}

void LKSurface::DrawSolidLine(const POINT &ptStart, const POINT &ptEnd, const RECT& rc) {
    const POINT pt[2] = {{ptStart.x, ptStart.y}, {ptEnd.x, ptEnd.y}};
    Polyline(pt, 2, rc);
}

void LKSurface::DrawLine(int x1, int y1, int x2, int y2) {
    const POINT pt[2] = {{x1, y1}, {x2, y2}};
    Polyline(pt, 2);
}

void LKSurface::DrawLine(int x1, int y1, int x2, int y2, int x3, int y3) {
    const POINT pt[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
    Polyline(pt, 3);
}

void LKSurface::DrawLine(enumType PenStyle, const int width, const POINT& ptStart, const POINT& ptEnd, const LKColor& cr, const RECT& rc) {
    LKPen Pen(PenStyle, width, cr);
    LKPen OldPen = SelectObject(Pen);

    const POINT pt[2] = {ptStart, ptEnd};
    Polyline(pt, 2, rc);

    SelectObject(OldPen);
}

void LKSurface::DrawDashLine(const int width, const POINT& ptStart, const POINT& ptEnd, const LKColor& cr, const RECT& rc) {
    int i;
    POINT pt[2];

    int Offset = ((width - 1) / 2) + 1;
    // amount to shift 1st line to center
    // the group of lines properly

    //Create a dot pen
    LKPen hpDash(PEN_DASH, 1, cr);
    LKPen hpOld = SelectObject(hpDash);

    pt[0] = ptStart;
    pt[1] = ptEnd;

    //increment on smallest variance
    if (abs(ptStart.x - ptEnd.x) < abs(ptStart.y - ptEnd.y)) {
        pt[0].x -= Offset;
        pt[1].x -= Offset;
        for (i = 0; i < width; i++) {
            pt[0].x += 1;
            pt[1].x += 1;
            Polyline(pt, 2, rc);
        }
    } else {
        pt[0].y -= Offset;
        pt[1].y -= Offset;
        for (i = 0; i < width; i++) {
            pt[0].y += 1;
            pt[1].y += 1;
            Polyline(pt, 2, rc);
        }
    }

    SelectObject(hpOld);
}

void LKSurface::FillRect(const RECT *lprc, const LKBrush& Brush) {
#ifdef WIN32
    ::FillRect(*this, lprc, Brush);
#endif    
}

bool LKSurface::Copy(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, const LKSurface& Surface, int nXOriginSrc, int nYOriginSrc) {
#ifdef WIN32
    return ::BitBlt(*this, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, Surface, nXOriginSrc, nYOriginSrc, SRCCOPY);
#else
    return false;
#endif
}

bool LKSurface::StretchCopy(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, const LKSurface& Surface, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc) {
#ifdef WIN32
    return ::StretchBlt(*this, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, Surface, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
#else
    return false;
#endif    
}

#ifdef __MINGW32CE__
#ifdef __cplusplus
extern "C" {
#endif
/* 
TransparentImage is not defined in arm-mingw32ce

  TransparentBlt (Windows CE 5.0)
		The TransparentBlt function is a wrapper for the TransparentImage function.
		http://msdn.microsoft.com/fr-fr/library/windows/apps/xaml/aa453778.aspx
*/
	WINGDIAPI WINBOOL WINAPI TransparentImage(HDC hdcDest,int xoriginDest,int yoriginDest,int wDest,int hDest,HDC hdcSrc,int xoriginSrc,int yoriginSrc,int wSrc,int hSrc,UINT crTransparent);
#ifdef __cplusplus
}
#endif
#endif

bool LKSurface::TransparentCopy(int xoriginDest, int yoriginDest, int wDest, int hDest, const LKSurface& Surface, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, const LKColor& crTransparent) {
#ifdef WIN32
#ifdef UNDER_CE
    return ::TransparentImage(*this, xoriginDest, yoriginDest, wDest, hDest, Surface, xoriginSrc, yoriginSrc, wSrc, hSrc, crTransparent);
#else
    return ::TransparentBlt(*this, xoriginDest, yoriginDest, wDest, hDest, Surface, xoriginSrc, yoriginSrc, wSrc, hSrc, crTransparent);
#endif
#else
    return false;
#endif
}

// for each white pixel in the mask, do nothing (NOP), and for each black pixel, do a Copy.
bool LKSurface::CopyWithMask(int nXDest, int nYDest, int nWidth, int nHeight, const LKSurface& hdcSrc, int nXSrc, int nYSrc, const LKBitmap& bmpMask, int xMask, int yMask) {
#ifdef WIN32
    return ::MaskBlt(*this, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, bmpMask, xMask, yMask, MAKEROP4(SRCAND,  0x00AA0029));
#else
    return false;
#endif
}

#ifdef UNDER_CE
LKSurface::TAlphaBlendF LKSurface::AlphaBlendF = NULL;
#endif

// tries to locate AlphaBlend() function
// sets pointer to AlphaBlend function (AlphaBlendF) 
// (returns false when AlphaBlending is not supported)
bool LKSurface::AlphaBlendSupported() {
#ifdef WIN32
#ifdef UNDER_CE
    static bool bInit = false;
    if(!bInit) {
        AlphaBlendF = (TAlphaBlendF) GetProcAddress(GetModuleHandle(TEXT("coredll.dll")), TEXT("AlphaBlend"));        
        bInit = true;
    }
    return (AlphaBlendF != NULL);
#else
    // always supported on Windows PC
    return true;
#endif    
#endif
    return true;
}

bool LKSurface::AlphaBlend(const RECT& dstRect, const LKSurface& Surface, const RECT& srcRect, uint8_t globalOpacity) {
    if(!AlphaBlendSupported()) {
        return false;
    }
    
#ifdef WIN32
  //BLENDFUNCTION bf = { AC_SRC_OVER, 0, globalOpacity, AC_SRC_ALPHA };
  // we are not using per-pixel alpha, so do not use AC_SRC_ALPHA flag
  BLENDFUNCTION bf = { AC_SRC_OVER, 0, globalOpacity, 0 };
#ifdef UNDER_CE
  static unsigned failedCount = 0;
  static bool Success = false;
  
  bool bOK = AlphaBlendF(
    *this, dstRect.left, dstRect.top, dstRect.right - dstRect.left, dstRect.bottom - dstRect.top,
    Surface, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, bf);
  
  if(!Success) {
      if(!bOK && dstRect.right - dstRect.left > 0 &&
                 dstRect.bottom - dstRect.top > 0 &&
                 srcRect.right - srcRect.left > 0 &&
                 srcRect.bottom - srcRect.top > 0) {

          // after more 10 consecutive failed, we assume AlphaBlend is not supported, don't use it anymore
          ++failedCount;
          if(failedCount>10) {
              AlphaBlendF = NULL;
          }
       }
       Success = bOK;
  }
  return bOK;
  
#else
    ::AlphaBlend(*this, dstRect.left, dstRect.top, dstRect.right - dstRect.left, dstRect.bottom - dstRect.top,
                        Surface, srcRect.left, srcRect.top, srcRect.right - srcRect.left, srcRect.bottom - srcRect.top, bf);          
    
    return true; // always return true because always implemented on Windows PC
#endif
#else
    return false;
#endif    
}

bool LKSurface::GetTextSize(const TCHAR* lpString, int cbString, SIZE* lpSize) {
#ifdef WIN32
    return ::GetTextExtentPoint(*this, lpString, cbString, lpSize);
#else
    return false;
#endif    
}

void LKSurface::DrawText(int X, int Y, const TCHAR* lpString, UINT cbCount) {
#ifdef WIN32
    ::ExtTextOut(*this, X, Y, ETO_OPAQUE, NULL, lpString, cbCount, NULL);
#endif    
}

void LKSurface::DrawPushButton(const RECT& rc, bool bPushed) {
#ifdef WIN32
    const UINT Style = bPushed? DFCS_BUTTONPUSH | DFCS_PUSHED : DFCS_BUTTONPUSH;
    RECT Rect = rc;
    ::DrawFrameControl(*this, &Rect, DFC_BUTTON, Style);
#endif    
}

#warning "TODO : replace 'uFormat' by enum for portability."
int LKSurface::DrawText(const TCHAR* lpchText, int nCount, RECT *lpRect, UINT uFormat) {
#ifdef WIN32
    return ::DrawText(*this, lpchText, nCount, lpRect, uFormat);
#else
    return false;
#endif    
}

void LKSurface::DrawTextClip(int x, int y, const TCHAR *text, int width) {
    int len = _tcslen(text);
    if (len > 0) {
        SIZE tsize;
        GetTextSize(text, len, &tsize);
        RECT rc = {x, y, x + std::min((LONG) width, tsize.cx), y + tsize.cy};
#ifdef WIN32
        ::ExtTextOut(*this, x, y, ETO_CLIPPED, &rc, text, len, NULL);
#endif
    }
}

int LKSurface::GetTextWidth(const TCHAR *text) {
    SIZE tsize;
    GetTextSize(text, _tcslen(text), &tsize);
    return tsize.cx;
}

int LKSurface::GetTextHeight(const TCHAR *text) {
    SIZE tsize;
    GetTextSize(text, _tcslen(text), &tsize);
    return tsize.cy;
}

LKColor LKSurface::SetPixel(int X, int Y, const LKColor& Color) {
#ifdef WIN32
    return LKColor(::SetPixel(*this, X, Y, Color));
#else
    return LKColor();
#endif
}

void LKSurface::Rectangle(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect) {
#ifdef WIN32
    ::Rectangle(*this, nLeftRect, nTopRect, nRightRect, nBottomRect);
#endif
}

bool LKSurface::InvertRect(const RECT& rc) {
#ifdef WIN32
    return ::InvertRect(*this, &rc);
#else
    return false;
#endif
}

bool LKSurface::RoundRect(const RECT& rc, int nWidth, int nHeight) {
#ifdef WIN32
    return ::RoundRect(*this, rc.left, rc.top, rc.right, rc.bottom, nWidth, nHeight);
#else
    return false;
#endif
}

void LKSurface::ExcludeClipRect(const RECT& rc) {
#ifdef WIN32
    ::ExcludeClipRect(*this, rc.left, rc.top, rc.right, rc.bottom);
#endif
}

int LKSurface::SaveState() {
#ifdef WIN32
    return ::SaveDC(*this);
#else
    return 0;
#endif
}

void LKSurface::RestoreState(int nSaved) {
#ifdef WIN32
    ::RestoreDC(*this, nSaved);
#endif
}


#define  _COS(x) cos( ((4*atan(1))/2)-(x*2.0*(4*atan(1))/64.0))
const double LKSurface::xcoords[] = {
    _COS(0), _COS(1), _COS(2), _COS(3), _COS(4), _COS(5), _COS(6), _COS(7),
    _COS(8), _COS(9), _COS(10), _COS(11), _COS(12), _COS(13), _COS(14), _COS(15),
    _COS(16), _COS(17), _COS(18), _COS(19), _COS(20), _COS(21), _COS(22), _COS(23),
    _COS(24), _COS(25), _COS(26), _COS(27), _COS(28), _COS(29), _COS(30), _COS(31),
    _COS(32), _COS(33), _COS(34), _COS(35), _COS(36), _COS(37), _COS(38), _COS(39),
    _COS(40), _COS(41), _COS(42), _COS(43), _COS(44), _COS(45), _COS(46), _COS(47),
    _COS(48), _COS(49), _COS(50), _COS(51), _COS(52), _COS(53), _COS(54), _COS(55),
    _COS(56), _COS(57), _COS(58), _COS(59), _COS(60), _COS(61), _COS(62), _COS(63)
};

#define  _SIN(x) sin(((4*atan(1))/2)-(x*2.0*(4*atan(1))/64.0))
const double LKSurface::ycoords[] = {
    _SIN(0), _SIN(1), _SIN(2), _SIN(3), _SIN(4), _SIN(5), _SIN(6), _SIN(7),
    _SIN(8), _SIN(9), _SIN(10), _SIN(11), _SIN(12), _SIN(13), _SIN(14), _SIN(15),
    _SIN(16), _SIN(17), _SIN(18), _SIN(19), _SIN(20), _SIN(21), _SIN(22), _SIN(23),
    _SIN(24), _SIN(25), _SIN(26), _SIN(27), _SIN(28), _SIN(29), _SIN(30), _SIN(31),
    _SIN(32), _SIN(33), _SIN(34), _SIN(35), _SIN(36), _SIN(37), _SIN(38), _SIN(39),
    _SIN(40), _SIN(41), _SIN(42), _SIN(43), _SIN(44), _SIN(45), _SIN(46), _SIN(47),
    _SIN(48), _SIN(49), _SIN(50), _SIN(51), _SIN(52), _SIN(53), _SIN(54), _SIN(55),
    _SIN(56), _SIN(57), _SIN(58), _SIN(59), _SIN(60), _SIN(61), _SIN(62), _SIN(63)
};

void LKSurface::buildCircle(const POINT& center, int radius, std::vector<POINT>& list) {
    int step = ((radius<20)?2:1);
    list.clear();
    list.reserve((64/step)+1);
    list.push_back( (POINT){ center.x + (long) (radius * xcoords[0]), center.y + (long) (radius * ycoords[0]) });
    for(register int i=64-step; i>=0; i-=step) {
        list.push_back( (POINT){ center.x + (long) (radius * xcoords[i]), center.y + (long) (radius * ycoords[i]) });
    }
}

bool LKSurface::Circle(long x, long y, int radius, const RECT& rc, bool clip, bool fill) {

    if ((x - radius) > rc.right) return false;
    if ((x + radius) < rc.left) return false;
    if ((y - radius) > rc.bottom) return false;
    if ((y + radius) < rc.top) return false;

    std::vector<POINT> CirclePt;
    buildCircle((POINT){x,y}, radius, CirclePt);
      
    if (clip) {
        std::vector<POINT> CirclePt_clipped;
        CirclePt_clipped.reserve(CirclePt.size());

        LKGeom::ClipPolygon(rc, CirclePt, CirclePt_clipped);

        CirclePt = std::move(CirclePt_clipped);
    }

    if (fill) {
        Polygon(CirclePt.data(), CirclePt.size());
    } else {
        Polyline(CirclePt.data(), CirclePt.size());
    }
    return true;
}

bool LKSurface::CircleNoCliping(long x, long y, int radius, const RECT& rc, bool fill) {
    return Circle(x, y, radius, rc, false, fill);
}

int LKSurface::Segment(long x, long y, int radius, const RECT& rc, double start, double end, bool horizon) {
    POINT pt[66];
    int i;
    int istart;
    int iend;

    if ((x - radius) > rc.right) return false;
    if ((x + radius) < rc.left) return false;
    if ((y - radius) > rc.bottom) return false;
    if ((y + radius) < rc.top) return false;

    // JMW added faster checking...

    start = AngleLimit360(start);
    end = AngleLimit360(end);

    istart = iround(start / 360.0 * 64);
    iend = iround(end / 360.0 * 64);

    int npoly = 0;

    if (istart > iend) {
        iend += 64;
    }
    istart++;
    iend--;

    if (!horizon) {
        pt[0].x = x;
        pt[0].y = y;
        npoly = 1;
    }
    pt[npoly].x = x + (long) (radius * fastsine(start));
    pt[npoly].y = y - (long) (radius * fastcosine(start));
    npoly++;

    for (i = 0; i < 64; i++) {
        if (i <= iend - istart) {
            pt[npoly].x = x + (long) (radius * xcoords[(i + istart) % 64]);
            pt[npoly].y = y - (long) (radius * ycoords[(i + istart) % 64]);
            npoly++;
        }
    }
    pt[npoly].x = x + (long) (radius * fastsine(end));
    pt[npoly].y = y - (long) (radius * fastcosine(end));
    npoly++;

    if (!horizon) {
        pt[npoly].x = x;
        pt[npoly].y = y;
        npoly++;
    } else {
        pt[npoly].x = pt[0].x;
        pt[npoly].y = pt[0].y;
        npoly++;
    }
    if (npoly > 2) {
        Polygon(pt, npoly, rc);
    } else if (npoly > 1) {
        Polyline(pt, npoly, rc);
    }
    return true;
}

/*
 * VENTA3 This is a modified Segment()
 */
int LKSurface::DrawArc(long x, long y, int radius, const RECT& rc, double start, double end) {
    POINT pt[66];
    int i;
    int istart;
    int iend;

    if ((x - radius) > rc.right) return false;
    if ((x + radius) < rc.left) return false;
    if ((y - radius) > rc.bottom) return false;
    if ((y + radius) < rc.top) return false;

    // JMW added faster checking...

    start = AngleLimit360(start);
    end = AngleLimit360(end);

    istart = iround(start / 360.0 * 64);
    iend = iround(end / 360.0 * 64);

    int npoly = 0;

    if (istart > iend) {
        iend += 64;
    }
    istart++;
    iend--;

    pt[npoly].x = x + (long) (radius * fastsine(start));
    pt[npoly].y = y - (long) (radius * fastcosine(start));
    npoly++;

    for (i = 0; i < 64; i++) {
        if (i <= iend - istart) {
            pt[npoly].x = x + (long) (radius * xcoords[(i + istart) % 64]);
            pt[npoly].y = y - (long) (radius * ycoords[(i + istart) % 64]);
            npoly++;
        }
    }
    pt[npoly].x = x + (long) (radius * fastsine(end));
    pt[npoly].y = y - (long) (radius * fastcosine(end));
    npoly++;

    Polyline(pt, npoly, rc);

    return true;
}

