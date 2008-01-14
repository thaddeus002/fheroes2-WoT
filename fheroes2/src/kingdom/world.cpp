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
#include <fstream> 
#include "error.h" 
#include "agg.h" 
#include "artifact.h"
#include "resource.h"
#include "config.h" 
#include "kingdom.h" 
#include "heroes.h" 
#include "castle.h" 
#include "gameevent.h" 
#include "mp2.h" 
#include "text.h" 
#include "rand.h" 
#include "world.h"

World & World::GetWorld(void)
{
    static World insideWorld;

    return insideWorld;
}

/* new maps */
void World::NewMaps(Maps::mapsize_t w, Maps::mapsize_t h)
{
    FreeOldMaps();

    width = w;
    height = h;

    ultimate_artifact = 0xFFFF;

    day = 0;
    week = 0;
    month = 0;

    begin_week = true;
    begin_month = true;

    free_recrut_hero1 = Heroes::UNKNOWN;
    free_recrut_hero2 = Heroes::UNKNOWN;

    // playing kingdom
    H2Config::SetKingdomColors(Color::BLUE | Color::GREEN | Color::RED | Color::YELLOW | Color::ORANGE | Color::PURPLE);
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) vec_kingdoms.push_back(new Kingdom(color));

    // initialize all heroes
    vec_heroes.push_back(new Heroes(Heroes::LORDKILBURN, Race::KNGT, "Lord Kilburn"));
    vec_heroes.push_back(new Heroes(Heroes::SIRGALLANTH, Race::KNGT, "Sir Gallanth"));
    vec_heroes.push_back(new Heroes(Heroes::ECTOR, Race::KNGT, "Ector"));
    vec_heroes.push_back(new Heroes(Heroes::GVENNETH, Race::KNGT, "Gvenneth"));
    vec_heroes.push_back(new Heroes(Heroes::TYRO, Race::KNGT, "Tyro"));
    vec_heroes.push_back(new Heroes(Heroes::AMBROSE, Race::KNGT, "Ambrose"));
    vec_heroes.push_back(new Heroes(Heroes::RUBY, Race::KNGT, "Ruby"));
    vec_heroes.push_back(new Heroes(Heroes::MAXIMUS, Race::KNGT, "Maximus"));
    vec_heroes.push_back(new Heroes(Heroes::DIMITRY, Race::KNGT, "Dimitry"));

    vec_heroes.push_back(new Heroes(Heroes::THUNDAX, Race::BARB, "Thundax"));
    vec_heroes.push_back(new Heroes(Heroes::FINEOUS, Race::BARB, "Fineous"));
    vec_heroes.push_back(new Heroes(Heroes::JOJOSH, Race::BARB, "Jojosh"));
    vec_heroes.push_back(new Heroes(Heroes::CRAGHACK, Race::BARB, "Crag Hack"));
    vec_heroes.push_back(new Heroes(Heroes::JEZEBEL, Race::BARB, "Jezebel"));
    vec_heroes.push_back(new Heroes(Heroes::JACLYN, Race::BARB, "Jaclyn"));
    vec_heroes.push_back(new Heroes(Heroes::ERGON, Race::BARB, "Ergon"));
    vec_heroes.push_back(new Heroes(Heroes::TSABU, Race::BARB, "Tsabu"));
    vec_heroes.push_back(new Heroes(Heroes::ATLAS, Race::BARB, "Atlas"));

    vec_heroes.push_back(new Heroes(Heroes::ASTRA, Race::SORC, "Astra"));
    vec_heroes.push_back(new Heroes(Heroes::NATASHA, Race::SORC, "Natasha"));
    vec_heroes.push_back(new Heroes(Heroes::TROYAN, Race::SORC, "Troyan"));
    vec_heroes.push_back(new Heroes(Heroes::VATAWNA, Race::SORC, "Vatawna"));
    vec_heroes.push_back(new Heroes(Heroes::REBECCA, Race::SORC, "Rebecca"));
    vec_heroes.push_back(new Heroes(Heroes::GEM, Race::SORC, "Gem"));
    vec_heroes.push_back(new Heroes(Heroes::ARIEL, Race::SORC, "Ariel"));
    vec_heroes.push_back(new Heroes(Heroes::CARLAWN, Race::SORC, "Carlawn"));
    vec_heroes.push_back(new Heroes(Heroes::LUNA, Race::SORC, "Luna"));

    vec_heroes.push_back(new Heroes(Heroes::ARIE, Race::WRLK, "Arie"));
    vec_heroes.push_back(new Heroes(Heroes::ALAMAR, Race::WRLK, "Alamar"));
    vec_heroes.push_back(new Heroes(Heroes::VESPER, Race::WRLK, "Vesper"));
    vec_heroes.push_back(new Heroes(Heroes::CRODO, Race::WRLK, "Crodo"));
    vec_heroes.push_back(new Heroes(Heroes::BAROK, Race::WRLK, "Barok"));
    vec_heroes.push_back(new Heroes(Heroes::KASTORE, Race::WRLK, "Kastore"));
    vec_heroes.push_back(new Heroes(Heroes::AGAR, Race::WRLK, "Agar"));
    vec_heroes.push_back(new Heroes(Heroes::FALAGAR, Race::WRLK, "Falagar"));
    vec_heroes.push_back(new Heroes(Heroes::WRATHMONT, Race::WRLK, "Wrathmont"));

    vec_heroes.push_back(new Heroes(Heroes::MYRA, Race::WZRD, "Myra"));
    vec_heroes.push_back(new Heroes(Heroes::FLINT, Race::WZRD, "Flint"));
    vec_heroes.push_back(new Heroes(Heroes::DAWN, Race::WZRD, "Dawn"));
    vec_heroes.push_back(new Heroes(Heroes::HALON, Race::WZRD, "Halon"));
    vec_heroes.push_back(new Heroes(Heroes::MYRINI, Race::WZRD, "Myrini"));
    vec_heroes.push_back(new Heroes(Heroes::WILFREY, Race::WZRD, "Wilfrey"));
    vec_heroes.push_back(new Heroes(Heroes::SARAKIN, Race::WZRD, "Sarakin"));
    vec_heroes.push_back(new Heroes(Heroes::KALINDRA, Race::WZRD, "Kalindra"));
    vec_heroes.push_back(new Heroes(Heroes::MANDIGAL, Race::WZRD, "Mandigal"));

    vec_heroes.push_back(new Heroes(Heroes::ZOM, Race::NECR, "Zom"));
    vec_heroes.push_back(new Heroes(Heroes::DARLANA, Race::NECR, "Darlana"));
    vec_heroes.push_back(new Heroes(Heroes::ZAM, Race::NECR, "Zam"));
    vec_heroes.push_back(new Heroes(Heroes::RANLOO, Race::NECR, "Ranloo"));
    vec_heroes.push_back(new Heroes(Heroes::CHARITY, Race::NECR, "Charity"));
    vec_heroes.push_back(new Heroes(Heroes::RIALDO, Race::NECR, "Rialdo"));
    vec_heroes.push_back(new Heroes(Heroes::ROXANA, Race::NECR, "Roxana"));
    vec_heroes.push_back(new Heroes(Heroes::SANDRO, Race::NECR, "Sandro"));
    vec_heroes.push_back(new Heroes(Heroes::CELIA, Race::NECR, "Celia"));

    vec_heroes.push_back(new Heroes(Heroes::ROLAND, Race::WZRD, "Roland"));
    vec_heroes.push_back(new Heroes(Heroes::CORLAGON, Race::KNGT, "Lord Corlagon"));
    vec_heroes.push_back(new Heroes(Heroes::ELIZA, Race::SORC, "Sister Eliza"));
    vec_heroes.push_back(new Heroes(Heroes::ARCHIBALD, Race::WRLK, "Archibald"));
    vec_heroes.push_back(new Heroes(Heroes::HALTON, Race::KNGT, "Lord Halton"));
    vec_heroes.push_back(new Heroes(Heroes::BAX, Race::NECR, "Brother Bax"));
    vec_heroes.push_back(new Heroes(Heroes::SANDYSANDY, Race::WRLK, "SandySandy"));
    vec_heroes.push_back(new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown"));

    // loading info
    display.Fill(0, 0, 0);
    TextBox("Maps Loading...", Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    AGG::PreloadObject("GROUND32.TIL");

    // init all tiles
    for(u16 ii = 0; ii < width * height; ++ii)
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

	vec_tiles.push_back(new Maps::Tiles(ii, mp2tile));
    }
}

