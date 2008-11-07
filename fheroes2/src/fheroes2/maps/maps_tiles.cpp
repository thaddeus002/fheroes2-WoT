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

#include <list>
#include <iostream>
#include <algorithm>
#include "agg.h"
#include "tools.h"
#include "world.h"
#include "race.h"
#include "config.h"
#include "heroes.h"
#include "sprite.h"
#include "maps.h"
#include "gamearea.h"
#include "game_focus.h"
#include "engine.h"
#include "object.h"
#include "objxloc.h"
#include "resource.h"
#include "maps_tiles.h"

Maps::TilesAddon::TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii) : level(lv), uniq(gid), object(obj), index(ii)
{
}

Maps::TilesAddon & Maps::TilesAddon::operator= (const Maps::TilesAddon & ta)
{
    level = ta.level;

    object = ta.object;

    index = ta.index;

    uniq = ta.uniq;
    
    return *this;
}

bool Maps::TilesAddon::PredicateSortRules1(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) > (ta2.level % 4));
}

bool Maps::TilesAddon::PredicateSortRules2(const Maps::TilesAddon & ta1, const Maps::TilesAddon & ta2)
{
    return ((ta1.level % 4) < (ta2.level % 4));
}

u16 Maps::TilesAddon::isRoad(const TilesAddon & ta)
{
    switch(MP2::GetICNObject(ta.object))
    {
	// castle and tower (gate)
	case ICN::OBJNTOWN:
	    return (13 == ta.index ||
		    29 == ta.index ||
	    	    45 == ta.index ||
	    	    61 == ta.index ||
	    	    77 == ta.index ||
	    	    93 == ta.index ||
	    	    109 == ta.index ||
	    	    125 == ta.index ||
	    	    141 == ta.index ||
	    	    157 == ta.index ||
	    	    173 == ta.index ||
	    	    189 == ta.index ? Direction::TOP | Direction::BOTTOM : 0);

	// castle lands (gate)
        case ICN::OBJNTWBA:
	    return ( 7 == ta.index ||
		    17 == ta.index ||
		    27 == ta.index ||
		    37 == ta.index ||
		    47 == ta.index ||
		    57 == ta.index ||
		    67 == ta.index ||
		    77 == ta.index ? Direction::TOP | Direction::BOTTOM : 0);

	// from sprite road
	case ICN::ROAD:
	    if(0  == ta.index ||
	       4  == ta.index ||
	       5  == ta.index ||
	       7  == ta.index ||
	       9  == ta.index ||
	       12 == ta.index ||
	       13 == ta.index ||
	       16 == ta.index ||
	       19 == ta.index ||
	       20 == ta.index ||
	       26 == ta.index)	return Direction::TOP | Direction::BOTTOM;
	    else
	    if(2  == ta.index ||
	       21 == ta.index ||
	       28 == ta.index)	return Direction::RIGHT | Direction::LEFT;
	    else
	    if(3  == ta.index ||
	       6  == ta.index ||
	       14 == ta.index)	return Direction::TOP | Direction::BOTTOM | Direction::RIGHT | Direction::LEFT;
	    else
	    if(17 == ta.index ||
	       29 == ta.index)	return Direction::TOP_LEFT | Direction::BOTTOM_RIGHT;
	    else
	    if(18 == ta.index ||
	       30 == ta.index)	return Direction::TOP_RIGHT | Direction::BOTTOM_LEFT;
	
	default:
	    break;
    }

    return 0;
}

bool Maps::TilesAddon::isStream(const TilesAddon & ta)
{
    return ICN::STREAM == MP2::GetICNObject(ta.object);
}

Maps::Tiles::Tiles(u16 mi, const MP2::mp2tile_t & mp2tile) : maps_index(mi), tile_sprite(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE), tile_index(mp2tile.tileIndex),
    shape(mp2tile.shape), general(mp2tile.generalObject), quantity1(mp2tile.quantity1), quantity2(mp2tile.quantity2),
    fogs(Color::BLUE | Color::GREEN | Color::RED | Color::YELLOW | Color::ORANGE | Color::PURPLE)
{
    tile_sprite.LoadPalette(AGG::Cache::Get().GetPAL());

    SetTile(mp2tile.tileIndex, mp2tile.shape);

    AddonsPushLevel1(mp2tile);
    AddonsPushLevel2(mp2tile);
}

void Maps::Tiles::SetTile(const u16 index, const u8 shape)
{
    AGG::GetTIL(TIL::GROUND32, index, shape, tile_sprite);
    tile_index = index;
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
    if(addons_level1.size()) addons_level1.sort(Maps::TilesAddon::PredicateSortRules1);

    if(addons_level2.size()) addons_level2.sort(Maps::TilesAddon::PredicateSortRules2);
}

Maps::Ground::ground_t Maps::Tiles::GetGround(void) const
{
    // list grounds from GROUND32.TIL
    if(30 > tile_index)
        return Maps::Ground::WATER;
        
    else if(92 > tile_index)
        return Maps::Ground::GRASS;

    else if(146 > tile_index)
        return Maps::Ground::SNOW;

    else if(208 > tile_index)
        return Maps::Ground::SWAMP;

    else if(262 > tile_index)
        return Maps::Ground::LAVA;

    else if(321 > tile_index)
        return Maps::Ground::DESERT;

    else if(361 > tile_index)
        return Maps::Ground::DIRT;

    else if(415 > tile_index)
        return Maps::Ground::WASTELAND;

    //else if(432 > tile_index)

	return Maps::Ground::BEACH;
}

void Maps::Tiles::Remove(u32 uniq)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator       it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	while(it1 != it2)
	    if(uniq == (*it1).uniq)
		it1 = addons_level1.erase(it1);
	    else
		++it1;
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator       it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	while(it1 != it2)
	    if(uniq == (*it1).uniq)
		it1 = addons_level2.erase(it1);
	    else
		++it1;
    }
}

void Maps::Tiles::RedrawTile(void) const
{
    const Rect & area = GameArea::Get().GetRect();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area & mp)
    {
	const s16 dstx = BORDERWIDTH + TILEWIDTH * (mp.x - area.x);
	const s16 dsty = BORDERWIDTH + TILEWIDTH * (mp.y - area.y);

	Display::Get().Blit(tile_sprite, dstx, dsty);
    }
}

