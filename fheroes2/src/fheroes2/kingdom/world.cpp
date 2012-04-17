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
#include <fstream>
#include <functional>
#include <algorithm>
#include "agg.h" 
#include "artifact.h"
#include "resource.h"
#include "settings.h" 
#include "kingdom.h" 
#include "heroes.h" 
#include "castle.h" 
#include "game_static.h"
#include "gameevent.h"
#include "mp2.h"
#include "text.h"
#include "race.h"
#include "pairs.h"
#include "game_over.h"
#include "resource.h"
#include "game_focus.h"
#include "world.h"
#include "ai.h"

CapturedObject & CapturedObjects::Get(const s32 & index)
{
    std::map<s32, CapturedObject> & my = *this;
    return my[index];
}

void CapturedObjects::SetColor(const s32 & index, u8 col)
{
    Get(index).SetColor(col);
}

void CapturedObjects::Set(const s32 & index, u8 obj, u8 col)
{
    CapturedObject & co = Get(index);

    if(co.GetColor() != col && co.guardians.isValid())
	co.guardians.Reset();

    co.Set(obj, col);
}

u16 CapturedObjects::GetCount(u8 obj, u8 col) const
{
    u16 result = 0;

    const ObjectColor objcol(obj, col);

    for(const_iterator it = begin(); it != end(); ++it)
        if(objcol == (*it).second.objcol)
	    ++result;

    return result;
}

u16 CapturedObjects::GetCountMines(u8 type, u8 col) const
{
    u16 result = 0;

    const ObjectColor objcol1(MP2::OBJ_MINES, col);
    const ObjectColor objcol2(MP2::OBJ_HEROES, col);

    for(const_iterator it = begin(); it != end(); ++it)
    {
        const ObjectColor & objcol = (*it).second.objcol;

        if(objcol == objcol1 || objcol == objcol2)
        {
            // scan for find mines
            const Maps::TilesAddon * addon = world.GetTiles((*it).first).FindObject(MP2::OBJ_MINES);

            if(addon)
            {
                // index sprite EXTRAOVR
                if(0 == addon->index && Resource::ORE == type) ++result;
                else
                if(1 == addon->index && Resource::SULFUR == type) ++result;
                else
                if(2 == addon->index && Resource::CRYSTAL == type) ++result;
                else
                if(3 == addon->index && Resource::GEMS == type) ++result;
                else
                if(4 == addon->index && Resource::GOLD == type) ++result;
            }
        }
    }

    return result;
}

u8 CapturedObjects::GetColor(const s32 & index) const
{
    const_iterator it = find(index);
    return it != end() ? (*it).second.GetColor() : Color::NONE;
}

void CapturedObjects::ClearFog(u8 colors)
{
    // clear abroad objects
    for(const_iterator it = begin(); it != end(); ++it)
    {
	const ObjectColor & objcol = (*it).second.objcol;

	if(objcol.isColor(colors))
	{
	    u8 scoute = 0;

	    switch(objcol.first)
	    {
		case MP2::OBJ_MINES:
		case MP2::OBJ_ALCHEMYLAB:
		case MP2::OBJ_SAWMILL:	scoute = 2; break;

		case MP2::OBJ_LIGHTHOUSE: scoute = 4; break; // FIXME: scoute and lighthouse

		default: break;
	    }

    	    if(scoute) Maps::ClearFog((*it).first, scoute, colors);
	}
    }
}

void CapturedObjects::ResetColor(u8 color)
{
    for(iterator it = begin(); it != end(); ++it)
    {
	ObjectColor & objcol = (*it).second.objcol;

	if(objcol.isColor(color))
	{
	    objcol.second = Color::UNUSED;
	    world.GetTiles((*it).first).CaptureFlags32(objcol.first, objcol.second);
	}
    }
}

Funds CapturedObjects::TributeCapturedObject(u8 color, u8 obj)
{
    Funds result;

    for(iterator it = begin(); it != end(); ++it)
    {
	const ObjectColor & objcol = (*it).second.objcol;

	if(objcol.isObject(obj) && objcol.isColor(color))
	{
	    Maps::Tiles & tile = world.GetTiles((*it).first);

	    result += Funds(tile.QuantityResourceCount());
	    tile.QuantityReset();
	}
    }

    return result;
}

World & world = World::Get();

namespace GameStatic
{
    extern u32 uniq;
}

World & World::Get(void)
{
    static World insideWorld;

    return insideWorld;
}

void World::Defaults(void)
{
    // playing kingdom
    vec_kingdoms.Init();

    // initialize all heroes
    vec_heroes.Init();

    vec_castles.Init();
}

/* new maps */
void World::NewMaps(const u16 sw, const u16 sh)
{
    Reset();
    Defaults();

    width = sw;
    height = sh;

    AGG::Cache::PreloadObject(TIL::GROUND32);

    vec_tiles.resize(width * height);

    // init all tiles
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
	MP2::mp2tile_t mp2tile;
	
	mp2tile.tileIndex	= Rand::Get(16, 19);	// index sprite ground, see ground32.til
        mp2tile.objectName1	= 0;			// object sprite level 1
        mp2tile.indexName1	= 0xff;			// index sprite level 1
        mp2tile.quantity1	= 0;
        mp2tile.quantity2	= 0;
        mp2tile.objectName2	= 0;			// object sprite level 2
        mp2tile.indexName2	= 0xff;			// index sprite level 2
        mp2tile.shape		= Rand::Get(0, 3);	// shape reflect % 4, 0 none, 1 vertical, 2 horizontal, 3 any
        mp2tile.generalObject	= MP2::OBJ_ZERO;
        mp2tile.indexAddon	= 0;
        mp2tile.uniqNumber1	= 0;
        mp2tile.uniqNumber2	= 0;

	(*it).Init(std::distance(vec_tiles.begin(), it), mp2tile);
    }

    Maps::FileInfo & fi = Settings::Get().CurrentFileInfo();

    // reset current maps info
    fi.size_w = width;
    fi.size_h = height;
}

