/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include <climits>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "rect.h"

Point::Point() : x(0), y(0)
{
}

Point::Point(s16 px, s16 py) : x(px), y(py)
{
}

bool Point::operator== (const Point & pt) const
{
    return (x == pt.x && y == pt.y);
}

bool Point::operator!= (const Point & pt) const
{
    return !(*this == pt);
}

Point & Point::operator+=(const Point & pt)
{
    x += pt.x;
    y += pt.y;

    return *this;
}

Point & Point::operator-=(const Point & pt)
{
    x -= pt.x;
    y -= pt.y;

    return *this;
}

Point Point::operator+(const Point & pt)
{
    return Point(x + pt.x, y + pt.y);
}

Point Point::operator-(const Point & pt)
{
    return Point(x - pt.x, y - pt.y);
}

bool Point::inABC(const Point & pt1, const Point & pt2, const Point & pt3) const
{
    s32 a = (pt1.x - x) * (pt2.y - pt1.y) - (pt2.x - pt1.x) * (pt1.y - y);
    s32 b = (pt2.x - x) * (pt3.y - pt2.y) - (pt3.x - pt2.x) * (pt2.y - y);
    s32 c = (pt3.x - x) * (pt1.y - pt3.y) - (pt1.x - pt3.x) * (pt3.y - y);

    return ((a >= 0 && b >= 0 && c >= 0) || (a < 0 && b < 0 && c < 0));
}

Size::Size() : w(0), h(0)
{
}

Size::Size(u16 sw, u16 sh) : w(sw), h(sh)
{
}

Size::Size(const Point & pt) : w(std::abs(pt.x)), h(std::abs(pt.y))
{
}

bool Size::operator== (const Size & sz) const
{
    return (w == sz.w && h == sz.h);
}

bool Size::operator!= (const Size & sz) const
{
    return !(*this == sz);
}

bool Size::isEmpty(void) const
{
    return 0 == w && 0 == h;
}

Rect::Rect()
{
}

Rect::Rect(s16 rx, s16 ry, u16 rw, u16 rh) : Point(rx, ry), Size(rw, rh)
{
}

Rect::Rect(const Point & pt, u16 rw, u16 rh) : Point(pt), Size(rw, rh)
{
}

Rect::Rect(const Point & pt, const Size & sz) : Point(pt), Size(sz)
{
}

Rect Rect::Get(const SDL_Rect & rt)
{
    return Rect(rt.x, rt.y, rt.w, rt.h);
}

Rect Rect::Get(const Point & pt1, const Point & pt2)
{
    Rect res;

    res.x = pt1.x < pt2.x ? pt1.x : pt2.x;
    res.y = pt1.y < pt2.y ? pt1.y : pt2.y;
    res.w = (pt1.x < pt2.x ? pt2.x - pt1.x : pt1.x - pt2.x) + 1;
    res.h = (pt1.y < pt2.y ? pt2.y - pt1.y : pt1.y - pt2.y) + 1;

    return res;
}

Rect Rect::Get(const Rect & rt1, const Rect & rt2, bool intersect)
{
    Rect rt3;

    if(intersect)
    {
	if(rt1 & rt2)
	{
    	    rt3.x = std::max(rt1.x, rt2.x);
    	    rt3.y = std::max(rt1.y, rt2.y);
    	    rt3.w = std::min(rt1.x + rt1.w, rt2.x + rt2.w) - rt3.x;
    	    rt3.h = std::min(rt1.y + rt1.h, rt2.y + rt2.h) - rt3.y;
	}
    }
    else
    // max
    {
	rt3.x = rt1.x < rt2.x ? rt1.x : rt2.x;
	rt3.y = rt1.y < rt2.y ? rt1.y : rt2.y;
	rt3.w = rt1.x + rt1.w > rt2.x + rt2.w ? rt1.x + rt1.w - rt3.x : rt2.x + rt2.w - rt3.x;
	rt3.h = rt1.y + rt1.h > rt2.y + rt2.h ? rt1.y + rt1.h - rt3.y : rt2.y + rt2.h - rt3.y;
    }

    return rt3;
}

Rect & Rect::operator= (const Point & pt)
{
    x = pt.x;
    y = pt.y;

    return *this;
}

bool Rect::operator== (const Rect & rt) const
{
    return (x == rt.x && y == rt.y && w == rt.w && h == rt.h);
}

bool Rect::operator!= (const Rect & rt) const
{
    return !(*this == rt);
}

bool Rect::operator& (const Point & pt) const
{
    return !(pt.x < x || pt.y < y || pt.x >= (x + w) || pt.y >= (y + h));
}

bool Rect::operator& (const Rect & rt) const
{
    return ! (x > rt.x + rt.w || x + w < rt.x || y > rt.y + rt.h || y + h < rt.y);
}

Rect Points::GetRect(void) const
{
    Rect res;

    if(1 < size())
    {
	res = Rect::Get(at(0), at(1));

	for(const_iterator
	    it = begin() + 2; it != end(); ++it)
	{
	    if((*it).x < res.x) res.x = (*it).x;
	    else
	    if((*it).x > res.x + res.w) res.w = (*it).x - res.x + 1;

	    if((*it).y < res.y) res.y = (*it).y;
	    else
	    if((*it).y > res.y + res.h) res.h = (*it).y - res.y + 1;
	}
    }

    return res;
}

Rect Rects::GetRect(void) const
{
    Rect res;

    if(size())
    {
	const_iterator it = begin();
	res = *it;

	++it;

	for(; it != end(); ++it)
	    res = Rect::Get(*it, res, false);
    }

    return res;
}

s32 Rects::GetIndex(const Point & pt) const
{
    for(const_iterator
	it = begin(); it != end(); ++it)
	if(*it & pt) return std::distance(begin(), it);
    return -1;
}

Rect Rect::Fixed(s16 & rx, s16 & ry, u16 rw, u16 rh, const Rect & max)
{
    Rect res;

    if(rw && rh &&
        rx + rw > max.x && ry + rh > max.y &&
        rx < max.x + max.w && ry < max.y + max.h)
    {
        res.w = rw;
        res.h = rh;

        if(rx < max.x)
        {
            res.x = max.x - rx;
            rx = max.x;
        }

        if(ry < max.y)
        {
            res.y = max.y - ry;
            ry = max.y;
        }

        if(rx + rw > max.x + max.w)
            res.w = max.x + max.w - rx;

        if(ry + rh > max.y + max.h)
            res.h = max.y + max.h - ry;
    }

    return res;
}
