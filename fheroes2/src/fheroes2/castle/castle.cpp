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
#include "display.h"
#include "sprite.h"
#include "castle.h"
#include "army.h"

Castle::Castle(s16 cx, s16 cy, const Race::race_t rc) : mp(cx, cy), race(rc), captain(*this),
    nearly_sea(3 > Maps::GetApproximateDistance(GetIndex(), world.GetNearestObject(GetIndex(), MP2::OBJ_COAST))),
    color(Color::GRAY), building(0), army_spread(true), allow_build(true), present_boat(false), mageguild(race),
    dwelling(CASTLEMAXMONSTER, 0), army(ARMYMAXTROOPS), castle_heroes(false)
{
    // set master primary skill to army
    Army::SetMasterSkill(army, captain);
}

void Castle::LoadFromMP2(const void *ptr)
{
    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;

    switch(*ptr8)
    {
	case 0x00: color = Color::BLUE;   break;
	case 0x01: color = Color::GREEN;  break;
        case 0x02: color = Color::RED;    break;
        case 0x03: color = Color::YELLOW; break;
        case 0x04: color = Color::ORANGE; break;
        case 0x05: color = Color::PURPLE; break;
        default:   color = Color::GRAY;   break;
    }
    ++ptr8;
    
    // custom building
    if(*ptr8)
    {
	++ptr8;
	
	// building
	LOAD16(ptr8, byte16);
        if(0x0002 & byte16) building |= BUILD_THIEVESGUILD;
        if(0x0004 & byte16) building |= BUILD_TAVERN;
        if(0x0008 & byte16) building |= BUILD_SHIPYARD;
        if(0x0010 & byte16) building |= BUILD_WELL;
        if(0x0080 & byte16) building |= BUILD_STATUE;
        if(0x0100 & byte16) building |= BUILD_LEFTTURRET;
        if(0x0200 & byte16) building |= BUILD_RIGHTTURRET;
        if(0x0400 & byte16) building |= BUILD_MARKETPLACE;
        if(0x1000 & byte16) building |= BUILD_MOAT;
        if(0x0800 & byte16) building |= BUILD_WEL2;
        if(0x2000 & byte16) building |= BUILD_SPEC;
	++ptr8;
	++ptr8;

	// dwelling
	LOAD16(ptr8, byte16);
        if(0x0008 & byte16) building |= DWELLING_MONSTER1;
        if(0x0010 & byte16) building |= DWELLING_MONSTER2;
        if(0x0020 & byte16) building |= DWELLING_MONSTER3;
        if(0x0040 & byte16) building |= DWELLING_MONSTER4;
        if(0x0080 & byte16) building |= DWELLING_MONSTER5;
        if(0x0100 & byte16) building |= DWELLING_MONSTER6;
        if(0x0200 & byte16) building |= DWELLING_UPGRADE2;
        if(0x0400 & byte16) building |= DWELLING_UPGRADE3;
        if(0x0800 & byte16) building |= DWELLING_UPGRADE4;
        if(0x1000 & byte16) building |= DWELLING_UPGRADE5;
        if(0x2000 & byte16) building |= DWELLING_UPGRADE6;
	++ptr8;
	++ptr8;

	
	// magic tower
	if(0 < *ptr8) building |= BUILD_MAGEGUILD1;
	if(1 < *ptr8) building |= BUILD_MAGEGUILD2;
	if(2 < *ptr8) building |= BUILD_MAGEGUILD3;
	if(3 < *ptr8) building |= BUILD_MAGEGUILD4;
	if(4 < *ptr8) building |= BUILD_MAGEGUILD5;
	++ptr8;
    }
    else
    {
	ptr8 += 6;

	// default building
        building |= BUILD_TAVERN;
        building |= DWELLING_MONSTER1;
        building |= DWELLING_MONSTER2;
	// default dwelling
	// default magic tower
    }

    // custom troops
    bool custom_troops = *ptr8;
    if(custom_troops)
    {
	++ptr8;
	
	// monster1
	army[0].SetMonster(Monster::Monster(*ptr8));
	++ptr8;

	// monster2
	army[1].SetMonster(Monster::Monster(*ptr8));
	++ptr8;

	// monster3
	army[2].SetMonster(Monster::Monster(*ptr8));
	++ptr8;

	// monster4
	army[3].SetMonster(Monster::Monster(*ptr8));
	++ptr8;

	// monster5
	army[4].SetMonster(Monster::Monster(*ptr8));
	++ptr8;

	// count1
	LOAD16(ptr8, byte16);
	army[0].SetCount(byte16);
	++ptr8;
	++ptr8;

	// count2
	LOAD16(ptr8, byte16);
	army[1].SetCount(byte16);
	++ptr8;
	++ptr8;

	// count3
	LOAD16(ptr8, byte16);
	army[2].SetCount(byte16);
	++ptr8;
	++ptr8;

	// count4
	LOAD16(ptr8, byte16);
	army[3].SetCount(byte16);
	++ptr8;
	++ptr8;

	// count5
	LOAD16(ptr8, byte16);
	army[4].SetCount(byte16);
	++ptr8;
	++ptr8;
    }
    else
    {
	ptr8 += 16;
    }
    
    // captain
    if(*ptr8) building |= BUILD_CAPTAIN;
    ++ptr8;
    
    // custom name
    ++ptr8;
    name = std::string(reinterpret_cast<const char *>(ptr8));
    ptr8 += 13;

    // race
    switch(*ptr8)
    { 	 
	case 0x00: race = Race::KNGT; break; 	 
	case 0x01: race = Race::BARB; break; 	 
        case 0x02: race = Race::SORC; break; 	 
	case 0x03: race = Race::WRLK; break; 	 
	case 0x04: race = Race::WZRD; break; 	 
        case 0x05: race = Race::NECR; break; 	 
        default: race = (Color::GRAY != color ? Settings::Get().FileInfo().KingdomRace(color) : Race::Rand()); break; 	 
    }
    ++ptr8;

    // castle
    if(*ptr8) building |= BUILD_CASTLE;
    ++ptr8;

    // allow upgrade to castle (0 - true, 1 - false)
    allow_castle = (*ptr8 ? false : true);
    ++ptr8;

    // unknown 29 byte
    //


    // troops auto pack
    if(!custom_troops)
    {
	const Monster::stats_t mon1 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));
	const Monster::stats_t mon2 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER2));

	switch(Settings::Get().GameDifficulty())
	{
    	    case Difficulty::EASY:
        	army[0].Set(mon1.monster, mon1.grown * 2);
        	army[1].Set(mon2.monster, mon2.grown * 2);
        	break;

    	    case Difficulty::NORMAL:
        	army[0].Set(mon1.monster, mon1.grown);
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

    // MageGuild
    if(building & BUILD_MAGEGUILD1) mageguild.BuildNextLevel();
    if(building & BUILD_MAGEGUILD2) mageguild.BuildNextLevel();
    if(building & BUILD_MAGEGUILD3) mageguild.BuildNextLevel();
    if(building & BUILD_MAGEGUILD4) mageguild.BuildNextLevel();
    if(building & BUILD_MAGEGUILD5) mageguild.BuildNextLevel();
    if(Race::WZRD == race && building & BUILD_SPEC) mageguild.UpgradeExt();

    // remove tavern from necromancer castle
    if(Race::NECR == race && !Settings::Get().Modes(Settings::PRICELOYALTY))
    	building &= ~BUILD_TAVERN;

    // end
    if(H2Config::Debug()) Error::Verbose((building & BUILD_CASTLE ? "Castle::LoadFromMP2: castle: " : "Castle::LoadFromMP2: town: ") + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

const Point & Castle::GetCenter(void) const
{ return mp; }

u16 Castle::GetIndex(void) const
{ return Maps::GetIndexFromAbsPoint(mp); }

bool Castle::ContainCoord(const u16 ax, const u16 ay) const
{
    return ((mp.x == ax && mp.y - 3 == ay) || (ax >= mp.x - 2 && ax <= mp.x + 2 && ay >= mp.y - 2 && ay <= mp.y + 1));
}

void Castle::ActionNewDay(void)
{
    // for learns new spells need today
    if(castle_heroes && GetLevelMageGuild()) (*castle_heroes).AppendSpellsToBook(mageguild);

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
    if(world.GetWeekType() == Week::PLAGUE)
    {
	std::vector<u16>::iterator it1 = dwelling.begin();
	std::vector<u16>::iterator it2 = dwelling.end();
	for(; it1 != it2; ++it1) if(*it1) (*it1) = (*it1) / 2;
    }
}

// change castle color
void Castle::ChangeColor(Color::color_t cl)
{
    color = cl;

    world.GetTiles(mp).CaptureFlags32(MP2::OBJ_CASTLE, color);
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

    static const std::string str_wel2[] = { "Farm", "Garbage Heap", "Crystal Garden", "Waterfall", "Orchard", "Skull Pile" };

    static const std::string str_spec[] = { "Fortifications", "Coliseum", "Rainbow", "Dungeon", "Library", "Storm" };

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
	"", "", "", "Black Tower", "", "" };

    static const std::string shrine = "Shrine";

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

    if(Settings::Get().Modes(Settings::PRICELOYALTY) && race == Race::NECR && build == BUILD_TAVERN) return shrine;

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

    static const std::string shrine_descr = "The Shrine increases the necromancy skill of all your necromancers by 10 percent.";

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

    if(Settings::Get().Modes(Settings::PRICELOYALTY) && race == Race::NECR && build == BUILD_TAVERN) return shrine_descr;
    
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

bool Castle::AllowBuyHero(void)
{
    if(castle_heroes) return false;

    const Kingdom & kingdom = world.GetKingdom(color);

    if(kingdom.GetHeroes().size() >= KINGDOMMAXHEROES) return false;

    const Resource::funds_t paymentCosts(Resource::GOLD, RECRUIT_HEROES_GOLD);

    if(! kingdom.AllowPayment(paymentCosts)) return false;

    return true;
}

void Castle::RecruitHero(const Heroes::heroes_t hero)
{
    if(! AllowBuyHero()) return;

    Kingdom & kingdom = world.GetKingdom(color);

    const Resource::funds_t paymentCosts(Resource::GOLD, RECRUIT_HEROES_GOLD);

    kingdom.OddFundsResource(paymentCosts);

    Heroes & heroes = world.GetHeroes(hero);

    heroes.SetCenter(GetCenter());
    heroes.Recruit(*this);

    kingdom.AddHeroes(&heroes);
    
    castle_heroes = &heroes;
}

/* recruit monster from building to castle army */
bool Castle::RecruitMonster(building_t dw, u16 count)
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
    u8 num_cell = ARMYMAXTROOPS;
    for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	if(ms == army[ii].Monster() || 0 == army[ii].Count()){ num_cell = ii; break; }

    // not found
    if(ARMYMAXTROOPS <= num_cell) return false;

    // buy
    const Resource::funds_t paymentCosts(PaymentConditions::BuyMonster(ms) * count);
    Kingdom & kingdom = world.GetKingdom(color);
    
    if(! kingdom.AllowPayment(paymentCosts)) return false;

    kingdom.OddFundsResource(paymentCosts);
    
    army[num_cell].Set(ms, army[num_cell].Count() + count);

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
	case BUILD_SPEC:
	    switch(race)
	    {
		case Race::WZRD:
		    requires |= BUILD_MAGEGUILD1;
		    break;

		default:
	        break;
	    }
	    break;

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
    if(! world.GetMyKingdom().AllowPayment(PaymentConditions::BuyBuilding(race, build))) return false;

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

    // other requires
    switch(build)
    {
	// buid shipyard only nearly sea
	case BUILD_SHIPYARD: if(! HaveNearlySea()) return false;
	
	default: break;
    }

    return true;
}

/* buy building */
void Castle::BuyBuilding(building_t build)
{
	if(! AllowBuyBuilding(build)) return;
	
	world.GetMyKingdom().OddFundsResource(PaymentConditions::BuyBuilding(race, build));

	// add build
	building |= build;

	switch(build)
	{
	    case BUILD_CASTLE:
		Maps::UpdateSpritesFromTownToCastle(GetCenter());
		break;

	    case BUILD_MAGEGUILD1:
	    case BUILD_MAGEGUILD2:
	    case BUILD_MAGEGUILD3:
	    case BUILD_MAGEGUILD4:
	    case BUILD_MAGEGUILD5: mageguild.BuildNextLevel();
		break;

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
	
	if(H2Config::Debug()) Error::Verbose("Castle::BuyBuilding: " + GetStringBuilding(build, race));
}

/* draw image castle to position */
void Castle::DrawImageCastle(const Point & pt)
{
    const Maps::Tiles & tile = world.GetTiles(mp.x, mp.y);
    Display & display = Display::Get();

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
	const Sprite & sprite = AGG::GetICN(ICN::OBJNTWBA, index + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 3 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }

    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN(ICN::OBJNTWBA, index + 5 + ii);
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
    const Sprite & sprite2 = AGG::GetICN(ICN::OBJNTOWN, index);
    dst_pt.x = pt.x + 2 * 32 + sprite2.x();
    dst_pt.y = pt.y + sprite2.y();
    display.Blit(sprite2, dst_pt);
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN(ICN::OBJNTOWN, index + 1 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN(ICN::OBJNTOWN, index + 6 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 2 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
    for(int ii = 0; ii < 5; ++ii)
    {
	const Sprite & sprite = AGG::GetICN(ICN::OBJNTOWN, index + 11 + ii);
        dst_pt.x = pt.x + ii * 32 + sprite.x();
	dst_pt.y = pt.y + 3 * 32 + sprite.y();
	display.Blit(sprite, dst_pt);
    }
}

ICN::icn_t Castle::GetICNBoat(const Race::race_t & race)
{
    switch(race)
    {
	case Race::BARB:	return ICN::TWNBBOAT;
	case Race::KNGT:	return ICN::TWNKBOAT;
	case Race::NECR:	return ICN::TWNNBOAT;
	case Race::SORC:	return ICN::TWNSBOAT;
	case Race::WRLK:	return ICN::TWNWBOAT;
	case Race::WZRD:	return ICN::TWNZBOAT;
	default: break;
    }

    Error::Warning("Castle::GetICNBoat: return unknown");
    return ICN::UNKNOWN;
}

/* get building name ICN */
ICN::icn_t Castle::GetICNBuilding(const Castle::building_t & build, const Race::race_t & race)
{
    if(Race::BARB == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNBCSTL;
	case BUILD_TENT:	return ICN::TWNBTENT;
	case BUILD_SPEC:	return ICN::TWNBSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNBCAPT;
	case BUILD_WEL2:	return ICN::TWNBWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNBLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNBRTUR;
	case BUILD_MOAT:	return ICN::TWNBMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNBMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNBTHIE;
	case BUILD_TAVERN:	return ICN::TWNBTVRN;
	case BUILD_WELL:	return ICN::TWNBWELL;
	case BUILD_STATUE:	return ICN::TWNBSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNBDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNBMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNBDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNBDW_1;
	case DWELLING_UPGRADE2: return ICN::TWNBUP_1;
	case DWELLING_MONSTER3:	return ICN::TWNBDW_2;
	case DWELLING_MONSTER4:	return ICN::TWNBDW_3;
	case DWELLING_UPGRADE4: return ICN::TWNBUP_3;
	case DWELLING_MONSTER5:	return ICN::TWNBDW_4;
	case DWELLING_UPGRADE5: return ICN::TWNBUP_4;
	case DWELLING_MONSTER6:	return ICN::TWNBDW_5;
	default: break;
	}
    }
    else
    if(Race::KNGT == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNKCSTL;
	case BUILD_TENT:	return ICN::TWNKTENT;
	case BUILD_SPEC:	return ICN::TWNKSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNKCAPT;
	case BUILD_WEL2:	return ICN::TWNKWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNKLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNKRTUR;
	case BUILD_MOAT:	return ICN::TWNKMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNKMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNKTHIE;
	case BUILD_TAVERN:	return ICN::TWNKTVRN;
	case BUILD_WELL:	return ICN::TWNKWELL;
	case BUILD_STATUE:	return ICN::TWNKSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNKDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNKMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNKDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNKDW_1;
	case DWELLING_UPGRADE2: return ICN::TWNKUP_1;
	case DWELLING_MONSTER3:	return ICN::TWNKDW_2;
	case DWELLING_UPGRADE3: return ICN::TWNKUP_2;
	case DWELLING_MONSTER4:	return ICN::TWNKDW_3;
	case DWELLING_UPGRADE4: return ICN::TWNKUP_3;
	case DWELLING_MONSTER5:	return ICN::TWNKDW_4;
	case DWELLING_UPGRADE5: return ICN::TWNKUP_4;
	case DWELLING_MONSTER6:	return ICN::TWNKDW_5;
	case DWELLING_UPGRADE6: return ICN::TWNKUP_5;
	default: break;
	}
    }
    else
    if(Race::NECR == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNNCSTL;
	case BUILD_TENT:	return ICN::TWNNTENT;
	case BUILD_SPEC:	return ICN::TWNNSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNNCAPT;
	case BUILD_WEL2:	return ICN::TWNNWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNNLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNNRTUR;
	case BUILD_MOAT:	return ICN::TWNNMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNNMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNNTHIE;
	// shrine
	case BUILD_TAVERN:	if(Settings::Get().Modes(Settings::PRICELOYALTY)) return ICN::TWNNTVRN;
				break;
	case BUILD_WELL:	return ICN::TWNNWELL;
	case BUILD_STATUE:	return ICN::TWNNSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNNDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNNMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNNDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNNDW_1;
	case DWELLING_UPGRADE2: return ICN::TWNNUP_1;
	case DWELLING_MONSTER3:	return ICN::TWNNDW_2;
	case DWELLING_UPGRADE3: return ICN::TWNNUP_2;
	case DWELLING_MONSTER4:	return ICN::TWNNDW_3;
	case DWELLING_UPGRADE4: return ICN::TWNNUP_3;
	case DWELLING_MONSTER5:	return ICN::TWNNDW_4;
	case DWELLING_UPGRADE5: return ICN::TWNNUP_4;
	case DWELLING_MONSTER6:	return ICN::TWNNDW_5;
	default: break;
	}
    }
    else
    if(Race::SORC == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNSCSTL;
	case BUILD_TENT:	return ICN::TWNSTENT;
	case BUILD_SPEC:	return ICN::TWNSSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNSCAPT;
	case BUILD_WEL2:	return ICN::TWNSWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNSLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNSRTUR;
	case BUILD_MOAT:	return ICN::TWNSMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNSMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNSTHIE;
	case BUILD_TAVERN:	return ICN::TWNSTVRN;
	case BUILD_WELL:	return ICN::TWNSWELL;
	case BUILD_STATUE:	return ICN::TWNSSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNSDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNSMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNSDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNSDW_1;
	case DWELLING_UPGRADE2: return ICN::TWNSUP_1;
	case DWELLING_MONSTER3:	return ICN::TWNSDW_2;
	case DWELLING_UPGRADE3: return ICN::TWNSUP_2;
	case DWELLING_MONSTER4:	return ICN::TWNSDW_3;
	case DWELLING_UPGRADE4: return ICN::TWNSUP_3;
	case DWELLING_MONSTER5:	return ICN::TWNSDW_4;
	case DWELLING_MONSTER6:	return ICN::TWNSDW_5;
	default: break;
	}
    }
    else
    if(Race::WRLK == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNWCSTL;
	case BUILD_TENT:	return ICN::TWNWTENT;
	case BUILD_SPEC:	return ICN::TWNWSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNWCAPT;
	case BUILD_WEL2:	return ICN::TWNWWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNWLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNWRTUR;
	case BUILD_MOAT:	return ICN::TWNWMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNWMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNWTHIE;
	case BUILD_TAVERN:	return ICN::TWNWTVRN;
	case BUILD_WELL:	return ICN::TWNWWELL;
	case BUILD_STATUE:	return ICN::TWNWSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNWDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNWMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNWDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNWDW_1;
	case DWELLING_MONSTER3:	return ICN::TWNWDW_2;
	case DWELLING_MONSTER4:	return ICN::TWNWDW_3;
	case DWELLING_UPGRADE4: return ICN::TWNWUP_3;
	case DWELLING_MONSTER5:	return ICN::TWNWDW_4;
	case DWELLING_MONSTER6:	return ICN::TWNWDW_5;
	case DWELLING_UPGRADE6: return ICN::TWNWUP_5;
	case DWELLING_UPGRADE7: return ICN::TWNWUP5B;
	default: break;
	}
    }
    else
    if(Race::WZRD == race)
    {
	switch(build)
	{
	case BUILD_CASTLE:	return ICN::TWNZCSTL;
	case BUILD_TENT:	return ICN::TWNZTENT;
	case BUILD_SPEC:	return ICN::TWNZSPEC;
	case BUILD_CAPTAIN:	return ICN::TWNZCAPT;
	case BUILD_WEL2:	return ICN::TWNZWEL2;
	case BUILD_LEFTTURRET:	return ICN::TWNZLTUR;
	case BUILD_RIGHTTURRET:	return ICN::TWNZRTUR;
	case BUILD_MOAT:	return ICN::TWNZMOAT;
	case BUILD_MARKETPLACE:	return ICN::TWNZMARK;
	case BUILD_THIEVESGUILD:return ICN::TWNZTHIE;
	case BUILD_TAVERN:	return ICN::TWNZTVRN;
	case BUILD_WELL:	return ICN::TWNZWELL;
	case BUILD_STATUE:	return ICN::TWNZSTAT;
	case BUILD_SHIPYARD:	return ICN::TWNZDOCK;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return ICN::TWNZMAGE;
	case DWELLING_MONSTER1:	return ICN::TWNZDW_0;
	case DWELLING_MONSTER2:	return ICN::TWNZDW_1;
	case DWELLING_MONSTER3:	return ICN::TWNZDW_2;
	case DWELLING_UPGRADE3: return ICN::TWNZUP_2;
	case DWELLING_MONSTER4:	return ICN::TWNZDW_3;
	case DWELLING_MONSTER5:	return ICN::TWNZDW_4;
	case DWELLING_UPGRADE5: return ICN::TWNZUP_4;
	case DWELLING_MONSTER6:	return ICN::TWNZDW_5;
	case DWELLING_UPGRADE6: return ICN::TWNZUP_5;
	default: break;
	}
    }

    Error::Warning("Castle::GetICNBuilding: return unknown, " + Castle::GetStringBuilding(build, race));

    return ICN::UNKNOWN;
}

