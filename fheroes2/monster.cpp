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

#include "error.h"
#include "monster.h"

namespace Monster {
    //                                 atck dfnc shts  min  max   hp      speed  cost grwn  name               icn sprite maps
    static const stats_t peasant    = {   1,   1,   0,   1,   1,   1,  VERYSLOW,    2,  12, "Peasant"        , "PEASANT.ICN" };
    static const stats_t archer     = {   5,   3,  12,   2,   3,  10,  VERYSLOW,  150,   8, "Archer"         , "ARCHER.ICN" };
    static const stats_t ranger     = {   5,   3,  24,   2,   3,  10,   AVERAGE,  200,   8, "Ranger"         , "ARCHER2.ICN" };
    static const stats_t pikeman    = {   5,   9,   0,   3,   4,  15,   AVERAGE,  200,   5, "Pikeman"        , "PIKEMAN.ICN" };
    static const stats_t pikeman2   = {   5,   9,   0,   3,   4,  20,      FAST,  250,   5, "Veteran Pikeman", "PIKEMAN2.ICN" };
    static const stats_t swordsman  = {   7,   9,   0,   4,   6,  25,   AVERAGE,  250,   4, "Swordsman"      , "SWORDSMN.ICN" };
    static const stats_t swordsman2 = {   7,   9,   0,   4,   6,  30,      FAST,  300,   4, "Master Swordman", "SWORDSM2.ICN" };
    static const stats_t cavalry    = {  10,   9,   0,   5,   9,  30,  VERYFAST,  300,   3, "Cavalry"        , "CAVALRYR.ICN" };
    static const stats_t champion   = {  10,   9,   0,   5,  10,  40, ULTRAFAST,  375,   3, "Champion"       , "CAVALRYB.ICN" };
    static const stats_t paladin    = {  11,  12,   0,  10,  20,  50,      FAST,  600,   2, "Paladin"        , "PALADIN.ICN" };
    static const stats_t crusader   = {  11,  12,   0,  10,  20,  65,  VERYFAST, 1000,   2, "Crusader"       , "PALADIN2.ICN" };

    static const stats_t goblin     = {   3,   1,   0,   1,   2,   3,   AVERAGE,   40,  10, "Goblin"         , "GOBLIN.ICN" };
    static const stats_t orc        = {   3,   4,   8,   2,   3,  10,  VERYSLOW,  140,   8, "Orc"            , "ORC.ICN" };
    static const stats_t orc2       = {   3,   4,  16,   3,   4,  15,      SLOW,  175,   8, "Orc Chief"      , "ORC2.ICN" };
    static const stats_t wolf       = {   6,   2,   0,   3,   5,  20,  VERYFAST,  200,   5, "Wolf"           , "WOLF.ICN" };
    static const stats_t ogre       = {   9,   5,   0,   4,   6,  40,  VERYSLOW,  300,   4, "Ogre"           , "OGRE.ICN" };
    static const stats_t ogre2      = {   9,   5,   0,   5,   7,  60,   AVERAGE,  500,   4, "Ogre Lord"      , "OGRE2.ICN" };
    static const stats_t troll      = {  10,   5,   8,   5,   7,  40,   AVERAGE,  600,   3, "Troll"          , "TROLL.ICN" };
    static const stats_t troll2     = {  10,   5,  16,   7,   9,  40,      FAST,  700,   3, "War Troll"      , "TROLL2.ICN" };
    static const stats_t cyclops    = {  12,   9,   0,  12,  24,  80,      FAST,  750,   2, "Cyclops"        , "CYCLOPS.ICN" };

    static const stats_t sprite     = {   4,   2,   0,   1,   2,   2,   AVERAGE,   50,   8, "Sprite"         , "SPRITE.ICN" };
    static const stats_t dwarf      = {   6,   5,   0,   2,   4,  20,  VERYSLOW,  200,   6, "Dwarf"          , "DWARF.ICN" };
    static const stats_t dwarf2     = {   6,   6,   0,   2,   4,  20,   AVERAGE,  250,   6, "Battle Dwarf"   , "DWARF2.ICN" };
    static const stats_t elf        = {   4,   3,  24,   2,   3,  15,   AVERAGE,  250,   4, "Elf"            , "ELF.ICN" };
    static const stats_t elf2       = {   5,   5,  24,   2,   3,  15,  VERYFAST,  300,   4, "Grand Elf"      , "ELF2.ICN" };
    static const stats_t druid      = {   7,   5,   8,   5,   8,  25,      FAST,  350,   3, "Druid"          , "DRUID.ICN" };
    static const stats_t druid2     = {   7,   7,  16,   5,   8,  25,  VERYFAST,  400,   3, "Greater Druid"  , "DRUID2.ICN" };
    static const stats_t unicorn    = {  10,   9,   0,   7,  14,  40,      FAST,  500,   2, "Unicorn"        , "UNICORN.ICN" };
    static const stats_t phoenix    = {  12,  10,   0,  20,  40, 100, ULTRAFAST, 1500,   1, "Phoenix"        , "PHOENIX.ICN" };

