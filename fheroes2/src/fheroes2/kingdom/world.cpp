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
#include "gameevent.h"
#include "mp2.h"
#include "text.h"
#include "race.h"
#include "pairs.h"
#include "algorithm.h"
#include "game_over.h"
#include "resource.h"
#include "interface_gamearea.h"
#include "world.h"
#include "ai.h"

struct InCastleAndGuardian : public std::binary_function <const Castle*, Heroes*, bool>
{
    bool operator() (const Castle* castle, Heroes* hero) const
    {
	const Point & cpt = castle->GetCenter();
	const Point & hpt = hero->GetCenter();
	return cpt.x == hpt.x && cpt.y == hpt.y + 1 && hero->Modes(Heroes::GUARDIAN);
    }
};

struct InCastleNotGuardian : public std::binary_function <const Castle*, Heroes*, bool>
{
    bool operator() (const Castle* castle, Heroes* hero) const
    {
	return castle->GetCenter() == hero->GetCenter() && !hero->Modes(Heroes::GUARDIAN);
    }
};

bool PredicateHeroesIsFreeman(const Heroes *h)
{
    return h && h->isFreeman();
}

World & world = World::Get();

u32 World::uniq0 = 0;

World & World::Get(void)
{
    static World insideWorld;

    return insideWorld;
}

void World::Defaults(void)
{
    // playing kingdom
    vec_kingdoms.resize(KINGDOMMAX + 1);
    vec_kingdoms[0] = new Kingdom(Color::BLUE);
    vec_kingdoms[1] = new Kingdom(Color::GREEN);
    vec_kingdoms[2] = new Kingdom(Color::RED);
    vec_kingdoms[3] = new Kingdom(Color::YELLOW);
    vec_kingdoms[4] = new Kingdom(Color::ORANGE);
    vec_kingdoms[5] = new Kingdom(Color::PURPLE);
    vec_kingdoms[6] = new Kingdom(Color::GRAY);

    // initialize all heroes
    vec_heroes.resize(HEROESMAXCOUNT + 2);
    vec_heroes[0] = new Heroes(Heroes::LORDKILBURN, Race::KNGT);
    vec_heroes[1] = new Heroes(Heroes::SIRGALLANTH, Race::KNGT);
    vec_heroes[2] = new Heroes(Heroes::ECTOR, Race::KNGT);
    vec_heroes[3] = new Heroes(Heroes::GVENNETH, Race::KNGT);
    vec_heroes[4] = new Heroes(Heroes::TYRO, Race::KNGT);
    vec_heroes[5] = new Heroes(Heroes::AMBROSE, Race::KNGT);
    vec_heroes[6] = new Heroes(Heroes::RUBY, Race::KNGT);
    vec_heroes[7] = new Heroes(Heroes::MAXIMUS, Race::KNGT);
    vec_heroes[8] = new Heroes(Heroes::DIMITRY, Race::KNGT);

    vec_heroes[9] = new Heroes(Heroes::THUNDAX, Race::BARB);
    vec_heroes[10] = new Heroes(Heroes::FINEOUS, Race::BARB);
    vec_heroes[11] = new Heroes(Heroes::JOJOSH, Race::BARB);
    vec_heroes[12] = new Heroes(Heroes::CRAGHACK, Race::BARB);
    vec_heroes[13] = new Heroes(Heroes::JEZEBEL, Race::BARB);
    vec_heroes[14] = new Heroes(Heroes::JACLYN, Race::BARB);
    vec_heroes[15] = new Heroes(Heroes::ERGON, Race::BARB);
    vec_heroes[16] = new Heroes(Heroes::TSABU, Race::BARB);
    vec_heroes[17] = new Heroes(Heroes::ATLAS, Race::BARB);

    vec_heroes[18] = new Heroes(Heroes::ASTRA, Race::SORC);
    vec_heroes[19] = new Heroes(Heroes::NATASHA, Race::SORC);
    vec_heroes[20] = new Heroes(Heroes::TROYAN, Race::SORC);
    vec_heroes[21] = new Heroes(Heroes::VATAWNA, Race::SORC);
    vec_heroes[22] = new Heroes(Heroes::REBECCA, Race::SORC);
    vec_heroes[23] = new Heroes(Heroes::GEM, Race::SORC);
    vec_heroes[24] = new Heroes(Heroes::ARIEL, Race::SORC);
    vec_heroes[25] = new Heroes(Heroes::CARLAWN, Race::SORC);
    vec_heroes[26] = new Heroes(Heroes::LUNA, Race::SORC);

    vec_heroes[27] = new Heroes(Heroes::ARIE, Race::WRLK);
    vec_heroes[28] = new Heroes(Heroes::ALAMAR, Race::WRLK);
    vec_heroes[29] = new Heroes(Heroes::VESPER, Race::WRLK);
    vec_heroes[30] = new Heroes(Heroes::CRODO, Race::WRLK);
    vec_heroes[31] = new Heroes(Heroes::BAROK, Race::WRLK);
    vec_heroes[32] = new Heroes(Heroes::KASTORE, Race::WRLK);
    vec_heroes[33] = new Heroes(Heroes::AGAR, Race::WRLK);
    vec_heroes[34] = new Heroes(Heroes::FALAGAR, Race::WRLK);
    vec_heroes[35] = new Heroes(Heroes::WRATHMONT, Race::WRLK);

    vec_heroes[36] = new Heroes(Heroes::MYRA, Race::WZRD);
    vec_heroes[37] = new Heroes(Heroes::FLINT, Race::WZRD);
    vec_heroes[38] = new Heroes(Heroes::DAWN, Race::WZRD);
    vec_heroes[39] = new Heroes(Heroes::HALON, Race::WZRD);
    vec_heroes[40] = new Heroes(Heroes::MYRINI, Race::WZRD);
    vec_heroes[41] = new Heroes(Heroes::WILFREY, Race::WZRD);
    vec_heroes[42] = new Heroes(Heroes::SARAKIN, Race::WZRD);
    vec_heroes[43] = new Heroes(Heroes::KALINDRA, Race::WZRD);
    vec_heroes[44] = new Heroes(Heroes::MANDIGAL, Race::WZRD);

    vec_heroes[45] = new Heroes(Heroes::ZOM, Race::NECR);
    vec_heroes[46] = new Heroes(Heroes::DARLANA, Race::NECR);
    vec_heroes[47] = new Heroes(Heroes::ZAM, Race::NECR);
    vec_heroes[48] = new Heroes(Heroes::RANLOO, Race::NECR);
    vec_heroes[49] = new Heroes(Heroes::CHARITY, Race::NECR);
    vec_heroes[50] = new Heroes(Heroes::RIALDO, Race::NECR);
    vec_heroes[51] = new Heroes(Heroes::ROXANA, Race::NECR);
    vec_heroes[52] = new Heroes(Heroes::SANDRO, Race::NECR);
    vec_heroes[53] = new Heroes(Heroes::CELIA, Race::NECR);

    vec_heroes[54] = new Heroes(Heroes::ROLAND, Race::WZRD);
    vec_heroes[55] = new Heroes(Heroes::CORLAGON, Race::KNGT);
    vec_heroes[56] = new Heroes(Heroes::ELIZA, Race::SORC);
    vec_heroes[57] = new Heroes(Heroes::ARCHIBALD, Race::WRLK);
    vec_heroes[58] = new Heroes(Heroes::HALTON, Race::KNGT);
    vec_heroes[59] = new Heroes(Heroes::BAX, Race::NECR);

    if(Settings::Get().PriceLoyaltyVersion())
    {
	vec_heroes[60] = new Heroes(Heroes::SOLMYR, Race::WZRD);
	vec_heroes[61] = new Heroes(Heroes::DAINWIN, Race::WRLK);
	vec_heroes[62] = new Heroes(Heroes::MOG, Race::NECR);
	vec_heroes[63] = new Heroes(Heroes::UNCLEIVAN, Race::BARB);
	vec_heroes[64] = new Heroes(Heroes::JOSEPH, Race::KNGT);
	vec_heroes[65] = new Heroes(Heroes::GALLAVANT, Race::KNGT);
	vec_heroes[66] = new Heroes(Heroes::ELDERIAN, Race::WRLK);
	vec_heroes[67] = new Heroes(Heroes::CEALLACH, Race::KNGT);
	vec_heroes[68] = new Heroes(Heroes::DRAKONIA, Race::WZRD);
	vec_heroes[69] = new Heroes(Heroes::MARTINE, Race::SORC);
	vec_heroes[70] = new Heroes(Heroes::JARKONAS, Race::BARB);
	vec_heroes[71] = IS_DEVEL() ? new Heroes(Heroes::SANDYSANDY, Race::WRLK) : new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
    }
    else
    {
	vec_heroes[60] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[61] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[62] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[63] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[64] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[65] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[66] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[67] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[68] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[69] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[70] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[71] = IS_DEVEL() ? new Heroes(Heroes::SANDYSANDY, Race::WRLK) : new Heroes(Heroes::UNKNOWN, Race::KNGT);
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT);
    }
}

