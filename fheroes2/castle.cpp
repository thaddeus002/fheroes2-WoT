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

#include "difficulty.h"
#include "config.h"
#include "error.h"
#include "payment.h"
#include "world.h"
#include "agg.h"
#include "castle.h"

Castle::Castle(u32 gid, u16 mapindex, const void *ptr, bool rnd)
    : building(0), army_spread(true), allow_build(true), dwelling(CASTLEMAXMONSTER, 0),
      army(CASTLEMAXARMY), uniq(gid), mp(mapindex % world.w(), mapindex / world.h())
{
    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    u16 swap16 = 0;

    switch(*byte8)
    {
	case 0x00: color = Color::BLUE;   break;
	case 0x01: color = Color::GREEN;  break;
        case 0x02: color = Color::RED;    break;
        case 0x03: color = Color::YELLOW; break;
        case 0x04: color = Color::ORANGE; break;
        case 0x05: color = Color::PURPLE; break;
        default:   color = Color::GRAY;   break;
    }
    ++byte8;
    
    // custom building
    if(*byte8)
    {
	++byte8;
	
	// building
	byte16 = reinterpret_cast<const u16 *>(byte8);
	swap16 = *byte16;
	SWAP16(swap16);
        if(0x0002 & swap16) building |= BUILD_THIEVESGUILD;
        if(0x0004 & swap16) building |= BUILD_TAVERN;
        if(0x0008 & swap16) building |= BUILD_SHIPYARD;
        if(0x0010 & swap16) building |= BUILD_WELL;
        if(0x0080 & swap16) building |= BUILD_STATUE;
        if(0x0100 & swap16) building |= BUILD_LEFTTURRET;
        if(0x0200 & swap16) building |= BUILD_RIGHTTURRET;
        if(0x0400 & swap16) building |= BUILD_MARKETPLACE;
        if(0x1000 & swap16) building |= BUILD_MOAT;
        if(0x0800 & swap16) building |= BUILD_WEL2;
        if(0x2000 & swap16) building |= BUILD_SPEC;
	++byte16;

	// dwelling
	swap16 = *byte16;
	SWAP16(swap16);
        if(0x0008 & swap16) building |= DWELLING_MONSTER1;
        if(0x0010 & swap16) building |= DWELLING_MONSTER2;
        if(0x0020 & swap16) building |= DWELLING_MONSTER3;
        if(0x0040 & swap16) building |= DWELLING_MONSTER4;
        if(0x0080 & swap16) building |= DWELLING_MONSTER5;
        if(0x0100 & swap16) building |= DWELLING_MONSTER6;
        if(0x0200 & swap16) building |= DWELLING_UPGRADE2;
        if(0x0400 & swap16) building |= DWELLING_UPGRADE3;
        if(0x0800 & swap16) building |= DWELLING_UPGRADE4;
        if(0x1000 & swap16) building |= DWELLING_UPGRADE5;
        if(0x2000 & swap16) building |= DWELLING_UPGRADE6;
	++byte16;

	
	// magic tower
	byte8 = reinterpret_cast<const u8 *>(byte16);
	if(0 < *byte8) building |= BUILD_MAGEGUILD1;
	if(1 < *byte8) building |= BUILD_MAGEGUILD2;
	if(2 < *byte8) building |= BUILD_MAGEGUILD3;
	if(3 < *byte8) building |= BUILD_MAGEGUILD4;
	if(4 < *byte8) building |= BUILD_MAGEGUILD5;
	++byte8;
    }
    else
    {
	byte8 += 6;

	// default building
        building |= BUILD_TAVERN;
        building |= DWELLING_MONSTER1;
        building |= DWELLING_MONSTER2;
	// default dwelling
	// default magic tower
    }

    // custom troops
    bool custom_troops = *byte8;
    if(custom_troops)
    {
	++byte8;
	
	// monster1
	army[0].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster2
	army[1].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster3
	army[2].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster4
	army[3].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster5
	army[4].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// count1
	byte16 = reinterpret_cast<const u16 *>(byte8);
	swap16 = *byte16;
	SWAP16(swap16);
	army[0].SetCount(swap16);
	++byte16;

	// count2
	swap16 = *byte16;
	SWAP16(swap16);
	army[1].SetCount(swap16);
	++byte16;

	// count3
	swap16 = *byte16;
	SWAP16(swap16);
	army[2].SetCount(swap16);
	++byte16;

	// count4
	swap16 = *byte16;
	SWAP16(swap16);
	army[3].SetCount(swap16);
	++byte16;

	// count5
	swap16 = *byte16;
	SWAP16(swap16);
	army[4].SetCount(swap16);
	++byte16;

	byte8 = reinterpret_cast<const u8 *>(byte16);
    }
    else
    {
	byte8 += 16;
    }
    
    // captain
    captain = (*byte8 ? true : false);
    ++byte8;
    
    // custom name
    ++byte8;
    name = std::string(reinterpret_cast<const char *>(byte8));
    byte8 += 13;

    // race
    switch(*byte8)
    {
	case 0x00: race = Race::KNGT; break;
	case 0x01: race = Race::BARB; break;
	case 0x02: race = Race::SORC; break;
	case 0x03: race = Race::WRLK; break;
	case 0x04: race = Race::WZRD; break;
	case 0x05: race = Race::NECR; break;
	default: race = (Color::GRAY != color ? H2Config::GetKingdomRace(color) : Race::Rand()); break;
    }
    ++byte8;

    // castle
    if(*byte8) building |= BUILD_CASTLE;
    ++byte8;

    building |= (building & BUILD_CASTLE ? BUILD_CASTLE : BUILD_TENT);

    // allow upgrade to castle (0 - true, 1 - false)
    allow_castle = (*byte8 ? false : true);
    ++byte8;

    // unknown 29 byte
    //


    // troops auto pack
    if(!custom_troops)
    {
	const Monster::stats_t mon1 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));
	const Monster::stats_t mon2 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER2));

	switch(H2Config::GetGameDifficulty())
	{
    	    case Difficulty::EASY:
        	army[0].SetMonster(mon1.monster);
        	army[0].SetCount(mon1.grown * 2);
        	army[1].SetMonster(mon2.monster);
        	army[1].SetCount(mon2.grown * 2);
        	break;

    	    case Difficulty::NORMAL:
        	army[0].SetMonster(mon1.monster);
        	army[0].SetCount(mon1.grown);
        	break;

    	    case Difficulty::HARD:
    	    case Difficulty::EXPERT:
    	    case Difficulty::IMPOSSIBLE:
        	break;
	}
    }
    // dwelling pack
    if(building & DWELLING_MONSTER1) dwelling[0]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER1));
    if(building & DWELLING_MONSTER2) dwelling[1]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER2));
    if(building & DWELLING_UPGRADE2) dwelling[1]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE2));
    if(building & DWELLING_MONSTER3) dwelling[2]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER3));
    if(building & DWELLING_UPGRADE3) dwelling[2]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE3));
    if(building & DWELLING_MONSTER4) dwelling[3]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER4));
    if(building & DWELLING_UPGRADE4) dwelling[3]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE4));
    if(building & DWELLING_MONSTER5) dwelling[4]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER5));
    if(building & DWELLING_UPGRADE5) dwelling[4]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE5));
    if(building & DWELLING_MONSTER6) dwelling[5]  = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER6));
    if(building & DWELLING_UPGRADE6) dwelling[5]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE6));
    if(building & DWELLING_UPGRADE7) dwelling[5]  = Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE7));

    // modify RND sprites
    if(rnd) CorrectAreaMaps();

    // minimize area maps id
    MinimizeAreaMapsID();
    
    present_boat = false;
    
    // end
    Error::Verbose((building & BUILD_CASTLE ? "add castle: " : "add town: ") + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

const Heroes * Castle::isHeroesPresent(void)
{
    return world.GetHeroes(mp.x, mp.y);
}

void Castle::ActionNewDay(void)
{
    allow_build = true;
}

void Castle::ActionNewWeek(void)
{
    u8 well = building & BUILD_WELL ? GROWN_WELL : 0;
    u8 wel2 = building & BUILD_WEL2 ? GROWN_WEL2 : 0;

    // dw 1
    if(building & DWELLING_MONSTER1) dwelling[0]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER1)) + well + wel2;

    // dw 2
    if(building & DWELLING_UPGRADE2) dwelling[1]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE2)) + well;
    else
    if(building & DWELLING_MONSTER2) dwelling[1]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER2)) + well;

    // dw 3
    if(building & DWELLING_UPGRADE3) dwelling[2]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE3)) + well;
    else
    if(building & DWELLING_MONSTER3) dwelling[2]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER3)) + well;

    // dw 4
    if(building & DWELLING_UPGRADE4) dwelling[3]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE4)) + well;
    else
    if(building & DWELLING_MONSTER4) dwelling[3]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER4)) + well;

    // dw 5
    if(building & DWELLING_UPGRADE5) dwelling[4]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE5)) + well;
    else
    if(building & DWELLING_MONSTER5) dwelling[4]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER5)) + well;

    // dw 6
    if(building & DWELLING_UPGRADE7) dwelling[5]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE7)) + well;
    else
    if(building & DWELLING_UPGRADE6) dwelling[5]  += Monster::GetGrown(Monster::Monster(race, DWELLING_UPGRADE6)) + well;
    else
    if(building & DWELLING_MONSTER6) dwelling[5]  += Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER6)) + well;
}

