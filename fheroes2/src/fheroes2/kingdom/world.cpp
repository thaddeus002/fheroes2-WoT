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
#include "SDL.h"
#include "engine.h" 
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
#include "algorithm.h" 
#include "world.h"
#include "army.h"
#include "spell.h"

World & world = World::GetWorld();

u32 World::uniq0 = 0;

World & World::GetWorld(void)
{
    static World insideWorld;

    return insideWorld;
}

/* new maps */
void World::NewMaps(const u16 sw, const u16 sh)
{
    FreeOldMaps();

    width = sw;
    height = sh;

    ultimate_artifact = 0xFFFF;

    day = 0;
    week = 0;
    month = 0;

    begin_week = true;
    begin_month = true;

    free_recruit_hero1 = Heroes::UNKNOWN;
    free_recruit_hero2 = Heroes::UNKNOWN;

    week_name = Week::TORTOISE;

    // reserve memory
    vec_eventsday.reserve(6);
    vec_eventsmap.reserve(6);
    vec_riddles.reserve(10);
    vec_rumors.reserve(10);
    vec_castles.reserve(MAXCASTLES);
    vec_teleports.reserve(10);

    // playing kingdom
    Settings::Get().FileInfo().SetKingdomColors(Color::BLUE | Color::GREEN | Color::RED | Color::YELLOW | Color::ORANGE | Color::PURPLE);
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
    vec_heroes[0] = new Heroes(Heroes::LORDKILBURN, Race::KNGT, "Lord Kilburn");
    vec_heroes[1] = new Heroes(Heroes::SIRGALLANTH, Race::KNGT, "Sir Gallanth");
    vec_heroes[2] = new Heroes(Heroes::ECTOR, Race::KNGT, "Ector");
    vec_heroes[3] = new Heroes(Heroes::GVENNETH, Race::KNGT, "Gvenneth");
    vec_heroes[4] = new Heroes(Heroes::TYRO, Race::KNGT, "Tyro");
    vec_heroes[5] = new Heroes(Heroes::AMBROSE, Race::KNGT, "Ambrose");
    vec_heroes[6] = new Heroes(Heroes::RUBY, Race::KNGT, "Ruby");
    vec_heroes[7] = new Heroes(Heroes::MAXIMUS, Race::KNGT, "Maximus");
    vec_heroes[8] = new Heroes(Heroes::DIMITRY, Race::KNGT, "Dimitry");

    vec_heroes[9] = new Heroes(Heroes::THUNDAX, Race::BARB, "Thundax");
    vec_heroes[10] = new Heroes(Heroes::FINEOUS, Race::BARB, "Fineous");
    vec_heroes[11] = new Heroes(Heroes::JOJOSH, Race::BARB, "Jojosh");
    vec_heroes[12] = new Heroes(Heroes::CRAGHACK, Race::BARB, "Crag Hack");
    vec_heroes[13] = new Heroes(Heroes::JEZEBEL, Race::BARB, "Jezebel");
    vec_heroes[14] = new Heroes(Heroes::JACLYN, Race::BARB, "Jaclyn");
    vec_heroes[15] = new Heroes(Heroes::ERGON, Race::BARB, "Ergon");
    vec_heroes[16] = new Heroes(Heroes::TSABU, Race::BARB, "Tsabu");
    vec_heroes[17] = new Heroes(Heroes::ATLAS, Race::BARB, "Atlas");

    vec_heroes[18] = new Heroes(Heroes::ASTRA, Race::SORC, "Astra");
    vec_heroes[19] = new Heroes(Heroes::NATASHA, Race::SORC, "Natasha");
    vec_heroes[20] = new Heroes(Heroes::TROYAN, Race::SORC, "Troyan");
    vec_heroes[21] = new Heroes(Heroes::VATAWNA, Race::SORC, "Vatawna");
    vec_heroes[22] = new Heroes(Heroes::REBECCA, Race::SORC, "Rebecca");
    vec_heroes[23] = new Heroes(Heroes::GEM, Race::SORC, "Gem");
    vec_heroes[24] = new Heroes(Heroes::ARIEL, Race::SORC, "Ariel");
    vec_heroes[25] = new Heroes(Heroes::CARLAWN, Race::SORC, "Carlawn");
    vec_heroes[26] = new Heroes(Heroes::LUNA, Race::SORC, "Luna");

    vec_heroes[27] = new Heroes(Heroes::ARIE, Race::WRLK, "Arie");
    vec_heroes[28] = new Heroes(Heroes::ALAMAR, Race::WRLK, "Alamar");
    vec_heroes[29] = new Heroes(Heroes::VESPER, Race::WRLK, "Vesper");
    vec_heroes[30] = new Heroes(Heroes::CRODO, Race::WRLK, "Crodo");
    vec_heroes[31] = new Heroes(Heroes::BAROK, Race::WRLK, "Barok");
    vec_heroes[32] = new Heroes(Heroes::KASTORE, Race::WRLK, "Kastore");
    vec_heroes[33] = new Heroes(Heroes::AGAR, Race::WRLK, "Agar");
    vec_heroes[34] = new Heroes(Heroes::FALAGAR, Race::WRLK, "Falagar");
    vec_heroes[35] = new Heroes(Heroes::WRATHMONT, Race::WRLK, "Wrathmont");

    vec_heroes[36] = new Heroes(Heroes::MYRA, Race::WZRD, "Myra");
    vec_heroes[37] = new Heroes(Heroes::FLINT, Race::WZRD, "Flint");
    vec_heroes[38] = new Heroes(Heroes::DAWN, Race::WZRD, "Dawn");
    vec_heroes[39] = new Heroes(Heroes::HALON, Race::WZRD, "Halon");
    vec_heroes[40] = new Heroes(Heroes::MYRINI, Race::WZRD, "Myrini");
    vec_heroes[41] = new Heroes(Heroes::WILFREY, Race::WZRD, "Wilfrey");
    vec_heroes[42] = new Heroes(Heroes::SARAKIN, Race::WZRD, "Sarakin");
    vec_heroes[43] = new Heroes(Heroes::KALINDRA, Race::WZRD, "Kalindra");
    vec_heroes[44] = new Heroes(Heroes::MANDIGAL, Race::WZRD, "Mandigal");

    vec_heroes[45] = new Heroes(Heroes::ZOM, Race::NECR, "Zom");
    vec_heroes[46] = new Heroes(Heroes::DARLANA, Race::NECR, "Darlana");
    vec_heroes[47] = new Heroes(Heroes::ZAM, Race::NECR, "Zam");
    vec_heroes[48] = new Heroes(Heroes::RANLOO, Race::NECR, "Ranloo");
    vec_heroes[49] = new Heroes(Heroes::CHARITY, Race::NECR, "Charity");
    vec_heroes[50] = new Heroes(Heroes::RIALDO, Race::NECR, "Rialdo");
    vec_heroes[51] = new Heroes(Heroes::ROXANA, Race::NECR, "Roxana");
    vec_heroes[52] = new Heroes(Heroes::SANDRO, Race::NECR, "Sandro");
    vec_heroes[53] = new Heroes(Heroes::CELIA, Race::NECR, "Celia");

    vec_heroes[54] = new Heroes(Heroes::ROLAND, Race::WZRD, "Roland");
    vec_heroes[55] = new Heroes(Heroes::CORLAGON, Race::KNGT, "Lord Corlagon");
    vec_heroes[56] = new Heroes(Heroes::ELIZA, Race::SORC, "Sister Eliza");
    vec_heroes[57] = new Heroes(Heroes::ARCHIBALD, Race::WRLK, "Archibald");
    vec_heroes[58] = new Heroes(Heroes::HALTON, Race::KNGT, "Lord Halton");
    vec_heroes[59] = new Heroes(Heroes::BAX, Race::NECR, "Brother Bax");

    if(Settings::Get().Modes(Settings::PRICELOYALTY))
    {
	vec_heroes[60] = new Heroes(Heroes::SOLMYR, Race::WZRD, "Solmyr");
	vec_heroes[61] = new Heroes(Heroes::DAINWIN, Race::WRLK, "Dainwin");
	vec_heroes[62] = new Heroes(Heroes::MOG, Race::NECR, "Mog");
	vec_heroes[63] = new Heroes(Heroes::UNCLEIVAN, Race::BARB, "Uncle Ivan");
	vec_heroes[64] = new Heroes(Heroes::JOSEPH, Race::KNGT, "Joseph");
	vec_heroes[65] = new Heroes(Heroes::GALLAVANT, Race::KNGT, "Gallavant");
	vec_heroes[66] = new Heroes(Heroes::ELDERIAN, Race::WRLK, "Elderian");
	vec_heroes[67] = new Heroes(Heroes::CEALLACH, Race::KNGT, "Ceallach");
	vec_heroes[68] = new Heroes(Heroes::DRAKONIA, Race::WZRD, "Drakonia");
	vec_heroes[69] = new Heroes(Heroes::MARTINE, Race::SORC, "Martine");
	vec_heroes[70] = new Heroes(Heroes::JARKONAS, Race::BARB, "Jarkonas");
	vec_heroes[71] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
    }
    else
    {
	vec_heroes[60] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[61] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[62] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[63] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[64] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[65] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[66] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[67] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[68] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[69] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[70] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[71] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
    }

    Display & display = Display::Get();

    // loading info
    display.Fill(0, 0, 0);
    TextBox("Maps Loading...", Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    AGG::PreloadObject(TIL::GROUND32);

    vec_tiles.resize(width * height);

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

	vec_tiles[ii] = new Maps::Tiles(ii, mp2tile);
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

    free_recruit_hero1 = Heroes::UNKNOWN;
    free_recruit_hero2 = Heroes::UNKNOWN;

    week_name = Week::TORTOISE;

    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()) Error::Except("LoadMP2: " + filename + ", file not found.");

    // reserve memory
    vec_eventsday.reserve(6);
    vec_eventsmap.reserve(6);
    vec_riddles.reserve(10);
    vec_rumors.reserve(10);
    vec_castles.reserve(MAXCASTLES);
    vec_teleports.reserve(10);

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
    vec_heroes[0] = new Heroes(Heroes::LORDKILBURN, Race::KNGT, "Lord Kilburn");
    vec_heroes[1] = new Heroes(Heroes::SIRGALLANTH, Race::KNGT, "Sir Gallanth");
    vec_heroes[2] = new Heroes(Heroes::ECTOR, Race::KNGT, "Ector");
    vec_heroes[3] = new Heroes(Heroes::GVENNETH, Race::KNGT, "Gvenneth");
    vec_heroes[4] = new Heroes(Heroes::TYRO, Race::KNGT, "Tyro");
    vec_heroes[5] = new Heroes(Heroes::AMBROSE, Race::KNGT, "Ambrose");
    vec_heroes[6] = new Heroes(Heroes::RUBY, Race::KNGT, "Ruby");
    vec_heroes[7] = new Heroes(Heroes::MAXIMUS, Race::KNGT, "Maximus");
    vec_heroes[8] = new Heroes(Heroes::DIMITRY, Race::KNGT, "Dimitry");

    vec_heroes[9] = new Heroes(Heroes::THUNDAX, Race::BARB, "Thundax");
    vec_heroes[10] = new Heroes(Heroes::FINEOUS, Race::BARB, "Fineous");
    vec_heroes[11] = new Heroes(Heroes::JOJOSH, Race::BARB, "Jojosh");
    vec_heroes[12] = new Heroes(Heroes::CRAGHACK, Race::BARB, "Crag Hack");
    vec_heroes[13] = new Heroes(Heroes::JEZEBEL, Race::BARB, "Jezebel");
    vec_heroes[14] = new Heroes(Heroes::JACLYN, Race::BARB, "Jaclyn");
    vec_heroes[15] = new Heroes(Heroes::ERGON, Race::BARB, "Ergon");
    vec_heroes[16] = new Heroes(Heroes::TSABU, Race::BARB, "Tsabu");
    vec_heroes[17] = new Heroes(Heroes::ATLAS, Race::BARB, "Atlas");

    vec_heroes[18] = new Heroes(Heroes::ASTRA, Race::SORC, "Astra");
    vec_heroes[19] = new Heroes(Heroes::NATASHA, Race::SORC, "Natasha");
    vec_heroes[20] = new Heroes(Heroes::TROYAN, Race::SORC, "Troyan");
    vec_heroes[21] = new Heroes(Heroes::VATAWNA, Race::SORC, "Vatawna");
    vec_heroes[22] = new Heroes(Heroes::REBECCA, Race::SORC, "Rebecca");
    vec_heroes[23] = new Heroes(Heroes::GEM, Race::SORC, "Gem");
    vec_heroes[24] = new Heroes(Heroes::ARIEL, Race::SORC, "Ariel");
    vec_heroes[25] = new Heroes(Heroes::CARLAWN, Race::SORC, "Carlawn");
    vec_heroes[26] = new Heroes(Heroes::LUNA, Race::SORC, "Luna");

    vec_heroes[27] = new Heroes(Heroes::ARIE, Race::WRLK, "Arie");
    vec_heroes[28] = new Heroes(Heroes::ALAMAR, Race::WRLK, "Alamar");
    vec_heroes[29] = new Heroes(Heroes::VESPER, Race::WRLK, "Vesper");
    vec_heroes[30] = new Heroes(Heroes::CRODO, Race::WRLK, "Crodo");
    vec_heroes[31] = new Heroes(Heroes::BAROK, Race::WRLK, "Barok");
    vec_heroes[32] = new Heroes(Heroes::KASTORE, Race::WRLK, "Kastore");
    vec_heroes[33] = new Heroes(Heroes::AGAR, Race::WRLK, "Agar");
    vec_heroes[34] = new Heroes(Heroes::FALAGAR, Race::WRLK, "Falagar");
    vec_heroes[35] = new Heroes(Heroes::WRATHMONT, Race::WRLK, "Wrathmont");

    vec_heroes[36] = new Heroes(Heroes::MYRA, Race::WZRD, "Myra");
    vec_heroes[37] = new Heroes(Heroes::FLINT, Race::WZRD, "Flint");
    vec_heroes[38] = new Heroes(Heroes::DAWN, Race::WZRD, "Dawn");
    vec_heroes[39] = new Heroes(Heroes::HALON, Race::WZRD, "Halon");
    vec_heroes[40] = new Heroes(Heroes::MYRINI, Race::WZRD, "Myrini");
    vec_heroes[41] = new Heroes(Heroes::WILFREY, Race::WZRD, "Wilfrey");
    vec_heroes[42] = new Heroes(Heroes::SARAKIN, Race::WZRD, "Sarakin");
    vec_heroes[43] = new Heroes(Heroes::KALINDRA, Race::WZRD, "Kalindra");
    vec_heroes[44] = new Heroes(Heroes::MANDIGAL, Race::WZRD, "Mandigal");

    vec_heroes[45] = new Heroes(Heroes::ZOM, Race::NECR, "Zom");
    vec_heroes[46] = new Heroes(Heroes::DARLANA, Race::NECR, "Darlana");
    vec_heroes[47] = new Heroes(Heroes::ZAM, Race::NECR, "Zam");
    vec_heroes[48] = new Heroes(Heroes::RANLOO, Race::NECR, "Ranloo");
    vec_heroes[49] = new Heroes(Heroes::CHARITY, Race::NECR, "Charity");
    vec_heroes[50] = new Heroes(Heroes::RIALDO, Race::NECR, "Rialdo");
    vec_heroes[51] = new Heroes(Heroes::ROXANA, Race::NECR, "Roxana");
    vec_heroes[52] = new Heroes(Heroes::SANDRO, Race::NECR, "Sandro");
    vec_heroes[53] = new Heroes(Heroes::CELIA, Race::NECR, "Celia");

    vec_heroes[54] = new Heroes(Heroes::ROLAND, Race::WZRD, "Roland");
    vec_heroes[55] = new Heroes(Heroes::CORLAGON, Race::KNGT, "Lord Corlagon");
    vec_heroes[56] = new Heroes(Heroes::ELIZA, Race::SORC, "Sister Eliza");
    vec_heroes[57] = new Heroes(Heroes::ARCHIBALD, Race::WRLK, "Archibald");
    vec_heroes[58] = new Heroes(Heroes::HALTON, Race::KNGT, "Lord Halton");
    vec_heroes[59] = new Heroes(Heroes::BAX, Race::NECR, "Brother Bax");

    if(Settings::Get().Modes(Settings::PRICELOYALTY))
    {
	vec_heroes[60] = new Heroes(Heroes::SOLMYR, Race::WZRD, "Solmyr");
	vec_heroes[61] = new Heroes(Heroes::DAINWIN, Race::WRLK, "Dainwin");
	vec_heroes[62] = new Heroes(Heroes::MOG, Race::NECR, "Mog");
	vec_heroes[63] = new Heroes(Heroes::UNCLEIVAN, Race::BARB, "Uncle Ivan");
	vec_heroes[64] = new Heroes(Heroes::JOSEPH, Race::KNGT, "Joseph");
	vec_heroes[65] = new Heroes(Heroes::GALLAVANT, Race::KNGT, "Gallavant");
	vec_heroes[66] = new Heroes(Heroes::ELDERIAN, Race::WRLK, "Elderian");
	vec_heroes[67] = new Heroes(Heroes::CEALLACH, Race::KNGT, "Ceallach");
	vec_heroes[68] = new Heroes(Heroes::DRAKONIA, Race::WZRD, "Drakonia");
	vec_heroes[69] = new Heroes(Heroes::MARTINE, Race::SORC, "Martine");
	vec_heroes[70] = new Heroes(Heroes::JARKONAS, Race::BARB, "Jarkonas");
	vec_heroes[71] = H2Config::Debug() ? new Heroes(Heroes::SANDYSANDY, Race::WRLK, "SandySandy") : new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
    }
    else
    {
	vec_heroes[60] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[61] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[62] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[63] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[64] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[65] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[66] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[67] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[68] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[69] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[70] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[71] = H2Config::Debug() ? new Heroes(Heroes::SANDYSANDY, Race::WRLK, "SandySandy") : new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
	vec_heroes[72] = new Heroes(Heroes::UNKNOWN, Race::KNGT, "Unknown");
    }

    Display & display = Display::Get();

    // loading info
    display.Fill(0, 0, 0);
    TextBox("Maps Loading...", Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    AGG::PreloadObject(TIL::GROUND32);

    char byte8;
    u16  byte16;
    u32  byte32;
    std::vector<u16> vec_object; // index maps for OBJ_CASTLE, OBJ_HEROES, OBJ_SIGN, OBJ_BOTTLE, OBJ_EVENT


    // endof
    fd.seekg(0, std::ios_base::end);
    const u32 endof_mp2 = fd.tellg();

    // read uniq
    fd.seekg(endof_mp2 - sizeof(u32), std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&uniq0), sizeof(u32));
    SWAP32(uniq0);

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

    vec_tiles.resize(width * height);

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

	vec_tiles[ii] = tile;
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
    char unk_byte;
    fd.read(&unk_byte, 1);
    if(4 < H2Config::Debug() && unk_byte)
	printf("World::World: dump unknown byte: %hhX\n", unk_byte);

    if(4 < H2Config::Debug())
	    printf("World::World: dump final block: ");

    // count final mp2 blocks
    u16 countblock = 0;
    byte16 = 0xffff;
    while(byte16)
    {
	// debug endof mp2
	if(endof_mp2 < fd.tellg()) Error::Except("World::World: read maps: out of range.");

	fd.read(reinterpret_cast<char *>(&byte16), sizeof(u16));
	SWAP16(byte16);

	if(4 < H2Config::Debug())
	    printf("%hX", byte16);

	if(byte16) countblock = byte16 - 1;
    }

    if(4 < H2Config::Debug())
	printf("\n");

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
	    Maps::Tiles & tile = *vec_tiles.at(*it_index);
	    const Maps::TilesAddon *addon = NULL;

	    switch(tile.GetObject())
	    {
		case MP2::OBJ_CASTLE:
		    // add castle
		    if(SIZEOFMP2CASTLE != sizeblock) Error::Warning("World::World: read castle: incorrect size block.");
		    else
		    {
			Castle *castle = const_cast<Castle *>(GetCastle(*it_index));
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			}
			else
			Error::Warning("World::World: load castle: not found, index: ", *it_index);
		    }
		    break;
		case MP2::OBJ_RNDTOWN:
		case MP2::OBJ_RNDCASTLE:
		    // add rnd castle
		    if(SIZEOFMP2CASTLE != sizeblock) Error::Warning("World::World: read castle: incorrect size block.");
		    else
		    {
			Castle *castle = const_cast<Castle *>(GetCastle(*it_index));
			if(castle)
			{
			    castle->LoadFromMP2(pblock);
			    Maps::UpdateRNDSpriteForCastle(castle->GetCenter(), castle->GetRace(), castle->isCastle());
			    Maps::MinimizeAreaForCastle(castle->GetCenter());
			}
			else
			Error::Warning("World::World: load castle: not found, index: ", *it_index);
		    }
		    break;
		case MP2::OBJ_HEROES:
		    // add heroes
		    if(SIZEOFMP2HEROES != sizeblock) Error::Warning("World::World: read heroes: incorrect size block.");
		    else
		    if(NULL != (addon = tile.FindMiniHero()))
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
			Race::race_t race = Race::BOMG;
			switch(index_name % 7)
			{
			    case 0: race = Race::KNGT; break;
			    case 1: race = Race::BARB; break;
			    case 2: race = Race::SORC; break;
			    case 3: race = Race::WRLK; break;
			    case 4: race = Race::WZRD; break;
			    case 5: race = Race::NECR; break;
			    case 6: race = (Color::GRAY != color ? Settings::Get().FileInfo().KingdomRace(color) : Race::Rand()); break;
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
			    	(*herow).LoadFromMP2(*it_index, pblock, color);
			    	kingdom.AddHeroes(herow);
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
			map_sign[*it_index] = std::string(&pblock[9]);
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
		    vec_rumors.push_back(&pblock[8]);
		    if(H2Config::Debug()) Error::Verbose("add Rumors: " + vec_rumors.back());
		}
	    }
	}
	// debug
	else Error::Warning("World::World: read maps: unknown block addons, size: ", sizeblock);

	delete [] pblock;
    }

    // last rumors
    vec_rumors.push_back("This game is now in pre alpha development version. ;)");

    // close mp2
    fd.close();

    // modify other objects
    const u16 vec_size = vec_tiles.size();

    for(u16 ii = 0; ii < vec_size; ++ii)
    {
	Maps::Tiles & tile = *vec_tiles[ii];
	const Maps::TilesAddon *addon = NULL;

	// fix loyalty version objects
	if(Settings::Get().Modes(Settings::PRICELOYALTY)) tile.FixLoyaltyVersion();

	//
	switch(tile.GetObject())
	{
	    case MP2::OBJ_WITCHSHUT:
		map_witchshut[ii] = Skill::Secondary::RandForWitchsHut();
		break;

	    case MP2::OBJ_SHRINE1:
		map_shrine[ii] = Rand::Get(10) % 2 ? Spell::RandCombat(1) : Spell::RandAdventure(1);
		break;
	    case MP2::OBJ_SHRINE2:
		map_shrine[ii] = Rand::Get(10) % 2 ? Spell::RandCombat(2) : Spell::RandAdventure(2);
		break;
	    case MP2::OBJ_SHRINE3:
		map_shrine[ii] = Rand::Get(10) % 2 ? Spell::RandCombat(3) : Spell::RandAdventure(3);
		break;

	    case MP2::OBJ_STONELIGHTS:
		vec_teleports.push_back(ii);
		break;

	    case MP2::OBJ_WHIRLPOOL:
		map_whirlpools[tile.GetUniq1()].push_back(ii);
		break;

	    case MP2::OBJ_EVENT:
		// remove event sprite
		if(NULL != (addon = tile.FindEvent()))
		{
		    tile.Remove(addon->uniq);
		}
		break;
	
    	    case MP2::OBJ_BOAT:
		// remove small sprite boat
		if(NULL != (addon = tile.FindBoat()))
		{
		    tile.Remove(addon->uniq);
		}
		break;

    	    case MP2::OBJ_RNDULTIMATEARTIFACT:
		// remove ultimate artifact sprite
		if(NULL != (addon = tile.FindUltimateArtifact()))
		{
		    tile.Remove(addon->uniq);
		    tile.SetQuantity1(Artifact::FromIndexSprite(addon->index));
		    tile.SetObject(MP2::OBJ_ZERO);
		    ultimate_artifact = ii;
		}
		break;

    	    case MP2::OBJ_RNDARTIFACT:
    	    case MP2::OBJ_RNDARTIFACT1:
    	    case MP2::OBJ_RNDARTIFACT2:
    	    case MP2::OBJ_RNDARTIFACT3:
		// modify rnd artifact sprite
		Artifact::ChangeTileWithRNDArtifact(tile);
    		tile.UpdateQuantity();
		break;

	    case MP2::OBJ_RNDRESOURCE:
		// modify rnd resource sprite
		Resource::ChangeTileWithRNDResource(tile);
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
            case MP2::OBJ_TREASURECHEST:
            case MP2::OBJ_SHIPWRECK:
            case MP2::OBJ_DERELICTSHIP:
            case MP2::OBJ_GRAVEYARD:
            case MP2::OBJ_PYRAMID:
            case MP2::OBJ_DAEMONCAVE:
            case MP2::OBJ_ABANDONEDMINE:
    		tile.UpdateQuantity();
		break;

	    case MP2::OBJ_ANCIENTLAMP:
		    tile.SetCountMonster(Rand::Get(2, 4));
		break;

	    case MP2::OBJ_MONSTER:
		if(0 == tile.GetQuantity1() && 0 == tile.GetQuantity2())
		    tile.SetCountMonster(Monster::GetRNDSize(Monster::Monster(tile)));
		else
		    // old format
		    tile.SetCountMonster(((static_cast<u16>(tile.GetQuantity2()) << 8) | tile.GetQuantity1()) >> 3);
		break;

	    case MP2::OBJ_RNDMONSTER:
	    case MP2::OBJ_RNDMONSTER1:
	    case MP2::OBJ_RNDMONSTER2:
	    case MP2::OBJ_RNDMONSTER3:
	    case MP2::OBJ_RNDMONSTER4:
		// modify rnd monster sprite
		Monster::ChangeTileWithRNDMonster(tile);
		if(0 == tile.GetQuantity1() && 0 == tile.GetQuantity2())
		    tile.SetCountMonster(Monster::GetRNDSize(Monster::Monster(tile)));
		else
		    // old format
		    tile.SetCountMonster(((static_cast<u16>(tile.GetQuantity2()) << 8) | tile.GetQuantity1()) >> 3);
		break;

	    // join dwelling
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
		// initial update dwelling population
		{
		    const Monster::monster_t monster = Monster::Monster(tile.GetObject());
		    if(Monster::UNKNOWN == monster) continue;

		    // save count
		    u32 count = 0;

		    // dependence from level monster
		    switch(Monster::GetLevel(monster))
		    {
			case Monster::LEVEL1:	count = 4 * Monster::GetGrown(monster); break;
			case Monster::LEVEL2:	count = 2 * Monster::GetGrown(monster); break;
			case Monster::LEVEL3:	count = Monster::GetGrown(monster); break;
			case Monster::LEVEL4:	count = 2;
			default: break;
		    }

		    // dependence from game difficulty
		    switch(Settings::Get().GameDifficulty())
		    {
			case Difficulty::EASY:	count = count * 2;
			case Difficulty::NORMAL:break;
			case Difficulty::HARD:	count = static_cast<u32>(count / 1.5);
			case Difficulty::EXPERT:count = static_cast<u32>(count / 2);
			case Difficulty::IMPOSSIBLE:count = 0;
		    }

		    tile.SetCountMonster(count);
		}
		break;

	//
	    default:
		break;
	}
    }

    AGG::FreeObject(TIL::GROUND32);

    // sort castles to kingdoms
    std::vector<Castle *>::const_iterator itc1 = vec_castles.begin();
    std::vector<Castle *>::const_iterator itc2 = vec_castles.end();
    for(; itc1 != itc2; ++itc1)
	if(*itc1) GetKingdom((*itc1)->GetColor()).AddCastle(*itc1);

    // play with debug hero
    if(H2Config::Debug())
    {
	// get first castle position
	Kingdom & kingdom = GetMyKingdom();

	if(kingdom.GetCastles().size())
	{
	    const Castle & castle = *(kingdom.GetCastles().at(0));
	    const Heroes *hero = vec_heroes[Heroes::SANDYSANDY];

	    // place hero
	    if(hero && (*hero).isFreeman())
	    {
		const_cast<Heroes &>(*hero).Recruit(castle);

		kingdom.AddHeroes(const_cast<Heroes *>(hero));
	    }
	}
    }
    else
    // play with hero
    if(Settings::Get().FileInfo().PlayWithHeroes())
	for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii)
	    if((*vec_kingdoms[ii]).isPlay() && (*vec_kingdoms[ii]).GetCastles().size())
	    {
		// get first castle position
		Kingdom & kingdom = *(vec_kingdoms[ii]);
		const Castle & castle = *(kingdom.GetCastles().at(0));

		// place hero
		if(const Heroes *hero = GetFreemanHeroes(castle.GetRace()))
		{
		    const_cast<Heroes &>(*hero).Recruit(castle);

		    kingdom.AddHeroes(const_cast<Heroes *>(hero));
		}
	    }

    if(H2Config::Debug()) Error::Verbose("World::LoadMaps: end load.");
}

