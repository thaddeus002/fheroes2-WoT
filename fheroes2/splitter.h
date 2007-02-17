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
#ifndef H2SPLITTER_H
#define H2SPLITTER_H

#include "rect.h"
#include "spritecursor.h"
#include "surface.h"
#include "gamedefs.h"

class Splitter : private SpriteCursor
{
public:
    typedef enum { HORIZONTAL, VERTICAL } positions_t;

    Splitter(const Surface &sf, const Rect &rt, positions_t pos);

    void Forward(void);
    void Backward(void);
    void Move(u16 pos);

    void SetRange(u16 smin, u16 smax);

    u16 GetCurrent(void) const{ return cur; };
    const Rect & GetRect(void) const{ return area; };

private:
    const Rect area;
    u16 step;
    u16 min;
    u16 max;
    u16 cur;
    positions_t position;
};

#endif
