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

#include <list>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "agg.h"
#include "world.h"
#include "race.h"
#include "settings.h"
#include "heroes.h"
#include "castle.h"
#include "maps.h"
#include "interface_gamearea.h"
#include "game_focus.h"
#include "object.h"
#include "mounts.h"
#include "trees.h"
#include "objxloc.h"
#include "objtown.h"
#include "objwatr.h"
#include "objlava.h"
#include "objmult.h"
#include "objdsrt.h"
#include "objdirt.h"
#include "objsnow.h"
#include "objswmp.h"
#include "objgras.h"
#include "objcrck.h"
#include "monster.h"
#include "spell.h"
#include "resource.h"
#include "maps_tiles.h"

static u8 monster_animation_cicle[] = { 0, 1, 2, 1, 0, 3, 4, 5, 4, 3 };

Maps::TilesAddon::TilesAddon() : level(0), uniq(0), object(0), index(0)
{
}

Maps::TilesAddon::TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii) : level(lv), uniq(gid), object(obj), index(ii)
{
}

std::string Maps::TilesAddon::String(int level) const
{
    std::ostringstream os;
    os << "----------------" << level << "--------" << std::endl <<
	  "object          : " << "0x" << std::setw(2) << std::setfill('0') << static_cast<int>(object) <<
				", (" << ICN::GetString(MP2::GetICNObject(object)) << ")" << std::endl <<
	  "index           : " << static_cast<int>(index) << std::endl <<
	  "uniq            : " << uniq << std::endl <<
	  "level           : " << static_cast<int>(level) << std::endl;
    return os.str();
}

Maps::TilesAddon & Maps::TilesAddon::operator= (const Maps::TilesAddon & ta)
{
    level = ta.level;
    object = ta.object;
    index = ta.index;
    uniq = ta.uniq;
    
    return *this;
}

bool Maps::TilesAddon::isUniq(u32 id) const
{
    return uniq == id;
}

bool Maps::TilesAddon::isICN(u16 icn) const
{
    return icn == MP2::GetICNObject(object);
}

bool Maps::TilesAddon::PredicateSortRules1(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) > (ta2.level % 4));
}

bool Maps::TilesAddon::PredicateSortRules2(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) < (ta2.level % 4));
}

bool Maps::TilesAddon::isPassable(const Maps::TilesAddon & ta, u16 direct, s32 maps_index)
{
    ICN::icn_t icn = MP2::GetICNObject(ta.object);
    const u16 & index = ta.index;

    switch(icn)
    {
	case ICN::MTNCRCK:
	case ICN::MTNSNOW:
	case ICN::MTNSWMP:
	case ICN::MTNLAVA:
	case ICN::MTNDSRT:
	case ICN::MTNDIRT:
	case ICN::MTNMULT:
	case ICN::MTNGRAS:	if(! Mounts::isPassable(icn, index, direct)) return false; break;

	case ICN::TREJNGL:
	case ICN::TREEVIL:
	case ICN::TRESNOW:
	case ICN::TREFIR:
	case ICN::TREFALL:
	case ICN::TREDECI:	if(! Trees::isPassable(icn, index, direct)) return false; break;

	case ICN::OBJNSNOW:	if(! ObjSnow::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNSWMP:	if(! ObjSwamp::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNGRAS:
	case ICN::OBJNGRA2:	if(! ObjGrass::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNCRCK:	if(! ObjWasteLand::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNDIRT:	if(! ObjDirt::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNDSRT:	if(! ObjDesert::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNMUL2:
	case ICN::OBJNMULT:	if(! ObjMulti::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNLAVA:
	case ICN::OBJNLAV3:
	case ICN::OBJNLAV2:	if(! ObjLava::isPassable(icn, index, direct)) return false; break;
	case ICN::OBJNWAT2:
	case ICN::OBJNWATR:	if(! ObjWater::isPassable(icn, index, direct)) return false; break;

	case ICN::MONS32:
	case ICN::MINIMON:	return false;

	case ICN::OBJNARTI:
	case ICN::OBJNRSRC:	if(index % 2) return false; break;

	case ICN::OBJNTWBA:
	case ICN::OBJNTOWN:	if(! ObjTown::isPassable(icn, index, direct, maps_index)) return false; break;

	case ICN::X_LOC1:
	case ICN::X_LOC2:
	case ICN::X_LOC3:	if(! ObjLoyalty::isPassable(icn, index, direct)) return false; break;

	// MANUAL.ICN
	case ICN::TREASURE:
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3:
	case ICN::FOUNTAIN:	return false; break;

	default: break;
    }

    return true;
}

bool Maps::TilesAddon::isRoad(u16 direct) const
{
    switch(MP2::GetICNObject(object))
    {
	// castle and tower (gate)
	case ICN::OBJNTOWN:
	    if(13 == index ||
		29 == index ||
	    	45 == index ||
	    	61 == index ||
	    	77 == index ||
	    	93 == index ||
	    	109 == index ||
	    	125 == index ||
	    	141 == index ||
	    	157 == index ||
	    	173 == index ||
	    	189 == index) return direct & (Direction::TOP | Direction::BOTTOM);

	// castle lands (gate)
        case ICN::OBJNTWBA:
	    if(7 == index ||
		17 == index ||
		27 == index ||
		37 == index ||
		47 == index ||
		57 == index ||
		67 == index ||
		77 == index) return direct & (Direction::TOP | Direction::BOTTOM);

	// from sprite road
	case ICN::ROAD:
	    if(0  == index ||
	       4  == index ||
	       5  == index ||
	       7  == index ||
	       9  == index ||
	       12 == index ||
	       13 == index ||
	       16 == index ||
	       19 == index ||
	       20 == index ||
	       26 == index) return direct & (Direction::TOP | Direction::BOTTOM);
	    else
	    if(2  == index ||
	       21 == index ||
	       28 == index) return direct & (Direction::RIGHT | Direction::LEFT);
	    else
	    if(3  == index ||
	       6  == index ||
	       14 == index) return direct & (Direction::TOP | Direction::BOTTOM | Direction::RIGHT | Direction::LEFT);
	    else
	    if(17 == index ||
	       29 == index) return direct & (Direction::TOP_LEFT | Direction::BOTTOM_RIGHT);
	    else
	    if(18 == index ||
	       30 == index) return direct & (Direction::TOP_RIGHT | Direction::BOTTOM_LEFT);
	
	default:
	    break;
    }

    return false;
}

bool Maps::TilesAddon::isStream(const TilesAddon & ta)
{
    return ICN::STREAM == MP2::GetICNObject(ta.object);
}

bool Maps::TilesAddon::isWaterResource(const TilesAddon & ta)
{
    return (ICN::OBJNWATR == MP2::GetICNObject(ta.object) &&
                (0 == ta.index ||    // buttle
                19 == ta.index ||    // chest
                45 == ta.index ||    // flotsam
                111 == ta.index)     // surviror
            );
}

bool Maps::TilesAddon::isWhirlPool(const TilesAddon & ta)
{
    return (ICN::OBJNWATR == MP2::GetICNObject(ta.object) &&
                (ta.index >= 202 && ta.index <= 225));
}

bool Maps::TilesAddon::isStandingStone(const TilesAddon & ta)
{
    return (ICN::OBJNMULT == MP2::GetICNObject(ta.object) &&
                (ta.index == 84 || ta.index == 85));
}

bool Maps::TilesAddon::isResource(const TilesAddon & ta)
{
            // OBJNRSRC
    return ((ICN::OBJNRSRC == MP2::GetICNObject(ta.object) && (ta.index % 2)) ||
            // TREASURE
            (ICN::TREASURE == MP2::GetICNObject(ta.object)));
}

bool Maps::TilesAddon::isRandomResource(const TilesAddon & ta)
{
	    // OBJNRSRC
    return (ICN::OBJNRSRC == MP2::GetICNObject(ta.object) && 17 == ta.index);
}

bool Maps::TilesAddon::isArtifact(const TilesAddon & ta)
{
    // OBJNARTI (skip ultimate)
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && (ta.index > 0x10) && (ta.index % 2));
}

bool Maps::TilesAddon::isRandomArtifact(const TilesAddon & ta)
{
    // OBJNARTI
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && 0xA3 == ta.index);
}

bool Maps::TilesAddon::isRandomArtifact1(const TilesAddon & ta)
{
    // OBJNARTI
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && 0xA7 == ta.index);
}

bool Maps::TilesAddon::isRandomArtifact2(const TilesAddon & ta)
{
    // OBJNARTI
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && 0xA9 == ta.index);
}

bool Maps::TilesAddon::isRandomArtifact3(const TilesAddon & ta)
{
    // OBJNARTI
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && 0xAB == ta.index);
}

bool Maps::TilesAddon::isUltimateArtifact(const TilesAddon & ta)
{
    // OBJNARTI
    return (ICN::OBJNARTI == MP2::GetICNObject(ta.object) && 0xA4 == ta.index);
}

bool Maps::TilesAddon::isCampFire(const TilesAddon & ta)
{
	    // MTNDSRT
    return ((ICN::OBJNDSRT == MP2::GetICNObject(ta.object) && 61 == ta.index) ||
	    // OBJNMULT
            (ICN::OBJNMULT == MP2::GetICNObject(ta.object) && 131 == ta.index) ||
	    // OBJNSNOW
            (ICN::OBJNSNOW == MP2::GetICNObject(ta.object) && 4 == ta.index));
}

bool Maps::TilesAddon::isMonster(const TilesAddon & ta)
{
	    // MONS32
    return (ICN::MONS32 == MP2::GetICNObject(ta.object));
}

bool Maps::TilesAddon::isArtesianSpring(const TilesAddon & ta)
{
    return (ICN::OBJNCRCK == MP2::GetICNObject(ta.object) &&
	    (ta.index == 3 || ta.index == 4));
}

bool Maps::TilesAddon::isSkeleton(const TilesAddon & ta)
{
    return (ICN::OBJNDSRT == MP2::GetICNObject(ta.object) &&
	    (ta.index == 83 || ta.index == 84));
}

bool Maps::TilesAddon::isOasis(const TilesAddon & ta)
{
    return (ICN::OBJNDSRT == MP2::GetICNObject(ta.object) &&
		(ta.index == 108 || ta.index == 109));
}

bool Maps::TilesAddon::isWateringHole(const TilesAddon & ta)
{
    return (ICN::OBJNCRCK == MP2::GetICNObject(ta.object) &&
		(ta.index >= 217 && ta.index <= 220));
}

bool Maps::TilesAddon::isJail(const TilesAddon & ta)
{
    return (ICN::X_LOC2 == MP2::GetICNObject(ta.object) && 0x09 == ta.index);
}

bool Maps::TilesAddon::isEvent(const TilesAddon & ta)
{
    // OBJNMUL2
    return (ICN::OBJNMUL2 == MP2::GetICNObject(ta.object) && 0xA3 == ta.index);
}

bool Maps::TilesAddon::isMine(const TilesAddon & ta)
{
    // EXTRAOVR
    return (ICN::EXTRAOVR == MP2::GetICNObject(ta.object));
}

bool Maps::TilesAddon::isBoat(const TilesAddon & ta)
{
    // OBJNWAT2
    return (ICN::OBJNWAT2 == MP2::GetICNObject(ta.object) && 0x17 == ta.index);
}

bool Maps::TilesAddon::isMiniHero(const TilesAddon & ta)
{
    // MINIHERO
    return (ICN::MINIHERO == MP2::GetICNObject(ta.object));
}

bool Maps::TilesAddon::isCastle(const TilesAddon & ta)
{
    // OBJNTOWN
    return (ICN::OBJNTOWN == MP2::GetICNObject(ta.object));
}

bool Maps::TilesAddon::isRandomCastle(const TilesAddon & ta)
{
    // OBJNTWRD
    return (ICN::OBJNTWRD == MP2::GetICNObject(ta.object) && 32 > ta.index);
}

bool Maps::TilesAddon::isRandomMonster(const TilesAddon & ta)
{
    // MONS32
    return(ICN::MONS32 == MP2::GetICNObject(ta.object) &&
	    (0x41 < ta.index && 0x47 > ta.index));
}

bool Maps::TilesAddon::isBarrier(const TilesAddon & ta)
{
    return (ICN::X_LOC3 == MP2::GetICNObject(ta.object) && 
		(60 == ta.index ||
		66 == ta.index ||
		72 == ta.index ||
		78 == ta.index ||
		84 == ta.index ||
		90 == ta.index ||
		96 == ta.index ||
		102 == ta.index));
}

bool Maps::TilesAddon::isAbandoneMineSprite(const TilesAddon & ta)
{
    return (ICN::OBJNGRAS == MP2::GetICNObject(ta.object) && 6 == ta.index) ||
	    (ICN::OBJNDIRT == MP2::GetICNObject(ta.object) && 8 == ta.index);
}

bool Maps::TilesAddon::isFlag32(const TilesAddon & ta)
{
    return ICN::FLAG32 == MP2::GetICNObject(ta.object);
}

bool Maps::TilesAddon::isX_LOC123(const TilesAddon & ta)
{
    return (ICN::X_LOC1 == MP2::GetICNObject(ta.object) ||
	    ICN::X_LOC2 == MP2::GetICNObject(ta.object) ||
	    ICN::X_LOC3 == MP2::GetICNObject(ta.object));
}

void Maps::TilesAddon::UpdateAbandoneMineLeftSprite(TilesAddon & ta, u8 resource)
{
    if(ICN::OBJNGRAS == MP2::GetICNObject(ta.object) && 6 == ta.index)
    {
	ta.object = 128; // MTNGRAS
	ta.index = 82;
    }
    else
    if(ICN::OBJNDIRT == MP2::GetICNObject(ta.object) && 8 == ta.index)
    {
	ta.object = 104; // MTNDIRT
	ta.index = 112;
    }
    else
    if(ICN::EXTRAOVR == MP2::GetICNObject(ta.object) && 5 == ta.index)
    {
	switch(resource)
	{
	    case Resource::ORE:		ta.index = 0; break;
	    case Resource::SULFUR:	ta.index = 1; break;
	    case Resource::CRYSTAL:	ta.index = 2; break;
	    case Resource::GEMS:	ta.index = 3; break;
	    case Resource::GOLD:	ta.index = 4; break;
	    default: break;
	}
    }
}

void Maps::TilesAddon::UpdateAbandoneMineRightSprite(TilesAddon & ta)
{
    if(ICN::OBJNDIRT == MP2::GetICNObject(ta.object) && ta.index == 9)
    {
	ta.object = 104;
	ta.index = 113;
    }
    else
    if(ICN::OBJNGRAS == MP2::GetICNObject(ta.object) && ta.index == 7)
    {
	ta.object = 128;
	ta.index = 83;
    }
}

void Maps::TilesAddon::UpdateFountainSprite(TilesAddon & ta)
{
    if(ICN::OBJNMUL2 == MP2::GetICNObject(ta.object) && 15 == ta.index)
    {
	ta.object = 0x14;
	ta.index = 0;
    }
}

void Maps::TilesAddon::UpdateTreasureChestSprite(TilesAddon & ta)
{
    if(ICN::OBJNRSRC == MP2::GetICNObject(ta.object) && 19 == ta.index)
    {
	ta.object = 0x15;
	ta.index = 0;
    }
}

void Maps::TilesAddon::UpdateStoneLightsSprite(TilesAddon & ta, u8 & type)
{
    if(ICN::OBJNMUL2 == MP2::GetICNObject(ta.object))
    switch(ta.index)
    {
	case 116: ta.object = 0x11; ta.index = 0; type = 1; break;
	case 119: ta.object = 0x12; ta.index = 0; type = 2; break;
	case 122: ta.object = 0x13; ta.index = 0; type = 3; break;
	default: break;
    }
}


/* Maps::Addons */

void Maps::Addons::Remove(u32 uniq)
{
    /*
    erase(std::remove_if(begin(), end(),
	    std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isUniq), uniq)),
	    end());
    */
    remove_if(std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isUniq), uniq));
}


