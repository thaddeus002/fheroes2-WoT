/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef H2RECT_H
#define H2RECT_H

#include "gamedefs.h"

class Size
{
public:
    u16 w;
    u16 h;

    Size(int sw = 0, int sh = 0) : w(sw), h(sh){};

    inline bool operator== (const Size & sz) const{ return (w == sz.w && h == sz.h); };
    inline bool operator!= (const Size & sz) const{ return !(*this == sz); };
};

class Point
{
public:
    s16 x;
    s16 y;

    Point(int px = -1, int py = -1) : x(px), y(py){};

    inline bool operator== (const Point & pt) const{ return (x == pt.x && y == pt.y); };
    inline bool operator!= (const Point & pt) const{ return !(*this == pt); };
};

class Rect : public Point, public Size
{
public:
    Rect(s16 rx = -1, s16 ry = -1, u16 rw = 0, u16 rh = 0) : Point(rx, ry), Size(rw, rh){};
    Rect(const Point & pt, u16 rw, u16 rh) : Point(pt), Size(rw, rh){};
    Rect(const Point & pt, const Size & sz) : Point(pt), Size(sz){};

    const Size & size(void) const{ return *this; };

    inline Rect & operator= (const Point & pt){ x = pt.x; y = pt.y; return *this; };
    inline bool operator== (const Rect & rt) const{ return (x == rt.x && y == rt.y && w == rt.w && h == rt.h); };
    inline bool operator!= (const Rect & rt) const{ return !(*this == rt); };
    // rect include point
    inline bool operator& (const Point & pt) const{ return !(pt.x < x || pt.y < y || pt.x > (x + w) || pt.y > (y + h)); };
};

#endif