void Castle::ActionNewMonth(void)
{
}

// change castle color
void Castle::ChangeColor(Color::color_t cl)
{
    color = cl;

    // modify left flag
    ModifyTIlesFlags(world.GetTiles(mp.x - 1, mp.y));

    // modify right flag
    ModifyTIlesFlags(world.GetTiles(mp.x + 1, mp.y));
}

/* correct sprites for RND castles */
void Castle::CorrectAreaMaps(void)
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

    // T0
    if(isCastle()) coords.push_back((mp.y - 3) * world.h() + mp.x);
    // T1
    coords.push_back((mp.y - 2) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 2) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 2);
    // T2
    coords.push_back((mp.y - 1) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 1) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 2);
    // B1
    coords.push_back(mp.y * world.h() + mp.x - 2);
    coords.push_back(mp.y * world.h() + mp.x - 1);
    coords.push_back(mp.y * world.h() + mp.x);
    coords.push_back(mp.y * world.h() + mp.x + 1);
    coords.push_back(mp.y * world.h() + mp.x + 2);
    // B2
    coords.push_back((mp.y + 1) * world.h() + mp.x - 2);
    coords.push_back((mp.y + 1) * world.h() + mp.x - 1);
    coords.push_back((mp.y + 1) * world.h() + mp.x);
    coords.push_back((mp.y + 1) * world.h() + mp.x + 1);
    coords.push_back((mp.y + 1) * world.h() + mp.x + 2);

    const u16 index_center = mp.y * world.h() + mp.x;
    
    Maps::Tiles & tile_center = world.GetTiles(index_center);

    // correct only RND town and castle
    switch(tile_center.GetObject())
    {
	case MP2::OBJ_RNDTOWN:
	case MP2::OBJ_RNDCASTLE:
	    break;
	
	default:
	    Error::Warning("Castle::CorrectAreaMaps: correct only RND town and castle. index: ", index_center);
	    return;
    }

    // modify all rnd sprites
    std::vector<u16>::const_iterator itc = coords.begin();
    for(; itc != coords.end(); ++itc) ModifyTIlesRNDSprite(world.GetTiles(*itc));
}

