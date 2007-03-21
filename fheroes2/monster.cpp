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

#include "rand.h"
#include "army.h"
#include "monster.h"

namespace Monster {

    static const stats_t all_monsters[] = {
	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// peasant
	{ LEVEL1,   1,   1,   0,   1,   1,   1,  VERYSLOW,    2,  12, "Peasant"        , "PEASANT.ICN" },
	// archer
	{ LEVEL1,   5,   3,  12,   2,   3,  10,  VERYSLOW,  150,   8, "Archer"         , "ARCHER.ICN" },
	// ranger
	{ LEVEL2,   5,   3,  24,   2,   3,  10,   AVERAGE,  200,   8, "Ranger"         , "ARCHER2.ICN" },
	// pikeman
	{ LEVEL2,   5,   9,   0,   3,   4,  15,   AVERAGE,  200,   5, "Pikeman"        , "PIKEMAN.ICN" },
	// veteran pikeman
	{ LEVEL2,   5,   9,   0,   3,   4,  20,      FAST,  250,   5, "Veteran Pikeman", "PIKEMAN2.ICN" },
	// swordsman
	{ LEVEL3,   7,   9,   0,   4,   6,  25,   AVERAGE,  250,   4, "Swordsman"      , "SWORDSMN.ICN" },
	// master swordsman
	{ LEVEL3,   7,   9,   0,   4,   6,  30,      FAST,  300,   4, "Master Swordman", "SWORDSM2.ICN" },
	// cavalry
	{ LEVEL3,  10,   9,   0,   5,   9,  30,  VERYFAST,  300,   3, "Cavalry"        , "CAVALRYR.ICN" },
	// champion
	{ LEVEL3,  10,   9,   0,   5,  10,  40, ULTRAFAST,  375,   3, "Champion"       , "CAVALRYB.ICN" },
	// paladin
	{ LEVEL4,  11,  12,   0,  10,  20,  50,      FAST,  600,   2, "Paladin"        , "PALADIN.ICN" },
	// crusader
	{ LEVEL4,  11,  12,   0,  10,  20,  65,  VERYFAST, 1000,   2, "Crusader"       , "PALADIN2.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// goblin
	{ LEVEL1,   3,   1,   0,   1,   2,   3,   AVERAGE,   40,  10, "Goblin"         , "GOBLIN.ICN" },
	// orc
	{ LEVEL1,   3,   4,   8,   2,   3,  10,  VERYSLOW,  140,   8, "Orc"            , "ORC.ICN" },
	// chief orc
	{ LEVEL2,   3,   4,  16,   3,   4,  15,      SLOW,  175,   8, "Orc Chief"      , "ORC2.ICN" },
	// wolf
	{ LEVEL2,   6,   2,   0,   3,   5,  20,  VERYFAST,  200,   5, "Wolf"           , "WOLF.ICN" },
	// ogre
	{ LEVEL3,   9,   5,   0,   4,   6,  40,  VERYSLOW,  300,   4, "Ogre"           , "OGRE.ICN" },
	// lord ogre
	{ LEVEL3,   9,   5,   0,   5,   7,  60,   AVERAGE,  500,   4, "Ogre Lord"      , "OGRE2.ICN" },
	// troll
	{ LEVEL3,  10,   5,   8,   5,   7,  40,   AVERAGE,  600,   3, "Troll"          , "TROLL.ICN" },
	// war troll
	{ LEVEL3,  10,   5,  16,   7,   9,  40,      FAST,  700,   3, "War Troll"      , "TROLL2.ICN" },
	// cyclops
	{ LEVEL4,  12,   9,   0,  12,  24,  80,      FAST,  750,   2, "Cyclops"        , "CYCLOPS.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// sprite
	{ LEVEL1,   4,   2,   0,   1,   2,   2,   AVERAGE,   50,   8, "Sprite"         , "SPRITE.ICN" },
	// dwarf
	{ LEVEL2,   6,   5,   0,   2,   4,  20,  VERYSLOW,  200,   6, "Dwarf"          , "DWARF.ICN" },
	// battle dwarf
	{ LEVEL2,   6,   6,   0,   2,   4,  20,   AVERAGE,  250,   6, "Battle Dwarf"   , "DWARF2.ICN" },
	// elf
	{ LEVEL2,   4,   3,  24,   2,   3,  15,   AVERAGE,  250,   4, "Elf"            , "ELF.ICN" },
	// grand elf
	{ LEVEL2,   5,   5,  24,   2,   3,  15,  VERYFAST,  300,   4, "Grand Elf"      , "ELF2.ICN" },
	// druid
	{ LEVEL3,   7,   5,   8,   5,   8,  25,      FAST,  350,   3, "Druid"          , "DRUID.ICN" },
	// greater druid
	{ LEVEL3,   7,   7,  16,   5,   8,  25,  VERYFAST,  400,   3, "Greater Druid"  , "DRUID2.ICN" },
	// unicorn
	{ LEVEL4,  10,   9,   0,   7,  14,  40,      FAST,  500,   2, "Unicorn"        , "UNICORN.ICN" },
	// phoenix
	{ LEVEL4,  12,  10,   0,  20,  40, 100, ULTRAFAST, 1500,   1, "Phoenix"        , "PHOENIX.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// centaur
	{ LEVEL1,   3,   1,   8,   1,   2,   5,   AVERAGE,   60,   8, "Centaur"        , "CENTAUR.ICN" },
	// gargoyle
	{ LEVEL2,   4,   7,   0,   2,   3,  15,  VERYFAST,  200,   6, "Gargoyle"       , "GARGOYLE.ICN" },
	// griffin
	{ LEVEL3,   6,   6,   0,   3,   5,  25,   AVERAGE,  300,   4, "Griffin"        , "GRIFFIN.ICN" },
	// minotaur
	{ LEVEL3,   9,   8,   0,   5,  10,  35,   AVERAGE,  400,   3, "Minotaur"       , "MINOTAUR.ICN" },
	// knight minotaur
	{ LEVEL3,   9,   8,   0,   5,  10,  45,  VERYFAST,  500,   3, "Minotaur King"  , "MINOTAU2.ICN" },
	// hydra
	{ LEVEL4,   8,   9,   0,   6,  12,  75,  VERYSLOW,  800,   2, "Hydra"          , "HYDRA.ICN" },
	// green dragon
	{ LEVEL4,  12,  12,   0,  25,  50, 200,   AVERAGE, 3000,   1, "Green Dragon"   , "DRAGGREE.ICN" },
	// red dragon
	{ LEVEL4,  13,  13,   0,  25,  50, 250,      FAST, 3500,   1, "Red Dragon"     , "DRAGRED.ICN" },
	// black dragon
	{ LEVEL4,  14,  14,   0,  25,  50, 300,  VERYFAST, 4000,   1, "Black Dragon"   , "DRAGBLAK.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// halfling
	{ LEVEL1,   2,   1,  12,   1,   3,   3,      SLOW,   50,   8, "Halfling"       , "HALFLING.ICN" },
	// boar
	{ LEVEL2,   5,   4,   0,   2,   3,  15,  VERYFAST,  150,   6, "Boar"           , "BOAR.ICN" },
	// iron golem
	{ LEVEL2,   5,  10,   0,   4,   5,  30,  VERYSLOW,  300,   4, "Iron Golem"     , "GOLEM.ICN" },
	// steel golem
	{ LEVEL3,   7,  10,   0,   4,   5,  35,      SLOW,  350,   4, "Steel Golem"    , "GOLEM2.ICN" },
	// roc
	{ LEVEL3,   7,   7,   0,   4,   8,  40,   AVERAGE,  400,   3, "Roc"            , "ROC.ICN" },
	// mage
	{ LEVEL3,  11,   7,  12,   7,   9,  30,      FAST,  600,   2, "Mage"           , "MAGE1.ICN" },
	// archi mage
	{ LEVEL3,  12,   8,  24,   7,   9,  35,  VERYFAST,  700,   2, "Archmage"       , "MAGE2.ICN" },
	// giant
	{ LEVEL4,  13,  10,   0,  20,  30, 150,   AVERAGE, 2000,   1, "Giant"          , "TITANBLU.ICN" },
	// titan
	{ LEVEL4,  15,  15,  24,  20,  30, 300,  VERYFAST, 5000,   1, "Titan"          , "TITANBLA.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// skeleton
	{ LEVEL1,   4,   3,   0,   2,   3,   4,   AVERAGE,   75,   8, "Skeleton"       , "SKELETON.ICN" },
	// zombie
	{ LEVEL1,   5,   2,   0,   2,   3,  15,  VERYSLOW,  150,   6, "Zombie"         , "ZOMBIE.ICN" },
	// mutant zombie
	{ LEVEL2,   5,   2,   0,   2,   3,  25,   AVERAGE,  200,   6, "Mutant Zombie"  , "ZOMBIE2.ICN" },
	// mummy
	{ LEVEL2,   6,   6,   0,   3,   4,  25,   AVERAGE,  250,   4, "Mummy"          , "MUMMYW.ICN" },
	// royal mummy
	{ LEVEL3,   6,   6,   0,   3,   4,  30,      FAST,  300,   4, "Royal Mummy"    , "MUMMY2.ICN" },
	// vampire
	{ LEVEL3,   8,   6,   0,   5,   7,  30,   AVERAGE,  500,   3, "Vampire"        , "VAMPIRE.ICN" },
	// lord vampire
	{ LEVEL3,   8,   6,   0,   5,   7,  40,      FAST,  650,   3, "Lord Vampire"   , "VAMPIRE2.ICN" },
	// lich
	{ LEVEL3,   7,  12,  12,   8,  10,  25,      FAST,  750,   2, "Lich"           , "LICH.ICN" },
	// power lich
	{ LEVEL4,   7,  13,  24,   8,  10,  35,  VERYFAST,  900,   2, "Power Lich"     , "LICH2.ICN" },
	// bone dragon
	{ LEVEL4,  11,   9,   0,  25,  45, 150,   AVERAGE, 1500,   1, "Bone Dragon"    , "DRAGBONE.ICN" },

	// level atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// rogue
	{ LEVEL1,  6,   1,   0,   1,   2,   4,      FAST,   50,   4, "Rogue"          , "ROGUE.ICN" },
	// nomad
	{ LEVEL2,  7,   6,   0,   2,   5,  20,  VERYFAST,  200,   4, "Nomad"          , "NOMAD.ICN" },
	// ghost
	{ LEVEL3,  8,   7,   0,   4,   6,  20,      FAST, 1000,   4, "Ghost"          , "GHOST.ICN" },
	// medusa
	{ LEVEL3,  8,   9,   0,   6,  10,  35,   AVERAGE,  500,   4, "Medusa"         , "MEDUSA.ICN" },
	// genie
	{ LEVEL4, 10,   9,   0,  20,  30,  50,  VERYFAST,  650,   4, "Genie"          , "GENIE.ICN" },
	// earth element
	{ LEVEL3,  8,   8,   0,   4,   5,  50,      SLOW,  500,   4, "Earth Element"  , "EELEM.ICN" },
	// air element
	{ LEVEL3,  7,   7,   0,   2,   8,  35,  VERYFAST,  500,   4, "Air Element"    , "AELEM.ICN" },
	// fire element
	{ LEVEL3,  8,   6,   0,   4,   6,  40,      FAST,  500,   4, "Fire Element"   , "FELEM.ICN" },
	// water element
	{ LEVEL3,  6,   8,   0,   3,   7,  45,   AVERAGE,  500,   4, "Water Element"  , "WELEM.ICN" },
	
	// unknown
	{ LEVEL1,  0,   0,   0,   0,   0,   0,  VERYSLOW, 9999,   0, "Unknown"       , "UNKNOWN.ICN" }
    };

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