/* Maps::Tiles */
Maps::Tiles::Tiles() : maps_index(0), tile_sprite_index(0), tile_sprite_shape(0),
    mp2_object(0), quantity1(0), quantity2(0), quantity3(0), quantity4(0), fogs(0xFF), quantity5(0), quantity6(0), quantity7(0)
{
}

void Maps::Tiles::Init(s32 index, const MP2::mp2tile_t & mp2)
{
    maps_index	= index;
    tile_sprite_index	= mp2.tileIndex;
    tile_sprite_shape	= mp2.shape;
    mp2_object	= mp2.generalObject;
    quantity1	= mp2.quantity1;
    quantity2	= mp2.quantity2;
    quantity3	= 0;
    quantity4	= 0;
    fogs	= 0xFF;
    quantity5	= 0;
    quantity6	= 0;
    quantity7	= 0;

    addons_level1.clear();
    addons_level2.clear();

    AddonsPushLevel1(mp2);
    AddonsPushLevel2(mp2);

    if(IS_DEVEL()) ClearFog(Players::HumanColors());
}

void Maps::Tiles::SetTile(const u16 sprite_index, const u8 sh)
{
    tile_sprite_index = sprite_index;
    tile_sprite_shape = sh;
}

const Surface & Maps::Tiles::GetTileSurface(void) const
{
    return AGG::GetTIL(TIL::GROUND32, tile_sprite_index, tile_sprite_shape);
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2tile_t & mt)
{
    if(mt.objectName1 && mt.indexName1 < 0xFF)
	addons_level1.push_back(TilesAddon(0, mt.uniqNumber1, mt.objectName1, mt.indexName1));
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN1 && ma.indexNameN1 < 0xFF)
	addons_level1.push_back(TilesAddon(ma.quantityN, ma.uniqNumberN1, ma.objectNameN1, ma.indexNameN1));
}

void Maps::Tiles::AddonsPushLevel1(const TilesAddon & ta)
{
    addons_level1.push_back(ta);
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2tile_t & mt)
{
    if(mt.objectName2 && mt.indexName2 < 0xFF)
	addons_level2.push_back(TilesAddon(0, mt.uniqNumber2, mt.objectName2, mt.indexName2));
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN2 && ma.indexNameN2 < 0xFF)
	addons_level2.push_back(TilesAddon(ma.quantityN, ma.uniqNumberN2, ma.objectNameN2, ma.indexNameN2));
}

void Maps::Tiles::AddonsPushLevel2(const TilesAddon & ta)
{
    addons_level2.push_back(ta);
}