void Castle::MinimizeAreaMapsID(void)
{
    // reset castle ID
    for(s8 yy = -3; yy < 2; ++yy)
	for(s8 xx = -2; xx < 3; ++xx)
    {
	Maps::Tiles & tile = world.GetTiles((mp.y + yy) * world.h() + mp.x + xx);

	if(MP2::OBJN_RNDCASTLE == tile.GetObject() ||
     	     MP2::OBJN_RNDTOWN == tile.GetObject() ||
	     MP2::OBJN_CASTLE  == tile.GetObject()) tile.SetObject(MP2::OBJ_ZERO);
    }

    // set minimum area castle ID
    for(s8 yy = -1; yy < 1; ++yy)
	for(s8 xx = -2; xx < 3; ++xx)
    {
	Maps::Tiles & tile = world.GetTiles((mp.y + yy) * world.h() + mp.x + xx);

	tile.SetObject(MP2::OBJN_CASTLE);
    }

    // restore center ID
    Maps::Tiles & tile_center = world.GetTiles(mp.y * world.h() + mp.x);

    tile_center.SetObject(MP2::OBJ_CASTLE);
}

/* modify RND sprites alghoritm */
void Castle::ModifyTIlesRNDSprite(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = NULL;

    if( (addon = tile.FindAddon(0x98, 0, 31)) || (addon = tile.FindAddon(0x99, 0, 31)) ||
	(addon = tile.FindAddon(0x9A, 0, 31)) || (addon = tile.FindAddon(0x9B, 0, 31)))
	{
            Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

            addon_w.SetObject((*addon).GetObject() - 12);

            switch(race)
            {
        	case Race::KNGT: break; //addon_w.SetIndex((*addon).GetIndex()); break;
                case Race::BARB: addon_w.SetIndex((*addon).GetIndex() + 32); break;
                case Race::SORC: addon_w.SetIndex((*addon).GetIndex() + 64); break;
                case Race::WRLK: addon_w.SetIndex((*addon).GetIndex() + 96); break;
                case Race::WZRD: addon_w.SetIndex((*addon).GetIndex() + 128); break;
                case Race::NECR: addon_w.SetIndex((*addon).GetIndex() + 160); break;
                default: Error::Warning("Castle::ModifyTIlesRNDSprite: unknown race."); break;
	    }
	}
}

/* modify flags sprite to origin color */
void Castle::ModifyTIlesFlags(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = tile.FindAddon(0x38);

    if(addon)
    {
        Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

	// right flag - event index
	bool right_event = (*addon).GetIndex() % 2 ? true : false;

	u8 index_sprite = 0;

	switch(color)
	{
	    case Color::BLUE:	index_sprite = right_event ? 0 : 1; break;
	    case Color::GREEN:	index_sprite = right_event ? 2 : 3; break;
	    case Color::RED:	index_sprite = right_event ? 4 : 5; break;
	    case Color::YELLOW:	index_sprite = right_event ? 6 : 7; break;
	    case Color::ORANGE:	index_sprite = right_event ? 8 : 9; break;
	    case Color::PURPLE:	index_sprite = right_event ? 10 : 11; break;
	    case Color::GRAY:	index_sprite = right_event ? 12 : 13; break;
	    default: Error::Warning("Castle::ModifyTIlesFlags: unknown color."); break;
	}

	addon_w.SetIndex(index_sprite);
    }
}

