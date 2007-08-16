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

#include <vector>
#include <iostream>
#include "agg.h"
#include "tools.h"
#include "world.h"
#include "race.h"
#include "config.h"
#include "color.h"
#include "error.h"
#include "heroes.h"
#include "sprite.h"
#include "maps.h"
#include "display.h"
#include "maps_tiles.h"

Maps::TilesAddon::TilesAddon(u8 lv, u32 gid, u8 obj, u8 ii) : level(GROUND), uniq(gid), object(obj), index(ii)
{
    switch(lv)
    {
	case GROUND:	level = GROUND;	break;
	case DOWN:	level = DOWN;	break;
	case SHADOW:	level = SHADOW;	break;
	case UPPER:	level = UPPER;	break;
    }
}

Maps::TilesAddon & Maps::TilesAddon::operator= (const Maps::TilesAddon & ta)
{
    level = ta.GetLevel();

    object = ta.GetObject();

    index = ta.GetIndex();

    uniq = ta.GetUniq();
    
    return *this;
}

Maps::Tiles::Tiles(u16 mi, const MP2::mp2tile_t & mp2tile) : maps_index(mi), tile_sprite(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE), tile_index(mp2tile.tileIndex),
    shape(mp2tile.shape), general(mp2tile.generalObject), quantity1(mp2tile.quantity1), quantity2(mp2tile.quantity2)
{
    tile_sprite.LoadPalette(AGG::GetPalette());
    AGG::GetTIL("GROUND32.TIL", mp2tile.tileIndex, mp2tile.shape, tile_sprite); 

    AddonsPushLevel1(mp2tile);
    AddonsPushLevel2(mp2tile);
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2tile_t & mt)
{
    if(mt.objectName1 && mt.indexName1 < 0xFF)
	addons_level1.push_back(TilesAddon(0, mt.uniqNumber1, mt.objectName1, mt.indexName1));
}

void Maps::Tiles::AddonsPushLevel1(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN1 && ma.indexNameN1 < 0xFF)
	addons_level1.push_back(TilesAddon(ma.quantityN % 4, ma.uniqNumberN1, ma.objectNameN1, ma.indexNameN1));
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2tile_t & mt)
{
    if(mt.objectName2 && mt.indexName2 < 0xFF)
	addons_level2.push_back(TilesAddon(0, mt.uniqNumber2, mt.objectName2, mt.indexName2));
}

void Maps::Tiles::AddonsPushLevel2(const MP2::mp2addon_t & ma)
{
    if(ma.objectNameN2 && ma.indexNameN2 < 0xFF)
	addons_level2.push_back(TilesAddon(ma.quantityN % 4, ma.uniqNumberN2, ma.objectNameN2, ma.indexNameN2));
}

void Maps::Tiles::AddonsSort(void)
{
    if(addons_level1.size()) std::sort(addons_level1.begin(), addons_level1.end(), TilesAddon::RulesCompare);

    if(addons_level2.size()) std::sort(addons_level2.begin(), addons_level2.end(), TilesAddon::RulesCompare);
}

Maps::Ground::ground_t Maps::Tiles::GetGround(void) const
{
    // maybe it is ROAD
    if(addons_level1.end() != find_if(addons_level1.begin(), addons_level1.end(), TilesAddon::isRoad))
	return Maps::Ground::ROAD;
    
    // list grouns from GROUND32.TIL
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
	std::vector<TilesAddon>::iterator       it1 = addons_level1.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1) if(uniq == (*it1).GetUniq()) addons_level1.erase(it1);
    }

    if(addons_level2.size())
    {
	std::vector<TilesAddon>::iterator       it1 = addons_level2.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1) if(uniq == (*it1).GetUniq()) addons_level1.erase(it1);
    }
}