void Maps::Tiles::RedrawBottom(const TilesAddon * skip) const
{
    Display & display = Display::Get();
    const Rect & area = GameArea::Get().GetRect();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area & mp)
    {
	const s16 dstx = BORDERWIDTH + TILEWIDTH * (mp.x - area.x);
	const s16 dsty = BORDERWIDTH + TILEWIDTH * (mp.y - area.y);

	if(addons_level1.size())
	{
	    std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	    std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	    for(; it1 != it2; ++it1)
	    {
		if(skip && skip == &(*it1)) continue;

		const u8 & object = (*it1).object;
	        const u8 & index  = (*it1).index;
		const ICN::icn_t icn = MP2::GetICNObject(object);

		if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
		{
		    const Sprite & sprite = AGG::GetICN(icn, index);
		    display.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		    // possible anime
		    if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		    {
			// skip animation for MagickGarden if void resource
			if(MP2::OBJ_MAGICGARDEN == general && 0 == quantity2) continue;

			const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
			display.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		    }
		}
	    }
	}
    }
}

void Maps::Tiles::RedrawTop(const TilesAddon * skip) const
{
    Display & display = Display::Get();
    const Rect & area = GameArea::Get().GetRect();
    const Point mp(maps_index % world.w(), maps_index / world.w());

    if(area & mp)
    {
	const s16 dstx = BORDERWIDTH + TILEWIDTH * (mp.x - area.x);
	const s16 dsty = BORDERWIDTH + TILEWIDTH * (mp.y - area.y);

	if(addons_level2.size())
	{
	    std::list<TilesAddon>::const_iterator it1 = addons_level2.begin();
	    std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	    for(; it1 != it2; ++it1)
	    {
		if(skip && skip == &(*it1)) continue;

		const u8 & object = (*it1).object;
		const u8 & index  = (*it1).index;
	        const ICN::icn_t icn = MP2::GetICNObject(object);

		if(ICN::UNKNOWN != icn && ICN::MINIHERO != icn && ICN::MONS32 != icn)
		{
		    const Sprite & sprite = AGG::GetICN(icn, index);
		    display.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		    // possible anime
		    if(const u16 anime_index = ICN::AnimationFrame(icn, index, Maps::AnimationTicket()))
		    {
			const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
			display.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		    }
		}
	    }
	}
    }
}

Maps::TilesAddon * Maps::Tiles::FindAddonICN1(u16 icn1)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(icn1 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonICN2(u16 icn2)
{
    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(icn2 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonLevel1(u32 uniq1)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(uniq1 == addon.uniq) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindAddonLevel2(u32 uniq2)
{
    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    if(uniq2 == addon.uniq) return &addon;
	}
    }

    return NULL;
}

void Maps::Tiles::DebugInfo(void) const
{
    std::list<TilesAddon>::const_iterator it1;
    std::list<TilesAddon>::const_iterator it2;

    std::cout << std::endl << "----------------:--------" << std::endl;

    std::string value;

    String::AddInt(value, maps_index);

    std::cout << "maps index      : " << value << std::endl;
    
    value.clear();
    
    String::AddInt(value, tile_index);

    std::cout << "tile            : " << value << std::endl;
    
    value.clear();

    value = Ground::String(GetGround());
    if(isRoad()) value += ", (road)";
    std::cout << "ground          : " << value << std::endl;

    value.clear();

    value = isPassable() ? "true" : "false";
    std::cout << "passable        : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, general);
    value += ", (" + std::string(MP2::StringObject(general)) + ")";
    std::cout << "general object  : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, quantity1);
    
    std::cout << "quantity 1      : " << value << std::endl;

    value.clear();
    
    String::AddInt(value, quantity2);
    
    std::cout << "quantity 2      : " << value << std::endl;

    if(addons_level1.size())
    {
	it1 = addons_level1.begin();
	it2 = addons_level1.end();
	
	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    std::cout << "----------------1--------" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.object);
    
	    std::cout << "object          : " << value << " (" << ICN::GetString(MP2::GetICNObject(addon.object)) << ")" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.index);
    
	    std::cout << "index           : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.uniq);
    
	    std::cout << "uniq            : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.level);
    
	    std::cout << "level           : " << value << std::endl;
	}
    }

    if(addons_level2.size())
    {
	it1 = addons_level2.begin();
	it2 = addons_level2.end();
	
	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    std::cout << "----------------2--------" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.object);
    
	    std::cout << "object          : " << value << " (" << ICN::GetString(MP2::GetICNObject(addon.object)) << ")" << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.index);
    
	    std::cout << "index           : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.uniq);
    
	    std::cout << "uniq            : " << value << std::endl;

	    value.clear();
    
	    String::AddInt(value, addon.level);
    
	    std::cout << "level           : " << value << std::endl;
	}
    }

    // extra obj info
    switch(general)
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
	    std::cout << "----------------I--------" << std::endl;
	    std::cout << "count           : " << GetCountMonster() << std::endl;
	    break;

	case MP2::OBJ_HEROES:
	    {
		const Heroes *hero = world.GetHeroes(maps_index);
		if(hero) hero->Dump();
	    }
	    break;

	default: break;
    }

    std::cout << "----------------:--------" << std::endl << std::endl;
}