// modify Town sprite to Castle
void Castle::ModifyTilesTownToCastle(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = NULL;

    if( (addon = tile.FindAddon(0x8C)) || (addon = tile.FindAddon(0x8D)) ||
	(addon = tile.FindAddon(0x8E)) || (addon = tile.FindAddon(0x8F)))
	{
            Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

            addon_w.SetIndex((*addon).GetIndex() - 16);
	}
}

void Castle::TownUpgradeToCastle(void)
{
    // correct area maps sprites
    std::vector<u16> coords;

    // T0
    coords.push_back((mp.y - 3) * world.h() + mp.x);
    // T1
    coords.push_back((mp.y - 2) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 2) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 2);
    // T2
    coords.push_back((mp.y - 1) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 1) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 2);
    // B1
    coords.push_back(mp.y * world.h() + mp.x - 2);
    coords.push_back(mp.y * world.h() + mp.x - 1);
    coords.push_back(mp.y * world.h() + mp.x);
    coords.push_back(mp.y * world.h() + mp.x + 1);
    coords.push_back(mp.y * world.h() + mp.x + 2);

    // modify all rnd sprites
    std::vector<u16>::const_iterator itc = coords.begin();
    for(; itc != coords.end(); ++itc) ModifyTilesTownToCastle(world.GetTiles(*itc));




    // remove tent
    building &= ~BUILD_TENT;
}

// return valid count army in castle
u8 Castle::GetCountArmy(void) const
{
    u8 result = 0;

    for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii) if(army[ii].Valid()) ++result;
    
    return result;
}

// return mage guild level
u8 Castle::GetLevelMageGuild(void)
{
    return mageguild.GetLevel();
}

const std::string & Castle::GetStringBuilding(const building_t & build, const Race::race_t & race)
{
    static const std::string str_build[] = { "Thieves' Guild", "Tavern", "Shipyard", "Well", "Statue", "Left Turret",
	"Right Turret", "Marketplace", "Moat", "Castle", "Tent", "Captain's Quarters", "Mage Guild, Level 1", 
	 "Mage Guild, Level 2",  "Mage Guild, Level 3",  "Mage Guild, Level 4",  "Mage Guild, Level 5", "Unknown" };

    static const std::string str_wel2[] = { "Farm", "Garbage Heap", "Crystal Garden", "Orchard", "Waterfall", "Skull Pile" };

    static const std::string str_spec[] = { "Fortifications", "Coliseum", "Rainbow", "Library", "Dungeon", "Storm" };

    static const std::string str_dwelling[] = {
	"Thatched Hut", "Hut", "Treehouse", "Cave", "Habitat", "Excavation",
	"Archery Range", "Stick Hut", "Cottage", "Crypt", "Pen", "Graveyard",
	"Blacksmith", "Den", "Archery Range", "Nest", "Foundry", "Pyramid",
	"Armory", "Adobe", "Stonehenge", "Maze", "Cliff Nest", "Mansion",
	"Jousting Arena", "Bridge", "Fenced Meadow", "Swamp", "Ivory Tower", "Mausoleum",
	"Cathedral", "Pyramid", "Red Tower", "Green Tower", "Cloud Castle", "Laboratory" };

    static const std::string str_upgrade[] = {
	"Upg. Archery Range", "Upg. Stick Hut", "Upg. Cottage", "Crypt", "Pen", "Upg. Graveyard",
	"Upg. Blacksmith", "Den", "Upg. Archery Range", "Nest", "Upg. Foundry", "Upg. Pyramid",
	"Upg. Armory", "Upg. Adobe", "Upg. Stonehenge", "Upg. Maze", "Cliff Nest", "Upg. Mansion",
	"Upg. Jousting Arena", "Upg. Bridge", "Fenced Meadow", "Swamp", "Upg. Ivory Tower", "Upg. Mausoleum",
	"Upg. Cathedral", "Pyramid", "Red Tower", "Red Tower", "Upg. Cloud Castle", "Laboratory",
	"Upg. Cathedral", "Pyramid", "Red Tower", "Black Tower", "Upg. Cloud Castle", "Laboratory" };

    u8 offset = 0;

    switch(race)
    {
	case Race::KNGT: offset = 0; break;
	case Race::BARB: offset = 1; break;
	case Race::SORC: offset = 2; break;
	case Race::WRLK: offset = 3; break;
	case Race::WZRD: offset = 4; break;
	case Race::NECR: offset = 5; break;
	default: break;
    }

    switch(build)
    {
        case BUILD_THIEVESGUILD:return str_build[0];
        case BUILD_TAVERN:	return str_build[1];
        case BUILD_SHIPYARD:	return str_build[2];
        case BUILD_WELL:	return str_build[3];
        case BUILD_STATUE:	return str_build[4];
        case BUILD_LEFTTURRET:	return str_build[5];
        case BUILD_RIGHTTURRET:	return str_build[6];
        case BUILD_MARKETPLACE:	return str_build[7];
        case BUILD_MOAT:	return str_build[8];
        case BUILD_CASTLE:	return str_build[9];
        case BUILD_TENT:	return str_build[10];
        case BUILD_CAPTAIN:	return str_build[11];
        case BUILD_MAGEGUILD1:	return str_build[12];
        case BUILD_MAGEGUILD2:	return str_build[13];
        case BUILD_MAGEGUILD3:	return str_build[14];
        case BUILD_MAGEGUILD4:	return str_build[15];
        case BUILD_MAGEGUILD5:	return str_build[16];

        case BUILD_SPEC:	return str_spec[offset];
        case BUILD_WEL2:	return str_wel2[offset];

        case DWELLING_MONSTER1:	return str_dwelling[offset];
        case DWELLING_MONSTER2:	return str_dwelling[6 + offset];
        case DWELLING_MONSTER3:	return str_dwelling[12 + offset];
        case DWELLING_MONSTER4:	return str_dwelling[18 + offset];
        case DWELLING_MONSTER5:	return str_dwelling[24 + offset];
        case DWELLING_MONSTER6:	return str_dwelling[30 + offset];

        case DWELLING_UPGRADE2: return str_upgrade[offset];
        case DWELLING_UPGRADE3: return str_upgrade[6 + offset];
        case DWELLING_UPGRADE4: return str_upgrade[12 + offset];
        case DWELLING_UPGRADE5: return str_upgrade[18 + offset];
        case DWELLING_UPGRADE6: return str_upgrade[24 + offset];
        case DWELLING_UPGRADE7: return str_upgrade[30 + offset];

	default: break;
    }
    
    return str_build[17];
}

