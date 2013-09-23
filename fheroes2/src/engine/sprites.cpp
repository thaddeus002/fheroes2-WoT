/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "sprites.h"
#include "display.h"

SpritePos::SpritePos()
{
}

SpritePos::SpritePos(const Point & pos, const Size & sz, bool amask) : Surface(sz.w, sz.h, amask), Point(pos)
{
}

const Point & SpritePos::GetPos(void) const
{
    return *this;
}

Rect SpritePos::GetArea(void) const
{
    return Rect(GetPos(), GetSize());
}

void SpritePos::SetPos(const Point & pos)
{
    Point::x = pos.x;
    Point::y = pos.y;
}

u32 SpritePos::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage() + sizeof(x) + sizeof(y);
}

SpriteBack::SpriteBack()
{
}

u32 SpriteBack::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage() + sizeof(Rect::x) + sizeof(Rect::y) + sizeof(Rect::w) + sizeof(Rect::h);
}

SpriteBack::SpriteBack(const Rect & pos)
{
    Save(pos);
}

void SpriteBack::SetPos(const Point & pos)
{
    Rect::x = pos.x;
    Rect::y = pos.y;
}

bool SpriteBack::isValid(void) const
{
    return Surface::isValid();
}

void SpriteBack::Save(const Rect & rt)
{
    // resize SpriteBack
    if(Surface::isValid() &&
	GetSize() != rt) FreeSurface(*this);

    if(rt.w && rt.h)
    {
	if(! Surface::isValid())
	{
	    Set(rt.w, rt.h, false);
	    SetDisplayFormat();
	}

	Display::Get().Blit(rt, 0, 0, *this);

	Rect::w = rt.w;
	Rect::h = rt.h;
    }

    Rect::x = rt.x;
    Rect::y = rt.y;
}

void SpriteBack::Save(const Point & pt)
{
    Save(Rect(pt, GetSize()));
}

void SpriteBack::Restore(void)
{
    if(Surface::isValid())
	Blit(GetPos(), Display::Get());
}

void SpriteBack::Destroy(void)
{
    Surface::FreeSurface(*this);
    Rect::w = 0;
    Rect::h = 0;
}

const Point & SpriteBack::GetPos(void) const
{
    const Point & pt = *this;
    return pt;
}

const Size & SpriteBack::GetSize(void) const
{
    const Size & sz = *this;
    return sz;
}

const Rect & SpriteBack::GetArea(void) const
{
    const Rect & rt = *this;
    return rt;
}

enum { _VISIBLE = 0x00001 };

SpriteMove::SpriteMove() : mode(0)
{
}

SpriteMove::SpriteMove(const Surface & sf) : mode(0)
{
    Set(sf, true);
}

void SpriteMove::Move(const Point & pt)
{
    if(GetPos() != pt)
        Hide();

    Show(pt);
}

void SpriteMove::Move(int ax, int ay)
{
    Move(Point(ax, ay));
}

void SpriteMove::Hide(void)
{
    if(isVisible())
    {
        background.Restore();
	mode &= ~(_VISIBLE);
    }
}

void SpriteMove::Show(const Point & pos)
{
    if(! isVisible() && Surface::isValid())
    {
        background.Save(Rect(pos, GetSize()));
        Surface::Blit(GetPos(), Display::Get());
	mode |= _VISIBLE;
    }
}

void SpriteMove::Redraw(void)
{
    Hide();
    Show();
}

void SpriteMove::Show(void)
{
    Show(GetPos());
}

bool SpriteMove::isVisible(void) const
{
    return mode & _VISIBLE;
}

const Point & SpriteMove::GetPos(void) const
{
    return background.GetPos();
}

const Rect & SpriteMove::GetArea(void) const
{
    return background.GetArea();
}

u32 SpriteMove::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage() +
	background.GetMemoryUsage() + sizeof(mode);
}