/* load maps */
void World::LoadMaps(const std::string &filename)
{
    Reset();
    Defaults();

    std::ifstream fd(filename.c_str(), std::ios::binary);
    if(!fd.is_open())
    {
	 DEBUG(DBG_GAME|DBG_ENGINE, DBG_WARN, "file not found " << filename.c_str());
	 Error::Except(__FUNCTION__, "load maps");
    }

    AGG::Cache::PreloadObject(TIL::GROUND32);

    u8   byte8;
    u16  byte16;
    u32  byte32;
    MapsIndexes vec_object; // index maps for OBJ_CASTLE, OBJ_HEROES, OBJ_SIGN, OBJ_BOTTLE, OBJ_EVENT
    vec_object.reserve(100);

    // endof
    fd.seekg(0, std::ios_base::end);
    const u32 endof_mp2 = fd.tellg();

    // read uniq
    fd.seekg(endof_mp2 - sizeof(u32), std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&GameStatic::uniq), sizeof(u32));
    SwapLE32(GameStatic::uniq);

    // offset data
    fd.seekg(MP2OFFSETDATA - 2 * sizeof(u32), std::ios_base::beg);

    // width
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SwapLE32(byte32);

    switch(byte32)
    {
        case Maps::SMALL:  width = Maps::SMALL;  break;
        case Maps::MEDIUM: width = Maps::MEDIUM; break;
        case Maps::LARGE:  width = Maps::LARGE;  break;
        case Maps::XLARGE: width = Maps::XLARGE; break;
	default: width = byte32; break;
    }

    // height
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SwapLE32(byte32);

    switch(byte32)
    {
        case Maps::SMALL:  height = Maps::SMALL;  break;
        case Maps::MEDIUM: height = Maps::MEDIUM; break;
        case Maps::LARGE:  height = Maps::LARGE;  break;
        case Maps::XLARGE: height = Maps::XLARGE; break;
	default: height = byte32; break;
    }

    //if(byte32 != static_cast<u32>(height)) DEBUG(DBG_GAME, DBG_WARN, "incrrect maps size");

    // seek to ADDONS block
    fd.ignore(width * height * SIZEOFMP2TILE);

    // count mp2addon_t
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SwapLE32(byte32);

    // read all addons
    std::vector<MP2::mp2addon_t> vec_mp2addons(byte32);

    for(unsigned int ii = 0; ii < byte32; ++ii)
    {
	MP2::mp2addon_t & mp2addon = vec_mp2addons[ii];

	fd.read(reinterpret_cast<char *>(&mp2addon.indexAddon), sizeof(u16));
	SwapLE16(mp2addon.indexAddon);

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2addon.objectNameN1 = byte8 * 2;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2addon.indexNameN1 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2addon.quantityN = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2addon.objectNameN2 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2addon.indexNameN2 = byte8;

	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN1), sizeof(u32));
	SwapLE32(mp2addon.uniqNumberN1);

	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN2), sizeof(u32));
	SwapLE32(mp2addon.uniqNumberN2);
    }

    const u32 endof_addons = fd.tellg();

    DEBUG(DBG_GAME, DBG_INFO, "read all tiles addons, tellg: " << endof_addons);

    // offset data
    fd.seekg(MP2OFFSETDATA, std::ios_base::beg);

    vec_tiles.resize(width * height);

    // read all tiles
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
	const size_t index = std::distance(vec_tiles.begin(), it);
	Maps::Tiles & tile = *it;

	MP2::mp2tile_t mp2tile;

	// byte16
	fd.read(reinterpret_cast<char *>(&mp2tile.tileIndex), sizeof(u16));
	SwapLE16(mp2tile.tileIndex);

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.objectName1 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.indexName1 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.quantity1 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.quantity2 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.objectName2 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.indexName2 = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.shape = byte8;

	fd.read(reinterpret_cast<char *>(&byte8), 1);
	mp2tile.generalObject = byte8;

	switch(mp2tile.generalObject)
	{
	    case MP2::OBJ_RNDTOWN:
	    case MP2::OBJ_RNDCASTLE:
	    case MP2::OBJ_CASTLE:
	    case MP2::OBJ_HEROES:
	    case MP2::OBJ_SIGN:
	    case MP2::OBJ_BOTTLE:
	    case MP2::OBJ_EVENT:
	    case MP2::OBJ_SPHINX:
	    case MP2::OBJ_JAIL:
		vec_object.push_back(index);
		break;
	    default:
		break;
	}

	// offset first addon
	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SwapLE16(byte16);

	// byte32
	fd.read(reinterpret_cast<char *>(&mp2tile.uniqNumber1), sizeof(u32));
	SwapLE32(mp2tile.uniqNumber1);

	// byte32
	fd.read(reinterpret_cast<char *>(&mp2tile.uniqNumber2), sizeof(u32));
	SwapLE32(mp2tile.uniqNumber2);

	tile.Init(index, mp2tile);

	// load all addon for current tils
	while(byte16)
	{
	    if(vec_mp2addons.size() <= byte16){ DEBUG(DBG_GAME, DBG_WARN, "index out of range"); break; }
	    tile.AddonsPushLevel1(vec_mp2addons[byte16]);
	    tile.AddonsPushLevel2(vec_mp2addons[byte16]);
	    byte16 = vec_mp2addons[byte16].indexAddon;
	}

	tile.AddonsSort();
    }

    DEBUG(DBG_GAME, DBG_INFO, "read all tiles, tellg: " << fd.tellg());

    // after addons
    fd.seekg(endof_addons, std::ios_base::beg);

    // cood castles
    // 72 x 3 byte (cx, cy, id)
    for(u8 ii = 0; ii < 72; ++ii)
    {
	u8 cx, cy, id;

	fd.read(reinterpret_cast<char *>(&cx), 1);
	fd.read(reinterpret_cast<char *>(&cy), 1);
	fd.read(reinterpret_cast<char *>(&id), 1);
	
	// empty block
	if(0xFF == cx && 0xFF == cy) continue;

	switch(id)
	{
	    case 0x00: // tower: knight
	    case 0x80: // castle: knight
		vec_castles.push_back(new Castle(cx, cy, Race::KNGT));	break;

	    case 0x01: // tower: barbarian
	    case 0x81: // castle: barbarian
		vec_castles.push_back(new Castle(cx, cy, Race::BARB));	break;

	    case 0x02: // tower: sorceress
	    case 0x82: // castle: sorceress
		vec_castles.push_back(new Castle(cx, cy, Race::SORC));	break;

	    case 0x03: // tower: warlock
	    case 0x83: // castle: warlock
		vec_castles.push_back(new Castle(cx, cy, Race::WRLK));	break;

	    case 0x04: // tower: wizard
	    case 0x84: // castle: wizard
		vec_castles.push_back(new Castle(cx, cy, Race::WZRD));	break;

	    case 0x05: // tower: necromancer
	    case 0x85: // castle: necromancer
		vec_castles.push_back(new Castle(cx, cy, Race::NECR));	break;

	    case 0x06: // tower: random
	    case 0x86: // castle: random
		vec_castles.push_back(new Castle(cx, cy, Race::NONE));	break;

	    default:
		DEBUG(DBG_GAME, DBG_WARN, "castle block: " << "unknown id: " << static_cast<int>(id) << ", maps index: " << cx + cy * w());
		break;
	}
	// preload in to capture objects cache
	map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_CASTLE, Color::NONE);
    }

    DEBUG(DBG_GAME, DBG_INFO, "read coord castles, tellg: " << fd.tellg());
    fd.seekg(endof_addons + (72 * 3), std::ios_base::beg);

    // cood resource kingdoms
    // 144 x 3 byte (cx, cy, id)
    for(u16 ii = 0; ii < 144; ++ii)
    {
	u8 cx, cy, id;

	fd.read(reinterpret_cast<char *>(&cx), 1);
	fd.read(reinterpret_cast<char *>(&cy), 1);
	fd.read(reinterpret_cast<char *>(&id), 1);
	
	// empty block
	if(0xFF == cx && 0xFF == cy) continue;

	switch(id)
	{
	    // mines: wood
	    case 0x00:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_SAWMILL, Color::NONE);
		break; 
	    // mines: mercury
	    case 0x01:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_ALCHEMYLAB, Color::NONE);
		break;
	    // mines: ore
 	    case 0x02:
	    // mines: sulfur
	    case 0x03:
	    // mines: crystal
	    case 0x04:
	    // mines: gems
	    case 0x05:
	    // mines: gold
	    case 0x06:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_MINES, Color::NONE);
		break; 
	    // lighthouse
	    case 0x64:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_LIGHTHOUSE, Color::NONE);
		break; 
	    // dragon city
	    case 0x65:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_DRAGONCITY, Color::NONE);
		break; 
	    // abandoned mines
	    case 0x67:
		map_captureobj.Set(Maps::GetIndexFromAbsPoint(cx, cy), MP2::OBJ_ABANDONEDMINE, Color::NONE);
		break;
	    default:
		DEBUG(DBG_GAME, DBG_WARN, "kingdom block: " << "unknown id: " << static_cast<int>(id) << ", maps index: " << cx + cy * w());
		break;
	}
    }

    DEBUG(DBG_GAME, DBG_INFO, "read coord other resource, tellg: " << fd.tellg());
    fd.seekg(endof_addons + (72 * 3) + (144 * 3), std::ios_base::beg);

    // unknown byte
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    DEBUG(DBG_GAME, DBG_TRACE, "dump unknown byte: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte8));

    // count final mp2 blocks
    u16 countblock = 0;
    while(1)
    {
	u8 l = 0;
	u8 h = 0;

	// debug endof mp2
	//if(endof_mp2 < fd.tellg()) Error::Except(__FUNCTION__, "read maps: out of range.");

	fd.read(reinterpret_cast<char *>(&l), 1);
	fd.read(reinterpret_cast<char *>(&h), 1);

	DEBUG(DBG_GAME, DBG_TRACE, "dump final block: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(l) << \
		std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(h));

	if(0 == h && 0 == l) break;
	else
	{
	    countblock = 256 * h + l - 1;
	}
    }

    //DEBUG(DBG_GAME, DBG_INFO, "read find final mp2 blocks, tellg: " << fd.tellg());

    // castle or heroes or (events, rumors, etc)
    for(u16 ii = 0; ii < countblock; ++ii)
    {
	// debug
	//if(endof_mp2 < fd.tellg()) Error::Except(__FUNCTION__, "read maps: out of range.");

	// size block
	u16 sizeblock;
	fd.read(reinterpret_cast<char *>(&sizeblock), sizeof(u16));
	SwapLE16(sizeblock);

	u8 *pblock = new u8[sizeblock];

	// read block
	fd.read(reinterpret_cast<char *>(pblock), sizeblock);

	s32 findobject = -1;

	for(MapsIndexes::const_iterator
	    it_index = vec_object.begin(); it_index != vec_object.end() && findobject < 0; ++it_index)
	{
	    const Maps::Tiles & tile = vec_tiles[*it_index];

	    // orders(quantity2, quantity1)
	    u16 orders = (tile.GetQuantity2() ? tile.GetQuantity2() : 0);
	    orders <<= 8;
	    orders |= static_cast<u16>(tile.GetQuantity1());
	    
	    if(orders && !(orders % 0x08) && (ii + 1 == orders / 0x08))
		findobject = *it_index;
	}

	if(0 <= findobject)
	{
	    const Maps::Tiles & tile = vec_tiles[findobject];
	    const Maps::TilesAddon *addon = NULL;

	    switch(tile.GetObject())
	    {
		case MP2::OBJ_CASTLE:
		    // add castle
		    if(SIZEOFMP2CASTLE != sizeblock)
		    {
			DEBUG(DBG_GAME, DBG_WARN, "read castle: " << "incorrect size block: " << sizeblock);
		    }
		    else
		    {
			Castle* castle = GetCastle(findobject);
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			    map_captureobj.SetColor(tile.GetIndex(), castle->GetColor());
			}
			else
			{
			    DEBUG(DBG_GAME, DBG_WARN, "load castle: " << "not found, index: " << findobject);
			}
		    }
		    break;
		case MP2::OBJ_RNDTOWN:
		case MP2::OBJ_RNDCASTLE:
		    // add rnd castle
		    if(SIZEOFMP2CASTLE != sizeblock)
		    {
			DEBUG(DBG_GAME , DBG_WARN, "read castle: " << "incorrect size block: " << sizeblock);
		    }
		    else
		    {
			Castle* castle = GetCastle(findobject);
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::UpdateRNDSpriteForCastle(castle->GetCenter(), castle->GetRace(), castle->isCastle());
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			    map_captureobj.SetColor(tile.GetIndex(), castle->GetColor());
			}
			else
			{
			    DEBUG(DBG_GAME , DBG_WARN, "load castle: " << "not found, index: " << findobject);
			}
		    }
		    break;
		case MP2::OBJ_JAIL:
		    // add jail
		    if(SIZEOFMP2HEROES != sizeblock)
		    {
			DEBUG(DBG_GAME , DBG_WARN, "read heroes: " << "incorrect size block: " << sizeblock);
		    }
		    else
		    {
			u8 race = Race::KNGT;
			switch(pblock[0x3c])
			{
			    case 1: race = Race::BARB; break;
			    case 2: race = Race::SORC; break;
			    case 3: race = Race::WRLK; break;
			    case 4: race = Race::WZRD; break;
			    case 5: race = Race::NECR; break;
			    default: break;
			}

			Heroes* hero = GetFreemanHeroes(race);

			if(hero)
			{
			    hero->LoadFromMP2(findobject, pblock, Color::NONE, hero->GetRace());
			    hero->SetModes(Heroes::JAIL);
			}
		    }
		    break;
		case MP2::OBJ_HEROES:
		    // add heroes
		    if(SIZEOFMP2HEROES != sizeblock)
		    {
			DEBUG(DBG_GAME, DBG_WARN, "read heroes: " << "incorrect size block: " << sizeblock);
		    }
		    else
		    if(NULL != (addon = tile.FindObjectConst(MP2::OBJ_HEROES)))
		    {
			// calculate color
			const u8 index_name = addon->index;
			Color::color_t color = Color::NONE;

			if( 7 > index_name)
    			    color = Color::BLUE;
			else
			if(14 > index_name)
    			    color = Color::GREEN;
			else
	    		if(21 > index_name)
		    	    color = Color::RED;
			else
			if(28 > index_name)
			    color = Color::YELLOW;
			else
			if(35 > index_name)
			    color = Color::ORANGE;
			else
			    color = Color::PURPLE;

			Kingdom & kingdom = GetKingdom(color);

			// caclulate race
			u8 race = Race::NONE;
			switch(index_name % 7)
			{
			    case 0: race = Race::KNGT; break;
			    case 1: race = Race::BARB; break;
			    case 2: race = Race::SORC; break;
			    case 3: race = Race::WRLK; break;
			    case 4: race = Race::WZRD; break;
			    case 5: race = Race::NECR; break;
			    case 6: race = (Color::NONE != color ?
					    kingdom.GetRace() : Race::Rand()); break;
			}

			// check heroes max count
			if(kingdom.AllowRecruitHero(false, 0))
			{
			    Heroes* hero = NULL;

			    if(pblock[17] &&
				pblock[18] < Heroes::BAX)
				hero = vec_heroes.Get(static_cast<Heroes::heroes_t>(pblock[18]));

			    if(!hero || !hero->isFreeman())
				hero = vec_heroes.GetFreeman(race);

			    if(hero)
				hero->LoadFromMP2(findobject, pblock, color, race);
			}
			else
			{
			    DEBUG(DBG_GAME , DBG_WARN, "load heroes maximum");
			}
		    }
		    break;
		case MP2::OBJ_SIGN:
		case MP2::OBJ_BOTTLE:
		    // add sign or buttle
		    if(SIZEOFMP2SIGN - 1 < sizeblock && 0x01 == pblock[0])
			map_sign[findobject] = Game::GetEncodeString(reinterpret_cast<char *>(&pblock[9]));
		    break;
		case MP2::OBJ_EVENT:
		    // add event maps
		    if(SIZEOFMP2EVENT - 1 < sizeblock && 0x01 == pblock[0])
				    vec_eventsmap.push_back(EventMaps(findobject, pblock));
		    break;
		case MP2::OBJ_SPHINX:
		    // add riddle sphinx
		    if(SIZEOFMP2RIDDLE - 1 < sizeblock && 0x00 == pblock[0])
				    vec_riddles.push_back(Riddle(findobject, pblock));
		    break;
		default:
		    break;
	    }
	}
	// other events
	else 
	if(0x00 == pblock[0])
	{
	    // add event day
	    if(SIZEOFMP2EVENT - 1 < sizeblock && 1 == pblock[42])
		vec_eventsday.push_back(EventDate(pblock));

	    // add rumors
	    else if(SIZEOFMP2RUMOR - 1 < sizeblock)
	    {
		if(pblock[8])
		{
		    vec_rumors.push_back(Game::GetEncodeString(reinterpret_cast<char *>(&pblock[8])));
		    DEBUG(DBG_GAME, DBG_INFO, "add rumors: " << vec_rumors.back());
		}
	    }
	}
	// debug
	else
	{
	    DEBUG(DBG_GAME, DBG_WARN, "read maps: unknown block addons, size: " << sizeblock);
	}

	delete [] pblock;
    }

    // last rumors
    vec_rumors.push_back(_("You can load the newest version of game from a site:\n http://sf.net/projects/fheroes2"));
    vec_rumors.push_back(_("This game is now in beta development version. ;)"));

    // close mp2
    fd.close();

    // modify other objects
    for(size_t ii = 0; ii < vec_tiles.size(); ++ii)
    {
	Maps::Tiles & tile = vec_tiles[ii];

	Maps::Tiles::FixedPreload(tile);

	//
	switch(tile.GetObject())
	{
	    case MP2::OBJ_WITCHSHUT:
	    case MP2::OBJ_SHRINE1:
	    case MP2::OBJ_SHRINE2:
	    case MP2::OBJ_SHRINE3:
	    case MP2::OBJ_STONELIGHTS:
	    case MP2::OBJ_FOUNTAIN:
	    case MP2::OBJ_EVENT:
    	    case MP2::OBJ_BOAT:
    	    case MP2::OBJ_RNDARTIFACT:
    	    case MP2::OBJ_RNDARTIFACT1:
    	    case MP2::OBJ_RNDARTIFACT2:
    	    case MP2::OBJ_RNDARTIFACT3:
	    case MP2::OBJ_RNDRESOURCE:
	    case MP2::OBJ_WATERCHEST:
	    case MP2::OBJ_TREASURECHEST:
	    case MP2::OBJ_ARTIFACT:
	    case MP2::OBJ_RESOURCE:
            case MP2::OBJ_MAGICGARDEN:
            case MP2::OBJ_WATERWHEEL:
            case MP2::OBJ_WINDMILL:
            case MP2::OBJ_WAGON:
            case MP2::OBJ_SKELETON:
            case MP2::OBJ_LEANTO:
            case MP2::OBJ_CAMPFIRE:
            case MP2::OBJ_FLOTSAM:
            case MP2::OBJ_SHIPWRECKSURVIROR:
            case MP2::OBJ_DERELICTSHIP:
            case MP2::OBJ_SHIPWRECK:
            case MP2::OBJ_GRAVEYARD:
            case MP2::OBJ_PYRAMID:
            case MP2::OBJ_DAEMONCAVE:
            case MP2::OBJ_ABANDONEDMINE:
	    case MP2::OBJ_ALCHEMYLAB:
	    case MP2::OBJ_SAWMILL:
	    case MP2::OBJ_MINES:
	    case MP2::OBJ_TREEKNOWLEDGE:
	    case MP2::OBJ_BARRIER:
	    case MP2::OBJ_TRAVELLERTENT:
	    case MP2::OBJ_MONSTER:
	    case MP2::OBJ_RNDMONSTER:
	    case MP2::OBJ_RNDMONSTER1:
	    case MP2::OBJ_RNDMONSTER2:
	    case MP2::OBJ_RNDMONSTER3:
	    case MP2::OBJ_RNDMONSTER4:
	    case MP2::OBJ_ANCIENTLAMP:
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
	    case MP2::OBJ_RUINS:
            case MP2::OBJ_TREECITY:
            case MP2::OBJ_WAGONCAMP:
            case MP2::OBJ_DESERTTENT:
            case MP2::OBJ_TROLLBRIDGE:
            case MP2::OBJ_DRAGONCITY:
            case MP2::OBJ_CITYDEAD:
    		tile.QuantityUpdate();
		break;

	    case MP2::OBJ_WATERALTAR:
    	    case MP2::OBJ_AIRALTAR:
    	    case MP2::OBJ_FIREALTAR:
    	    case MP2::OBJ_EARTHALTAR:
	    case MP2::OBJ_BARROWMOUNDS:
    		tile.QuantityReset();
    		tile.QuantityUpdate();
		break;

	    case MP2::OBJ_HEROES:
	    {
    		Maps::TilesAddon* addon = tile.FindAddonICN1(ICN::MINIHERO);
    		// remove event sprite
    		if(addon) tile.Remove(addon->uniq);

    		tile.SetHeroes(GetHeroes(ii));
	    }
	    break;

	    default:
		break;
	}
    }

    // add heroes to kingdoms
    vec_kingdoms.AddHeroes(vec_heroes);

    // add castles to kingdoms
    vec_kingdoms.AddCastles(vec_castles);

    if(Settings::Get().ExtWorldStartHeroLossCond4Humans())
	vec_kingdoms.AddCondLossHeroes(vec_heroes);

    // update wins, loss conditions
    if(GameOver::WINS_HERO & Settings::Get().ConditionWins())
    {
	Heroes* hero = GetHeroes(Settings::Get().WinsMapsIndexObject());
	heroes_cond_wins = hero ? hero->GetID() : Heroes::UNKNOWN;
    }
    if(GameOver::LOSS_HERO & Settings::Get().ConditionLoss())
    {
	Heroes* hero = GetHeroes(Settings::Get().LossMapsIndexObject());
	if(hero)
	{
	    heroes_cond_loss = hero->GetID();
	    hero->SetModes(Heroes::NOTDISMISS | Heroes::NOTDEFAULTS);
	}
    }

    // update tile passable
    std::for_each(vec_tiles.begin(), vec_tiles.end(),
	    std::mem_fun_ref(&Maps::Tiles::UpdatePassable));

    // play with hero
    vec_kingdoms.ApplyPlayWithStartingHero();

    // play with debug hero
    if(IS_DEVEL())
    {
	// get first castle position
	Kingdom & kingdom = GetKingdom(Color::GetFirst(Players::HumanColors()));

	if(kingdom.GetCastles().size())
	{
	    const Castle* castle = kingdom.GetCastles().front();
	    Heroes* hero = vec_heroes.Get(Heroes::SANDYSANDY);

	    if(hero)
	    {
		const Point & cp = castle->GetCenter();
		hero->Recruit(castle->GetColor(), Point(cp.x, cp.y + 1));
	    }
	}
    }

    // set ultimate
    MapsTiles::iterator it = std::find_if(vec_tiles.begin(), vec_tiles.end(),
	    std::bind2nd(std::mem_fun_ref(&Maps::Tiles::isObject), MP2::OBJ_RNDULTIMATEARTIFACT));

    // not found
    if(vec_tiles.end() == it)
    {
	// generate position for ultimate
	MapsIndexes pools;
	pools.reserve(vec_tiles.size() / 2);

	for(size_t ii = 0; ii < vec_tiles.size(); ++ii)
	{
	    const Maps::Tiles & tile = vec_tiles[ii];
	    const u16 x = tile.GetIndex() % width;
	    const u16 y = tile.GetIndex() / width;
	    if(tile.GoodForUltimateArtifact() &&
		x > 5 && x < width - 5 && y > 5 && y < height - 5) pools.push_back(tile.GetIndex());
	}

	if(pools.size())
	{
	    const s32 pos = *Rand::Get(pools);
	    ultimate_artifact.Set(pos, Artifact::Rand(Artifact::ART_ULTIMATE));
	}
    }
    else
    {
	const Maps::TilesAddon *addon = NULL;

	// remove ultimate artifact sprite
	if(NULL != (addon = (*it).FindObjectConst(MP2::OBJ_RNDULTIMATEARTIFACT)))
	{
	    ultimate_artifact.Set((*it).GetIndex(), Artifact::FromMP2IndexSprite(addon->index));
	    (*it).Remove(addon->uniq);
	    (*it).SetObject(MP2::OBJ_ZERO);
	}
    }

    DEBUG(DBG_GAME, DBG_INFO, "end load");
}