void Maps::Tiles::Blit(u16 dstx, u16 dsty, u32 anime_frame) const
{
    // level 0
    display.Blit(tile_sprite, dstx, dsty);
    
    // level 1
    if(addons_level1.size())
    {
	std::vector<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    u8 object = (*it1).GetObject();
	    u8 index  = (*it1).GetIndex();
	    const char *icn = MP2::GetICNObject(object);

	    if(icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		display.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());
		
		// possible anime
		if(u8 anime_index = MP2::GetAnimationFrame(object, index, anime_frame))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    display.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }

    switch(general)
    {
	 // heroes
	case MP2::OBJ_HEROES:
	    RedrawHeroes(dstx, dsty);
	    break;

	// monster
	case MP2::OBJ_MONSTER:
	    RedrawMonster(dstx, dsty, anime_frame);
	    break;
	
	default:
	    break;
    }
    
    // level 2
    if(addons_level2.size())
    {
	std::vector<TilesAddon>::const_iterator it1 = addons_level2.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    u8 object = (*it1).GetObject();
	    u8 index  = (*it1).GetIndex();
	    const char *icn = MP2::GetICNObject(object);

	    if(icn)
	    {
		const Sprite & sprite = AGG::GetICN(icn, index);
		display.Blit(sprite, dstx + sprite.x(), dsty + sprite.y());

		// possible anime
		if(u8 anime_index = MP2::GetAnimationFrame(object, index, anime_frame))
		{
		    const Sprite & anime_sprite = AGG::GetICN(icn, anime_index);
		    display.Blit(anime_sprite, dstx + anime_sprite.x(), dsty + anime_sprite.y());
		}
	    }
	}
    }

    // put point for grid
    /*
    if(H2Config::Debug())
    {
	display.Lock();
	display.SetPixel2(dstx, dsty, AGG::GetColor(0x40));
	display.Unlock();
    }
    */
}

bool Maps::Tiles::isAnimation(u16 dstx, u16 dsty) const
{
    // check object
    switch(general)
    {
	case MP2::OBJ_HEROES:
	case MP2::OBJ_MONSTER:
	    return true;
	
	default: break;
    }
    
    // level 1
    if(addons_level1.size())
    {
	std::vector<TilesAddon>::const_iterator it1 = addons_level1.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level1.end();

	for(; it1 != it2; ++it1)
	{
	    u8 object = (*it1).GetObject();
	    u8 index  = (*it1).GetIndex();

	    if(MP2::GetICNObject(object) && MP2::GetAnimationFrame(object, index, 0)) return true;
	}
    }

    // level 2
    if(addons_level2.size())
    {
	std::vector<TilesAddon>::const_iterator it1 = addons_level2.begin();
	std::vector<TilesAddon>::const_iterator it2 = addons_level2.end();

	for(; it1 != it2; ++it1)
	{
	    u8 object = (*it1).GetObject();
	    u8 index  = (*it1).GetIndex();
	    
	    if(MP2::GetICNObject(object) && MP2::GetAnimationFrame(object, index, 0)) return true;
	}
    }
    
    return false;
}

const Maps::TilesAddon * Maps::Tiles::FindAddon(u8 object, u8 index_min, u8 index_max) const
{
    if(addons_level1.size())
	for(u8 ii = 0; ii < addons_level1.size(); ++ii)
	    if(object == addons_level1[ii].GetObject() &&
		index_min <= addons_level1[ii].GetIndex() &&
		index_max >= addons_level1[ii].GetIndex()) return &addons_level1[ii];

    if(addons_level2.size())
	for(u8 ii = 0; ii < addons_level2.size(); ++ii)
	    if(object == addons_level2[ii].GetObject() &&
		index_min <= addons_level2[ii].GetIndex() &&
		index_max >= addons_level2[ii].GetIndex()) return &addons_level2[ii];

    return NULL;
}

const Maps::TilesAddon * Maps::Tiles::FindAddon(u8 object, u8 index) const
{
    if(addons_level1.size())
	for(u8 ii = 0; ii < addons_level1.size(); ++ii)
	    if(object == addons_level1[ii].GetObject() &&
	      (0xFF == index || index == addons_level1[ii].GetIndex())) return &addons_level1[ii];

    if(addons_level2.size())
	for(u8 ii = 0; ii < addons_level2.size(); ++ii)
	    if(object == addons_level2[ii].GetObject() &&
	      (0xFF == index || index == addons_level2[ii].GetIndex())) return &addons_level2[ii];

    return NULL;
}

const Maps::TilesAddon * Maps::Tiles::FindAddonLevel1(u32 uniq1) const
{
    if(addons_level1.size())
	for(u8 ii = 0; ii < addons_level1.size(); ++ii)
	    if(uniq1 == addons_level1[ii].GetUniq()) return &addons_level1[ii];

    return NULL;
}

const Maps::TilesAddon * Maps::Tiles::FindAddonLevel2(u32 uniq2) const
{
    if(addons_level2.size())
	for(u8 ii = 0; ii < addons_level2.size(); ++ii)
	    if(uniq2 == addons_level2[ii].GetUniq()) return &addons_level2[ii];

    return NULL;
}

