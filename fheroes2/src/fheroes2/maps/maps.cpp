/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <cstdlib>
#include <algorithm>
#include "world.h"
#include "settings.h"
#include "maps.h"
#include "race.h"
#include "kingdom.h"
#include "difficulty.h"
#include "maps_tiles.h"

struct ComparsionDistance
{
    ComparsionDistance(const s32 & index) : center(index)
    {
    }

    bool operator() (const s32 & index1, const s32 & index2) const
    {
        return Maps::GetApproximateDistance(center, index1) < Maps::GetApproximateDistance(center, index2);
    }

    s32 center;
};

bool TileIsObject(s32 index, u8 obj)
{
    return obj == world.GetTiles(index).GetObject();
}

bool TileIsObjects(s32 index, const u8* objs)
{
    while(*objs)
    {
	if(*objs == world.GetTiles(index).GetObject()) return true;
	++objs;
    }
    return false;
}

MapsIndexes & MapsIndexesFilteredObjects(MapsIndexes & indexes, const u8* objs)
{
    indexes.resize(std::distance(indexes.begin(),
	    std::remove_if(indexes.begin(), indexes.end(), std::not1(std::bind2nd(std::ptr_fun(&TileIsObjects), objs)))));
    return indexes;
}

MapsIndexes & MapsIndexesFilteredObject(MapsIndexes & indexes, u8 obj)
{
    indexes.resize(std::distance(indexes.begin(),
	    std::remove_if(indexes.begin(), indexes.end(), std::not1(std::bind2nd(std::ptr_fun(&TileIsObject), obj)))));
    return indexes;
}

const char* Maps::SizeString(u16 s)
{
    const char* mapsize[] = { "Unknown", _("maps|Small"), _("maps|Medium"), _("maps|Large"), _("maps|Extra Large"), "size256", "size320" };

    switch(s)
    {
	case SMALL:	return mapsize[1];
	case MEDIUM:	return mapsize[2];
	case LARGE:	return mapsize[3];
	case XLARGE:	return mapsize[4];
	case XLARGE2:	return mapsize[5];
	case XLARGE3:	return mapsize[6];
        default: break;
    }

    return mapsize[0];
}

const char* Maps::GetMinesName(u8 type)
{
    switch(type)
    {
	case Resource::ORE:	return _("Ore Mine");
        case Resource::SULFUR:	return _("Sulfur Mine");
        case Resource::CRYSTAL:	return _("Crystal Mine");
        case Resource::GEMS:	return _("Gems Mine");
        case Resource::GOLD:	return _("Gold Mine");
	default: break;
    }

    return _("Mine");
}

s32 Maps::GetDirectionIndex(const s32 & from, u16 vector)
{
    switch(vector)
    {
	case Direction::TOP:		return from - world.w();
	case Direction::TOP_RIGHT:	return from - world.w() + 1;
	case Direction::RIGHT:		return from + 1;
	case Direction::BOTTOM_RIGHT:	return from + world.w() + 1;
	case Direction::BOTTOM:		return from + world.w();
	case Direction::BOTTOM_LEFT:	return from + world.w() - 1;
	case Direction::LEFT:		return from - 1;
	case Direction::TOP_LEFT:	return from - world.w() - 1;
	default: break;
    }

    return -1;
}

// check bound
bool Maps::isValidDirection(const s32 & from, u16 vector)
{
    switch(vector)
    {
	case Direction::TOP:            return (from >= world.w());
	case Direction::RIGHT:          return ((from % world.w()) < (world.w() - 1));
	case Direction::BOTTOM:         return (from < world.w() * (world.h() - 1));
	case Direction::LEFT:           return (from % world.w());

	case Direction::TOP_RIGHT:
	    return isValidDirection(from, Direction::TOP) && isValidDirection(from, Direction::RIGHT);

	case Direction::BOTTOM_RIGHT:
	    return isValidDirection(from, Direction::BOTTOM) && isValidDirection(from, Direction::RIGHT);

	case Direction::BOTTOM_LEFT:
	    return isValidDirection(from, Direction::BOTTOM) && isValidDirection(from, Direction::LEFT);

	case Direction::TOP_LEFT:
	    return isValidDirection(from, Direction::TOP) && isValidDirection(from, Direction::LEFT);

	default: break;
    }

    return false;
}

