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

Background::Background(const Rect &rt) : Surface(),Rect(rt)
{
}

Background::Background(s16 x, s16 y, u16 w, u16 h) : Surface(), Rect(x, y, w, h)
{
}

Background::Background(const Point &pt, u16 w, u16 h) : Surface(), Rect(pt, w, h)
{
}

bool Background::valid(void)
{
    return Surface::valid();
}

void Background::Save(void)
{
    // resize background
    if(Surface::valid() && (Size::w != Surface::w() || Size::h != Surface::h())) FreeSurface();

    if(! Surface::valid()) CreateSurface(*this, DEFAULT_DEPTH, SDL_SWSURFACE);

    Blit(Display::Get(), *this, 0, 0);
    SetDisplayFormat();
}

void Background::Save(s16 ax, s16 ay)
{
    x = ax;
    y = ay;

    Save();
}

void Background::Save(s16 ax, s16 ay, u16 aw, u16 ah)
{
    x = ax;
    y = ay;
    Size::w = aw;
    Size::h = ah;

    Save();
}

void Background::Save(const Point &pt)
{
    x = pt.x;
    y = pt.y;

    Save();
}

void Background::Save(const Rect &rt)
{
    Save(rt.x, rt.y, rt.w, rt.h);
}

void Background::Restore(void)
{
    Display::Get().Blit(*this, x, y);
}

const Rect & Background::GetRect(void) const
{
    return *this;
}

const Point & Background::GetPos(void) const
{
    return *this;
}

const Size & Background::GetSize(void) const
{
    return *this;
}
