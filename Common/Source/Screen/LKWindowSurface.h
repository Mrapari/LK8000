/*
 * LK8000 Tactical Flight Computer -  WWW.LK8000.IT
 * Released under GNU/GPL License v.2
 * See CREDITS.TXT file for authors and copyrights
 *
 * File:   LKWindowSurface.h
 * Author: Bruno de Lacheisserie
 *
 * Created on 19 octobre 2014, 12:31
 */

#ifndef LKWINDOWSURFACE_H
#define	LKWINDOWSURFACE_H

#include "LKSurface.h"

class LKWindowSurface : public LKSurface {
public:
    LKWindowSurface();
    virtual ~LKWindowSurface();

    virtual void Release();
    
#ifdef WIN32    
    explicit LKWindowSurface(HWND hWnd);
    void Create(HWND hWnd);

private:
    HWND _hWnd;
#endif
};

class LKPaintSurface : public LKSurface {
public:
	LKPaintSurface() = delete;
        virtual ~LKPaintSurface();

        virtual void Release();
#ifdef WIN32
	explicit LKPaintSurface(HWND hWnd); // BeginPaint

protected:
	HWND _hWnd;
	
public:
	PAINTSTRUCT	 _ps;
	
#endif

};

#endif	/* LKWINDOWSURFACE_H */

