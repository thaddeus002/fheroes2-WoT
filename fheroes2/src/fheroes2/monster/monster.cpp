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

#define ANIMATTNONE {1,1,1,1,1,1,1,1}
#define ANIM_NONE {1,1,1,1,1,1,1,1,ANIMATTNONE,ANIMATTNONE}
namespace Monster {

    static const stats_t all_monsters[] = {
	// fly   wide   monster         atck dfnc shts  min  max   hp             speed grwn  name            file                                            walk  idle  pain   die     attpr  att1   att2   att3   
	{ false, false, PEASANT,           1,   1,   0,   1,   1,   1,  Speed::VERYSLOW,  12, "Peasant"        , ICN::PEASANT , ICN::MONH0000, ICN::UNKNOWN, {5, 8, 1, 4, 13, 3, 34, 4, {16, 3, 19, 4, 23, 6, 29, 5}, ANIMATTNONE },                   M82::PSNTATTK, M82::UNKNOWN, M82::PSNTKILL, M82::PSNTMOVE, M82::UNKNOWN, M82::PSNTWNCE },
	{ false, false, ARCHER,            5,   3,  12,   2,   3,  10,  Speed::VERYSLOW,   8, "Archer"         , ICN::ARCHER  , ICN::MONH0001, ICN::ARCH_MSL, {5, 8, 1, 4, 13, 3, 45, 6, {32, 4, 36, 3, 39, 3, 42, 3}, {16, 4, 20, 4, 24, 4, 28, 4} }, M82::ARCHATTK, M82::UNKNOWN, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHSHOT, M82::ARCHWNCE },
	{ false, false, RANGER,            5,   3,  24,   2,   3,  10,   Speed::AVERAGE,   8, "Ranger"         , ICN::ARCHER2 , ICN::MONH0002, ICN::ARCH_MSL, {5, 8, 1, 4, 13, 3, 45, 6, {32, 4, 36, 3, 39, 3, 42, 3}, {16, 4, 20, 4, 24, 4, 28, 4} }, M82::ARCHATTK, M82::UNKNOWN, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHSHOT, M82::ARCHWNCE },
	{ false, false, PIKEMAN,           5,   9,   0,   3,   4,  15,   Speed::AVERAGE,   5, "Pikeman"        , ICN::PIKEMAN , ICN::MONH0003, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 30, 6, {1, 1, 17, 3, 20, 4, 24, 6}, ANIMATTNONE },                    M82::PIKEATTK, M82::UNKNOWN, M82::PIKEKILL, M82::PIKEMOVE, M82::UNKNOWN, M82::PIKEWNCE },
	{ false, false, VETERAN_PIKEMAN,   5,   9,   0,   3,   4,  20,      Speed::FAST,   5, "Veteran Pikeman", ICN::PIKEMAN2, ICN::MONH0004, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 30, 6, {1, 1, 17, 3, 20, 4, 24, 6}, ANIMATTNONE },                    M82::PIKEATTK, M82::UNKNOWN, M82::PIKEKILL, M82::PIKEMOVE, M82::UNKNOWN, M82::PIKEWNCE },
	{ false, false, SWORDSMAN,         7,   9,   0,   4,   6,  25,   Speed::AVERAGE,   4, "Swordsman"      , ICN::SWORDSMN, ICN::MONH0005, ICN::UNKNOWN, {1, 11, 39, 6, 36, 3, 29, 7, {1, 1, 19, 5, 12, 7, 24, 5}, ANIMATTNONE },                  M82::SWDMATTK, M82::UNKNOWN, M82::SWDMKILL, M82::SWDMMOVE, M82::UNKNOWN, M82::SWDMWNCE },
	{ false, false, MASTER_SWORDSMAN,  7,   9,   0,   4,   6,  30,      Speed::FAST,   4, "Master Swordman", ICN::SWORDSM2, ICN::MONH0006, ICN::UNKNOWN, {1, 11, 39, 6, 36, 3, 29, 7, {1, 1, 19, 5, 12, 7, 24, 5}, ANIMATTNONE },                  M82::SWDMATTK, M82::UNKNOWN, M82::SWDMKILL, M82::SWDMMOVE, M82::UNKNOWN, M82::SWDMWNCE },
	{ false, true,  CAVALRY,          10,   9,   0,   5,   9,  30,  Speed::VERYFAST,   3, "Cavalry"        , ICN::CAVALRYR, ICN::MONH0007, ICN::UNKNOWN, {1, 7, 19, 4, 17, 2, 23, 5, {8, 2, 12, 3, 10, 2, 15, 2}, ANIMATTNONE },                   M82::CAVLATTK, M82::UNKNOWN, M82::CAVLKILL, M82::CAVLMOVE, M82::UNKNOWN, M82::CAVLWNCE },
	{ false, true,  CHAMPION,         10,   9,   0,   5,  10,  40, Speed::ULTRAFAST,   3, "Champion"       , ICN::CAVALRYB, ICN::MONH0008, ICN::UNKNOWN, {1, 7, 19, 4, 17, 2, 23, 5, {8, 2, 12, 3, 10, 2, 15, 2}, ANIMATTNONE },                   M82::CAVLATTK, M82::UNKNOWN, M82::CAVLKILL, M82::CAVLMOVE, M82::UNKNOWN, M82::CAVLWNCE },
	{ false, false, PALADIN,          11,  12,   0,  10,  20,  50,      Speed::FAST,   2, "Paladin"        , ICN::PALADIN , ICN::MONH0009, ICN::UNKNOWN, {12, 8, 1, 11, 32, 2, 34, 5, {20, 4, 24, 2, 26, 3, 29, 3}, ANIMATTNONE },                 M82::PLDNATTK, M82::UNKNOWN, M82::PLDNKILL, M82::PLDNMOVE, M82::UNKNOWN, M82::PLDNWNCE },
	{ false, false, CRUSADER,         11,  12,   0,  10,  20,  65,  Speed::VERYFAST,   2, "Crusader"       , ICN::PALADIN2, ICN::MONH0010, ICN::UNKNOWN, {12, 8, 1, 11, 32, 2, 34, 5, {20, 4, 24, 2, 26, 3, 29, 3}, ANIMATTNONE },                 M82::PLDNATTK, M82::UNKNOWN, M82::PLDNKILL, M82::PLDNMOVE, M82::UNKNOWN, M82::PLDNWNCE },

	// fly   wide   monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ false, false, GOBLIN,            3,   1,   0,   1,   2,   3,   Speed::AVERAGE,  10, "Goblin"         , ICN::GOBLIN  , ICN::MONH0011, ICN::UNKNOWN, {1, 9, 33, 7, 25, 3, 28, 5, {10, 3, 17, 4, 13, 4, 21, 4}, ANIMATTNONE },                     M82::GBLNATTK, M82::UNKNOWN, M82::GBLNKILL, M82::GBLNMOVE, M82::UNKNOWN, M82::GBLNWNCE },
	{ false, false, ORC,               3,   4,   8,   2,   3,  10,  Speed::VERYSLOW,   8, "Orc"            , ICN::ORC     , ICN::MONH0012, ICN::ORC__MSL, {5, 8, 1, 4, 13, 3, 40, 4, {28, 2, 30, 3, 33, 3, 36, 4}, {1, 1, 16, 12, 16, 12, 16, 12} },  M82::ORC_ATTK, M82::UNKNOWN, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_SHOT, M82::ORC_WNCE },
	{ false, false, CHIEF_ORC,         3,   4,  16,   3,   4,  15,      Speed::SLOW,   8, "Orc Chief"      , ICN::ORC2    , ICN::MONH0013, ICN::ORC__MSL, {5, 8, 1, 4, 13, 3, 40, 4, {28, 2, 30, 3, 33, 3, 36, 4}, {1, 1, 16, 12, 16, 12, 16, 12} },  M82::ORC_ATTK, M82::UNKNOWN, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_SHOT, M82::ORC_WNCE },
	{ false, true,  WOLF,              6,   2,   0,   3,   5,  20,  Speed::VERYFAST,   5, "Wolf"           , ICN::WOLF    , ICN::MONH0014, ICN::UNKNOWN, {7, 6, 20, 6, 13, 4, 26, 7, {1, 3, 4, 3, 4, 3, 17, 3}, ANIMATTNONE },                        M82::WOLFATTK, M82::UNKNOWN, M82::WOLFKILL, M82::WOLFMOVE, M82::UNKNOWN, M82::WOLFWNCE },
	{ false, false, OGRE,              9,   5,   0,   4,   6,  40,  Speed::VERYSLOW,   4, "Ogre"           , ICN::OGRE    , ICN::MONH0015, ICN::UNKNOWN, {5, 8, 1, 4, 13, 3, 37, 4, {1, 1, 16, 6, 22, 7, 29, 8}, ANIMATTNONE },                       M82::OGREATTK, M82::UNKNOWN, M82::OGREKILL, M82::OGREMOVE, M82::UNKNOWN, M82::OGREWNCE },
	{ false, false, LORD_OGRE,         9,   5,   0,   5,   7,  60,   Speed::AVERAGE,   4, "Ogre Lord"      , ICN::OGRE2   , ICN::MONH0016, ICN::UNKNOWN, {5, 8, 1, 4, 13, 3, 37, 4, {1, 1, 16, 6, 22, 7, 29, 8}, ANIMATTNONE },                       M82::OGREATTK, M82::UNKNOWN, M82::OGREKILL, M82::OGREMOVE, M82::UNKNOWN, M82::OGREWNCE },
	{ false, false, TROLL,            10,   5,   8,   5,   7,  40,   Speed::AVERAGE,   3, "Troll"          , ICN::TROLL   , ICN::MONH0017, ICN::TROLLMSL, {1, 15, 16, 7, 53, 3, 56, 10, {1, 1, 48, 5, 48, 5, 48, 5}, {34, 14, 23, 6, 23, 6, 29, 5} }, M82::TRLLATTK, M82::UNKNOWN, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLSHOT, M82::TRLLWNCE },
	{ false, false, WAR_TROLL,        10,   5,  16,   7,   9,  40,      Speed::FAST,   3, "War Troll"      , ICN::TROLL2  , ICN::MONH0018, ICN::TROLLMSL, {1, 15, 16, 7, 53, 3, 56, 10, {1, 1, 48, 5, 48, 5, 48, 5}, {34, 14, 23, 6, 23, 6, 29, 5} }, M82::TRLLATTK, M82::UNKNOWN, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLSHOT, M82::TRLLWNCE },
	{ false, false, CYCLOPS,          12,   9,   0,  12,  24,  80,      Speed::FAST,   2, "Cyclops"        , ICN::CYCLOPS , ICN::MONH0019, ICN::UNKNOWN, {1, 7, 30, 9, 23, 2, 25, 5, {1, 1, 13, 5, 8, 5, 18, 5}, ANIMATTNONE },                       M82::CYCLATTK, M82::UNKNOWN, M82::CYCLKILL, M82::CYCLMOVE, M82::UNKNOWN, M82::CYCLWNCE },

	// fly   wide   monster        atck dfnc shts  min  max   hp             speed grwn  name            file
	{ true,  false, SPRITE,            4,   2,   0,   1,   2,   2,   Speed::AVERAGE,   8, "Sprite"         , ICN::SPRITE  , ICN::MONH0020, ICN::UNKNOWN, {1, 8, 16, 9, 9, 2, 11, 5, {25, 1, 31, 5, 26, 5, 36, 5}, ANIMATTNONE },                       M82::SPRTATTK, M82::UNKNOWN, M82::SPRTKILL, M82::SPRTMOVE, M82::UNKNOWN, M82::SPRTWNCE },
	{ false, false, DWARF,             6,   5,   0,   2,   4,  20,  Speed::VERYSLOW,   6, "Dwarf"          , ICN::DWARF   , ICN::MONH0021, ICN::UNKNOWN, {1, 9, 45, 4, 38, 7, 49, 7, {1, 1, 19, 8, 10, 9, 27, 11 }, ANIMATTNONE },                     M82::DWRFATTK, M82::UNKNOWN, M82::DWRFKILL, M82::DWRFMOVE, M82::UNKNOWN, M82::DWRFWNCE },
	{ false, false, BATTLE_DWARF,      6,   6,   0,   2,   4,  20,   Speed::AVERAGE,   6, "Battle Dwarf"   , ICN::DWARF2  , ICN::MONH0022, ICN::UNKNOWN, {1, 9, 45, 4, 38, 7, 49, 7, {1, 1, 19, 8, 10, 9, 27, 11 }, ANIMATTNONE },                     M82::DWRFATTK, M82::UNKNOWN, M82::DWRFKILL, M82::DWRFMOVE, M82::UNKNOWN, M82::DWRFWNCE },
	{ false, false, ELF,               4,   3,  24,   2,   3,  15,   Speed::AVERAGE,   4, "Elf"            , ICN::ELF     , ICN::MONH0023, ICN::ELF__MSL, {1, 12, 42, 6, 36, 2, 36, 6, {23, 3, 26, 3, 29, 4, 33, 3}, {1, 1, 13, 10, 13, 10, 13, 10} }, M82::ELF_ATTK, M82::UNKNOWN, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_SHOT, M82::ELF_WNCE },
	{ false, false, GRAND_ELF,         5,   5,  24,   2,   3,  15,  Speed::VERYFAST,   4, "Grand Elf"      , ICN::ELF2    , ICN::MONH0024, ICN::ELF__MSL, {1, 12, 42, 6, 36, 2, 36, 6, {23, 3, 26, 3, 29, 4, 33, 3}, {1, 1, 13, 10, 13, 10, 13, 10} }, M82::ELF_ATTK, M82::UNKNOWN, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_SHOT, M82::ELF_WNCE },
	{ false, false, DRUID,             7,   5,   8,   5,   8,  25,      Speed::FAST,   3, "Druid"          , ICN::DRUID   , ICN::MONH0025, ICN::DRUIDMSL, {1, 13, 46, 4, 44, 2, 26, 9, {14, 8, 35, 4, 22, 4, 39, 5}, {14, 8, 35, 4, 22, 4, 39, 5} },   M82::DRUIATTK, M82::UNKNOWN, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUISHOT, M82::DRUIWNCE },
	{ false, false, GREATER_DRUID,     7,   7,  16,   5,   8,  25,  Speed::VERYFAST,   3, "Greater Druid"  , ICN::DRUID2  , ICN::MONH0026, ICN::DRUIDMSL, {1, 13, 46, 4, 44, 2, 26, 9, {14, 8, 35, 4, 22, 4, 39, 5}, {14, 8, 35, 4, 22, 4, 39, 5} },   M82::DRUIATTK, M82::UNKNOWN, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUISHOT, M82::DRUIWNCE },
	{ false, true,  UNICORN,          10,   9,   0,   7,  14,  40,      Speed::FAST,   2, "Unicorn"        , ICN::UNICORN , ICN::MONH0027, ICN::UNKNOWN, {10, 8, 1, 9, 41, 3, 44, 8, {18, 2, 20, 7, 27, 7, 34, 7}, ANIMATTNONE },                      M82::UNICATTK, M82::UNKNOWN, M82::UNICKILL, M82::UNICMOVE, M82::UNKNOWN, M82::UNICWNCE },
	{ true,  true,  PHOENIX,          12,  10,   0,  20,  40, 100, Speed::ULTRAFAST,   1, "Phoenix"        , ICN::PHOENIX , ICN::MONH0028, ICN::UNKNOWN, {1, 9, 30, 5, 35, 1, 35, 13, {10, 1, 14, 4, 11, 3, 18, 5 }, ANIMATTNONE },                    M82::PHOEATTK, M82::UNKNOWN, M82::PHOEKILL, M82::PHOEMOVE, M82::UNKNOWN, M82::PHOEWNCE },

	// fly   wide   monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ false, true,  CENTAUR,           3,   1,   8,   1,   2,   5,   Speed::AVERAGE,   8, "Centaur"        , ICN::CENTAUR , ICN::MONH0029, ICN::ARCH_MSL, {1, 7, 64, 7, 58, 4, 46, 9, {1, 1, 38, 4, 34, 4, 42, 4 }, {18, 4, 27, 3, 22, 5, 30, 4} },    M82::CNTRATTK, M82::UNKNOWN, M82::CNTRKILL, M82::CNTRMOVE, M82::CNTRSHOT, M82::CNTRWNCE },
	{ true,  false, GARGOYLE,          4,   7,   0,   2,   3,  15,  Speed::VERYFAST,   6, "Gargoyle"       , ICN::GARGOYLE, ICN::MONH0030, ICN::UNKNOWN, {5, 7, 1, 4, 12, 3, 27, 5, {1, 1, 15, 4, 19, 4, 23, 4}, ANIMATTNONE },                        M82::GARGATTK, M82::UNKNOWN, M82::GARGKILL, M82::GARGMOVE, M82::UNKNOWN, M82::GARGWNCE },
	{ true,  true,  GRIFFIN,           6,   6,   0,   3,   5,  25,   Speed::AVERAGE,   4, "Griffin"        , ICN::GRIFFIN , ICN::MONH0031, ICN::UNKNOWN, {1, 6, 16, 9, 25, 3, 25, 9, {7, 1, 8, 3, 15, 1, 11, 4}, ANIMATTNONE },                        M82::GRIFATTK, M82::UNKNOWN, M82::GRIFKILL, M82::GRIFMOVE, M82::UNKNOWN, M82::GRIFWNCE },
	{ false, false, MINOTAUR,          9,   8,   0,   5,  10,  35,   Speed::AVERAGE,   3, "Minotaur"       , ICN::MINOTAUR, ICN::MONH0032, ICN::UNKNOWN, {6, 7, 1, 5, 27, 3, 30, 5, {13, 3, 23, 4, 16, 4, 20, 3}, ANIMATTNONE },                       M82::MINOATTK, M82::UNKNOWN, M82::MINOKILL, M82::MINOMOVE, M82::UNKNOWN, M82::MINOWNCE },
	{ false, false, KNIGHT_MINOTAUR,   9,   8,   0,   5,  10,  45,  Speed::VERYFAST,   3, "Minotaur King"  , ICN::MINOTAU2, ICN::MONH0033, ICN::UNKNOWN, {6, 7, 1, 5, 27, 3, 30, 5, {13, 3, 23, 4, 16, 4, 20, 3}, ANIMATTNONE },                       M82::MINOATTK, M82::UNKNOWN, M82::MINOKILL, M82::MINOMOVE, M82::UNKNOWN, M82::MINOWNCE },
	{ false, false, HYDRA,             8,   9,   0,   6,  12,  75,  Speed::VERYSLOW,   2, "Hydra"          , ICN::HYDRA   , ICN::MONH0034, ICN::UNKNOWN, {1, 8, 28, 15, 16, 6, 22, 6, {1, 1, 9, 7, 9, 7, 9, 7 }, ANIMATTNONE },                        M82::HYDRATTK, M82::UNKNOWN, M82::HYDRKILL, M82::HYDRMOVE, M82::UNKNOWN, M82::HYDRWNCE },
	{ true,  true,  GREEN_DRAGON,     12,  12,   0,  25,  50, 200,   Speed::AVERAGE,   1, "Green Dragon"   , ICN::DRAGGREE, ICN::MONH0035, ICN::UNKNOWN, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                     M82::DRGNATTK, M82::UNKNOWN, M82::DRGNKILL, M82::DRGNMOVE, M82::UNKNOWN, M82::DRGNWNCE },
	{ true,  true,  RED_DRAGON,       13,  13,   0,  25,  50, 250,      Speed::FAST,   1, "Red Dragon"     , ICN::DRAGRED , ICN::MONH0036, ICN::UNKNOWN, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                     M82::DRGNATTK, M82::UNKNOWN, M82::DRGNKILL, M82::DRGNMOVE, M82::UNKNOWN, M82::DRGNWNCE },
	{ true,  true,  BLACK_DRAGON,     14,  14,   0,  25,  50, 300,  Speed::VERYFAST,   1, "Black Dragon"   , ICN::DRAGBLAK, ICN::MONH0037, ICN::UNKNOWN, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                     M82::DRGNATTK, M82::UNKNOWN, M82::DRGNKILL, M82::DRGNMOVE, M82::UNKNOWN, M82::DRGNWNCE },

	// fly   wide   monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ false, false, HALFLING,          2,   1,  12,   1,   3,   3,      Speed::SLOW,   8, "Halfling"       , ICN::HALFLING, ICN::MONH0038, ICN::HALFLMSL, {5, 8, 1, 4, 13, 4, 37, 4, {1, 1, 29, 3, 32, 3, 35, 2}, {17, 6, 23, 3, 26, 1, 27, 2} },      M82::HALFATTK, M82::UNKNOWN, M82::HALFKILL, M82::HALFMOVE, M82::HALFSHOT, M82::HALFWNCE },
	{ false, true,  BOAR,              5,   4,   0,   2,   3,  15,  Speed::VERYFAST,   6, "Boar"           , ICN::BOAR    , ICN::MONH0039, ICN::UNKNOWN, {5, 8, 1, 4, 13, 3, 24, 5, {16, 1, 17, 2, 19, 3, 22, 2}, ANIMATTNONE },                       M82::BOARATTK, M82::UNKNOWN, M82::BOARKILL, M82::BOARMOVE, M82::UNKNOWN, M82::BOARWNCE },
	{ false, false, IRON_GOLEM,        5,  10,   0,   4,   5,  30,  Speed::VERYSLOW,   4, "Iron Golem"     , ICN::GOLEM   , ICN::MONH0040, ICN::UNKNOWN, {1, 12, 34, 6, 32, 2, 26, 6, {13, 3, 16, 3, 19, 4, 23, 3}, ANIMATTNONE },                     M82::GOLMATTK, M82::UNKNOWN, M82::GOLMKILL, M82::GOLMMOVE, M82::UNKNOWN, M82::GOLMWNCE },
	{ false, false, STEEL_GOLEM,       7,  10,   0,   4,   5,  35,      Speed::SLOW,   4, "Steel Golem"    , ICN::GOLEM2  , ICN::MONH0041, ICN::UNKNOWN, {1, 12, 34, 6, 32, 2, 26, 6, {13, 3, 16, 3, 19, 4, 23, 3}, ANIMATTNONE },                     M82::GOLMATTK, M82::UNKNOWN, M82::GOLMKILL, M82::GOLMMOVE, M82::UNKNOWN, M82::GOLMWNCE },
	{ true,  true,  ROC,               7,   7,   0,   4,   8,  40,   Speed::AVERAGE,   3, "Roc"            , ICN::ROC     , ICN::MONH0042, ICN::UNKNOWN, {1, 8, 18, 4, 22, 2, 24, 10, {1, 1, 15, 3, 9, 2, 11, 4, }, ANIMATTNONE },                     M82::ROC_ATTK, M82::UNKNOWN, M82::ROC_KILL, M82::ROC_MOVE, M82::UNKNOWN, M82::ROC_WNCE },
	{ false, false, MAGE,             11,   7,  12,   7,   9,  30,      Speed::FAST,   2, "Mage"           , ICN::MAGE1   , ICN::MONH0043, ICN::DRUIDMSL, {43, 8, 1, 7, 8, 4, 12, 6, {27, 2, 34, 3, 29, 5, 37, 6}, {18, 1, 21, 2, 19, 2, 23, 4} },     M82::MAGEATTK, M82::UNKNOWN, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGESHOT, M82::MAGEWNCE },
	{ false, false, ARCHMAGE,         12,   8,  24,   7,   9,  35,  Speed::VERYFAST,   2, "Archmage"       , ICN::MAGE2   , ICN::MONH0044, ICN::DRUIDMSL, {43, 8, 1, 7, 8, 4, 12, 6, {27, 2, 34, 3, 29, 5, 37, 6}, {18, 1, 21, 2, 19, 2, 23, 4} },     M82::MAGEATTK, M82::UNKNOWN, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGESHOT, M82::MAGEWNCE },
	{ false, false, GIANT,            13,  10,   0,  20,  30, 150,   Speed::AVERAGE,   1, "Giant"          , ICN::TITANBLU, ICN::MONH0045, ICN::UNKNOWN, {7, 7, 1, 6, 29, 3, 32, 6, {1, 1, 19, 5, 14, 5, 24, 5}, ANIMATTNONE },                        M82::TITNATTK, M82::UNKNOWN, M82::TITNKILL, M82::TITNMOVE, M82::TITNSHOT, M82::TITNWNCE },
	{ false, false, TITAN,            15,  15,  24,  20,  30, 300,  Speed::VERYFAST,   1, "Titan"          , ICN::TITANBLA, ICN::MONH0046, ICN::TITANMSL, {7, 7, 1, 6, 29, 3, 32, 6, {1, 1, 19, 5, 14, 5, 24, 5}, {1, 1, 45, 5, 38, 6, 50, 6} },       M82::TITNATTK, M82::UNKNOWN, M82::TITNKILL, M82::TITNMOVE, M82::TITNSHOT, M82::TITNWNCE },

	// fly   wide   monster         atck dfnc shts  min  max   hp             speed grwn  name            file
	{ false, false, SKELETON,          4,   3,   0,   2,   3,   4,   Speed::AVERAGE,   8, "Skeleton"       , ICN::SKELETON, ICN::MONH0047, ICN::UNKNOWN, {4, 8, 35, 4, 25, 3, 28, 7, {1, 1, 20, 5, 12, 4, 16, 4}, ANIMATTNONE },                      M82::SKELATTK, M82::UNKNOWN, M82::SKELKILL, M82::SKELMOVE, M82::UNKNOWN, M82::SKELWNCE },
	{ false, false, ZOMBIE,            5,   2,   0,   2,   3,  15,  Speed::VERYSLOW,   6, "Zombie"         , ICN::ZOMBIE  , ICN::MONH0048, ICN::UNKNOWN, {1, 13, 14, 12, 40, 7, 47, 7, {1, 1, 26, 6, 32, 5, 37, 3}, ANIMATTNONE },                     M82::ZOMBATTK, M82::UNKNOWN, M82::ZOMBKILL, M82::ZOMBMOVE, M82::UNKNOWN, M82::ZOMBWNCE },
	{ false, false, MUTANT_ZOMBIE,     5,   2,   0,   2,   3,  25,   Speed::AVERAGE,   6, "Mutant Zombie"  , ICN::ZOMBIE2 , ICN::MONH0049, ICN::UNKNOWN, {1, 13, 14, 12, 40, 7, 47, 7, {1, 1, 26, 6, 32, 5, 37, 3}, ANIMATTNONE },                     M82::ZOMBATTK, M82::UNKNOWN, M82::ZOMBKILL, M82::ZOMBMOVE, M82::UNKNOWN, M82::ZOMBWNCE },
	{ false, false, MUMMY,             6,   6,   0,   3,   4,  25,   Speed::AVERAGE,   4, "Mummy"          , ICN::MUMMYW  , ICN::MONH0050, ICN::UNKNOWN, {5, 12, 1, 4, 17, 4, 30, 10, {1, 1, 21, 3, 24, 3, 27, 3}, ANIMATTNONE },                      M82::MUMYATTK, M82::UNKNOWN, M82::MUMYKILL, M82::MUMYMOVE, M82::UNKNOWN, M82::MUMYWNCE },
	{ false, false, ROYAL_MUMMY,       6,   6,   0,   3,   4,  30,      Speed::FAST,   4, "Royal Mummy"    , ICN::MUMMY2  , ICN::MONH0051, ICN::UNKNOWN, {5, 12, 1, 4, 17, 4, 30, 10, {1, 1, 21, 3, 24, 3, 27, 3}, ANIMATTNONE },                      M82::MUMYATTK, M82::UNKNOWN, M82::MUMYKILL, M82::MUMYMOVE, M82::UNKNOWN, M82::MUMYWNCE },
	{ true,  false, VAMPIRE,           8,   6,   0,   5,   7,  30,   Speed::AVERAGE,   3, "Vampire"        , ICN::VAMPIRE , ICN::MONH0052, ICN::UNKNOWN, {5, 8, 1, 4, 13, 2, 28, 8, {1, 1, 15, 4, 19, 4, 23, 5}, ANIMATTNONE },                        M82::VAMPATTK, M82::VAMPEXT1, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPEXT2, M82::VAMPWNCE },
	{ true,  false, LORD_VAMPIRE,      8,   6,   0,   5,   7,  40,      Speed::FAST,   3, "Lord Vampire"   , ICN::VAMPIRE2, ICN::MONH0053, ICN::UNKNOWN, {5, 8, 1, 4, 13, 2, 28, 8, {1, 1, 15, 4, 19, 4, 23, 5}, ANIMATTNONE },                        M82::VAMPATTK, M82::VAMPEXT1, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPEXT2, M82::VAMPWNCE },
	{ false, false, LICH,              7,  12,  12,   8,  10,  25,      Speed::FAST,   2, "Lich"           , ICN::LICH    , ICN::MONH0054, ICN::LICH_MSL, {5, 8, 1, 4, 28, 3, 31, 7, {1, 1, 13, 5, 18, 5, 24, 4}, ANIMATTNONE },                       M82::LICHATTK, M82::LICHEXPL, M82::LICHKILL, M82::LICHMOVE, M82::LICHSHOT, M82::LICHWNCE },
	{ false, false, POWER_LICH,        7,  13,  24,   8,  10,  35,  Speed::VERYFAST,   2, "Power Lich"     , ICN::LICH2   , ICN::MONH0055, ICN::LICH_MSL, {5, 8, 1, 4, 28, 3, 31, 7, {1, 1, 13, 5, 18, 5, 24, 4}, ANIMATTNONE },                       M82::LICHATTK, M82::LICHEXPL, M82::LICHKILL, M82::LICHMOVE, M82::LICHSHOT, M82::LICHWNCE },
	{ true,  true,  BONE_DRAGON,      11,   9,   0,  25,  45, 150,   Speed::AVERAGE,   1, "Bone Dragon"    , ICN::DRAGBONE, ICN::MONH0056, ICN::UNKNOWN, {1, 12, 22, 7, 20, 2, 29, 12, {1, 1, 48, 4, 13, 3, 16, 4, }, ANIMATTNONE },                   M82::BONEATTK, M82::UNKNOWN, M82::BONEKILL, M82::BONEMOVE, M82::UNKNOWN, M82::BONEWNCE },

	// fly   wide   monster         atck dfnc shts  min  max   hp            speed grwn  name            file
	{ false, false, ROGUE,             6,   1,   0,   1,   2,   4,      Speed::FAST,   4, "Rogue"          , ICN::ROGUE   , ICN::MONH0057, ICN::UNKNOWN, {5, 8, 1, 4, 13, 3, 43, 7, {1, 1, 16, 7, 23, 9, 32, 11}, ANIMATTNONE },                       M82::ROGUATTK, M82::UNKNOWN, M82::ROGUKILL, M82::ROGUMOVE, M82::UNKNOWN, M82::ROGUWNCE },
	{ false, true,  NOMAD,             7,   6,   0,   2,   5,  20,  Speed::VERYFAST,   4, "Nomad"          , ICN::NOMAD   , ICN::MONH0058, ICN::UNKNOWN, {10, 8, 1, 9, 35, 2, 35, 7, {18, 2, 26, 5, 20, 6, 31, 4}, ANIMATTNONE },                      M82::NMADATTK, M82::UNKNOWN, M82::NMADKILL, M82::NMADMOVE, M82::UNKNOWN, M82::NMADWNCE },
	{ true,  false, GHOST,             8,   7,   0,   4,   6,  20,      Speed::FAST,   4, "Ghost"          , ICN::GHOST   , ICN::MONH0059, ICN::UNKNOWN, {1, 9, 1, 9, 20, 1, 20, 6, {10, 1, 11, 3, 14, 2, 16, 4}, ANIMATTNONE },                       M82::GHSTATTK, M82::UNKNOWN, M82::GHSTKILL, M82::GHSTMOVE, M82::UNKNOWN, M82::GHSTWNCE },
	{ false, true,  MEDUSA,            8,   9,   0,   6,  10,  35,   Speed::AVERAGE,   4, "Medusa"         , ICN::MEDUSA  , ICN::MONH0060, ICN::UNKNOWN, {1, 16, 17, 7, 36, 6, 24, 12, {42, 1, 65, 10, 43, 11, 54, 11}, ANIMATTNONE },                 M82::MEDSATTK, M82::UNKNOWN, M82::MEDSKILL, M82::MEDSMOVE, M82::UNKNOWN, M82::MEDSWNCE },
	{ true,  false, GENIE,            10,   9,   0,  20,  30,  50,  Speed::VERYFAST,   4, "Genie"          , ICN::GENIE   , ICN::MONH0061, ICN::UNKNOWN, {10, 5, 1, 9, 31, 2, 31, 9, {15, 3, 26, 5, 18, 3, 21, 5}, ANIMATTNONE },                      M82::GENIATTK, M82::UNKNOWN, M82::GENIKILL, M82::GENIMOVE, M82::UNKNOWN, M82::GENIWNCE },
	{ false, false, EARTH_ELEMENT,     8,   8,   0,   4,   5,  50,      Speed::SLOW,   4, "Earth Element"  , ICN::EELEM   , ICN::MONH0062, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                       M82::EELMATTK, M82::UNKNOWN, M82::EELMKILL, M82::EELMMOVE, M82::UNKNOWN, M82::EELMWNCE },
	{ false, false, AIR_ELEMENT,       7,   7,   0,   2,   8,  35,  Speed::VERYFAST,   4, "Air Element"    , ICN::AELEM   , ICN::MONH0063, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                       M82::AELMATTK, M82::UNKNOWN, M82::AELMKILL, M82::AELMMOVE, M82::UNKNOWN, M82::AELMWNCE },
	{ false, false, FIRE_ELEMENT,      8,   6,   0,   4,   6,  40,      Speed::FAST,   4, "Fire Element"   , ICN::FELEM   , ICN::MONH0064, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                       M82::FELMATTK, M82::UNKNOWN, M82::FELMKILL, M82::FELMMOVE, M82::UNKNOWN, M82::FELMWNCE },
	{ false, false, WATER_ELEMENT,     6,   8,   0,   3,   7,  45,   Speed::AVERAGE,   4, "Water Element"  , ICN::WELEM   , ICN::MONH0065, ICN::UNKNOWN, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                       M82::WELMATTK, M82::UNKNOWN, M82::WELMKILL, M82::WELMMOVE, M82::UNKNOWN, M82::WELMWNCE },
	
	// fly   wide   unknown
	{ false, false, UNKNOWN,           0,   0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Unknown"        , ICN::UNKNOWN , ICN::UNKNOWN, ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN }
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

void Monster::GetAnimFrames(monster_t monster, animstate_t anim, u8 & start, u8 & length, bool attranged)
{
    switch(anim) {
    case Monster::AS_NONE:
	start = 1;
	length = 1;
	break;
    case Monster::AS_WALK:
	start = all_monsters[monster].animation.walk_start;
	length = all_monsters[monster].animation.walk_count;
	break;
    case Monster::AS_IDLE:
	start = all_monsters[monster].animation.idle_start;
	length = all_monsters[monster].animation.idle_count;
	break;
    case Monster::AS_PAIN:
	start = all_monsters[monster].animation.pain_start;
	length = all_monsters[monster].animation.pain_count;
	break;
    case Monster::AS_DIE:
	start = all_monsters[monster].animation.die_start;
	length = all_monsters[monster].animation.die_count;
	break;
    case Monster::AS_ATTPREP:
	start = attranged ? all_monsters[monster].animation.ar.attprep_start : all_monsters[monster].animation.a.attprep_start;
	length = attranged ? all_monsters[monster].animation.ar.attprep_count : all_monsters[monster].animation.a.attprep_count;
	break;
    case Monster::AS_ATT1:
	start = attranged ? all_monsters[monster].animation.ar.attack1_start : all_monsters[monster].animation.a.attack1_start;
	length = attranged ? all_monsters[monster].animation.ar.attack1_count : all_monsters[monster].animation.a.attack1_count;
	break;
    case Monster::AS_ATT2:
	start = attranged ? all_monsters[monster].animation.ar.attack2_start : all_monsters[monster].animation.a.attack2_start;
	length = attranged ? all_monsters[monster].animation.ar.attack2_count : all_monsters[monster].animation.a.attack2_count;
	break;
    case Monster::AS_ATT3:
	start = attranged ? all_monsters[monster].animation.ar.attack3_start : all_monsters[monster].animation.a.attack3_start;
	length = attranged ? all_monsters[monster].animation.ar.attack3_count : all_monsters[monster].animation.a.attack3_count;
	break;
    default:
	start = 1;
	length = 1;
    }
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

void Monster::ChangeTileWithRNDMonster(Maps::Tiles & tile)
{
    Maps::TilesAddon *addon = tile.FindRNDMonster();

    u8 index = 0;

    switch(tile.GetObject())
    {
	case MP2::OBJ_RNDMONSTER:	index = Monster::Rand(); break;
	case MP2::OBJ_RNDMONSTER1:	index = Monster::Rand1();break;
	case MP2::OBJ_RNDMONSTER2:	index = Monster::Rand2();break;
	case MP2::OBJ_RNDMONSTER3:	index = Monster::Rand3();break;
	case MP2::OBJ_RNDMONSTER4:	index = Monster::Rand4();break;

	default: return;
    }
    
    if(addon)
    {
	addon->index = index;
	tile.SetObject(MP2::OBJ_MONSTER);
    }
    else
	Error::Warning("ChangeTileWithRNDMonster: FindRNDMonster return is NULL");
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

	default: break;
    }
    
    return LEVEL1;
}

/* get rnd count monster */
u16 Monster::GetRNDSize(monster_t monster)
{
    float level = 100;

    switch(Settings::Get().GameDifficulty())
    {
	case Difficulty::EASY:		level = 50; break;
	case Difficulty::NORMAL:	level = 80; break;
	case Difficulty::HARD:		level = 120; break;
	case Difficulty::EXPERT:	level = 150; break;
	case Difficulty::IMPOSSIBLE:	level = 175; break;
    }

    u16 randSize = 1;
    
    switch(GetLevel(monster))
    {
	case LEVEL1:
		    randSize = Rand::Get(Army::HORDE, static_cast<u32>(Army::THRONG * level / 100));
                    break;
	case LEVEL2:
		    randSize = Rand::Get(Army::LOTS, static_cast<u32>(Army::HORDE * level / 100));
                    break;
	case LEVEL3:
		    randSize = Rand::Get(Army::PACK, static_cast<u32>(Army::LOTS * level / 100));
                    break;
	case LEVEL4:
		    randSize = Rand::Get(Army::SEVERAL, static_cast<u32>(Army::PACK * level / 100));
                    break;
    }

    return static_cast<u16>(randSize * level / 100);
}

Monster::monster_t Monster::Monster(const Maps::Tiles & tile)
{
    Maps::TilesAddon * addons = const_cast<Maps::Tiles &>(tile).FindMonster();

    return (addons ? Monster::Monster(addons->index) : Monster::UNKNOWN);
}

u16 Monster::GetSize(const Maps::Tiles & tile)
{
    return tile.GetCountMonster();
}

u32 Monster::Dwelling(const monster_t monster)
{
    switch(monster)
    {
	case PEASANT:
	case GOBLIN:
	case SPRITE:
	case CENTAUR:
	case HALFLING:
	case SKELETON:
	    return Castle::DWELLING_MONSTER1;

	case ARCHER:
	case ORC:
	case ZOMBIE:
	case DWARF:
	case GARGOYLE:
	case BOAR:
	    return Castle::DWELLING_MONSTER2;

	case RANGER:
	case CHIEF_ORC:
	case BATTLE_DWARF:
	case MUTANT_ZOMBIE:
	    return Castle::DWELLING_UPGRADE2;

	case PIKEMAN:
	case WOLF:
	case ELF:
	case IRON_GOLEM:
	case MUMMY:
	case GRIFFIN:
	    return Castle::DWELLING_MONSTER3;

	case VETERAN_PIKEMAN:
	case GRAND_ELF:
	case STEEL_GOLEM:
	case ROYAL_MUMMY:
	    return Castle::DWELLING_UPGRADE3;

	case SWORDSMAN:
	case OGRE:
	case DRUID:
	case MINOTAUR:
	case ROC:
	case VAMPIRE:
	    return Castle::DWELLING_MONSTER4;

	case MASTER_SWORDSMAN:
	case LORD_OGRE:
	case GREATER_DRUID:
	case KNIGHT_MINOTAUR:
	case LORD_VAMPIRE:
	    return Castle::DWELLING_UPGRADE4;

	case CAVALRY:
	case TROLL:
	case MAGE:
	case LICH:
	case UNICORN:
	case HYDRA:
	    return Castle::DWELLING_MONSTER5;

	case CHAMPION:
	case WAR_TROLL:
	case ARCHMAGE:
	case POWER_LICH:
	    return Castle::DWELLING_UPGRADE5;

	case PALADIN:
	case CYCLOPS:
	case PHOENIX:
	case GREEN_DRAGON:
	case GIANT:
	case BONE_DRAGON:
	    return Castle::DWELLING_MONSTER6;

	case CRUSADER:
	case RED_DRAGON:
	case TITAN:
	    return Castle::DWELLING_UPGRADE6;

	case BLACK_DRAGON:
	    return Castle::DWELLING_UPGRADE7;
	
	default: break;
    }

    return 0;
}

Monster::monster_t Monster::Monster(const MP2::object_t obj)
{
    switch(obj)
    {
        case MP2::OBJ_WATCHTOWER:	return ORC;
        case MP2::OBJ_EXCAVATION:	return SKELETON;
        case MP2::OBJ_CAVE:		return CENTAUR;
        case MP2::OBJ_TREEHOUSE:	return SPRITE;
        case MP2::OBJ_ARCHERHOUSE:	return ARCHER;
        case MP2::OBJ_GOBLINHUT:	return GOBLIN;
        case MP2::OBJ_DWARFCOTT:	return DWARF;
        case MP2::OBJ_HALFLINGHOLE:	return HALFLING;
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT: 	return PEASANT;

	case MP2::OBJ_RUINS:		return MEDUSA;
        case MP2::OBJ_TREECITY:		return SPRITE;
        case MP2::OBJ_WAGONCAMP:	return ROGUE;
        case MP2::OBJ_TROLLBRIDGE:	return TROLL;
        case MP2::OBJ_DESERTTENT:	return NOMAD;

        case MP2::OBJ_ANCIENTLAMP:	return GENIE;

        default: break;
    }

    return UNKNOWN;
}

const std::string Monster::MultipleNames(monster_t monster)
{
    switch(monster)
    {
	case WOLF:		return "Wolves";
	case PIKEMAN:
	case VETERAN_PIKEMAN:
	case SWORDSMAN:
	case MASTER_SWORDSMAN:
	case WAR_TROLL:
	case PHOENIX:
	case MAGE:
	case ARCHMAGE:		return String(monster);
	case CAVALRY:		return "Cavalries";
	case CYCLOPS:		return "Cyclopes";
	case DWARF:		return "Dwarves";
	case BATTLE_DWARF:	return "Battle Dwarves";
	case ELF:		return "Elves";
	case GRAND_ELF:		return "Grand Elves";
	default: break;
    }

    return String(monster) + "s";
}