const std::string & Castle::GetDescriptionBuilding(const building_t & build, const Race::race_t & race)
{
    static const std::string desc_build[] = {
	"The Thieves' Guild provides information on enemy players. Thieves' Guilds can also provide scouting information on enemy towns.",
	"The Tavern increases morale for troops defending the castle.",
	"The Shipyard allows ships to be built.",
	"The Well increases the growth rate of all dwellings by 2 creatures per week.",
	"The Statue increases your town's income by 250 per day.",
	"The Left Turret provides extra firepower during castle combat.",
	"The Right Turret provides extra firepower during castle combat.",
	"The Marketplace can be used to convert one type of resource into another. The more marketplaces you control, the better the exchange rate.",
	"The Moat slows attacking units. Any unit entering the moat must end its turn there and becomes more vulnerable to attack.",
	"The Castle improves town defense and increases income to 1000 gold per day.",
	"The Tent provides workers to build a castle, provided the materials and the gold are available.",
	"The Captain's Quarters provides a captain to assist in the castle's defense when no hero is present.",
	"The Mage Guild allows heroes to learn spells and replenish their spell points.", "Unknown" };

    static const std::string desc_wel2[] = {
	"The Farm increases production of Peasants by 8 per week.",
	"The Garbage Heap increases production of Goblins by 8 per week.",
	"The Crystal Garden increases production of Sprites by 8 per week.",
	"The Waterfall increases production of Centaurs by 8 per week.",
	"The Orchard increases production of Halflings by 8 per week.",
	"The Skull Pile increases production of Skeletons by 8 per week." };

    static const std::string desc_spec[] = {
	"The Fortifications increase the toughness of the walls, increasing the number of turns it takes to knock them down.",
	"The Coliseum provides inspiring spectacles to defending troops, raising their morale by two during combat.",
	"The Rainbow increases the luck of the defending units by two.",
	"The Dungeon increases the income of the town by 500 / day.",
	"The Library increases the number of spells in the Guild by one for each level of the guild.",
	"The Storm adds +2 to the power of spells of a defending spell caster." };

    u8 offset = 0;

    switch(race)
    {
	case Race::KNGT: offset = 0; break;
	case Race::BARB: offset = 1; break;
	case Race::SORC: offset = 2; break;
	case Race::WRLK: offset = 3; break;
	case Race::WZRD: offset = 4; break;
	case Race::NECR: offset = 5; break;
	default: break;
    }

    switch(build)
    {
        case BUILD_THIEVESGUILD:return desc_build[0];
        case BUILD_TAVERN:	return desc_build[1];
        case BUILD_SHIPYARD:	return desc_build[2];
        case BUILD_WELL:	return desc_build[3];
        case BUILD_STATUE:	return desc_build[4];
        case BUILD_LEFTTURRET:	return desc_build[5];
        case BUILD_RIGHTTURRET:	return desc_build[6];
        case BUILD_MARKETPLACE:	return desc_build[7];
        case BUILD_MOAT:	return desc_build[8];
        case BUILD_CASTLE:	return desc_build[9];
        case BUILD_TENT:	return desc_build[10];
        case BUILD_CAPTAIN:	return desc_build[11];
        case BUILD_MAGEGUILD1:
        case BUILD_MAGEGUILD2:
        case BUILD_MAGEGUILD3:
        case BUILD_MAGEGUILD4:
        case BUILD_MAGEGUILD5:	return desc_build[12];

        case BUILD_SPEC:	return desc_spec[offset];
        case BUILD_WEL2:	return desc_wel2[offset];

	default: break;
    }

    return desc_build[13];
}