/* load maps */
void World::LoadMaps(const std::string &filename)
{
    FreeOldMaps();

    ultimate_artifact = 0xFFFF;

    day = 0;
    week = 0;
    month = 0;

    begin_week = true;
    begin_month = true;

    free_recrut_hero1 = Heroes::UNKNOWN;
    free_recrut_hero2 = Heroes::UNKNOWN;

    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    // playing kingdom
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) vec_kingdoms.push_back(new Kingdom(color));

    // initialize all heroes
    vec_heroes.push_back(new Heroes(Heroes::LORDKILBURN, Race::KNGT, "Lord Kilburn"));
    vec_heroes.push_back(new Heroes(Heroes::SIRGALLANTH, Race::KNGT, "Sir Gallanth"));
    vec_heroes.push_back(new Heroes(Heroes::ECTOR, Race::KNGT, "Ector"));
    vec_heroes.push_back(new Heroes(Heroes::GVENNETH, Race::KNGT, "Gvenneth"));
    vec_heroes.push_back(new Heroes(Heroes::TYRO, Race::KNGT, "Tyro"));
    vec_heroes.push_back(new Heroes(Heroes::AMBROSE, Race::KNGT, "Ambrose"));
    vec_heroes.push_back(new Heroes(Heroes::RUBY, Race::KNGT, "Ruby"));
    vec_heroes.push_back(new Heroes(Heroes::MAXIMUS, Race::KNGT, "Maximus"));
    vec_heroes.push_back(new Heroes(Heroes::DIMITRY, Race::KNGT, "Dimitry"));

    vec_heroes.push_back(new Heroes(Heroes::THUNDAX, Race::BARB, "Thundax"));
    vec_heroes.push_back(new Heroes(Heroes::FINEOUS, Race::BARB, "Fineous"));
    vec_heroes.push_back(new Heroes(Heroes::JOJOSH, Race::BARB, "Jojosh"));
    vec_heroes.push_back(new Heroes(Heroes::CRAGHACK, Race::BARB, "Crag Hack"));
    vec_heroes.push_back(new Heroes(Heroes::JEZEBEL, Race::BARB, "Jezebel"));
    vec_heroes.push_back(new Heroes(Heroes::JACLYN, Race::BARB, "Jaclyn"));
    vec_heroes.push_back(new Heroes(Heroes::ERGON, Race::BARB, "Ergon"));
    vec_heroes.push_back(new Heroes(Heroes::TSABU, Race::BARB, "Tsabu"));
    vec_heroes.push_back(new Heroes(Heroes::ATLAS, Race::BARB, "Atlas"));

    vec_heroes.push_back(new Heroes(Heroes::ASTRA, Race::SORC, "Astra"));
    vec_heroes.push_back(new Heroes(Heroes::NATASHA, Race::SORC, "Natasha"));
    vec_heroes.push_back(new Heroes(Heroes::TROYAN, Race::SORC, "Troyan"));
    vec_heroes.push_back(new Heroes(Heroes::VATAWNA, Race::SORC, "Vatawna"));
    vec_heroes.push_back(new Heroes(Heroes::REBECCA, Race::SORC, "Rebecca"));
    vec_heroes.push_back(new Heroes(Heroes::GEM, Race::SORC, "Gem"));
    vec_heroes.push_back(new Heroes(Heroes::ARIEL, Race::SORC, "Ariel"));
    vec_heroes.push_back(new Heroes(Heroes::CARLAWN, Race::SORC, "Carlawn"));
    vec_heroes.push_back(new Heroes(Heroes::LUNA, Race::SORC, "Luna"));

    vec_heroes.push_back(new Heroes(Heroes::ARIE, Race::WRLK, "Arie"));
    vec_heroes.push_back(new Heroes(Heroes::ALAMAR, Race::WRLK, "Alamar"));
    vec_heroes.push_back(new Heroes(Heroes::VESPER, Race::WRLK, "Vesper"));
    vec_heroes.push_back(new Heroes(Heroes::CRODO, Race::WRLK, "Crodo"));
    vec_heroes.push_back(new Heroes(Heroes::BAROK, Race::WRLK, "Barok"));
    vec_heroes.push_back(new Heroes(Heroes::KASTORE, Race::WRLK, "Kastore"));
    vec_heroes.push_back(new Heroes(Heroes::AGAR, Race::WRLK, "Agar"));
    vec_heroes.push_back(new Heroes(Heroes::FALAGAR, Race::WRLK, "Falagar"));
    vec_heroes.push_back(new Heroes(Heroes::WRATHMONT, Race::WRLK, "Wrathmont"));

    vec_heroes.push_back(new Heroes(Heroes::MYRA, Race::WZRD, "Myra"));
    vec_heroes.push_back(new Heroes(Heroes::FLINT, Race::WZRD, "Flint"));
    vec_heroes.push_back(new Heroes(Heroes::DAWN, Race::WZRD, "Dawn"));
    vec_heroes.push_back(new Heroes(Heroes::HALON, Race::WZRD, "Halon"));
    vec_heroes.push_back(new Heroes(Heroes::MYRINI, Race::WZRD, "Myrini"));
    vec_heroes.push_back(new Heroes(Heroes::WILFREY, Race::WZRD, "Wilfrey"));
    vec_heroes.push_back(new Heroes(Heroes::SARAKIN, Race::WZRD, "Sarakin"));
    vec_heroes.push_back(new Heroes(Heroes::KALINDRA, Race::WZRD, "Kalindra"));
    vec_heroes.push_back(new Heroes(Heroes::MANDIGAL, Race::WZRD, "Mandigal"));

    vec_heroes.push_back(new Heroes(Heroes::ZOM, Race::NECR, "Zom"));
    vec_heroes.push_back(new Heroes(Heroes::DARLANA, Race::NECR, "Darlana"));
    vec_heroes.push_back(new Heroes(Heroes::ZAM, Race::NECR, "Zam"));
    vec_heroes.push_back(new Heroes(Heroes::RANLOO, Race::NECR, "Ranloo"));
    vec_heroes.push_back(new Heroes(Heroes::CHARITY, Race::NECR, "Charity"));
    vec_heroes.push_back(new Heroes(Heroes::RIALDO, Race::NECR, "Rialdo"));
    vec_heroes.push_back(new Heroes(Heroes::ROXANA, Race::NECR, "Roxana"));
    vec_heroes.push_back(new Heroes(Heroes::SANDRO, Race::NECR, "Sandro"));
    vec_heroes.push_back(new Heroes(Heroes::CELIA, Race::NECR, "Celia"));

    vec_heroes.push_back(new Heroes(Heroes::ROLAND, Race::WZRD, "Roland"));
    vec_heroes.push_back(new Heroes(Heroes::CORLAGON, Race::KNGT, "Lord Corlagon"));
    vec_heroes.push_back(new Heroes(Heroes::ELIZA, Race::SORC, "Sister Eliza"));
    vec_heroes.push_back(new Heroes(Heroes::ARCHIBALD, Race::WRLK, "Archibald"));
    vec_heroes.push_back(new Heroes(Heroes::HALTON, Race::KNGT, "Lord Halton"));
    vec_heroes.push_back(new Heroes(Heroes::BAX, Race::NECR, "Brother Bax"));
    vec_heroes.push_back(new Heroes(Heroes::SANDYSANDY, Race::WRLK, "SandySandy"));
    vec_heroes.push_back(new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown"));

    // loading info
    display.Fill(0, 0, 0);
    TextBox("Maps Loading...", Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    AGG::PreloadObject("GROUND32.TIL");

    char byte8;
    u16  byte16;
    u32  byte32;
    std::vector<u16> vec_object; // index maps for OBJ_CASTLE, OBJ_HEROES, OBJ_SIGN, OBJ_BOTTLE, OBJ_EVENT

    // endof
    fd.seekg(0, std::ios_base::end);
    const u32 endof_mp2 = fd.tellg();

    // offset data
    fd.seekg(MP2OFFSETDATA - 2 * sizeof(u32), std::ios_base::beg);

    // width
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);

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
    SWAP32(byte32);

    if(byte32 != static_cast<u32>(height)) Error::Warning("World::World: maps size mismatch!");

    // seek to ADDONS block
    fd.ignore(width * height * SIZEOFMP2TILE);

    // count mp2addon_t
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(u32));
    SWAP32(byte32);

    // read all addons
    std::vector<MP2::mp2addon_t> vec_mp2addons;

    for(unsigned int ii = 0; ii < byte32; ++ii)
    {
	MP2::mp2addon_t mp2addon;

	fd.read(reinterpret_cast<char *>(&mp2addon.indexAddon), sizeof(u16));
	SWAP16(mp2addon.indexAddon);

	fd.read(&byte8, 1);
	mp2addon.objectNameN1 = byte8 * 2;

	fd.read(&byte8, 1);
	mp2addon.indexNameN1 = byte8;

	fd.read(&byte8, 1);
	mp2addon.quantityN = byte8;

	fd.read(&byte8, 1);
	mp2addon.objectNameN2 = byte8;

	fd.read(&byte8, 1);
	mp2addon.indexNameN2 = byte8;

	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN1), sizeof(u32));
	SWAP32(mp2addon.uniqNumberN1);

	fd.read(reinterpret_cast<char *>(&mp2addon.uniqNumberN2), sizeof(u32));
	SWAP32(mp2addon.uniqNumberN2);

	vec_mp2addons.push_back(mp2addon);
    }

    const u32 endof_addons = fd.tellg();

    if(H2Config::Debug()) Error::Verbose("World::World: read all tiles addons, tellg: ", endof_addons);

    // offset data
    fd.seekg(MP2OFFSETDATA, std::ios_base::beg);

    // read all tiles
    for(u16 ii = 0; ii < width * height; ++ii)
    {
	MP2::mp2tile_t mp2tile;

	// byte16
	fd.read(reinterpret_cast<char *>(&mp2tile.tileIndex), sizeof(u16));
	SWAP16(mp2tile.tileIndex);

	fd.read(&byte8, 1);
	mp2tile.objectName1 = byte8;

	fd.read(&byte8, 1);
	mp2tile.indexName1 = byte8;

	fd.read(&byte8, 1);
	mp2tile.quantity1 = byte8;

	fd.read(&byte8, 1);
	mp2tile.quantity2 = byte8;

	fd.read(&byte8, 1);
	mp2tile.objectName2 = byte8;

	fd.read(&byte8, 1);
	mp2tile.indexName2 = byte8;

	fd.read(&byte8, 1);
	mp2tile.shape = byte8;

	fd.read(&byte8, 1);
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
		vec_object.push_back(ii);
		break;
	    default:
		break;
	}

	// offset first addon
	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);

	// byte32
	fd.read(reinterpret_cast<char *>(&mp2tile.uniqNumber1), sizeof(u32));
	SWAP32(mp2tile.uniqNumber1);

	// byte32
	fd.read(reinterpret_cast<char *>(&mp2tile.uniqNumber2), sizeof(u32));
	SWAP32(mp2tile.uniqNumber2);

	Maps::Tiles * tile = new Maps::Tiles(ii, mp2tile);

	// load all addon for current tils
	while(byte16){

	    if(vec_mp2addons.size() <= byte16){ Error::Warning("World::World: index addons out of range!"); break; }

	    (*tile).AddonsPushLevel1(vec_mp2addons.at(byte16));
	    (*tile).AddonsPushLevel2(vec_mp2addons.at(byte16));

	    byte16 = vec_mp2addons.at(byte16).indexAddon;
	}

	(*tile).AddonsSort();

	vec_tiles.push_back(tile);
    }

    if(H2Config::Debug()) Error::Verbose("World::World: read all tiles, tellg: ", fd.tellg());

    // after addons
    fd.seekg(endof_addons, std::ios_base::beg);

    // cood castles
    // 72 x 3 byte (cx, cy, id)
    for(u8 ii = 0; ii < 72; ++ii)
    {
	char cx, cy, id;

	fd.read(&cx, 1);
	fd.read(&cy, 1);
	fd.read(&id, 1);
	
	// empty block
	if(-1 == cx && -1 == cy) continue;

	switch(id){
	    case 0x00: break; // tower: knight
	    case 0x01: break; // tower: barbarian
	    case 0x02: break; // tower: sorceress
	    case 0x03: break; // tower: warlock
	    case 0x04: break; // tower: wizard
	    case 0x05: break; // tower: necromancer
	    case 0x06: break; // tower: random
	    case 0x80: break; // castle: knight
	    case 0x81: break; // castle: barbarian
	    case 0x82: break; // castle: sorceress
	    case 0x83: break; // castle: warlock
	    case 0x84: break; // castle: wizard
	    case 0x85: break; // castle: necromancer
	    case 0x86: break; // castle: random
	    default:
		Error::Warning("World::World: castle block, unknown id: ", id);
		if(H2Config::Debug()) Error::Verbose("maps index: ", cx + cy * w());
		break;
	}
    }

    if(H2Config::Debug()) Error::Verbose("World::World: read coord castles, tellg: ", fd.tellg());

    // cood resource kingdoms
    // 144 x 3 byte (cx, cy, id)
    for(u8 ii = 0; ii < 144; ++ii)
    {
	char cx, cy, id;

	fd.read(&cx, 1);
	fd.read(&cy, 1);
	fd.read(&id, 1);
	
	// empty block
	if(-1 == cx && -1 == cy) continue;

	switch(id){
	    case 0x00: break; // mines: wood
	    case 0x01: break; // mines: mercury
 	    case 0x02: break; // mines: ore
	    case 0x03: break; // mines: sulfur
	    case 0x04: break; // mines: crystal
	    case 0x05: break; // mines: gems
	    case 0x06: break; // mines: gold
	    case 0x64: break; // lighthouse
	    case 0x65: break; // dragon city
	    case 0x67: break; // abandoned mines
	    default:
		Error::Warning("World::World: kingdom block, unknown id: ", id);
		if(H2Config::Debug()) Error::Verbose("maps index: ", cx + cy * w());
		break;	
	}
    }

    if(H2Config::Debug()) Error::Verbose("World::World: read coord other resource, tellg: ", fd.tellg());

    // unknown byte
    fd.ignore();

    // count final mp2 blocks
    u16 countblock = 0;
    byte16 = 0xffff;
    while(byte16)
    {
	// debug endof mp2
	if(endof_mp2 < fd.tellg()) Error::Except("World::World: read maps: out of range.");

	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);

	if(byte16) countblock = byte16 - 1;
    }

    if(H2Config::Debug()) Error::Verbose("World::World: read find final mp2 blocks, tellg: ", fd.tellg());

    // castle or heroes or (events, rumors, etc)
    for(u16 ii = 0; ii < countblock; ++ii)
    {
	// debug
	if(endof_mp2 < fd.tellg()) Error::Except("World::World: read maps: out of range.");

	// size block
	u16 sizeblock;
	fd.read(reinterpret_cast<char *>(&sizeblock), sizeof(u16));
	SWAP16(sizeblock);

	char *pblock = new char[sizeblock];

	// read block
	fd.read(pblock, sizeblock);

	std::vector<u16>::const_iterator it_index = vec_object.begin();
	bool findobject = false;

	while(it_index != vec_object.end())
	{
	    const Maps::Tiles & tile = *vec_tiles.at(*it_index);

	    // orders(quantity2, quantity1)
	    u16 orders = (tile.GetQuantity2() ? tile.GetQuantity2() : 0);
	    orders <<= 8;
	    orders |= static_cast<u16>(tile.GetQuantity1());
	    
	    if(orders && !(orders % 0x08) && (ii + 1 == orders / 0x08)){ findobject = true; break; }

	    ++it_index;
	}

	if(findobject)
	{
	    const Maps::Tiles & tile = *vec_tiles.at(*it_index);
	    const Maps::TilesAddon *addon = NULL;

	    switch(tile.GetObject())
	    {
		case MP2::OBJ_CASTLE:
		    // add castle
		    if(SIZEOFMP2CASTLE != sizeblock) Error::Warning("World::World: read castle: incorrect size block.");
		    else
		    vec_castles.push_back(new Castle(tile.GetUniq1(), *it_index, pblock));
		    break;
		case MP2::OBJ_RNDTOWN:
		case MP2::OBJ_RNDCASTLE:
		    // add rnd castle
		    if(SIZEOFMP2CASTLE != sizeblock) Error::Warning("World::World: read castle: incorrect size block.");
		    else
		    vec_castles.push_back(new Castle(tile.GetUniq1(), *it_index, pblock, true));
		    break;
		case MP2::OBJ_HEROES:
		    // add heroes
		    if(SIZEOFMP2HEROES != sizeblock) Error::Warning("World::World: read heroes: incorrect size block.");
		    else
		    if((addon = tile.FindAddon(0x54)) || (addon = tile.FindAddon(0x55)) ||
		       (addon = tile.FindAddon(0x56)) || (addon = tile.FindAddon(0x57)))
		    {
			// calculate color
			const u8 index_name = (*addon).GetIndex();
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

			const Kingdom & kingdom = GetKingdom(color);

			// caclulate race
			Race::race_t race = Race::BOMG;
			switch(index_name % 7)
			{
			    case 0: race = Race::KNGT; break;
			    case 1: race = Race::BARB; break;
			    case 2: race = Race::SORC; break;
			    case 3: race = Race::WRLK; break;
			    case 4: race = Race::WZRD; break;
			    case 5: race = Race::NECR; break;
			    case 6: race = (Color::GRAY != color ? H2Config::GetKingdomRace(color) : Race::Rand()); break;
			}

			// check heroes max count
			if(kingdom.GetHeroes().size() < KINGDOMMAXHEROES)
			{
			    const Heroes * hero = NULL;

			    hero = (pblock[17] &&
				    pblock[18] < Heroes::BAX &&
				    static_cast<u8>(pblock[18]) < vec_heroes.size() &&
				    vec_heroes.at(pblock[18])->isFreeman() ?
				    vec_heroes[pblock[18]] : GetFreemanHeroes(race));

			    if(hero)
			    {
				Heroes * herow = const_cast<Heroes *>(hero);
				Kingdom & kingdomw = const_cast<Kingdom &>(kingdom);
			    	(*herow).LoadFromMP2(*it_index, pblock, color);
			    	kingdomw.AddHeroes(herow);
			    }
			}
			else
			    Error::Warning("World::LoadMaps: load heroes maximum");
		    }
		    break;
		case MP2::OBJ_SIGN:
		case MP2::OBJ_BOTTLE:
		    // add sign or buttle
		    if(SIZEOFMP2SIGN - 1 < sizeblock && 0x01 == pblock[0])
				    vec_signs.push_back(new GameEvent::Sign(*it_index, &pblock[9]));
		    break;
		case MP2::OBJ_EVENT:
		    // add event maps
		    if(SIZEOFMP2EVENT - 1 < sizeblock && 0x01 == pblock[0])
				    vec_eventsmap.push_back(new GameEvent::Coord(*it_index, pblock));
		    break;
		case MP2::OBJ_SPHINX:
		    // add riddle sphinx
		    if(SIZEOFMP2RIDDLE - 1 < sizeblock && 0x00 == pblock[0])
				    vec_riddles.push_back(new GameEvent::Riddle(*it_index, pblock));
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
		vec_eventsday.push_back(new GameEvent::Day(pblock));

	    // add rumors
	    else if(SIZEOFMP2RUMOR - 1 < sizeblock)
	    {
		if(pblock[8])
		{
		    vec_rumors.push_back(new GameEvent::Rumor(&pblock[8]));
		    if(H2Config::Debug()) Error::Verbose("add Rumors: " + (*vec_rumors.back()).GetString());
		}
	    }
	}
	// debug
	else Error::Warning("World::World: read maps: unknown block addons, size: ", sizeblock);

	delete [] pblock;
    }

    // last rumors
    vec_rumors.push_back(new GameEvent::Rumor("This game is now in pre alpha development version. ;)"));

    // close mp2
    fd.close();

    // modify other objects
    const u16 vec_size = vec_tiles.size();

    for(u16 ii = 0; ii < vec_size; ++ii)
    {
	Maps::Tiles & tile = *vec_tiles[ii];
	const Maps::TilesAddon *addon = NULL;

	switch(tile.GetObject())
	{
	    case MP2::OBJ_STONELIGHTS:
		vec_stonelights.push_back(ii);
		break;

	    case MP2::OBJ_EVENT:
		// remove event sprite
		if( (addon = tile.FindAddon(0xA4, 0xA3)) || (addon = tile.FindAddon(0xA5, 0xA3)) ||
		    (addon = tile.FindAddon(0xA6, 0xA3)) || (addon = tile.FindAddon(0xA7, 0xA3)))
		{
		    tile.Remove((*addon).GetUniq());
		}
		break;
	
    	    case MP2::OBJ_RNDARTIFACT:
    	    case MP2::OBJ_RNDARTIFACT1:
    	    case MP2::OBJ_RNDARTIFACT2:
    	    case MP2::OBJ_RNDARTIFACT3:
		// modify rnd artifact sprite
		Artifact::ChangeTileWithRNDArtifact(vec_tiles, ii);
		break;

    	    case MP2::OBJ_RNDULTIMATEARTIFACT:
		// remove ultimate artifact sprite
		if( (addon = tile.FindAddon(0x2C, 0xA4)) || (addon = tile.FindAddon(0x2D, 0xA4)) ||
		    (addon = tile.FindAddon(0x2E, 0xA4)) || (addon = tile.FindAddon(0x2F, 0xA4)))
		{
		    tile.Remove((*addon).GetUniq());
		    tile.SetObject(MP2::OBJ_ZERO);
		    ultimate_artifact = ii;
		}
		break;

	    case MP2::OBJ_RESOURCE:
		    //tile.SetQuantity1();
		break;

	    case MP2::OBJ_RNDRESOURCE:
		// modify rnd resource sprite
		Resource::ChangeTileWithRNDResource(vec_tiles, ii);
		break;

	    case MP2::OBJ_RNDMONSTER:
	    case MP2::OBJ_RNDMONSTER1:
	    case MP2::OBJ_RNDMONSTER2:
	    case MP2::OBJ_RNDMONSTER3:
	    case MP2::OBJ_RNDMONSTER4:
		// modify rnd monster sprite
		Monster::ChangeTileWithRNDMonster(vec_tiles, ii);
		break;

	    default:
		break;
	}
    }

    AGG::FreeObject("GROUND32.TIL");

    // sort castles to kingdoms
    for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii)
	if((*vec_kingdoms[ii]).isPlay()) for(u16 cc = 0; cc < vec_castles.size(); ++cc)
	    if((*vec_kingdoms[ii]).GetColor() == (*vec_castles[cc]).GetColor())
		(*vec_kingdoms[ii]).AddCastle(vec_castles[cc]);

    // play with hero
    if(H2Config::PlayWithHeroes())
	for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii)
	    if((*vec_kingdoms[ii]).isPlay() && (*vec_kingdoms[ii]).GetCastles().size())
	    {
		// get first castle position
		const Kingdom & kingdom = *(vec_kingdoms[ii]);
		const Castle & castle = *(kingdom.GetCastles().at(0));

		// place debugger hero
		if(H2Config::Debug() && H2Config::GetMyColor() == kingdom.GetColor() && (*vec_heroes[Heroes::SANDYSANDY]).isFreeman())
		{
		    const Heroes *hero = vec_heroes[Heroes::SANDYSANDY];

		    const_cast<Heroes &>(*hero).Recrut(castle);

		    const_cast<Kingdom &>(kingdom).AddHeroes(const_cast<Heroes *>(hero));
		}
		else
		// place near hero
		if(const Heroes *hero = GetFreemanHeroes(castle.GetRace()))
		{
		    const_cast<Heroes &>(*hero).Recrut(castle);

		    const_cast<Kingdom &>(kingdom).AddHeroes(const_cast<Heroes *>(hero));
		}
	    }

    if(H2Config::Debug()) Error::Verbose("World::LoadMaps: end load.");
}