bool Maps::isValidAbsPoint(const Point & pt)
{
    return isValidAbsPoint(pt.x, pt.y);
}

bool Maps::isValidAbsIndex(const s32 & ii)
{
    return 0 <= ii && ii < world.w() * world.h();
}

bool Maps::isValidAbsPoint(s16 x, s16 y)
{
    return 0 <= x && world.w() > x && 0 <= y && world.h() > y;
}

/* convert maps point to index maps */
s32 Maps::GetIndexFromAbsPoint(const Point & mp)
{
    return GetIndexFromAbsPoint(mp.x, mp.y);
}

s32 Maps::GetIndexFromAbsPoint(s16 px, s16 py)
{
    s32 res = py * world.w() + px;

    if(px < 0 || py < 0)
    {
	VERBOSE("Maps::GetIndexFromAbsPoint: error coods, " << "x: " << px << ", y: " << py);
	return -1;
    }

    return res;
}

MapsIndexes Maps::GetAroundIndexes(const s32 & center, u16 filter)
{
    MapsIndexes result;
    result.reserve(8);

    if(isValidAbsIndex(center))
	for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	    if((filter & direct) && 
	    isValidDirection(center, direct))
		result.push_back(GetDirectionIndex(center, direct));

    return result;
}

MapsIndexes Maps::GetDistanceIndexes(const s32 & center, u16 dist, bool sort)
{
    MapsIndexes results;
    results.reserve(dist * 12);

    const s16 cx = center % world.w();
    const s16 cy = center / world.w();

    for(s16 xx = cx - dist; xx <= cx + dist; ++xx)
	for(s16 yy = cy - dist; yy <= cy + dist; ++yy)
    {
	if(xx != cx && yy != cy &&
	    isValidAbsPoint(xx, yy))
	    results.push_back(GetIndexFromAbsPoint(xx, yy));
    }

    if(sort)
	std::sort(results.begin(), results.end(), ComparsionDistance(center));

    return results;
}

void Maps::ClearFog(const s32 & index, u8 scoute, u8 color)
{
    if(0 != scoute && isValidAbsIndex(index))
    {
	const Point center(index % world.w(), index / world.w());
	const Settings & conf = Settings::Get();

	// AI advantage
	if(CONTROL_AI & world.GetKingdom(color).GetControl())
	{
	    switch(conf.GameDifficulty())
	    {
    		case Difficulty::NORMAL:	scoute += 2; break;
    		case Difficulty::HARD:		scoute += 3; break;
    		case Difficulty::EXPERT:	scoute += 4; break;
    		case Difficulty::IMPOSSIBLE:	scoute += 6; break;
		default: break;
	    }
	}

	u8 colors = conf.ExtUnionsAllowViewMaps() ? Players::GetPlayerFriends(color) : color;

	for(s16 y = center.y - scoute; y <= center.y + scoute; ++y)
    	    for(s16 x = center.x - scoute; x <= center.x + scoute; ++x)
        	if(isValidAbsPoint(x, y) &&
		    (scoute + scoute / 2) >= std::abs(x - center.x) + std::abs(y - center.y))
            	    world.GetTiles(GetIndexFromAbsPoint(x, y)).ClearFog(colors);
    }
}

MapsIndexes Maps::ScanAroundObjectsV(const s32 & center, const u8* objs)
{
    MapsIndexes results = Maps::GetAroundIndexes(center);
    return MapsIndexesFilteredObjects(results, objs);
}

MapsIndexes Maps::ScanAroundObjectV(const s32 & center, u8 obj)
{
    MapsIndexes results = Maps::GetAroundIndexes(center);
    return MapsIndexesFilteredObject(results, obj);
}

