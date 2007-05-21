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
#include "agg.h"
#include "rand.h"
#include "army.h"
#include "castle.h"
#include "error.h"
#include "game.h"
#include "gamearea.h"
#include "maps.h"
#include "monster.h"

namespace Monster {

    static const stats_t all_monsters[] = {
	// monster            level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ PEASANT,          Race::KNGT, LEVEL1,   1,   1,   0,   1,   1,   1,  Speed::VERYSLOW,    2,  12, "Peasant"        , "PEASANT.ICN" },
	{ ARCHER,           Race::KNGT, LEVEL1,   5,   3,  12,   2,   3,  10,  Speed::VERYSLOW,  150,   8, "Archer"         , "ARCHER.ICN" },
	{ RANGER,           Race::KNGT, LEVEL2,   5,   3,  24,   2,   3,  10,   Speed::AVERAGE,  200,   8, "Ranger"         , "ARCHER2.ICN" },
	{ PIKEMAN,          Race::KNGT, LEVEL2,   5,   9,   0,   3,   4,  15,   Speed::AVERAGE,  200,   5, "Pikeman"        , "PIKEMAN.ICN" },
	{ VETERAN_PIKEMAN,  Race::KNGT, LEVEL2,   5,   9,   0,   3,   4,  20,      Speed::FAST,  250,   5, "Veteran Pikeman", "PIKEMAN2.ICN" },
	{ SWORDSMAN,        Race::KNGT, LEVEL3,   7,   9,   0,   4,   6,  25,   Speed::AVERAGE,  250,   4, "Swordsman"      , "SWORDSMN.ICN" },
	{ MASTER_SWORDSMAN, Race::KNGT, LEVEL3,   7,   9,   0,   4,   6,  30,      Speed::FAST,  300,   4, "Master Swordman", "SWORDSM2.ICN" },
	{ CAVALRY,          Race::KNGT, LEVEL3,  10,   9,   0,   5,   9,  30,  Speed::VERYFAST,  300,   3, "Cavalry"        , "CAVALRYR.ICN" },
	{ CHAMPION,         Race::KNGT, LEVEL3,  10,   9,   0,   5,  10,  40, Speed::ULTRAFAST,  375,   3, "Champion"       , "CAVALRYB.ICN" },
	{ PALADIN,          Race::KNGT, LEVEL4,  11,  12,   0,  10,  20,  50,      Speed::FAST,  600,   2, "Paladin"        , "PALADIN.ICN" },
	{ CRUSADER,         Race::KNGT, LEVEL4,  11,  12,   0,  10,  20,  65,  Speed::VERYFAST, 1000,   2, "Crusader"       , "PALADIN2.ICN" },

	// monster     level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ GOBLIN,    Race::BARB, LEVEL1,   3,   1,   0,   1,   2,   3,   Speed::AVERAGE,   40,  10, "Goblin"         , "GOBLIN.ICN" },
	{ ORC,       Race::BARB, LEVEL1,   3,   4,   8,   2,   3,  10,  Speed::VERYSLOW,  140,   8, "Orc"            , "ORC.ICN" },
	{ CHIEF_ORC, Race::BARB, LEVEL2,   3,   4,  16,   3,   4,  15,      Speed::SLOW,  175,   8, "Orc Chief"      , "ORC2.ICN" },
	{ WOLF,      Race::BARB, LEVEL2,   6,   2,   0,   3,   5,  20,  Speed::VERYFAST,  200,   5, "Wolf"           , "WOLF.ICN" },
	{ OGRE,      Race::BARB, LEVEL3,   9,   5,   0,   4,   6,  40,  Speed::VERYSLOW,  300,   4, "Ogre"           , "OGRE.ICN" },
	{ LORD_OGRE, Race::BARB, LEVEL3,   9,   5,   0,   5,   7,  60,   Speed::AVERAGE,  500,   4, "Ogre Lord"      , "OGRE2.ICN" },
	{ TROLL,     Race::BARB, LEVEL3,  10,   5,   8,   5,   7,  40,   Speed::AVERAGE,  600,   3, "Troll"          , "TROLL.ICN" },
	{ WAR_TROLL, Race::BARB, LEVEL3,  10,   5,  16,   7,   9,  40,      Speed::FAST,  700,   3, "War Troll"      , "TROLL2.ICN" },
	{ CYCLOPS,   Race::BARB, LEVEL4,  12,   9,   0,  12,  24,  80,      Speed::FAST,  750,   2, "Cyclops"        , "CYCLOPS.ICN" },

	// monster         level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ SPRITE,        Race::SORC, LEVEL1,   4,   2,   0,   1,   2,   2,   Speed::AVERAGE,   50,   8, "Sprite"         , "SPRITE.ICN" },
	{ DWARF,         Race::SORC, LEVEL2,   6,   5,   0,   2,   4,  20,  Speed::VERYSLOW,  200,   6, "Dwarf"          , "DWARF.ICN" },
	{ BATTLE_DWARF,  Race::SORC, LEVEL2,   6,   6,   0,   2,   4,  20,   Speed::AVERAGE,  250,   6, "Battle Dwarf"   , "DWARF2.ICN" },
	{ ELF,           Race::SORC, LEVEL2,   4,   3,  24,   2,   3,  15,   Speed::AVERAGE,  250,   4, "Elf"            , "ELF.ICN" },
	{ GRAND_ELF,     Race::SORC, LEVEL2,   5,   5,  24,   2,   3,  15,  Speed::VERYFAST,  300,   4, "Grand Elf"      , "ELF2.ICN" },
	{ DRUID,         Race::SORC, LEVEL3,   7,   5,   8,   5,   8,  25,      Speed::FAST,  350,   3, "Druid"          , "DRUID.ICN" },
	{ GREATER_DRUID, Race::SORC, LEVEL3,   7,   7,  16,   5,   8,  25,  Speed::VERYFAST,  400,   3, "Greater Druid"  , "DRUID2.ICN" },
	{ UNICORN,       Race::SORC, LEVEL4,  10,   9,   0,   7,  14,  40,      Speed::FAST,  500,   2, "Unicorn"        , "UNICORN.ICN" },
	{ PHOENIX,       Race::SORC, LEVEL4,  12,  10,   0,  20,  40, 100, Speed::ULTRAFAST, 1500,   1, "Phoenix"        , "PHOENIX.ICN" },

	// monster           level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ CENTAUR,         Race::WRLK, LEVEL1,   3,   1,   8,   1,   2,   5,   Speed::AVERAGE,   60,   8, "Centaur"        , "CENTAUR.ICN" },
	{ GARGOYLE,        Race::WRLK, LEVEL2,   4,   7,   0,   2,   3,  15,  Speed::VERYFAST,  200,   6, "Gargoyle"       , "GARGOYLE.ICN" },
	{ GRIFFIN,         Race::WRLK, LEVEL3,   6,   6,   0,   3,   5,  25,   Speed::AVERAGE,  300,   4, "Griffin"        , "GRIFFIN.ICN" },
	{ MINOTAUR,        Race::WRLK, LEVEL3,   9,   8,   0,   5,  10,  35,   Speed::AVERAGE,  400,   3, "Minotaur"       , "MINOTAUR.ICN" },
	{ KNIGHT_MINOTAUR, Race::WRLK, LEVEL3,   9,   8,   0,   5,  10,  45,  Speed::VERYFAST,  500,   3, "Minotaur King"  , "MINOTAU2.ICN" },
	{ HYDRA,           Race::WRLK, LEVEL4,   8,   9,   0,   6,  12,  75,  Speed::VERYSLOW,  800,   2, "Hydra"          , "HYDRA.ICN" },
	{ GREEN_DRAGON,    Race::WRLK, LEVEL4,  12,  12,   0,  25,  50, 200,   Speed::AVERAGE, 3000,   1, "Green Dragon"   , "DRAGGREE.ICN" },
	{ RED_DRAGON,      Race::WRLK, LEVEL4,  13,  13,   0,  25,  50, 250,      Speed::FAST, 3500,   1, "Red Dragon"     , "DRAGRED.ICN" },
	{ BLACK_DRAGON,    Race::WRLK, LEVEL4,  14,  14,   0,  25,  50, 300,  Speed::VERYFAST, 4000,   1, "Black Dragon"   , "DRAGBLAK.ICN" },

	// monster       level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ HALFLING,    Race::WZRD, LEVEL1,   2,   1,  12,   1,   3,   3,      Speed::SLOW,   50,   8, "Halfling"       , "HALFLING.ICN" },
	{ BOAR,        Race::WZRD, LEVEL2,   5,   4,   0,   2,   3,  15,  Speed::VERYFAST,  150,   6, "Boar"           , "BOAR.ICN" },
	{ IRON_GOLEM,  Race::WZRD, LEVEL2,   5,  10,   0,   4,   5,  30,  Speed::VERYSLOW,  300,   4, "Iron Golem"     , "GOLEM.ICN" },
	{ STEEL_GOLEM, Race::WZRD, LEVEL3,   7,  10,   0,   4,   5,  35,      Speed::SLOW,  350,   4, "Steel Golem"    , "GOLEM2.ICN" },
	{ ROC,         Race::WZRD, LEVEL3,   7,   7,   0,   4,   8,  40,   Speed::AVERAGE,  400,   3, "Roc"            , "ROC.ICN" },
	{ MAGE,        Race::WZRD, LEVEL3,  11,   7,  12,   7,   9,  30,      Speed::FAST,  600,   2, "Mage"           , "MAGE1.ICN" },
	{ ARCHMAGE,    Race::WZRD, LEVEL3,  12,   8,  24,   7,   9,  35,  Speed::VERYFAST,  700,   2, "Archmage"       , "MAGE2.ICN" },
	{ GIANT,       Race::WZRD, LEVEL4,  13,  10,   0,  20,  30, 150,   Speed::AVERAGE, 2000,   1, "Giant"          , "TITANBLU.ICN" },
	{ TITAN,       Race::WZRD, LEVEL4,  15,  15,  24,  20,  30, 300,  Speed::VERYFAST, 5000,   1, "Titan"          , "TITANBLA.ICN" },

	// monster         level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ SKELETON,      Race::NECR, LEVEL1,   4,   3,   0,   2,   3,   4,   Speed::AVERAGE,   75,   8, "Skeleton"       , "SKELETON.ICN" },
	{ ZOMBIE,        Race::NECR, LEVEL1,   5,   2,   0,   2,   3,  15,  Speed::VERYSLOW,  150,   6, "Zombie"         , "ZOMBIE.ICN" },
	{ MUTANT_ZOMBIE, Race::NECR, LEVEL2,   5,   2,   0,   2,   3,  25,   Speed::AVERAGE,  200,   6, "Mutant Zombie"  , "ZOMBIE2.ICN" },
	{ MUMMY,         Race::NECR, LEVEL2,   6,   6,   0,   3,   4,  25,   Speed::AVERAGE,  250,   4, "Mummy"          , "MUMMYW.ICN" },
	{ ROYAL_MUMMY,   Race::NECR, LEVEL3,   6,   6,   0,   3,   4,  30,      Speed::FAST,  300,   4, "Royal Mummy"    , "MUMMY2.ICN" },
	{ VAMPIRE,       Race::NECR, LEVEL3,   8,   6,   0,   5,   7,  30,   Speed::AVERAGE,  500,   3, "Vampire"        , "VAMPIRE.ICN" },
	{ LORD_VAMPIRE,  Race::NECR, LEVEL3,   8,   6,   0,   5,   7,  40,      Speed::FAST,  650,   3, "Lord Vampire"   , "VAMPIRE2.ICN" },
	{ LICH,          Race::NECR, LEVEL3,   7,  12,  12,   8,  10,  25,      Speed::FAST,  750,   2, "Lich"           , "LICH.ICN" },
	{ POWER_LICH,    Race::NECR, LEVEL4,   7,  13,  24,   8,  10,  35,  Speed::VERYFAST,  900,   2, "Power Lich"     , "LICH2.ICN" },
	{ BONE_DRAGON,   Race::NECR, LEVEL4,  11,   9,   0,  25,  45, 150,   Speed::AVERAGE, 1500,   1, "Bone Dragon"    , "DRAGBONE.ICN" },

	// monster         level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	{ ROGUE,         Race::BOMG, LEVEL1,  6,   1,   0,   1,   2,   4,      Speed::FAST,   50,   4, "Rogue"          , "ROGUE.ICN" },
	{ NOMAD,         Race::BOMG, LEVEL2,  7,   6,   0,   2,   5,  20,  Speed::VERYFAST,  200,   4, "Nomad"          , "NOMAD.ICN" },
	{ GHOST,         Race::BOMG, LEVEL3,  8,   7,   0,   4,   6,  20,      Speed::FAST, 1000,   4, "Ghost"          , "GHOST.ICN" },
	{ MEDUSA,        Race::BOMG, LEVEL3,  8,   9,   0,   6,  10,  35,   Speed::AVERAGE,  500,   4, "Medusa"         , "MEDUSA.ICN" },
	{ GENIE,         Race::BOMG, LEVEL4, 10,   9,   0,  20,  30,  50,  Speed::VERYFAST,  650,   4, "Genie"          , "GENIE.ICN" },
	{ EARTH_ELEMENT, Race::BOMG, LEVEL3,  8,   8,   0,   4,   5,  50,      Speed::SLOW,  500,   4, "Earth Element"  , "EELEM.ICN" },
	{ AIR_ELEMENT,   Race::BOMG, LEVEL3,  7,   7,   0,   2,   8,  35,  Speed::VERYFAST,  500,   4, "Air Element"    , "AELEM.ICN" },
	{ FIRE_ELEMENT,  Race::BOMG, LEVEL3,  8,   6,   0,   4,   6,  40,      Speed::FAST,  500,   4, "Fire Element"   , "FELEM.ICN" },
	{ WATER_ELEMENT, Race::BOMG, LEVEL3,  6,   8,   0,   3,   7,  45,   Speed::AVERAGE,  500,   4, "Water Element"  , "WELEM.ICN" },
	
	// unknown
	{ UNKNOWN,       Race::BOMG, LEVEL1,  0,   0,   0,   0,   0,   0,  Speed::VERYSLOW, 9999,   0, "Unknown"       , "UNKNOWN.ICN" }
    };

}

