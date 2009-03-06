/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "castle.h"
#include "difficulty.h"
#include "mp2.h"
#include "speed.h"
#include "maps_tiles.h"
#include "settings.h"
#include "luck.h"
#include "morale.h"
#include "monster.h"

static const struct
{
    u8 attack;
    u8 defence;
    u8 damageMin;
    u8 damageMax;
    u16 hp;
    Speed::speed_t speed;
    u8 grown;
    const std::string name;
    const std::string multiname;

} monsters[] = {
	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Unknown Monster", "Unknown Monsters" },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     1,   1,   1,   1,   1,  Speed::VERYSLOW,  12, _("Peasant")        , _("Peasants") },
	{     5,   3,   2,   3,  10,  Speed::VERYSLOW,   8, _("Archer")         , _("Archers") },
	{     5,   3,   2,   3,  10,   Speed::AVERAGE,   8, _("Ranger")         , _("Rangers") },
	{     5,   9,   3,   4,  15,   Speed::AVERAGE,   5, _("Pikeman")        , _("Pikeman") },
	{     5,   9,   3,   4,  20,      Speed::FAST,   5, _("Veteran Pikeman"), _("Veteran Pikeman") },
	{     7,   9,   4,   6,  25,   Speed::AVERAGE,   4, _("Swordsman")      , _("Swordsman") },
	{     7,   9,   4,   6,  30,      Speed::FAST,   4, _("Master Swordman"), _("Master Swordman") },
	{    10,   9,   5,   9,  30,  Speed::VERYFAST,   3, _("Cavalry")        , _("Cavalries") },
	{    10,   9,   5,  10,  40, Speed::ULTRAFAST,   3, _("Champion")       , _("Champions") },
	{    11,  12,  10,  20,  50,      Speed::FAST,   2, _("Paladin")        , _("Paladins") },
	{    11,  12,  10,  20,  65,  Speed::VERYFAST,   2, _("Crusader")       , _("Crusaders") },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     3,   1,   1,   2,   3,   Speed::AVERAGE,  10, _("Goblin")         , _("Goblins") },
	{     3,   4,   2,   3,  10,  Speed::VERYSLOW,   8, _("Orc")            , _("Orcs") },
	{     3,   4,   3,   4,  15,      Speed::SLOW,   8, _("Orc Chief")      , _("Orc Chiefs") },
	{     6,   2,   3,   5,  20,  Speed::VERYFAST,   5, _("Wolf")           , _("Wolves") },
	{     9,   5,   4,   6,  40,  Speed::VERYSLOW,   4, _("Ogre")           , _("Ogres") },
	{     9,   5,   5,   7,  60,   Speed::AVERAGE,   4, _("Ogre Lord")      , _("Ogre Lords") },
	{    10,   5,   5,   7,  40,   Speed::AVERAGE,   3, _("Troll")          , _("Trolls") },
	{    10,   5,   7,   9,  40,      Speed::FAST,   3, _("War Troll")      , _("War Troll") },
	{    12,   9,  12,  24,  80,      Speed::FAST,   2, _("Cyclops")        , _("Cyclopes") },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     4,   2,   1,   2,   2,   Speed::AVERAGE,   8, _("Sprite")         , _("Sprites") },
	{     6,   5,   2,   4,  20,  Speed::VERYSLOW,   6, _("Dwarf")          , _("Dwarves") },
	{     6,   6,   2,   4,  20,   Speed::AVERAGE,   6, _("Battle Dwarf")   , _("Battle Dwarves") },
	{     4,   3,   2,   3,  15,   Speed::AVERAGE,   4, _("Elf")            , _("Elves") },
	{     5,   5,   2,   3,  15,  Speed::VERYFAST,   4, _("Grand Elf")      , _("Grand Elves") },
	{     7,   5,   5,   8,  25,      Speed::FAST,   3, _("Druid")          , _("Druids") },
	{     7,   7,   5,   8,  25,  Speed::VERYFAST,   3, _("Greater Druid")  , _("Greater Druids") },
	{    10,   9,   7,  14,  40,      Speed::FAST,   2, _("Unicorn")        , _("Unicorns") },
	{    12,  10,  20,  40, 100, Speed::ULTRAFAST,   1, _("Phoenix")        , _("Phoenix") },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     3,   1,   1,   2,   5,   Speed::AVERAGE,   8, _("Centaur")        , _("Centaurs") },
	{     4,   7,   2,   3,  15,  Speed::VERYFAST,   6, _("Gargoyle")       , _("Gargoyles") },
	{     6,   6,   3,   5,  25,   Speed::AVERAGE,   4, _("Griffin")        , _("Griffins") },
	{     9,   8,   5,  10,  35,   Speed::AVERAGE,   3, _("Minotaur")       , _("Minotaurs") },
	{     9,   8,   5,  10,  45,  Speed::VERYFAST,   3, _("Minotaur King")  , _("Minotaur Kings") },
	{     8,   9,   6,  12,  75,  Speed::VERYSLOW,   2, _("Hydra")          , _("Hydras") },
	{    12,  12,  25,  50, 200,   Speed::AVERAGE,   1, _("Green Dragon")   , _("Green Dragons") },
	{    13,  13,  25,  50, 250,      Speed::FAST,   1, _("Red Dragon")     , _("Red Dragons") }, 
	{    14,  14,  25,  50, 300,  Speed::VERYFAST,   1, _("Black Dragon")   , _("Black Dragons") },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     2,   1,   1,   3,   3,      Speed::SLOW,   8, _("Halfling")       , _("Halflings") },
	{     5,   4,   2,   3,  15,  Speed::VERYFAST,   6, _("Boar")           , _("Boars") },
	{     5,  10,   4,   5,  30,  Speed::VERYSLOW,   4, _("Iron Golem")     , _("Iron Golems") },
	{     7,  10,   4,   5,  35,      Speed::SLOW,   4, _("Steel Golem")    , _("Steel Golems") },
	{     7,   7,   4,   8,  40,   Speed::AVERAGE,   3, _("Roc")            , _("Rocs") },
	{    11,   7,   7,   9,  30,      Speed::FAST,   2, _("Mage")           , _("Mage") },
	{    12,   8,   7,   9,  35,  Speed::VERYFAST,   2, _("Archmage")       , _("Archmage") },
	{    13,  10,  20,  30, 150,   Speed::AVERAGE,   1, _("Giant")          , _("Giants") },
	{    15,  15,  20,  30, 300,  Speed::VERYFAST,   1, _("Titan")          , _("Titans") },

	// atck dfnc  min  max   hp             speed grwn  name               multiname
	{     4,   3,   2,   3,   4,   Speed::AVERAGE,   8, _("Skeleton")       , _("Skeletons") },
	{     5,   2,   2,   3,  15,  Speed::VERYSLOW,   6, _("Zombie")         , _("Zombies") },
	{     5,   2,   2,   3,  25,   Speed::AVERAGE,   6, _("Mutant Zombie")  , _("Mutant Zombies") },
	{     6,   6,   3,   4,  25,   Speed::AVERAGE,   4, _("Mummy")          , _("Mummys") },
	{     6,   6,   3,   4,  30,      Speed::FAST,   4, _("Royal Mummy")    , _("Royal Mummys") },
	{     8,   6,   5,   7,  30,   Speed::AVERAGE,   3, _("Vampire")        , _("Vampires") },
	{     8,   6,   5,   7,  40,      Speed::FAST,   3, _("Lord Vampire")   , _("Lord Vampires") },
	{     7,  12,   8,  10,  25,      Speed::FAST,   2, _("Lich")           , _("Lichs") },
	{     7,  13,   8,  10,  35,  Speed::VERYFAST,   2, _("Power Lich")     , _("Power Lichs") }, 
	{    11,   9,  25,  45, 150,   Speed::AVERAGE,   1, _("Bone Dragon")    , _("Bone Dragons") },

        // atck dfnc  min  max   hp             speed grwn  name               multiname
	{     6,   1,   1,   2,   4,      Speed::FAST,   4, _("Rogue")          , _("Rogues") },
	{     7,   6,   2,   5,  20,  Speed::VERYFAST,   4, _("Nomad")          , _("Nomads") },
	{     8,   7,   4,   6,  20,      Speed::FAST,   4, _("Ghost")          , _("Ghosts") },
	{    10,   9,  20,  30,  50,  Speed::VERYFAST,   4, _("Genie")          , _("Genies") },
	{     8,   9,   6,  10,  35,   Speed::AVERAGE,   4, _("Medusa")         , _("Medusas") },
	{     8,   8,   4,   5,  50,      Speed::SLOW,   4, _("Earth Element")  , _("Earth Elements") },
	{     7,   7,   2,   8,  35,  Speed::VERYFAST,   4, _("Air Element")    , _("Air Elements") },
	{     8,   6,   4,   6,  40,      Speed::FAST,   4, _("Fire Element")   , _("Fire Elements") },
	{     6,   8,   3,   7,  45,   Speed::AVERAGE,   4, _("Water Element")  , _("Water Elements") },

	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Random Monster"  , "Random Monsters" },
	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Random Monster 1", "Random Monsters 3" },
	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Random Monster 2", "Random Monsters 2" },
	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Random Monster 3", "Random Monsters 3" },
	{     0,   0,   0,   0,   0,  Speed::VERYSLOW,   0, "Random Monster 4", "Random Monsters 4" },

};

