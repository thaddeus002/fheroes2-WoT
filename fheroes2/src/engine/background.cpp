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

#include "background.h"

#include "display.h"
#include "error.h"

Background::Background(const Rect &rt) : Surface(), pos(rt)
{
}

Background::Background(s16 x, s16 y, u16 w, u16 h) : Surface(), pos(x, y, w, h)
{
}

Background::Background(const Point &pt, u16 w, u16 h) : Surface(), pos(pt, w, h)
{
}

void Background::Save(void)
{
    // resize background
    if(valid() && (pos.w != w() || pos.h != h())) FreeSurface();

    if(! valid()) CreateSurface(pos, DEFAULT_DEPTH, SDL_SWSURFACE);

    Blit(display, pos, 0, 0);
}

void Background::Save(s16 ax, s16 ay)
{
    pos.x = ax;
    pos.y = ay;
    Save();
}

void Background::Save(const Point &pt)
{
    pos = pt;
    Save();
}

void Background::Save(const Rect &rt)
{
    pos = rt;
    Save();
}

void Background::Restore(void)
{
    display.Blit(*this, pos.x, pos.y);
}

const Rect & Background::GetRect(void) const
{
    return pos;
}