void Maps::Tiles::AddonsSort(void)
{
    if(!addons_level1.empty()) addons_level1.sort(Maps::TilesAddon::PredicateSortRules1);
    if(!addons_level2.empty()) addons_level2.sort(Maps::TilesAddon::PredicateSortRules2);
}

Maps::Ground::ground_t Maps::Tiles::GetGround(void) const
{
    // list grounds from GROUND32.TIL
    if(30 > tile_sprite_index)
        return Maps::Ground::WATER;
        
    else if(92 > tile_sprite_index)
        return Maps::Ground::GRASS;

    else if(146 > tile_sprite_index)
        return Maps::Ground::SNOW;

    else if(208 > tile_sprite_index)
        return Maps::Ground::SWAMP;

    else if(262 > tile_sprite_index)
        return Maps::Ground::LAVA;

    else if(321 > tile_sprite_index)
        return Maps::Ground::DESERT;

    else if(361 > tile_sprite_index)
        return Maps::Ground::DIRT;

    else if(415 > tile_sprite_index)
        return Maps::Ground::WASTELAND;

    //else if(432 > tile_sprite_index)

	return Maps::Ground::BEACH;
}

void Maps::Tiles::Remove(u32 uniq)
{
    if(!addons_level1.empty()) addons_level1.Remove(uniq);
    if(!addons_level2.empty()) addons_level2.Remove(uniq);
}

void Maps::Tiles::RedrawTile(Surface & dst) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area.GetRectMaps() & mp)
	area.BlitOnTile(dst, GetTileSurface(), 0, 0, mp);
}

void Maps::Tiles::RedrawBottom(Surface & dst, bool skip_objs) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if((area.GetRectMaps() & mp) &&
	!addons_level1.empty())
    {
	Addons::const_iterator it1 = addons_level1.begin();
	Addons::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    // skip
	    if(skip_objs &&
		MP2::isRemoveObject(mp2_object) &&
		FindObject(mp2_object) == &(*it1)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		area.BlitOnTile(dst, sprite, mp);

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket(), quantity2))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    area.BlitOnTile(dst, anime_sprite, mp);
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawObjects(Surface & dst) const
{
    switch(mp2_object)
    {
        // boat
        case MP2::OBJ_BOAT:	RedrawBoat(dst); break;
        // monster
        case MP2::OBJ_MONSTER:	RedrawMonster(dst); break;
	//
	default: break;
    }
}

void Maps::Tiles::RedrawMonster(Surface & dst) const
{
    const Settings & conf = Settings::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());
    const Interface::GameArea & area = Interface::GameArea::Get();
    s32 dst_index = -1;

    if(!(area.GetRectMaps() & mp)) return;

    // scan hero around
    const u16 dst_around = Maps::ScanAroundObject(maps_index, MP2::OBJ_HEROES);
    for(Direction::vector_t dir = Direction::TOP_LEFT; dir < Direction::CENTER; ++dir) if(dst_around & dir)
    {
	dst_index = Maps::GetDirectionIndex(maps_index, dir);
	const Heroes* hero = world.GetHeroes(dst_index);

	if(!hero ||
	    // skip bottom, bottom_right, bottom_left with ground objects
	    (((Direction::BOTTOM | Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT) & dir) && MP2::isGroundObject(hero->GetUnderObject())))
	    dst_index = -1;
	else
	    break;
    }

    const Monster mons(quantity3);

    // draw attack sprite
    if(-1 != dst_index && !conf.ExtOnlyFirstMonsterAttack())
    {
	bool revert = false;

	switch(Direction::Get(maps_index, dst_index))
	{
	    case Direction::TOP_LEFT:
	    case Direction::LEFT:
	    case Direction::BOTTOM_LEFT:	revert = true;
	    default: break;
	}

	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, mons.GetSpriteIndex() * 9 + (revert ? 8 : 7));
	area.BlitOnTile(dst, sprite_first, sprite_first.x() + 16, TILEWIDTH + sprite_first.y(), mp);
    }
    else
    {
	// draw first sprite
	const Sprite & sprite_first = AGG::GetICN(ICN::MINIMON, mons.GetSpriteIndex() * 9);
	area.BlitOnTile(dst, sprite_first, sprite_first.x() + 16, TILEWIDTH + sprite_first.y(), mp);

	// draw second sprite
	const Sprite & sprite_next = AGG::GetICN(ICN::MINIMON, mons.GetSpriteIndex() * 9 + 1 + 
	    monster_animation_cicle[ (Maps::AnimationTicket() + mp.x * mp.y) % (sizeof(monster_animation_cicle) / sizeof(monster_animation_cicle[0])) ]);
	area.BlitOnTile(dst, sprite_next, sprite_next.x() + 16, TILEWIDTH + sprite_next.y(), mp);
    }
}

void Maps::Tiles::RedrawBoat(Surface & dst) const
{
    const Point mp(maps_index % world.w(), maps_index / world.w());
    const Interface::GameArea & area = Interface::GameArea::Get();

    if(!(area.GetRectMaps() & mp)) return;

    if(Settings::Get().Editor())
	area.BlitOnTile(dst, AGG::GetICN(ICN::OBJNWAT2, 23), 0, 0, mp);
    else
    {
        // FIXME: restore direction from Maps::Tiles
        const Sprite & sprite = AGG::GetICN(ICN::BOAT32, 18);
	area.BlitOnTile(dst, sprite, sprite.x(), TILEWIDTH + sprite.y(), mp);
    }
}

void Maps::Tiles::RedrawBottom4Hero(Surface & dst) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if((area.GetRectMaps() & mp) &&
	!addons_level1.empty())
    {
	Addons::const_iterator it1 = addons_level1.begin();
	Addons::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::SkipBottomForRedrawHeroes(icn, index)) continue;

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		area.BlitOnTile(dst, sprite, mp);

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket(), quantity2))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    area.BlitOnTile(dst, anime_sprite, mp);
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawTop(Surface & dst, const TilesAddon* skip) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(!(area.GetRectMaps() & mp)) return;

    // fix for haut mine
    if(MP2::OBJ_MINES == mp2_object)
    {
	if(quantity4 == Spell::HAUNT)
	{
    	    const Sprite & anime_sprite = AGG::GetICN(ICN::OBJNHAUN,  Maps::AnimationTicket() % 15);
	    area.BlitOnTile(dst, anime_sprite, mp);
	}
	else
	if(quantity4 >= Spell::SETEGUARDIAN && quantity4 <= Spell::SETWGUARDIAN)
	{
	    const Spell spell(quantity4);
	    const Monster mons(spell);
	    if(mons.isValid())
		area.BlitOnTile(dst, AGG::GetICN(ICN::MONS32, mons.GetSpriteIndex()), TILEWIDTH, 0, mp);
	}
    }

    if(!addons_level2.empty())
    {
	Addons::const_iterator it1 = addons_level2.begin();
	Addons::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    if(skip && skip == &(*it1)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		area.BlitOnTile(dst, sprite, mp);

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    area.BlitOnTile(dst, anime_sprite, mp);
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawTop4Hero(Surface & dst, bool skip_ground) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(!(area.GetRectMaps() & mp)) return;

    if(!addons_level2.empty())
    {
	Addons::const_iterator it1 = addons_level2.begin();
	Addons::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    if(skip_ground && MP2::isGroundObject((*it1).object)) continue;

	    const u8 & object = (*it1).object;
	    const u8 & index  = (*it1).index;
	    const ICN::icn_t icn = MP2::GetICNObject(object);

	    if(ICN::HighlyObjectSprite(icn, index))
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		area.BlitOnTile(dst, sprite, mp);

		// possible anime
		if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    area.BlitOnTile(dst, anime_sprite, mp);
		}
	    }
	}
    }
}

Maps::TilesAddon* Maps::Tiles::FindAddonICN1(u16 icn1)
{
    Addons::iterator it = std::find_if(addons_level1.begin(), addons_level1.end(),
			    std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isICN), icn1));

    return it != addons_level1.end() ? &(*it) : NULL;
}

Maps::TilesAddon* Maps::Tiles::FindAddonICN2(u16 icn2)
{
    Addons::iterator it = std::find_if(addons_level2.begin(), addons_level2.end(),
			    std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isICN), icn2));

    return it != addons_level2.end() ? &(*it) : NULL;
}

Maps::TilesAddon* Maps::Tiles::FindAddonLevel1(u32 uniq1)
{
    Addons::iterator it = std::find_if(addons_level1.begin(), addons_level1.end(),
			    std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isUniq), uniq1));

    return it != addons_level1.end() ? &(*it) : NULL;
}

Maps::TilesAddon* Maps::Tiles::FindAddonLevel2(u32 uniq2)
{
    Addons::iterator it = std::find_if(addons_level2.begin(), addons_level2.end(),
			    std::bind2nd(std::mem_fun_ref(&Maps::TilesAddon::isUniq), uniq2));

    return it != addons_level2.end() ? &(*it) : NULL;
}