/* recrut monster from building to castle army */
bool Castle::RecrutMonster(building_t dw, u16 count)
{
    Monster::monster_t ms = Monster::UNKNOWN;

    u8 dw_index = 0;

    switch(dw)
    {
	case DWELLING_MONSTER1:
	    ms = Monster::Monster(race, DWELLING_MONSTER1);
	    dw_index = 0;
	    break;
	case DWELLING_MONSTER2:
	    ms = Monster::Monster(race, building & DWELLING_UPGRADE2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2);
	    dw_index = 1;
	    break;
	case DWELLING_MONSTER3:
	    ms = Monster::Monster(race, building & DWELLING_UPGRADE3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3);
	    dw_index = 2;
	    break;
	case DWELLING_MONSTER4:
	    ms = Monster::Monster(race, building & DWELLING_UPGRADE4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4);
	    dw_index = 3;
	    break;
	case DWELLING_MONSTER5:
	    ms = Monster::Monster(race, building & DWELLING_UPGRADE5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5);
	    dw_index = 4;
	    break;
	case DWELLING_MONSTER6:
	    ms = Monster::Monster(race, building & DWELLING_UPGRADE7 ? DWELLING_UPGRADE7 : (building & DWELLING_UPGRADE6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6));
	    dw_index = 5;
	    break;	
	default: return false;
    }

    // incorrect count
    if(dwelling[dw_index] < count) return false;

    // find free cell
    u8 num_cell = CASTLEMAXARMY;
    for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii)
	if(ms == army[ii].GetMonster() || 0 == army[ii].GetCount()){ num_cell = ii; break; }

    // not found
    if(CASTLEMAXARMY <= num_cell) return false;

    // buy
    const Resource::funds_t paymentCosts(PaymentConditions::BuyMonster(ms) * count);
    Kingdom & kingdom = const_cast<Kingdom &>(world.GetKingdom(color));
    
    if(paymentCosts > kingdom.GetFundsResource()) return false;

    kingdom.OddFundsResource(paymentCosts);
    
    army[num_cell].SetMonster(ms);
    army[num_cell].SetCount(army[num_cell].GetCount() + count);

    dwelling[dw_index] -= count;

    return true;
}

/* return current count monster in dwelling */
u16 Castle::GetDwellingLivedCount(building_t dw)
{
   switch(dw)
   {
      case DWELLING_MONSTER1: return dwelling[0];
      case DWELLING_MONSTER2:
      case DWELLING_UPGRADE2: return dwelling[1];
      case DWELLING_MONSTER3:
      case DWELLING_UPGRADE3: return dwelling[2];
      case DWELLING_MONSTER4:
      case DWELLING_UPGRADE4: return dwelling[3];
      case DWELLING_MONSTER5:
      case DWELLING_UPGRADE5: return dwelling[4];
      case DWELLING_MONSTER6:
      case DWELLING_UPGRADE6:
      case DWELLING_UPGRADE7: return dwelling[5];
      
      default: break;
    }

    return 0;                                                                 
}