Monster::monster_t Monster::Monster(u8 num)
{
    return UNKNOWN > num ? all_monsters[num].monster : UNKNOWN;
}

/* get stats monster */
const Monster::stats_t & Monster::GetStats(monster_t monster)
{ return (WATER_ELEMENT < monster ? all_monsters[WATER_ELEMENT + 1] : all_monsters[monster]); }

/* get string name */
const std::string & Monster::String(monster_t monster)
{ return Monster::GetStats(monster).name; }

/* get rnd monster */
Monster::monster_t Monster::Rand(void)
{ return static_cast<Monster::monster_t>(Rand::Get(Monster::PEASANT, Monster::WATER_ELEMENT)); }

/* get rnd monster level 1 */
Monster::monster_t Monster::Rand1(void)
{
    switch(Rand::Get(1, 10)){
	case 1:  return Monster::PEASANT;	break;
	case 2:  return Monster::ARCHER;	break;
	case 3:  return Monster::GOBLIN;	break;
	case 4:  return Monster::ORC;		break;
	case 5:  return Monster::SPRITE;	break;
	case 6:  return Monster::CENTAUR;	break;
	case 7:  return Monster::HALFLING;	break;
	case 8:  return Monster::SKELETON;	break;
	case 9:  return Monster::ZOMBIE;	break;
	default: break;
    }

    return Monster::ROGUE;
}