    static const stats_t centaur    = {   3,   1,   8,   1,   2,   5,   AVERAGE,   60,   8, "Centaur"        , "CENTAUR.ICN" };
    static const stats_t gargoyle   = {   4,   7,   0,   2,   3,  15,  VERYFAST,  200,   6, "Gargoyle"       , "GARGOYLE.ICN" };
    static const stats_t griffin    = {   6,   6,   0,   3,   5,  25,   AVERAGE,  300,   4, "Griffin"        , "GRIFFIN.ICN" };
    static const stats_t minotaur   = {   9,   8,   0,   5,  10,  35,   AVERAGE,  400,   3, "Minotaur"       , "MINOTAUR.ICN" };
    static const stats_t minotaur2  = {   9,   8,   0,   5,  10,  45,  VERYFAST,  500,   3, "Minotaur King"  , "MINOTAU2.ICN" };
    static const stats_t hydra      = {   8,   9,   0,   6,  12,  75,  VERYSLOW,  800,   2, "Hydra"          , "HYDRA.ICN" };
    static const stats_t dragon     = {  12,  12,   0,  25,  50, 200,   AVERAGE, 3000,   1, "Green Dragon"   , "DRAGGREE.ICN" };
    static const stats_t dragon2    = {  13,  13,   0,  25,  50, 250,      FAST, 3500,   1, "Red Dragon"     , "DRAGRED.ICN" };
    static const stats_t dragon3    = {  14,  14,   0,  25,  50, 300,  VERYFAST, 4000,   1, "Black Dragon"   , "DRAGBLAK.ICN" };

    static const stats_t halfling   = {   2,   1,  12,   1,   3,   3,      SLOW,   50,   8, "Halfling"       , "HALFLING.ICN" };
    static const stats_t boar       = {   5,   4,   0,   2,   3,  15,  VERYFAST,  150,   6, "Boar"           , "BOAR.ICN" };
    static const stats_t golem      = {   5,  10,   0,   4,   5,  30,  VERYSLOW,  300,   4, "Iron Golem"     , "GOLEM.ICN" };
    static const stats_t golem2     = {   7,  10,   0,   4,   5,  35,      SLOW,  350,   4, "Steel Golem"    , "GOLEM2.ICN" };
    static const stats_t roc        = {   7,   7,   0,   4,   8,  40,   AVERAGE,  400,   3, "Roc"            , "ROC.ICN" };
    static const stats_t mage       = {  11,   7,  12,   7,   9,  30,      FAST,  600,   2, "Mage"           , "MAGE1.ICN" };
    static const stats_t mage2      = {  12,   8,  24,   7,   9,  35,  VERYFAST,  700,   2, "Archmage"       , "MAGE2.ICN" };
    static const stats_t giant      = {  13,  10,   0,  20,  30, 150,   AVERAGE, 2000,   1, "Giant"          , "TITANBLU.ICN" };
    static const stats_t titan      = {  15,  15,  24,  20,  30, 300,  VERYFAST, 5000,   1, "Titan"          , "TITANBLA.ICN" };

    static const stats_t skeleton   = {   4,   3,   0,   2,   3,   4,   AVERAGE,   75,   8, "Skeleton"       , "SKELETON.ICN" };
    static const stats_t zombie     = {   5,   2,   0,   2,   3,  15,  VERYSLOW,  150,   6, "Zombie"         , "ZOMBIE.ICN" };
    static const stats_t zombie2    = {   5,   2,   0,   2,   3,  25,   AVERAGE,  200,   6, "Mutant Zombie"  , "ZOMBIE2.ICN" };
    static const stats_t mummy      = {   6,   6,   0,   3,   4,  25,   AVERAGE,  250,   4, "Mummy"          , "MUMMYW.ICN" };
    static const stats_t mummy2     = {   6,   6,   0,   3,   4,  30,      FAST,  300,   4, "Royal Mummy"    , "MUMMY2.ICN" };
    static const stats_t vampire    = {   8,   6,   0,   5,   7,  30,   AVERAGE,  500,   3, "Vampire"        , "VAMPIRE.ICN" };
    static const stats_t vampire2   = {   8,   6,   0,   5,   7,  40,      FAST,  650,   3, "Lord Vampire"   , "VAMPIRE2.ICN" };
    static const stats_t lich       = {   7,  12,  12,   8,  10,  25,      FAST,  750,   2, "Lich"           , "LICH.ICN" };
    static const stats_t lich2      = {   7,  13,  24,   8,  10,  35,  VERYFAST,  900,   2, "Power Lich"     , "LICH2.ICN" };
    static const stats_t dragbone   = {  11,   9,   0,  25,  45, 150,   AVERAGE, 1500,   1, "Bone Dragon"    , "DRAGBONE.ICN" };