MP2::object_t Maps::Tiles::GetObject(void) const
{
    switch(general)
    {
	case MP2::OBJ_ZERO:		return MP2::OBJ_ZERO;
	case MP2::OBJN_ALCHEMYLAB:	return MP2::OBJN_ALCHEMYLAB;
	case MP2::OBJN_DAEMONCAVE:	return MP2::OBJN_DAEMONCAVE;
	case MP2::OBJN_FAERIERING:	return MP2::OBJN_FAERIERING;
	case MP2::OBJN_GAZEBO:		return MP2::OBJN_GAZEBO;
	case MP2::OBJN_GRAVEYARD:	return MP2::OBJN_GRAVEYARD;
        case MP2::OBJN_ARCHERHOUSE:	return MP2::OBJN_ARCHERHOUSE;
        case MP2::OBJN_DWARFCOTT:	return MP2::OBJN_DWARFCOTT;

        case MP2::OBJN_PEASANTHUT:	return MP2::OBJN_PEASANTHUT;
	case MP2::OBJN_DRAGONCITY:	return MP2::OBJN_DRAGONCITY;
	case MP2::OBJN_LIGHTHOUSE:	return MP2::OBJN_LIGHTHOUSE;
	case MP2::OBJN_WATERWHEEL:	return MP2::OBJN_WATERWHEEL;
	case MP2::OBJN_MINES:		return MP2::OBJN_MINES;
	case MP2::OBJN_OBELISK:		return MP2::OBJN_OBELISK;
	case MP2::OBJN_OASIS:		return MP2::OBJN_OASIS;
	case MP2::OBJ_COAST:		return MP2::OBJ_COAST;
	case MP2::OBJN_SAWMILL:		return MP2::OBJN_SAWMILL;
	case MP2::OBJN_ORACLE:		return MP2::OBJN_ORACLE;

	case MP2::OBJN_DERELICTSHIP:	return MP2::OBJN_DERELICTSHIP;
	case MP2::OBJN_DESERTTENT:	return MP2::OBJN_DESERTTENT;
	case MP2::OBJN_CASTLE:		return MP2::OBJN_CASTLE;
        case MP2::OBJN_STONELIGHTS:	return MP2::OBJN_STONELIGHTS;
	case MP2::OBJN_WAGONCAMP:	return MP2::OBJN_WAGONCAMP;
	case MP2::OBJN_WINDMILL:	return MP2::OBJN_WINDMILL;

	case MP2::OBJN_RNDTOWN:		return MP2::OBJN_RNDTOWN;
	case MP2::OBJN_RNDCASTLE:	return MP2::OBJN_RNDCASTLE;
	case MP2::OBJ_SHRUB2:		return MP2::OBJ_SHRUB2;
	case MP2::OBJ_NOTHINGSPECIAL:	return MP2::OBJ_NOTHINGSPECIAL;
	case MP2::OBJN_WATCHTOWER:	return MP2::OBJN_WATCHTOWER;
	case MP2::OBJN_TREEHOUSE:	return MP2::OBJN_TREEHOUSE;
	case MP2::OBJN_TREECITY:	return MP2::OBJN_TREECITY;
	case MP2::OBJN_RUINS:		return MP2::OBJN_RUINS;
	case MP2::OBJN_FORT:		return MP2::OBJN_FORT;
	case MP2::OBJN_TRADINGPOST:	return MP2::OBJN_TRADINGPOST;

	case MP2::OBJN_ABANDONEDMINE:	return MP2::OBJN_ABANDONEDMINE;
	case MP2::OBJN_TREEKNOWLEDGE:	return MP2::OBJN_TREEKNOWLEDGE;
	case MP2::OBJN_DOCTORHUT:	return MP2::OBJN_DOCTORHUT;
	case MP2::OBJN_TEMPLE:		return MP2::OBJN_TEMPLE;
	case MP2::OBJN_HILLFORT:	return MP2::OBJN_HILLFORT;
	case MP2::OBJN_HALFLINGHOLE:	return MP2::OBJN_HALFLINGHOLE;
	case MP2::OBJN_MERCENARYCAMP:	return MP2::OBJN_MERCENARYCAMP;
	case MP2::OBJN_PYRAMID:		return MP2::OBJN_PYRAMID;
	case MP2::OBJN_CITYDEAD:	return MP2::OBJN_CITYDEAD;
	case MP2::OBJN_EXCAVATION:	return MP2::OBJN_EXCAVATION;
	case MP2::OBJN_SPHINX:		return MP2::OBJN_SPHINX;

	case MP2::OBJ_TARPIT:		return MP2::OBJ_TARPIT;
	case MP2::OBJN_TROLLBRIDGE:	return MP2::OBJN_TROLLBRIDGE;
	case MP2::OBJN_WITCHSHUT:	return MP2::OBJN_WITCHSHUT;
	case MP2::OBJN_XANADU:		return MP2::OBJN_XANADU;
	case MP2::OBJN_CAVE:		return MP2::OBJN_CAVE;
	case MP2::OBJN_MAGELLANMAPS:	return MP2::OBJN_MAGELLANMAPS;
	case MP2::OBJN_SHIPWRECK:	return MP2::OBJN_SHIPWRECK;
	case MP2::OBJN_MAGICWELL:	return MP2::OBJN_MAGICWELL;

	case MP2::OBJN_OBSERVATIONTOWER:return MP2::OBJN_OBSERVATIONTOWER;
	case MP2::OBJN_FREEMANFOUNDRY:	return MP2::OBJN_FREEMANFOUNDRY;
	case MP2::OBJ_TREES:		return MP2::OBJ_TREES;
	case MP2::OBJ_MOUNTS:		return MP2::OBJ_MOUNTS;
	case MP2::OBJ_VOLCANO:		return MP2::OBJ_VOLCANO;
	case MP2::OBJ_STONES:		return MP2::OBJ_STONES;
	case MP2::OBJ_FLOWERS:		return MP2::OBJ_FLOWERS;
	case MP2::OBJ_WATERLAKE:	return MP2::OBJ_WATERLAKE;
	case MP2::OBJ_MANDRAKE:		return MP2::OBJ_MANDRAKE;
	case MP2::OBJ_DEADTREE:		return MP2::OBJ_DEADTREE;
	case MP2::OBJ_STUMP:		return MP2::OBJ_STUMP;
	case MP2::OBJ_CRATER:		return MP2::OBJ_CRATER;
	case MP2::OBJ_CACTUS:		return MP2::OBJ_CACTUS;
	case MP2::OBJ_MOUND:		return MP2::OBJ_MOUND;
	case MP2::OBJ_DUNE:		return MP2::OBJ_DUNE;

	case MP2::OBJ_LAVAPOOL:		return MP2::OBJ_LAVAPOOL;
	case MP2::OBJ_SHRUB:		return MP2::OBJ_SHRUB;

	case MP2::OBJ_ALCHEMYLAB:	return MP2::OBJ_ALCHEMYLAB;
	case MP2::OBJ_SIGN:		return MP2::OBJ_SIGN;
	case MP2::OBJ_BUOY:		return MP2::OBJ_BUOY;
	case MP2::OBJ_SKELETON:		return MP2::OBJ_SKELETON;
	case MP2::OBJ_DAEMONCAVE:	return MP2::OBJ_DAEMONCAVE;
	case MP2::OBJ_TREASURECHEST:	return MP2::OBJ_TREASURECHEST;
	case MP2::OBJ_FAERIERING:	return MP2::OBJ_FAERIERING;
	case MP2::OBJ_CAMPFIRE:		return MP2::OBJ_CAMPFIRE;
	case MP2::OBJ_FOUNTAIN:		return MP2::OBJ_FOUNTAIN;
	case MP2::OBJ_GAZEBO:		return MP2::OBJ_GAZEBO;
	case MP2::OBJ_ANCIENTLAMP:	return MP2::OBJ_ANCIENTLAMP;
	case MP2::OBJ_GRAVEYARD:	return MP2::OBJ_GRAVEYARD;
	case MP2::OBJ_ARCHERHOUSE:	return MP2::OBJ_ARCHERHOUSE;
	case MP2::OBJ_GOBLINHUT:	return MP2::OBJ_GOBLINHUT;
	case MP2::OBJ_DWARFCOTT:	return MP2::OBJ_DWARFCOTT;

	case MP2::OBJ_PEASANTHUT:	return MP2::OBJ_PEASANTHUT;
	case MP2::OBJ_EVENT:		return MP2::OBJ_EVENT;
	case MP2::OBJ_DRAGONCITY:	return MP2::OBJ_DRAGONCITY;
	case MP2::OBJ_LIGHTHOUSE:	return MP2::OBJ_LIGHTHOUSE;
	case MP2::OBJ_WATERWHEEL:	return MP2::OBJ_WATERWHEEL;
	case MP2::OBJ_MINES:		return MP2::OBJ_MINES;
	case MP2::OBJ_MONSTER:		return MP2::OBJ_MONSTER;
	case MP2::OBJ_OBELISK:		return MP2::OBJ_OBELISK;
	case MP2::OBJ_OASIS:		return MP2::OBJ_OASIS;
	case MP2::OBJ_RESOURCE:		return MP2::OBJ_RESOURCE;
	case MP2::OBJ_SAWMILL:		return MP2::OBJ_SAWMILL;
	case MP2::OBJ_ORACLE:		return MP2::OBJ_ORACLE;
	case MP2::OBJ_SHRINE1:		return MP2::OBJ_SHRINE1;

	case MP2::OBJ_DERELICTSHIP:	return MP2::OBJ_DERELICTSHIP;
	case MP2::OBJ_DESERTTENT:	return MP2::OBJ_DESERTTENT;
	case MP2::OBJ_CASTLE:		return MP2::OBJ_CASTLE;
	case MP2::OBJ_STONELIGHTS:	return MP2::OBJ_STONELIGHTS;
	case MP2::OBJ_WAGONCAMP:	return MP2::OBJ_WAGONCAMP;
	case MP2::OBJ_WHIRLPOOL:	return MP2::OBJ_WHIRLPOOL;
	case MP2::OBJ_WINDMILL:		return MP2::OBJ_WINDMILL;
	case MP2::OBJ_ARTIFACT:		return MP2::OBJ_ARTIFACT;
	case MP2::OBJ_BOAT:		return MP2::OBJ_BOAT;
	case MP2::OBJ_RNDULTIMATEARTIFACT:return MP2::OBJ_RNDULTIMATEARTIFACT;
	case MP2::OBJ_RNDARTIFACT:	return MP2::OBJ_RNDARTIFACT;
	case MP2::OBJ_RNDRESOURCE:	return MP2::OBJ_RNDRESOURCE;
	case MP2::OBJ_RNDMONSTER:	return MP2::OBJ_RNDMONSTER;

	case MP2::OBJ_RNDTOWN:		return MP2::OBJ_RNDTOWN;
	case MP2::OBJ_RNDCASTLE:	return MP2::OBJ_RNDCASTLE;
	case MP2::OBJ_RNDMONSTER1:	return MP2::OBJ_RNDMONSTER1;
	case MP2::OBJ_RNDMONSTER2:	return MP2::OBJ_RNDMONSTER2;
	case MP2::OBJ_RNDMONSTER3:	return MP2::OBJ_RNDMONSTER3;
	case MP2::OBJ_RNDMONSTER4:	return MP2::OBJ_RNDMONSTER4;
	case MP2::OBJ_HEROES:		return MP2::OBJ_HEROES;
	case MP2::OBJ_WATCHTOWER:	return MP2::OBJ_WATCHTOWER;
	case MP2::OBJ_TREEHOUSE:	return MP2::OBJ_TREEHOUSE;
	case MP2::OBJ_TREECITY:		return MP2::OBJ_TREECITY;
	case MP2::OBJ_RUINS:		return MP2::OBJ_RUINS;
	case MP2::OBJ_FORT:		return MP2::OBJ_FORT;
	case MP2::OBJ_TRADINGPOST:	return MP2::OBJ_TRADINGPOST;

	case MP2::OBJ_ABANDONEDMINE:	return MP2::OBJ_ABANDONEDMINE;
	case MP2::OBJ_THATCHEDHUT:	return MP2::OBJ_THATCHEDHUT;
	case MP2::OBJ_STANDINGSTONES:	return MP2::OBJ_STANDINGSTONES;
	case MP2::OBJ_IDOL:		return MP2::OBJ_IDOL;
	case MP2::OBJ_TREEKNOWLEDGE:	return MP2::OBJ_TREEKNOWLEDGE;
	case MP2::OBJ_DOCTORHUT:	return MP2::OBJ_DOCTORHUT;
	case MP2::OBJ_TEMPLE:		return MP2::OBJ_TEMPLE;
	case MP2::OBJ_HILLFORT:		return MP2::OBJ_HILLFORT;
	case MP2::OBJ_HALFLINGHOLE:	return MP2::OBJ_HALFLINGHOLE;
	case MP2::OBJ_MERCENARYCAMP:	return MP2::OBJ_MERCENARYCAMP;
	case MP2::OBJ_SHRINE2:		return MP2::OBJ_SHRINE2;
	case MP2::OBJ_SHRINE3:		return MP2::OBJ_SHRINE3;
	case MP2::OBJ_PYRAMID:		return MP2::OBJ_PYRAMID;
	case MP2::OBJ_CITYDEAD:		return MP2::OBJ_CITYDEAD;
	case MP2::OBJ_EXCAVATION:	return MP2::OBJ_EXCAVATION;
	case MP2::OBJ_SPHINX:		return MP2::OBJ_SPHINX;

	case MP2::OBJ_WAGON:		return MP2::OBJ_WAGON;
	case MP2::OBJN_ARTESIANSPRING:	return MP2::OBJN_ARTESIANSPRING;
	case MP2::OBJ_ARTESIANSPRING:	return MP2::OBJ_ARTESIANSPRING;
	case MP2::OBJ_TROLLBRIDGE:	return MP2::OBJ_TROLLBRIDGE;
	case MP2::OBJN_WATERINGHOLE:	return MP2::OBJN_WATERINGHOLE;
	case MP2::OBJ_WATERINGHOLE:	return MP2::OBJ_WATERINGHOLE;
	case MP2::OBJ_WITCHSHUT:	return MP2::OBJ_WITCHSHUT;
	case MP2::OBJ_XANADU:		return MP2::OBJ_XANADU;
	case MP2::OBJ_CAVE:		return MP2::OBJ_CAVE;
	case MP2::OBJ_LEANTO:		return MP2::OBJ_LEANTO;
	case MP2::OBJ_MAGELLANMAPS:	return MP2::OBJ_MAGELLANMAPS;
	case MP2::OBJ_FLOTSAM:		return MP2::OBJ_FLOTSAM;
	case MP2::OBJ_SHIPWRECK:	return MP2::OBJ_SHIPWRECK;
	case MP2::OBJ_SHIPWRECKSURVIROR:return MP2::OBJ_SHIPWRECKSURVIROR;
	case MP2::OBJ_BOTTLE:		return MP2::OBJ_BOTTLE;
	case MP2::OBJ_MAGICWELL:	return MP2::OBJ_MAGICWELL;
	case MP2::OBJ_MAGICGARDEN:	return MP2::OBJ_MAGICGARDEN;

	case MP2::OBJ_OBSERVATIONTOWER:	return MP2::OBJ_OBSERVATIONTOWER;
	case MP2::OBJ_FREEMANFOUNDRY:	return MP2::OBJ_FREEMANFOUNDRY;

	case MP2::OBJ_RNDARTIFACT1:	return MP2::OBJ_RNDARTIFACT1;
	case MP2::OBJ_RNDARTIFACT2:	return MP2::OBJ_RNDARTIFACT2;
	case MP2::OBJ_RNDARTIFACT3:	return MP2::OBJ_RNDARTIFACT3;


        // loyality version
        case MP2::OBJN_JAIL:		return MP2::OBJN_JAIL;
        case MP2::OBJ_JAIL:		return MP2::OBJ_JAIL;
        case MP2::OBJN_TRAVELLERTENT:	return MP2::OBJN_TRAVELLERTENT;
        case MP2::OBJ_TRAVELLERTENT:	return MP2::OBJ_TRAVELLERTENT;
        case MP2::OBJ_BARRIER:		return MP2::OBJ_BARRIER;
    
	case MP2::OBJN_FIREALTAR:	return MP2::OBJN_FIREALTAR;
	case MP2::OBJ_FIREALTAR:	return MP2::OBJ_FIREALTAR;
	case MP2::OBJN_AIRALTAR:	return MP2::OBJN_AIRALTAR;
	case MP2::OBJ_AIRALTAR:		return MP2::OBJ_AIRALTAR;
	case MP2::OBJN_EARTHALTAR:	return MP2::OBJN_EARTHALTAR;
	case MP2::OBJ_EARTHALTAR:	return MP2::OBJ_EARTHALTAR;
	case MP2::OBJN_WATERALTAR:	return MP2::OBJN_WATERALTAR;
	case MP2::OBJ_WATERALTAR:	return MP2::OBJ_WATERALTAR;
	case MP2::OBJN_BARROWMOUNDS:	return MP2::OBJN_BARROWMOUNDS;
	case MP2::OBJ_BARROWMOUNDS:	return MP2::OBJ_BARROWMOUNDS;
	case MP2::OBJN_ARENA:		return MP2::OBJN_ARENA;
	case MP2::OBJ_ARENA:		return MP2::OBJ_ARENA;
	case MP2::OBJN_STABLES:		return MP2::OBJN_STABLES;
	case MP2::OBJ_STABLES:		return MP2::OBJ_STABLES;
	case MP2::OBJN_ALCHEMYTOWER:	return MP2::OBJN_ALCHEMYTOWER;
	case MP2::OBJ_ALCHEMYTOWER:	return MP2::OBJ_ALCHEMYTOWER;
	case MP2::OBJN_HUTMAGI:		return MP2::OBJN_HUTMAGI;
	case MP2::OBJ_HUTMAGI:		return MP2::OBJ_HUTMAGI;
	case MP2::OBJN_EYEMAGI:		return MP2::OBJN_EYEMAGI;
	case MP2::OBJ_EYEMAGI:		return MP2::OBJ_EYEMAGI;
	case MP2::OBJN_MERMAID:		return MP2::OBJN_MERMAID;
	case MP2::OBJ_MERMAID:		return MP2::OBJ_MERMAID;
	case MP2::OBJN_SIRENS:		return MP2::OBJN_SIRENS;
	case MP2::OBJ_SIRENS:		return MP2::OBJ_SIRENS;
	case MP2::OBJ_REEFS:		return MP2::OBJ_REEFS;

	case MP2::OBJ_UNKNW_02:		return MP2::OBJ_UNKNW_02;
	case MP2::OBJ_UNKNW_03:		return MP2::OBJ_UNKNW_03;
	case MP2::OBJ_UNKNW_04:		return MP2::OBJ_UNKNW_04;
	case MP2::OBJ_UNKNW_06:		return MP2::OBJ_UNKNW_06;
	case MP2::OBJ_UNKNW_08:		return MP2::OBJ_UNKNW_08;
	case MP2::OBJ_UNKNW_09:		return MP2::OBJ_UNKNW_09;
	case MP2::OBJ_UNKNW_0B:		return MP2::OBJ_UNKNW_0B;
	case MP2::OBJ_UNKNW_0E:		return MP2::OBJ_UNKNW_0E;
	case MP2::OBJ_UNKNW_11:		return MP2::OBJ_UNKNW_11;
	case MP2::OBJ_UNKNW_12:		return MP2::OBJ_UNKNW_12;
	case MP2::OBJ_UNKNW_13:		return MP2::OBJ_UNKNW_13;
	case MP2::OBJ_UNKNW_18:		return MP2::OBJ_UNKNW_18;
	case MP2::OBJ_UNKNW_1B:		return MP2::OBJ_UNKNW_1B;
	case MP2::OBJ_UNKNW_1F:		return MP2::OBJ_UNKNW_1F;
	case MP2::OBJ_UNKNW_21:		return MP2::OBJ_UNKNW_21;
	case MP2::OBJ_UNKNW_26:		return MP2::OBJ_UNKNW_26;
	case MP2::OBJ_UNKNW_27:		return MP2::OBJ_UNKNW_27;
	case MP2::OBJ_UNKNW_29:		return MP2::OBJ_UNKNW_29;
	case MP2::OBJ_UNKNW_2A:		return MP2::OBJ_UNKNW_2A;
	case MP2::OBJ_UNKNW_2B:		return MP2::OBJ_UNKNW_2B;
	case MP2::OBJ_UNKNW_2C:		return MP2::OBJ_UNKNW_2C;
	case MP2::OBJ_UNKNW_2D:		return MP2::OBJ_UNKNW_2D;
	case MP2::OBJ_UNKNW_2E:		return MP2::OBJ_UNKNW_2E;
	case MP2::OBJ_UNKNW_2F:		return MP2::OBJ_UNKNW_2F;
	case MP2::OBJ_UNKNW_32:		return MP2::OBJ_UNKNW_32;
	case MP2::OBJ_UNKNW_33:		return MP2::OBJ_UNKNW_33;
	case MP2::OBJ_UNKNW_34:		return MP2::OBJ_UNKNW_34;
	case MP2::OBJ_UNKNW_35:		return MP2::OBJ_UNKNW_35;
	case MP2::OBJ_UNKNW_36:		return MP2::OBJ_UNKNW_36;
	case MP2::OBJ_UNKNW_37:		return MP2::OBJ_UNKNW_37;
	case MP2::OBJ_UNKNW_41:		return MP2::OBJ_UNKNW_41;
	case MP2::OBJ_UNKNW_42:		return MP2::OBJ_UNKNW_42;
	case MP2::OBJ_UNKNW_43:		return MP2::OBJ_UNKNW_43;
	case MP2::OBJ_UNKNW_4A:		return MP2::OBJ_UNKNW_4A;
	case MP2::OBJ_UNKNW_4B:		return MP2::OBJ_UNKNW_4B;
	case MP2::OBJ_UNKNW_50:		return MP2::OBJ_UNKNW_50;
	case MP2::OBJ_UNKNW_58:		return MP2::OBJ_UNKNW_58;
	case MP2::OBJ_UNKNW_5A:		return MP2::OBJ_UNKNW_5A;
	case MP2::OBJ_UNKNW_5C:		return MP2::OBJ_UNKNW_5C;
	case MP2::OBJ_UNKNW_5D:		return MP2::OBJ_UNKNW_5D;
	case MP2::OBJ_UNKNW_5F:		return MP2::OBJ_UNKNW_5F;
	case MP2::OBJ_UNKNW_62:		return MP2::OBJ_UNKNW_62;
	case MP2::OBJ_UNKNW_79:		return MP2::OBJ_UNKNW_79;
	case MP2::OBJ_UNKNW_7A:		return MP2::OBJ_UNKNW_7A;
	case MP2::OBJ_UNKNW_80:		return MP2::OBJ_UNKNW_80;
	case MP2::OBJ_UNKNW_91:		return MP2::OBJ_UNKNW_91;
	case MP2::OBJ_UNKNW_92:		return MP2::OBJ_UNKNW_92;
	case MP2::OBJ_UNKNW_A1:		return MP2::OBJ_UNKNW_A1;
	case MP2::OBJ_UNKNW_A6:		return MP2::OBJ_UNKNW_A6;
	case MP2::OBJ_UNKNW_AA:		return MP2::OBJ_UNKNW_AA;
	case MP2::OBJ_UNKNW_B2:		return MP2::OBJ_UNKNW_B2;
	case MP2::OBJ_UNKNW_B8:		return MP2::OBJ_UNKNW_B8;
	case MP2::OBJ_UNKNW_B9:		return MP2::OBJ_UNKNW_B9;
	case MP2::OBJ_UNKNW_D1:		return MP2::OBJ_UNKNW_D1;
	case MP2::OBJ_UNKNW_E2:		return MP2::OBJ_UNKNW_E2;
	case MP2::OBJ_UNKNW_E3:		return MP2::OBJ_UNKNW_E3;
	case MP2::OBJ_UNKNW_E4:		return MP2::OBJ_UNKNW_E4;
	case MP2::OBJ_UNKNW_E5:		return MP2::OBJ_UNKNW_E5;
	case MP2::OBJ_UNKNW_E6:		return MP2::OBJ_UNKNW_E6;
	case MP2::OBJ_UNKNW_E7:		return MP2::OBJ_UNKNW_E7;
	case MP2::OBJ_UNKNW_E8:		return MP2::OBJ_UNKNW_E8;
	case MP2::OBJ_UNKNW_F9:		return MP2::OBJ_UNKNW_F9;
	case MP2::OBJ_UNKNW_FA:		return MP2::OBJ_UNKNW_FA;


	default: Error::Warning("Maps::Tiles::GetObject: unknown object: ", general); Error::Warning("maps index: ", maps_index); break;
    }
    
    return MP2::OBJ_ZERO;
}