/* get rnd monster level 2 */
Monster::monster_t Monster::Rand2(void)
{
    switch(Rand::Get(1, 15)){
	case 1:  return Monster::RANGER;		break;
	case 2:  return Monster::PIKEMAN;		break;
	case 3:  return Monster::VETERAN_PIKEMAN;	break;
	case 4:  return Monster::CHIEF_ORC;		break;
	case 5:  return Monster::WOLF;			break;
	case 6:  return Monster::DWARF;			break;
	case 7:  return Monster::BATTLE_DWARF;		break;
	case 8:  return Monster::ELF;			break;
	case 9:  return Monster::GRAND_ELF;		break;
	case 10: return Monster::GARGOYLE;		break;
	case 11: return Monster::BOAR;			break;
	case 12: return Monster::IRON_GOLEM;		break;
	case 13: return Monster::MUTANT_ZOMBIE;		break;
	case 14: return Monster::MUMMY;			break;
	default: break;
    }
    
    return Monster::NOMAD;
}

/* get rnd monster level 3 */
Monster::monster_t Monster::Rand3(void)
{
    switch(Rand::Get(1, 27)){
	case 1:  return Monster::SWORDSMAN;		break;
	case 2:  return Monster::MASTER_SWORDSMAN;	break;
	case 3:  return Monster::CAVALRY;		break;
	case 4:  return Monster::CHAMPION;		break;
	case 5:  return Monster::OGRE;			break;
	case 6:  return Monster::LORD_OGRE;		break;
	case 7:  return Monster::TROLL;			break;
	case 8:  return Monster::WAR_TROLL;		break;
	case 9:  return Monster::DRUID;			break;
	case 10: return Monster::GREATER_DRUID;		break;
	case 11: return Monster::GRIFFIN;		break;
	case 12: return Monster::MINOTAUR;		break;
	case 13: return Monster::KNIGHT_MINOTAUR;	break;
	case 14: return Monster::STEEL_GOLEM;		break;
	case 15: return Monster::ROC;			break;
	case 16: return Monster::MAGE;			break;
	case 17: return Monster::ARCHMAGE;		break;
	case 18: return Monster::ROYAL_MUMMY;		break;
	case 19: return Monster::VAMPIRE;		break;
	case 20: return Monster::LORD_VAMPIRE;		break;
	case 21: return Monster::LICH;			break;
	case 22: return Monster::GHOST;			break;
	case 23: return Monster::MEDUSA;		break;
	case 24: return Monster::EARTH_ELEMENT;		break;
	case 25: return Monster::AIR_ELEMENT;		break;
	case 26: return Monster::FIRE_ELEMENT;		break;
	default: break;
    }

    return Monster::WATER_ELEMENT;
}