/* get human kindom */
Kingdom & World::GetMyKingdom(void)
{ return GetKingdom(Settings::Get().MyColor()); }

const Kingdom & World::GetMyKingdom(void) const
{ return GetKingdom(Settings::Get().MyColor()); }

/* get kingdom */
Kingdom & World::GetKingdom(Color::color_t color)
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

    if(Settings::Get().Debug()) Error::Warning("World::GetKingdom: return Color::GRAY.");

    return *vec_kingdoms[6];
}

const Kingdom & World::GetKingdom(Color::color_t color) const
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

    Error::Warning("World::GetKingdom: return Color::GRAY.");

    return *vec_kingdoms[6];
}

/* get castle from index maps */
const Castle * World::GetCastle(u16 maps_index)
{
    return GetCastle(maps_index % width, maps_index / height);
}

/* get castle from coord maps */
const Castle * World::GetCastle(u8 ax, u8 ay)
{
    std::vector<Castle *>::const_iterator it1 = vec_castles.begin();
    std::vector<Castle *>::const_iterator it2 = vec_castles.end();

    for(; it1 != it2; ++it1)
        if(*it1 && (*it1)->ContainCoord(ax, ay)) return *it1;

    Error::Warning("World::GetCastle: return NULL pointer");

    return NULL;
}