/* return requires for building */
u32 Castle::GetBuildingRequires(const building_t & build) const
{
    u32 requires = 0;

    switch(build)
    {
	case DWELLING_MONSTER2:
	    switch(race)
	    {
		case Race::KNGT:
		case Race::BARB:
		case Race::WZRD:
		case Race::WRLK:
		case Race::NECR:
		    requires |= DWELLING_MONSTER1;
		    break;

		case Race::SORC:
		    requires |= DWELLING_MONSTER1;
		    requires |= BUILD_TAVERN;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_MONSTER3:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER1;
		    requires |= BUILD_WELL;
		    break;

		case Race::BARB:
		case Race::SORC:
		case Race::WZRD:
		case Race::WRLK:
		case Race::NECR:
		    requires |= DWELLING_MONSTER1;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_MONSTER4:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER1;
		    requires |= BUILD_TAVERN;
		    break;

		case Race::BARB:
		    requires |= DWELLING_MONSTER1;
			break;

		case Race::SORC:
		    requires |= DWELLING_MONSTER2;
		    requires |= BUILD_MAGEGUILD1;
		    break;

		case Race::WZRD:
		case Race::WRLK:
		    requires |= DWELLING_MONSTER2;
		    break;

		case Race::NECR:
		    requires |= DWELLING_MONSTER3;
		    requires |= BUILD_THIEVESGUILD;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_MONSTER5:
	    switch(race)
	    {
		case Race::KNGT:
		case Race::BARB:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::SORC:
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::WRLK:
		    requires |= DWELLING_MONSTER3;
		    break;

		case Race::WZRD:
		    requires |= DWELLING_MONSTER3;
		    requires |= BUILD_MAGEGUILD1;
		    break;

		case Race::NECR:
		    requires |= DWELLING_MONSTER2;
		    requires |= BUILD_MAGEGUILD1;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_MONSTER6:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::BARB:
		case Race::SORC:
		case Race::NECR:
		    requires |= DWELLING_MONSTER5;
		    break;

		case Race::WRLK:
		case Race::WZRD:
		    requires |= DWELLING_MONSTER4;
		    requires |= DWELLING_MONSTER5;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_UPGRADE2:
	    switch(race)
	    {
		case Race::KNGT:
		case Race::BARB:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::SORC:
		    requires |= DWELLING_MONSTER2;
		    requires |= BUILD_WELL;
		    break;

		case Race::NECR:
		    requires |= DWELLING_MONSTER2;
		    break;
		
		default:
		    break;
	    }
	    break;

	case DWELLING_UPGRADE3:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::SORC:
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::WZRD:
		    requires |= DWELLING_MONSTER3;
		    requires |= BUILD_WELL;
		    break;

		case Race::NECR:
		    requires |= DWELLING_MONSTER3;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_UPGRADE4:
	    switch(race)
	    {
		case Race::KNGT:
		case Race::BARB:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    break;

		case Race::SORC:
		case Race::WRLK:
		case Race::NECR:
		    requires |= DWELLING_MONSTER4;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_UPGRADE5:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    requires |= DWELLING_MONSTER5;
		    break;
		
		case Race::BARB:
		    requires |= DWELLING_MONSTER5;
		    break;

		case Race::WZRD:
		    requires |= BUILD_SPEC;
		    requires |= DWELLING_MONSTER5;
		    break;

		case Race::NECR:
		    requires |= BUILD_MAGEGUILD2;
		    requires |= DWELLING_MONSTER5;
		    break;

		default:
		    break;
	    }
	    break;

	case DWELLING_UPGRADE6:
	    switch(race)
	    {
		case Race::KNGT:
		    requires |= DWELLING_MONSTER2;
		    requires |= DWELLING_MONSTER3;
		    requires |= DWELLING_MONSTER4;
		    requires |= DWELLING_MONSTER6;
		    break;

		case Race::WRLK:
		case Race::WZRD:
		    requires |= DWELLING_MONSTER6;
		    break;
		
		default:
		    break;
	    }
	    break;

	default: break;
    }
    
    return requires;
}

/* check allow buy building */
bool Castle::AllowBuyBuilding(building_t build) const
{
    // check allow building
    if(!allow_build) return false;

    // check valid payment
    if(PaymentConditions::BuyBuilding(race, build) > world.GetMyKingdom().GetFundsResource()) return false;
    
    // check build requirements
    std::bitset<32> requires(Castle::GetBuildingRequires(build));
    
    if(requires.any())
    {
        for(u8 pos = 0; pos < requires.size(); ++pos)
        {
            if(requires.test(pos))
            {
                u32 value = 1;
                value <<= pos;

                if(! (building & value)) return false;
	    }
	}
    }

    return true;
}

/* buy building */
void Castle::BuyBuilding(building_t build)
{
	if(! AllowBuyBuilding(build)) return;
	
	const_cast<Kingdom &>(world.GetMyKingdom()).OddFundsResource(PaymentConditions::BuyBuilding(race, build));

	// add build
	building |= build;

	switch(build)
	{
	    case BUILD_CASTLE: TownUpgradeToCastle(); break;

	    case BUILD_MAGEGUILD1:
	    case BUILD_MAGEGUILD2:
	    case BUILD_MAGEGUILD3:
	    case BUILD_MAGEGUILD4:
	    case BUILD_MAGEGUILD5: mageguild.BuildNextLevel(); break;
            
            // build library
            case BUILD_SPEC: if(Race::WZRD == race) mageguild.UpgradeExt(); break;

	    case DWELLING_MONSTER1: dwelling[0] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER1)); break;
	    case DWELLING_MONSTER2: dwelling[1] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER2)); break;
	    case DWELLING_MONSTER3: dwelling[2] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER3)); break;
	    case DWELLING_MONSTER4: dwelling[3] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER4)); break;
	    case DWELLING_MONSTER5: dwelling[4] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER5)); break;
	    case DWELLING_MONSTER6: dwelling[5] = Monster::GetGrown(Monster::Monster(race, DWELLING_MONSTER6)); break;
	    default: break;
	}

	// disable day build
	allow_build = false;
	
	Error::Verbose("Castle::BuyBuilding: " + GetStringBuilding(build, race));
}