/* get rnd monster level 4 */
Monster::monster_t Monster::Rand4(void)
{
    switch(Rand::Get(1, 14)){
	case 1:  return Monster::PALADIN;	break;
	case 2:  return Monster::CRUSADER;	break;
	case 3:  return Monster::CYCLOPS;	break;
	case 4:  return Monster::UNICORN;	break;
	case 5:  return Monster::PHOENIX;	break;
	case 6:  return Monster::HYDRA;		break;
	case 7:  return Monster::GREEN_DRAGON;	break;
	case 8:  return Monster::RED_DRAGON;	break;
	case 9:  return Monster::BLACK_DRAGON;	break;
	case 10: return Monster::GIANT;		break;
	case 11: return Monster::TITAN;		break;
	case 12: return Monster::POWER_LICH;	break;
	case 13: return Monster::BONE_DRAGON;	break;
	default: break;
    }

    return Monster::GENIE;
}

/* get rnd count monster */
u16 Monster::RandCount(Monster::monster_t monster)
{
    switch(GetStats(monster).level){
        case Monster::LEVEL1: return Rand::Get(Army::THRONG, Army::LOTS) + Army::LOTS;
	case Monster::LEVEL2: return Rand::Get(Army::HORDE, Army::PACK) + Army::PACK;
	case Monster::LEVEL3: return Rand::Get(Army::LOTS, Army::SEVERAL) + Army::SEVERAL;
	default: break;
    }

    return Rand::Get(Army::PACK, Army::FEW) + Army::FEW;
}