void Maps::Tiles::DebugInfo(u16 index) const
{
    std::cout << std::endl << "----------------:--------" << std::endl;

    std::string value;

    if(0xFFFF > index)
    {
	String::AddInt(value, index);

	std::cout << "index           : " << value << std::endl;
    
	value.clear();
    }
    
    String::AddInt(value, tile_index);

    std::cout << "tile            : " << value << std::endl;
    
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

    for(u8 ii = 0; ii < addons_level1.size(); ++ii)
    {
	std::cout << "----------------1--------" << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level1.at(ii).GetObject());
    
	std::cout << "object          : " << value << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level1.at(ii).GetIndex());
    
	std::cout << "index           : " << value << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level1.at(ii).GetUniq());
    
	std::cout << "uniq            : " << value << std::endl;
    }

    for(u8 ii = 0; ii < addons_level2.size(); ++ii)
    {
	std::cout << "----------------2--------" << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level2.at(ii).GetObject());
    
	std::cout << "object          : " << value << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level2.at(ii).GetIndex());
    
	std::cout << "index           : " << value << std::endl;

	value.clear();
    
	String::AddInt(value, addons_level2.at(ii).GetUniq());
    
	std::cout << "uniq            : " << value << std::endl;
    }

    std::cout << "----------------:--------" << std::endl << std::endl;
}

void Maps::Tiles::RedrawMonster(u16 dx, u16 dy, u32 anime_sprite) const
{
    const TilesAddon * addons = NULL;

    if((addons = FindAddon(0x30)) ||
       (addons = FindAddon(0x31)) ||
       (addons = FindAddon(0x32)) ||
       (addons = FindAddon(0x33)))
    {
	Monster::monster_t monster = Monster::Monster(addons->GetIndex());

	if(Monster::UNKNOWN <= monster)
	{
	    Error::Warning("Maps::Tiles::RedrawMonster: unknown monster, ", monster);

	    return;
	}

	// redraw top tiles
	if(BORDERWIDTH < dy)
	{
	    world.GetTiles(maps_index - world.w()).Blit(dx, dy - TILEWIDTH, anime_sprite);
	}

	// redraw left tiles
	if(BORDERWIDTH < dx)
	{
	    world.GetTiles(maps_index - 1).Blit(dx - TILEWIDTH, dy, anime_sprite);
	}

	// draw first sprite
	const Sprite & sprite_first = AGG::GetICN("MINIMON.ICN", monster * 9);

	s16 ax = dx + TILEWIDTH - std::abs(sprite_first.x()) - 19;
	s16 ay = dy + TILEWIDTH - std::abs(sprite_first.y()) - 4;

	Rect src_rt(0, 0, sprite_first.w(), sprite_first.h());

	// left bound
	if(ax < BORDERWIDTH)
	{
	    src_rt.x = (ax < 0 ? std::abs(ax) + BORDERWIDTH : BORDERWIDTH - ax);
	    src_rt.w -= src_rt.x;
	    ax = BORDERWIDTH;
	}

	// top bound
	if(ay < BORDERWIDTH)
	{
	    src_rt.y = (ay < 0 ? std::abs(ay) + BORDERWIDTH : BORDERWIDTH - ay);
	    src_rt.h -= src_rt.y;
	    ay = BORDERWIDTH;
	}

	display.Blit(sprite_first, src_rt, ax, ay);

	// draw second sprite
	const Sprite & sprite_next = AGG::GetICN("MINIMON.ICN", monster * 9 + 1 + (anime_sprite % 6));

	ax = dx + TILEWIDTH - std::abs(sprite_next.x()) - 19;
	ay = dy + TILEWIDTH - std::abs(sprite_next.y()) - 4;

	src_rt = Rect(0, 0, sprite_next.w(), sprite_next.h());

	// left bound
	if(ax < BORDERWIDTH)
	{
	    src_rt.x = (ax < 0 ? std::abs(ax) + BORDERWIDTH : BORDERWIDTH - ax);
	    src_rt.w -= src_rt.x;
	    ax = BORDERWIDTH;
	}

	// top bound
	if(ay < BORDERWIDTH)
	{
	    src_rt.y = (ay < 0 ? std::abs(ay) + BORDERWIDTH : BORDERWIDTH - ay);
	    src_rt.h -= src_rt.y;
	    ay = BORDERWIDTH;
	}

	display.Blit(sprite_next, src_rt, ax, ay);
    }
}