Kingdoms & World::GetKingdoms(void)
{
    return vec_kingdoms;
}

/* get kingdom */
Kingdom & World::GetKingdom(u8 color)
{
    return vec_kingdoms.GetKingdom(color);
}

const Kingdom & World::GetKingdom(u8 color) const
{
    return vec_kingdoms.GetKingdom(color);
}

/* get castle from index maps */
Castle* World::GetCastle(s32 maps_index)
{
    return vec_castles.Get(maps_index);
}

const Castle* World::GetCastle(s32 maps_index) const
{
    return vec_castles.Get(maps_index);
}

Heroes* World::GetHeroes(Heroes::heroes_t id)
{
    return vec_heroes.Get(id);
}

const Heroes* World::GetHeroes(Heroes::heroes_t id) const
{
    return vec_heroes.Get(id);
}

/* get heroes from index maps */
Heroes* World::GetHeroes(s32 maps_index)
{
    return vec_heroes.Get(maps_index);
}

const Heroes* World::GetHeroes(s32 maps_index) const
{
    return vec_heroes.Get(maps_index);
}

CastleHeroes World::GetHeroes(const Castle & castle) const
{
    return CastleHeroes(vec_heroes.GetGuest(castle), vec_heroes.GetGuard(castle));
}

/* new day */
void World::NewDay(void)
{
    ++day;

    if(BeginWeek())
    {
        ++week;
        if(BeginMonth()) ++month;
    }

    // action new day
    vec_kingdoms.NewDay();

    // action new week
    if(BeginWeek())
    {
        NewWeek();
        vec_kingdoms.NewWeek();
    }
    
    // action new month
    if(BeginMonth())
    {
        NewMonth();
        vec_kingdoms.NewMonth();
    }

    // remove deprecated events
    if(day) vec_eventsday.remove_if(std::bind2nd(std::mem_fun_ref(&EventDate::isDeprecated), day - 1));
}

