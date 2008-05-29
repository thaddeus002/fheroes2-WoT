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
#include "settings.h"
#include "maps.h"

namespace Maps
{
    static u32 animation_ticket = 0;
};

u32 Maps::AnimationTicket(void)
{
    return animation_ticket;
}

void Maps::IncreaseAnimationTicket(void)
{
    ++animation_ticket;
}

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

bool Maps::isValidAbsPoint(const Point & pt)
{
    return isValidAbsPoint(pt.x, pt.y);
}

bool Maps::isValidAbsPoint(const s16 x, const s16 y)
{
    return 0 <= x && world.w() > x && 0 <= y && world.h() > y;
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
    return GetIndexFromAbsPoint(mp.x, mp.y);
}

u16 Maps::GetIndexFromAbsPoint(s16 px, s16 py)
{
    return py * world.w() + px;
}

/* convert area point to index maps */
int Maps::GetIndexFromAreaPoint(const Point & pt)
{
    return GetIndexFromAreaPoint(pt.x, pt.y);
}

int Maps::GetIndexFromAreaPoint(s16 px, s16 py)
{
    const Rect & area_pos = GameArea::Get().GetRect();

    u16 result = (area_pos.y + (py - BORDERWIDTH) / TILEWIDTH) * world.w() + area_pos.x + (px - BORDERWIDTH) / TILEWIDTH;

    return result > world.w() * world.h() - 1 ? -1 : result;
}

u16 Maps::GetAroundFogDirection(u16 center, u8 color)
{
    if(!isValidAbsPoint(center % world.w(), center / world.h())) return 0;
    if(0 == color) color = Settings::Get().MyColor();

    u16 result = 0;

    for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	if(!isValidDirection(center, direct) ||
	   world.GetTiles(GetDirectionIndex(center, direct)).isFog(color)) result |= direct;

    if(world.GetTiles(center).isFog(color)) result |= Direction::CENTER;

    return result;
}

void Maps::ClearFog(const Point & center, const u8 scoute, const u8 color)
{
    if(0 == scoute) return;

    for(s16 y = center.y - scoute; y <= center.y + scoute; ++y)
        for(s16 x = center.x - scoute; x <= center.x + scoute; ++x)
            if(isValidAbsPoint(x, y) &&  (scoute + scoute / 2) >= std::abs(x - center.x) + std::abs(y - center.y))
                world.GetTiles(GetIndexFromAbsPoint(x, y)).ClearFog(color);
}