/* get human kindom */
const Kingdom & World::GetMyKingdom(void) const
{ return GetKingdom(H2Config::GetMyColor()); }

/* get kingdom */
const Kingdom & World::GetKingdom(Color::color_t color) const
{
    switch(color)
    {
        case Color::BLUE:       return *vec_kingdoms[0]; break;
        case Color::GREEN:      return *vec_kingdoms[1]; break;
        case Color::RED:        return *vec_kingdoms[2]; break;
        case Color::YELLOW:     return *vec_kingdoms[3]; break;
        case Color::ORANGE:     return *vec_kingdoms[4]; break;
        case Color::PURPLE:     return *vec_kingdoms[5]; break;
        case Color::GRAY:       break;
    }

    Error::Warning("World::GetKingdom: return Color::GRAY.");

    return *vec_kingdoms[0];
}

/* get castle from index maps */
const Castle * World::GetCastle(u16 maps_index)
{
    return GetCastle(maps_index % width, maps_index / height);
}

/* get castle from coord maps */
const Castle * World::GetCastle(u8 ax, u8 ay)
{
    std::vector<Castle *>::const_iterator itc = vec_castles.begin();

    for(; itc != vec_castles.end(); ++itc)
    {
        const Point & pt = (**itc).GetCenter();

        if((pt.x == ax && pt.y - 3 == ay) ||
           (ax >= pt.x - 2 && ax <= pt.x + 2 && ay >= pt.y - 2 && ay <= pt.y + 1)) return *itc;
    }

    Error::Warning("World::GetCastle: return NULL pointer");

    return NULL;
}