void World::NewWeek(void)
{
    // update week type
    week_current = week_next;
    const u8 type = LastWeek() ? Week::MonthRand() : Week::WeekRand();
    if(Week::MONSTERS == type)
	week_next = Week(type, LastWeek() ? Monster::Rand4MonthOf() : Monster::Rand4WeekOf());
    else
	week_next = Week(type);

    if(1 < week)
    {
	// update week object
	for(MapsTiles::iterator
	    it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
	    if(MP2::isWeekLife((*it).GetObject(false)) ||
		MP2::OBJ_MONSTER == (*it).GetObject()) (*it).QuantityUpdate();

	// update gray towns
        for(AllCastles::iterator
	    it = vec_castles.begin(); it != vec_castles.end(); ++it)
	    if((*it)->GetColor() == Color::NONE) (*it)->ActionNewWeek();
    }

    // add events
    if(Settings::Get().ExtWorldExtObjectsCaptured())
    {
	vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_WATERWHEEL);
	vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_WINDMILL);
	vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_MAGICGARDEN);
    }

    // new day - reset option: "heroes: remember MP/SP for retreat/surrender result"
    std::for_each(vec_heroes.begin(), vec_heroes.end(),
	std::bind2nd(std::mem_fun(&Heroes::ResetModes), Heroes::SAVEPOINTS));
}

