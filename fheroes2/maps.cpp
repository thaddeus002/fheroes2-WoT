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

#include "world.h"
#include "gamearea.h"
#include "error.h"
#include "maps.h"

u16 Maps::GetDirectionIndex(u16 from, Direction::vector_t vector)
{
    switch(vector)
    {
	case Direction::TOP:		return GetTopIndex(from);
	case Direction::TOP_RIGHT:	return GetTopRightIndex(from);
	case Direction::RIGHT:		return GetRightIndex(from);
	case Direction::BOTTOM_RIGHT:	return GetBottomRightIndex(from);
	case Direction::BOTTOM:		return GetBottomIndex(from);
	case Direction::BOTTOM_LEFT:	return GetBottomLeftIndex(from);
	case Direction::LEFT:		return GetLeftIndex(from);
	case Direction::TOP_LEFT:	return GetTopLeftIndex(from);
	default: break;
    }

    return MAXU16;
}

// check bound
bool Maps::isValidDirection(u16 from, Direction::vector_t vector)
{
    switch(vector)
    {
	case Direction::TOP:		return (!(from < world.w()));
	case Direction::TOP_RIGHT:	return (!(from < world.w()) && (world.w() - 1 > (from % world.w())));
	case Direction::RIGHT:		return (world.w() - 1 > (from % world.w()));
	case Direction::BOTTOM_RIGHT:	return ((from < world.w() * (world.h() - 1)) && (world.w() - 1 > (from % world.w())));
	case Direction::BOTTOM:		return (from < world.w() * (world.h() - 1));
	case Direction::BOTTOM_LEFT:	return ((from < world.w() * (world.h() - 1)) && (from % world.w()));
	case Direction::LEFT:		return (from % world.w());
	case Direction::TOP_LEFT:	return (!(from < world.w()) && (from % world.w()));
	default: break;
    }

    return false;
}
    
u16 Maps::GetTopIndex(u16 from)
{
    return from - world.w();
}

u16 Maps::GetTopRightIndex(u16 from)
{
    return from - world.w() + 1;    
}

u16 Maps::GetRightIndex(u16 from)
{
    return from + 1;
}

u16 Maps::GetBottomRightIndex(u16 from)
{
    return from + world.w() + 1;
}

u16 Maps::GetBottomIndex(u16 from)
{
    return from + world.w();
}

u16 Maps::GetBottomLeftIndex(u16 from)
{
    return from + world.w() - 1;
}

u16 Maps::GetLeftIndex(u16 from)
{
    return from - 1;
}

u16 Maps::GetTopLeftIndex(u16 from)
{
    return from - world.w() - 1;
}

/* convert maps point to index maps */
u16 Maps::GetIndexFromAbsPoint(const Point & mp)
{
    return mp.y * world.w() + mp.x;
}

/* convert area point to index maps */
u16 Maps::GetIndexFromAreaPoint(const Point & pt)
{
    const Rect & area_pos = GameArea::GetRect();

    u16 result = (area_pos.y + (pt.y - BORDERWIDTH) / TILEWIDTH) * world.w() + area_pos.x + (pt.x - BORDERWIDTH) / TILEWIDTH;

    if(result > world.w() * world.h() - 1) Error::Except("Game::GetIndexMaps: position, out of range.");

    return result;
}
