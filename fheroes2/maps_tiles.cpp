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

Maps::Tiles::Tiles(const MP2::mp2tile_t & mp2tile) : tile_sprite(TILEWIDTH, TILEWIDTH, 8, SDL_SWSURFACE), tile_index(mp2tile.tileIndex),
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
		index == addons_level1[ii].GetIndex()) return &addons_level1[ii];

    if(addons_level2.size())
	for(u8 ii = 0; ii < addons_level2.size(); ++ii)
	    if(object == addons_level2[ii].GetObject() &&
		index == addons_level2[ii].GetIndex()) return &addons_level2[ii];

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