/* get heroes from index maps */
const Heroes * World::GetHeroes(u16 maps_index) const
{
    return GetHeroes(maps_index % width, maps_index / height);
}

/* get heroes from coord maps */
const Heroes * World::GetHeroes(u8 ax, u8 ay) const
{
    std::vector<Heroes *>::const_iterator it1 = vec_heroes.begin();
    std::vector<Heroes *>::const_iterator it2 = vec_heroes.end();

    for(; it1 != it2; ++it1)
        if(*it1 && (*it1)->GetCenter().x == ax && (*it1)->GetCenter().y == ay) return *it1;

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
        NewWeek();
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewWeek();
    }
    
    // action new month
    if(begin_month)
    {
        NewMonth();
        for(u8 ii = 0; ii < vec_kingdoms.size(); ++ii) if((*vec_kingdoms[ii]).isPlay()) (*vec_kingdoms[ii]).ActionNewMonth();
    }
}

void World::NewWeek(void)
{
    // change free recruit hero
    const Race::race_t & rc = Settings::Get().FileInfo().KingdomRace(Settings::Get().MyColor());
    free_recruit_hero1 = GetFreemanHeroes(rc)->GetID();
    free_recruit_hero2 = GetFreemanHeroes()->GetID();
    
    UpdateDwellingPopulation();

    UpdateMonsterPopulation();

    // update week object
    std::vector<Maps::Tiles *>::const_iterator it1 = vec_tiles.begin();
    std::vector<Maps::Tiles *>::const_iterator it2 = vec_tiles.end();

    for(; it1 != it2; ++it1)
	if(*it1 && MP2::isWeekLife((*it1)->GetObject())) (*it1)->UpdateQuantity();

    // update week type
    week_name = BeginMonth() ? Week::MonthRand() : Week::WeekRand();

    // TODO:: action for week type: PLAGUE and MONSTERS
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

const Heroes::heroes_t & World::GetFreeRecruit1(void)
{
    const Race::race_t & rc = Settings::Get().FileInfo().KingdomRace(Settings::Get().MyColor());

    if(Heroes::UNKNOWN == free_recruit_hero1 || !(*vec_heroes[free_recruit_hero1]).isFreeman()) free_recruit_hero1 = GetFreemanHeroes(rc)->GetID();

    return free_recruit_hero1;
}

const Heroes::heroes_t & World::GetFreeRecruit2(void)
{
    if(Heroes::UNKNOWN == free_recruit_hero2 || !(*vec_heroes[free_recruit_hero2]).isFreeman()) free_recruit_hero2 = GetFreemanHeroes()->GetID();

    while(free_recruit_hero1 == free_recruit_hero2)
    {
	Error::Verbose("World::GetFreeRecruit2: hero1 equal hero2");

	free_recruit_hero2 = GetFreemanHeroes()->GetID();
    }

    return free_recruit_hero2;
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
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back((*vec_heroes[ii]).GetID());

    // not found, find other race
    if(Race::BOMG == rc || freeman_heroes.empty())
	for(u8 ii = 0; ii <= 53; ++ii)
	    if((*vec_heroes[ii]).isFreeman()) freeman_heroes.push_back((*vec_heroes[ii]).GetID());

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
    return vec_rumors[Rand::Get(vec_rumors.size() - 1)];
}

/* return spell from shrine circle */
Spell::spell_t World::SpellFromShrine(const u16 index)
{
    return map_shrine[index];
}

/* return random teleport destination */
u16 World::NextTeleport(const u16 index) const
{
    if(2 > vec_teleports.size())
    {
	Error::Warning("World::NextTeleport: is empty.");

	return index;
    }

    std::vector<u16> v;
    v.reserve(vec_teleports.size());

    std::vector<u16>::const_iterator it1 = vec_teleports.begin();
    std::vector<u16>::const_iterator it2 = vec_teleports.end();
    for(; it1 != it2; ++it1)
    {
	const u16 & i = *it1;
	if(i == index) continue;
	if(NULL != world.GetHeroes(i)) continue;
	v.push_back(i);
    }

    if(v.empty()) Error::Warning("World::NextTeleport: is full.");

    return v.size() ? *Rand::Get(v) : index;
}

/* return random whirlpools destination */
u16 World::NextWhirlpool(const u16 index)
{
    if(2 > map_whirlpools.size())
    {
	Error::Warning("World::NextWhirlpool: is empty.");

	return index;
    }

    const Maps::Tiles & tile = GetTiles(index);
    std::vector<u32> v1;
    v1.reserve(map_whirlpools.size());

    std::map<u32, std::vector<u16> >::const_iterator it1 = map_whirlpools.begin();
    std::map<u32, std::vector<u16> >::const_iterator it2 = map_whirlpools.end();
    for(; it1 != it2; ++it1)
    {
	const u32 & uniq = (*it1).first;

	if(uniq == tile.GetUniq1()) continue;
	v1.push_back(uniq);
    }

    std::vector<u16> & v2 = map_whirlpools[*Rand::Get(v1)];

    std::vector<u16> v3;
    v3.reserve(v2.size());

    std::vector<u16>::const_iterator it3 = v2.begin();
    std::vector<u16>::const_iterator it4 = v2.end();
    for(; it3 != it4; ++it3)
    {
	const u16 & i = *it3;
	if(i == index) continue;
	if(NULL != world.GetHeroes(i)) continue;
	v3.push_back(i);
    }

    if(v3.empty()) Error::Warning("World::NextWhirlpool: is full.");

    return v3.size() ? *Rand::Get(v3) : index;
}

/* return skill from witchs hut */
Skill::Secondary::skill_t World::SkillFromWitchsHut(const u16 index)
{
    return map_witchshut[index];
}

/* return message from sign */
const std::string & World::MessageSign(const u16 index)
{
    return map_sign[index];
}

/* return count captured object */
u16 World::CountCapturedObject(const MP2::object_t obj, const Color::color_t col) const
{
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = map_captureobj.begin();
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = map_captureobj.end();

    u16 result = 0;

    for(; it1 != it2; ++it1) if((*it1).second.first == obj && (*it1).second.second == col) ++result;

    return result;
}

/* return count captured mines */
u16 World::CountCapturedMines(const Resource::resource_t res, const Color::color_t col) const
{
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = map_captureobj.begin();
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = map_captureobj.end();

    u16 result = 0;

    for(; it1 != it2; ++it1)
	if(MP2::OBJ_MINES == (*it1).second.first || MP2::OBJ_HEROES == (*it1).second.first)
    {
	    // scan for find mines
	    const Maps::TilesAddon * addon = GetTiles((*it1).first).FindMines();

	    if(addon)
	    {
		// index sprite EXTRAOVR
		if(0 == addon->index && Resource::ORE == res && (*it1).second.second == col) ++result;
		else
		if(1 == addon->index && Resource::SULFUR == res && (*it1).second.second == col) ++result;
		else
		if(2 == addon->index && Resource::CRYSTAL == res && (*it1).second.second == col) ++result;
		else
		if(3 == addon->index && Resource::GEMS == res && (*it1).second.second == col) ++result;
		else
		if(4 == addon->index && Resource::GOLD == res && (*it1).second.second == col) ++result;
	    }
    }
    
    return result;
}

/* capture object */
void World::CaptureObject(const u16 index, const Color::color_t col)
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
}

