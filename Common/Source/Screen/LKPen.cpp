/*
 * LK8000 Tactical Flight Computer -  WWW.LK8000.IT
 * Released under GNU/GPL License v.2
 * See CREDITS.TXT file for authors and copyrights
 *
 * File:   LKPen.cpp
 * Author: bruno de Lacheisserie
 * 
 * Created on 16 octobre 2014
 */
#ifdef WIN32
#include <windows.h>
#else
#warning "TODO: need to implement"
#endif

#include "LKPen.h"
#include "LKColor.h"

#ifdef WIN32

const LKPen LK_NULL_PEN((HPEN)GetStockObject(NULL_PEN));
const LKPen LK_BLACK_PEN((HPEN)GetStockObject(BLACK_PEN));
const LKPen LK_WHITE_PEN((HPEN)GetStockObject(WHITE_PEN));


LKPen::LKPen() : _Pen(), _Destroy() {
}

LKPen::LKPen(LKPen&& Pen) : _Pen(), _Destroy() {
    *this = Pen;
}

LKPen::LKPen(const LKPen& Pen) : _Pen(), _Destroy() {
    *this = Pen;
}

LKPen::LKPen(enumType Type, unsigned Size, const LKColor& Color) : _Pen(), _Destroy() {
    Create(Type, Size, Color);
}

#else

LKPen::LKPen(){
}

LKPen::LKPen(const LKPen& Pen) {
    *this = Pen;
}

LKPen::LKPen(enumType Type, unsigned Size, const LKColor& Color) {
    Create(Type, Size, Color);
}

#endif

LKPen::~LKPen() {
    Release();
}

LKPen& LKPen::operator= (LKPen&& Pen) {
    Release();

#ifdef WIN32
    _Pen = Pen._Pen;
    _Destroy = Pen._Destroy;

    Pen._Pen = NULL;
    Pen._Destroy = false;
#endif
    return (*this);
}

LKPen& LKPen::operator=(const LKPen& pen) {
    Release();

#ifdef WIN32
    _Pen = (HPEN) pen;
    _Destroy = false; // don't take ownership
#endif
    
    return * this;
}

void LKPen::Create(enumType Type, unsigned Size, const LKColor& Color) {
    Release();

#ifdef WIN32
    _Pen = CreatePen(Type, Size, Color);
    _Destroy = true; // take ownership
#endif
}

void LKPen::Release() {
#ifdef WIN32
    if (_Destroy && _Pen) {
        ::DeleteObject(_Pen);
    }
    _Pen = NULL;
#endif
}