void Maps::Tiles::RedrawHeroes(u16 dx, u16 dy) const
{
    if(const Heroes *heroes = world.GetHeroes(maps_index))
    {
	const Race::race_t & race = (*heroes).GetRace();
	const Color::color_t & color = (*heroes).GetColor();
	
	u16 index_sprite = 0;

	switch(color)
	{
	    case Color::BLUE:	index_sprite =  0; break;
	    case Color::GREEN:	index_sprite =  7; break;
	    case Color::RED:	index_sprite = 14; break;
	    case Color::YELLOW:	index_sprite = 21; break;
	    case Color::ORANGE:	index_sprite = 28; break;
	    case Color::PURPLE:	index_sprite = 35; break;
	    default: Error::Warning("Maps::Tiles::RedrawHeroes: unknown color hero, maps index: ", maps_index); return;
	}

	switch(race)
	{
	    case Race::KNGT: break;
	    case Race::BARB: index_sprite += 1; break;
	    case Race::SORC: index_sprite += 2; break;
	    case Race::WRLK: index_sprite += 3; break;
	    case Race::WZRD: index_sprite += 4; break;
	    case Race::NECR: index_sprite += 5; break;
	    default: Error::Warning("Maps::Tiles::RedrawHeroes: unknown race hero, maps index: ", maps_index); return;
	}

	const Sprite & sprite = AGG::GetICN("MINIHERO.ICN", index_sprite);

	const Point dst_pt(dx + sprite.x(), BORDERWIDTH > dy + sprite.y() - 22 ? BORDERWIDTH : dy + sprite.y() - 22);
	const Rect  src_rt(0,  dst_pt.y > BORDERWIDTH ? 0 : 22, sprite.w(), sprite.h());

	display.Blit(sprite, src_rt, dst_pt);
    }
    else
	Error::Warning("Maps::Tiles::RedrawHeroes: unknown hero, maps index: ", maps_index);
}