/* return color captured object */
Color::color_t World::ColorCapturedObject(const u16 index) const
{
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = map_captureobj.begin();
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = map_captureobj.end();

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

        for(; it1 != it2; ++it1) if(*it1 && color & (**it1).GetColor()) Maps::ClearFog((**it1).GetCenter(), 5, color);
    }

    // clear adboar heroes
    if(vec_heroes.size())
    {
        std::vector<Heroes *>::const_iterator it1 = vec_heroes.begin();
        std::vector<Heroes *>::const_iterator it2 = vec_heroes.end();

        for(; it1 != it2; ++it1) if(*it1 && color & (**it1).GetColor()) (**it1).Scoute();
    }

    // clear abroad objects
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = map_captureobj.begin();
    std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = map_captureobj.end();

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
    std::vector<Maps::Tiles *>::iterator it1 = vec_tiles.begin();
    std::vector<Maps::Tiles *>::const_iterator it2 = vec_tiles.end();
    for(; it1 != it2; ++it1)
    {
	Maps::Tiles & tile = **it1;
	const MP2::object_t & obj = tile.GetObject();
	switch(obj)
	{
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
	    {
		const Monster::monster_t monster = Monster::Monster(obj);
		if(Monster::UNKNOWN == monster) continue;

		tile.SetCountMonster(tile.GetCountMonster() + Monster::GetGrown(monster));
		break;
	    }

	    case MP2::OBJ_RUINS:
            case MP2::OBJ_TREECITY:
            case MP2::OBJ_WAGONCAMP:
            case MP2::OBJ_DESERTTENT:
            case MP2::OBJ_TROLLBRIDGE:
            case MP2::OBJ_DRAGONCITY:
            case MP2::OBJ_CITYDEAD:
            {
		const Monster::monster_t monster = Monster::Monster(obj);
		if(Monster::UNKNOWN == monster) continue;

		if(0 == tile.GetCountMonster()) tile.SetCountMonster(tile.GetCountMonster() + Monster::GetGrown(monster));
        	break;
	    }

	    default: break;;
	}
    }
}