const Heroes* Castle::GetHeroes(void) const
{
    return world.GetHeroes(mp.x, mp.y);
}

bool Castle::HaveNearlySea(void) const
{
    return nearly_sea;
}

bool Castle::PresentBoat(void) const
{
    return 3 > Maps::GetApproximateDistance(GetIndex(), world.GetNearestObject(GetIndex(), MP2::OBJ_BOAT));
}

u32 Castle::GetUpgradeBuilding(const u32 build, const Race::race_t & race)
{
    switch(build)
    {
	case BUILD_TENT:	return BUILD_CASTLE;
	case BUILD_MAGEGUILD1:	return BUILD_MAGEGUILD2;
	case BUILD_MAGEGUILD2:	return BUILD_MAGEGUILD3;
	case BUILD_MAGEGUILD3:	return BUILD_MAGEGUILD4;
	case BUILD_MAGEGUILD4:	return BUILD_MAGEGUILD5;
	default: break;
    }

    if(Race::BARB == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER2:	return DWELLING_UPGRADE2;
	    case DWELLING_MONSTER4:	return DWELLING_UPGRADE4;
	    case DWELLING_MONSTER5:	return DWELLING_UPGRADE5;
	    default: break;
	}
    }
    else
    if(Race::KNGT == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER2:	return DWELLING_UPGRADE2;
	    case DWELLING_MONSTER3:	return DWELLING_UPGRADE3;
	    case DWELLING_MONSTER4:	return DWELLING_UPGRADE4;
	    case DWELLING_MONSTER5:	return DWELLING_UPGRADE5;
	    case DWELLING_MONSTER6:	return DWELLING_UPGRADE6;
	    default: break;
	}
    }
    else
    if(Race::NECR == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER2:	return DWELLING_UPGRADE2;
	    case DWELLING_MONSTER3:	return DWELLING_UPGRADE3;
	    case DWELLING_MONSTER4:	return DWELLING_UPGRADE4;
	    case DWELLING_MONSTER5:	return DWELLING_UPGRADE5;
	    default: break;
	}
    }
    else
    if(Race::SORC == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER2:	return DWELLING_UPGRADE2;
	    case DWELLING_MONSTER3:	return DWELLING_UPGRADE3;
	    case DWELLING_MONSTER4:	return DWELLING_UPGRADE4;
	    default: break;
	}
    }
    else
    if(Race::WRLK == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER4:	return DWELLING_UPGRADE4;
	    case DWELLING_MONSTER6:	return DWELLING_UPGRADE6;
	    case DWELLING_UPGRADE6: return DWELLING_UPGRADE7;
	    default: break;
	}
    }
    else
    if(Race::WZRD == race)
    {
	switch(build)
	{
	    case DWELLING_MONSTER3:	return DWELLING_UPGRADE3;
	    case DWELLING_MONSTER5:	return DWELLING_UPGRADE5;
	    case DWELLING_MONSTER6:	return DWELLING_UPGRADE6;
	    default: break;
	}
    }

    return build;
}