MP2::object_t Maps::Tiles::GetObject(void) const
{
    switch(general)
    {
	case MP2::OBJ_ZERO:		return MP2::OBJ_ZERO;
	case MP2::OBJN_ALCHEMYTOWER:	return MP2::OBJN_ALCHEMYTOWER;
	case MP2::OBJN_DAEMONCAVE:	return MP2::OBJN_DAEMONCAVE;
	case MP2::OBJN_FAERIERING:	return MP2::OBJN_FAERIERING;
	case MP2::OBJN_GAZEBO:		return MP2::OBJN_GAZEBO;
	case MP2::OBJN_GRAVEYARD:	return MP2::OBJN_GRAVEYARD;
        case MP2::OBJN_ARCHERHOUSE:	return MP2::OBJN_ARCHERHOUSE;
        case MP2::OBJN_DWARFCOTT:	return MP2::OBJN_DWARFCOTT;

        case MP2::OBJN_PEASANTHUNT:	return MP2::OBJN_PEASANTHUNT;
	case MP2::OBJN_DRAGONCITY:	return MP2::OBJN_DRAGONCITY;
	case MP2::OBJN_LIGHTHOUSE:	return MP2::OBJN_LIGHTHOUSE;
	case MP2::OBJN_WATERMILL:	return MP2::OBJN_WATERMILL;
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
	case MP2::OBJN_DOCTORHUNT:	return MP2::OBJN_DOCTORHUNT;
	case MP2::OBJN_TEMPLE:		return MP2::OBJN_TEMPLE;
	case MP2::OBJN_HILLFORT:	return MP2::OBJN_HILLFORT;
	case MP2::OBJN_HALFLINGHOLE:	return MP2::OBJN_HALFLINGHOLE;
	case MP2::OBJN_MERCENARYCAMP:	return MP2::OBJN_MERCENARYCAMP;
	case MP2::OBJN_PIRAMID:		return MP2::OBJN_PIRAMID;
	case MP2::OBJN_CITYDEAD:	return MP2::OBJN_CITYDEAD;
	case MP2::OBJN_EXCAVATION:	return MP2::OBJN_EXCAVATION;
	case MP2::OBJN_SPHINX:		return MP2::OBJN_SPHINX;

	case MP2::OBJ_OILLAKE:		return MP2::OBJ_OILLAKE;
	case MP2::OBJN_TROLLBRIDGE:	return MP2::OBJN_TROLLBRIDGE;
	case MP2::OBJN_WITCHHUNT:	return MP2::OBJN_WITCHHUNT;
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
	case MP2::OBJ_VEGETATION2:	return MP2::OBJ_VEGETATION2;
	case MP2::OBJ_DEADTREE:		return MP2::OBJ_DEADTREE;
	case MP2::OBJ_STUMP:		return MP2::OBJ_STUMP;
	case MP2::OBJ_CRATER:		return MP2::OBJ_CRATER;
	case MP2::OBJ_CACTUS:		return MP2::OBJ_CACTUS;
	case MP2::OBJ_MOUND:		return MP2::OBJ_MOUND;
	case MP2::OBJ_DUNE:		return MP2::OBJ_DUNE;

	case MP2::OBJ_LAVALAKE:		return MP2::OBJ_LAVALAKE;
	case MP2::OBJ_SHRUB:		return MP2::OBJ_SHRUB;

	case MP2::OBJ_ALCHEMYTOWER:	return MP2::OBJ_ALCHEMYTOWER;
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
	case MP2::OBJ_GOBLINHUNT:	return MP2::OBJ_GOBLINHUNT;
	case MP2::OBJ_DWARFCOTT:	return MP2::OBJ_DWARFCOTT;

	case MP2::OBJ_PEASANTHUNT:	return MP2::OBJ_PEASANTHUNT;
	case MP2::OBJ_EVENT:		return MP2::OBJ_EVENT;
	case MP2::OBJ_DRAGONCITY:	return MP2::OBJ_DRAGONCITY;
	case MP2::OBJ_LIGHTHOUSE:	return MP2::OBJ_LIGHTHOUSE;
	case MP2::OBJ_WATERMILL:	return MP2::OBJ_WATERMILL;
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
	case MP2::OBJ_PEASANTHUNT2:	return MP2::OBJ_PEASANTHUNT2;
	case MP2::OBJ_STANDINGSTONES:	return MP2::OBJ_STANDINGSTONES;
	case MP2::OBJ_IDOL:		return MP2::OBJ_IDOL;
	case MP2::OBJ_TREEKNOWLEDGE:	return MP2::OBJ_TREEKNOWLEDGE;
	case MP2::OBJ_DOCTORHUNT:	return MP2::OBJ_DOCTORHUNT;
	case MP2::OBJ_TEMPLE:		return MP2::OBJ_TEMPLE;
	case MP2::OBJ_HILLFORT:		return MP2::OBJ_HILLFORT;
	case MP2::OBJ_HALFLINGHOLE:	return MP2::OBJ_HALFLINGHOLE;
	case MP2::OBJ_MERCENARYCAMP:	return MP2::OBJ_MERCENARYCAMP;
	case MP2::OBJ_SHRINE2:		return MP2::OBJ_SHRINE2;
	case MP2::OBJ_SHRINE3:		return MP2::OBJ_SHRINE3;
	case MP2::OBJ_PIRAMID:		return MP2::OBJ_PIRAMID;
	case MP2::OBJ_CITYDEAD:		return MP2::OBJ_CITYDEAD;
	case MP2::OBJ_EXCAVATION:	return MP2::OBJ_EXCAVATION;
	case MP2::OBJ_SPHINX:		return MP2::OBJ_SPHINX;

	case MP2::OBJ_WAGON:		return MP2::OBJ_WAGON;
	case MP2::OBJN_ARTESIANSPRING:	return MP2::OBJN_ARTESIANSPRING;
	case MP2::OBJ_ARTESIANSPRING:	return MP2::OBJ_ARTESIANSPRING;
	case MP2::OBJ_TROLLBRIDGE:	return MP2::OBJ_TROLLBRIDGE;
	case MP2::OBJN_CRAKEDLAKE:	return MP2::OBJN_CRAKEDLAKE;
	case MP2::OBJ_CRAKEDLAKE:	return MP2::OBJ_CRAKEDLAKE;
	case MP2::OBJ_WITCHHUNT:	return MP2::OBJ_WITCHHUNT;
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
	
	default: Error::Warning("Maps::Tiles::GetObject: unknown object: ", general); Error::Warning("maps index: ", maps_index); break;
    }
    
    return MP2::OBJ_ZERO;
}