void World::UpdateMonsterPopulation(void)
{
    std::vector<Maps::Tiles *>::const_iterator it1 = vec_tiles.begin();
    std::vector<Maps::Tiles *>::const_iterator it2 = vec_tiles.end();
        
    for(; it1 != it2; ++it1) if(*it1 && MP2::OBJ_MONSTER == (*it1)->GetObject())
    {
	Maps::Tiles & tile = **it1;
	const u16 count = tile.GetCountMonster();

	if(0 == count)
	    tile.SetCountMonster(Monster::GetRNDSize(Monster::Monster(tile)));
	else
	    tile.SetCountMonster(count * 8 / 7);
    }
}

Artifact::artifact_t World::DiggingForUltimateArtifacts(const Point & center)
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

    if(ultimate_artifact == tile.GetIndex() && tile.GetQuantity1())
    {
	tile.SetQuantity1(0);
	ultimate_artifact = 0xFFFF;

	return Artifact::Artifact(tile.GetQuantity1());
    }

    return  Artifact::UNKNOWN;
}

void World::ActionForMagellanMaps(u8 color)
{
    std::vector<Maps::Tiles *>::const_iterator it1 = vec_tiles.begin();
    std::vector<Maps::Tiles *>::const_iterator it2 = vec_tiles.end();
        
    for(; it1 != it2; ++it1) if(*it1 && Maps::Ground::WATER == (*it1)->GetGround()) (*it1)->ClearFog(color);
}