MapsIndexes Maps::ScanDistanceObject(const s32 & center, u8 obj, u16 dist)
{
    MapsIndexes results = Maps::GetDistanceIndexes(center, dist, true);
    return MapsIndexesFilteredObject(results, obj);
}

MapsIndexes Maps::ScanDistanceObjects(const s32 & center, const u8* objs, u16 dist)
{
    MapsIndexes results = Maps::GetDistanceIndexes(center, dist, true);
    return MapsIndexesFilteredObjects(results, objs);
}

bool MapsTileIsUnderProtection(const s32 from, const s32 index) /* from: center, index: monster */
{
    bool result = false;
    const Maps::Tiles & tile1 = world.GetTiles(from);
    const Maps::Tiles & tile2 = world.GetTiles(index);

    if(tile1.isWater() == tile2.isWater())
    {
	/* if monster can attack to */
	result = (tile2.GetPassable() & Direction::Get(index, from)) &&
		(tile1.GetPassable() & Direction::Get(from, index));
    }

    return result;
}

bool Maps::TileIsUnderProtection(const s32 & center)
{
    return MP2::OBJ_MONSTER == world.GetTiles(center).GetObject() ? true :
	    GetTilesUnderProtection(center).size();
}

MapsIndexes Maps::GetTilesUnderProtection(const s32 & center)
{
    MapsIndexes indexes = Maps::ScanAroundObjectV(center, MP2::OBJ_MONSTER);

    indexes.resize(std::distance(indexes.begin(),
	    std::remove_if(indexes.begin(), indexes.end(),
		std::not1(std::bind1st(std::ptr_fun(&MapsTileIsUnderProtection), center)))));

    if(MP2::OBJ_MONSTER == world.GetTiles(center).GetObject())
	indexes.push_back(center);

    return indexes;
}

u16 Maps::GetApproximateDistance(const s32 & index1, const s32 & index2)
{
    return std::max(std::abs((index1 % world.w()) - (index2 % world.w())), 
	            std::abs((index1 / world.w()) - (index2 / world.w())));
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

	// skip angle
	if(yy == -1 && (xx == -2 || xx == 2)) continue;

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
    MapsIndexes coords;
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
	    DEBUG(DBG_GAME, DBG_WARN, "incorrect object" << ", index: " << GetIndexFromAbsPoint(center.x, center.y));
	    return;
    }

    // modify all rnd sprites
    for(MapsIndexes::const_iterator
	it = coords.begin(); it != coords.end(); ++it)
    if(isValidAbsIndex(*it))
    {
	Maps::TilesAddon *addon = world.GetTiles(*it).FindObject(MP2::OBJ_RNDCASTLE);
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
    MapsIndexes coords;
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
    for(MapsIndexes::const_iterator
	it = coords.begin(); it != coords.end(); ++it)
    if(isValidAbsIndex(*it))
    {
	TilesAddon *addon = world.GetTiles(*it).FindObject(MP2::OBJ_CASTLE);
	if(addon) addon->index -= 16;
    }

    // T0
    if(isValidAbsIndex(GetIndexFromAbsPoint(center.x, center.y - 3) && isValidAbsIndex(GetIndexFromAbsPoint(center.x, center.y - 2))))
    {
	TilesAddon *addon = world.GetTiles(GetIndexFromAbsPoint(center.x, center.y - 2)).FindObject(MP2::OBJ_CASTLE);
	if(addon)
	    world.GetTiles(GetIndexFromAbsPoint(center.x, center.y - 3)).AddonsPushLevel2(TilesAddon(addon->level, addon->uniq, addon->object, addon->index - 3));
    }
}

u16  Maps::TileIsCoast(const s32 & index, u16 direct)
{
    u16 result = 0;
    const MapsIndexes & v = GetAroundIndexes(index, direct);
    for(MapsIndexes::const_iterator
        it = v.begin(); it != v.end(); ++it)
        if(world.GetTiles(*it).isWater()) result |= Direction::Get(index, *it);
    return result;
}