bool Castle::PredicateIsCastle(const Castle* castle)
{
    return castle && castle->isCastle();
}

bool Castle::PredicateIsTown(const Castle* castle)
{
    return castle && !castle->isCastle();
}

bool Castle::PredicateIsBuildMarketplace(const Castle* castle)
{
    return castle && castle->isBuild(Castle::BUILD_MARKETPLACE);
}

void Castle::Dump(void) const
{
    std::cout << "----------------I--------" << std::endl;
    std::cout << "name            : " << name << std::endl;
    std::cout << "race            : " << Race::String(race) << std::endl;
    std::cout << "color           : " << Color::String(color) << std::endl;
    std::cout << "build           : " << building << std::endl;
    std::cout << "present heroes  : " << (GetHeroes() ? "yes" : "no") << std::endl;
    std::cout << "present boat    : " << (PresentBoat() ? "yes" : "no") << std::endl;
    std::cout << "nearly sea      : " << (nearly_sea ? "yes" : "no") << std::endl;
}

s8 Castle::GetMoraleWithModificators(std::list<std::string> *list) const
{
    s8 result(Morale::NORMAL);

    // and tavern
    if(Race::NECR != race && isBuild(BUILD_TAVERN))
    {
        result += 1;
        if(list) list->push_back(GetStringBuilding(BUILD_TAVERN, race) + "+1");
    }

    // and barbarian coliseum
    if(Race::BARB == race && isBuild(BUILD_SPEC))
    {
        result += 2;
        if(list) list->push_back(GetStringBuilding(BUILD_SPEC, race) + "+2");
    }

    return result;
}

s8 Castle::GetLuckWithModificators(std::list<std::string> *list) const
{
    s8 result(Luck::NORMAL);

    if(Race::SORC == race && isBuild(BUILD_SPEC))
    {
        result += 2;
        if(list) list->push_back(Castle::GetStringBuilding(BUILD_SPEC, race) + "+2");
    }

    return result;
}