/* accept move */
bool Maps::Tiles::isPassable(void) const
{
    if(Game::Focus::Get().Type() != Game::Focus::HEROES) return false;
    if(0 == Settings::Get().Debug() && isFog(Settings::Get().MyColor())) return false;


    if(Game::Focus::Get().GetHeroes().isShipMaster())
    {
    	if(Ground::WATER != Maps::Tiles::GetGround()) return false;

        switch(general)
	{
	    case MP2::OBJ_BOAT:
            case MP2::OBJ_HEROES:	return false;

	    default: break;
	}
    }
    else
    {
	if(Ground::WATER == Maps::Tiles::GetGround()) return false;

        switch(general)
	{
            case MP2::OBJ_HEROES:	return false;

	    default: break;
	}

    }

    return Object::isPassable(addons_level1);
}

/* check road */
bool Maps::Tiles::isRoad(const Direction::vector_t & direct) const
{
    switch(direct)
    {
	case Direction::UNKNOWN:
	case Direction::CENTER:	return addons_level1.end() != find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRoad);

	default: break;
    }

    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1) if(direct & TilesAddon::isRoad(*it1)) return true;
    }

    return false;
}

bool Maps::Tiles::isStream(void) const
{
    return addons_level1.end() != std::find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isStream);
}

Maps::TilesAddon * Maps::Tiles::FindResource(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNRSRC
	    if(ICN::OBJNRSRC == MP2::GetICNObject(addon.object) && (addon.index % 2)) return &addon;
	    else
	    // OBJNWATR
	    if(ICN::OBJNWATR == MP2::GetICNObject(addon.object) && 
		(0 == addon.index ||	// buttle
		19 == addon.index ||	// chest
		45 == addon.index ||	// flotsam
		111 == addon.index))	// surviror
				return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDResource(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNRSRC
	    if(ICN::OBJNRSRC == MP2::GetICNObject(addon.object) && 17 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindArtifact(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && (addon.index % 2)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDArtifact(const u8 level)
{
    u8 index = 0xA3;

    switch(level)
    {
	case MP2::OBJ_RNDARTIFACT1: index = 0xA7; break;
	case MP2::OBJ_RNDARTIFACT2: index = 0xA9; break;
	case MP2::OBJ_RNDARTIFACT3: index = 0xAB; break;
	default: break;
    }

    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && index == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindUltimateArtifact(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNARTI
	    if(ICN::OBJNARTI == MP2::GetICNObject(addon.object) && 0xA4 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindMiniHero(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MINIHERO
	    if(ICN::MINIHERO == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindEvent(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNMUL2
            if(ICN::OBJNMUL2 == MP2::GetICNObject(addon.object) && 0xA3 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindBoat(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNWAT2
            if(ICN::OBJNWAT2 == MP2::GetICNObject(addon.object) && 0x17 == addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindCastle(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTOWN
            if(ICN::OBJNTOWN == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTOWN
            if(ICN::OBJNTOWN == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDCastle(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTWRD
            if(ICN::OBJNTWRD == MP2::GetICNObject(addon.object) && 32 > addon.index) return &addon;
	}
    }

    if(addons_level2.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level2.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // OBJNTWRD
            if(ICN::OBJNTWRD == MP2::GetICNObject(addon.object)  && 32 > addon.index) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindFlags(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // FLAG32
            if(ICN::FLAG32 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindRNDMonster(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MONS32
	    if(ICN::MONS32 == MP2::GetICNObject(addon.object) &&
	    (0x43 == addon.index ||
	    0x44 == addon.index ||
	    0x45 == addon.index ||
	    0x46 == addon.index)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindMonster(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MONS32
	    if(ICN::MONS32 == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
}

Maps::TilesAddon * Maps::Tiles::FindCampFire(void)
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    TilesAddon & addon = *it1;

	    // MTNDSRT
            if(ICN::MTNDSRT == MP2::GetICNObject(addon.object) && 61 == addon.index) return &addon;
	    else
	    // OBJNMULT
            if(ICN::OBJNMULT == MP2::GetICNObject(addon.object) && 131 == addon.index) return &addon;
	    else
	    // OBJNSNOW
            if(ICN::OBJNSNOW == MP2::GetICNObject(addon.object) && 4 == addon.index) return &addon;
	}
    }

    return NULL;
}

const Maps::TilesAddon * Maps::Tiles::FindMines(void) const
{
    if(addons_level1.size())
    {
	std::list<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    const TilesAddon & addon = *it1;

	    // EXTRAOVR
	    if(ICN::EXTRAOVR == MP2::GetICNObject(addon.object)) return &addon;
	}
    }

    return NULL;
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
	case Color::GRAY:	index = 6; break;

	default: break;
    }

    switch(obj)
    {
	case MP2::OBJ_MINES:		index += 14; CorrectFlags32(index); break;
	//case MP2::OBJ_DRAGONCITY:	index += 35; CorrectFlags32(index); break; unused
        case MP2::OBJ_LIGHTHOUSE:	index += 42; CorrectFlags32(index); break;

	case MP2::OBJ_ALCHEMYLAB:
	{
	    index += 21;
	    if(Maps::isValidDirection(maps_index, Direction::TOP))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP));
		tile.CorrectFlags32(index);
	    }
	}
	break;

	case MP2::OBJ_SAWMILL:
	{
	    index += 28;
	    if(Maps::isValidDirection(maps_index, Direction::TOP_RIGHT))
    	    {
    		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::TOP_RIGHT));
    		tile.CorrectFlags32(index);
	    }
	}
	break;

	case MP2::OBJ_CASTLE:
	{
	    index *= 2;

	    if(Maps::isValidDirection(maps_index, Direction::LEFT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::LEFT));
    		tile.CorrectFlags32(index);
	    }

	    if(Maps::isValidDirection(maps_index, Direction::RIGHT))
	    {
		Maps::Tiles & tile = world.GetTiles(Maps::GetDirectionIndex(maps_index, Direction::RIGHT));
    		tile.CorrectFlags32(index);
	    }
	}
	break;

	default: return;
    }
}

/* correct flags, ICN::FLAGS32 vesion */
void Maps::Tiles::CorrectFlags32(const u8 index)
{
    TilesAddon * taddon = FindFlags();

    // replace flag
    if(NULL != taddon)
	taddon->index = index;
    else
    // or new flag
	addons_level2.push_back(TilesAddon(TilesAddon::UPPER, world.GetUniq(), 0x38, index));
}

void Maps::Tiles::FixLoyaltyVersion(void)
{
    switch(general)
    {
	case MP2::OBJ_UNKNW_79:
        case MP2::OBJ_UNKNW_7A:
        case MP2::OBJ_UNKNW_F9:
        case MP2::OBJ_UNKNW_FA:
	{
	    std::list<TilesAddon>::iterator it1 = addons_level1.begin();
	    std::list<TilesAddon>::const_iterator it2 = addons_level1.end();

	    // check level1
	    for(; it1 != it2; ++it1)
	    {
		TilesAddon & addon = *it1;

		if(ICN::X_LOC1 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC2 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC3 == MP2::GetICNObject(addon.object))
		{
		    const u8 newobj = ObjLoyalty::LearnObject(addon);
		    if(MP2::OBJ_ZERO != newobj) general = newobj;
		    return;
		}
	    }

	    it1 = addons_level2.begin();
	    it2 = addons_level2.end();

	    // check level2
	    for(; it1 != it2; ++it1)
	    {
		TilesAddon & addon = *it1;

		if(ICN::X_LOC1 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC2 == MP2::GetICNObject(addon.object) ||
		    ICN::X_LOC3 == MP2::GetICNObject(addon.object))
		{
		    const u8 newobj = ObjLoyalty::LearnObject(addon);
		    if(MP2::OBJ_ZERO != newobj) general = newobj;
		    return;
		}
	    }

	    if(H2Config::Debug()) Error::Warning("Maps::Tiles::FixLoyaltyVersion: index: ", maps_index);

	} break;

	default: break;
    }
}

/* for few object need many resource */
void Maps::Tiles::UpdateQuantity(void)
{
    const TilesAddon * addon = NULL;

    switch(general)
    {
	case MP2::OBJ_SKELETON:
	    switch(Rand::Get(1, 2))
	    {
		case 1:
		    quantity2 = Rand::Get(1, 3);
            	    quantity1 = (1 == quantity2 ? Artifact::Rand1() : (2 == quantity2 ? Artifact::Rand2() : Artifact::Rand3()));
		    quantity2 = 0;
		break;
		default: break;
	    }
	break;

	case MP2::OBJ_WAGON:
	    switch(Rand::Get(1, 3))
	    {
		case 1:
            	    quantity1 = (1 == Rand::Get(1, 2) ? Artifact::Rand1() : Artifact::Rand2());
		    quantity2 = 0;
		break;
		case 2:
		    quantity1 = Resource::Rand();
		    quantity2 = Rand::Get(2, 5);
		break;
		default: break;
	    }
	break;

	case MP2::OBJ_ARTIFACT:
	    // quantity1 - art, quantity2 - conditions for pickup: 
	    // 1,2,3 - 2000g, 2500g+3res, 3000g+5res,
	    // 4,5 - need have skill wisard or leadership,
	    // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4, other - none
	    addon = FindArtifact();
	    if(addon)
	    {
		quantity1 = Artifact::FromIndexSprite(addon->index);
		quantity2 = Rand::Get(1, 18);
	    }
	break;

	case MP2::OBJ_RESOURCE:
	    addon = FindResource();
	    if(addon)
	    {
		quantity1 = Resource::FromIndexSprite(addon->index);
		quantity2 = Rand::Get(RNDRESOURCEMIN, RNDRESOURCEMAX);
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
	    quantity1 = Resource::Rand();
	    quantity2 = 2;
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
	    switch(Rand::Get(1, 4))
	    {
		// 25%: 500 gold + 10 wood
		case 1:
		    quantity1 = 5;
		    quantity2 = 10;
		break;
		// 25%: 200 gold + 5 wood
		case 2:
		    quantity1 = 2;
		    quantity2 = 5;
		break;
		// 25%: 5 wood
		case 3:
		    quantity2 = 5;
		break;
		default: break;
	    }
	break;

	case MP2::OBJ_SHIPWRECKSURVIROR:
	    // artifact
	    quantity2 = Rand::Get(1, 3);
            quantity1 = (1 == quantity2 ? Artifact::Rand1() : (2 == quantity2 ? Artifact::Rand2() : Artifact::Rand3()));
	    quantity2 = 0;
	break;

	case MP2::OBJ_TREASURECHEST:
	    if(Maps::Ground::WATER == GetGround())
	    {
		switch(Rand::Get(1, 10))
		{
            	    // 70% - 1500 gold
                    default:
                	quantity2 = 15;
            	    break;
                    // 20% - empty
                    case 7:
		    case 8:
            	    break;
                    // 10% - 1000 gold + art
		    case 10:
                	quantity1 = Artifact::Rand1();
			quantity2 = 10;
		    break;
		}
	    }
	    else
	    {
		switch(Rand::Get(1, 20))
		{
		    // 32% - 2000 gold or 1500 exp
		    default:
			quantity2 = 20;
		    break;
		    // 31% - 1500 gold or 1000 exp
		    case 2:
		    case 5:
		    case 8:
		    case 11:
		    case 14:
		    case 17:
			quantity2 = 15;
		    break;
		    // 31% - 1000 gold or 500 exp
		    case 3:
		    case 6:
		    case 9:
		    case 12:
		    case 15:
		    case 18:
			quantity2 = 10;
		    break;
		    // 10% - art
		    case 20:
			quantity1 = Artifact::Rand1();
		    break;
		}
	    }
	break;

	case MP2::OBJ_SHIPWRECK:
	    // variant: 10ghost(1000g), 15 ghost(2000g), 25ghost(5000g) or 50ghost(2000g+art)
	    quantity2 = Rand::Get(1, 4);
	    quantity1 = 4 == quantity2 ? Artifact::Rand() : 0;
	break;

	case MP2::OBJ_DERELICTSHIP:
	    // 5000 gold
	    quantity2 = 50;
	break;

	case MP2::OBJ_GRAVEYARD:
	    // 1000 gold + art
		quantity1 = Artifact::Rand();
		quantity2 = 10;
	break;

	case MP2::OBJ_PYRAMID:
	    quantity1 = 0; // FIXME: PackQuantity: Pyramid: add Spell Level 5
	break;

	case MP2::OBJ_DAEMONCAVE:
	    // 1000 exp or 1000 exp + 2500 gold or 1000 exp + art
	    switch(Rand::Get(1, 3))
	    {
		case 1:
		    quantity2 = 25;
		break;
		case 2:
		    quantity1 = Artifact::Rand();
		break;
		default: break;
	    }
	break;

	default: return;
    }
}

void Maps::Tiles::RemoveObjectSprite(void)
{
    const Maps::TilesAddon *addon = NULL;

    switch(general)
    {
	case MP2::OBJ_ARTIFACT:		addon = FindArtifact(); break;
	case MP2::OBJ_CAMPFIRE:		addon = FindCampFire(); break;

	case MP2::OBJ_BOTTLE:
	case MP2::OBJ_FLOTSAM:

	case MP2::OBJ_SHIPWRECKSURVIROR:
	case MP2::OBJ_TREASURECHEST:
	case MP2::OBJ_ANCIENTLAMP:
	case MP2::OBJ_RESOURCE:		addon = FindResource(); break;

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

u16 Maps::Tiles::GetCountMonster(void) const
{
    return quantity2 * 0xFF + quantity1;
}

void Maps::Tiles::SetCountMonster(const u16 count)
{
    quantity1 = count % 0xFF;
    quantity2 = count / 0xFF;
}
