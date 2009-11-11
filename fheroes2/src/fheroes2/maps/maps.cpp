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
#include "settings.h"
#include "maps.h"
#include "kingdom.h"
#include "maps_tiles.h"

namespace Maps
{
    static u32 animation_ticket = 0;
};

const char* Maps::SizeString(u8 s)
{
    const char* mapsize[] = { "Unknown", _("Small"), _("Medium"), _("Large"), _("Extra Large") };

    switch(s)
    {
	case SMALL:	return mapsize[1];
	case MEDIUM:	return mapsize[2];
	case LARGE:	return mapsize[3];
	case XLARGE:	return mapsize[4];
        default: break;
    }

    return mapsize[0];
}

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

bool Maps::isValidAbsIndex(const s16 i)
{
    return 0 <= i && i < world.w() * world.h();
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
s16 Maps::GetIndexFromAbsPoint(const Point & mp)
{
    return GetIndexFromAbsPoint(mp.x, mp.y);
}

s16 Maps::GetIndexFromAbsPoint(s16 px, s16 py)
{
    return py * world.w() + px;
}

u16 Maps::GetDirectionAroundFog(const u16 center, const u8 color)
{
    if(!isValidAbsPoint(center % world.w(), center / world.h())) return 0;

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

void Maps::ClearFog(u16 index, u8 scoute, const u8 color)
{
    if(0 == scoute) return;
    const Point center(index % world.w(), index / world.w());

    // AI advantage
    if(Game::AI == world.GetKingdom(color).Control())
    {
	switch(Settings::Get().GameDifficulty())
	{
    	    case Difficulty::HARD:	scoute += 2; break;
    	    case Difficulty::EXPERT:	scoute += 4; break;
    	    case Difficulty::IMPOSSIBLE:scoute += 6; break;
	    default: break;
	}
    }

    for(s16 y = center.y - scoute; y <= center.y + scoute; ++y)
        for(s16 x = center.x - scoute; x <= center.x + scoute; ++x)
            if(isValidAbsPoint(x, y) &&  (scoute + scoute / 2) >= std::abs(x - center.x) + std::abs(y - center.y))
                world.GetTiles(GetIndexFromAbsPoint(x, y)).ClearFog(color);
}

bool Maps::ScanAroundObject(const u16 center, const u8 obj, bool full, u16 *res)
{
    const s16 cx = center % world.w();
    const s16 cy = center / world.w();
    u16 res2 = 0;

    for(s16 y = -1; y <= 1; ++y)
        for(s16 x = -1; x <= 1; ++x)
    {
            if((!y && !x) || (y && x && !full)) continue;

	    res2 = GetIndexFromAbsPoint(cx + x, cy + y);

	    if(isValidAbsPoint(cx + x, cy + y) &&
		obj == world.GetTiles(res2).GetObject())
	    {
		if(res) *res = res2;
		return true;
	    }
    }

    if(res) *res = MAXU16;
    return false;
}

bool Maps::ScanDistanceObject(const u16 center, const u8 obj, const u16 dist, u16 *res)
{
    const s16 cx = center % world.w();
    const s16 cy = center / world.w();
    u16 res2 = 0;

    // from center to abroad
    for(u16 ii = 1; ii <= dist; ++ii)
    {
	const s16 tx = cx - ii;
	const s16 ty = cy - ii;

	const s16 mx = tx + 2 * ii;
	const s16 my = ty + 2 * ii;

	for(s16 iy = ty; iy <= my; ++iy)
	    for(s16 ix = tx; ix <= mx; ++ix)
	{
	    if(ty < iy && iy < my && tx < ix && ix < mx) continue;

	    res2 = GetIndexFromAbsPoint(ix, iy);

	    if(isValidAbsPoint(ix, iy) &&
		obj == world.GetTiles(res2).GetObject())
	    {
		if(res) *res = res2;
		return true;
	    }
	}
    }

    if(res) *res = MAXU16;
    return false;
}

u16 Maps::GetApproximateDistance(const u16 index1, const u16 index2)
{
    return std::max(std::abs(static_cast<s32>(index1 % world.w()) - static_cast<s32>(index2 % world.w())), 
	            std::abs(static_cast<s32>(index1 / world.w()) - static_cast<s32>(index2 / world.w())));
}


void Maps::MinimizeAreaForCastle(const Point & center)
{
    // reset castle ID
    for(s8 yy = -3; yy < 2; ++yy)
        for(s8 xx = -2; xx < 3; ++xx)
    {
        Maps::Tiles & tile = world.GetTiles((center.y + yy) * world.h() + center.x + xx);

        if(MP2::OBJN_RNDCASTLE == tile.GetObject() ||
             MP2::OBJN_RNDTOWN == tile.GetObject() ||
             MP2::OBJN_CASTLE  == tile.GetObject()) tile.SetObject(MP2::OBJ_ZERO);
    }

    // set minimum area castle ID
    for(s8 yy = -1; yy < 1; ++yy)
        for(s8 xx = -2; xx < 3; ++xx)
    {
        Maps::Tiles & tile = world.GetTiles((center.y + yy) * world.h() + center.x + xx);

        tile.SetObject(MP2::OBJN_CASTLE);
    }

    // restore center ID
    world.GetTiles(center).SetObject(MP2::OBJ_CASTLE);
}

/* correct sprites for RND castles */
void Maps::UpdateRNDSpriteForCastle(const Point & center, u8 race, bool castle)
{
/* 
castle size: T and B - sprite, S - shadow, XX - center

              T0
      S1S1T1T1T1T1T1
    S2S2S2T2T2T2T2T2
      S3S3B1B1XXB1B1
        S4B2B2  B2B2
*/
    std::vector<u16> coords;
    coords.reserve(21);

    // T0
    if(castle) coords.push_back(GetIndexFromAbsPoint(center.x, center.y - 3));
    // T1
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y - 2));
    // T2
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y - 1));
    // B1
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y));
    // B2
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y + 1));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y + 1));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y + 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y + 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y + 1));

    Maps::Tiles & tile_center = world.GetTiles(center);

    // correct only RND town and castle
    switch(tile_center.GetObject())
    {
	case MP2::OBJ_RNDTOWN:
	case MP2::OBJ_RNDCASTLE:
	    break;
	
	default:
	    DEBUG(DBG_GAME , DBG_WARN, "Maps::UpdateRNDSpriteForCastle: correct only RND town and castle. index: " << GetIndexFromAbsPoint(center.x, center.y));
	    return;
    }

    // modify all rnd sprites
    std::vector<u16>::const_iterator it1 = coords.begin();
    std::vector<u16>::const_iterator it2 = coords.end();

    for(; it1 != it2; ++it1) if(isValidAbsIndex(*it1))
    {
	Maps::TilesAddon *addon = world.GetTiles(*it1).FindRNDCastle();
	if(addon)
	{
    	    addon->object -= 12;

    	    switch(race)
    	    {
    		case Race::KNGT: break;
        	case Race::BARB: addon->index += 32; break;
        	case Race::SORC: addon->index += 64; break;
        	case Race::WRLK: addon->index += 96; break;
        	case Race::WZRD: addon->index += 128; break;
        	case Race::NECR: addon->index += 160; break;
        	default: break;
	    }
	}
    }
}

