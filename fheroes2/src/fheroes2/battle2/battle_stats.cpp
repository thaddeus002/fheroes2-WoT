/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include <cstring>
#include <algorithm>
#include <functional>
#include "engine.h"
#include "agg.h"
#include "luck.h"
#include "morale.h"
#include "speed.h"
#include "heroes.h"
#include "settings.h"
#include "battle_arena.h"
#include "battle_cell.h"
#include "battle_interface.h"
#include "battle_stats.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

void GetLinePoints(const Point & pt1, const Point & pt2, u16 step, std::vector<Point> & res);

namespace Battle2
{
    static MonsterInfo monsters_info[] = {

	{ ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
	{ ICN::PEASANT , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 6}, {22, 6}, {28, 6}, {13, 3}, {34, 4}, M82::PSNTATTK, M82::PSNTKILL, M82::PSNTMOVE, M82::PSNTWNCE },
	{ ICN::ARCHER  , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 4}, {24, 4}, {28, 4}, {32, 4}, {36, 3}, {39, 3}, {42, 3}, {13, 3}, {45, 6}, M82::ARCHATTK, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHWNCE },
	{ ICN::ARCHER2 , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 4}, {24, 4}, {28, 4}, {32, 4}, {36, 3}, {39, 3}, {42, 3}, {13, 3}, {45, 6}, M82::ARCHATTK, M82::ARCHKILL, M82::ARCHMOVE, M82::ARCHWNCE },
	{ ICN::PIKEMAN , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {17, 3}, {20, 4}, {24, 6}, {13, 4}, {30, 6}, M82::PIKEATTK, M82::PIKEKILL, M82::PIKEMOVE, M82::PIKEWNCE },
	{ ICN::PIKEMAN2, { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {17, 3}, {20, 4}, {24, 6}, {13, 4}, {30, 6}, M82::PIKEATTK, M82::PIKEKILL, M82::PIKEMOVE, M82::PIKEWNCE },
	{ ICN::SWORDSMN, {39, 6}, { 2, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {12, 2}, {19, 5}, {14, 5}, {24, 5}, {36, 3}, {29, 7}, M82::SWDMATTK, M82::SWDMKILL, M82::SWDMMOVE, M82::SWDMWNCE },
	{ ICN::SWORDSM2, {39, 6}, { 2, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {12, 2}, {19, 5}, {14, 5}, {24, 5}, {36, 3}, {29, 7}, M82::SWDMATTK, M82::SWDMKILL, M82::SWDMMOVE, M82::SWDMWNCE },
	{ ICN::CAVALRYR, {19, 4}, { 1, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 8, 1}, {12, 3}, { 9, 3}, {15, 2}, {28, 2}, {23, 5}, M82::CAVLATTK, M82::CAVLKILL, M82::CAVLMOVE, M82::CAVLWNCE },
	{ ICN::CAVALRYB, {19, 4}, { 1, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 8, 1}, {12, 3}, { 9, 3}, {15, 2}, {28, 2}, {23, 5}, M82::CAVLATTK, M82::CAVLKILL, M82::CAVLMOVE, M82::CAVLWNCE },
	{ ICN::PALADIN , { 1,11}, {12, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {20, 2}, {22, 3}, {25, 3}, {28, 3}, {31, 2}, {34, 5}, M82::PLDNATTK, M82::PLDNKILL, M82::PLDNMOVE, M82::PLDNWNCE },
	{ ICN::PALADIN2, { 1,11}, {12, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {20, 2}, {22, 3}, {25, 3}, {28, 3}, {31, 2}, {34, 5}, M82::PLDNATTK, M82::PLDNKILL, M82::PLDNMOVE, M82::PLDNWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
	{ ICN::GOBLIN  , {33, 7}, { 1, 9}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {10, 3}, {17, 4}, {13, 4}, {21, 4}, {25, 4}, {29, 4}, M82::GBLNATTK, M82::GBLNKILL, M82::GBLNMOVE, M82::GBLNWNCE },
	{ ICN::ORC     , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {16,13}, { 0, 0}, { 0, 0}, { 0, 0}, {28, 2}, {30, 3}, {33, 4}, {37, 3}, {13, 3}, {40, 4}, M82::ORC_ATTK, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_WNCE },
        { ICN::ORC2    , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {16,13}, { 0, 0}, { 0, 0}, { 0, 0}, {28, 2}, {30, 3}, {33, 4}, {37, 3}, {13, 3}, {40, 4}, M82::ORC_ATTK, M82::ORC_KILL, M82::ORC_MOVE, M82::ORC_WNCE },
	{ ICN::WOLF    , {20, 6}, { 7, 6}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {13, 3}, { 2, 5}, {16, 4}, {26, 3}, {28, 5}, M82::WOLFATTK, M82::WOLFKILL, M82::WOLFMOVE, M82::WOLFWNCE },
        { ICN::OGRE    , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 6}, {22, 7}, {29, 8}, {13, 3}, {37, 4}, M82::OGREATTK, M82::OGREKILL, M82::OGREMOVE, M82::OGREWNCE },
        { ICN::OGRE2   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 6}, {22, 7}, {29, 8}, {13, 3}, {37, 4}, M82::OGREATTK, M82::OGREKILL, M82::OGREMOVE, M82::OGREWNCE },
        { ICN::TROLL   , {16, 7}, { 1,15}, { 0, 0}, { 0, 0}, { 0, 0}, {23, 5}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {48, 6}, {23, 6}, {29, 5}, {54, 3}, {57, 9}, M82::TRLLATTK, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLWNCE },
        { ICN::TROLL2  , {16, 7}, { 1,15}, { 0, 0}, { 0, 0}, { 0, 0}, {23, 5}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {48, 6}, {23, 6}, {29, 5}, {54, 3}, {57, 9}, M82::TRLLATTK, M82::TRLLKILL, M82::TRLLMOVE, M82::TRLLWNCE },
        { ICN::CYCLOPS , {30, 9}, { 1, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 2}, {11, 2}, {21, 2}, { 0, 0}, {14, 3}, { 8, 3}, {18, 3}, {23, 2}, {25, 5}, M82::CYCLATTK, M82::CYCLKILL, M82::CYCLMOVE, M82::CYCLWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
        { ICN::SPRITE  , {16, 9}, { 0, 0}, { 1, 3}, { 4, 4}, { 7, 2}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {30, 6}, {25, 5}, {36, 5}, { 9, 2}, {11, 5}, M82::SPRTATTK, M82::SPRTKILL, M82::SPRTMOVE, M82::SPRTWNCE },
        { ICN::DWARF   , {44, 5}, { 1, 9}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {20, 7}, {10,10}, {27, 9}, {36, 8}, {49, 7}, M82::DWRFATTK, M82::DWRFKILL, M82::DWRFMOVE, M82::DWRFWNCE },
        { ICN::DWARF2  , {44, 5}, { 1, 9}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {20, 7}, {10,10}, {27, 9}, {36, 8}, {49, 7}, M82::DWRFATTK, M82::DWRFKILL, M82::DWRFMOVE, M82::DWRFWNCE },
	{ ICN::ELF     , {42, 6}, { 1,12}, { 0, 0}, { 0, 0}, { 0, 0}, {13,10}, { 0, 0}, { 0, 0}, { 0, 0}, {23, 3}, {26, 3}, {29, 3}, {32, 4}, {36, 2}, {38, 4}, M82::ELF_ATTK, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_WNCE },
        { ICN::ELF2    , {42, 6}, { 1,12}, { 0, 0}, { 0, 0}, { 0, 0}, {13,10}, { 0, 0}, { 0, 0}, { 0, 0}, {23, 3}, {26, 3}, {29, 3}, {32, 4}, {36, 2}, {38, 4}, M82::ELF_ATTK, M82::ELF_KILL, M82::ELF_MOVE, M82::ELF_WNCE },
	{ ICN::DRUID   , {46, 4}, { 3,11}, { 0, 0}, { 0, 0}, { 0, 0}, {14, 8}, {35, 4}, {22, 4}, {39, 5}, {14, 8}, {35, 4}, {22, 4}, {39, 4}, {44, 2}, {26, 9}, M82::DRUIATTK, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUIWNCE },
        { ICN::DRUID2  , {46, 4}, { 3,11}, { 0, 0}, { 0, 0}, { 0, 0}, {14, 8}, {35, 4}, {22, 4}, {39, 5}, {14, 8}, {35, 4}, {22, 4}, {39, 4}, {44, 2}, {26, 9}, M82::DRUIATTK, M82::DRUIKILL, M82::DRUIMOVE, M82::DRUIWNCE },
        { ICN::UNICORN , { 1, 9}, {10, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {18, 9}, {27, 7}, {34, 7}, {41, 4}, {45, 7}, M82::UNICATTK, M82::UNICKILL, M82::UNICMOVE, M82::UNICWNCE },
        { ICN::PHOENIX , {30, 5}, { 0, 0}, { 1, 3}, { 4, 6}, { 0, 0}, { 0, 0}, {25, 3}, {23, 2}, {28, 2}, { 0, 0}, {13, 4}, {10, 4}, {17, 4}, {35, 2}, {37,11}, M82::PHOEATTK, M82::PHOEKILL, M82::PHOEMOVE, M82::PHOEWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
        { ICN::CENTAUR , {65, 6}, { 2, 6}, { 0, 0}, { 0, 0}, { 0, 0}, {18, 4}, {27, 3}, {22, 5}, {30, 3}, {33, 1}, {38, 4}, {34, 4}, {42, 4}, {46, 2}, {48, 7}, M82::CNTRATTK, M82::CNTRKILL, M82::CNTRMOVE, M82::CNTRWNCE },
        { ICN::GARGOYLE, { 1, 4}, { 0, 0}, { 5, 2}, { 7, 4}, {11, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {14, 1}, {15, 4}, {19, 4}, {23, 4}, {12, 2}, {27, 5}, M82::GARGATTK, M82::GARGKILL, M82::GARGMOVE, M82::GARGWNCE },
	{ ICN::GRIFFIN , {16, 9}, { 0, 0}, { 1, 3}, { 3, 3}, { 5, 2}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 7, 1}, { 8, 2}, {14, 2}, {11, 3}, { 9, 2}, {25, 9}, M82::GRIFATTK, M82::GRIFKILL, M82::GRIFMOVE, M82::GRIFWNCE },
        { ICN::MINOTAUR, { 1, 5}, { 6, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {13, 3}, {20, 3}, {16, 4}, {23, 4}, {27, 2}, {29, 6}, M82::MINOATTK, M82::MINOKILL, M82::MINOMOVE, M82::MINOWNCE },
        { ICN::MINOTAU2, { 1, 5}, { 6, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {13, 3}, {20, 3}, {16, 4}, {23, 4}, {27, 2}, {29, 6}, M82::MINOATTK, M82::MINOKILL, M82::MINOMOVE, M82::MINOWNCE },
        { ICN::HYDRA   , {28,15}, { 1, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 9, 8}, { 9, 8}, { 9, 8}, {17, 5}, {21, 7}, M82::HYDRATTK, M82::HYDRKILL, M82::HYDRMOVE, M82::HYDRWNCE },
        { ICN::DRAGGREE, {41,13}, { 0, 0}, { 1, 4}, { 5, 3}, { 8, 5}, {13, 1}, {17, 3}, {23, 3}, {30, 2}, {13, 1}, {14, 3}, {20, 3}, {26, 4}, {32, 3}, {35, 6}, M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },
        { ICN::DRAGRED , {41,13}, { 0, 0}, { 1, 4}, { 5, 3}, { 8, 5}, {13, 1}, {17, 3}, {23, 3}, {30, 2}, {13, 1}, {14, 3}, {20, 3}, {26, 4}, {32, 3}, {35, 6}, M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },
	{ ICN::DRAGBLAK, {41,13}, { 0, 0}, { 1, 4}, { 5, 3}, { 8, 5}, {13, 1}, {17, 3}, {23, 3}, {30, 2}, {13, 1}, {14, 3}, {20, 3}, {26, 4}, {32, 3}, {35, 6}, M82::DRGNATTK, M82::DRGNKILL, M82::DRGNMOVE, M82::DRGNWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
        { ICN::HALFLING, { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {17, 5}, {22, 2}, {26, 1}, {27, 2}, {29, 1}, {30, 2}, {32, 3}, {35, 2}, {13, 4}, {37, 4}, M82::HALFATTK, M82::HALFKILL, M82::HALFMOVE, M82::HALFWNCE },
        { ICN::BOAR    , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {15, 1}, {16, 3}, {19, 3}, {22, 2}, {13, 2}, {24, 5}, M82::BOARATTK, M82::BOARKILL, M82::BOARMOVE, M82::BOARWNCE },
        { ICN::GOLEM   , {34, 6}, { 1,12}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {22, 4}, {13, 5}, {18, 4}, {32, 4}, {26, 6}, M82::GOLMATTK, M82::GOLMKILL, M82::GOLMMOVE, M82::GOLMWNCE },
        { ICN::GOLEM2  , {34, 6}, { 1,12}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {22, 4}, {13, 5}, {18, 4}, {32, 4}, {26, 6}, M82::GOLMATTK, M82::GOLMKILL, M82::GOLMMOVE, M82::GOLMWNCE },
        { ICN::ROC     , {18, 7}, { 0, 0}, { 1, 3}, { 4, 3}, { 7, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {14, 3}, { 8, 3}, {11, 3}, {25, 3}, {25, 9}, M82::ROC_ATTK, M82::ROC_KILL, M82::ROC_MOVE, M82::ROC_WNCE },
        { ICN::MAGE1   , { 1, 7}, {43, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {18, 1}, {21, 2}, {19, 2}, {23, 4}, {27, 1}, {34, 3}, {28, 6}, {37, 6}, { 8, 3}, {11, 7}, M82::MAGEATTK, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGEWNCE },
        { ICN::MAGE2   , { 1, 7}, {43, 8}, { 0, 0}, { 0, 0}, { 0, 0}, {18, 1}, {21, 2}, {19, 2}, {23, 4}, {27, 1}, {34, 3}, {28, 6}, {37, 6}, { 8, 3}, {11, 7}, M82::MAGEATTK, M82::MAGEKILL, M82::MAGEMOVE, M82::MAGEWNCE },
        { ICN::TITANBLU, { 1, 6}, { 7, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {19, 5}, {14, 5}, {24, 5}, {29, 2}, {31, 7}, M82::TITNATTK, M82::TITNKILL, M82::TITNMOVE, M82::TITNWNCE },
        { ICN::TITANBLA, { 1, 6}, { 7, 7}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {45, 5}, {38, 5}, {50, 5}, { 0, 0}, {19, 5}, {14, 5}, {24, 5}, {29, 2}, {31, 7}, M82::TITNATTK, M82::TITNKILL, M82::TITNMOVE, M82::TITNWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
        { ICN::SKELETON, {35, 4}, { 3, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {20, 4}, {12, 4}, {16, 4}, {24, 3}, {27, 8}, M82::SKELATTK, M82::SKELKILL, M82::SKELMOVE, M82::SKELWNCE },
        { ICN::ZOMBIE  , {14,12}, { 1,13}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {26, 5}, {31, 5}, {36, 5}, {40, 3}, {47, 7}, M82::ZOMBATTK, M82::ZOMBKILL, M82::ZOMBMOVE, M82::ZOMBWNCE },
        { ICN::ZOMBIE2 , {14,12}, { 1,13}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {26, 5}, {31, 5}, {36, 5}, {40, 3}, {47, 7}, M82::ZOMBATTK, M82::ZOMBKILL, M82::ZOMBMOVE, M82::ZOMBWNCE },
	{ ICN::MUMMYW  , { 1, 4}, { 5,12}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {21, 3}, {24, 3}, {27, 3}, {17, 4}, {30,10}, M82::MUMYATTK, M82::MUMYKILL, M82::MUMYMOVE, M82::MUMYWNCE },
        { ICN::MUMMY2  , { 1, 4}, { 5,12}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {21, 3}, {24, 3}, {27, 3}, {17, 4}, {30,10}, M82::MUMYATTK, M82::MUMYKILL, M82::MUMYMOVE, M82::MUMYWNCE },
        { ICN::VAMPIRE , { 1, 4}, { 0, 0}, { 5, 4}, { 9, 4}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {15, 4}, {19, 4}, {23, 5}, {13, 3}, {28, 8}, M82::VAMPATTK, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPWNCE },
        { ICN::VAMPIRE2, { 1, 4}, { 0, 0}, { 5, 4}, { 9, 4}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {15, 4}, {19, 4}, {23, 5}, {13, 3}, {28, 8}, M82::VAMPATTK, M82::VAMPKILL, M82::VAMPMOVE, M82::VAMPWNCE },
        { ICN::LICH    , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {13, 5}, {18, 5}, {23, 5}, { 0, 0}, {13, 5}, {18, 5}, {23, 5}, {28, 3}, {31, 7}, M82::LICHATTK, M82::LICHKILL, M82::LICHMOVE, M82::LICHWNCE },
        { ICN::LICH2   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {13, 5}, {18, 8}, {23, 5}, { 0, 0}, {13, 5}, {18, 5}, {23, 5}, {28, 3}, {31, 7}, M82::LICHATTK, M82::LICHKILL, M82::LICHMOVE, M82::LICHWNCE },
        { ICN::DRAGBONE, {22, 7}, { 0, 0}, { 1, 4}, { 5, 3}, { 8, 4}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {47, 5}, {12, 4}, {16, 4}, {20, 2}, {41, 6}, M82::BONEATTK, M82::BONEKILL, M82::BONEMOVE, M82::BONEWNCE },

	// icn_file      idle     move     fly1     fly2     fly3     shot0    shot1    shot2    shot3    attk0    attk1    attk2    attk3    wcne     kill     m82_attk       m82_kill       m82_move       m82_wnce
        { ICN::ROGUE   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 8}, {24,10}, {34, 9}, {13, 3}, {43, 7}, M82::ROGUATTK, M82::ROGUKILL, M82::ROGUMOVE, M82::ROGUWNCE },
        { ICN::NOMAD   , { 1, 9}, {10, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {18, 2}, {26, 5}, {20, 6}, {31, 4}, {35, 2}, {37, 5}, M82::NMADATTK, M82::NMADKILL, M82::NMADMOVE, M82::NMADWNCE },
        { ICN::GHOST   , { 1, 3}, { 0, 0}, { 0, 0}, { 4, 5}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {10, 3}, {13, 3}, {16, 4}, {20, 2}, {20, 8}, M82::GHSTATTK, M82::GHSTKILL, M82::GHSTMOVE, M82::GHSTWNCE },
        { ICN::GENIE   , { 1, 9}, { 0, 0}, {10, 1}, {11, 4}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {26, 5}, {16, 5}, {21, 5}, {31, 2}, {33, 7}, M82::GENIATTK, M82::GENIKILL, M82::GENIMOVE, M82::GENIWNCE },
        { ICN::MEDUSA  , {17, 7}, { 1,16}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {65,10}, {41,13}, {54,11}, {36, 2}, {24,12}, M82::MEDSATTK, M82::MEDSKILL, M82::MEDSMOVE, M82::MEDSWNCE },
        { ICN::EELEM   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 5}, {25, 6}, {13, 3}, {31,11}, M82::EELMATTK, M82::EELMKILL, M82::EELMMOVE, M82::EELMWNCE },
        { ICN::AELEM   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 5}, {25, 6}, {13, 3}, {31,11}, M82::AELMATTK, M82::AELMKILL, M82::AELMMOVE, M82::AELMWNCE },
        { ICN::FELEM   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 5}, {25, 6}, {13, 3}, {31,11}, M82::FELMATTK, M82::FELMKILL, M82::FELMMOVE, M82::FELMWNCE },
        { ICN::WELEM   , { 1, 4}, { 5, 8}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, {16, 4}, {20, 5}, {25, 6}, {13, 3}, {31,11}, M82::WELMATTK, M82::WELMKILL, M82::WELMMOVE, M82::WELMWNCE },

        { ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
        { ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
        { ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
        { ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
	{ ICN::UNKNOWN , { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN, M82::UNKNOWN },
    };

    static u8 genie_enemy_half_percent = 10;
}

void Battle2::UpdateMonsterAttributes(const std::string & spec)
{
#ifdef WITH_XML
    // parse battle.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_battle = NULL;

    if(doc.LoadFile(spec.c_str()) &&
        NULL != (xml_battle = doc.FirstChildElement("battle")))
    {
	const TiXmlElement* xml_element;
	int value;

        // genie
        xml_element = xml_battle->FirstChildElement("genie");
        if(xml_element)
        {
	    xml_element->Attribute("enemy_half_percent", &value);
	    if(value > 100) value = 100;
	    genie_enemy_half_percent = value;
	}
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

void Battle2::UpdateMonsterInfoAnimation(const std::string & spec)
{
#ifdef WITH_XML
    // parse battle.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_animation = NULL;

    if(doc.LoadFile(spec.c_str()) &&
        NULL != (xml_animation = doc.FirstChildElement("animations")))
    {
        const TiXmlElement* xml_icn = xml_animation->FirstChildElement("icn");
        for(; xml_icn; xml_icn = xml_icn->NextSiblingElement("icn"))
	{
	    std::string icn_name = String::Upper(xml_icn->Attribute("name"));
	    // find icn name
	    ICN::icn_t icn = ICN::FromString(icn_name.c_str());
	    if(icn == ICN::UNKNOWN) continue;

	    // find monster info position
	    MonsterInfo* ptr = &monsters_info[1];
	    while(ptr->icn_file != ICN::UNKNOWN && icn != ptr->icn_file) ++ptr;
	    if(ptr->icn_file == ICN::UNKNOWN) continue;

    	    const TiXmlElement *xml_anim = xml_icn->FirstChildElement("animation");
	    int start, count;
	    const char* state;

	    for(; xml_anim; xml_anim = xml_anim->NextSiblingElement("animation"))
	    {
		state = xml_anim->Attribute("state");
		xml_anim->Attribute("start", &start);
		xml_anim->Attribute("count", &count);
		animframe_t frm;
		frm.start = start;
		frm.count = count;

		if(0 == std::strcmp("idle", state)) ptr->frm_idle = frm;
		else
		if(0 == std::strcmp("move", state)) ptr->frm_move = frm;
		else
		if(0 == std::strcmp("fly1", state)) ptr->frm_fly1 = frm;
		else
		if(0 == std::strcmp("fly2", state)) ptr->frm_fly2 = frm;
		else
		if(0 == std::strcmp("fly3", state)) ptr->frm_fly3 = frm;
		else
		if(0 == std::strcmp("shot0", state)) ptr->frm_shot0 = frm;
		else
		if(0 == std::strcmp("shot1", state)) ptr->frm_shot1 = frm;
		else
		if(0 == std::strcmp("shot2", state)) ptr->frm_shot2 = frm;
		else
		if(0 == std::strcmp("shot3", state)) ptr->frm_shot3 = frm;
		else
		if(0 == std::strcmp("attk0", state)) ptr->frm_attk0 = frm;
		else
		if(0 == std::strcmp("attk1", state)) ptr->frm_attk1 = frm;
		else
		if(0 == std::strcmp("attk2", state)) ptr->frm_attk2 = frm;
		else
		if(0 == std::strcmp("attk3", state)) ptr->frm_attk3 = frm;
		else
		if(0 == std::strcmp("wnce", state)) ptr->frm_wnce = frm;
		else
		if(0 == std::strcmp("kill", state)) ptr->frm_kill = frm;
	    }
	}
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

Battle2::ModeDuration::ModeDuration() : std::pair<u32, u16>(0, 0)
{
}

Battle2::ModeDuration::ModeDuration(u32 mode, u16 duration) : std::pair<u32, u16>(mode, duration)
{
}

bool Battle2::ModeDuration::isMode(u32 mode) const
{
    return (first & mode);
}

bool Battle2::ModeDuration::isZeroDuration(void) const
{
    return 0 == second;
}

void Battle2::ModeDuration::DecreaseDuration(void)
{
    if(second) --second;
}

Battle2::ModesAffected::ModesAffected()
{
    reserve(3);
}

void Battle2::ModesAffected::AddMode(u32 mode, u16 duration)
{
    iterator it = std::find_if(begin(), end(), std::bind2nd(std::mem_fun_ref(&ModeDuration::isMode), mode));
    if(it == end())
	push_back(ModeDuration(mode, duration));
    else
	(*it).second = duration;
}

void Battle2::ModesAffected::RemoveMode(u32 mode)
{
    iterator it = std::find_if(begin(), end(), std::bind2nd(std::mem_fun_ref(&ModeDuration::isMode), mode));
    if(it != end())
    {
	// erase(it)
	if(it + 1 != end()) std::swap(*it, back());
        pop_back();
    }
}

void Battle2::ModesAffected::DecreaseDuration(void)
{
    std::for_each(begin(), end(), std::mem_fun_ref(&ModeDuration::DecreaseDuration));
}

u32 Battle2::ModesAffected::FindZeroDuration(void) const
{
    const_iterator it = std::find_if(begin(), end(), std::mem_fun_ref(&ModeDuration::isZeroDuration));
    return it == end() ? 0 : (*it).first;
}

Battle2::Stats::Stats(Army::Troop & t)
    : troop(t), arena(NULL), id(0), position(0), hp(0), count(0), dead(0), shots(t.GetShots()), disruptingray(0),
    reflect(false), mirror(NULL), animstate(0), animframe(0), animstep(1)
{
    id = (troop.GetColor() << 8) | troop.GetArmyIndex();
    count = troop.count;
    hp = GetMonster().GetHitPoints() * count;

    contours[0] = NULL;
    contours[1] = NULL;
    contours[2] = NULL;
    contours[3] = NULL;

    ResetAnimFrame(AS_IDLE);
}

Battle2::Stats::~Stats()
{
    if(contours[0]) delete contours[0];
    if(contours[1]) delete contours[1];
    if(contours[2]) delete contours[2];
    if(contours[3]) delete contours[3];
    
    // reset summon elemental and mirror image
    if(Modes(CAP_SUMMONELEM) || Modes(CAP_MIRRORIMAGE))
    {
	troop.count = 0;
    }
}

bool Battle2::Stats::isID(u16 v) const
{
    return id == v;
}

u16 Battle2::Stats::GetID(void) const
{
    return id;
}

const Battle2::MonsterInfo & Battle2::Stats::GetMonsterInfo(void) const
{
    return monsters_info[troop.GetID()];
}

void Battle2::Stats::InitContours(void)
{
    const  MonsterInfo & msi = GetMonsterInfo();
    const Sprite *sprite1 = &AGG::GetICN(msi.icn_file, msi.frm_idle.start, false);
    const Sprite *sprite2 = &AGG::GetICN(msi.icn_file, msi.frm_idle.start, true);
    
    if(sprite1 && !contours[0])
    {
	contours[0] = new Sprite();
    	Surface::MakeContour(*contours[0], *sprite1, sprite1->GetColor(0xDA));
    }

    // revert
    if(sprite2 && !contours[1])
    {
	contours[1] = new Sprite();
	Surface::MakeContour(*contours[1], *sprite2, sprite2->GetColor(0xDA));
    }

    // create white black sprite
    if(sprite1 && !contours[2])
    {
	contours[2] = new Sprite(*sprite1);
	contours[2]->GrayScale();
    }
    if(sprite2 && !contours[3])
    {
	contours[3] = new Sprite(*sprite2);
	contours[3]->GrayScale();
    }
}

void Battle2::Stats::SetPosition(u16 pos)
{
    position = pos;
}

void Battle2::Stats::SetArena(Arena* a)
{
    arena = a;
}

Monster Battle2::Stats::GetMonster(void) const
{
    return troop;
}

void Battle2::Stats::SetReflection(bool r)
{
    if(isWide() && reflect != r)
	position = GetTailIndex();

    reflect = r;
}

u8 Battle2::Stats::GetShots(void) const
{
    return shots;
}

u16 Battle2::Stats::GetPosition(void) const
{
    return position;
}

u16 Battle2::Stats::GetTailIndex(void) const
{
    if(isWide())
    {
	if(Board::isValidDirection(position, reflect ? RIGHT : LEFT))
	    return Board::GetIndexDirection(position, reflect ? RIGHT : LEFT);

	DEBUG(DBG_BATTLE, DBG_WARN, "error: "<< "out of range");
    }

    return MAXU16;
}

void Battle2::Stats::SetRandomMorale(void)
{
    switch(troop.GetMorale())
    {
        case Morale::TREASON:   if(9 > Rand::Get(1, 16)) SetModes(MORALE_BAD); break;     // 50%
        case Morale::AWFUL:     if(6 > Rand::Get(1, 15)) SetModes(MORALE_BAD); break;     // 30%
	case Morale::POOR:      if(2 > Rand::Get(1, 15)) SetModes(MORALE_BAD); break;     // 15%
        case Morale::GOOD:      if(2 > Rand::Get(1, 15)) SetModes(MORALE_GOOD); break;    // 15%
        case Morale::GREAT:     if(6 > Rand::Get(1, 15)) SetModes(MORALE_GOOD); break;    // 30%
        case Morale::BLOOD:     if(9 > Rand::Get(1, 16)) SetModes(MORALE_GOOD); break;    // 50%
	default: break;
    }
}

void Battle2::Stats::SetRandomLuck(void)
{
    //check bone dragon
    const Army::army_t* enemy = arena->GetArmy(arena->GetOppositeColor(troop.GetColor()));

    s8 f = troop.GetLuck();
    if(enemy && enemy->HasMonster(Monster::BONE_DRAGON)) --f;

    switch(f)
    {
	case Luck::CURSED:      if(9 > Rand::Get(1, 16)) SetModes(LUCK_BAD); break;       // 50%
        case Luck::AWFUL:       if(6 > Rand::Get(1, 15)) SetModes(LUCK_BAD); break;       // 30%
        case Luck::BAD:         if(2 > Rand::Get(1, 15)) SetModes(LUCK_BAD); break;       // 15%
        case Luck::GOOD:        if(2 > Rand::Get(1, 15)) SetModes(LUCK_GOOD); break;      // 15%
        case Luck::GREAT:       if(6 > Rand::Get(1, 15)) SetModes(LUCK_GOOD); break;      // 30%
        case Luck::IRISH:       if(9 > Rand::Get(1, 16)) SetModes(LUCK_GOOD); break;      // 50%
        default: break;
    }

    if(Modes(SP_BLESS) && Modes(LUCK_GOOD))
	ResetModes(LUCK_GOOD);
    else
    if(Modes(SP_CURSE) && Modes(LUCK_BAD))
	ResetModes(LUCK_BAD);
}

bool Battle2::Stats::isFly(void) const
{
    return troop.isFly() && !Modes(SP_SLOW);
}

bool Battle2::Stats::isWide(void) const
{
    return troop.isWide();
}

bool Battle2::Stats::isUndead(void) const
{
    return troop.isUndead();
}

bool Battle2::Stats::isAlive(void) const
{
    return !isUndead() && !isElemental();
}

bool Battle2::Stats::isElemental(void) const
{
    return troop.isElemental();
}

bool Battle2::Stats::isValid(void) const
{
    return count;
}

bool Battle2::Stats::isReflect(void) const
{
    return reflect;
}

bool Battle2::Stats::OutOfWalls(void) const
{
    return ((position <=  8) ||
    	    (11 <= position && position <= 19) ||
    	    (22 <= position && position <= 29) ||
    	    (33 <= position && position <= 40) ||
    	    (44 <= position && position <= 50) ||
    	    (55 <= position && position <= 62) ||
    	    (66 <= position && position <= 73) ||
    	    (77 <= position && position <= 85) ||
    	    (88 <= position && position <= 96));
}

u8 Battle2::Stats::GetObstaclesPenalty(const Stats & attacker) const
{
    if(!arena || Modes(CAP_TOWER) || attacker.Modes(CAP_TOWER)) return 0;

    // check golden bow artifact
    const HeroBase* enemy = arena->army1.GetCommander();
    if(enemy && enemy->HasArtifact(Artifact::GOLDEN_BOW)) return 0;

    u8 result = 0;

    if(arena->castle)
    {
	// archery skill
	if(enemy && Skill::Level::NONE != enemy->GetLevelSkill(Skill::Secondary::ARCHERY)) return 0;

	if(attacker.troop.GetColor() == arena->army2.GetColor())
	    return 0;

	// check castle walls defensed
	if(troop.GetColor() == arena->army2.GetColor())
	{
	    if(!attacker.OutOfWalls() || OutOfWalls())
		return 0;

	    const Rect & pos1 = attacker.GetCellPosition();
	    const Rect & pos2 = GetCellPosition();
	    std::vector<Point> points;

	    GetLinePoints(pos1, pos2, pos1.w / 3, points);

	    std::vector<Point>::const_iterator it1 = points.begin();
	    std::vector<Point>::const_iterator it2 = points.end();
	    for(; it1 != it2; ++it1)
	    {
		if(0 == arena->board[8].object && (arena->board[8].pos & *it1)) return 0;
		else
		if(0 == arena->board[29].object && (arena->board[29].pos & *it1)) return 0;
		else
		if(0 == arena->board[73].object && (arena->board[73].pos & *it1)) return 0;
		else
		if(0 == arena->board[96].object && (arena->board[96].pos & *it1)) return 0;
	    }
	}

	result = 1;
    }
    else
    if(Settings::Get().ExtBattleObjectsArchersPenalty())
    {
	std::vector<Point> points;
	std::vector<u16> indexes;

	Rect pos1 = attacker.GetCellPosition();
	Rect pos2 = GetCellPosition();

	pos1.y += pos1.h / 2;
	pos2.y += pos2.h / 2;

	GetLinePoints(pos1, pos2, pos1.w / 3, points);
	arena->board.GetIndexesFromAbsPoints(indexes, points);

	if(indexes.size())
	{
	    for(u16 ii = 0; ii < indexes.size(); ++ii)
	    {
		const u16 index = indexes[ii];

		// obstacles
		switch(arena->board[index].object)
		{
		    // tree
		    case 0x82:
		    // trock
		    case 0x85:
		    // tree
		    case 0x89:
		    // tree
		    case 0x8D:
		    // rock
		    case 0x95:
		    case 0x96:
		    // stub
		    case 0x9A:
		    // dead tree
		    case 0x9B:
		    // tree
		    case 0x9C: ++result; break;

		    default: break;
		}
	    }
	}

	if(enemy)
	{
	    switch(enemy->GetLevelSkill(Skill::Secondary::ARCHERY))
	    {
		case Skill::Level::BASIC:	if(result < 2) return 0; break;
		case Skill::Level::ADVANCED:	if(result < 3) return 0; break;
		case Skill::Level::EXPERT:	return 0;
		default: break;
	    }
	}
    }

    return result;
}

bool Battle2::Stats::isHandFighting(void) const
{
    if(count && !Modes(CAP_TOWER))
    {
	for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
        {
    	    const Cell* cell = arena->GetCell(position, dir);
    	    const Stats* enemy = cell ? arena->GetTroopBoard(cell->index) : NULL;
	    if(enemy && (enemy->GetColor() != GetColor() || enemy->Modes(SP_HYPNOTIZE))) return true;
	}

	if(isWide())
	{
	    for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
    	    {
    		const Cell* cell = arena->GetCell(GetTailIndex(), dir);
    		const Stats* enemy = cell ? arena->GetTroopBoard(cell->index) : NULL;
		if(enemy && (enemy->GetColor() != GetColor() || enemy->Modes(SP_HYPNOTIZE))) return true;
	    }
	}
    }

    return false;
}

bool Battle2::Stats::isHandFighting(const Stats & a, const Stats & b)
{
    if(a.isValid() && !a.Modes(CAP_TOWER))
    {
	for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
        {
    	    const Cell* cell = a.arena->GetCell(a.position, dir);
    	    const Stats* enemy = cell ? a.arena->GetTroopBoard(cell->index) : NULL;
	    if(enemy && (enemy->GetColor() != a.GetColor() || enemy->Modes(SP_HYPNOTIZE)) && enemy->id == b.id) return true;
	}

	if(a.isWide())
	{
	    for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
    	    {
    		const Cell* cell = a.arena->GetCell(a.GetTailIndex(), dir);
    		const Stats* enemy = cell ? a.arena->GetTroopBoard(cell->index) : NULL;
		if(enemy && (enemy->GetColor() != a.GetColor() || enemy->Modes(SP_HYPNOTIZE)) && enemy->id == b.id) return true;
	    }
	}
    }

    return false;
}

void Battle2::Stats::NewTurn(void)
{
    if(isResurectLife()) hp = GetMonster().GetHitPoints() * count;

    ResetModes(TR_RESPONSED);
    ResetModes(TR_MOVED);
    ResetModes(TR_SKIPMOVE);
    ResetModes(TR_HARDSKIP);
    ResetModes(MORALE_BAD);
    ResetModes(MORALE_GOOD);
    ResetModes(LUCK_BAD);
    ResetModes(LUCK_GOOD);

    // decrease spell duration
    affected.DecreaseDuration();

    // remove spell duration
    u32 mode = 0;
    while(0 != (mode = affected.FindZeroDuration()))
    {
	affected.RemoveMode(mode);
	ResetModes(mode);

	// cancel mirror image
	if(mode == CAP_MIRRORIMAGE)
	{
    	    if(arena->interface) arena->interface->RedrawActionRemoveMirrorImage(*mirror);
	    Army::Troop & mirror_troop = mirror->troop;
	    mirror_troop.BattleQuit();
	    mirror_troop.Reset();
	    mirror = NULL;
	}
    }

    if(! Modes(SP_BLIND | IS_PARALYZE_MAGIC))
    {
	// define morale
	if(troop.isAffectedByMorale())
	    SetRandomMorale();

	// define luck
	SetRandomLuck();
    }
}

u8 Battle2::Stats::GetSpeed(bool skip_standing_check) const
{
    if(!skip_standing_check && (!count || Modes(TR_MOVED | SP_BLIND | IS_PARALYZE_MAGIC))) return Speed::STANDING;

    const u8 speed = GetMonster().GetSpeed();
    Spell spell;

    if(Modes(SP_HASTE))
    {
	spell = Spell::HASTE;
	return spell.ExtraValue() ? speed + spell.ExtraValue() : Speed::GetOriginalFast(speed);
    }
    else
    if(Modes(SP_SLOW))
    {
	spell = Spell::SLOW;
	return spell.ExtraValue() ? speed - spell.ExtraValue() : Speed::GetOriginalSlow(speed);
    }

    return speed;
}

u32 Battle2::Stats::GetDamageMin(const Stats & enemy) const
{
    return Modes(SP_BLESS) ? GetDamageMax(enemy) :
	CalculateDamageStats(enemy, GetMonster().GetDamageMin() * count);
}

u32 Battle2::Stats::GetDamageMax(const Stats & enemy) const
{
    return Modes(SP_CURSE) ? GetDamageMin(enemy) :
	CalculateDamageStats(enemy, GetMonster().GetDamageMax() * count);
}

u32 Battle2::Stats::CalculateDamageStats(const Stats & enemy, double dmg) const
{
    if(troop.isArchers())
    {
    	if(isHandFighting())
        {
    	    switch(troop())
            {
            	// skip
                case Monster::MAGE:
                case Monster::ARCHMAGE:
                case Monster::TITAN: break;

                default: dmg /= 2; break;
            }
    	}
        else
        {
            // check skill archery +%10, +%25, +%50
            if(GetCommander())
            {
		dmg += (dmg * GetCommander()->GetSecondaryValues(Skill::Secondary::ARCHERY) / 100);
	    }

	    // check castle defense
	    if(enemy.GetObstaclesPenalty(*this)) dmg /= 2;

	    // check spell shield
	    if(enemy.Modes(SP_SHIELD)) dmg /= Spell(Spell::SHIELD).ExtraValue();
	}
    }

    // check luck x2
    if(Modes(LUCK_GOOD)) dmg *= 2;
    else
    if(Modes(LUCK_BAD)) dmg /= 2;

    // after blind
    if(Modes(SP_BLIND)) dmg /= 2;

    // stone cap.
    if(enemy.Modes(SP_STONE)) dmg /= 2;

    // check capability
    switch(troop())
    {
	case Monster::CRUSADER:
	    // double damage for undead
	    if(enemy.troop.isUndead()) dmg *= 2;
	    break;

	default: break;
    }

    // approximate.. from faq
    int r = GetAttack() - enemy.GetDefense();
    if(enemy.troop.isDragons() && Modes(SP_DRAGONSLAYER)) r+= Spell(Spell::DRAGONSLAYER).ExtraValue();
    dmg *= 1 + (0 < r ? 0.1 * std::min(r,  20) : 0.05 * std::max(r, -15));

    switch(troop())
    {
	case Monster::GENIE:
	    // 10% half
	    if(dmg < enemy.hp &&
	       genie_enemy_half_percent >= Rand::Get(1, 100))
	    {
		dmg = enemy.hp / 2;
		if(arena->interface)
		{
		    std::string str(_("%{name} half the enemy troops!"));
		    String::Replace(str, "%{name}", GetName());
		    arena->interface->SetStatus(str, true);
		}
	    }
	    break;

	default: break;
    }

    return static_cast<u32>(dmg) < 1 ? 1 : static_cast<u32>(dmg);
}

u32 Battle2::Stats::GetDamage(const Stats & enemy) const
{
    if(Modes(SP_BLESS))
	return GetDamageMax(enemy);
    else
    if(Modes(SP_CURSE))
    	return GetDamageMin(enemy);

    return Rand::Get(GetDamageMin(enemy), GetDamageMax(enemy));
}

u32 Battle2::Stats::HowManyCanKill(const Stats & b) const
{
    return b.HowManyWillKilled((GetDamageMin(b) + GetDamageMax(b)) / 2);
}

u32 Battle2::Stats::HowManyWillKilled(u32 dmg) const
{
    return dmg >= hp ? count : count - Monster::GetCountFromHitPoints(troop(), hp - dmg);
}

u32 Battle2::Stats::ApplyDamage(u32 dmg)
{
    if(dmg && count)
    {
	u32 killed = HowManyWillKilled(dmg);

	// kill mirror image (slave)
	if(Modes(CAP_MIRRORIMAGE))
	{
    	    if(arena->interface) arena->interface->RedrawActionRemoveMirrorImage(*this);
    	    mirror->ResetModes(CAP_MIRROROWNER);
	    dmg = hp;
    	    killed = count;
    	    mirror = NULL;
	}

	DEBUG(DBG_BATTLE, DBG_TRACE, dmg << " to " << String() << " and killed: " << killed);

	dead += (killed >= count ? count : killed);
	count -= (killed >= count ? count : killed);
	hp -= (dmg >= hp ? hp : dmg);

	if(! isValid()) PostKilledAction();

	return killed;
    }

    return 0;
}

void Battle2::Stats::PostKilledAction(void)
{
    // kill mirror image (master)
    if(Modes(CAP_MIRROROWNER))
    {
    	if(arena->interface) arena->interface->RedrawActionRemoveMirrorImage(*mirror);
        modes = 0;
	mirror->hp = 0;
	mirror->count = 0;
	mirror->mirror = NULL;
        mirror->animstate = 0;
        mirror->animframe = 0;
	mirror = NULL;
	ResetModes(CAP_MIRROROWNER);
    }

    ResetModes(IS_MAGIC);
    ResetModes(TR_RESPONSED);
    ResetModes(TR_SKIPMOVE);
    ResetModes(LUCK_GOOD);
    ResetModes(LUCK_BAD);
    ResetModes(MORALE_GOOD);
    ResetModes(MORALE_BAD);

    SetModes(TR_MOVED);

    // save troop to graveyard
    // skip mirror and summon
    if(!Modes(CAP_MIRRORIMAGE) && !Modes(CAP_SUMMONELEM))
	arena->graveyard.AddTroop(*this);

    DEBUG(DBG_BATTLE, DBG_TRACE, String() << "is dead...");
    // possible also..
}

u32 Battle2::Stats::Resurrect(u32 points, bool allow_overflow, bool skip_dead)
{
    u32 resurrect = Monster::GetCountFromHitPoints(troop(), hp + points) - count;

    count += resurrect;
    hp += points;

    if(allow_overflow)
    {
	if(troop.count < count) troop.count = count;
    }
    else
    if(count > troop.count)
    {
	resurrect -= count - troop.count;
	count = troop.count;
	hp = count * GetMonster().GetHitPoints();
    }

    if(!skip_dead)
	dead -= (resurrect < dead ? resurrect : dead);

    return resurrect;
}

u32 Battle2::Stats::ApplyDamage(Stats & enemy, u32 dmg)
{
    const u32 killed = ApplyDamage(dmg);
    u32 resurrect;

    if(killed) switch(enemy.troop())
    {
	case Monster::GHOST:
	    resurrect = killed * enemy.GetMonster().GetHitPoints();
	    DEBUG(DBG_BATTLE, DBG_TRACE, String() << ", enemy: " << enemy.String() << " resurrect: " << resurrect);
	    // grow troop
	    enemy.Resurrect(resurrect, true, false);
	    break;

	case Monster::VAMPIRE_LORD:
	    resurrect = killed * GetMonster().GetHitPoints();
	    DEBUG(DBG_BATTLE, DBG_TRACE, String() << ", enemy: " << enemy.String() << " resurrect: " << resurrect);
	    // restore hit points
	    enemy.Resurrect(resurrect, false, false);
	    break;

	default: break;
    }

    // clean paralyze or stone magic
    if(Modes(IS_PARALYZE_MAGIC))
    {
	SetModes(TR_RESPONSED);
	SetModes(TR_MOVED);
        ResetModes(IS_PARALYZE_MAGIC);
        affected.RemoveMode(IS_PARALYZE_MAGIC);
    }

    return killed;
}

bool Battle2::Stats::AllowApplySpell(const Spell & spell, const HeroBase* hero, std::string *msg) const
{
    if(Modes(SP_ANTIMAGIC)) return false;

    if((Modes(CAP_MIRRORIMAGE) || Modes(CAP_MIRROROWNER)) &&
	(spell == Spell::ANTIMAGIC || spell == Spell::MIRRORIMAGE)) return false;

    // check global
    // if(arena->DisableCastSpell(spell, msg)) return false; // disable - recursion!

    if(hero && spell.isApplyToFriends() && GetColor() != hero->GetColor()) return false;
    if(hero && spell.isApplyToEnemies() && GetColor() == hero->GetColor()) return false;
    if(isMagicResist(spell, (hero ? hero->GetPower() : 0))) return false;

    const HeroBase* myhero = GetCommander();
    if(!myhero) return true;

    // check artifact
    Artifact guard_art(Artifact::UNKNOWN);
    switch(spell())
    {
	case Spell::CURSE:
	case Spell::MASSCURSE:	guard_art = Artifact::HOLY_PENDANT; break;
	case Spell::HYPNOTIZE:	guard_art = Artifact::PENDANT_FREE_WILL; break;
	case Spell::DEATHRIPPLE:
	case Spell::DEATHWAVE:	guard_art = Artifact::PENDANT_LIFE; break;
	case Spell::BERSERKER:	guard_art = Artifact::SERENITY_PENDANT; break;
	case Spell::BLIND:	guard_art = Artifact::SEEING_EYE_PENDANT; break;
	case Spell::PARALYZE:	guard_art = Artifact::KINETIC_PENDANT; break;
	case Spell::HOLYWORD:
	case Spell::HOLYSHOUT:	guard_art = Artifact::PENDANT_DEATH; break;
	case Spell::DISPEL:	guard_art = Artifact::WAND_NEGATION; break;

	default: break;
    }

    if(guard_art.isValid() && myhero->HasArtifact(guard_art))
    {
	if(msg)
	{
	    *msg = _("The %{artifact} artifact is in effect for this battle, disabling %{spell} spell.");
	    String::Replace(*msg, "%{artifact}", guard_art.GetName());
	    String::Replace(*msg, "%{spell}", spell.GetName());
	}
	return false;
    }

    return true;
}

bool Battle2::Stats::ApplySpell(const Spell & spell, const HeroBase* hero, TargetInfo & target)
{
    if(! AllowApplySpell(spell, hero)) return false;

    DEBUG(DBG_BATTLE, DBG_TRACE, spell.GetName() << " to " << String());

    u16 spoint = hero ? hero->GetPower() : 3;

    if(spell.isDamage())
	SpellApplyDamage(spell, spoint, hero, target);
    else
    if(spell.isRestore())
	SpellRestoreAction(spell, spoint, hero);
    else
    {
    	SpellModesAction(spell, spoint, hero);
    }

    return true;
}

std::string Battle2::Stats::String(bool more) const
{
    std::stringstream ss;

    ss << "Troop: " <<
	// id
	"0x" << std::setw(4) << std::setfill('0') << std::hex << id << std::dec <<
	// info
	" [ " << count << " " << GetName() << ", " << Color::String(GetColor());

    if(more)
	ss << ", mode(0x" << std::hex << modes <<
	std::dec <<
	", pos(" << position << (reflect ? ", reflect)" : ")") <<
	", speed(" << Speed::String(GetSpeed()) << ", " << static_cast<int>(GetSpeed()) << ")" <<
	", hp(" << hp << ")" << ", die(" << dead << ")" << ", cell(" << GetCellQuality() << ")";

    ss << " ]";

    return ss.str();
}

void Battle2::Stats::Pack(Action & msg) const
{
    msg.Push(modes);
    msg.Push(position);
    msg.Push(hp);
    msg.Push(count);
    msg.Push(dead);
    msg.Push(shots);
    msg.Push(disruptingray);
    msg.Push(static_cast<u8>(reflect));
    msg.Push(mirror ? mirror->GetID() : static_cast<u16>(0));
    msg.Push(static_cast<u32>(affected.size()));
    for(size_t ii = 0; ii < affected.size(); ++ii)
    {
	msg.Push(affected[ii].first);
	msg.Push(affected[ii].second);
    }
}

void Battle2::Stats::Unpack(Action & msg)
{
    u8 byte8; u16 byte16; u32 byte32;

    msg.Pop(modes);
    msg.Pop(position);
    msg.Pop(hp);
    msg.Pop(count);
    msg.Pop(dead);
    msg.Pop(shots);
    msg.Pop(disruptingray);
    msg.Pop(byte8); reflect = byte8;
    msg.Pop(byte16); if(byte16) mirror = arena->GetTroopID(byte16);
    msg.Pop(byte32); affected.clear();
    for(size_t ii = 0; ii < byte32; ++ii)
    {
	ModeDuration md;
	msg.Pop(md.first);
	msg.Pop(md.second);
	affected.push_back(md);
    }
}

bool Battle2::Stats::AllowResponse(void) const
{
    return (isAlwayResponse() || (!Modes(TR_RESPONSED) && !Modes(IS_PARALYZE_MAGIC)));
}

void Battle2::Stats::SetResponse(void)
{
    SetModes(TR_RESPONSED);
}

void Battle2::Stats::PostAttackAction(Stats & enemy)
{
    switch(troop())
    {
	case Monster::ARCHMAGE:
	    // 20% clean magic state
	    if(enemy.isValid() && enemy.Modes(IS_GOOD_MAGIC) && 3 > Rand::Get(1, 10)) enemy.ResetModes(IS_GOOD_MAGIC);
	    break;

	default: break;
    }

    // decrease shots
    if(isArchers())
    {
	// check ammo cart artifact
	const HeroBase* hero = GetCommander();
	if(!hero || !hero->HasArtifact(Artifact::AMMO_CART))
	    --shots;
    }

    // clean berserker spell
    if(Modes(SP_BERSERKER))
    {
        ResetModes(SP_BERSERKER);
        affected.RemoveMode(SP_BERSERKER);
    }

    // clean hypnotize spell
    if(Modes(SP_HYPNOTIZE))
    {
        ResetModes(SP_HYPNOTIZE);
        affected.RemoveMode(SP_HYPNOTIZE);
    }
    if(enemy.Modes(SP_HYPNOTIZE))
    {
        enemy.ResetModes(SP_HYPNOTIZE);
        enemy.affected.RemoveMode(SP_HYPNOTIZE);
    }

    // clean luck capability
    ResetModes(LUCK_GOOD);
    ResetModes(LUCK_BAD);
}

void Battle2::Stats::ResetBlind(void)
{
    // remove blind action
    if(Modes(SP_BLIND))
    {
    	SetModes(TR_MOVED);
        ResetModes(SP_BLIND);
        affected.RemoveMode(SP_BLIND);
    }
}

u16 Battle2::Stats::GetAttack(void) const
{
    u16 res = GetMonster().GetAttack() +
	(GetCommander() ? GetCommander()->GetAttack() : 0);

    if(Modes(SP_BLOODLUST)) res += Spell(Spell::BLOODLUST).ExtraValue();

    return res;
}

u16 Battle2::Stats::GetDefense(void) const
{
    s16 res = GetMonster().GetDefense() +
	(GetCommander() ? GetCommander()->GetDefense() : 0);

    if(Modes(SP_STONESKIN)) res += Spell(Spell::STONESKIN).ExtraValue();
    else
    if(Modes(SP_STEELSKIN)) res += Spell(Spell::STEELSKIN).ExtraValue();

    // extra
    if(Modes(TR_HARDSKIP) && Settings::Get().ExtBattleSkipIncreaseDefense()) res += 2;

    // disrupting ray accumulate effect
    if(disruptingray) res -= disruptingray * Spell(Spell::DISRUPTINGRAY).ExtraValue();
    if(0 > res) res = 1;

    return res;
}

const char* Battle2::Stats::GetName(void) const
{
    return troop.GetName();
}

const char* Battle2::Stats::GetPluralName(u32 count) const
{
    return troop.GetPluralName(count);
}

s32 Battle2::Stats::GetScoreQuality(const Stats & defender) const
{
    const Stats & attacker = *this;

    // initial value: (hitpoints)
    const u32 & damage = (attacker.GetDamageMin(defender) + attacker.GetDamageMax(defender)) / 2;
    const u32 & kills = defender.HowManyWillKilled(attacker.isTwiceAttack() ? damage * 2 : damage);
    double res = kills * defender.GetMonster().GetHitPoints();
    bool noscale = false;

    // attacker
    switch(attacker.GetMonster().GetID())
    {
	case Monster::GHOST:
	    // priority: from killed only
	    noscale = true;
	    break;

	case Monster::VAMPIRE_LORD:
	    if(attacker.isHaveDamage())
	    {
		// alive priority
		if(defender.isElemental() || defender.isUndead()) res /= 2;
	    }
	    break;

	default: break;
    }

    // scale on ability
    if(! noscale)
    {
	if(defender.isArchers())		res += res * 0.7;
	if(defender.isFly())			res += res * 0.6;
	if(defender.isHideAttack())		res += res * 0.5;
	if(defender.isTwiceAttack())		res += res * 0.4;
	if(defender.isResurectLife())		res += res * 0.3;
	if(defender.isDoubleCellAttack())	res += res * 0.3;
	if(defender.isAlwayResponse())		res -= res * 0.5;
    }

    // extra
    if(defender.Modes(CAP_MIRRORIMAGE)) res += res * 0.7;
    if(!attacker.isArchers())
    {
	if(defender.Modes(TR_RESPONSED))
	    res += res * 0.3;
	else
	{
	    if(defender.Modes(LUCK_BAD))  res += res * 0.3;
	    else
	    if(defender.Modes(LUCK_GOOD)) res -= res * 0.3;
	}
    }

    return res > 1.0 ? static_cast<u32>(res) : 1;
}

u32 Battle2::Stats::GetHitPoints(void) const
{
    return hp;
}

u8 Battle2::Stats::GetControl(void) const
{
    return Modes(SP_BERSERKER) ? CONTROL_AI : arena->GetArmy(GetColor())->GetControl();
}

bool Battle2::Stats::isArchers(void) const
{
    return troop.isArchers() && shots;
}

void Battle2::Stats::SpellModesAction(const Spell & spell, u8 duration, const HeroBase* hero)
{
    if(hero)
    {
	if(hero->HasArtifact(Artifact::WIZARD_HAT)) duration += Artifact(Artifact::WIZARD_HAT).ExtraValue();
	if(hero->HasArtifact(Artifact::ENCHANTED_HOURGLASS)) duration += Artifact(Artifact::ENCHANTED_HOURGLASS).ExtraValue();
    }

    switch(spell())
    {
	case Spell::BLESS:
	case Spell::MASSBLESS:
	    if(Modes(SP_CURSE))
	    {
		ResetModes(SP_CURSE);
		affected.RemoveMode(SP_CURSE);
	    }
	    SetModes(SP_BLESS);
	    affected.AddMode(SP_BLESS, duration);
	    break;

	case Spell::BLOODLUST:
	    SetModes(SP_BLOODLUST);
	    affected.AddMode(SP_BLOODLUST, 3);
	    break;

	case Spell::CURSE:
	case Spell::MASSCURSE:
	    if(Modes(SP_BLESS))
	    {
		ResetModes(SP_BLESS);
		affected.RemoveMode(SP_BLESS);
	    }
	    SetModes(SP_CURSE);
	    affected.AddMode(SP_CURSE, duration);
	    break;

	case Spell::HASTE:
	case Spell::MASSHASTE:
	    if(Modes(SP_SLOW))
	    {
		ResetModes(SP_SLOW);
		affected.RemoveMode(SP_SLOW);
	    }
	    SetModes(SP_HASTE);
	    affected.AddMode(SP_HASTE, duration);
	    break;

	case Spell::DISPEL:
	case Spell::MASSDISPEL:
	    if(Modes(IS_MAGIC))
	    {
		ResetModes(IS_MAGIC);
		affected.RemoveMode(IS_MAGIC);
	    }
	    break;

	case Spell::SHIELD:
	case Spell::MASSSHIELD:
	    SetModes(SP_SHIELD);
	    affected.AddMode(SP_SHIELD, duration);
	    break;

	case Spell::SLOW:
	case Spell::MASSSLOW:
	    if(Modes(SP_HASTE))
	    {
		ResetModes(SP_HASTE);
		affected.RemoveMode(SP_HASTE);
	    }
	    SetModes(SP_SLOW);
	    affected.AddMode(SP_SLOW, duration);
	    break;

	case Spell::STONESKIN:
	    if(Modes(SP_STEELSKIN))
	    {
		ResetModes(SP_STEELSKIN);
		affected.RemoveMode(SP_STEELSKIN);
	    }
	    SetModes(SP_STONESKIN);
	    affected.AddMode(SP_STONESKIN, duration);
	    break;

	case Spell::BLIND:
	    SetModes(SP_BLIND);
	    affected.AddMode(SP_BLIND, duration);
	    break;

	case Spell::DRAGONSLAYER:
	    SetModes(SP_DRAGONSLAYER);
	    affected.AddMode(SP_DRAGONSLAYER, duration);
	    break;

	case Spell::STEELSKIN:
	    if(Modes(SP_STONESKIN))
	    {
		ResetModes(SP_STONESKIN);
		affected.RemoveMode(SP_STONESKIN);
	    }
	    SetModes(SP_STEELSKIN);
	    affected.AddMode(SP_STEELSKIN, duration);
	    break;

	case Spell::ANTIMAGIC:
	    ResetModes(IS_MAGIC);
	    SetModes(SP_ANTIMAGIC);
	    affected.AddMode(SP_ANTIMAGIC, duration);
	    break;

	case Spell::PARALYZE:
	    SetModes(SP_PARALYZE);
	    affected.AddMode(SP_PARALYZE, duration);
	    break;

	case Spell::BERSERKER:
	    SetModes(SP_BERSERKER);
	    affected.AddMode(SP_BERSERKER, duration);
	    break;

	case Spell::HYPNOTIZE:
	    SetModes(SP_HYPNOTIZE);
	    affected.AddMode(SP_HYPNOTIZE, (hero && hero->HasArtifact(Artifact::GOLD_WATCH) ? duration * 2 : duration));
	    break;

        case Spell::STONE:
	    SetModes(SP_STONE);
	    affected.AddMode(SP_STONE, duration);
	    break;

	case Spell::MIRRORIMAGE:
	    affected.AddMode(CAP_MIRRORIMAGE, duration);
	    break;

	case Spell::DISRUPTINGRAY:
	    ++disruptingray;
	    break;

	default: break;
    }
}

void Battle2::Stats::SpellApplyDamage(const Spell & spell, u8 spoint, const HeroBase* hero, TargetInfo & target)
{
    u32 dmg = spell.Damage() * spoint;

    switch(troop())
    {
	case Monster::IRON_GOLEM:
	case Monster::STEEL_GOLEM:
	    switch(spell())
	    {
		// 50% damage
                case Spell::COLDRAY:
                case Spell::COLDRING:
                case Spell::FIREBALL:
                case Spell::FIREBLAST:
                case Spell::LIGHTNINGBOLT:
                case Spell::CHAINLIGHTNING:
                case Spell::ELEMENTALSTORM:
                case Spell::ARMAGEDDON:
            	    dmg /= 2; break;
                default: break;
            }
	    break;

        case Monster::WATER_ELEMENT:
	    switch(spell())
	    {
		// 200% damage
                case Spell::FIREBALL:
                case Spell::FIREBLAST:
            	    dmg *= 2; break;
                default: break;
            }
	    break;

	case Monster::AIR_ELEMENT:
	    switch(spell())
	    {
		// 200% damage
                case Spell::ELEMENTALSTORM:
                case Spell::LIGHTNINGBOLT:
                case Spell::CHAINLIGHTNING:
            	    dmg *= 2; break;
                default: break;
            }
	    break;

	case Monster::FIRE_ELEMENT:
	    switch(spell())
	    {
		// 200% damage
                case Spell::COLDRAY:
                case Spell::COLDRING:
            	    dmg *= 2; break;
                default: break;
            }
	    break;

	default: break;
    }

    // check artifact 
    if(hero)
    {
	const HeroBase* myhero = GetCommander();

	switch(spell())
	{
            case Spell::COLDRAY:
            case Spell::COLDRING:
		// +50%
    		if(hero->HasArtifact(Artifact::EVERCOLD_ICICLE)) dmg += dmg * Artifact(Artifact::EVERCOLD_ICICLE).ExtraValue() / 100;
		// -50%
    		if(myhero && myhero->HasArtifact(Artifact::ICE_CLOAK)) dmg /= 2;
    		if(myhero && myhero->HasArtifact(Artifact::HEART_ICE)) dmg -= dmg * Artifact(Artifact::HEART_ICE).ExtraValue() / 100;
    		// 100%
    		if(myhero && myhero->HasArtifact(Artifact::HEART_FIRE)) dmg *= 2;
    		break;

            case Spell::FIREBALL:
            case Spell::FIREBLAST:
		// +50%
    		if(hero->HasArtifact(Artifact::EVERHOT_LAVA_ROCK)) dmg += dmg * Artifact(Artifact::EVERHOT_LAVA_ROCK).ExtraValue() / 100;
		// -50%
    		if(myhero && myhero->HasArtifact(Artifact::FIRE_CLOAK)) dmg /= 2;
    		if(myhero && myhero->HasArtifact(Artifact::HEART_FIRE)) dmg -= dmg * Artifact(Artifact::HEART_FIRE).ExtraValue() / 100;
    		// 100%
    		if(myhero && myhero->HasArtifact(Artifact::HEART_ICE)) dmg *= 2;
    		break;

            case Spell::LIGHTNINGBOLT:
		// +50%
    		if(hero->HasArtifact(Artifact::LIGHTNING_ROD)) dmg += dmg * Artifact(Artifact::LIGHTNING_ROD).ExtraValue() / 100;
		// -50%
    		if(myhero && myhero->HasArtifact(Artifact::LIGHTNING_HELM)) dmg /= 2;
		break;

            case Spell::CHAINLIGHTNING:
		// +50%
    		if(hero->HasArtifact(Artifact::LIGHTNING_ROD)) dmg += dmg / 2;
		// -50%
    		if(myhero && myhero->HasArtifact(Artifact::LIGHTNING_HELM)) dmg /= 2;
		// update orders damage
		switch(target.damage)
		{
		    case 0: 	break;
		    case 1:	dmg /= 2; break;
		    case 2:	dmg /= 4; break;
		    case 3:	dmg /= 8; break;
		    default: break;
		}
    		break;

        case Spell::ELEMENTALSTORM:
        case Spell::ARMAGEDDON:
		// -50%
    		if(myhero && myhero->HasArtifact(Artifact::BROACH_SHIELDING)) dmg /= 2;
    		break;

	    default: break;
	}
    }

    // apply damage
    if(dmg)
    {
	target.damage = dmg;
	target.killed = ApplyDamage(dmg);
	if(target.defender && target.defender->Modes(SP_BLIND)) target.defender->ResetBlind();
    }
}

void Battle2::Stats::SpellRestoreAction(const Spell & spell, u8 spoint, const HeroBase* hero)
{
    switch(spell())
    {
	case Spell::CURE:
	case Spell::MASSCURE:
	    // clear bad magic
	    if(Modes(IS_BAD_MAGIC))
	    {
		ResetModes(IS_BAD_MAGIC);
		affected.RemoveMode(IS_BAD_MAGIC);
	    }
	    // restore
	    hp += (spell.Restore() * spoint);
	    if(hp > count * GetMonster().GetHitPoints()) hp = count * GetMonster().GetHitPoints();
	    break;

        case Spell::RESURRECT:
	case Spell::ANIMATEDEAD:
        case Spell::RESURRECTTRUE:
	{
	    u32 restore = spell.Resurrect() * spoint;
	    // remove from graveyard
	    if(!isValid())
	    {
		arena->graveyard.RemoveTroop(*this);
		ResetAnimFrame(AS_IDLE);
	    }
	    // restore hp
	    if(hero && hero->HasArtifact(Artifact::ANKH)) restore *= 2;

	    const u16 res = Resurrect(restore, false, (spell == Spell::RESURRECT));

	    if(arena->interface)
	    {
		std::string str(_("%{count} %{name} rise(s) from the dead!"));
		String::Replace(str, "%{count}", res);
		String::Replace(str, "%{name}", GetName());
		arena->interface->SetStatus(str, true);
	    }
	}
        break;

	default: break;
    }
}

bool Battle2::Stats::isTwiceAttack(void) const
{
    switch(troop())
    {
        case Monster::ELF:
        case Monster::GRAND_ELF:
        case Monster::RANGER:
	    return ! isHandFighting();

        default: break;
    }

    return troop.isTwiceAttack();
}

bool Battle2::Stats::isResurectLife(void) const
{
    switch(troop())
    {
        case Monster::TROLL:
        case Monster::WAR_TROLL:
            return true;

        default: break;
    }

    return false;
}

bool Battle2::Stats::isDoubleCellAttack(void) const
{
    switch(troop())
    {
        case Monster::CYCLOPS:
        case Monster::PHOENIX:
        case Monster::GREEN_DRAGON:
        case Monster::RED_DRAGON:
        case Monster::BLACK_DRAGON:
            return true;

        default: break;
    }

    return false;
}

bool Battle2::Stats::isMultiCellAttack(void) const
{
    switch(troop())
    {
        case Monster::HYDRA:
            return true;

        default: break;
    }

    return false;

}

bool Battle2::Stats::isHideAttack(void) const
{
    return troop.isHideAttack();
}

bool Battle2::Stats::isAlwayResponse(void) const
{
    switch(troop())
    {
        case Monster::GRIFFIN:
            return true;

        default: break;
    }

    return false;
}

bool Battle2::Stats::isMagicResist(const Spell & spell, u8 spower) const
{
    return 100 <= GetMagicResist(spell, spower);
}

u8 Battle2::Stats::GetMagicResist(const Spell & spell, u8 spower) const
{
    if(spell.isMindInfluence() &&
        (troop.isUndead() || troop.isElemental() ||
    	troop() == Monster::GIANT || troop() == Monster::TITAN)) return 100;

    if(spell.isALiveOnly() &&
        troop.isUndead()) return 100;

    if(spell.isUndeadOnly() &&
        !troop.isUndead()) return 100;

    if(Settings::Get().ExtBattleMagicTroopCanResist() && spell == GetSpellMagic(true))
	return 20;

    switch(troop())
    {
        case Monster::ARCHMAGE:
	    if(Settings::Get().ExtBattleArchmageCanResistBadMagic() &&
		(spell.isDamage() || spell.isApplyToEnemies())) return 20;
	    break;

	// 25% unfortunatly
	case Monster::DWARF:
	case Monster::BATTLE_DWARF:
	    if(spell.isDamage() || spell.isApplyToEnemies()) return 25;
            break;

        case Monster::GREEN_DRAGON:
        case Monster::RED_DRAGON:
        case Monster::BLACK_DRAGON:
            return 100;

        case Monster::PHOENIX:
            switch(spell())
            {
                case Spell::COLDRAY:
                case Spell::COLDRING:
                case Spell::FIREBALL:
                case Spell::FIREBLAST:
                case Spell::LIGHTNINGBOLT:
                case Spell::CHAINLIGHTNING:
                case Spell::ELEMENTALSTORM:
                    return 100;
                default: break;
            }
            break;

        case Monster::CRUSADER:
            switch(spell())
            {
                case Spell::CURSE:
                case Spell::MASSCURSE:
                    return 100;
                default: break;
            }
            break;

        case Monster::EARTH_ELEMENT:
            switch(spell())
            {
                case Spell::METEORSHOWER:
                case Spell::LIGHTNINGBOLT:
                case Spell::CHAINLIGHTNING:
                    return 100;
                default: break;
            }
            break;

        case Monster::AIR_ELEMENT:
            switch(spell())
            {
                case Spell::METEORSHOWER:
                    return 100;
                default: break;
            }
            break;

        case Monster::FIRE_ELEMENT:
            switch(spell())
            {
                case Spell::FIREBALL:
                case Spell::FIREBLAST:
                    return 100;
                default: break;
            }
            break;

        case Monster::WATER_ELEMENT:
            switch(spell())
            {
                case Spell::COLDRAY:
                case Spell::COLDRING:
                    return 100;
                default: break;
            }
            break;

        default: break;
    }

    switch(spell())
    {
	case Spell::CURE:
	case Spell::MASSCURE:
	    if(troop.isUndead()) return 100;
	    if(!isHaveDamage() && !(modes & IS_MAGIC)) return 100;
	    break;

	case Spell::RESURRECT:
	case Spell::RESURRECTTRUE:
	case Spell::ANIMATEDEAD:
	    if(!isHaveDamage()) return 100;
	    break;

	case Spell::DISPEL:
	    if(! (modes & IS_MAGIC)) return 100;
	    break;

	case Spell::HYPNOTIZE:
	    if(spell.ExtraValue() * spower < hp) return 100;
	    break;

	default: break;
    }

    return 0;
}

bool Battle2::Stats::isMagicAttack(void) const
{
    return GetSpellMagic(true) != Spell::NONE;
}


u8 Battle2::Stats::GetSpellMagic(bool force) const
{
    switch(troop())
    {
        case Monster::UNICORN:
            // 20% blind
            if(force || 3 > Rand::Get(1, 10)) return Spell::BLIND;
            break;

        case Monster::CYCLOPS:
            // 20% paralyze
            if(force || 3 > Rand::Get(1, 10)) return Spell::PARALYZE;
            break;

        case Monster::MUMMY:
            // 20% curse
            if(force || 3 > Rand::Get(1, 10)) return Spell::CURSE;
            break;

        case Monster::ROYAL_MUMMY:
            // 30% curse
            if(force || 4 > Rand::Get(1, 10)) return Spell::CURSE;
            break;

        /* skip: see Stats::PostAttackAction
	case Monster::ARCHMAGE:
            // 20% dispel
            if(!force && 3 > Rand::Get(1, 10)) return Spell::DISPEL;
            break;
	*/

        case Monster::MEDUSA:
            // 20% stone
            if(force || 3 > Rand::Get(1, 10)) return Spell::STONE;
            break;

        default: break;
    }

    return Spell::NONE;
}

bool Battle2::Stats::isHaveDamage(void) const
{
    return hp < troop.count * GetMonster().GetHitPoints();
}

u8 Battle2::Stats::GetFrameStart(void) const
{
    return animstep < 0 ? GetFrameState().start + GetFrameState().count - 1 : GetFrameState().start;
}

u8 Battle2::Stats::GetFrameOffset(void) const
{
    return animframe - GetFrameStart();
}

u8 Battle2::Stats::GetFrameCount(void) const
{
    return GetFrameState().count;
}

void Battle2::Stats::IncreaseAnimFrame(bool loop)
{
    if(! isFinishAnimFrame())
	animframe += animstep;
    else
    if(loop)
	animframe = GetFrameStart();
}

bool Battle2::Stats::isStartAnimFrame(void) const
{
    return GetFrameStart() == animframe;
}

bool Battle2::Stats::isFinishAnimFrame(void) const
{
    if(0 == GetFrameState().count)
	return true;
    else
    if(animstep < 0)
	return animframe <= GetFrameState().start;
    else
    if(animstep > 0)
	return animframe >= GetFrameState().start + GetFrameState().count - 1;

    return true;
}

const Battle2::animframe_t & Battle2::Stats::GetFrameState(u8 state) const
{
    const  MonsterInfo & msi = GetMonsterInfo();

    switch(state)
    {
	case AS_IDLE:	return msi.frm_idle;
        case AS_MOVE:	return msi.frm_move;
        case AS_FLY1:	return msi.frm_fly1;
        case AS_FLY2:	return msi.frm_fly2;
        case AS_FLY3:	return msi.frm_fly3;
        case AS_SHOT0:	return msi.frm_shot0;
        case AS_SHOT1:	return msi.frm_shot1;
        case AS_SHOT2:	return msi.frm_shot2;
        case AS_SHOT3:	return msi.frm_shot3;
        case AS_ATTK0:	return msi.frm_attk0;
        case AS_ATTK1:	return msi.frm_attk1;
        case AS_ATTK2:	return msi.frm_attk2;
        case AS_ATTK3:	return msi.frm_attk3;
        case AS_WNCE:	return msi.frm_wnce;
        case AS_KILL:	return msi.frm_kill;
	default: break;
    }

    return msi.frm_idle;
}

const Battle2::animframe_t & Battle2::Stats::GetFrameState(void) const
{
    return GetFrameState(animstate);
}

void Battle2::Stats::ResetAnimFrame(u8 rule)
{
    animstep = 1;
    animstate = rule;
    animframe = GetFrameStart();

    if(AS_FLY3 == rule && 0 == GetFrameState().count)
    {
	animstep = -1;
	animstate = AS_FLY1;
	animframe = GetFrameStart();
    }
}

M82::m82_t Battle2::Stats::M82Attk(void) const
{
    if(isArchers() && !isHandFighting())
    {
	switch(troop())
	{
	    case Monster::ARCHER:
	    case Monster::RANGER:	return M82::ARCHSHOT;
	    case Monster::ORC:
	    case Monster::ORC_CHIEF:	return M82::ORC_SHOT;
	    case Monster::TROLL:
	    case Monster::WAR_TROLL:	return M82::TRLLSHOT;
	    case Monster::ELF:
	    case Monster::GRAND_ELF:	return M82::ELF_SHOT;
	    case Monster::DRUID:
	    case Monster::GREATER_DRUID:return M82::DRUISHOT;
	    case Monster::CENTAUR:	return M82::CNTRSHOT;
	    case Monster::HALFLING:	return M82::HALFSHOT;
	    case Monster::MAGE:
	    case Monster::ARCHMAGE:	return M82::MAGESHOT;
	    case Monster::TITAN:	return M82::TITNSHOT;
	    case Monster::LICH:
	    case Monster::POWER_LICH:	return M82::LICHSHOT;
	    default: break;
	}
    }

    return GetMonsterInfo().m82_attk;
}

M82::m82_t Battle2::Stats::M82Kill(void) const
{
    return GetMonsterInfo().m82_kill;
}

M82::m82_t Battle2::Stats::M82Move(void) const
{
    return GetMonsterInfo().m82_move;
}

M82::m82_t Battle2::Stats::M82Wnce(void) const
{
    return GetMonsterInfo().m82_wnce;
}

M82::m82_t Battle2::Stats::M82Expl(void) const
{
    switch(troop.GetID())
    {
        case Monster::VAMPIRE:           return M82::VAMPEXT1;
        case Monster::VAMPIRE_LORD:      return M82::VAMPEXT1;
        case Monster::LICH:              return M82::LICHEXPL;
        case Monster::POWER_LICH:        return M82::LICHEXPL;

        default: break;
    }

    return M82::UNKNOWN;
}

ICN::icn_t Battle2::Stats::ICNFile(void) const
{
    return GetMonsterInfo().icn_file;
}

ICN::icn_t Battle2::Stats::ICNMiss(void) const
{
    switch(troop.GetID())
    {
        case Monster::ARCHER:            return ICN::ARCH_MSL;
        case Monster::RANGER:            return ICN::ARCH_MSL;
        case Monster::ORC:               return ICN::ORC__MSL;
        case Monster::ORC_CHIEF:         return ICN::ORC__MSL;
        case Monster::TROLL:             return ICN::TROLLMSL;
        case Monster::WAR_TROLL:         return ICN::TROLLMSL;
        case Monster::ELF:               return ICN::ELF__MSL;
        case Monster::GRAND_ELF:         return ICN::ELF__MSL;
        case Monster::DRUID:             return ICN::DRUIDMSL;
        case Monster::GREATER_DRUID:     return ICN::DRUIDMSL;
        case Monster::CENTAUR:           return ICN::ARCH_MSL;
        case Monster::HALFLING:          return ICN::HALFLMSL;
        case Monster::MAGE:              return ICN::DRUIDMSL;
        case Monster::ARCHMAGE:          return ICN::DRUIDMSL;
        case Monster::TITAN:             return ICN::TITANMSL;
        case Monster::LICH:              return ICN::LICH_MSL;
        case Monster::POWER_LICH:        return ICN::LICH_MSL;

        default: break;
    }

    return ICN::UNKNOWN;
}

u32 Battle2::Stats::GetCount(void) const
{
    return count;
}

s32 Battle2::Stats::GetCellQuality(void) const
{
    return arena && arena->board.size() > position ? arena->board[position].quality : 0;
}

const Rect & Battle2::Stats::GetCellPosition(void) const
{
    return arena->board[position].pos;
}

bool Battle2::Stats::AppertainsArmy1(void) const
{
    return arena && arena->army1.GetColor() == troop.GetColor();
}

void Battle2::Stats::UpdateDirection(void)
{
    if(reflect && AppertainsArmy1())
	SetReflection(false);
    else
    if(!reflect && !AppertainsArmy1())
	SetReflection(true);
}

bool Battle2::Stats::UpdateDirection(const Cell & cell)
{
    bool need = arena && arena->board[position].pos.x > cell.pos.x;

    if(need != reflect)
    {
	SetReflection(need);
	return true;
    }
    return false;
}

s8 Battle2::Stats::GetStartMissileOffset(u8 state) const
{
    switch(troop.GetID())
    {
	case Monster::ARCHER:
	case Monster::RANGER:
	    switch(state)
	    {
    		case AS_SHOT1: return -15;
    		case AS_SHOT2: return -3;
    		case AS_SHOT3: return 10;
    		default: break;
	    }
	    break;

	case Monster::ORC:
	case Monster::ORC_CHIEF:
    		return 5;

	case Monster::TROLL:
	case Monster::WAR_TROLL:
    		return -20;

	case Monster::LICH:
	case Monster::POWER_LICH:
	    switch(state)
	    {
    		case AS_SHOT1: return -30;
    		case AS_SHOT2: return -20;
    		case AS_SHOT3: return 0;
    		default: break;
	    }
	    break;

	case Monster::ELF:
	case Monster::GRAND_ELF:
	    switch(state)
	    {
    		case AS_SHOT1: return -5;
    		case AS_SHOT2: return 0;
    		case AS_SHOT3: return 5;
    		default: break;
	    }
	    break;

	case Monster::CENTAUR:
	    switch(state)
	    {
    		case AS_SHOT1: return -20;
    		case AS_SHOT2: return -10;
    		case AS_SHOT3: return 5;
    		default: break;
	    }
	    break;

	case Monster::DRUID:
	case Monster::GREATER_DRUID:
	    switch(state)
	    {
    		case AS_SHOT1: return -20;
    		case AS_SHOT2: return -5;
    		case AS_SHOT3: return 15;
    		default: break;
	    }
	    break;

	case Monster::HALFLING:
	    switch(state)
	    {
    		case AS_SHOT1: return -20;
    		case AS_SHOT2: return 10;
    		case AS_SHOT3: return 20;
    		default: break;
	    }
	    break;

	case Monster::MAGE:
	case Monster::ARCHMAGE:
	    switch(state)
	    {
    		case AS_SHOT1: return -40;
    		case AS_SHOT2: return -10;
    		case AS_SHOT3: return 25;
    		default: break;
	    }
	    break;

	case Monster::TITAN:
	    switch(state)
	    {
    		case AS_SHOT1: return -80;
    		case AS_SHOT2: return -20;
    		case AS_SHOT3: return 15;
    		default: break;
	    }
	    break;

	default: break;
    }

    return 0;
}

u8 Battle2::Stats::GetArmyColor(void) const
{
    return troop.GetColor();
}

u8 Battle2::Stats::GetColor(void) const
{
    return (Modes(SP_BERSERKER) ? 0 : Modes(SP_HYPNOTIZE) ? arena->GetOppositeColor(troop.GetColor()) : troop.GetColor());
}

const Army::army_t* Battle2::Stats::GetArmy(void) const
{
    return arena ? arena->GetArmy(troop.GetColor()) : NULL;
}

Army::army_t* Battle2::Stats::GetArmy(void)
{
    return arena ? arena->GetArmy(troop.GetColor()) : NULL;
}

const HeroBase* Battle2::Stats::GetCommander(void) const
{
    return GetArmy() ? GetArmy()->GetCommander() : NULL;
}

HeroBase* Battle2::Stats::GetCommander(void)
{
    return GetArmy() ? GetArmy()->GetCommander() : NULL;
}

namespace Battle2
{
    bool SlowestStats(const Stats* b1, const Stats* b2)
    {
	return Army::SlowestTroop(b1->troop, b2->troop);
    }

    bool FastestStats(const Stats* b1, const Stats* b2)
    {
	return SlowestStats(b2, b1);
    }

    bool StrongestStats(const Stats* b1, const Stats* b2)
    {
	return Army::StrongestTroop(b1->troop, b2->troop);
    }

    bool WeakestStats(const Stats* b1, const Stats* b2)
    {
	return Army::WeakestTroop(b1->troop, b2->troop);
    }

    bool AllowPart1(const Stats* b)
    {
	return ! b->Modes(TR_SKIPMOVE) && Speed::STANDING < b->GetSpeed();
    }

    bool AllowPart2(const Stats* b)
    {
	return b->Modes(TR_SKIPMOVE) && Speed::STANDING < b->GetSpeed();
    }
}

Battle2::Armies::Armies(Army::army_t & a) : parent(a)
{
    reserve(a.troops.size());
    Init();
}

void Battle2::Armies::Init(void)
{
    clear();

    for(Army::Troops::const_iterator
        it = parent.troops.begin(); it != parent.troops.end(); ++it)
            if((*it).isValid() && (*it).battle) push_back((*it).battle);
}

Battle2::Stats* Battle2::Armies::GetRandom(void)
{
    return *Rand::Get(*this);
}

void Battle2::Armies::SortSlowest(void)
{
    std::sort(begin(), end(), SlowestStats);
}

void Battle2::Armies::SortFastest(void)
{
    std::sort(begin(), end(), FastestStats);
}

void Battle2::Armies::SortStrongest(void)
{
    std::sort(begin(), end(), StrongestStats);
}

void Battle2::Armies::SortWeakest(void)
{
    std::sort(begin(), end(), WeakestStats);
}

void Battle2::Armies::NewTurn(void)
{
    if(parent.commander)
	parent.commander->ResetModes(Heroes::SPELLCASTED);

    std::for_each(begin(), end(), std::mem_fun(&Stats::NewTurn));
}

Battle2::Stats* Battle2::Armies::FindID(u16 id)
{
    Armies::iterator it = std::find_if(begin(), end(),
				std::bind2nd(std::mem_fun(&Stats::isID), id));

    return it == end() ? NULL : *it;
}

Battle2::Stats* Battle2::Armies::FindMode(u32 mod)
{
    Armies::iterator it = std::find_if(begin(), end(),                                                
				std::bind2nd(std::mem_fun(&Stats::Modes), mod));

    return it == end() ? NULL : *it;
}

Battle2::Stats* Battle2::Armies::CreateNewStats(const Monster & mons, u32 count)
{
    Army::Troops & armies = parent.troops;
    Army::Troops::iterator it = armies.begin();

    // find free invalid
    while(it != armies.end() && (*it).battle) ++it;

    if(armies.end() == it)
    {
        armies.resize(armies.size() + 1);
        it = armies.end() - 1;
    }

    (*it).Set(mons, count);
    (*it).army = &parent;

    (*it).BattleInit();

    push_back((*it).battle);

    return (*it).battle;
}

Battle2::Stats* Battle2::Armies::GetStatsPart1(Armies & armies1, Armies & armies2, Stats* last)
{
    armies1.Init();
    armies2.Init();

    armies1.SortFastest();
    armies2.SortFastest();

    Stats* result = NULL;

    Armies::iterator it1 = std::find_if(armies1.begin(), armies1.end(), AllowPart1);
    Armies::iterator it2 = std::find_if(armies2.begin(), armies2.end(), AllowPart1);

    if(it1 != armies1.end() &&
	it2 != armies2.end())
    {
	// attacker first
	if((*it1)->GetSpeed() > (*it2)->GetSpeed())
	{
	    result = *it1;
	}
	else
	if((*it2)->GetSpeed() > (*it1)->GetSpeed())
	{
	    result = *it2;
	}
	else
	{
	    // attacker first
	    if(!last ||
		armies2.parent.GetColor() == last->GetColor())
	    {
		result = *it1;
	    }
	    else
	    {
		result = *it2;
	    }
	}
    }
    else
    if(it1 != armies1.end())
	result = *it1;
    else
    if(it2 != armies2.end())
	result = *it2;

    return result &&
	result->isValid() &&
	result->GetSpeed() > Speed::STANDING ? result : NULL;
}

Battle2::Stats* Battle2::Armies::GetStatsPart2(Armies & armies1, Armies & armies2, Stats* last)
{
    armies1.Init();
    armies2.Init();

    armies1.SortFastest();
    armies2.SortFastest();

    Stats* result = NULL;

    Armies::reverse_iterator it1 = std::find_if(armies1.rbegin(), armies1.rend(), AllowPart2);
    Armies::reverse_iterator it2 = std::find_if(armies2.rbegin(), armies2.rend(), AllowPart2);

    if(it1 != armies1.rend() &&
	it2 != armies2.rend())
    {
	// defender first
	if((*it2)->GetSpeed() < (*it1)->GetSpeed())
	{
	    result = *it2;
	}
	else
	if((*it1)->GetSpeed() < (*it2)->GetSpeed())
	{
	    result = *it1;
	}
	else
	{
	    // defender first
	    if(!last ||
		armies1.parent.GetColor() == last->GetColor())
	    {
		result = *it2;
	    }
	    else
	    {
		result = *it1;
	    }
	}
    }
    else
    if(it1 != armies1.rend())
	result = *it1;
    else
    if(it2 != armies2.rend())
	result = *it2;

    return result &&
	result->isValid() &&
	result->GetSpeed() > Speed::STANDING ? result : NULL;
}

void Battle2::Armies::PackStats(Action & msg) const
{
    msg.Push(static_cast<u32>(size()));
    for(const_iterator it = begin(); it != end(); ++it)
    {
	const Stats & b = (**it);
	msg.Push(b.GetID());
	b.Pack(msg);
    }
}

void Battle2::Armies::UnpackStats(Action & msg)
{
    u32 size;
    msg.Pop(size);
    for(u32 ii = 0; ii < size; ++ii)
    {
	u16 id;
	msg.Pop(id);
	Stats* b = FindID(id);
	if(b) b->Unpack(msg);
    }
}