/* get heroes from index maps */
const Heroes * World::GetHeroes(u16 maps_index)
{
    return GetHeroes(maps_index % width, maps_index / height);
}

/* get heroes from coord maps */
const Heroes * World::GetHeroes(u8 ax, u8 ay)
{
    std::vector<Heroes *>::const_iterator ith = vec_heroes.begin();

    for(; ith != vec_heroes.end(); ++ith)
    {
        const Point & pt = (**ith).GetCenter();

        if(pt.x == ax && pt.y == ay) return *ith;
    }

    //dinamic object
    //Error::Warning("World::GetHeroes: return NULL pointer");

    return NULL;
}

/* end day */
void World::NextDay(void)
{
    ++day;

    if(!(day % DAYOFWEEK))
    {
        ++week;
    
        if(!(week % WEEKOFMONTH)) ++month;
    }
    
    begin_week = (day % DAYOFWEEK ? false : true);
            
    begin_month = (begin_week && !(week % WEEKOFMONTH) ? true : false);

    // action new day
    for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewDay();

    // action new week
    if(begin_week)
    {
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewWeek();
        
        NewWeek();
    }
    
    // action new month
    if(begin_month)
    {
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewMonth();
        
        NewMonth();
    }
}

void World::NewWeek(void)
{
    // change free recrut hero
    const Race::race_t & rc = H2Config::GetKingdomRace(H2Config::GetMyColor());
    free_recrut_hero1 = GetFreemanHeroes(rc)->GetHeroes();
    free_recrut_hero2 = GetFreemanHeroes()->GetHeroes();
}