void Maps::UpdateSpritesFromTownToCastle(const Point & center)
{
    // correct area maps sprites
    std::vector<u16> coords;
    coords.reserve(15);

    // T1
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y - 2));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y - 2));
    // T2
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y - 1));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y - 1));
    // B1
    coords.push_back(GetIndexFromAbsPoint(center.x - 2, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x - 1, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x + 1, center.y));
    coords.push_back(GetIndexFromAbsPoint(center.x + 2, center.y));

    // modify all town sprites
    std::vector<u16>::const_iterator it1 = coords.begin();
    std::vector<u16>::const_iterator it2 = coords.end();
    for(; it1 != it2; ++it1) if(isValidAbsIndex(*it1))
    {
	TilesAddon *addon = world.GetTiles(*it1).FindCastle();
	if(addon) addon->index -= 16;
    }

    // T0
    if(isValidAbsIndex(GetIndexFromAbsPoint(center.x, center.y - 3) && isValidAbsIndex(GetIndexFromAbsPoint(center.x, center.y - 2))))
    {
	TilesAddon *addon = world.GetTiles(GetIndexFromAbsPoint(center.x, center.y - 2)).FindCastle();
	if(addon)
	    world.GetTiles(GetIndexFromAbsPoint(center.x, center.y - 3)).AddonsPushLevel2(TilesAddon(addon->level, addon->uniq, addon->object, addon->index - 3));
    }
}

bool Maps::TileUnderProtection(const u16 index, u16 *res)
{
    return Maps::ScanAroundObject(index, MP2::OBJ_MONSTER, Settings::Get().Original(), res);
}
