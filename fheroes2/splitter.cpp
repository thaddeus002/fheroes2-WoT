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

#include "splitter.h"

/* splitter constructor */
Splitter::Splitter(const Surface &sf, const Rect &rt, positions_t pos)
    : SpriteCursor(sf, rt), area(rt), step(0), min(0), max(0), cur(0), position(pos)
{
}

/* set range */
void Splitter::SetRange(u16 smin, u16 smax)
{
    min = smin;
    max = smax;

    // recalculate step
    if(max) step = (Splitter::VERTICAL == position ? 100 * (area.h - h()) / max : 100 * (area.w - w()) / max);
    
    Move(min);
}

/* move splitter to pos */
void Splitter::Move(u16 pos)
{
    if(cur == pos) return;

    Point pt(GetPos());

    cur = pos;

    if(Splitter::VERTICAL == position)
        pt.y = area.y + cur * step / 100;
    else
        pt.x = area.x + cur * step / 100;

    SpriteCursor::Move(pt);
}

/* forward spliter */
void Splitter::Forward(void)
{
    if(cur == max) return;

    Point pt(GetPos());

    ++cur;

    if(Splitter::VERTICAL == position)
        pt.y = area.y + cur * step / 100;
    else
        pt.x = area.x + cur * step / 100;

    SpriteCursor::Move(pt);
}

/* backward spliter */
void Splitter::Backward(void)
{
    if(! cur) return;

    Point pt(GetPos());

    --cur;
		    
    if(Splitter::VERTICAL == position)
	pt.y = area.y + cur * step / 100;
    else
        pt.x = area.x + cur * step / 100;

    SpriteCursor::Move(pt);
}