    static const stats_t rogue      = {   6,   1,   0,   1,   2,   4,      FAST,   50,   4, "Rogue"          , "ROGUE.ICN" };
    static const stats_t nomad      = {   7,   6,   0,   2,   5,  20,  VERYFAST,  200,   4, "Nomad"          , "NOMAD.ICN" };
    static const stats_t ghost      = {   8,   7,   0,   4,   6,  20,      FAST, 1000,   4, "Ghost"          , "GHOST.ICN" };
    static const stats_t medusa     = {   8,   9,   0,   6,  10,  35,   AVERAGE,  500,   4, "Medusa"         , "MEDUSA.ICN" };
    static const stats_t genie      = {  10,   9,   0,  20,  30,  50,  VERYFAST,  650,   4, "Genie"          , "GENIE.ICN" };
    static const stats_t eelement   = {   8,   8,   0,   4,   5,  50,      SLOW,  500,   4, "Earth Element"  , "EELEM.ICN" };
    static const stats_t aelement   = {   7,   7,   0,   2,   8,  35,  VERYFAST,  500,   4, "Air Element"    , "AELEM.ICN" };
    static const stats_t felement   = {   8,   6,   0,   4,   6,  40,      FAST,  500,   4, "Fire Element"   , "FELEM.ICN" };
    static const stats_t welement   = {   6,   8,   0,   3,   7,  45,   AVERAGE,  500,   4, "Water Element"  , "WELEM.ICN" };

    static const stats_t unknown   = {    0,   0,   0,   0,   0,   0,  VERYSLOW, 9999,   0, "Unknown"        , "UNKNOWN.ICN" };
}

/* get stats monster */
const Monster::stats_t & Monster::GetStats(monster_t monster)
{
    switch(monster){

	// knight
        case Monster::PEASANT:			return peasant;
        case Monster::ARCHER:			return archer;
        case Monster::RANGER:			return ranger;
        case Monster::PIKEMAN:			return pikeman;
        case Monster::VETERAN_PIKEMAN:		return pikeman2;
        case Monster::SWORDSMAN:		return swordsman;
	case Monster::MASTER_SWORDSMAN:		return swordsman2;
	case Monster::CAVALRY:			return cavalry;
	case Monster::CHAMPION:			return champion;
	case Monster::PALADIN:			return paladin;
	case Monster::CRUSADER:			return crusader;
	
	// barbarian
	case Monster::GOBLIN:			return goblin;
	case Monster::ORC:			return orc;
	case Monster::CHIEF_ORC:		return orc2;
	case Monster::WOLF:			return wolf;
	case Monster::OGRE:			return ogre;
	case Monster::LORD_OGRE:		return ogre2;
	case Monster::TROLL:			return troll;
	case Monster::WAR_TROLL:		return troll2;
	case Monster::CYCLOPS:			return cyclops;
	
	// sorceress
	case Monster::SPRITE:			return sprite;
	case Monster::DWARF:			return dwarf;
	case Monster::BATTLE_DWARF:		return dwarf2;
	case Monster::ELF:			return elf;
	case Monster::GRAND_ELF:		return elf2;
	case Monster::DRUID:			return druid;
	case Monster::GREATER_DRUID:		return druid2;
	case Monster::UNICORN:			return unicorn;
	case Monster::PHOENIX:			return phoenix;
	
	// warlock
	case Monster::CENTAUR:			return centaur;
	case Monster::GARGOYLE:			return gargoyle;
	case Monster::GRIFFIN:			return griffin;
	case Monster::MINOTAUR:			return minotaur;
	case Monster::KNIGHT_MINOTAUR:		return minotaur2;
	case Monster::HYDRA:			return hydra;
	case Monster::GREEN_DRAGON:		return dragon;
	case Monster::RED_DRAGON:		return dragon2;
	case Monster::BLACK_DRAGON:		return dragon3;
	
	// wizard
	case Monster::HALFLING:			return halfling;
	case Monster::BOAR:			return boar;
	case Monster::IRON_GOLEM:		return golem;
	case Monster::STEEL_GOLEM:		return golem2;
	case Monster::ROC:			return roc;
	case Monster::MAGE:			return mage;
	case Monster::ARCHMAGE:			return mage2;
	case Monster::GIANT:			return giant;
	case Monster::TITAN:			return titan;
	
	// necromancer
	case Monster::SKELETON:			return skeleton;
	case Monster::ZOMBIE:			return zombie;
	case Monster::MUTANT_ZOMBIE:		return zombie2;
	case Monster::MUMMY:			return mummy;
	case Monster::ROYAL_MUMMY:		return mummy2;
	case Monster::VAMPIRE:			return vampire;
	case Monster::LORD_VAMPIRE:		return vampire2;
	case Monster::LICH:			return lich;
	case Monster::POWER_LICH:		return lich2;
	case Monster::BONE_DRAGON:		return dragbone;

	// bomg
	case Monster::ROGUE:			return rogue;
	case Monster::NOMAD:			return nomad;
	case Monster::GHOST:			return ghost;
	case Monster::GENIE:			return genie;
	case Monster::MEDUSA:			return medusa;
	case Monster::EARTH_ELEMENT:		return eelement;
	case Monster::AIR_ELEMENT:		return aelement;
	case Monster::FIRE_ELEMENT:		return felement;
	case Monster::WATER_ELEMENT:		return welement;
	
	default: Error::Warning("Monster::GetStats: unknown: ", monster); break;
    }

    return unknown;
}

/* get string name */
const std::string & Monster::String(monster_t monster){ return Monster::GetStats(monster).name; }