/* new maps */
void World::NewMaps(const u16 sw, const u16 sh)
{
    Reset();
    Defaults();

    width = sw;
    height = sh;

    AGG::PreloadObject(TIL::GROUND32);

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

	(*it).Set(std::distance(vec_tiles.begin(), it), mp2tile);
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
	 Error::Except("load maps");
    }

    AGG::PreloadObject(TIL::GROUND32);

    u8   byte8;
    u16  byte16;
    u32  byte32;
    std::vector<s32> vec_object; // index maps for OBJ_CASTLE, OBJ_HEROES, OBJ_SIGN, OBJ_BOTTLE, OBJ_EVENT
    vec_object.reserve(100);

    // endof
    fd.seekg(0, std::ios_base::end);
    const u32 endof_mp2 = fd.tellg();

    // read uniq
    fd.seekg(endof_mp2 - sizeof(u32), std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&uniq0), sizeof(u32));
    SwapLE32(uniq0);

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
	default: width = Maps::ZERO; break;
    }
    height = width;

    // height
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SwapLE32(byte32);

    if(byte32 != static_cast<u32>(height)) DEBUG(DBG_GAME, DBG_WARN, "incrrect maps size");

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

	tile.Set(index, mp2tile);

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
		vec_castles.push_back(new Castle(cx, cy, Race::BOMG));	break;

	    default:
		DEBUG(DBG_GAME, DBG_WARN, "castle block: " << "unknown id: " << static_cast<int>(id) << ", maps index: " << cx + cy * w());
		break;
	}
	// preload in to capture objects cache
	map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_CASTLE, Color::GRAY);
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
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_SAWMILL, Color::GRAY);
		break; 
	    // mines: mercury
	    case 0x01:
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_ALCHEMYLAB, Color::GRAY);
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
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_MINES, Color::GRAY);
		break; 
	    // lighthouse
	    case 0x64:
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_LIGHTHOUSE, Color::GRAY);
		break; 
	    // dragon city
	    case 0x65:
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_DRAGONCITY, Color::GRAY);
		break; 
	    // abandoned mines
	    case 0x67:
		map_captureobj[Maps::GetIndexFromAbsPoint(cx, cy)] = ObjectColor(MP2::OBJ_ABANDONEDMINE, Color::GRAY);
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
	//if(endof_mp2 < fd.tellg()) Error::Except("World::World: ", "read maps: out of range.");

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
	//if(endof_mp2 < fd.tellg()) Error::Except("World::World: ", "read maps: out of range.");

	// size block
	u16 sizeblock;
	fd.read(reinterpret_cast<char *>(&sizeblock), sizeof(u16));
	SwapLE16(sizeblock);

	u8 *pblock = new u8[sizeblock];

	// read block
	fd.read(reinterpret_cast<char *>(pblock), sizeblock);

	s32 findobject = -1;

	for(std::vector<s32>::const_iterator
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
			Castle *castle = GetCastle(findobject);
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			    map_captureobj[tile.GetIndex()].second = castle->GetColor();
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
			Castle *castle = GetCastle(findobject);
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::UpdateRNDSpriteForCastle(castle->GetCenter(), castle->GetRace(), castle->isCastle());
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			    map_captureobj[tile.GetIndex()].second = castle->GetColor();
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
			    hero->LoadFromMP2(findobject, pblock, Color::GRAY, hero->GetRace());
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
		    if(NULL != (addon = tile.FindObject(MP2::OBJ_HEROES)))
		    {
			// calculate color
			const u8 index_name = addon->index;
			Color::color_t color = Color::GRAY;

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
			u8 race = Race::BOMG;
			switch(index_name % 7)
			{
			    case 0: race = Race::KNGT; break;
			    case 1: race = Race::BARB; break;
			    case 2: race = Race::SORC; break;
			    case 3: race = Race::WRLK; break;
			    case 4: race = Race::WZRD; break;
			    case 5: race = Race::NECR; break;
			    case 6: race = (Color::GRAY != color ? Settings::Get().KingdomRace(color) : Race::Rand()); break;
			}

			// check heroes max count
			if(kingdom.AllowRecruitHero(false, 0))
			{
			    const Heroes* hero = NULL;

			    hero = (pblock[17] &&
				    pblock[18] < Heroes::BAX &&
				    static_cast<u8>(pblock[18]) < vec_heroes.size() &&
				    vec_heroes.at(pblock[18])->isFreeman() ?
				    vec_heroes[pblock[18]] : GetFreemanHeroes(race));

			    if(hero)
			    {
				Heroes* herow = const_cast<Heroes *>(hero);
			    	herow->LoadFromMP2(findobject, pblock, color, race);
			    	kingdom.AddHeroes(herow);
			    }
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
	const Maps::TilesAddon *addon = NULL;

	// fix loyalty version objects
	if(Settings::Get().PriceLoyaltyVersion()) tile.FixLoyaltyVersion();

	//
	switch(tile.GetObject())
	{
	    case MP2::OBJ_WITCHSHUT:
	    case MP2::OBJ_SHRINE1:
	    case MP2::OBJ_SHRINE2:
	    case MP2::OBJ_SHRINE3:
    		tile.UpdateQuantity();
		break;

	    case MP2::OBJ_STONELIGHTS:
		Maps::Tiles::UpdateStoneLightsSprite(tile);
		break;

	    case MP2::OBJ_FOUNTAIN:
		Maps::Tiles::UpdateFountainSprite(tile);
		break;

	    case MP2::OBJ_EVENT:
		// remove event sprite
		if(NULL != (addon = tile.FindObject(MP2::OBJ_EVENT)))
		{
		    tile.Remove(addon->uniq);
		}
		break;
	
    	    case MP2::OBJ_BOAT:
		// remove small sprite boat
		if(NULL != (addon = tile.FindObject(MP2::OBJ_BOAT)))
		{
		    tile.Remove(addon->uniq);
		}
		break;

    	    case MP2::OBJ_RNDULTIMATEARTIFACT:
		// remove ultimate artifact sprite
		if(NULL != (addon = tile.FindObject(MP2::OBJ_RNDULTIMATEARTIFACT)))
		{
		    Artifact artifact = Artifact::FromMP2IndexSprite(addon->index);
		    tile.SetQuantity1(artifact());
		    tile.Remove(addon->uniq);
		    tile.SetObject(MP2::OBJ_ZERO);
		    ultimate_index = ii;
		}
		break;

    	    case MP2::OBJ_RNDARTIFACT:
    	    case MP2::OBJ_RNDARTIFACT1:
    	    case MP2::OBJ_RNDARTIFACT2:
    	    case MP2::OBJ_RNDARTIFACT3:
		// modify rnd artifact sprite
		Maps::Tiles::UpdateRNDArtifactSprite(tile);
    		tile.UpdateQuantity();
		break;

	    case MP2::OBJ_RNDRESOURCE:
		// modify rnd resource sprite
		Maps::Tiles::UpdateRNDResourceSprite(tile);
		tile.UpdateQuantity();
		break;

            case MP2::OBJ_TREASURECHEST:
		if(Maps::Ground::WATER == tile.GetGround())
		    tile.SetObject(MP2::OBJ_WATERCHEST);
    		else
		    Maps::Tiles::UpdateTreasureChestSprite(tile);
    		tile.UpdateQuantity();
		break;

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
	    case MP2::OBJ_TREEKNOWLEDGE:
	    case MP2::OBJ_BARRIER:
	    case MP2::OBJ_TRAVELLERTENT:
    		tile.UpdateQuantity();
		break;

	    case MP2::OBJ_MONSTER:
	    case MP2::OBJ_RNDMONSTER:
	    case MP2::OBJ_RNDMONSTER1:
	    case MP2::OBJ_RNDMONSTER2:
	    case MP2::OBJ_RNDMONSTER3:
	    case MP2::OBJ_RNDMONSTER4:
		// modify rnd monster sprite
		Maps::Tiles::UpdateMonsterInfo(tile);
		break;

	    // join dwelling
	    case MP2::OBJ_ANCIENTLAMP:
		    tile.SetCountMonster(Monster(Monster::FromObject(tile.GetObject())).GetRNDSize(true));
		break;

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
	    // recruit dwelling
	    case MP2::OBJ_RUINS:
            case MP2::OBJ_TREECITY:
            case MP2::OBJ_WAGONCAMP:
            case MP2::OBJ_DESERTTENT:
            case MP2::OBJ_TROLLBRIDGE:
            case MP2::OBJ_DRAGONCITY:
            case MP2::OBJ_CITYDEAD:
	    case MP2::OBJ_WATERALTAR:
    	    case MP2::OBJ_AIRALTAR:
    	    case MP2::OBJ_FIREALTAR:
    	    case MP2::OBJ_EARTHALTAR:
	    case MP2::OBJ_BARROWMOUNDS:
		    tile.SetCountMonster(0);
		break;

	//
	    default:
		break;
	}
    }

    // sort castles to kingdoms
    std::vector<Castle *>::const_iterator itc1 = vec_castles.begin();
    std::vector<Castle *>::const_iterator itc2 = vec_castles.end();
    for(; itc1 != itc2; ++itc1)
	if(*itc1) GetKingdom((*itc1)->GetColor()).AddCastle(*itc1);

    // update wins, loss conditions
    if(GameOver::WINS_HERO & Settings::Get().ConditionWins())
    {
	const Heroes* hero = GetHeroes(Settings::Get().WinsMapsIndexObject());
	heroes_cond_wins = hero ? hero->GetID() : Heroes::UNKNOWN;
    }
    if(GameOver::LOSS_HERO & Settings::Get().ConditionLoss())
    {
	const Heroes* hero = GetHeroes(Settings::Get().LossMapsIndexObject());
	heroes_cond_loss = hero ? hero->GetID() : Heroes::UNKNOWN;
    }

    // play with hero
    std::for_each(vec_kingdoms.begin(), vec_kingdoms.end(), std::mem_fun(&Kingdom::ApplyPlayWithStartingHero));

    // play with debug hero
    if(IS_DEVEL())
    {
	// get first castle position
	Kingdom & kingdom = GetMyKingdom();

	if(kingdom.GetCastles().size())
	{
	    const Castle* castle = kingdom.GetCastles().front();
	    Heroes* hero = vec_heroes[Heroes::SANDYSANDY];

	    if(hero)
	    {
		const Point & cp = castle->GetCenter();
		hero->Recruit(castle->GetColor(), Point(cp.x, cp.y + 1));
	    }
	}
    }

    // generate position for ultimate
    if(-1 == ultimate_index)
    {
	std::vector<s32> pools;
	pools.reserve(vec_tiles.size());

	for(size_t ii = 0; ii < vec_tiles.size(); ++ii)
	{
	    const Maps::Tiles & tile = vec_tiles[ii];
	    const u16 x = tile.GetIndex() % width;
	    const u16 y = tile.GetIndex() / width;
	    if(Maps::Ground::WATER != tile.GetGround() &&
		tile.GoodForUltimateArtifact() &&
		x > 5 && x < width - 5 && y > 5 && y < height - 5) pools.push_back(tile.GetIndex());
	}

	if(pools.size())
	{
	    const s32 pos = *Rand::Get(pools);
	    ultimate_index = pos;
	    vec_tiles[pos].SetQuantity1(Artifact::Rand(Artifact::ART_ULTIMATE));
	}
    }

    if(Maps::isValidAbsIndex(ultimate_index))
	Interface::GameArea::GenerateUltimateArtifactAreaSurface(ultimate_index, puzzle_surface);

    DEBUG(DBG_GAME, DBG_INFO, "end load");
}

/* get human kindom */
Kingdom & World::GetMyKingdom(void)
{ return GetKingdom(Settings::Get().MyColor()); }

const Kingdom & World::GetMyKingdom(void) const
{ return GetKingdom(Settings::Get().MyColor()); }

/* get kingdom */
Kingdom & World::GetKingdom(u8 color)
{
    switch(color)
    {
        case Color::BLUE:       return *vec_kingdoms[0];
        case Color::GREEN:      return *vec_kingdoms[1];
        case Color::RED:        return *vec_kingdoms[2];
        case Color::YELLOW:     return *vec_kingdoms[3];
        case Color::ORANGE:     return *vec_kingdoms[4];
        case Color::PURPLE:     return *vec_kingdoms[5];
        case Color::GRAY:       return *vec_kingdoms[6];
    }

    DEBUG(DBG_GAME, DBG_WARN, "unknown color");

    return *vec_kingdoms[6];
}

const Kingdom & World::GetKingdom(u8 color) const
{
    switch(color)
    {
        case Color::BLUE:       return *vec_kingdoms[0];
        case Color::GREEN:      return *vec_kingdoms[1];
        case Color::RED:        return *vec_kingdoms[2];
        case Color::YELLOW:     return *vec_kingdoms[3];
        case Color::ORANGE:     return *vec_kingdoms[4];
        case Color::PURPLE:     return *vec_kingdoms[5];
        case Color::GRAY:       return *vec_kingdoms[6];
    }

    DEBUG(DBG_GAME, DBG_WARN, "unknown color");

    return *vec_kingdoms[6];
}

/* get castle from index maps */
Castle* World::GetCastle(s32 maps_index)
{
    const Point center(maps_index % width, maps_index / height);

    for(std::vector<Castle *>::iterator
	it = vec_castles.begin(); it != vec_castles.end(); ++it)
	    if((*it)->isPosition(center)) return *it;

    return NULL;
}

const Castle* World::GetCastle(s32 maps_index) const
{
    const Point center(maps_index % width, maps_index / height);

    for(std::vector<Castle *>::const_iterator
	it = vec_castles.begin(); it != vec_castles.end(); ++it)
	    if((*it)->isPosition(center)) return *it;

    return NULL;
}

Heroes* World::GetHeroes(Heroes::heroes_t id)
{
    return vec_heroes[id];
}

const Heroes* World::GetHeroes(Heroes::heroes_t id) const
{
    return vec_heroes[id];
}

/* get heroes from index maps */
Heroes* World::GetHeroes(s32 maps_index)
{
    std::vector<Heroes *>::iterator it =
	std::find_if(vec_heroes.begin(), vec_heroes.end(), 
	    std::bind2nd(std::mem_fun(&Heroes::isPosition), maps_index));

    return vec_heroes.end() != it ? *it : NULL;
}

const Heroes* World::GetHeroes(s32 maps_index) const
{
    std::vector<Heroes *>::const_iterator it =
	std::find_if(vec_heroes.begin(), vec_heroes.end(),
	    std::bind2nd(std::mem_fun(&Heroes::isPosition), maps_index));

    return vec_heroes.end() != it ? *it : NULL;
}

CastleHeroes World::GetHeroes(const Castle & castle) const
{
    std::vector<Heroes *>::const_iterator guest, guard;

    guest = std::find_if(vec_heroes.begin(), vec_heroes.end(), std::bind1st(InCastleNotGuardian(), &castle));
    guard = Settings::Get().ExtAllowCastleGuardians() ?
	std::find_if(vec_heroes.begin(), vec_heroes.end(), std::bind1st(InCastleAndGuardian(), &castle)) : vec_heroes.end();

    return CastleHeroes((guest != vec_heroes.end() ? *guest : NULL),
			    (guard != vec_heroes.end() ? *guard : NULL));
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
    for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewDay();

    // action new week
    if(BeginWeek())
    {
        NewWeek();
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewWeek();
    }
    
    // action new month
    if(BeginMonth())
    {
        NewMonth();
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewMonth();
    }
}

void World::NewWeek(void)
{
    UpdateDwellingPopulation();

    if(1 < week)
    {
	UpdateMonsterPopulation();

	// update week object
	for(MapsTiles::iterator
	    it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
	    if(MP2::isWeekLife((*it).GetObject())) (*it).UpdateQuantity();
    }

    // update week type
    week_name = BeginMonth() ? Week::MonthRand() : Week::WeekRand();

    // added army for gray castle
    std::vector<Castle *>::const_iterator itc = vec_castles.begin();
    for(; itc != vec_castles.end(); ++itc) if(*itc && Color::GRAY == (*itc)->GetColor()) (*itc)->JoinRNDArmy();

    // TODO:: action for week type: PLAGUE and MONSTERS
}

void World::NewMonth(void)
{
}

void World::Reset(void)
{
    // maps tiles
    vec_tiles.clear();

    // kingdoms
    if(vec_kingdoms.size())
    {
	std::vector<Kingdom *>::const_iterator it = vec_kingdoms.begin();
	
	for(; it != vec_kingdoms.end(); ++it) delete *it;
    }
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
    if(vec_castles.size())
    {
	std::vector<Castle *>::const_iterator it = vec_castles.begin();
	
	for(; it != vec_castles.end(); ++it) delete *it;
    }
    vec_castles.clear();
    
    // heroes
    if(vec_heroes.size())
    {
	std::vector<Heroes *>::const_iterator it = vec_heroes.begin();
	
	for(; it != vec_heroes.end(); ++it) delete *it;
    }
    vec_heroes.clear();

    // extra
    map_sign.clear();
    map_captureobj.clear();

    ultimate_index = -1;
    Surface::FreeSurface(puzzle_surface);

    day = 0;
    week = 0;
    month = 0;

    week_name = Week::TORTOISE;

    heroes_cond_wins = Heroes::UNKNOWN;
    heroes_cond_loss = Heroes::UNKNOWN;

    // reserve memory
    vec_eventsday.reserve(6);
    vec_eventsmap.reserve(6);
    vec_riddles.reserve(10);
    vec_rumors.reserve(10);
    vec_castles.reserve(MAXCASTLES);
    vec_kingdoms.reserve(KINGDOMMAX);
    vec_heroes.reserve(HEROESMAXCOUNT + 2);
}

Heroes* World::GetFreemanHeroes(u8 rc) const
{
    u8 min = 0;
    u8 max = 0;

    switch(rc)
    {
	case Race::KNGT:
	    min = 0;
	    max = 8;
	    break;
	
	case Race::BARB:
	    min = 9;
	    max = 17;
	    break;
	
	case Race::SORC:
	    min = 18;
	    max = 26;
	    break;
	    
	case Race::WRLK:
	    min = 27;
	    max = 35;
	    break;
	
	case Race::WZRD:
	    min = 36;
	    max = 44;
	    break;
	
	case Race::NECR:
	    min = 45;
	    max = 53;
	    break;
	
	default:
	    min = 0;
	    max = 53;
	    break;
    }

    std::vector<u8> freeman_heroes;
    freeman_heroes.reserve(HEROESMAXCOUNT);

    // find freeman in race
    if(Race::BOMG != rc)
	for(u8 ii = min; ii <= max; ++ii)
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back(ii);

    // not found, find other race
    if(Race::BOMG == rc || freeman_heroes.empty())
	for(u8 ii = 0; ii <= 53; ++ii)
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back(ii);

    // not found, all heroes busy
    if(freeman_heroes.empty())
    {
	DEBUG(DBG_GAME, DBG_WARN, "freeman not found, all heroes busy.");

	return NULL;
    }

    return vec_heroes.at(*Rand::Get(freeman_heroes));
}

const std::string & World::GetRumors(void)
{
    return vec_rumors[Rand::Get(vec_rumors.size() - 1)];
}

/* return random teleport destination */
s32 World::NextTeleport(const s32 index) const
{
    std::vector<s32> vec_teleports;
    
    vec_teleports.reserve(10);
    GetObjectPositions(MP2::OBJ_STONELIGHTS, vec_teleports);

    if(2 > vec_teleports.size())
    {
	DEBUG(DBG_GAME, DBG_WARN, "is empty");
	return index;
    }

    const u8 type = GetTiles(index).GetQuantity1();

    std::vector<s32> v;
    v.reserve(vec_teleports.size());

    for(std::vector<s32>::const_iterator itv = vec_teleports.begin(); itv != vec_teleports.end(); ++itv)
	if(type == GetTiles(*itv).GetQuantity1()) v.push_back(*itv);

    if(v.empty()) DEBUG(DBG_GAME , DBG_WARN, "not found");

    return v.size() ? *Rand::Get(v) : index;
}

/* return random whirlpools destination */
s32 World::NextWhirlpool(const s32 index)
{
    std::vector<s32> whilrpools;
    std::vector<s32>::const_iterator itv;

    whilrpools.reserve(40);
    GetObjectPositions(MP2::OBJ_WHIRLPOOL, whilrpools);

    std::map<s32, std::vector<s32> > uniq_whirlpools;

    for(itv = whilrpools.begin(); itv != whilrpools.end(); ++itv)
    {
    	const Maps::TilesAddon* addon = GetTiles(*itv).FindObject(MP2::OBJ_WHIRLPOOL);
	if(addon) uniq_whirlpools[addon->uniq].push_back(*itv);
    }
    whilrpools.clear();

    if(2 > uniq_whirlpools.size())
    {
	DEBUG(DBG_GAME , DBG_WARN, "is empty");
	return index;
    }

    const Maps::TilesAddon* addon = GetTiles(index).FindObject(MP2::OBJ_WHIRLPOOL);
    std::vector<u32> uniqs;
    uniqs.reserve(uniq_whirlpools.size());

    if(addon)
    {
	std::map<s32, std::vector<s32> >::const_iterator it1 = uniq_whirlpools.begin();
	std::map<s32, std::vector<s32> >::const_iterator it2 = uniq_whirlpools.end();
	for(; it1 != it2; ++it1)
	{
	    const u32 & uniq = (*it1).first;
	    if(uniq == addon->uniq) continue;
	    uniqs.push_back(uniq);
	}
    }

    std::vector<s32> & dest = uniq_whirlpools[*Rand::Get(uniqs)];
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
u16 World::CountCapturedObject(const MP2::object_t obj, const Color::color_t col) const
{
    std::map<s32, ObjectColor>::const_iterator it1 = map_captureobj.begin();
    std::map<s32, ObjectColor>::const_iterator it2 = map_captureobj.end();

    u16 result = 0;
    for(; it1 != it2; ++it1) if((*it1).second.isObject(obj) && (*it1).second.isColor(col)) ++result;

    return result;
}

/* return count captured mines */
u16 World::CountCapturedMines(const u8 res, const Color::color_t col) const
{
    u16 result = 0;

    for(std::map<s32, ObjectColor>::const_iterator
	it = map_captureobj.begin(); it != map_captureobj.end(); ++it)
	if((*it).second.isObject(MP2::OBJ_MINES) || (*it).second.isObject(MP2::OBJ_HEROES))
    {
	    // scan for find mines
	    const Maps::TilesAddon * addon = GetTiles((*it).first).FindObject(MP2::OBJ_MINES);

	    if(addon)
	    {
		// index sprite EXTRAOVR
		if(0 == addon->index && Resource::ORE == res && (*it).second.isColor(col)) ++result;
		else
		if(1 == addon->index && Resource::SULFUR == res && (*it).second.isColor(col)) ++result;
		else
		if(2 == addon->index && Resource::CRYSTAL == res && (*it).second.isColor(col)) ++result;
		else
		if(3 == addon->index && Resource::GEMS == res && (*it).second.isColor(col)) ++result;
		else
		if(4 == addon->index && Resource::GOLD == res && (*it).second.isColor(col)) ++result;
	    }
    }
    
    return result;
}

/* capture object */
void World::CaptureObject(const s32 index, const Color::color_t col)
{
    MP2::object_t obj = GetTiles(index).GetObject();
    
    if(MP2::OBJ_HEROES == obj)
    {
	const Heroes * hero = GetHeroes(index);
	if(NULL == hero) return;

	obj = hero->GetUnderObject();
    }

    map_captureobj[index].first = obj;
    map_captureobj[index].second = col;

    if(MP2::OBJ_CASTLE == obj)
    {
	Castle *castle = GetCastle(index);
	if(castle) castle->ChangeColor(col);
    }

    GetTiles(index).CaptureFlags32(obj, col);
}

/* return color captured object */
Color::color_t World::ColorCapturedObject(const s32 index) const
{
    std::map<s32, ObjectColor>::const_iterator it1 = map_captureobj.begin();
    std::map<s32, ObjectColor>::const_iterator it2 = map_captureobj.end();

    for(; it1 != it2; ++it1) if((*it1).first == index) return (*it1).second.second;

    return Color::GRAY;
}

void World::ClearFog(const u8 color)
{
    // clear abroad castles
    if(vec_castles.size())
    {
        std::vector<Castle *>::const_iterator it1 = vec_castles.begin();
        std::vector<Castle *>::const_iterator it2 = vec_castles.end();

        for(; it1 != it2; ++it1) if(*it1 && color & (**it1).GetColor()) (**it1).Scoute();
    }

    // clear abroad heroes
    if(vec_heroes.size())
    {
        std::vector<Heroes *>::const_iterator it1 = vec_heroes.begin();
        std::vector<Heroes *>::const_iterator it2 = vec_heroes.end();

        for(; it1 != it2; ++it1) if(*it1 && color & (**it1).GetColor()) (**it1).Scoute();
    }

    // clear abroad objects
    std::map<s32, ObjectColor>::const_iterator it1 = map_captureobj.begin();
    std::map<s32, ObjectColor>::const_iterator it2 = map_captureobj.end();

    for(; it1 != it2; ++it1)
	if(color & (*it1).second.second)
    {
	u8 scoute = 0;

	switch((*it1).second.first)
	{
	    case MP2::OBJ_MINES:
	    case MP2::OBJ_ALCHEMYLAB:
	    case MP2::OBJ_SAWMILL:	scoute = 2; break;

	    case MP2::OBJ_LIGHTHOUSE:	scoute = 4; break; // FIXME: scoute and lighthouse

	    default: break;
	}

        if(scoute) Maps::ClearFog((*it1).first, scoute, color);
    }
}

/* update population monster in dwelling */
void World::UpdateDwellingPopulation(void)
{
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
	Maps::Tiles & tile = *it;
	MP2::object_t obj = tile.GetObject();
	float count = 0;

	if(obj == MP2::OBJ_HEROES)
	{
	    const Heroes* hero = world.GetHeroes(tile.GetIndex());
	    if(hero) obj = hero->GetUnderObject();
	}

	switch(obj)
	{
    	    // join monsters
            case MP2::OBJ_HALFLINGHOLE:
            case MP2::OBJ_PEASANTHUT:
            case MP2::OBJ_THATCHEDHUT:
            case MP2::OBJ_EXCAVATION:
            case MP2::OBJ_CAVE:
            case MP2::OBJ_TREEHOUSE:
            case MP2::OBJ_GOBLINHUT:
	    {
		const Monster m(Monster::FromObject(obj));
		count = m.GetRNDSize(true) * 3 / 2;
		break;
	    }

            case MP2::OBJ_TREECITY:
	    {
		const Monster m(Monster::FromObject(obj));
		count = 2 * m.GetRNDSize(true);
		break;
	    }
	    
    	    case MP2::OBJ_WATCHTOWER:
            case MP2::OBJ_ARCHERHOUSE:
            case MP2::OBJ_DWARFCOTT:
	    //
	    case MP2::OBJ_RUINS:
            case MP2::OBJ_WAGONCAMP:
            case MP2::OBJ_DESERTTENT:
            case MP2::OBJ_WATERALTAR:
            case MP2::OBJ_AIRALTAR:
            case MP2::OBJ_FIREALTAR:
            case MP2::OBJ_EARTHALTAR:
	    case MP2::OBJ_BARROWMOUNDS:
	    {
		const Monster m(Monster::FromObject(obj));
		count = m.GetRNDSize(true);
		break;
	    }

            case MP2::OBJ_TROLLBRIDGE:
            case MP2::OBJ_CITYDEAD:
	    {
		const Monster m(Monster::FromObject(obj));
		count = m.GetRNDSize(true);
		break;
	    }

            case MP2::OBJ_DRAGONCITY:
        	count = 1;
        	break;

	    default: break;
	}

	if(week == 1)
	    count = count * 3 / 2;
	else
        // check guardians beaten
	switch(obj)
	{
            case MP2::OBJ_TROLLBRIDGE:
            case MP2::OBJ_CITYDEAD:
            case MP2::OBJ_DRAGONCITY:
		if(0 == tile.GetQuantity4()) count = 0;
		break;

	    default: break;
	}
		
	if(count) tile.SetCountMonster(tile.GetCountMonster() + static_cast<u16>(count));
    }
}

void World::UpdateMonsterPopulation(void)
{
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    if(MP2::OBJ_MONSTER == (*it).GetObject())
    {
	Maps::Tiles & tile = *it;
	const Army::Troop troop(tile);

	if(0 == troop.GetCount())
	    tile.SetCountMonster(troop.GetRNDSize(false));
	else
	if(! tile.FixedCountMonster())
	    tile.SetCountMonster(troop.GetCount() * 8 / 7);
    }
}

Artifact World::GetUltimateArtifact(void) const
{
    return Maps::isValidAbsIndex(ultimate_index) ?
	Artifact(vec_tiles[ultimate_index].GetQuantity1()) : Artifact(Artifact::UNKNOWN);
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

    return ultimate_index == tile.GetIndex();
}

void World::ActionForMagellanMaps(u8 color)
{
    for(MapsTiles::iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
	if(Maps::Ground::WATER == (*it).GetGround()) (*it).ClearFog(color);
}

s32 World::GetNearestObject(s32 center, MP2::object_t obj, bool check_hero) const
{
    std::vector<IndexDistance> resv;
    resv.reserve(10);

    if(Maps::isValidAbsIndex(center) &&
	GetObjectPositions(center, obj, resv, check_hero))
    {
	std::sort(resv.begin(), resv.end(), IndexDistance::Shortest);
	return resv.front().first;
    }

    return -1;
}

EventsDate World::GetEventsDate(const Color::color_t c) const
{
    EventsDate res;
    res.reserve(vec_eventsday.size());

    for(EventsDate::const_iterator
	it = vec_eventsday.begin(); it != vec_eventsday.end(); ++it)
	if((*it).isAllow(c, day)) res.push_back(*it);

    return res;
}

EventMaps* World::GetEventMaps(const Color::color_t c, const s32 index)
{
    if(vec_eventsmap.size())
    {
	for(EventsMaps::iterator
	    it = vec_eventsmap.begin(); it != vec_eventsmap.end(); ++it)
	    if((*it).isAllow(c, index)) return &(*it);
    }

    return NULL;
}

void World::DateDump(void) const
{
    VERBOSE("World::Date: month: " << static_cast<int>(GetMonth()) <<
	    ", week " << static_cast<int>(GetWeek()) << ", day: " << static_cast<int>(GetDay()));
}

bool IsObeliskOnMaps(const Maps::Tiles & tile)
{
    switch(tile.GetObject())
    {
	case MP2::OBJ_OBELISK:	return true;
	case MP2::OBJ_HEROES:
	{
	    const Heroes* hero = world.GetHeroes(tile.GetIndex());
	    if(hero && MP2::OBJ_OBELISK == hero->GetUnderObject()) return true;
	}
	default: break;
    }
    return false;
}

u16 World::CountObeliskOnMaps(void)
{
    u16 res = std::count_if(vec_tiles.begin(), vec_tiles.end(), IsObeliskOnMaps);
    return res ? res : 6;
}

void World::KingdomLoss(const Color::color_t color)
{
    // castles
    std::vector<Castle *>::iterator itc1 = vec_castles.begin();
    std::vector<Castle *>::const_iterator itc2 = vec_castles.end();
    for(; itc1 != itc2; ++itc1) if(*itc1 && (*itc1)->GetColor() == color) (*itc1)->ChangeColor(color);

    // capture object
    std::map<s32, ObjectColor>::iterator it1 = map_captureobj.begin();
    std::map<s32, ObjectColor>::const_iterator it2 = map_captureobj.end();
    for(; it1 != it2; ++it1)
    {
	ObjectColor & pair = (*it1).second;
	if(pair.isColor(color))
	{
	    pair.second = Color::GRAY;
	    GetTiles((*it1).first).CaptureFlags32(pair.first, Color::GRAY);
	}
    }
}

Heroes* World::FromJail(s32 index)
{
    std::vector<Heroes *>::iterator ith1 = vec_heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = vec_heroes.end();
    for(; ith1 != ith2; ++ith1) if(*ith1 && (*ith1)->Modes(Heroes::JAIL) && index == (*ith1)->GetIndex()) return *ith1;

    return NULL;
}

void World::ActionToEyeMagi(const Color::color_t color) const
{
    std::vector<s32> vec_eyes;
    vec_eyes.reserve(10);
    GetObjectPositions(MP2::OBJ_EYEMAGI, vec_eyes, true);

    if(vec_eyes.size())
    {
	std::vector<s32>::const_iterator it1 = vec_eyes.begin();
	std::vector<s32>::const_iterator it2 = vec_eyes.end();

	for(; it1 != it2; ++it1) Maps::ClearFog(*it1, Game::GetViewDistance(Game::VIEW_MAGI_EYES), color);
    }
}

Riddle* World::GetSphinx(const s32 index)
{
    Riddles::iterator
	it = std::find(vec_riddles.begin(), vec_riddles.end(), index);
    return it != vec_riddles.end() ? &(*it) : NULL;
}

bool World::GetObjectPositions(s32 center, MP2::object_t obj, std::vector<IndexDistance> & v, bool check_hero) const
{
    if(v.size()) v.clear();

    for(MapsTiles::const_iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
	MP2::object_t obj2 = check_hero && MP2::OBJ_HEROES == (*it).GetObject() && GetHeroes((*it).GetIndex()) ?
		GetHeroes((*it).GetIndex())->GetUnderObject() : (*it).GetObject();

	if(obj == obj2)
		v.push_back(IndexDistance((*it).GetIndex(),
			Maps::GetApproximateDistance(center, (*it).GetIndex())));
    }

    return v.size();
}

bool World::GetObjectPositions(MP2::object_t obj, std::vector<s32> & v, bool check_hero) const
{
    if(v.size()) v.clear();

    for(MapsTiles::const_iterator
	it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
	MP2::object_t obj2 = check_hero && MP2::OBJ_HEROES == (*it).GetObject() && GetHeroes((*it).GetIndex()) ?
		GetHeroes((*it).GetIndex())->GetUnderObject() : (*it).GetObject();

	if(obj == obj2)
		v.push_back((*it).GetIndex());
    }

    return v.size();
}

void World::UpdateRecruits(Recruits & recruits) const
{
    if(2 < std::count_if(vec_heroes.begin(), vec_heroes.end(), PredicateHeroesIsFreeman))
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
        
bool World::CheckKingdomNormalVictory(const Kingdom & kingdom) const
{
    return !kingdom.isLoss() &&
           1 == std::count_if(&vec_kingdoms[0], &vec_kingdoms[6], std::not1(std::mem_fun(&Kingdom::isLoss)));
}

bool WorldCheckKingdomLossWins(const Kingdom* kingdom)
{
    if(kingdom)
    switch(world.CheckKingdomWins(*kingdom))
    {
	case GameOver::WINS_ALL:
        case GameOver::WINS_TOWN:
        case GameOver::WINS_HERO:
        case GameOver::COND_NONE:
    	    return false;

        default:
    	    return true;
    }

    return false;
}

u16 World::CheckKingdomWins(const Kingdom & kingdom) const
{
    const Settings & conf = Settings::Get();

    if((conf.ConditionWins() & GameOver::WINS_ALL) && CheckKingdomNormalVictory(kingdom))
	return GameOver::WINS_ALL;
    else
    if(conf.ConditionWins() & GameOver::WINS_TOWN)
    {
	const Castle *town = GetCastle(conf.WinsMapsIndexObject());

	// check comp also wins
	if(Game::AI == kingdom.Control() && !conf.WinsCompAlsoWins())
	    return GameOver::COND_NONE;
	else
        if(town && town->GetColor() == kingdom.GetColor())
	    return GameOver::WINS_TOWN;
    }
    else
    if(conf.ConditionWins() & GameOver::WINS_HERO)
    {
        const Heroes *hero = GetHeroesCondWins();
        if(hero && Heroes::UNKNOWN != heroes_cond_wins &&
    	    hero->isFreeman() &&
    	    hero->GetKillerColor() == kingdom.GetColor())
        	return GameOver::WINS_HERO;
    }
    else
    if(conf.ConditionWins() & GameOver::WINS_ARTIFACT)
    {
	const std::vector<Heroes *> & heroes  = kingdom.GetHeroes();
	if(conf.WinsFindUltimateArtifact())
	{
	    if(heroes.end() != std::find_if(heroes.begin(), heroes.end(),
		std::mem_fun(&Heroes::HasUltimateArtifact)))
		return GameOver::WINS_ARTIFACT;
	}
	else
	{
	    const Artifact art = conf.WinsFindArtifactID();
	    if(heroes.end() != std::find_if(heroes.begin(), heroes.end(),
		std::bind2nd(HeroHasArtifact(), art)))
		return GameOver::WINS_ARTIFACT;
	}
    }
    else
    if(conf.ConditionWins() & GameOver::WINS_SIDE)
    {
	u8 side1 = conf.GetUnions(kingdom.GetColor());
	u8 side2 = conf.KingdomColors() & ~side1;
	u8 loss  = 0;

	for(std::vector<Kingdom *>::const_iterator
	    it = vec_kingdoms.begin(); it != vec_kingdoms.end(); ++it)
    	    if(*it && (*it)->isLoss()) loss |= (*it)->GetColor();

	if(side2 == (loss & side2))
	    return GameOver::WINS_SIDE;
    }
    else
    if(conf.ConditionWins() & GameOver::WINS_GOLD)
    {
	// check comp also wins
    	if(Game::AI == kingdom.Control() && !conf.WinsCompAlsoWins())
    	    return GameOver::COND_NONE;
	else
	if(kingdom.GetFundsGold() >= conf.WinsAccumulateGold())
	    return GameOver::WINS_GOLD;
    }
    
    return GameOver::COND_NONE;
}

u16 World::CheckKingdomLoss(const Kingdom & kingdom) const
{
    const Settings & conf = Settings::Get();

    // if other wins return false (skip for alianse)
    if(!(conf.ConditionWins() & GameOver::WINS_SIDE))
    {
	std::vector<Kingdom *>::const_iterator it = std::find_if(vec_kingdoms.begin(), vec_kingdoms.end(), WorldCheckKingdomLossWins);
	if(vec_kingdoms.end() != it && *it && (*it)->GetColor() != kingdom.GetColor())
	    return CheckKingdomWins(**it);
    }

    if(conf.ConditionLoss() & GameOver::LOSS_ALL)
    {
	if(kingdom.isLoss())
    	    return GameOver::LOSS_ALL;
    }
    else
    if(conf.ConditionLoss() & GameOver::LOSS_TOWN)
    {
        const Castle *town = GetCastle(conf.LossMapsIndexObject());
        if(town && town->GetColor() != kingdom.GetColor())
    	    return GameOver::LOSS_TOWN;
    }
    else
    if(conf.ConditionLoss() & GameOver::LOSS_HERO)
    {
        const Heroes *hero = GetHeroesCondLoss();
        if(hero && Heroes::UNKNOWN != heroes_cond_loss &&
    	    hero->isFreeman() &&
    	    hero->GetKillerColor() != kingdom.GetColor())
        	return GameOver::LOSS_HERO;
    }
    else
    if(conf.ConditionLoss() & GameOver::LOSS_TIME)
    {
        if(CountDay() > conf.LossCountDays())
    	    return GameOver::LOSS_TIME;
    }

    return GameOver::COND_NONE;
}

const Surface & World::GetPuzzleSurface(void) const
{
    return puzzle_surface;
}