void World::NewMonth(void)
{
}

void World::FreeOldMaps(void)
{
    // maps tiles
    if(vec_tiles.size())
    {
	std::vector<Maps::Tiles *>::const_iterator it = vec_tiles.begin();
	
	for(; it != vec_tiles.end(); ++it) delete *it;
    }
    vec_tiles.clear();

    // kingdoms
    if(vec_kingdoms.size())
    {
	std::vector<Kingdom *>::const_iterator it = vec_kingdoms.begin();
	
	for(; it != vec_kingdoms.end(); ++it) delete *it;
    }
    vec_kingdoms.clear();

    // signs
    if(vec_signs.size())
    {
	std::vector<GameEvent::Sign *>::const_iterator it = vec_signs.begin();
	
	for(; it != vec_signs.end(); ++it) delete *it;
    }
    vec_signs.clear();

    // event day
    if(vec_eventsday.size())
    {
	std::vector<GameEvent::Day *>::const_iterator it = vec_eventsday.begin();
	
	for(; it != vec_eventsday.end(); ++it) delete *it;
    }
    vec_eventsday.clear();

    // event maps
    if(vec_eventsmap.size())
    {
	std::vector<GameEvent::Coord *>::const_iterator it = vec_eventsmap.begin();
	
	for(; it != vec_eventsmap.end(); ++it) delete *it;
    }
    vec_eventsmap.clear();

    // riddle
    if(vec_riddles.size())
    {
	std::vector<GameEvent::Riddle *>::const_iterator it = vec_riddles.begin();
	
	for(; it != vec_riddles.end(); ++it) delete *it;
    }
    vec_riddles.clear();

    // rumors
    if(vec_rumors.size())
    {
	std::vector<GameEvent::Rumor *>::const_iterator it = vec_rumors.begin();

	for(; it != vec_rumors.end(); ++it) delete *it;
    }
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
}

