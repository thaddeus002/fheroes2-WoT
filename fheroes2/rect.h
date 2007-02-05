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

class Rect;

class Point : public SDLmm::SPoint
{
public:
    Point() : SDLmm::SPoint(-1, -1){};
    Point(int x, int y) : SDLmm::SPoint(x, y){};
    Point(const Point & pt) : SDLmm::SPoint(pt){};

    inline Point & operator= (const Point & pt){ x = pt.x; y = pt.y; return *this; };
    inline bool operator== (const Point & pt) const{ return (x == pt.x && y == pt.y); };
};

class Rect : public SDLmm::SRect
{
public:
    Rect() : SDLmm::SRect(-1, -1, 0, 0){};
    Rect(u16 nw, u16 nh) : SDLmm::SRect(nw, nh){};
    Rect(s16 nx, s16 ny, u16 nw, u16 nh) : SDLmm::SRect(nx, ny, nw, nh){};
    Rect(const Point & lu, const Point & rb) : SDLmm::SRect(lu, rb){};
    Rect(const Rect & rt) : SDLmm::SRect(rt){};

    inline Rect & operator= (const Rect & rt){ x = rt.x; y = rt.y; w = rt.w; h = rt.h; return *this; };
    inline bool operator== (const Rect & rt) const{ return (x == rt.x && y == rt.y && w == rt.w && h == rt.h); };
    inline bool operator!= (const Rect & rt) const{ return !(*this == rt); };
    // rect include point
    inline bool operator& (const Point & pt) const{ return !(pt.x < x || pt.y < y || pt.x > (x + w) || pt.y > (y + h)); };
};

#endif
