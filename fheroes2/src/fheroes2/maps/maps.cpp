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

#include <cstdlib>
#include <algorithm>
#include "world.h"
#include "gamearea.h"
#include "settings.h"
#include "maps.h"
#include "maps_tiles.h"

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

u16 Maps::GetDirectionAroundFog(const u16 center, u8 color)
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

u16 Maps::GetDirectionAroundGround(const u16 center, const u16 ground)
{
    if(0 == ground || !isValidAbsPoint(center % world.w(), center / world.h())) return 0;

    u16 result = 0;

    for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	if(!isValidDirection(center, direct))
	    result |= direct;
	else
	if(ground & world.GetTiles(GetDirectionIndex(center, direct)).GetGround()) result |= direct;

    return result;
}

u8 Maps::GetCountAroundGround(const u16 center, const u16 ground)
{
    if(0 == ground || !isValidAbsPoint(center % world.w(), center / world.h())) return 0;

    u8 result = 0;

    for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	if(!isValidDirection(center, direct))
	    ++result;
	else
	if(ground & world.GetTiles(GetDirectionIndex(center, direct)).GetGround()) ++result;

    return result;
}

u16 Maps::GetMaxGroundAround(const u16 center)
{
    std::vector<u8> grounds(9, 0);
    u16 result = 0;

    for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
    {
    	const Maps::Tiles & tile = (Maps::isValidDirection(center, direct) ? world.GetTiles(GetDirectionIndex(center, direct)) : world.GetTiles(center));

	switch(tile.GetGround())
	{
	    case Maps::Ground::DESERT:	++grounds[0]; break;
	    case Maps::Ground::SNOW:	++grounds[1]; break;
	    case Maps::Ground::SWAMP:	++grounds[2]; break;
	    case Maps::Ground::WASTELAND:++grounds[3]; break;
	    case Maps::Ground::BEACH:	++grounds[4]; break;
	    case Maps::Ground::LAVA:	++grounds[5]; break;
	    case Maps::Ground::DIRT:	++grounds[6]; break;
	    case Maps::Ground::GRASS:	++grounds[7]; break;
	    case Maps::Ground::WATER:	++grounds[8]; break;
	    default: break;
	}
    }
    
    const u8 max = *std::max_element(grounds.begin(), grounds.end());

    if(max == grounds[0]) result |= Maps::Ground::DESERT;
    if(max == grounds[1]) result |= Maps::Ground::SNOW;
    if(max == grounds[2]) result |= Maps::Ground::SWAMP;
    if(max == grounds[3]) result |= Maps::Ground::WASTELAND;
    if(max == grounds[4]) result |= Maps::Ground::BEACH;
    if(max == grounds[5]) result |= Maps::Ground::LAVA;
    if(max == grounds[6]) result |= Maps::Ground::DIRT;
    if(max == grounds[7]) result |= Maps::Ground::GRASS;
    if(max == grounds[8]) result |= Maps::Ground::WATER;

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

u16 Maps::ScanAroundObject(const u16 center, const u8 obj, bool full)
{
    const s16 cx = center % world.w();
    const s16 cy = center / world.w();

    for(s16 y = -1; y <= 1; ++y)
        for(s16 x = -1; x <= 1; ++x)
    {
            if((!y && !x) || (y && x && !full)) continue;
	    if(isValidAbsPoint(cx + x, cy + y) &&
		obj == world.GetTiles(GetIndexFromAbsPoint(cx + x, cy + y)).GetObject()) return (cy + y) * world.w() + (cx + x);
    }
    return MAXU16;
}

u16 Maps::GetApproximateDistance(const u16 index1, const u16 index2)
{
    return std::max(std::abs(static_cast<s32>(index1 % world.w()) - static_cast<s32>(index2 % world.w())), 
	            std::abs(static_cast<s32>(index1 / world.w()) - static_cast<s32>(index2 / world.w())));
}