const Heroes::heroes_t & World::GetFreeRecrut1(void)
{
    const Race::race_t & rc = H2Config::GetKingdomRace(H2Config::GetMyColor());

    if(Heroes::UNKNOWN == free_recrut_hero1 || !(*vec_heroes[free_recrut_hero1]).isFreeman()) free_recrut_hero1 = GetFreemanHeroes(rc)->GetHeroes();

    return free_recrut_hero1;
}

const Heroes::heroes_t & World::GetFreeRecrut2(void)
{
    if(Heroes::UNKNOWN == free_recrut_hero2 || !(*vec_heroes[free_recrut_hero2]).isFreeman()) free_recrut_hero2 = GetFreemanHeroes()->GetHeroes();

    while(free_recrut_hero1 == free_recrut_hero2)
    {
	Error::Verbose("World::GetFreeRecrut2: hero1 equal hero2");

	free_recrut_hero2 = GetFreemanHeroes()->GetHeroes();
    }

    return free_recrut_hero2;
}

const Heroes * World::GetFreemanHeroes(Race::race_t rc)
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

    std::vector<Heroes::heroes_t> freeman_heroes;
    
    // find freeman in race
    if(Race::BOMG != rc)
	for(u8 ii = min; ii <= max; ++ii)
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back((*vec_heroes[ii]).GetHeroes());

    // not found, find other race
    if(Race::BOMG == rc || freeman_heroes.empty())
	for(u8 ii = 0; ii <= 53; ++ii)
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back((*vec_heroes[ii]).GetHeroes());

    // not found, all heroes busy
    if(freeman_heroes.empty())
    {
	Error::Warning("World::GetFreemanHeroes: freeman not found, all heroes busy.");

	return NULL;
    }

    return vec_heroes[ freeman_heroes[Rand::Get(freeman_heroes.size() - 1)] ];
}

const std::string & World::GetRumors(void)
{
    //vec_rumors.size();

    return (*vec_rumors[Rand::Get(vec_rumors.size() - 1)]).GetString();
}
