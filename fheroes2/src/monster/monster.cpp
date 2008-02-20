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

#include "agg.h"
#include "rand.h"
#include "army.h"
#include "castle.h"
#include "error.h"
#include "game.h"
#include "config.h"
#include "gamearea.h"
#include "maps.h"
#include "difficulty.h"
#include "monster.h"

namespace Monster {

    static const stats_t all_monsters[] = {
	// monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ PEASANT,           1,   1,   0,   1,   1,   1,  Speed::VERYSLOW,  12, "Peasant"        , ICN::PEASANT , ICN::MONH0000 },
	{ ARCHER,            5,   3,  12,   2,   3,  10,  Speed::VERYSLOW,   8, "Archer"         , ICN::ARCHER  , ICN::MONH0001 },
	{ RANGER,            5,   3,  24,   2,   3,  10,   Speed::AVERAGE,   8, "Ranger"         , ICN::ARCHER2 , ICN::MONH0002 },
	{ PIKEMAN,           5,   9,   0,   3,   4,  15,   Speed::AVERAGE,   5, "Pikeman"        , ICN::PIKEMAN , ICN::MONH0003 },
	{ VETERAN_PIKEMAN,   5,   9,   0,   3,   4,  20,      Speed::FAST,   5, "Veteran Pikeman", ICN::PIKEMAN2, ICN::MONH0004 },
	{ SWORDSMAN,         7,   9,   0,   4,   6,  25,   Speed::AVERAGE,   4, "Swordsman"      , ICN::SWORDSMN, ICN::MONH0005 },
	{ MASTER_SWORDSMAN,  7,   9,   0,   4,   6,  30,      Speed::FAST,   4, "Master Swordman", ICN::SWORDSM2, ICN::MONH0006 },
	{ CAVALRY,          10,   9,   0,   5,   9,  30,  Speed::VERYFAST,   3, "Cavalry"        , ICN::CAVALRYR, ICN::MONH0007 },
	{ CHAMPION,         10,   9,   0,   5,  10,  40, Speed::ULTRAFAST,   3, "Champion"       , ICN::CAVALRYB, ICN::MONH0008 },
	{ PALADIN,          11,  12,   0,  10,  20,  50,      Speed::FAST,   2, "Paladin"        , ICN::PALADIN , ICN::MONH0009 },
	{ CRUSADER,         11,  12,   0,  10,  20,  65,  Speed::VERYFAST,   2, "Crusader"       , ICN::PALADIN2, ICN::MONH0010 },

	// monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ GOBLIN,            3,   1,   0,   1,   2,   3,   Speed::AVERAGE,  10, "Goblin"         , ICN::GOBLIN  , ICN::MONH0011 },
	{ ORC,               3,   4,   8,   2,   3,  10,  Speed::VERYSLOW,   8, "Orc"            , ICN::ORC     , ICN::MONH0012 },
	{ CHIEF_ORC,         3,   4,  16,   3,   4,  15,      Speed::SLOW,   8, "Orc Chief"      , ICN::ORC2    , ICN::MONH0013 },
	{ WOLF,              6,   2,   0,   3,   5,  20,  Speed::VERYFAST,   5, "Wolf"           , ICN::WOLF    , ICN::MONH0014 },
	{ OGRE,              9,   5,   0,   4,   6,  40,  Speed::VERYSLOW,   4, "Ogre"           , ICN::OGRE    , ICN::MONH0015 },
	{ LORD_OGRE,         9,   5,   0,   5,   7,  60,   Speed::AVERAGE,   4, "Ogre Lord"      , ICN::OGRE2   , ICN::MONH0016 },
	{ TROLL,            10,   5,   8,   5,   7,  40,   Speed::AVERAGE,   3, "Troll"          , ICN::TROLL   , ICN::MONH0017 },
	{ WAR_TROLL,        10,   5,  16,   7,   9,  40,      Speed::FAST,   3, "War Troll"      , ICN::TROLL2  , ICN::MONH0018 },
	{ CYCLOPS,          12,   9,   0,  12,  24,  80,      Speed::FAST,   2, "Cyclops"        , ICN::CYCLOPS , ICN::MONH0019 },

	// monster        atck dfnc shts  min  max   hp             speed grwn  name            file
	{ SPRITE,            4,   2,   0,   1,   2,   2,   Speed::AVERAGE,   8, "Sprite"         , ICN::SPRITE  , ICN::MONH0020 },
	{ DWARF,             6,   5,   0,   2,   4,  20,  Speed::VERYSLOW,   6, "Dwarf"          , ICN::DWARF   , ICN::MONH0021 },
	{ BATTLE_DWARF,      6,   6,   0,   2,   4,  20,   Speed::AVERAGE,   6, "Battle Dwarf"   , ICN::DWARF2  , ICN::MONH0022 },
	{ ELF,               4,   3,  24,   2,   3,  15,   Speed::AVERAGE,   4, "Elf"            , ICN::ELF     , ICN::MONH0023 },
	{ GRAND_ELF,         5,   5,  24,   2,   3,  15,  Speed::VERYFAST,   4, "Grand Elf"      , ICN::ELF2    , ICN::MONH0024 },
	{ DRUID,             7,   5,   8,   5,   8,  25,      Speed::FAST,   3, "Druid"          , ICN::DRUID   , ICN::MONH0025 },
	{ GREATER_DRUID,     7,   7,  16,   5,   8,  25,  Speed::VERYFAST,   3, "Greater Druid"  , ICN::DRUID2  , ICN::MONH0026 },
	{ UNICORN,          10,   9,   0,   7,  14,  40,      Speed::FAST,   2, "Unicorn"        , ICN::UNICORN , ICN::MONH0027 },
	{ PHOENIX,          12,  10,   0,  20,  40, 100, Speed::ULTRAFAST,   1, "Phoenix"        , ICN::PHOENIX , ICN::MONH0028 },

	// monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ CENTAUR,           3,   1,   8,   1,   2,   5,   Speed::AVERAGE,   8, "Centaur"        , ICN::CENTAUR , ICN::MONH0029 },
	{ GARGOYLE,          4,   7,   0,   2,   3,  15,  Speed::VERYFAST,   6, "Gargoyle"       , ICN::GARGOYLE, ICN::MONH0030 },
	{ GRIFFIN,           6,   6,   0,   3,   5,  25,   Speed::AVERAGE,   4, "Griffin"        , ICN::GRIFFIN , ICN::MONH0031 },
	{ MINOTAUR,          9,   8,   0,   5,  10,  35,   Speed::AVERAGE,   3, "Minotaur"       , ICN::MINOTAUR, ICN::MONH0032 },
	{ KNIGHT_MINOTAUR,   9,   8,   0,   5,  10,  45,  Speed::VERYFAST,   3, "Minotaur King"  , ICN::MINOTAU2, ICN::MONH0033 },
	{ HYDRA,             8,   9,   0,   6,  12,  75,  Speed::VERYSLOW,   2, "Hydra"          , ICN::HYDRA   , ICN::MONH0034 },
	{ GREEN_DRAGON,     12,  12,   0,  25,  50, 200,   Speed::AVERAGE,   1, "Green Dragon"   , ICN::DRAGGREE, ICN::MONH0035 },
	{ RED_DRAGON,       13,  13,   0,  25,  50, 250,      Speed::FAST,   1, "Red Dragon"     , ICN::DRAGRED , ICN::MONH0036 },
	{ BLACK_DRAGON,     14,  14,   0,  25,  50, 300,  Speed::VERYFAST,   1, "Black Dragon"   , ICN::DRAGBLAK, ICN::MONH0037 },

	// monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ HALFLING,          2,   1,  12,   1,   3,   3,      Speed::SLOW,   8, "Halfling"       , ICN::HALFLING, ICN::MONH0038 },
	{ BOAR,              5,   4,   0,   2,   3,  15,  Speed::VERYFAST,   6, "Boar"           , ICN::BOAR    , ICN::MONH0039 },
	{ IRON_GOLEM,        5,  10,   0,   4,   5,  30,  Speed::VERYSLOW,   4, "Iron Golem"     , ICN::GOLEM   , ICN::MONH0040 },
	{ STEEL_GOLEM,       7,  10,   0,   4,   5,  35,      Speed::SLOW,   4, "Steel Golem"    , ICN::GOLEM2  , ICN::MONH0041 },
	{ ROC,               7,   7,   0,   4,   8,  40,   Speed::AVERAGE,   3, "Roc"            , ICN::ROC     , ICN::MONH0042 },
	{ MAGE,             11,   7,  12,   7,   9,  30,      Speed::FAST,   2, "Mage"           , ICN::MAGE1   , ICN::MONH0043 },
	{ ARCHMAGE,         12,   8,  24,   7,   9,  35,  Speed::VERYFAST,   2, "Archmage"       , ICN::MAGE2   , ICN::MONH0044 },
	{ GIANT,            13,  10,   0,  20,  30, 150,   Speed::AVERAGE,   1, "Giant"          , ICN::TITANBLU, ICN::MONH0045 },
	{ TITAN,            15,  15,  24,  20,  30, 300,  Speed::VERYFAST,   1, "Titan"          , ICN::TITANBLA, ICN::MONH0046 },

	// monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ SKELETON,          4,   3,   0,   2,   3,   4,   Speed::AVERAGE,   8, "Skeleton"       , ICN::SKELETON, ICN::MONH0047 },
	{ ZOMBIE,            5,   2,   0,   2,   3,  15,  Speed::VERYSLOW,   6, "Zombie"         , ICN::ZOMBIE  , ICN::MONH0048 },
	{ MUTANT_ZOMBIE,     5,   2,   0,   2,   3,  25,   Speed::AVERAGE,   6, "Mutant Zombie"  , ICN::ZOMBIE2 , ICN::MONH0049 },
	{ MUMMY,             6,   6,   0,   3,   4,  25,   Speed::AVERAGE,   4, "Mummy"          , ICN::MUMMYW  , ICN::MONH0050 },
	{ ROYAL_MUMMY,       6,   6,   0,   3,   4,  30,      Speed::FAST,   4, "Royal Mummy"    , ICN::MUMMY2  , ICN::MONH0051 },
	{ VAMPIRE,           8,   6,   0,   5,   7,  30,   Speed::AVERAGE,   3, "Vampire"        , ICN::VAMPIRE , ICN::MONH0052 },
	{ LORD_VAMPIRE,      8,   6,   0,   5,   7,  40,      Speed::FAST,   3, "Lord Vampire"   , ICN::VAMPIRE2, ICN::MONH0053 },
	{ LICH,              7,  12,  12,   8,  10,  25,      Speed::FAST,   2, "Lich"           , ICN::LICH    , ICN::MONH0054 },
	{ POWER_LICH,        7,  13,  24,   8,  10,  35,  Speed::VERYFAST,   2, "Power Lich"     , ICN::LICH2   , ICN::MONH0055 },
	{ BONE_DRAGON,      11,   9,   0,  25,  45, 150,   Speed::AVERAGE,   1, "Bone Dragon"    , ICN::DRAGBONE, ICN::MONH0056 },

	// monster         atck dfnc shts  min  max   hp            speed grwn  name            file
	{ ROGUE,             6,   1,   0,   1,   2,   4,      Speed::FAST,   4, "Rogue"          , ICN::ROGUE   , ICN::MONH0057 },
	{ NOMAD,             7,   6,   0,   2,   5,  20,  Speed::VERYFAST,   4, "Nomad"          , ICN::NOMAD   , ICN::MONH0058 },
	{ GHOST,             8,   7,   0,   4,   6,  20,      Speed::FAST,   4, "Ghost"          , ICN::GHOST   , ICN::MONH0059 },
	{ MEDUSA,            8,   9,   0,   6,  10,  35,   Speed::AVERAGE,   4, "Medusa"         , ICN::MEDUSA  , ICN::MONH0060 },
	{ GENIE,            10,   9,   0,  20,  30,  50,  Speed::VERYFAST,   4, "Genie"          , ICN::GENIE   , ICN::MONH0061 },
	{ EARTH_ELEMENT,     8,   8,   0,   4,   5,  50,      Speed::SLOW,   4, "Earth Element"  , ICN::EELEM   , ICN::MONH0062 },
	{ AIR_ELEMENT,       7,   7,   0,   2,   8,  35,  Speed::VERYFAST,   4, "Air Element"    , ICN::AELEM   , ICN::MONH0063 },
	{ FIRE_ELEMENT,      8,   6,   0,   4,   6,  40,      Speed::FAST,   4, "Fire Element"   , ICN::FELEM   , ICN::MONH0064 },
	{ WATER_ELEMENT,     6,   8,   0,   3,   7,  45,   Speed::AVERAGE,   4, "Water Element"  , ICN::WELEM   , ICN::MONH0065 },
	
	// unknown
	{ UNKNOWN,           0,   0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Unknown"        , ICN::UNKNOWN , ICN::UNKNOWN }
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

u8 Monster::GetGrown(monster_t monster)
{ return Monster::GetStats(monster).grown; }

/* get race monster */
Race::race_t Monster::GetRace(monster_t monster)
{
    if(GOBLIN > monster)	return Race::KNGT;
    else
    if(SPRITE > monster)	return Race::BARB;
    else
    if(CENTAUR > monster)	return Race::SORC;
    else
    if(HALFLING > monster)	return Race::WRLK;
    else
    if(SKELETON > monster)	return Race::WZRD;
    else
    if(ROGUE > monster)		return Race::NECR;

    return Race::BOMG;
}

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

    Error::Warning("Monster::Monster: unknown level or race.");

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

bool Monster::AllowUpgrade(monster_t monster)
{
    return monster != Upgrade(monster);
}

void Monster::ChangeTileWithRNDMonster(std::vector<Maps::Tiles *> & vector, u16 center)
{
    Maps::Tiles & tile = *vector[center];
    const Maps::TilesAddon *addon = NULL;

    u8 icn_index = 0xFF;
    u8 index = 0;

    switch(tile.GetObject())
    {
	case MP2::OBJ_RNDMONSTER:
	icn_index = 0x42;
	index = Monster::Rand();
	break;
	case MP2::OBJ_RNDMONSTER1:
	icn_index = 0x43;
	index = Monster::Rand1();
	break;
	case MP2::OBJ_RNDMONSTER2:
	icn_index = 0x44;
	index = Monster::Rand2();
	break;
	case MP2::OBJ_RNDMONSTER3:
	icn_index = 0x45;
	index = Monster::Rand3();
	break;
	case MP2::OBJ_RNDMONSTER4:
	icn_index = 0x46;
	index = Monster::Rand4();
	break;
	default:
	return;
    }
    
    if( (addon = tile.FindAddon(0x30, icn_index)) ||
	(addon = tile.FindAddon(0x31, icn_index)) ||
	(addon = tile.FindAddon(0x32, icn_index)) ||
	(addon = tile.FindAddon(0x33, icn_index)))
    {
	//u32 uniq = (*addon).GetUniq();
	(*const_cast<Maps::TilesAddon *>(addon)).SetIndex(index);
	tile.SetObject(MP2::OBJ_MONSTER);
    }
}

Monster::level_t Monster::GetLevel(monster_t monster)
{
    switch(monster)
    {
	case PEASANT:
	case ARCHER:
	case GOBLIN:
	case ORC:
	case SPRITE:
	case CENTAUR:
	case HALFLING:
	case SKELETON:
	case ZOMBIE:
	case ROGUE:
	case UNKNOWN:
	case MONSTER_RND1:
			return LEVEL1;

	case RANGER:
	case PIKEMAN:
	case VETERAN_PIKEMAN:
	case CHIEF_ORC:
	case WOLF:
	case DWARF:
	case BATTLE_DWARF:
	case ELF:
	case GRAND_ELF:
	case GARGOYLE:
	case BOAR:
	case IRON_GOLEM:
	case MUTANT_ZOMBIE:
	case MUMMY:
	case NOMAD:
	case MONSTER_RND2:
			return LEVEL2;

	case SWORDSMAN:
	case MASTER_SWORDSMAN:
	case CAVALRY:
	case CHAMPION:
	case OGRE:
	case LORD_OGRE:
	case TROLL:
	case WAR_TROLL:
	case DRUID:
	case GREATER_DRUID:
	case GRIFFIN:
	case MINOTAUR:
	case KNIGHT_MINOTAUR:
	case STEEL_GOLEM:
	case ROC:
	case MAGE:
	case ARCHMAGE:
	case ROYAL_MUMMY:
	case VAMPIRE:
	case LORD_VAMPIRE:
	case LICH:
	case GHOST:
	case MEDUSA:
	case EARTH_ELEMENT:
	case AIR_ELEMENT:
	case FIRE_ELEMENT:
	case WATER_ELEMENT:
	case MONSTER_RND3:
			return LEVEL3;

	case PALADIN:
	case CRUSADER:
	case CYCLOPS:
	case UNICORN:
	case PHOENIX:
	case HYDRA:
	case GREEN_DRAGON:
	case RED_DRAGON:
	case BLACK_DRAGON:
	case GIANT:
	case TITAN:
	case POWER_LICH:
	case BONE_DRAGON:
	case GENIE:
	case MONSTER_RND4:
			return LEVEL4;
	
	case MONSTER_RND:
	    switch(Rand::Get(0, 3))
	    {
		default: return LEVEL1;
		case 1: return LEVEL2;
		case 2: return LEVEL3;
		case 3: return LEVEL4;
	    }
    }
    
    return LEVEL1;
}

/* get rnd count monster */
u16 Monster::GetRNDSize(monster_t monster)
{
    u8 level = 0;

    switch(Settings::Get().GameDifficulty())
    {
	case Difficulty::EASY:		level = 50; break;
	case Difficulty::NORMAL:	level = 100; break;
	case Difficulty::HARD:		level = 150; break;
	case Difficulty::EXPERT:	level = 200; break;
	case Difficulty::IMPOSSIBLE:	level = 250; break;
    }

    switch(GetLevel(monster))
    {
	case LEVEL1:
		    return (Rand::Get(Army::HORDE, Army::SWARM) * (level / 100));
	case LEVEL2:
		    return (Rand::Get(Army::LOTS, Army::THRONG) * (level / 100));
	case LEVEL3:
		    return (Rand::Get(Army::PACK, Army::HORDE) * (level / 100));
	case LEVEL4:
		    return (Rand::Get(Army::SEVERAL, Army::LOTS) * (level / 100));
    }

    return 1;
}

Monster::monster_t Monster::Monster(const Maps::Tiles & tile)
{
    const Maps::TilesAddon * addons = NULL;
    
    return ((addons = tile.FindAddon(0x30)) ||
	    (addons = tile.FindAddon(0x31)) ||
            (addons = tile.FindAddon(0x32)) ||
            (addons = tile.FindAddon(0x33)) ? Monster::Monster(addons->GetIndex()) : Monster::UNKNOWN);
}

void Monster::ChangeTileWithRNDSize(Maps::Tiles & tile)
{
    monster_t monster = Monster(tile);

    if(Monster::UNKNOWN > monster)
    {
	u16 size = GetRNDSize(monster);

	size <<= 3;
	tile.SetQuantity2(static_cast<u8>(0x00FF & size));

	size >>= 8;
	tile.SetQuantity1(static_cast<u8>(0x00FF & size));
    }
}

u16 Monster::GetSize(const Maps::Tiles & tile)
{
    u16 size = tile.GetQuantity2();
    size <<= 8;
    size |= tile.GetQuantity1();
    size >>= 3;

    // random
    if(0 == size)
    {
	size = GetRNDSize(Monster(tile));
	
	ChangeTileWithRNDSize(const_cast<Maps::Tiles &>(tile));
    }

    return size;
}