std::string Maps::Tiles::String(void) const
{
    std::ostringstream os;

    os <<
	"----------------:--------" << std::endl <<
	"maps index      : " << maps_index << std::endl <<
	"tile            : " << tile_sprite_index << std::endl <<
	"ground          : " << Ground::String(GetGround()) << (isRoad() ? ", (road)" : "") << std::endl <<
	"passable        : " << (isPassable(NULL, Direction::UNKNOWN, false) ? "true" : "false") << std::endl <<
	"mp2 object      : " << "0x" << std::setw(2) << std::setfill('0') << static_cast<int>(mp2_object) <<
				    ", (" << MP2::StringObject(mp2_object) << ")" << std::endl <<
	"quantity 1      : " << static_cast<int>(quantity1) << std::endl <<
	"quantity 2      : " << static_cast<int>(quantity2) << std::endl <<
	"quantity 3      : " << static_cast<int>(quantity3) << std::endl <<
	"quantity 4      : " << static_cast<int>(quantity4) << std::endl;

    for(Addons::const_iterator
	it = addons_level1.begin(); it != addons_level1.end(); ++it)
	os << (*it).String(1);

    for(Addons::const_iterator
	it = addons_level2.begin(); it != addons_level2.end(); ++it)
	os << (*it).String(2);

    os <<
	"----------------I--------" << std::endl;

    // extra obj info
    switch(mp2_object)
    {
	// dwelling
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_DESERTTENT:
	case MP2::OBJ_TROLLBRIDGE:
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
	//
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
	//
	case MP2::OBJ_MONSTER:
	    os << "count           : " << GetCountMonster() << std::endl;
	    break;

	case MP2::OBJ_HEROES:
	    {
		const Heroes *hero = world.GetHeroes(maps_index);
		if(hero) os << hero->String();
	    }
	    break;

	case MP2::OBJN_CASTLE:
	case MP2::OBJ_CASTLE:
	    {
		const Castle *castle = world.GetCastle(maps_index);
		if(castle) os << castle->String();
	    }
	    break;

	default:
	{
	    const u16 dst_around = Maps::TileUnderProtection(maps_index);
	    if(dst_around)
	    {
		os << "protection      : ";
		for(Direction::vector_t dir = Direction::TOP_LEFT; dir < Direction::CENTER; ++dir)
		    if(dst_around & dir)
			os << Maps::GetDirectionIndex(maps_index, dir) << ", ";
		os << std::endl;
	    }
	    break;
	}
    }

    os << "----------------:--------" << std::endl;
    return os.str();
}

MP2::object_t Maps::Tiles::GetObject(void) const
{
    return static_cast<MP2::object_t>(mp2_object);
}

bool Maps::Tiles::GoodForUltimateArtifact(void) const
{
    return Ground::WATER != Maps::Tiles::GetGround() && isPassable(NULL, Direction::UNKNOWN, true);
}

/* accept move */
bool Maps::Tiles::isPassable(const Heroes* hero, Direction::vector_t direct, bool skipfog) const
{
    if(!skipfog && isFog(Settings::Get().CurrentColor()))
	return false;

    if(hero)
    {
       if(hero->GetIndex() == maps_index) return true;

       if(hero->isShipMaster())
       {
           if(Ground::WATER != Maps::Tiles::GetGround()) return false;

           switch(mp2_object)
           {
               case MP2::OBJ_BOAT:
               case MP2::OBJ_HEROES:   return false;

               default: break;
           }
       }
       else
       {
           if(Ground::WATER == Maps::Tiles::GetGround()) return false;

           switch(mp2_object)
           {
               case MP2::OBJ_HEROES:   return false;

               default: break;
           }
       }
    }

    for(Addons::const_iterator
	it = addons_level1.begin(); it != addons_level1.end(); ++it)
	if(! TilesAddon::isPassable(*it, direct, maps_index)) return false;

    return true;
}

/* check road */
bool Maps::Tiles::isRoad(u16 direct) const
{
    return addons_level1.end() != std::find_if(addons_level1.begin(), addons_level1.end(),
				std::bind2nd(std::mem_fun_ref(&TilesAddon::isRoad), direct));
}

bool Maps::Tiles::isStream(void) const
{
    return addons_level1.end() != std::find_if(addons_level1.begin(), addons_level1.end(),
				TilesAddon::isStream);
}

Maps::TilesAddon* Maps::Tiles::FindObject(u8 objs)
{
    Addons::iterator it = addons_level1.begin();

    switch(objs)
    {
	case MP2::OBJ_CAMPFIRE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isCampFire);
	    break;

        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_ANCIENTLAMP:
        case MP2::OBJ_RESOURCE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isResource);
	    break;

        case MP2::OBJ_RNDRESOURCE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomResource);
	    break;

        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_BOTTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWaterResource);
	    break;

	case MP2::OBJ_ARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isArtifact);
	    break;

	case MP2::OBJ_RNDARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact);
	    break;

	case MP2::OBJ_RNDARTIFACT1:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact1);
	    break;

	case MP2::OBJ_RNDARTIFACT2:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact2);
	    break;

	case MP2::OBJ_RNDARTIFACT3:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact3);
	    break;

	case MP2::OBJ_RNDULTIMATEARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isUltimateArtifact);
	    break;

	case MP2::OBJ_MONSTER:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMonster);
	    break;

	case MP2::OBJ_WHIRLPOOL:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWhirlPool);
	    break;

	case MP2::OBJ_STANDINGSTONES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isStandingStone);
	    break;

	case MP2::OBJ_ARTESIANSPRING:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isArtesianSpring);
	    break;

	case MP2::OBJ_OASIS:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isOasis);
	    break;

	case MP2::OBJ_WATERINGHOLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWateringHole);
	    break;

	case MP2::OBJ_MINES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMine);
	    break;

	case MP2::OBJ_JAIL:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isJail);
	    break;

	case MP2::OBJ_EVENT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isEvent);
	    break;

	case MP2::OBJ_BOAT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isBoat);
	    break;

	case MP2::OBJ_BARRIER:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isBarrier);
	    break;

	case MP2::OBJ_HEROES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMiniHero);
	    break;

	case MP2::OBJ_CASTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isCastle);
	    if(it == addons_level1.end())
	    {
		it = std::find_if(addons_level2.begin(), addons_level2.end(), TilesAddon::isCastle);
		return addons_level2.end() != it ? &(*it) : NULL;
	    }
	    break;

	case MP2::OBJ_RNDCASTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomCastle);
	    if(it == addons_level1.end())
	    {
		it = std::find_if(addons_level2.begin(), addons_level2.end(), TilesAddon::isRandomCastle);
		return addons_level2.end() != it ? &(*it) : NULL;
	    }
	    break;

	case MP2::OBJ_RNDMONSTER:
	case MP2::OBJ_RNDMONSTER1:
	case MP2::OBJ_RNDMONSTER2:
	case MP2::OBJ_RNDMONSTER3:
	case MP2::OBJ_RNDMONSTER4:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomMonster);
	    break;

	case MP2::OBJ_SKELETON:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isSkeleton);
	    break;

	default: break;
    }

    return addons_level1.end() != it ? &(*it) : NULL;
}

const Maps::TilesAddon* Maps::Tiles::FindObject(u8 objs) const
{
    Addons::const_iterator it = addons_level1.begin();

    switch(objs)
    {
	case MP2::OBJ_CAMPFIRE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isCampFire);
	    break;

        case MP2::OBJ_TREASURECHEST:
        case MP2::OBJ_ANCIENTLAMP:
        case MP2::OBJ_RESOURCE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isResource);
	    break;

        case MP2::OBJ_RNDRESOURCE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomResource);
	    break;

        case MP2::OBJ_FLOTSAM:
        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_BOTTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWaterResource);
	    break;

	case MP2::OBJ_ARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isArtifact);
	    break;

	case MP2::OBJ_RNDARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact);
	    break;

	case MP2::OBJ_RNDARTIFACT1:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact1);
	    break;

	case MP2::OBJ_RNDARTIFACT2:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact2);
	    break;

	case MP2::OBJ_RNDARTIFACT3:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomArtifact3);
	    break;

	case MP2::OBJ_RNDULTIMATEARTIFACT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isUltimateArtifact);
	    break;

	case MP2::OBJ_MONSTER:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMonster);
	    break;

	case MP2::OBJ_WHIRLPOOL:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWhirlPool);
	    break;

	case MP2::OBJ_STANDINGSTONES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isStandingStone);
	    break;

	case MP2::OBJ_ARTESIANSPRING:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isArtesianSpring);
	    break;

	case MP2::OBJ_OASIS:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isOasis);
	    break;

	case MP2::OBJ_WATERINGHOLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isWateringHole);
	    break;

	case MP2::OBJ_MINES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMine);
	    break;

	case MP2::OBJ_JAIL:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isJail);
	    break;

	case MP2::OBJ_EVENT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isEvent);
	    break;

	case MP2::OBJ_BOAT:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isBoat);
	    break;

	case MP2::OBJ_BARRIER:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isBarrier);
	    break;

	case MP2::OBJ_HEROES:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isMiniHero);
	    break;

	case MP2::OBJ_CASTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isCastle);
	    if(it == addons_level1.end())
	    {
		it = std::find_if(addons_level2.begin(), addons_level2.end(), TilesAddon::isCastle);
		return addons_level2.end() != it ? &(*it) : NULL;
	    }
	    break;

	case MP2::OBJ_RNDCASTLE:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomCastle);
	    if(it == addons_level1.end())
	    {
		it = std::find_if(addons_level2.begin(), addons_level2.end(), TilesAddon::isRandomCastle);
		return addons_level2.end() != it ? &(*it) : NULL;
	    }
	    break;

	case MP2::OBJ_RNDMONSTER:
	case MP2::OBJ_RNDMONSTER1:
	case MP2::OBJ_RNDMONSTER2:
	case MP2::OBJ_RNDMONSTER3:
	case MP2::OBJ_RNDMONSTER4:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRandomMonster);
	    break;

	case MP2::OBJ_SKELETON:
	    it = std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isSkeleton);
	    break;

	default: break;
    }

    return addons_level1.end() != it ? &(*it) : NULL;
}