Monster::monster_t Monster::Monster(Race::race_t race, u32 dwelling)
{
    switch(dwelling)
    {
        case Castle::DWELLING_MONSTER1:
	    switch(race)
	    {
		case Race::KNGT: return PEASANT;
		case Race::BARB: return GOBLIN;
		case Race::SORC: return SPRITE;
		case Race::WRLK: return CENTAUR;
		case Race::WZRD: return HALFLING;
		case Race::NECR: return SKELETON;
		default: break;
	    }
	    break;

        case Castle::DWELLING_MONSTER2:
	    switch(race)
	    {
		case Race::KNGT: return ARCHER;
		case Race::BARB: return ORC;
		case Race::SORC: return DWARF;
		case Race::WRLK: return GARGOYLE;
		case Race::WZRD: return BOAR;
		case Race::NECR: return ZOMBIE;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE2:
	    switch(race)
	    {
		case Race::KNGT: return RANGER;
		case Race::BARB: return CHIEF_ORC;
		case Race::SORC: return BATTLE_DWARF;
		case Race::WRLK: return GARGOYLE;
		case Race::WZRD: return BOAR;
		case Race::NECR: return MUTANT_ZOMBIE;
		default: break;
	    }
	    break;

        case Castle::DWELLING_MONSTER3:
	    switch(race)
	    {
		case Race::KNGT: return PIKEMAN;
		case Race::BARB: return WOLF;
		case Race::SORC: return ELF;
		case Race::WRLK: return GRIFFIN;
		case Race::WZRD: return IRON_GOLEM;
		case Race::NECR: return MUMMY;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE3:
	    switch(race)
	    {
		case Race::KNGT: return VETERAN_PIKEMAN;
		case Race::BARB: return WOLF;
		case Race::SORC: return GRAND_ELF;
		case Race::WRLK: return GRIFFIN;
		case Race::WZRD: return STEEL_GOLEM;
		case Race::NECR: return ROYAL_MUMMY;
		default: break;
	    }
	    break;

        case Castle::DWELLING_MONSTER4:
	    switch(race)
	    {
		case Race::KNGT: return SWORDSMAN;
		case Race::BARB: return OGRE;
		case Race::SORC: return DRUID;
		case Race::WRLK: return MINOTAUR;
		case Race::WZRD: return ROC;
		case Race::NECR: return VAMPIRE;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE4:
	    switch(race)
	    {
		case Race::KNGT: return MASTER_SWORDSMAN;
		case Race::BARB: return LORD_OGRE;
		case Race::SORC: return GREATER_DRUID;
		case Race::WRLK: return KNIGHT_MINOTAUR;
		case Race::WZRD: return ROC;
		case Race::NECR: return LORD_VAMPIRE;
		default: break;
	    }
	    break;

        case Castle::DWELLING_MONSTER5:
	    switch(race)
	    {
		case Race::KNGT: return CAVALRY;
		case Race::BARB: return TROLL;
		case Race::SORC: return UNICORN;
		case Race::WRLK: return HYDRA;
		case Race::WZRD: return MAGE;
		case Race::NECR: return LICH;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE5:
	    switch(race)
	    {
		case Race::KNGT: return CHAMPION;
		case Race::BARB: return WAR_TROLL;
		case Race::SORC: return UNICORN;
		case Race::WRLK: return HYDRA;
		case Race::WZRD: return ARCHMAGE;
		case Race::NECR: return POWER_LICH;
		default: break;
	    }
	    break;

    	case Castle::DWELLING_MONSTER6:
	    switch(race)
	    {
		case Race::KNGT: return PALADIN;
		case Race::BARB: return CYCLOPS;
		case Race::SORC: return PHOENIX;
		case Race::WRLK: return GREEN_DRAGON;
		case Race::WZRD: return GIANT;
		case Race::NECR: return BONE_DRAGON;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE6:
	    switch(race)
	    {
		case Race::KNGT: return CRUSADER;
		case Race::BARB: return CYCLOPS;
		case Race::SORC: return PHOENIX;
		case Race::WRLK: return RED_DRAGON;
		case Race::WZRD: return TITAN;
		case Race::NECR: return BONE_DRAGON;
		default: break;
	    }
	    break;

        case Castle::DWELLING_UPGRADE7:
	    switch(race)
	    {
		case Race::KNGT: return CRUSADER;
		case Race::BARB: return CYCLOPS;
		case Race::SORC: return PHOENIX;
		case Race::WRLK: return BLACK_DRAGON;
		case Race::WZRD: return TITAN;
		case Race::NECR: return BONE_DRAGON;
		default: break;
	    }
	    break;

        default: break;
    }

    Error::Warning("Monster::GetMonster: unknown level or race.");

    return Monster::UNKNOWN;
}

Monster::monster_t Monster::Upgrade(Monster::monster_t monster)
{
    switch(monster)
    {
        case ARCHER:	return RANGER;
        case PIKEMAN:	return VETERAN_PIKEMAN;
        case SWORDSMAN:	return MASTER_SWORDSMAN;
        case CAVALRY:	return CHAMPION;
        case PALADIN:	return CRUSADER;
        case ORC:	return CHIEF_ORC;
        case OGRE:	return LORD_OGRE;
        case TROLL:	return WAR_TROLL;
        case DWARF:	return BATTLE_DWARF;
        case ELF:	return GRAND_ELF;
        case DRUID:	return GREATER_DRUID;
        case ZOMBIE:	return MUTANT_ZOMBIE;
        case MUMMY:	return ROYAL_MUMMY;
        case VAMPIRE:	return LORD_VAMPIRE;
        case LICH:	return POWER_LICH;
        case MINOTAUR:	return KNIGHT_MINOTAUR;
        case GREEN_DRAGON:return RED_DRAGON;
        case RED_DRAGON:return BLACK_DRAGON;
        case IRON_GOLEM:return STEEL_GOLEM;
        case MAGE:	return ARCHMAGE;
        case GIANT:	return TITAN;
	default: break;
    }
    
    return monster;
}

/*
void Monster::BlitMiniSprite(u16 ox, u16 oy, monster_t monster, bool first)
{
    static u32 ticket = 0;

    u16 gaw = GameArea::GetRect().w;
    u16 gah = GameArea::GetRect().h;

    // draw first sprite
    const Sprite & sprite_first = AGG::GetICN("MINIMON.ICN", monster * 9);

    s16 dx = ox + TILEWIDTH / 2;
	dx -= std::abs(sprite_first.x());
    s16 dy = oy + TILEWIDTH;
        dy -= std::abs(sprite_first.y());
	dy -= 5;

    Rect src_rt(0, 0, sprite_first.w(), sprite_first.h());

    // left bound
    if(dx < BORDERWIDTH){
	src_rt.x = (dx < 0 ? std::abs(dx) + BORDERWIDTH : BORDERWIDTH - dx);
	src_rt.w -= src_rt.x;
        dx = BORDERWIDTH;
    }

    // top bound
    if(dy < BORDERWIDTH){
        src_rt.y = (dy < 0 ? std::abs(dy) + BORDERWIDTH : BORDERWIDTH - dy);
        src_rt.h -= src_rt.y;
        dy = BORDERWIDTH;
    }

    // right bound
    if(dx + sprite_first.w() > BORDERWIDTH + TILEWIDTH * gaw)
	src_rt.w -= (dx + sprite_first.w()) - (BORDERWIDTH + TILEWIDTH * gaw);

    // bottom bound
    if(dy + sprite_first.h() > BORDERWIDTH + TILEWIDTH * gah)
	src_rt.h -= (dy + sprite_first.h()) - (BORDERWIDTH + TILEWIDTH * gah);

    display.Blit(sprite_first, src_rt, dx, dy);

    // draw first sprite
    const Sprite & sprite_next = (first ? AGG::GetICN("MINIMON.ICN", monster * 9 + 1) :
	AGG::GetICN("MINIMON.ICN", monster * 9 + 1 + (ticket % 6)));

    dx = ox + TILEWIDTH / 2;
    dx -= std::abs(sprite_next.x());
    dy = oy + TILEWIDTH;
    dy -= std::abs(sprite_next.y());
    dy -= 5;

    src_rt = Rect(0, 0, sprite_next.w(), sprite_next.h());

    // left bound
    if(dx < BORDERWIDTH){
	src_rt.x = (dx < 0 ? std::abs(dx) + BORDERWIDTH : BORDERWIDTH - dx);
	src_rt.w -= src_rt.x;
        dx = BORDERWIDTH;
    }

    // top bound
    if(dy < BORDERWIDTH){
        src_rt.y = (dy < 0 ? std::abs(dy) + BORDERWIDTH : BORDERWIDTH - dy);
        src_rt.h -= src_rt.y;
        dy = BORDERWIDTH;
    }

    // right bound
    if(dx + sprite_next.w() > BORDERWIDTH + TILEWIDTH * gaw)
	src_rt.w -= (dx + sprite_next.w()) - (BORDERWIDTH + TILEWIDTH * gaw);

    // bottom bound
    if(dy + sprite_next.h() > BORDERWIDTH + TILEWIDTH * gah)
	src_rt.h -= (dy + sprite_next.h()) - (BORDERWIDTH + TILEWIDTH * gah);

    display.Blit(sprite_next, src_rt, dx, dy);

    ++ticket;    
}
*/