#define ANIMATTNONE {1,1,1,1,1,1,1,1}
#define ANIM_NONE {1,1,1,1,1,1,1,1,ANIMATTNONE,ANIMATTNONE}

static const struct
{
    ICN::icn_t icn_file;
    ICN::icn_t icn_monh;
    Monster::anim_t animation;
    M82::m82_t m82_attk;
    M82::m82_t m82_kill;
    M82::m82_t m82_move;
    M82::m82_t m82_wnce;
} animonsters[] = {
    { ICN::UNKNOWN , ICN::UNKNOWN,  ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },

    { ICN::PEASANT , ICN::MONH0000, {5, 8, 1, 4, 13, 3, 34, 4, {16, 3, 19, 4, 23, 6, 29, 5}, ANIMATTNONE },                   M82::PSNTATTK, M82::PSNTKILL, M82::PSNTMOVE, M82::PSNTWNCE },
    { ICN::ARCHER  , ICN::MONH0001, {5, 8, 1, 4, 13, 3, 45, 6, {32, 4, 36, 3, 39, 3, 42, 3}, {16, 4, 20, 4, 24, 4, 28, 4} },  M82::ARCHATTK, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHWNCE },
    { ICN::ARCHER2 , ICN::MONH0002, {5, 8, 1, 4, 13, 3, 45, 6, {32, 4, 36, 3, 39, 3, 42, 3}, {16, 4, 20, 4, 24, 4, 28, 4} },  M82::ARCHATTK, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHWNCE },
    { ICN::PIKEMAN , ICN::MONH0003, {5, 8, 1, 4, 13, 4, 30, 6, {1, 1, 17, 3, 20, 4, 24, 6}, ANIMATTNONE },                    M82::PIKEATTK, M82::PIKEKILL, M82::PIKEMOVE, M82::PIKEWNCE },
    { ICN::PIKEMAN2, ICN::MONH0004, {5, 8, 1, 4, 13, 4, 30, 6, {1, 1, 17, 3, 20, 4, 24, 6}, ANIMATTNONE },                    M82::PIKEATTK, M82::PIKEKILL, M82::PIKEMOVE, M82::PIKEWNCE },
    { ICN::SWORDSMN, ICN::MONH0005, {1, 11, 39, 6, 36, 3, 29, 7, {1, 1, 19, 5, 12, 7, 24, 5}, ANIMATTNONE },                  M82::SWDMATTK, M82::SWDMKILL, M82::SWDMMOVE, M82::SWDMWNCE },
    { ICN::SWORDSM2, ICN::MONH0006, {1, 11, 39, 6, 36, 3, 29, 7, {1, 1, 19, 5, 12, 7, 24, 5}, ANIMATTNONE },                  M82::SWDMATTK, M82::SWDMKILL, M82::SWDMMOVE, M82::SWDMWNCE },
    { ICN::CAVALRYR, ICN::MONH0007, {1, 7, 19, 4, 17, 2, 23, 5, {8, 2, 12, 3, 10, 2, 15, 2}, ANIMATTNONE },                   M82::CAVLATTK, M82::CAVLKILL, M82::CAVLMOVE, M82::CAVLWNCE },
    { ICN::CAVALRYB, ICN::MONH0008, {1, 7, 19, 4, 17, 2, 23, 5, {8, 2, 12, 3, 10, 2, 15, 2}, ANIMATTNONE },                   M82::CAVLATTK, M82::CAVLKILL, M82::CAVLMOVE, M82::CAVLWNCE },
    { ICN::PALADIN , ICN::MONH0009, {12, 8, 1, 11, 32, 2, 34, 5, {20, 4, 24, 2, 26, 3, 29, 3}, ANIMATTNONE },                 M82::PLDNATTK, M82::PLDNKILL, M82::PLDNMOVE, M82::PLDNWNCE },
    { ICN::PALADIN2, ICN::MONH0010, {12, 8, 1, 11, 32, 2, 34, 5, {20, 4, 24, 2, 26, 3, 29, 3}, ANIMATTNONE },                 M82::PLDNATTK, M82::PLDNKILL, M82::PLDNMOVE, M82::PLDNWNCE },

    { ICN::GOBLIN  , ICN::MONH0011, {1, 9, 33, 7, 25, 3, 28, 5, {10, 3, 17, 4, 13, 4, 21, 4}, ANIMATTNONE },                   M82::GBLNATTK, M82::GBLNKILL, M82::GBLNMOVE, M82::GBLNWNCE },
    { ICN::ORC     , ICN::MONH0012, {5, 8, 1, 4, 13, 3, 40, 4, {28, 2, 30, 3, 33, 3, 36, 4}, {1, 1, 16, 12, 16, 12, 16, 12} }, M82::ORC_ATTK, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_WNCE },
    { ICN::ORC2    , ICN::MONH0013, {5, 8, 1, 4, 13, 3, 40, 4, {28, 2, 30, 3, 33, 3, 36, 4}, {1, 1, 16, 12, 16, 12, 16, 12} }, M82::ORC_ATTK, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_WNCE },
    { ICN::WOLF    , ICN::MONH0014, {7, 6, 20, 6, 13, 4, 26, 7, {1, 3, 4, 3, 4, 3, 17, 3}, ANIMATTNONE },                      M82::WOLFATTK, M82::WOLFKILL, M82::WOLFMOVE, M82::WOLFWNCE },
    { ICN::OGRE    , ICN::MONH0015, {5, 8, 1, 4, 13, 3, 37, 4, {1, 1, 16, 6, 22, 7, 29, 8}, ANIMATTNONE },                     M82::OGREATTK, M82::OGREKILL, M82::OGREMOVE, M82::OGREWNCE },
    { ICN::OGRE2   , ICN::MONH0016, {5, 8, 1, 4, 13, 3, 37, 4, {1, 1, 16, 6, 22, 7, 29, 8}, ANIMATTNONE },                     M82::OGREATTK, M82::OGREKILL, M82::OGREMOVE, M82::OGREWNCE },
    { ICN::TROLL   , ICN::MONH0017, {1, 15, 16, 7, 53, 3, 56, 10, {1, 1, 48, 5, 48, 5, 48, 5}, {34, 14, 23, 6, 23, 6, 29, 5} },M82::TRLLATTK, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLWNCE },
    { ICN::TROLL2  , ICN::MONH0018, {1, 15, 16, 7, 53, 3, 56, 10, {1, 1, 48, 5, 48, 5, 48, 5}, {34, 14, 23, 6, 23, 6, 29, 5} },M82::TRLLATTK, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLWNCE },
    { ICN::CYCLOPS , ICN::MONH0019, {1, 7, 30, 9, 23, 2, 25, 5, {1, 1, 13, 5, 8, 5, 18, 5}, ANIMATTNONE },                     M82::CYCLATTK, M82::CYCLKILL, M82::CYCLMOVE, M82::CYCLWNCE },

    { ICN::SPRITE  , ICN::MONH0020, {1, 8, 16, 9, 9, 2, 11, 5, {25, 1, 31, 5, 26, 5, 36, 5}, ANIMATTNONE },                     M82::SPRTATTK, M82::SPRTKILL, M82::SPRTMOVE, M82::SPRTWNCE },
    { ICN::DWARF   , ICN::MONH0021, {1, 9, 45, 4, 38, 7, 49, 7, {1, 1, 19, 8, 10, 9, 27, 11 }, ANIMATTNONE },                   M82::DWRFATTK, M82::DWRFKILL, M82::DWRFMOVE, M82::DWRFWNCE },
    { ICN::DWARF2  , ICN::MONH0022, {1, 9, 45, 4, 38, 7, 49, 7, {1, 1, 19, 8, 10, 9, 27, 11 }, ANIMATTNONE },                   M82::DWRFATTK, M82::DWRFKILL, M82::DWRFMOVE, M82::DWRFWNCE },
    { ICN::ELF     , ICN::MONH0023, {1, 12, 42, 6, 36, 2, 36, 6, {23, 3, 26, 3, 29, 4, 33, 3}, {1, 1, 13, 10, 13, 10, 13, 10} },M82::ELF_ATTK, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_WNCE },
    { ICN::ELF2    , ICN::MONH0024, {1, 12, 42, 6, 36, 2, 36, 6, {23, 3, 26, 3, 29, 4, 33, 3}, {1, 1, 13, 10, 13, 10, 13, 10} },M82::ELF_ATTK, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_WNCE },
    { ICN::DRUID   , ICN::MONH0025, {1, 13, 46, 4, 44, 2, 26, 9, {14, 8, 35, 4, 22, 4, 39, 5}, {14, 8, 35, 4, 22, 4, 39, 5} },  M82::DRUIATTK, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUIWNCE },
    { ICN::DRUID2  , ICN::MONH0026, {1, 13, 46, 4, 44, 2, 26, 9, {14, 8, 35, 4, 22, 4, 39, 5}, {14, 8, 35, 4, 22, 4, 39, 5} },  M82::DRUIATTK, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUIWNCE },
    { ICN::UNICORN , ICN::MONH0027, {10, 8, 1, 9, 41, 3, 44, 8, {18, 2, 20, 7, 27, 7, 34, 7}, ANIMATTNONE },                    M82::UNICATTK, M82::UNICKILL, M82::UNICMOVE, M82::UNICWNCE },
    { ICN::PHOENIX , ICN::MONH0028, {1, 9, 30, 5, 35, 1, 35, 13, {10, 1, 14, 4, 11, 3, 18, 5 }, ANIMATTNONE },                  M82::PHOEATTK, M82::PHOEKILL, M82::PHOEMOVE, M82::PHOEWNCE },

    { ICN::CENTAUR , ICN::MONH0029, {1, 7, 64, 7, 58, 4, 46, 9, {1, 1, 38, 4, 34, 4, 42, 4 }, {18, 4, 27, 3, 22, 5, 30, 4} },    M82::CNTRATTK, M82::CNTRKILL, M82::CNTRMOVE, M82::CNTRWNCE },
    { ICN::GARGOYLE, ICN::MONH0030, {5, 7, 1, 4, 12, 3, 27, 5, {1, 1, 15, 4, 19, 4, 23, 4}, ANIMATTNONE },                       M82::GARGATTK, M82::GARGKILL, M82::GARGMOVE, M82::GARGWNCE },
    { ICN::GRIFFIN , ICN::MONH0031, {1, 6, 16, 9, 25, 3, 25, 9, {7, 1, 8, 3, 15, 1, 11, 4}, ANIMATTNONE },                       M82::GRIFATTK, M82::GRIFKILL, M82::GRIFMOVE, M82::GRIFWNCE },
    { ICN::MINOTAUR, ICN::MONH0032, {6, 7, 1, 5, 27, 3, 30, 5, {13, 3, 23, 4, 16, 4, 20, 3}, ANIMATTNONE },                      M82::MINOATTK, M82::MINOKILL, M82::MINOMOVE, M82::MINOWNCE },
    { ICN::MINOTAU2, ICN::MONH0033, {6, 7, 1, 5, 27, 3, 30, 5, {13, 3, 23, 4, 16, 4, 20, 3}, ANIMATTNONE },                      M82::MINOATTK, M82::MINOKILL, M82::MINOMOVE, M82::MINOWNCE },
    { ICN::HYDRA   , ICN::MONH0034, {1, 8, 28, 15, 16, 6, 22, 6, {1, 1, 9, 7, 9, 7, 9, 7 }, ANIMATTNONE },                       M82::HYDRATTK, M82::HYDRKILL, M82::HYDRMOVE, M82::HYDRWNCE },
    { ICN::DRAGGREE, ICN::MONH0035, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                    M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },
    { ICN::DRAGRED , ICN::MONH0036, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                    M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },
    { ICN::DRAGBLAK, ICN::MONH0037, {1, 12, 41, 13, 32, 4, 36, 5, {1, 1, 13, 7, 20, 6, 26, 8}, ANIMATTNONE },                    M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },

    { ICN::HALFLING, ICN::MONH0038, {5, 8, 1, 4, 13, 4, 37, 4, {1, 1, 29, 3, 32, 3, 35, 2}, {17, 6, 23, 3, 26, 1, 27, 2} },      M82::HALFATTK, M82::HALFKILL, M82::HALFMOVE, M82::HALFWNCE },
    { ICN::BOAR    , ICN::MONH0039, {5, 8, 1, 4, 13, 3, 24, 5, {16, 1, 17, 2, 19, 3, 22, 2}, ANIMATTNONE },                      M82::BOARATTK, M82::BOARKILL, M82::BOARMOVE, M82::BOARWNCE },
    { ICN::GOLEM   , ICN::MONH0040, {1, 12, 34, 6, 32, 2, 26, 6, {13, 3, 16, 3, 19, 4, 23, 3}, ANIMATTNONE },                    M82::GOLMATTK, M82::GOLMKILL, M82::GOLMMOVE, M82::GOLMWNCE },
    { ICN::GOLEM2  , ICN::MONH0041, {1, 12, 34, 6, 32, 2, 26, 6, {13, 3, 16, 3, 19, 4, 23, 3}, ANIMATTNONE },                    M82::GOLMATTK, M82::GOLMKILL, M82::GOLMMOVE, M82::GOLMWNCE },
    { ICN::ROC     , ICN::MONH0042, {1, 8, 18, 4, 22, 2, 24, 10, {1, 1, 15, 3, 9, 2, 11, 4, }, ANIMATTNONE },                    M82::ROC_ATTK, M82::ROC_KILL, M82::ROC_MOVE, M82::ROC_WNCE },
    { ICN::MAGE1   , ICN::MONH0043, {43, 8, 1, 7, 8, 4, 12, 6, {27, 2, 34, 3, 29, 5, 37, 6}, {18, 1, 21, 2, 19, 2, 23, 4} },     M82::MAGEATTK, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGEWNCE },
    { ICN::MAGE2   , ICN::MONH0044, {43, 8, 1, 7, 8, 4, 12, 6, {27, 2, 34, 3, 29, 5, 37, 6}, {18, 1, 21, 2, 19, 2, 23, 4} },     M82::MAGEATTK, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGEWNCE },
    { ICN::TITANBLU, ICN::MONH0045, {7, 7, 1, 6, 29, 3, 32, 6, {1, 1, 19, 5, 14, 5, 24, 5}, ANIMATTNONE },                       M82::TITNATTK, M82::TITNKILL, M82::TITNMOVE, M82::TITNWNCE },
    { ICN::TITANBLA, ICN::MONH0046, {7, 7, 1, 6, 29, 3, 32, 6, {1, 1, 19, 5, 14, 5, 24, 5}, {1, 1, 45, 5, 38, 6, 50, 6} },       M82::TITNATTK, M82::TITNKILL, M82::TITNMOVE, M82::TITNWNCE },

    { ICN::SKELETON, ICN::MONH0047, {4, 8, 35, 4, 25, 3, 28, 7, {1, 1, 20, 5, 12, 4, 16, 4}, ANIMATTNONE },                      M82::SKELATTK, M82::SKELKILL, M82::SKELMOVE, M82::SKELWNCE },
    { ICN::ZOMBIE  , ICN::MONH0048, {1, 13, 14, 12, 40, 7, 47, 7, {1, 1, 26, 6, 32, 5, 37, 3}, ANIMATTNONE },                    M82::ZOMBATTK, M82::ZOMBKILL, M82::ZOMBMOVE, M82::ZOMBWNCE },
    { ICN::ZOMBIE2 , ICN::MONH0049, {1, 13, 14, 12, 40, 7, 47, 7, {1, 1, 26, 6, 32, 5, 37, 3}, ANIMATTNONE },                    M82::ZOMBATTK, M82::ZOMBKILL, M82::ZOMBMOVE, M82::ZOMBWNCE },
    { ICN::MUMMYW  , ICN::MONH0050, {5, 12, 1, 4, 17, 4, 30, 10, {1, 1, 21, 3, 24, 3, 27, 3}, ANIMATTNONE },                     M82::MUMYATTK, M82::MUMYKILL, M82::MUMYMOVE, M82::MUMYWNCE },
    { ICN::MUMMY2  , ICN::MONH0051, {5, 12, 1, 4, 17, 4, 30, 10, {1, 1, 21, 3, 24, 3, 27, 3}, ANIMATTNONE },                     M82::MUMYATTK, M82::MUMYKILL, M82::MUMYMOVE, M82::MUMYWNCE },
    { ICN::VAMPIRE , ICN::MONH0052, {5, 8, 1, 4, 13, 2, 28, 8, {1, 1, 15, 4, 19, 4, 23, 5}, ANIMATTNONE },                       M82::VAMPATTK, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPWNCE },
    { ICN::VAMPIRE2, ICN::MONH0053, {5, 8, 1, 4, 13, 2, 28, 8, {1, 1, 15, 4, 19, 4, 23, 5}, ANIMATTNONE },                       M82::VAMPATTK, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPWNCE },
    { ICN::LICH    , ICN::MONH0054, {5, 8, 1, 4, 28, 3, 31, 7, {1, 1, 13, 5, 18, 5, 23, 5}, {1, 1, 13, 5, 18, 5, 23, 5} },       M82::LICHATTK, M82::LICHKILL, M82::LICHMOVE, M82::LICHWNCE },
    { ICN::LICH2   , ICN::MONH0055, {5, 8, 1, 4, 28, 3, 31, 7, {1, 1, 13, 5, 18, 5, 23, 5}, {1, 1, 13, 5, 18, 5, 23, 5} },       M82::LICHATTK, M82::LICHKILL, M82::LICHMOVE, M82::LICHWNCE },
    { ICN::DRAGBONE, ICN::MONH0056, {1, 12, 22, 7, 20, 2, 29, 12, {1, 1, 48, 4, 13, 3, 16, 4, }, ANIMATTNONE },                  M82::BONEATTK, M82::BONEKILL, M82::BONEMOVE, M82::BONEWNCE },

    { ICN::ROGUE   , ICN::MONH0057, {5, 8, 1, 4, 13, 3, 43, 7, {1, 1, 16, 7, 23, 9, 32, 11}, ANIMATTNONE },                      M82::ROGUATTK, M82::ROGUKILL, M82::ROGUMOVE, M82::ROGUWNCE },
    { ICN::NOMAD   , ICN::MONH0058, {10, 8, 1, 9, 35, 2, 35, 7, {18, 2, 26, 5, 20, 6, 31, 4}, ANIMATTNONE },                     M82::NMADATTK, M82::NMADKILL, M82::NMADMOVE, M82::NMADWNCE },
    { ICN::GHOST   , ICN::MONH0059, {1, 9, 1, 9, 20, 1, 20, 6, {10, 1, 11, 3, 14, 2, 16, 4}, ANIMATTNONE },                      M82::GHSTATTK, M82::GHSTKILL, M82::GHSTMOVE, M82::GHSTWNCE },
    { ICN::GENIE   , ICN::MONH0060, {10, 5, 1, 9, 31, 2, 31, 9, {15, 3, 26, 5, 18, 3, 21, 5}, ANIMATTNONE },                     M82::GENIATTK, M82::GENIKILL, M82::GENIMOVE, M82::GENIWNCE },
    { ICN::MEDUSA  , ICN::MONH0061, {1, 16, 17, 7, 36, 6, 24, 12, {42, 1, 65, 10, 43, 11, 54, 11}, ANIMATTNONE },                M82::MEDSATTK, M82::MEDSKILL, M82::MEDSMOVE, M82::MEDSWNCE },
    { ICN::EELEM   , ICN::MONH0062, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                      M82::EELMATTK, M82::EELMKILL, M82::EELMMOVE, M82::EELMWNCE },
    { ICN::AELEM   , ICN::MONH0063, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                      M82::AELMATTK, M82::AELMKILL, M82::AELMMOVE, M82::AELMWNCE },
    { ICN::FELEM   , ICN::MONH0064, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                      M82::FELMATTK, M82::FELMKILL, M82::FELMMOVE, M82::FELMWNCE },
    { ICN::WELEM   , ICN::MONH0065, {5, 8, 1, 4, 13, 4, 31, 10, {1, 1, 17, 3, 20, 5, 25, 6}, ANIMATTNONE },                      M82::WELMATTK, M82::WELMKILL, M82::WELMMOVE, M82::WELMWNCE },

    { ICN::UNKNOWN , ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
    { ICN::UNKNOWN , ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
    { ICN::UNKNOWN , ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
    { ICN::UNKNOWN , ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
    { ICN::UNKNOWN , ICN::UNKNOWN, ANIM_NONE, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
};

Monster::Monster() : id(UNKNOWN)
{
}

Monster::Monster(monster_t m) : id(m)
{
}

Monster::Monster(const Maps::Tiles & t) : id(FromMaps(t))
{
}

Monster::Monster(u8 race, u32 dwelling) : id(FromDwelling(race, dwelling))
{
}

bool Monster::operator== (monster_t m) const
{
    return id == m;
}

bool Monster::operator!= (monster_t m) const
{
    return id != m;
}

Monster::monster_t Monster::operator() (void) const
{
    return id;
}

Monster::monster_t Monster::GetID(void) const
{
    return id;
}

void Monster::Set(monster_t m)
{
    id = m;
}

void Monster::Upgrade(void)
{
    id = Upgrade(id);
}

u8 Monster::GetAttack(void) const
{
    return monsters[id].attack;
}

u8 Monster::GetDefense(void) const
{
    return monsters[id].defence;
}

u8 Monster::GetPower(void) const
{
    return 0;
}

u8 Monster::GetKnowledge(void) const
{
    return 0;
}

s8 Monster::GetMorale(void) const
{
    return Morale::NORMAL;
}

s8 Monster::GetLuck(void) const
{
    return Luck::NORMAL;
}

Race::race_t Monster::GetRace(void) const
{
    if(UNKNOWN == id)	return Race::BOMG;
    else
    if(GOBLIN > id)	return Race::KNGT;
    else
    if(SPRITE > id)	return Race::BARB;
    else
    if(CENTAUR > id)	return Race::SORC;
    else
    if(HALFLING > id)	return Race::WRLK;
    else
    if(SKELETON > id)	return Race::WZRD;
    else
    if(ROGUE > id)	return Race::NECR;

    return Race::BOMG;
}

u16 Monster::GetSpellPoints(void) const
{
    return 0;
}

u8 Monster::GetLevelSkill(const Skill::Secondary::skill_t skill) const
{
    return 0;
}

Spell::Book* Monster::GetSpellBook(void)
{
    return NULL;
}

u8 Monster::GetType(void) const
{
    return Skill::Primary::MONSTER;
}

Color::color_t Monster::GetColor(void) const
{
    return Color::GRAY;
}

u8  Monster::GetDamageMin(void) const
{
    return monsters[id].damageMin;
}

u8  Monster::GetDamageMax(void) const
{
    return monsters[id].damageMax;
}

u8  Monster::GetShots(void) const
{
    switch(id)
    {
	case ARCHER:		return 12;
	case RANGER:		return 24;
	case ORC:		return 8;
	case CHIEF_ORC:		return 16;
	case TROLL:		return 8;
	case WAR_TROLL:		return 16;
	case ELF:		return 24;
	case GRAND_ELF:		return 24;
	case DRUID:		return 8;
	case GREATER_DRUID:	return 16;
	case CENTAUR:		return 8;
	case HALFLING:		return 12;
	case MAGE:		return 12;
	case ARCHMAGE:		return 24;
	case TITAN:		return 24;
	case LICH:		return 12;
	case POWER_LICH:	return 24;

	default: break;
    }

    return 0;
}

u16 Monster::GetHitPoints(void) const
{
    return monsters[id].hp;
}

u8  Monster::GetSpeed(void) const
{
    return monsters[id].speed;
}

u8  Monster::GetGrown(void) const
{
    return monsters[id].grown;
}

u8  Monster::GetLevel(void) const
{
    return GetLevel(id);
}

u16 Monster::GetRNDSize(void) const
{
    return GetRNDSize(id);
}

u8 Monster::GetSpriteIndex(void) const
{
    return UNKNOWN < id ? id - 1 : 0;
}

const std::string & Monster::GetName(void) const
{
    return monsters[id].name;
}

const std::string & Monster::GetMultiName(void) const
{
    return monsters[id].multiname;
}

bool Monster::isUndead(void) const
{
    switch(id)
    {
        case SKELETON:
        case ZOMBIE:
        case MUTANT_ZOMBIE:
        case MUMMY:
        case ROYAL_MUMMY:
        case VAMPIRE:
        case LORD_VAMPIRE:
        case LICH:
        case POWER_LICH:
        case BONE_DRAGON:
	case GHOST: return true;

	default: break;
    }

    return false;
}

bool Monster::isElemental(void) const
{
    switch(id)
    {
        case EARTH_ELEMENT:
        case AIR_ELEMENT:
        case FIRE_ELEMENT:
        case WATER_ELEMENT: return true;

	default: break;
    }

    return false;
}

bool Monster::isFly(void) const
{
    switch(id)
    {
	case SPRITE:
	case PHOENIX:
	case GARGOYLE:
	case GRIFFIN:
	case GREEN_DRAGON:
	case RED_DRAGON:
	case BLACK_DRAGON:
	case ROC:
	case VAMPIRE:
	case LORD_VAMPIRE:
	case BONE_DRAGON:
	case GHOST:
	case GENIE:	return true;

	default: break;
    }

    return false;
}

bool Monster::isWide(void) const
{
    switch(id)
    {
	case CAVALRY:
	case CHAMPION:
	case WOLF:
	case UNICORN:
	case PHOENIX:
	case CENTAUR:
	case GRIFFIN:
	case HYDRA:
	case GREEN_DRAGON:
	case RED_DRAGON:
	case BLACK_DRAGON:
	case BOAR:
	case ROC:
	case BONE_DRAGON:
	case NOMAD:
	case MEDUSA:	return true;

	default: break;
    }

    return false;
}

bool Monster::isArchers(void) const
{
    return GetShots();
}

bool Monster::isAllowUpgrade(void) const
{
    return id != Upgrade(id);
}

bool Monster::isTwiceAttack(void) const
{
    switch(id)
    {
        case WOLF:
        case PALADIN:
        case CRUSADER:
        case ELF:
        case GRAND_ELF:
        case RANGER: return true;

        default: break;
    }

    return false;
}

Monster::monster_t Monster::Upgrade(monster_t m)
{
    switch(m)
    {
        case ARCHER:		return RANGER;
        case PIKEMAN:		return VETERAN_PIKEMAN;
        case SWORDSMAN:		return MASTER_SWORDSMAN;
        case CAVALRY:		return CHAMPION;
        case PALADIN:		return CRUSADER;
        case ORC:		return CHIEF_ORC;
        case OGRE:		return LORD_OGRE;
        case TROLL:		return WAR_TROLL;
        case DWARF:		return BATTLE_DWARF;
        case ELF:		return GRAND_ELF;
        case DRUID:		return GREATER_DRUID;
        case ZOMBIE:		return MUTANT_ZOMBIE;
        case MUMMY:		return ROYAL_MUMMY;
        case VAMPIRE:		return LORD_VAMPIRE;
        case LICH:		return POWER_LICH;
        case MINOTAUR:		return KNIGHT_MINOTAUR;
        case GREEN_DRAGON:	return RED_DRAGON;
        case RED_DRAGON:	return BLACK_DRAGON;
        case IRON_GOLEM:	return STEEL_GOLEM;
        case MAGE:		return ARCHMAGE;
        case GIANT:		return TITAN;

	default: break;
    }

    return m;
}

Monster::monster_t Monster::FromInt(u8 num)
{
    return MONSTER_RND < num ? UNKNOWN : static_cast<monster_t>(num);
}

Monster::monster_t Monster::FromDwelling(u8 race, u32 dwelling)
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

    return UNKNOWN;
}

Monster::monster_t Monster::FromObject(u8 obj)
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
        case MP2::OBJ_DESERTTENT:	return NOMAD;

        case MP2::OBJ_TROLLBRIDGE:	return TROLL;
        case MP2::OBJ_DRAGONCITY:	return RED_DRAGON;
        case MP2::OBJ_CITYDEAD:		return POWER_LICH;

        case MP2::OBJ_ANCIENTLAMP:	return GENIE;

        // loyalty version
	case MP2::OBJ_WATERALTAR:	return EARTH_ELEMENT;
        case MP2::OBJ_AIRALTAR:		return AIR_ELEMENT;
        case MP2::OBJ_FIREALTAR:	return FIRE_ELEMENT;
        case MP2::OBJ_EARTHALTAR:	return WATER_ELEMENT;
	case MP2::OBJ_BARROWMOUNDS:	return GHOST;

        default: break;
    }

    return UNKNOWN;
}

Monster::monster_t Monster::FromMaps(const Maps::Tiles & tile)
{
    Maps::TilesAddon * addons = const_cast<Maps::Tiles &>(tile).FindMonster();

    return (addons ? FromInt(addons->index + 1) : UNKNOWN);
}

Monster::monster_t Monster::Rand(level_t level)
{
    switch(level)
    {
	case LEVEL0: return FromInt(Rand::Get(PEASANT, WATER_ELEMENT));

	case LEVEL1:
	    switch(Rand::Get(1, 10))
	    {
		case 1:  return PEASANT;
		case 2:  return ARCHER;
		case 3:  return GOBLIN;
		case 4:  return ORC;
		case 5:  return SPRITE;
		case 6:  return CENTAUR;
		case 7:  return HALFLING;
		case 8:  return SKELETON;
		case 9:  return ZOMBIE;
		default: break;
	    }
	    break;
    
	case LEVEL2:
	    switch(Rand::Get(1, 15))
	    {
		case 1:  return RANGER;
		case 2:  return PIKEMAN;
		case 3:  return VETERAN_PIKEMAN;
		case 4:  return CHIEF_ORC;
		case 5:  return WOLF;
		case 6:  return DWARF;
		case 7:  return BATTLE_DWARF;
		case 8:  return ELF;
		case 9:  return GRAND_ELF;
		case 10: return GARGOYLE;
		case 11: return BOAR;
		case 12: return IRON_GOLEM;
		case 13: return MUTANT_ZOMBIE;
		case 14: return MUMMY;
		default: break;
	    }
	    break;

	case LEVEL3:
	    switch(Rand::Get(1, 27))
	    {
	        case 1:  return SWORDSMAN;
		case 2:  return MASTER_SWORDSMAN;
	        case 3:  return CAVALRY;
		case 4:  return CHAMPION;
		case 5:  return OGRE;
		case 6:  return LORD_OGRE;
		case 7:  return TROLL;
		case 8:  return WAR_TROLL;
		case 9:  return DRUID;
		case 10: return GREATER_DRUID;
		case 11: return GRIFFIN;
		case 12: return MINOTAUR;
		case 13: return KNIGHT_MINOTAUR;
		case 14: return STEEL_GOLEM;
		case 15: return ROC;
		case 16: return MAGE;
		case 17: return ARCHMAGE;
		case 18: return ROYAL_MUMMY;
		case 19: return VAMPIRE;
		case 20: return LORD_VAMPIRE;
		case 21: return LICH;
		case 22: return GHOST;
		case 23: return MEDUSA;
		case 24: return EARTH_ELEMENT;
		case 25: return AIR_ELEMENT;
		case 26: return FIRE_ELEMENT;
		default: break;
	    }
	    break;

	case LEVEL4:
	    switch(Rand::Get(1, 14))
	    {
		case 1:  return PALADIN;
		case 2:  return CRUSADER;
		case 3:  return CYCLOPS;
		case 4:  return UNICORN;
		case 5:  return PHOENIX;
		case 6:  return HYDRA;
		case 7:  return GREEN_DRAGON;
		case 8:  return RED_DRAGON;
		case 9:  return BLACK_DRAGON;
		case 10: return GIANT;
		case 11: return TITAN;
		case 12: return POWER_LICH;
		case 13: return BONE_DRAGON;
		default: break;
	    }
	    break;
    }

    return UNKNOWN;
}


u16 Monster::GetRNDSize(monster_t m)
{
    const u8 grown = Monster(m).GetGrown();

    return Rand::Get(grown - grown / 2, grown + grown / 2);
}

u8 Monster::GetLevel(monster_t m)
{
    switch(m)
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
	case MONSTER_RND1:	return LEVEL1;

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
	case MONSTER_RND2:	return LEVEL2;

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
	case MONSTER_RND3:	return LEVEL3;

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
	case MONSTER_RND4:	return LEVEL4;
    
	case MONSTER_RND:
    	    switch(Rand::Get(0, 3))
    	    {
		default:	return LEVEL1;
		case 1:		return LEVEL2;
		case 2:		return LEVEL3;
		case 3:		return LEVEL4;
    	    }
	    break;

	default: break;
    }

    return LEVEL0;
}

const std::string & Monster::String(monster_t m)
{
    return monsters[m].name;
}

u32 Monster::GetDwelling(monster_t m)
{
    switch(m)
    {
	case PEASANT:
	case GOBLIN:
	case SPRITE:
	case CENTAUR:
	case HALFLING:
	case SKELETON:		return Castle::DWELLING_MONSTER1;

	case ARCHER:
	case ORC:
	case ZOMBIE:
	case DWARF:
	case GARGOYLE:
	case BOAR:		return Castle::DWELLING_MONSTER2;

	case RANGER:
	case CHIEF_ORC:
	case BATTLE_DWARF:
	case MUTANT_ZOMBIE:	return Castle::DWELLING_UPGRADE2;

	case PIKEMAN:
	case WOLF:
	case ELF:
	case IRON_GOLEM:
	case MUMMY:
	case GRIFFIN:		return Castle::DWELLING_MONSTER3;

	case VETERAN_PIKEMAN:
	case GRAND_ELF:
	case STEEL_GOLEM:
	case ROYAL_MUMMY:	return Castle::DWELLING_UPGRADE3;

	case SWORDSMAN:
	case OGRE:
	case DRUID:
	case MINOTAUR:
	case ROC:
	case VAMPIRE:		return Castle::DWELLING_MONSTER4;

	case MASTER_SWORDSMAN:
	case LORD_OGRE:
	case GREATER_DRUID:
	case KNIGHT_MINOTAUR:
	case LORD_VAMPIRE:	return Castle::DWELLING_UPGRADE4;

	case CAVALRY:
	case TROLL:
	case MAGE:
	case LICH:
	case UNICORN:
	case HYDRA:		return Castle::DWELLING_MONSTER5;

	case CHAMPION:
	case WAR_TROLL:
	case ARCHMAGE:
	case POWER_LICH:	return Castle::DWELLING_UPGRADE5;

	case PALADIN:
	case CYCLOPS:
	case PHOENIX:
	case GREEN_DRAGON:
	case GIANT:
	case BONE_DRAGON:	return Castle::DWELLING_MONSTER6;

	case CRUSADER:
	case RED_DRAGON:
	case TITAN:		return Castle::DWELLING_UPGRADE6;

	case BLACK_DRAGON:	return Castle::DWELLING_UPGRADE7;
    
	default: break;
    }

    return 0;
}

const std::string & Monster::String(Monster & m)
{
    return String(m.id);
}

Monster::monster_t Monster::Upgrade(Monster & m)
{
    return Upgrade(m.id);
}

u16 Monster::GetRNDSize(Monster & m)
{
    return GetRNDSize(m.id);
}

u8 Monster::GetLevel(Monster & m)
{
    return GetLevel(m.id);
}

u32 Monster::GetDwelling(Monster & m)
{
    return GetDwelling(m.id);
}


ICN::icn_t Monster::ICNFile(void) const
{
    return animonsters[id].icn_file;
}

ICN::icn_t Monster::ICNMonh(void) const
{
    return animonsters[id].icn_monh;
}

ICN::icn_t Monster::ICNMiss(void) const
{
    switch(id)
    {
        case ARCHER:            return ICN::ARCH_MSL;
        case RANGER:            return ICN::ARCH_MSL;
        case ORC:               return ICN::ORC__MSL;
        case CHIEF_ORC:         return ICN::ORC__MSL;
        case TROLL:             return ICN::TROLLMSL;
        case WAR_TROLL:         return ICN::TROLLMSL;
        case ELF:               return ICN::ELF__MSL;
        case GRAND_ELF:         return ICN::ELF__MSL;
        case DRUID:             return ICN::DRUIDMSL;
        case GREATER_DRUID:     return ICN::DRUIDMSL;
        case CENTAUR:           return ICN::ARCH_MSL;
        case HALFLING:          return ICN::HALFLMSL;
        case MAGE:              return ICN::DRUIDMSL;
        case ARCHMAGE:          return ICN::DRUIDMSL;
        case TITAN:             return ICN::TITANMSL;
        case LICH:              return ICN::LICH_MSL;
        case POWER_LICH:        return ICN::LICH_MSL;

        default: break;
    }

    return ICN::UNKNOWN;
}


M82::m82_t Monster::M82Attk(void) const
{
    return animonsters[id].m82_attk;
}

M82::m82_t Monster::M82Expl(void) const
{
    switch(id)
    {
	case VAMPIRE:		return M82::VAMPEXT1;
        case LORD_VAMPIRE:	return M82::VAMPEXT1;
        case LICH:		return M82::LICHEXPL;
        case POWER_LICH:	return M82::LICHEXPL;

        default: break;
    }

    return M82::UNKNOWN;
}

M82::m82_t Monster::M82Kill(void) const
{
    return animonsters[id].m82_kill;
}

M82::m82_t Monster::M82Move(void) const
{
    return animonsters[id].m82_move;
}

M82::m82_t Monster::M82Shot(void) const
{
    switch(id)
    {
        case ARCHER:            return M82::ARCHSHOT;
        case RANGER:            return M82::ARCHSHOT;
        case ORC:               return M82::ORC_SHOT;
        case CHIEF_ORC:         return M82::ORC_SHOT;
        case TROLL:             return M82::TRLLSHOT;
        case WAR_TROLL:         return M82::TRLLSHOT;
        case ELF:               return M82::ELF_SHOT;
        case GRAND_ELF:         return M82::ELF_SHOT;
        case DRUID:             return M82::DRUISHOT;
        case GREATER_DRUID:     return M82::DRUISHOT;
        case CENTAUR:           return M82::CNTRSHOT;
        case HALFLING:          return M82::HALFSHOT;
        case MAGE:              return M82::MAGESHOT;
        case ARCHMAGE:          return M82::MAGESHOT;
        case TITAN:             return M82::TITNSHOT;
        case LICH:              return M82::LICHSHOT;
        case POWER_LICH:        return M82::LICHSHOT;

        default: break;
    }

    return M82::UNKNOWN;
}

M82::m82_t Monster::M82Wnce(void) const
{
    return animonsters[id].m82_wnce;
}

const Monster::anim_t & Monster::Animation(void) const
{
    return animonsters[id].animation;
}

void Monster::GetAnimFrames(u8 anim, u8 & start, u8 & length, bool attranged) const
{
    switch(anim)
    {
	case AS_WALK:
    	    start = animonsters[id].animation.walk_start;
    	    length = animonsters[id].animation.walk_count;
    	    break;

	case AS_IDLE:
    	    start = animonsters[id].animation.idle_start;
    	    length = animonsters[id].animation.idle_count;
    	    break;

	case AS_PAIN:
    	    start = animonsters[id].animation.pain_start;
    	    length = animonsters[id].animation.pain_count;
    	    break;

	case AS_DIE:
    	    start = animonsters[id].animation.die_start;
    	    length = animonsters[id].animation.die_count;
    	    break;

	case AS_ATTPREP:
    	    start = attranged ? animonsters[id].animation.ar.attprep_start : animonsters[id].animation.a.attprep_start;
    	    length = attranged ? animonsters[id].animation.ar.attprep_count : animonsters[id].animation.a.attprep_count;
    	    break;

	case AS_ATT1:
    	    start = attranged ? animonsters[id].animation.ar.attack1_start : animonsters[id].animation.a.attack1_start;
    	    length = attranged ? animonsters[id].animation.ar.attack1_count : animonsters[id].animation.a.attack1_count;
    	    break;

	case AS_ATT2:
    	    start = attranged ? animonsters[id].animation.ar.attack2_start : animonsters[id].animation.a.attack2_start;
    	    length = attranged ? animonsters[id].animation.ar.attack2_count : animonsters[id].animation.a.attack2_count;
    	    break;

	case AS_ATT3:
    	    start = attranged ? animonsters[id].animation.ar.attack3_start : animonsters[id].animation.a.attack3_start;
    	    length = attranged ? animonsters[id].animation.ar.attack3_count : animonsters[id].animation.a.attack3_count;
    	    break;

	default:
    	    start = 1;
    	    length = 1;
	    break;
    }
}