Maps::TilesAddon* Maps::Tiles::FindFlags(void)
{
    Addons::iterator it =
	std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isFlag32);

    if(it == addons_level1.end())
    {
	it = std::find_if(addons_level2.begin(), addons_level2.end(), TilesAddon::isFlag32);
	return addons_level2.end() != it ? &(*it) : NULL;
    }

    return addons_level1.end() != it ? &(*it) : NULL;
}

/* ICN::FLAGS32 version */
void Maps::Tiles::CaptureFlags32(const MP2::object_t obj, const Color::color_t col)
{
    u8 index = 0;

    switch(col)
    {
	case Color::BLUE:	index = 0; break;
	case Color::GREEN:	index = 1; break;
	case Color::RED:	index = 2; break;
	case Color::YELLOW:	index = 3; break;
	case Color::ORANGE:	index = 4; break;
	case Color::PURPLE:	index = 5; break;
	case Color::NONE:	index = 6; break;

	default: break;
    }

    switch(obj)
    {
	case MP2::OBJ_MINES:		index += 14; CorrectFlags32(index, true); break;
	//case MP2::OBJ_DRAGONCITY:	index += 35; CorrectFlags32(index); break; unused
        case MP2::OBJ_LIGHTHOUSE:	index += 42; CorrectFlags32(index, false); break;

	case MP2::OBJ_ALCHEMYLAB:
	{
	    index += 21;
	    if(Maps::isValidDirection(maps_index, Direction::TOP))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP));
		tile.CorrectFlags32(index, true);
	    }
	}
	break;

	case MP2::OBJ_SAWMILL:
	{
	    index += 28;
	    if(Maps::isValidDirection(maps_index, Direction::TOP_RIGHT))
    	    {
    		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP_RIGHT));
    		tile.CorrectFlags32(index, true);
	    }
	}
	break;

	case MP2::OBJ_CASTLE:
	{
	    index *= 2;
	    if(Maps::isValidDirection(maps_index, Direction::LEFT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT));
    		tile.CorrectFlags32(index, true);
	    }

	    index += 1;
	    if(Maps::isValidDirection(maps_index, Direction::RIGHT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::RIGHT));
    		tile.CorrectFlags32(index, true);
	    }
	}
	break;

	default: return;
    }
}

/* correct flags, ICN::FLAGS32 vesion */
void Maps::Tiles::CorrectFlags32(const u8 index, bool up)
{
    TilesAddon* taddon = FindFlags();

    // replace flag
    if(NULL != taddon)
	taddon->index = index;
    else
    if(up)
	// or new flag
	addons_level2.push_back(TilesAddon(TilesAddon::UPPER, world.GetUniq(), 0x38, index));
    else
	// or new flag
	addons_level1.push_back(TilesAddon(TilesAddon::UPPER, world.GetUniq(), 0x38, index));
}

void Maps::Tiles::FixLoyaltyVersion(void)
{
    switch(mp2_object)
    {
	case MP2::OBJ_UNKNW_79:
        case MP2::OBJ_UNKNW_7A:
        case MP2::OBJ_UNKNW_F9:
        case MP2::OBJ_UNKNW_FA:
	{
	    u8 newobj = MP2::OBJ_ZERO;
	    Addons::iterator it = std::find_if(addons_level1.begin(), addons_level1.end(),
								    TilesAddon::isX_LOC123);

	    if(it != addons_level1.end())
	    {
		newobj = ObjLoyalty::LearnObject(*it);
	    }
	    else
	    {
		it = std::find_if(addons_level2.begin(), addons_level2.end(),
							TilesAddon::isX_LOC123);
		if(it != addons_level2.end())
		    newobj = ObjLoyalty::LearnObject(*it);
	    }

	    if(MP2::OBJ_ZERO != newobj)
		mp2_object = newobj;
	    else
	    {
		DEBUG(DBG_GAME, DBG_WARN, "index: " << maps_index);
	    }
	} break;

	default: break;
    }
}

u8 Maps::Tiles::GetMinesType(void) const
{
    const TilesAddon* taddon = FindObject(MP2::OBJ_MINES);

    if(taddon) switch(taddon->index)
    {
	case 0:	return Resource::ORE;
	case 1: return Resource::SULFUR;
        case 2: return Resource::CRYSTAL;
        case 3: return Resource::GEMS;
	case 4: return Resource::GOLD;
        default: break;
    }
    return 0;
}