/* draw image castle to position */
void Castle::DrawImageCastle(const Point & pt)
{
    const Maps::Tiles & tile = world.GetTiles(mp.x, mp.y);
    
    u8 index = 0;
    Point dst_pt;

    // draw ground
    switch(tile.GetGround())
    {
	case Maps::Ground::GRASS:	index =  0; break;
	case Maps::Ground::SNOW:	index = 10; break;
	case Maps::Ground::SWAMP:	index = 20; break;
	case Maps::Ground::LAVA:	index = 30; break;
	case Maps::Ground::DESERT:	index = 40; break;
	case Maps::Ground::DIRT:	index = 50; break;
	case Maps::Ground::WASTELAND:	index = 60; break;
	case Maps::Ground::BEACH:	index = 70; break;

	default: return;
    }

    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN("OBJNTWBA.ICN", index + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 3 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }

    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN("OBJNTWBA.ICN", index + 5 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 4 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
    
    // draw castle
    switch(race)
    {
        case Race::KNGT: index = 0; break;
        case Race::BARB: index = 32; break;
	case Race::SORC: index = 64; break;
	case Race::WRLK: index = 96; break;
	case Race::WZRD: index = 128; break;
	case Race::NECR: index = 160; break;
	default: break;
    }
    if(! (BUILD_CASTLE & building)) index += 16;
    const Sprite & sprite2 = AGG::GetICN("OBJNTOWN.ICN", index);
    dst_pt.x = pt.x + 2 * 32 + sprite2.x();
    dst_pt.y = pt.y + sprite2.y();
    display.Blit(sprite2, dst_pt);
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN("OBJNTOWN.ICN", index + 1 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN("OBJNTOWN.ICN", index + 6 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 2 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN("OBJNTOWN.ICN", index + 11 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 3 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
}

/* prepare building name ICN string */
void Castle::PrepareICNString(const Castle::building_t & build, const Race::race_t & race, std::string & result)
{
    result = "TWN";

    switch(race)
    {
	case Race::KNGT: result += "K"; break;
	case Race::BARB: result += "B"; break;
	case Race::SORC: result += "S"; break;
	case Race::WRLK: result += "W"; break;
	case Race::WZRD: result += "Z"; break;
	case Race::NECR: result += "N"; break;
	default: result += "K"; break;
    }

    switch(build)
    {
	case BUILD_CASTLE:	result += "CSTL.ICN"; break;
	case BUILD_TENT:	result += "TENT.ICN"; break;
	case BUILD_SPEC:	result += "SPEC.ICN"; break;
	case BUILD_CAPTAIN:	result += "CAPT.ICN"; break;
	case BUILD_WEL2:	result += "WEL2.ICN"; break;
	case BUILD_LEFTTURRET:	result += "LTUR.ICN"; break;
	case BUILD_RIGHTTURRET:	result += "RTUR.ICN"; break;
	case BUILD_MOAT:	result += "MOAT.ICN"; break;
	case BUILD_MARKETPLACE:	result += "MARK.ICN"; break;
	case BUILD_THIEVESGUILD:result += "THIE.ICN"; break;
	case BUILD_TAVERN:	result += "TVRN.ICN"; break;
	case BUILD_WELL:	result += "WELL.ICN"; break;
	case BUILD_STATUE:	result += "STAT.ICN"; break;
	case BUILD_BOAT:	result += "BOAT.ICN"; break;
	case BUILD_SHIPYARD:	result += "DOCK.ICN"; break;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	result += "MAGE.ICN"; break;
	case DWELLING_MONSTER1:	result += "DW_0.ICN"; break;
	case DWELLING_MONSTER2:	result += "DW_1.ICN"; break;
	case DWELLING_UPGRADE2: result += "UP_1.ICN"; break;
	case DWELLING_MONSTER3:	result += "DW_2.ICN"; break;
	case DWELLING_UPGRADE3: result += "UP_2.ICN"; break;
	case DWELLING_MONSTER4:	result += "DW_3.ICN"; break;
	case DWELLING_UPGRADE4: result += "UP_3.ICN"; break;
	case DWELLING_MONSTER5:	result += "DW_4.ICN"; break;
	case DWELLING_UPGRADE5: result += "UP_4.ICN"; break;
	case DWELLING_MONSTER6:	result += "DW_5.ICN"; break;
	case DWELLING_UPGRADE6: result += "UP_5.ICN"; break;
	default: break;
    }
}

bool Castle::HaveNearlySea(void) const
{
    if(world.h() <= mp.y + 2) return false;

    const u16 index_c = (mp.y + 2 ) * world.h() + mp.x;

    const Maps::Tiles & tile_c = world.GetTiles(index_c);
    const Maps::Tiles & tile_l = world.GetTiles(index_c - 1);
    const Maps::Tiles & tile_r = world.GetTiles(index_c + 2);

    return Maps::Ground::WATER == tile_c.GetGround() || Maps::Ground::WATER == tile_l.GetGround() || Maps::Ground::WATER == tile_r.GetGround();
}