void World::NewMonth(void)
{
    // skip first month
    if(1 < week && week_current.GetType() == Week::MONSTERS && !Settings::Get().ExtWorldBanMonthOfMonsters())
	MonthOfMonstersAction(Monster(week_current.GetMonster()));

    // update gray towns
    for(AllCastles::iterator
	it = vec_castles.begin(); it != vec_castles.end(); ++it)
	if((*it)->GetColor() == Color::NONE) (*it)->ActionNewMonth();
}

void World::MonthOfMonstersAction(const Monster & mons)
{
    if(mons.isValid())
    {
	MapsIndexes tiles, excld;
	tiles.reserve(vec_tiles.size() / 2);
	excld.reserve(vec_tiles.size() / 2);

	const u16 dist = 2;
	const u8 objs[] = { MP2::OBJ_MONSTER, MP2::OBJ_HEROES, MP2::OBJ_CASTLE, MP2::OBJN_CASTLE, 0 };

	// create exclude list
	{
	    const MapsIndexes & objv = Maps::GetObjectsPositions(objs);

	    for(MapsIndexes::const_iterator
		it = objv.begin(); it != objv.end(); ++it)
	    {
		const MapsIndexes & obja = Maps::GetDistanceIndexes(*it, dist);
		excld.insert(excld.end(), obja.begin(), obja.end());
	    }
	}

	// create valid points
	for(MapsTiles::const_iterator
	    it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
	{
	    const Maps::Tiles & tile = *it;

	    if(! tile.isWater() &&
		MP2::OBJ_ZERO == tile.GetObject() &&
		tile.isPassable(NULL, Direction::CENTER, true) &&
		excld.end() == std::find(excld.begin(), excld.end(), tile.GetIndex()))
	    {
		tiles.push_back(tile.GetIndex());
		const MapsIndexes & obja = Maps::GetDistanceIndexes(tile.GetIndex(), dist);
		excld.insert(excld.end(), obja.begin(), obja.end());
	    }
	}

	const u8 area = 12;
	const u16 maxc = (width / area) * (height / area);
	std::random_shuffle(tiles.begin(), tiles.end());
	if(tiles.size() > maxc) tiles.resize(maxc);

	for(MapsIndexes::const_iterator
	    it = tiles.begin(); it != tiles.end(); ++it)
		Maps::Tiles::PlaceMonsterOnTile(vec_tiles[*it], mons, 0 /* random */);
    }
}

void World::Reset(void)
{
    // maps tiles
    vec_tiles.clear();

    // kingdoms
    vec_kingdoms.clear();

    // event day
    vec_eventsday.clear();

    // event maps
    vec_eventsmap.clear();

    // riddle
    vec_riddles.clear();

    // rumors
    vec_rumors.clear();

    // castles
    vec_castles.clear();
    
    // heroes
    vec_heroes.clear();

    // extra
    map_sign.clear();
    map_captureobj.clear();

    ultimate_artifact.Reset();

    day = 0;
    week = 0;
    month = 0;

    week_current = Week::TORTOISE;
    week_next = Week::WeekRand();

    heroes_cond_wins = Heroes::UNKNOWN;
    heroes_cond_loss = Heroes::UNKNOWN;
}

Heroes* World::GetFreemanHeroes(u8 rc) const
{
    return vec_heroes.GetFreeman(rc);
}

const std::string & World::GetRumors(void)
{
    // vec_rumors always contain values
    return *Rand::Get(vec_rumors);
}

bool TeleportCheckType(s32 index, u8 type)
{
    return world.GetTiles(index).QuantityTeleportType() == type;
}

bool TeleportCheckGround(s32 index, bool water)
{
    return world.GetTiles(index).isWater() == water;
}

/* return random teleport destination */
s32 World::NextTeleport(const s32 index, bool onwater) const
{
    MapsIndexes vec_teleports = Maps::GetObjectPositions(MP2::OBJ_STONELIGHTS, true);

    if(2 > vec_teleports.size())
    {
	DEBUG(DBG_GAME, DBG_WARN, "is empty");
	return index;
    }

    MapsIndexes::iterator itend = vec_teleports.end();

    // remove if not type
    itend = std::remove_if(vec_teleports.begin(), itend,
		    std::not1(std::bind2nd(std::ptr_fun(&TeleportCheckType), GetTiles(index).QuantityTeleportType())));

    // remove if index
    itend = std::remove(vec_teleports.begin(), itend, index);

    // remove if not ground
    itend = std::remove_if(vec_teleports.begin(), itend,
		    std::not1(std::bind2nd(std::ptr_fun(&TeleportCheckGround), onwater)));

    vec_teleports.resize(std::distance(vec_teleports.begin(), itend));

    if(vec_teleports.empty()) DEBUG(DBG_GAME , DBG_WARN, "not found");

    return vec_teleports.size() ? *Rand::Get(vec_teleports) : index;
}

/* return random whirlpools destination */
s32 World::NextWhirlpool(const s32 index)
{
    MapsIndexes whilrpools = Maps::GetObjectPositions(MP2::OBJ_WHIRLPOOL, false);

    std::map<s32, MapsIndexes> uniq_whirlpools;

    for(MapsIndexes::const_iterator
	it = whilrpools.begin(); it != whilrpools.end(); ++it)
    {
    	const Maps::TilesAddon* addon = GetTiles(*it).FindObjectConst(MP2::OBJ_WHIRLPOOL);
	if(addon) uniq_whirlpools[addon->uniq].push_back(*it);
    }
    whilrpools.clear();

    if(2 > uniq_whirlpools.size())
    {
	DEBUG(DBG_GAME , DBG_WARN, "is empty");
	return index;
    }

    const Maps::TilesAddon* addon = GetTiles(index).FindObjectConst(MP2::OBJ_WHIRLPOOL);
    MapsIndexes uniqs;
    uniqs.reserve(uniq_whirlpools.size());

    if(addon)
    {
	for(std::map<s32, MapsIndexes>::const_iterator
	    it = uniq_whirlpools.begin(); it != uniq_whirlpools.end(); ++it)
	{
	    const u32 & uniq = (*it).first;
	    if(uniq == addon->uniq) continue;
	    uniqs.push_back(uniq);
	}
    }

    MapsIndexes & dest = uniq_whirlpools[*Rand::Get(uniqs)];
    uniqs.clear();

    if(dest.empty()) DEBUG(DBG_GAME, DBG_WARN, "is full");

    return dest.size() ? *Rand::Get(dest) : index;
}

/* return message from sign */
const std::string & World::MessageSign(const s32 index)
{
    return map_sign[index];
}

/* return count captured object */
u16 World::CountCapturedObject(u8 obj, u8 col) const
{
    return map_captureobj.GetCount(obj, col);
}

/* return count captured mines */
u16 World::CountCapturedMines(u8 type, u8 color) const
{
    switch(type)
    {
	case Resource::WOOD:	return CountCapturedObject(MP2::OBJ_SAWMILL, color);
	case Resource::MERCURY:	return CountCapturedObject(MP2::OBJ_ALCHEMYLAB, color);
	default: break;
    }

    return map_captureobj.GetCountMines(type, color);
}

/* capture object */
void World::CaptureObject(const s32 & index, u8 color)
{
    const MP2::object_t obj = GetTiles(index).GetObject(false);

    map_captureobj.Set(index, obj, color);

    if(MP2::OBJ_CASTLE == obj)
    {
	Castle* castle = GetCastle(index);
	if(castle) castle->ChangeColor(Color::Get(color));
    }

    if(color & (Color::ALL | Color::UNUSED))
	GetTiles(index).CaptureFlags32(obj, color);
}

/* return color captured object */
u8 World::ColorCapturedObject(const s32 & index) const
{
    return map_captureobj.GetColor(index);
}

CapturedObject & World::GetCapturedObject(const s32 & index)
{
    return map_captureobj.Get(index);
}

void World::ClearFog(u8 colors)
{
    if(Settings::Get().ExtUnionsAllowViewMaps())
	colors = Players::GetPlayerFriends(colors);

    // clear abroad castles
    vec_castles.Scoute(colors);

    // clear abroad heroes
    vec_heroes.Scoute(colors);

    map_captureobj.ClearFog(colors);
}

const UltimateArtifact & World::GetUltimateArtifact(void) const
{
    return ultimate_artifact;
}

bool World::DiggingForUltimateArtifact(const Point & center)
{
    Maps::Tiles & tile = GetTiles(center);

    // puts hole sprite
    u8 obj = 0;
    u8 idx = 0;
        
    switch(tile.GetGround())
    {
        case Maps::Ground::WASTELAND: obj = 0xE4; idx = 70; break;	// ICN::OBJNCRCK
        case Maps::Ground::DIRT:      obj = 0xE0; idx = 140; break;	// ICN::OBJNDIRT
        case Maps::Ground::DESERT:    obj = 0xDC; idx = 68; break;	// ICN::OBJNDSRT
        case Maps::Ground::LAVA:      obj = 0xD8; idx = 26; break;	// ICN::OBJNLAVA
        case Maps::Ground::GRASS:
        default:                      obj = 0xC0; idx = 9; break;	// ICN::OBJNGRA2
    }
    tile.AddonsPushLevel1(Maps::TilesAddon(0, GetUniq(), obj, idx));

    // reset
    if(ultimate_artifact.isPosition(tile.GetIndex()) && ! ultimate_artifact.isFound())
    {
	ultimate_artifact.SetFound(true);
	return true;
    }

    return false;
}

void World::ActionForMagellanMaps(u8 color)
{
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
	if((*it).isWater()) (*it).ClearFog(color);
}

void World::AddEventDate(const EventDate & event)
{
    vec_eventsday.push_back(event);
}

EventsDate World::GetEventsDate(u8 color) const
{
    EventsDate res;

    for(EventsDate::const_iterator
	it = vec_eventsday.begin(); it != vec_eventsday.end(); ++it)
	if((*it).isAllow(color, day)) res.push_back(*it);

    return res;
}

EventMaps* World::GetEventMaps(u8 color, s32 index)
{
    if(vec_eventsmap.size())
    {
	for(EventsMaps::iterator
	    it = vec_eventsmap.begin(); it != vec_eventsmap.end(); ++it)
	    if((*it).isAllow(color, index)) return &(*it);
    }

    return NULL;
}

std::string World::DateString(void) const
{
    std::ostringstream os;
    os << "month: " << static_cast<int>(GetMonth()) <<
	    ", " << "week: " << static_cast<int>(GetWeek()) << ", " << "day: " << static_cast<int>(GetDay());
    return os.str();
}

bool IsObeliskOnMaps(const Maps::Tiles & tile)
{
    return MP2::OBJ_OBELISK == tile.GetObject(false);
}

u16 World::CountObeliskOnMaps(void)
{
    u16 res = std::count_if(vec_tiles.begin(), vec_tiles.end(), IsObeliskOnMaps);
    return res ? res : 6;
}

void World::ResetCapturedObjects(u8 color)
{
    map_captureobj.ResetColor(color);
}

Heroes* World::FromJail(s32 index)
{
    return vec_heroes.FromJail(index);
}

void World::ActionToEyeMagi(u8 color) const
{
    MapsIndexes vec_eyes = Maps::GetObjectPositions(MP2::OBJ_EYEMAGI, true);

    if(vec_eyes.size())
    {
	for(MapsIndexes::const_iterator
	    it = vec_eyes.begin(); it != vec_eyes.end(); ++it)
	    Maps::ClearFog(*it, Game::GetViewDistance(Game::VIEW_MAGI_EYES), color);
    }
}

Riddle* World::GetSphinxRiddle(s32 index)
{
    Riddles::iterator
	it = std::find(vec_riddles.begin(), vec_riddles.end(), index);
    return it != vec_riddles.end() ? &(*it) : NULL;
}

void World::UpdateRecruits(Recruits & recruits) const
{
    if(vec_heroes.HaveTwoFreemans())
        while(recruits.GetID1() == recruits.GetID2()) recruits.SetHero2(GetFreemanHeroes());
    else
        recruits.SetHero2(NULL);
}

const Heroes* World::GetHeroesCondWins(void) const
{
    return GetHeroes(heroes_cond_wins);
}

const Heroes* World::GetHeroesCondLoss(void) const
{
    return GetHeroes(heroes_cond_loss);
}

bool World::KingdomIsWins(const Kingdom & kingdom, u16 wins) const
{
    const Settings & conf = Settings::Get();

    switch(wins)
    {
	case GameOver::WINS_ALL:
	    return kingdom.GetColor() == vec_kingdoms.GetNotLossColors();

	case GameOver::WINS_TOWN:
	{
	    const Castle* town = GetCastle(conf.WinsMapsIndexObject());
	    // check comp also wins
	    return (((CONTROL_HUMAN & kingdom.GetControl()) || conf.WinsCompAlsoWins()) &&
    	       (town && town->GetColor() == kingdom.GetColor()));
	}

	case GameOver::WINS_HERO:
	{
    	    const Heroes* hero = GetHeroesCondWins();
    	    return (hero && Heroes::UNKNOWN != heroes_cond_wins &&
    		    hero->isFreeman() &&
    		    hero->GetKillerColor() == kingdom.GetColor());
	}

	case GameOver::WINS_ARTIFACT:
	{
	    const KingdomHeroes & heroes  = kingdom.GetHeroes();
	    if(conf.WinsFindUltimateArtifact())
	    {
		return (heroes.end() != std::find_if(heroes.begin(), heroes.end(),
		    std::mem_fun(&Heroes::HasUltimateArtifact)));
	    }
	    else
	    {
		const Artifact art = conf.WinsFindArtifactID();
		return (heroes.end() != std::find_if(heroes.begin(), heroes.end(),
		    std::bind2nd(HeroHasArtifact(), art)));
	    }
	}

	case GameOver::WINS_SIDE:
	{
	    return ! (Game::GetActualKingdomColors() & ~Players::GetPlayerFriends(kingdom.GetColor()));
	}

	case GameOver::WINS_GOLD:
	    // check comp also wins
	    return (((CONTROL_HUMAN & kingdom.GetControl()) || conf.WinsCompAlsoWins()) &&
		    0 < kingdom.GetFunds().Get(Resource::GOLD) &&
		    static_cast<u32>(kingdom.GetFunds().Get(Resource::GOLD)) >= conf.WinsAccumulateGold());

	default: break;
    }

    return false;
}

bool World::KingdomIsLoss(const Kingdom & kingdom, u16 loss) const
{
    const Settings & conf = Settings::Get();

    switch(loss)
    {
	case GameOver::LOSS_ALL:
	    return kingdom.isLoss();

	case GameOver::LOSS_TOWN:
	{
    	    const Castle* town = GetCastle(conf.LossMapsIndexObject());
    	    return (town && town->GetColor() != kingdom.GetColor());
	}

	case GameOver::LOSS_HERO:
	{
    	    const Heroes* hero = GetHeroesCondLoss();
            return (hero && Heroes::UNKNOWN != heroes_cond_loss &&
    		    hero->isFreeman() &&
    		    hero->GetKillerColor() != kingdom.GetColor());
	}

	case GameOver::LOSS_TIME:
    	    return (CountDay() > conf.LossCountDays() && (CONTROL_HUMAN & kingdom.GetControl()));

	default: break;
    }

    return false;
}

u16 World::CheckKingdomWins(const Kingdom & kingdom) const
{
    const Settings & conf = Settings::Get();
    const u16 wins [] = { GameOver::WINS_ALL, GameOver::WINS_TOWN, GameOver::WINS_HERO, GameOver::WINS_ARTIFACT, GameOver::WINS_SIDE, GameOver::WINS_GOLD, 0 };

    for(u8 ii = 0; wins[ii]; ++ii)
	if((conf.ConditionWins() & wins[ii]) &&
	    KingdomIsWins(kingdom, wins[ii])) return wins[ii];

    return GameOver::COND_NONE;
}

u16 World::CheckKingdomLoss(const Kingdom & kingdom) const
{
    const Settings & conf = Settings::Get();

    // firs check priority: other WINS_GOLD or WINS_ARTIFACT
    if(conf.ConditionWins() & GameOver::WINS_GOLD)
    {
	u8 priority = vec_kingdoms.FindWins(GameOver::WINS_GOLD);
	if(priority && priority != kingdom.GetColor())
    	    return GameOver::LOSS_ALL;
    }
    else
    if(conf.ConditionWins() & GameOver::WINS_ARTIFACT)
    {
	u8 priority = vec_kingdoms.FindWins(GameOver::WINS_ARTIFACT);
	if(priority && priority != kingdom.GetColor())
    	    return GameOver::LOSS_ALL;
    }

    const u16 loss [] = { GameOver::LOSS_ALL, GameOver::LOSS_TOWN, GameOver::LOSS_HERO, GameOver::LOSS_TIME, 0 };

    for(u8 ii = 0; loss[ii]; ++ii)
	if((conf.ConditionLoss() & loss[ii]) &&
	    KingdomIsLoss(kingdom, loss[ii])) return loss[ii];

    if(conf.ExtWorldStartHeroLossCond4Humans())
    {
	if(kingdom.GetFirstHeroStartCondLoss())
	    return GameOver::LOSS_STARTHERO;
    }

    return GameOver::COND_NONE;
}

u32 World::GetUniq(void)
{
    return ++GameStatic::uniq;
}

StreamBase & operator<< (StreamBase & msg, const CapturedObject & obj)
{
    return msg << obj.objcol << obj.guardians << obj.split;
}

StreamBase & operator>> (StreamBase & msg, CapturedObject & obj)
{
    return msg >> obj.objcol >> obj.guardians >> obj.split;
}

StreamBase & operator<< (StreamBase & msg, const World & w)
{
    const Size & sz = w;

    return msg << sz <<
	w.vec_tiles <<
	w.vec_heroes <<
	w.vec_castles <<
	w.vec_kingdoms <<
	w.vec_rumors <<
	w.vec_eventsday <<
	w.vec_eventsmap <<
	w.vec_riddles <<
	w.map_sign <<
	w.map_captureobj <<
	w.ultimate_artifact <<
	w.day <<
	w.week <<
	w.month <<
	w.week_current <<
	w.week_next <<
	w.heroes_cond_wins <<
	w.heroes_cond_loss;
}

StreamBase & operator>> (StreamBase & msg, World & w)
{
    Size & sz = w;

    msg >> sz >>
	w.vec_tiles >>
	w.vec_heroes >>
	w.vec_castles >>
	w.vec_kingdoms >>
	w.vec_rumors >>
	w.vec_eventsday >>
	w.vec_eventsmap >>
	w.vec_riddles >>
	w.map_sign >>
	w.map_captureobj >>
	w.ultimate_artifact >>
	w.day >>
	w.week >>
	w.month >>
	w.week_current >>
	w.week_next >>
	w.heroes_cond_wins >>
	w.heroes_cond_loss;

    // update tile passable
    std::for_each(w.vec_tiles.begin(), w.vec_tiles.end(),
        std::mem_fun_ref(&Maps::Tiles::UpdatePassable));

    // heroes postfix
    std::for_each(w.vec_heroes.begin(), w.vec_heroes.end(),
	std::mem_fun(&Heroes::RescanPathPassable));

    return msg;
}