u16 World::GetNearestObject(const u16 center, const MP2::object_t obj)
{
    u16 res = MAXU16;
    u16 min = MAXU16;

    std::vector<Maps::Tiles *>::const_iterator it1 = vec_tiles.begin();
    std::vector<Maps::Tiles *>::const_iterator it2 = vec_tiles.end();

    for(; it1 != it2; ++it1) if(*it1 && obj == (*it1)->GetObject())
    {
	const u16 min2 = Maps::GetApproximateDistance(center, (*it1)->GetIndex());
	if(min2 < min)
	{
	    min = min2;
	    res = (*it1)->GetIndex();
	}
    }

    return res;
}

// create boat: with option build or summon
bool World::CreateBoat(const u16 center, bool build)
{
    // scan 3x3
    const s16 cx = center % world.w();
    const s16 cy = center / world.w();

    for(s16 y = -3; y <= 3; ++y)
        for(s16 x = -3; x <= 3; ++x)
	    if(Maps::isValidAbsPoint(cx + x, cy + y))
    {
        Maps::Tiles & tile = GetTiles(Maps::GetIndexFromAbsPoint(cx + x, cy + y));

        if(Maps::Ground::WATER == tile.GetGround() &&
    	    MP2::OBJ_ZERO == tile.GetObject())
	{
    	    const u16 coast = Maps::ScanAroundObject(tile.GetIndex(), MP2::OBJ_COAST, false);

	    if(MAXU16 != coast && Algorithm::PathFind(NULL, center, coast, 256))
    	    {
    		if(build)
    		{
    		    tile.SetObject(MP2::OBJ_BOAT);
    		    return true;
    		}
    		else
    		{
    		    const u16 boat = GetNearestObject(center, MP2::OBJ_BOAT);
    		    if(MAXU16 != boat)
    		    {
    			world.GetTiles(boat).SetObject(MP2::OBJ_ZERO);
    			tile.SetObject(MP2::OBJ_BOAT);
    			return true;
    		    }
    		}
    		break;
    	    }
	}
    }

    Error::Warning("World::CreateBoat: failed");
    return false;
}

const GameEvent::Day*  World::GetEventDay(const Color::color_t c) const
{
    if(vec_eventsday.size())
    {
	std::vector<GameEvent::Day *>::const_iterator it1 = vec_eventsday.begin();
	std::vector<GameEvent::Day *>::const_iterator it2 = vec_eventsday.end();

	for(; it1 != it2; ++it1) if(*it1)
	{
	    const GameEvent::Day & event = **it1;
	    const u16 today = day + 1;
	    const u16 first = event.GetFirst();
	    const u16 sequent = event.GetSubsequent();

	    if((first == today ||
	       (sequent && (first < today && 0 == ((today - first) % sequent)))) &&
	       (c & event.GetColors())) return *it1;
	}
    }

    return NULL;
}

const GameEvent::Coord* World::GetEventMaps(const Color::color_t c, const u16 index) const
{
    if(vec_eventsmap.size())
    {
	std::vector<GameEvent::Coord *>::const_iterator it1 = vec_eventsmap.begin();
	std::vector<GameEvent::Coord *>::const_iterator it2 = vec_eventsmap.end();

	for(; it1 != it2; ++it1)
	    if(*it1 && (*it1)->GetIndex() == index && (c & (*it1)->GetColors())) return *it1;
    }

    return NULL;
}