/* for few object need many resource */
void Maps::Tiles::UpdateQuantity(void)
{
    const TilesAddon * addon = NULL;
    const Heroes* hero = mp2_object == MP2::OBJ_HEROES ? world.GetHeroes(maps_index) : NULL;

    switch(hero ? hero->GetUnderObject() : mp2_object)
    {
        case MP2::OBJ_WITCHSHUT:
            quantity1 = Skill::Secondary::RandForWitchsHut();
	    break;

	case MP2::OBJ_SHRINE1:
            quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(1)() : Spell::RandAdventure(1)();
            break;

        case MP2::OBJ_SHRINE2:
            quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(2)() : Spell::RandAdventure(2)();
            break;

        case MP2::OBJ_SHRINE3:
            quantity1 = Rand::Get(10) % 2 ? Spell::RandCombat(3)() : Spell::RandAdventure(3)();
            break;

	case MP2::OBJ_SKELETON:
	{
	    Rand::Queue percents(2);
	    // 80%: empty
	    percents.Push(0, 80);
	    // 20%: artifact 1 or 2 or 3
	    percents.Push(1, 20);
	    
	    if(percents.Get())
	    {
		switch(Rand::Get(1, 3))
		{
		    case 1: quantity1 = Artifact::Rand(Artifact::ART_LEVEL1); break;
		    case 2: quantity1 = Artifact::Rand(Artifact::ART_LEVEL2); break;
		    case 3: quantity1 = Artifact::Rand(Artifact::ART_LEVEL3); break;
		    default: break;
		}
	    }
	}
	break;

	case MP2::OBJ_WAGON:
	{
	    Rand::Queue percents(3);
	    // 20%: empty
	    percents.Push(0, 20);
	    // 10%: artifact 1 or 2
	    percents.Push(1, 10);
	    // 50%: resource
	    percents.Push(2, 50);

	    switch(percents.Get())
	    {
		case 1:
            	    quantity1 = Artifact::Rand(1 == Rand::Get(1, 2) ? Artifact::ART_LEVEL1 : Artifact::ART_LEVEL2);
		    break;
		case 2:
		    quantity1 = Resource::Rand();
		    quantity2 = Rand::Get(2, 5);
		    break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_ARTIFACT:
	    // quantity1 - art, quantity2 - conditions for pickup: 
	    // 1,2,3 - 2000g, 2500g+3res, 3000g+5res,
	    // 4,5 - need have skill wisard or leadership,
	    // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4, other - none
	    addon = FindObject(MP2::OBJ_ARTIFACT);
	    if(addon)
	    {
		Artifact art = Artifact::FromMP2IndexSprite(addon->index);

		if(art() == Artifact::SPELL_SCROLL)
		{
		    // spell from origin mp2
		    Spell spell(1 + (quantity2 * 256 + quantity1) / 8);
		    quantity1 = art();
		    // always available
		    quantity2 = 0;
		    quantity3 = spell();
		}
		else
		if(art.isValid())
		{
		    quantity1 = art();
		    // conditions: 70% empty
		    quantity2 = Rand::Get(1, 10) < 4 ? Rand::Get(1, 13) : 0;
		    //  added resource
		    if(quantity2 == 2 || quantity2 == 3)
			quantity4 = Resource::Rand();

		    // always available
		    if(Settings::Get().ExtNoRequirementsForArtifacts())
			quantity2 = 0;
		}
		else
		{
		    DEBUG(DBG_GAME, DBG_INFO, "unknown artifact");
		}
	    }
	break;

	case MP2::OBJ_RESOURCE:
	    addon = FindObject(MP2::OBJ_RESOURCE);
	    if(addon)
	    {
		quantity1 = Resource::FromIndexSprite(addon->index);
		switch(quantity1)
		{
		    case Resource::WOOD:
		    case Resource::ORE:
		    case Resource::GOLD:
			quantity2 = Rand::Get(5, 10);
			break;
		    default:
			quantity2 = Rand::Get(3, 6);
			break;
		}
	    }
	break;

	case MP2::OBJ_MAGICGARDEN:
	    // 5 gems or 500 gold
	    quantity1 = (Rand::Get(1) ? Resource::GEMS : Resource::GOLD);
	    quantity2 = 5;
	break;

	case MP2::OBJ_WATERWHEEL:
	    // first week 500 gold, next week 100 gold
	    quantity1 = Resource::GOLD;
	    quantity2 = (world.CountDay() == 1 ? 5 : 10);
	break;

	case MP2::OBJ_WINDMILL:
	    // 2 rnd resource
	    quantity2 = 2;
	    quantity1 = Resource::WOOD;
	    // except: wood, bugs: #3117478
	    while(quantity1 == Resource::WOOD) quantity1 = Resource::Rand();
	break;

	case MP2::OBJ_LEANTO:
	    // 1-4 rnd resource
	    quantity1 = Resource::Rand();
	    quantity2 = Rand::Get(1, 4);
	break;

	case MP2::OBJ_CAMPFIRE:
	    // 4-6 rnd resource and + 400-600 gold
	    quantity1 = Resource::Rand();
	    quantity2 = Rand::Get(4, 6);
	break;

	case MP2::OBJ_FLOTSAM:
	{
	    Rand::Queue percents(3);
	    // 25%: empty
	    percents.Push(0, 25);
	    // 25%: 500 gold + 10 wood
	    percents.Push(1, 25);
	    // 25%: 200 gold + 5 wood
	    percents.Push(2, 25);
	    // 25%: 5 wood
	    percents.Push(3, 25);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity1 = 5; quantity2 = 10; break;
		case 2: quantity1 = 2; quantity2 = 5;  break;
		case 3: quantity2 = 5; break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_SHIPWRECKSURVIROR:
	{
	    Rand::Queue percents(3);
	    // 55%: artifact 1
	    percents.Push(1, 55);
	    // 30%: artifact 2
	    percents.Push(1, 30);
	    // 15%: artifact 3
	    percents.Push(1, 15);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity1 = Artifact::Rand(Artifact::ART_LEVEL1); break;
		case 2: quantity1 = Artifact::Rand(Artifact::ART_LEVEL2); break;
		case 3: quantity1 = Artifact::Rand(Artifact::ART_LEVEL3); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_WATERCHEST:
	{
	    Rand::Queue percents(3);
            // 20% - empty
	    percents.Push(0, 20);
            // 70% - 1500 gold
	    percents.Push(1, 70);
            // 10% - 1000 gold + art
	    percents.Push(2, 10);

	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity2 = 15; break;
		case 2: quantity2 = 10; quantity1 = Artifact::Rand(Artifact::ART_LEVEL1); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_TREASURECHEST:
	{
	    Rand::Queue percents(4);
	    // 31% - 2000 gold or 1500 exp
	    percents.Push(1, 31);
	    // 32% - 1500 gold or 1000 exp
	    percents.Push(2, 32);
	    // 32% - 1000 gold or 500 exp
	    percents.Push(3, 32);
	    // 5% - art
	    percents.Push(4,  5);
	    
	    // variant
	    switch(percents.Get())
	    {
		case 1: quantity2 = 20; break;
		case 2: quantity2 = 15; break;
		case 3: quantity2 = 10; break;
		case 4: quantity1 = Artifact::Rand(Artifact::ART_LEVEL1); break;
		default: break;
	    }
	}
	break;

	case MP2::OBJ_DERELICTSHIP:
	    // 5000 gold
	    quantity2 = 50;
	break;

	case MP2::OBJ_SHIPWRECK:
	    // variant: 10ghost(1000g), 15 ghost(2000g), 25ghost(5000g) or 50ghost(2000g+art)
	    switch(Rand::Get(1, 4))
	    {
		case 1: quantity2 = 10; break;
		case 2: quantity2 = 15; break;
		case 3: quantity2 = 25; break;
		case 4: quantity1 = Artifact::Rand(Artifact::ART_LEVEL123);
		        quantity2 = 50; break;
		default: break;
	    }
	break;

	case MP2::OBJ_GRAVEYARD:
	    // 1000 gold + art
		quantity1 = Artifact::Rand(Artifact::ART_LEVEL123);
		quantity2 = 10;
	break;

	case MP2::OBJ_PYRAMID:
	    // random spell level 5
	    quantity1 = (Rand::Get(1) ? Spell::RandCombat(5)() : Spell::RandAdventure(5)());
	    quantity2 = 1;
	break;

	case MP2::OBJ_DAEMONCAVE:
	    // 1000 exp or 1000 exp + 2500 gold or 1000 exp + art or (-2500 or remove hero)
	    quantity2 = Rand::Get(1, 4);
	    quantity1 = Artifact::Rand(Artifact::ART_LEVEL123);
	break;

	// aband mines
	case MP2::OBJ_ABANDONEDMINE:
	    SetCountMonster(Rand::Get(39, 45));	// I checked in Heroes II: min 3 x 13, and max 3 x 15
	    quantity3 = Monster::GHOST;
	    if(! Settings::Get().ExtAbandonedMineRandom())
		quantity4 =  Resource::GOLD;
	    else
		switch(Rand::Get(1, 5))
		{
		    case 1: quantity4 =  Resource::ORE; break;
		    case 2: quantity4 =  Resource::SULFUR; break;
		    case 3: quantity4 =  Resource::CRYSTAL; break;
		    case 4: quantity4 =  Resource::GEMS; break;
		    default: quantity4 =  Resource::GOLD; break;
		}
	break;

	case MP2::OBJ_TREEKNOWLEDGE:
	    // variant: 10 gems, 2000 gold or free
	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    quantity2 = 10;
		break;
		case 2:
		    quantity2 = 20;
		break;
		default: break;
	    }
	break;

        case MP2::OBJ_BARRIER:
        case MP2::OBJ_TRAVELLERTENT:
	    quantity1 = BarrierColor::FromMP2(quantity1);
	break;

	default: break;
    }
}

bool Maps::Tiles::ValidQuantity(void) const
{
    if(MP2::isQuantityObject(mp2_object))
	    return quantity1 || quantity2;

    return false;
}

bool Maps::Tiles::OtherObjectsIsProtection(void) const
{
    u8 object = mp2_object;

    if(MP2::OBJ_HEROES == mp2_object && world.GetHeroes(maps_index))
	object = world.GetHeroes(maps_index)->GetUnderObject();

    switch(object)
    {
	case MP2::OBJ_ARTIFACT:
	    return quantity2 > 5 && quantity2 < 14;

	case MP2::OBJ_DERELICTSHIP:
	case MP2::OBJ_SHIPWRECK:
	case MP2::OBJ_GRAVEYARD:
	    return quantity2;

	case MP2::OBJ_PYRAMID:
	case MP2::OBJ_DAEMONCAVE:
	    return quantity2;

	case MP2::OBJ_MONSTER:
	case MP2::OBJ_ABANDONEDMINE:
	    return GetCountMonster();

	default:
	    break;
    }

    return false;
}

/* true: if protection or has guardians */
bool Maps::Tiles::CaptureObjectIsProtection(u8 color) const
{
    u8 object = mp2_object;

    if(MP2::OBJ_HEROES == mp2_object)
    {
	const Heroes* hero = world.GetHeroes(maps_index);
	if(hero) object = hero->GetUnderObject();
    }

    if(color &&
	MP2::isCaptureObject(object))
    {
	if(MP2::OBJ_CASTLE == object)
	{
	    Castle* castle = world.GetCastle(maps_index);
	    if(castle)
		return castle->GetArmy().isValid();
	}
	else
            return quantity3 && GetCountMonster();
    }

    return false;
}

void Maps::Tiles::RemoveObjectSprite(void)
{
    const Maps::TilesAddon *addon = NULL;

    switch(mp2_object)
    {
	case MP2::OBJ_WATERCHEST:
	case MP2::OBJ_BOTTLE:
	case MP2::OBJ_FLOTSAM:
	case MP2::OBJ_SHIPWRECKSURVIROR:
	case MP2::OBJ_TREASURECHEST:
	case MP2::OBJ_ANCIENTLAMP:
	case MP2::OBJ_RESOURCE:
	case MP2::OBJ_ARTIFACT:
	case MP2::OBJ_CAMPFIRE:		addon = FindObject(mp2_object); break;

	case MP2::OBJ_JAIL:		RemoveJailSprite(); return;

	case MP2::OBJ_BARRIER:		RemoveBarrierSprite(); return;

	default: return;
    }
    
    if(addon)
    {
        // remove shadow sprite from left cell
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	    world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT)).Remove(addon->uniq);

	Remove(addon->uniq);
    }
}

void Maps::Tiles::RemoveBarrierSprite(void)
{
    const Maps::TilesAddon *addon = FindObject(MP2::OBJ_BARRIER);

    if(addon)
    {
        // remove left sprite
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	{
	    const s32 left = Maps::GetDirectionIndex(maps_index, Direction::LEFT);
	    world.GetTiles(left).Remove(addon->uniq);
	}

	Remove(addon->uniq);
    }
}

void Maps::Tiles::RemoveJailSprite(void)
{
    const Maps::TilesAddon *addon = FindObject(MP2::OBJ_JAIL);

    if(addon)
    {
        // remove left sprite
        if(Maps::isValidDirection(maps_index, Direction::LEFT))
    	{
	    const s32 left = Maps::GetDirectionIndex(maps_index, Direction::LEFT);
	    world.GetTiles(left).Remove(addon->uniq);

    	    // remove left left sprite
    	    if(Maps::isValidDirection(left, Direction::LEFT))
    		world.GetTiles(Maps::GetDirectionIndex(left, Direction::LEFT)).Remove(addon->uniq);
	}

        // remove top sprite
        if(Maps::isValidDirection(maps_index, Direction::TOP))
    	{
	    const s32 top = Maps::GetDirectionIndex(maps_index, Direction::TOP);
	    world.GetTiles(top).Remove(addon->uniq);
	    world.GetTiles(top).SetObject(MP2::OBJ_ZERO);

    	    // remove top left sprite
    	    if(Maps::isValidDirection(top, Direction::LEFT))
    	    {
		world.GetTiles(Maps::GetDirectionIndex(top, Direction::LEFT)).Remove(addon->uniq);
		world.GetTiles(Maps::GetDirectionIndex(top, Direction::LEFT)).SetObject(MP2::OBJ_ZERO);
	    }
	}

	Remove(addon->uniq);
    }
}

bool Maps::Tiles::FixedCountMonster(void) const
{
    return quantity5;
}

u16 Maps::Tiles::GetCountMonster(void) const
{
    return quantity2 * 0xFF + quantity1;
}

void Maps::Tiles::SetCountMonster(const u16 count)
{
    if(count > 0xFF * 0xFF + 0xFF - 1)
    {
	quantity1 = 0xFF;
	quantity2 = 0xFF;
    }
    else
    {
	quantity1 = count % 0xFF;
	quantity2 = count / 0xFF;
    }
}

void Maps::Tiles::PlaceMonsterOnTile(Tiles & tile, const Monster & mons, u32 uniq)
{
    tile.ResetQuantity();
    tile.SetObject(MP2::OBJ_MONSTER);
    tile.SetCountMonster(5 * mons.GetRNDSize(true));
    tile.SetQuantity3(mons());
    // join for money
    tile.SetQuantity4(1);

    // set sprite
    tile.AddonsPushLevel1(TilesAddon(TilesAddon::UPPER, uniq, 0x33, mons() - 1));
}

void Maps::Tiles::UpdateMonsterInfo(Tiles & tile)
{
    Maps::TilesAddon *addon = tile.FindObject(MP2::OBJ_RNDMONSTER);

    if(addon)
    {
	switch(tile.mp2_object)
	{
    	    case MP2::OBJ_RNDMONSTER:       addon->index = Monster::Rand().GetID(); break;
    	    case MP2::OBJ_RNDMONSTER1:      addon->index = Monster::Rand(Monster::LEVEL1).GetID(); break;
    	    case MP2::OBJ_RNDMONSTER2:      addon->index = Monster::Rand(Monster::LEVEL2).GetID(); break;
    	    case MP2::OBJ_RNDMONSTER3:      addon->index = Monster::Rand(Monster::LEVEL3).GetID(); break;
    	    case MP2::OBJ_RNDMONSTER4:      addon->index = Monster::Rand(Monster::LEVEL4).GetID(); break;
	    default: addon = NULL; break;
	}

	// ICN::MONS32 start from PEASANT
        if(addon) addon->index = addon->index - 1;

	tile.mp2_object = MP2::OBJ_MONSTER;
    }

    addon = tile.FindObject(MP2::OBJ_MONSTER);
    const Monster m(addon ? addon->index + 1 : Monster::UNKNOWN);
    // fixed count
    tile.quantity5 = 0;

    // update random count
    if(0 == tile.quantity1 && 0 == tile.quantity2)
        tile.SetCountMonster(4 * m.GetRNDSize(false));
    // update fixed count (mp2 format)
    else
    {
	u16 count = tile.quantity2;
	    count <<= 8;
	    count |= tile.quantity1;
	    count >>= 3;

        tile.SetCountMonster(count);
	tile.quantity5 = 1;
    }

    // set monster
    tile.quantity3 = m();

    // extra params:
    // quantity4 - join conditions (0: skip, 1: money, 2: free, 3: force (for campain need store color also)

    // skip join
    if(m() == Monster::GHOST || m.isElemental())
	tile.quantity4 = 0;
    else
    if(tile.FixedCountMonster())
	// for money
	tile.quantity4 = 1;
    else
	// 20% chance of joining
        tile.quantity4 = (3 > Rand::Get(1, 10) ? 2 : 1);
}

void Maps::Tiles::UpdateAbandoneMineSprite(Tiles & tile)
{
    Addons::iterator it = std::find_if(tile.addons_level1.begin(), tile.addons_level1.end(),
						    TilesAddon::isAbandoneMineSprite);
    u32 uniq = it != tile.addons_level1.end() ? (*it).uniq : 0;

    if(uniq)
    {
	for(Addons::iterator
	    it = tile.addons_level1.begin(); it != tile.addons_level1.end(); ++it)
		TilesAddon::UpdateAbandoneMineLeftSprite(*it, tile.quantity4);

	if(Maps::isValidDirection(tile.maps_index, Direction::RIGHT))
	{
    	    Tiles & tile2 = world.GetTiles(Maps::GetDirectionIndex(tile.maps_index, Direction::RIGHT));
    	    TilesAddon *mines = tile2.FindAddonLevel1(uniq);

	    if(mines) TilesAddon::UpdateAbandoneMineRightSprite(*mines);
	    if(tile2.mp2_object == MP2::OBJN_ABANDONEDMINE) tile2.mp2_object = MP2::OBJN_MINES;
	}
    }

    if(Maps::isValidDirection(tile.maps_index, Direction::LEFT))
    {
        Tiles & tile2 = world.GetTiles(Maps::GetDirectionIndex(tile.maps_index, Direction::LEFT));
	if(tile2.mp2_object == MP2::OBJN_ABANDONEDMINE) tile2.mp2_object = MP2::OBJN_MINES;
    }

    if(Maps::isValidDirection(tile.maps_index, Direction::TOP))
    {
        Tiles & tile2 = world.GetTiles(Maps::GetDirectionIndex(tile.maps_index, Direction::TOP));
	if(tile2.mp2_object == MP2::OBJN_ABANDONEDMINE) tile2.mp2_object = MP2::OBJN_MINES;

	if(Maps::isValidDirection(tile2.maps_index, Direction::LEFT))
	{
    	    Tiles & tile3 = world.GetTiles(Maps::GetDirectionIndex(tile2.maps_index, Direction::LEFT));
	    if(tile3.mp2_object == MP2::OBJN_ABANDONEDMINE) tile3.mp2_object = MP2::OBJN_MINES;
	}

	if(Maps::isValidDirection(tile2.maps_index, Direction::RIGHT))
	{
    	    Tiles & tile3 = world.GetTiles(Maps::GetDirectionIndex(tile2.maps_index, Direction::RIGHT));
	    if(tile3.mp2_object == MP2::OBJN_ABANDONEDMINE) tile3.mp2_object = MP2::OBJN_MINES;
	}
    }
}

void Maps::Tiles::UpdateRNDArtifactSprite(Tiles & tile)
{
    TilesAddon *addon = NULL;
    u8 index = 0;
    Artifact art;

    switch(tile.mp2_object)
    {
        case MP2::OBJ_RNDARTIFACT:
            addon = tile.FindObject(MP2::OBJ_RNDARTIFACT);
	    art = Artifact::Rand(Artifact::ART_LEVEL123);
            index = art.IndexSprite();
            break;
        case MP2::OBJ_RNDARTIFACT1:
            addon = tile.FindObject(MP2::OBJ_RNDARTIFACT1);
	    art = Artifact::Rand(Artifact::ART_LEVEL1);
            index = art.IndexSprite();
            break;
        case MP2::OBJ_RNDARTIFACT2:
            addon = tile.FindObject(MP2::OBJ_RNDARTIFACT2);
	    art = Artifact::Rand(Artifact::ART_LEVEL2);
            index = art.IndexSprite();
            break;
        case MP2::OBJ_RNDARTIFACT3:
            addon = tile.FindObject(MP2::OBJ_RNDARTIFACT3);
	    art = Artifact::Rand(Artifact::ART_LEVEL3);
            index = art.IndexSprite();
            break;
        default: return;
    }

    if(!art.isValid())
    {
    	DEBUG(DBG_GAME, DBG_INFO, "unknown artifact");
    }
    else
    if(addon)
    {
        addon->index = index;
        tile.mp2_object = MP2::OBJ_ARTIFACT;

        // replace shadow artifact
        if(Maps::isValidDirection(tile.maps_index, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(tile.maps_index, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(addon->uniq);

            if(shadow) shadow->index = index - 1;
        }
    }
}

void Maps::Tiles::UpdateRNDResourceSprite(Tiles & tile)
{
    TilesAddon *addon = tile.FindObject(MP2::OBJ_RNDRESOURCE);

    if(addon)
    {
        addon->index = Resource::GetIndexSprite(Resource::Rand());
        tile.mp2_object = MP2::OBJ_RESOURCE;

        // replace shadow artifact
        if(Maps::isValidDirection(tile.maps_index, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(tile.maps_index, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(addon->uniq);

            if(shadow) shadow->index = addon->index - 1;
        }
    }
}

void Maps::Tiles::UpdateStoneLightsSprite(Tiles & tile)
{
    for(Addons::iterator
	it = tile.addons_level1.begin(); it != tile.addons_level1.end(); ++it)
	    TilesAddon::UpdateStoneLightsSprite(*it, tile.quantity1);
}

void Maps::Tiles::UpdateFountainSprite(Tiles & tile)
{
    for(Addons::iterator
	it = tile.addons_level1.begin(); it != tile.addons_level1.end(); ++it)
	    TilesAddon::UpdateFountainSprite(*it);
}

void Maps::Tiles::UpdateTreasureChestSprite(Tiles & tile)
{
    for(Addons::iterator
	it = tile.addons_level1.begin(); it != tile.addons_level1.end(); ++it)
	    TilesAddon::UpdateTreasureChestSprite(*it);
}

bool Maps::Tiles::isFog(u8 color) const
{
    return fogs & color;
}

void Maps::Tiles::SetFog(u8 color)
{
    fogs |= (Settings::Get().ExtUnionsAllowViewMaps() ? Players::GetPlayerFriends(color) : color);
}

void Maps::Tiles::ClearFog(u8 color)
{
    fogs &= ~(Settings::Get().ExtUnionsAllowViewMaps() ? Players::GetPlayerFriends(color) : color);
}

void Maps::Tiles::ResetQuantity(void)
{
    quantity1 = 0;
    quantity2 = 0;
    quantity3 = 0;
    quantity4 = 0;
    quantity5 = 0;
    quantity6 = 0;
    quantity7 = 0;
}

void Maps::Tiles::RedrawFogs(Surface & dst, u8 color) const
{
    const Interface::GameArea & area = Interface::GameArea::Get();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    // get direction around fogs
    u16 around = 0;

    for(Direction::vector_t direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
        if(!Maps::isValidDirection(maps_index, direct) ||
           world.GetTiles(Maps::GetDirectionIndex(maps_index, direct)).isFog(color)) around |= direct;

    if(isFog(color)) around |= Direction::CENTER;
 
    // TIL::CLOF32
    if(DIRECTION_ALL == around)
    {
	const Surface & sf = AGG::GetTIL(TIL::CLOF32, maps_index % 4, 0);
	area.BlitOnTile(dst, sf, 0, 0, mp);
    }
    else
    {
	u8 index = 0;
	bool revert = false;

	// see ICN::CLOP32: sprite 10
	if((around & Direction::CENTER) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)))
	{ index = 10; revert = false; }
	else
	// see ICN::CLOP32: sprite 6, 7, 8
	if(around & (Direction::CENTER | Direction::TOP) && !(around & (Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)))
	{ index = 6; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::LEFT)))
	{ index = 7; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::LEFT) && !(around & (Direction::TOP | Direction::BOTTOM | Direction::RIGHT)))
	{ index = 7; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::BOTTOM) && !(around & (Direction::TOP | Direction::LEFT | Direction::RIGHT)))
	{ index = 8; revert = false; }
	else
	// see ICN::CLOP32: sprite 9, 29
	if(around & (DIRECTION_CENTER_COL) && !(around & (Direction::LEFT | Direction::RIGHT)))
	{ index = 9; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW) && !(around & (Direction::TOP | Direction::BOTTOM)))
	{ index = 29; revert = false; }
	else
	// see ICN::CLOP32: sprite 15, 22
	if(around == (DIRECTION_ALL & (~Direction::TOP_RIGHT)))
	{ index = 15; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~Direction::TOP_LEFT)))
	{ index = 15; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~Direction::BOTTOM_RIGHT)))
	{ index = 22; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~Direction::BOTTOM_LEFT)))
	{ index = 22; revert = true; }
	else
	// see ICN::CLOP32: sprite 16, 17, 18, 23
	if(around == (DIRECTION_ALL & (~(Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT))))
	{ index = 16; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::BOTTOM_LEFT))))
	{ index = 16; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_RIGHT | Direction::BOTTOM_LEFT))))
	{ index = 17; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::BOTTOM_RIGHT))))
	{ index = 17; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::TOP_LEFT | Direction::TOP_RIGHT))))
	{ index = 18; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~(Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT))))
	{ index = 23; revert = false; }
	else
	// see ICN::CLOP32: sprite 13, 14
	if(around == (DIRECTION_ALL & (~DIRECTION_TOP_RIGHT_CORNER)))
	{ index = 13; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_TOP_LEFT_CORNER)))
	{ index = 13; revert = true; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_BOTTOM_RIGHT_CORNER)))
	{ index = 14; revert = false; }
	else
	if(around == (DIRECTION_ALL & (~DIRECTION_BOTTOM_LEFT_CORNER)))
	{ index = 14; revert = true; }
	else
	// see ICN::CLOP32: sprite 11, 12
	if(around & (Direction::CENTER | Direction::LEFT | Direction::BOTTOM_LEFT | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::TOP_RIGHT | Direction::RIGHT)))
	{ index = 11; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::BOTTOM_RIGHT | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::TOP_LEFT | Direction::LEFT)))
	{ index = 11; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::LEFT | Direction::TOP_LEFT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::BOTTOM_RIGHT | Direction::RIGHT)))
	{ index = 12; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::TOP_RIGHT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::BOTTOM_LEFT | Direction::LEFT)))
	{ index = 12; revert = true; }
	else
	// see ICN::CLOP32: sprite 19, 20, 22
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::TOP_LEFT) &&
	 !(around & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT | Direction::TOP_RIGHT)))
	{ index = 19; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::TOP_RIGHT) &&
	 !(around & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT | Direction::TOP_LEFT)))
	{ index = 19; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::BOTTOM_LEFT) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT | Direction::TOP_LEFT)))
	{ index = 20; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP | Direction::BOTTOM_RIGHT) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_LEFT | Direction::TOP_LEFT)))
	{ index = 20; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::TOP) &&
	 !(around & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT | Direction::BOTTOM_LEFT | Direction::TOP_LEFT)))
	{ index = 22; revert = false; }
	else
	// see ICN::CLOP32: sprite 24, 25, 26, 30
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::BOTTOM_LEFT) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_RIGHT)))
	{ index = 24; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM | Direction::BOTTOM_RIGHT) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_LEFT)))
	{ index = 24; revert = true; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::LEFT | Direction::TOP_LEFT) &&
	 !(around & (Direction::RIGHT | Direction::BOTTOM_LEFT)))
	{ index = 25; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::RIGHT | Direction::TOP_RIGHT) &&
	 !(around & (Direction::LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 25; revert = true; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::BOTTOM_LEFT | Direction::LEFT) &&
	 !(around & (Direction::RIGHT | Direction::TOP_LEFT)))
	{ index = 26; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::BOTTOM_RIGHT | Direction::RIGHT) &&
	 !(around & (Direction::LEFT | Direction::TOP_RIGHT)))
	{ index = 26; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP_LEFT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_RIGHT)))
	{ index = 30; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP_RIGHT | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_LEFT)))
	{ index = 30; revert = true; }
	else
	// see ICN::CLOP32: sprite 27, 28
	if(around & (Direction::CENTER | Direction::BOTTOM | Direction::LEFT) &&
	 !(around & (Direction::TOP | Direction::TOP_RIGHT | Direction::RIGHT | Direction::BOTTOM_LEFT)))
	{ index = 27; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::BOTTOM | Direction::RIGHT) &&
	 !(around & (Direction::TOP | Direction::TOP_LEFT | Direction::LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 27; revert = true; }
	else
	if(around & (Direction::CENTER | Direction::LEFT | Direction::TOP) &&
	 !(around & (Direction::TOP_LEFT | Direction::RIGHT | Direction::BOTTOM | Direction::BOTTOM_RIGHT)))
	{ index = 28; revert = false; }
	else
	if(around & (Direction::CENTER | Direction::RIGHT | Direction::TOP) &&
	 !(around & (Direction::TOP_RIGHT | Direction::LEFT | Direction::BOTTOM | Direction::BOTTOM_LEFT)))
	{ index = 28; revert = true; }
	else
	// see ICN::CLOP32: sprite 31, 32, 33
	if(around & (DIRECTION_CENTER_ROW | Direction::TOP) &&
	 !(around & (Direction::BOTTOM | Direction::TOP_LEFT | Direction::TOP_RIGHT)))
	{ index = 31; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::RIGHT) &&
	 !(around & (Direction::LEFT | Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT)))
	{ index = 32; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | Direction::LEFT) &&
	 !(around & (Direction::RIGHT | Direction::TOP_LEFT | Direction::BOTTOM_LEFT)))
	{ index = 32; revert = true; }
	else
	if(around & (DIRECTION_CENTER_ROW | Direction::BOTTOM) &&
	 !(around & (Direction::TOP | Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT)))
	{ index = 33; revert = false; }
	else
	// see ICN::CLOP32: sprite 0, 1, 2, 3, 4, 5
	if(around & (DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW) &&
	 !(around & (Direction::TOP)))
	{ index = maps_index % 2 ? 0 : 1; revert = false; }
	else
	if(around & (DIRECTION_CENTER_ROW | DIRECTION_TOP_ROW) &&
	 !(around & (Direction::BOTTOM)))
	{ index = maps_index % 2 ? 4 : 5; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | DIRECTION_LEFT_COL) &&
	 !(around & (Direction::RIGHT)))
	{ index = maps_index % 2 ? 2 : 3; revert = false; }
	else
	if(around & (DIRECTION_CENTER_COL | DIRECTION_RIGHT_COL) &&
	 !(around & (Direction::LEFT)))
	{ index = maps_index % 2 ? 2 : 3; revert = true; }
	// unknown
	else
	{
	    const Surface & sf = AGG::GetTIL(TIL::CLOF32, maps_index % 4, 0);
	    area.BlitOnTile(dst, sf, 0, 0, mp);
	    return;
	}

	const Sprite & sprite = AGG::GetICN(ICN::CLOP32, index, revert);
	area.BlitOnTile(dst, sprite, (revert ? sprite.x() + TILEWIDTH - sprite.w() : sprite.x()), sprite.y(), mp);
    }
}
