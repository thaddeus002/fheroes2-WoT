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
	SetPos(rt);
    }
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
}

const Point & SpriteBack::GetPos(void) const
{
    return SpritePos::GetPos();
}

Rect SpriteBack::GetArea(void) const
{
    return SpritePos::GetArea();
}

Size SpriteBack::GetSize(void) const
{
    return Surface::GetSize();
}

u32 SpriteBack::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage();
}

SpriteMove::SpriteMove() : visible(false)
{
}

SpriteMove::SpriteMove(const Surface & sf) : visible(false)
{
    Set(sf, true);
}

void SpriteMove::Move(const Point & pt)
{
    if(GetPos() != pt)
        Hide();

    Show(pt);
}

void SpriteMove::Move(s16 ax, s16 ay)
{
    Move(Point(ax, ay));
}

void SpriteMove::Hide(void)
{
    if(isVisible())
    {
        background.Restore();
        visible = false;
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

void SpriteMove::Show(const Point & pos)
{
    if(! isVisible() && Surface::isValid())
    {
        background.Save(Rect(pos, GetSize()));
        Surface::Blit(GetPos(), Display::Get());
        visible = true;
    }
}

bool SpriteMove::isVisible(void) const
{
    return visible;
}

const Point & SpriteMove::GetPos(void) const
{
    return background.GetPos();
}

Rect SpriteMove::GetArea(void) const
{
    return Rect(GetPos(), GetSize());
}

Size SpriteMove::GetSize(void) const
{
    return Surface::GetSize();
}

u32 SpriteMove::GetMemoryUsage(void) const
{
    return Surface::GetMemoryUsage() +
	background.GetMemoryUsage() + sizeof(visible);
}
