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

#include "monster.h"

namespace Monster {

    static const stats_t all_monsters[] = {
	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// peasant
	{   1,   1,   0,   1,   1,   1,  VERYSLOW,    2,  12, "Peasant"        , "PEASANT.ICN" },
	// archer
	{   5,   3,  12,   2,   3,  10,  VERYSLOW,  150,   8, "Archer"         , "ARCHER.ICN" },
	// ranger
	{   5,   3,  24,   2,   3,  10,   AVERAGE,  200,   8, "Ranger"         , "ARCHER2.ICN" },
	// pikeman
	{   5,   9,   0,   3,   4,  15,   AVERAGE,  200,   5, "Pikeman"        , "PIKEMAN.ICN" },
	// veteran pikeman
	{   5,   9,   0,   3,   4,  20,      FAST,  250,   5, "Veteran Pikeman", "PIKEMAN2.ICN" },
	// swordsman
	{   7,   9,   0,   4,   6,  25,   AVERAGE,  250,   4, "Swordsman"      , "SWORDSMN.ICN" },
	// master swordsman
	{   7,   9,   0,   4,   6,  30,      FAST,  300,   4, "Master Swordman", "SWORDSM2.ICN" },
	// cavalry
	{  10,   9,   0,   5,   9,  30,  VERYFAST,  300,   3, "Cavalry"        , "CAVALRYR.ICN" },
	// champion
	{  10,   9,   0,   5,  10,  40, ULTRAFAST,  375,   3, "Champion"       , "CAVALRYB.ICN" },
	// paladin
	{  11,  12,   0,  10,  20,  50,      FAST,  600,   2, "Paladin"        , "PALADIN.ICN" },
	// crusader
	{  11,  12,   0,  10,  20,  65,  VERYFAST, 1000,   2, "Crusader"       , "PALADIN2.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// goblin
	{   3,   1,   0,   1,   2,   3,   AVERAGE,   40,  10, "Goblin"         , "GOBLIN.ICN" },
	// orc
	{   3,   4,   8,   2,   3,  10,  VERYSLOW,  140,   8, "Orc"            , "ORC.ICN" },
	// chief orc
	{   3,   4,  16,   3,   4,  15,      SLOW,  175,   8, "Orc Chief"      , "ORC2.ICN" },
	// wolf
	{   6,   2,   0,   3,   5,  20,  VERYFAST,  200,   5, "Wolf"           , "WOLF.ICN" },
	// ogre
	{   9,   5,   0,   4,   6,  40,  VERYSLOW,  300,   4, "Ogre"           , "OGRE.ICN" },
	// lord ogre
	{   9,   5,   0,   5,   7,  60,   AVERAGE,  500,   4, "Ogre Lord"      , "OGRE2.ICN" },
	// troll
	{  10,   5,   8,   5,   7,  40,   AVERAGE,  600,   3, "Troll"          , "TROLL.ICN" },
	// war troll
	{  10,   5,  16,   7,   9,  40,      FAST,  700,   3, "War Troll"      , "TROLL2.ICN" },
	// cyclops
	{  12,   9,   0,  12,  24,  80,      FAST,  750,   2, "Cyclops"        , "CYCLOPS.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// sprite
	{   4,   2,   0,   1,   2,   2,   AVERAGE,   50,   8, "Sprite"         , "SPRITE.ICN" },
	// dwarf
	{   6,   5,   0,   2,   4,  20,  VERYSLOW,  200,   6, "Dwarf"          , "DWARF.ICN" },
	// battle dwarf
	{   6,   6,   0,   2,   4,  20,   AVERAGE,  250,   6, "Battle Dwarf"   , "DWARF2.ICN" },
	// elf
	{   4,   3,  24,   2,   3,  15,   AVERAGE,  250,   4, "Elf"            , "ELF.ICN" },
	// grand elf
	{   5,   5,  24,   2,   3,  15,  VERYFAST,  300,   4, "Grand Elf"      , "ELF2.ICN" },
	// druid
	{   7,   5,   8,   5,   8,  25,      FAST,  350,   3, "Druid"          , "DRUID.ICN" },
	// greater druid
	{   7,   7,  16,   5,   8,  25,  VERYFAST,  400,   3, "Greater Druid"  , "DRUID2.ICN" },
	// unicorn
	{  10,   9,   0,   7,  14,  40,      FAST,  500,   2, "Unicorn"        , "UNICORN.ICN" },
	// phoenix
	{  12,  10,   0,  20,  40, 100, ULTRAFAST, 1500,   1, "Phoenix"        , "PHOENIX.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// centaur
	{   3,   1,   8,   1,   2,   5,   AVERAGE,   60,   8, "Centaur"        , "CENTAUR.ICN" },
	// gargoyle
	{   4,   7,   0,   2,   3,  15,  VERYFAST,  200,   6, "Gargoyle"       , "GARGOYLE.ICN" },
	// griffin
	{   6,   6,   0,   3,   5,  25,   AVERAGE,  300,   4, "Griffin"        , "GRIFFIN.ICN" },
	// minotaur
	{   9,   8,   0,   5,  10,  35,   AVERAGE,  400,   3, "Minotaur"       , "MINOTAUR.ICN" },
	// knight minotaur
	{   9,   8,   0,   5,  10,  45,  VERYFAST,  500,   3, "Minotaur King"  , "MINOTAU2.ICN" },
	// hydra
	{   8,   9,   0,   6,  12,  75,  VERYSLOW,  800,   2, "Hydra"          , "HYDRA.ICN" },
	// green dragon
	{  12,  12,   0,  25,  50, 200,   AVERAGE, 3000,   1, "Green Dragon"   , "DRAGGREE.ICN" },
	// red dragon
	{  13,  13,   0,  25,  50, 250,      FAST, 3500,   1, "Red Dragon"     , "DRAGRED.ICN" },
	// black dragon
	{  14,  14,   0,  25,  50, 300,  VERYFAST, 4000,   1, "Black Dragon"   , "DRAGBLAK.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// halfling
	{   2,   1,  12,   1,   3,   3,      SLOW,   50,   8, "Halfling"       , "HALFLING.ICN" },
	// boar
	{   5,   4,   0,   2,   3,  15,  VERYFAST,  150,   6, "Boar"           , "BOAR.ICN" },
	// iron golem
	{   5,  10,   0,   4,   5,  30,  VERYSLOW,  300,   4, "Iron Golem"     , "GOLEM.ICN" },
	// steel golem
	{   7,  10,   0,   4,   5,  35,      SLOW,  350,   4, "Steel Golem"    , "GOLEM2.ICN" },
	// roc
	{   7,   7,   0,   4,   8,  40,   AVERAGE,  400,   3, "Roc"            , "ROC.ICN" },
	// mage
	{  11,   7,  12,   7,   9,  30,      FAST,  600,   2, "Mage"           , "MAGE1.ICN" },
	// archi mage
	{  12,   8,  24,   7,   9,  35,  VERYFAST,  700,   2, "Archmage"       , "MAGE2.ICN" },
	// giant
	{  13,  10,   0,  20,  30, 150,   AVERAGE, 2000,   1, "Giant"          , "TITANBLU.ICN" },
	// titan
	{  15,  15,  24,  20,  30, 300,  VERYFAST, 5000,   1, "Titan"          , "TITANBLA.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// skeleton
	{   4,   3,   0,   2,   3,   4,   AVERAGE,   75,   8, "Skeleton"       , "SKELETON.ICN" },
	// zombie
	{   5,   2,   0,   2,   3,  15,  VERYSLOW,  150,   6, "Zombie"         , "ZOMBIE.ICN" },
	// mutant zombie
	{   5,   2,   0,   2,   3,  25,   AVERAGE,  200,   6, "Mutant Zombie"  , "ZOMBIE2.ICN" },
	// mummy
	{   6,   6,   0,   3,   4,  25,   AVERAGE,  250,   4, "Mummy"          , "MUMMYW.ICN" },
	// royal mummy
	{   6,   6,   0,   3,   4,  30,      FAST,  300,   4, "Royal Mummy"    , "MUMMY2.ICN" },
	// vampire
	{   8,   6,   0,   5,   7,  30,   AVERAGE,  500,   3, "Vampire"        , "VAMPIRE.ICN" },
	// lord vampire
	{   8,   6,   0,   5,   7,  40,      FAST,  650,   3, "Lord Vampire"   , "VAMPIRE2.ICN" },
	// lich
	{   7,  12,  12,   8,  10,  25,      FAST,  750,   2, "Lich"           , "LICH.ICN" },
	// power lich
	{   7,  13,  24,   8,  10,  35,  VERYFAST,  900,   2, "Power Lich"     , "LICH2.ICN" },
	// bone dragon
	{  11,   9,   0,  25,  45, 150,   AVERAGE, 1500,   1, "Bone Dragon"    , "DRAGBONE.ICN" },

	//atck dfnc shts  min  max   hp      speed  cost grwn  name            file
	// rogue
	{   6,   1,   0,   1,   2,   4,      FAST,   50,   4, "Rogue"          , "ROGUE.ICN" },
	// nomad
	{   7,   6,   0,   2,   5,  20,  VERYFAST,  200,   4, "Nomad"          , "NOMAD.ICN" },
	// ghost
	{   8,   7,   0,   4,   6,  20,      FAST, 1000,   4, "Ghost"          , "GHOST.ICN" },
	// medusa
	{   8,   9,   0,   6,  10,  35,   AVERAGE,  500,   4, "Medusa"         , "MEDUSA.ICN" },
	// genie
	{  10,   9,   0,  20,  30,  50,  VERYFAST,  650,   4, "Genie"          , "GENIE.ICN" },
	// earth element
	{   8,   8,   0,   4,   5,  50,      SLOW,  500,   4, "Earth Element"  , "EELEM.ICN" },
	// air element
	{   7,   7,   0,   2,   8,  35,  VERYFAST,  500,   4, "Air Element"    , "AELEM.ICN" },
	// fire element
	{   8,   6,   0,   4,   6,  40,      FAST,  500,   4, "Fire Element"   , "FELEM.ICN" },
	// water element
	{   6,   8,   0,   3,   7,  45,   AVERAGE,  500,   4, "Water Element"  , "WELEM.ICN" },
	
	// unknown
	{    0,   0,   0,   0,   0,   0,  VERYSLOW, 9999,   0, "Unknown"       , "UNKNOWN.ICN" }
    };

}

/* get stats monster */
const Monster::stats_t & Monster::GetStats(monster_t monster)
{ return (WATER_ELEMENT < monster ? all_monsters[WATER_ELEMENT + 1] : all_monsters[monster]); }

/* get string name */
const std::string & Monster::String(monster_t monster)
{ return Monster::GetStats(monster).name; }
