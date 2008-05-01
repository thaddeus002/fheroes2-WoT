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

#include <vector>
#include "types.h"

class Point
{
public:
    s16 x;
    s16 y;

    Point(s16 px = 0, s16 py = 0);

    bool operator== (const Point & pt) const;
    bool operator!= (const Point & pt) const;

    Point & operator+=(const Point & pt);
    Point & operator-=(const Point & pt);
};

Point operator+(const Point& pt1, const Point& pt2);

Point operator-(const Point& pt1, const Point& pt2);

class Size
{
public:
    u16 w;
    u16 h;

    Size(u16 sw = 0, u16 sh = 0);

    bool valid(void) const;

    bool operator== (const Size & sz) const;
    bool operator!= (const Size & sz) const;
};

class Rect : public Point, public Size
{
public:
    Rect(s16 rx = -1, s16 ry = -1, u16 rw = 0, u16 rh = 0);
    Rect(const SDL_Rect & rt);
    Rect(const Point & pt, u16 rw, u16 rh);
    Rect(const Point & pt, const Size & sz);
    Rect(const std::vector<Rect> & vect);

    SDL_Rect* SDLRect(void);

    bool valid(void) const;

    Rect & operator= (const Point & pt);
    bool operator== (const Rect & rt) const;
    bool operator!= (const Rect & rt) const;

    // rect include point
    bool operator& (const Point & pt) const;

    // rect intersects rect
    bool operator& (const Rect & rt) const;
};

#endif
