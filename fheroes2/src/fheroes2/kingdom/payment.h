/*************************************************************************** 
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2PAYMENT_H
#define H2PAYMENT_H

#include "resource.h"

#define RATE_DIFFERENCE_UPGRADE		2

#define RECRUIT_HEROES_GOLD		2500

// profit
#define INCOME_CASTLE_GOLD              1000
#define INCOME_TOWN_GOLD                250
#define INCOME_STATUE_GOLD              250
#define INCOME_DUNGEON_GOLD             500
#define INCOME_ENDLESS_SACK_GOLD	1000
#define INCOME_ENDLESS_BAG_GOLD		750
#define INCOME_ENDLESS_PURSE_GOLD	500
#define INCOME_ENDLESS_POUCH_SULFUR	1
#define INCOME_ENDLESS_VIAL_MERCURY	1
#define INCOME_ENDLESS_POUCH_GEMS	1
#define INCOME_ENDLESS_CORD_WOOD	2
#define INCOME_ENDLESS_CART_ORE		2
#define INCOME_ENDLESS_POUCH_CRYSTAL	1

// building cost
#define BUILD_CASTLE_GOLD		5000
#define BUILD_CASTLE_WOOD		20
#define BUILD_CASTLE_ORE		20
#define BUILD_MAGEGUILD1_GOLD		2000
#define BUILD_MAGEGUILD1_ORE		5
#define BUILD_MAGEGUILD1_WOOD		5
#define BUILD_MAGEGUILD2_GOLD		1000
#define BUILD_MAGEGUILD2_ORE		5
#define BUILD_MAGEGUILD2_WOOD		5
#define BUILD_MAGEGUILD2_CRYSTAL	4
#define BUILD_MAGEGUILD2_GEMS		4
#define BUILD_MAGEGUILD2_MERCURY	4
#define BUILD_MAGEGUILD2_SULFUR		4
#define BUILD_MAGEGUILD3_GOLD		1000
#define BUILD_MAGEGUILD3_ORE		5
#define BUILD_MAGEGUILD3_WOOD		5
#define BUILD_MAGEGUILD3_CRYSTAL	6
#define BUILD_MAGEGUILD3_GEMS		6
#define BUILD_MAGEGUILD3_MERCURY	6
#define BUILD_MAGEGUILD3_SULFUR		6
#define BUILD_MAGEGUILD4_GOLD		1000
#define BUILD_MAGEGUILD4_ORE		5
#define BUILD_MAGEGUILD4_WOOD		5
#define BUILD_MAGEGUILD4_CRYSTAL	8
#define BUILD_MAGEGUILD4_GEMS		8
#define BUILD_MAGEGUILD4_MERCURY	8
#define BUILD_MAGEGUILD4_SULFUR		8
#define BUILD_MAGEGUILD5_GOLD		1000
#define BUILD_MAGEGUILD5_ORE		5
#define BUILD_MAGEGUILD5_WOOD		5
#define BUILD_MAGEGUILD5_CRYSTAL	10
#define BUILD_MAGEGUILD5_GEMS		10
#define BUILD_MAGEGUILD5_MERCURY	10
#define BUILD_MAGEGUILD5_SULFUR		10
#define BUILD_TAVERN_GOLD		500
#define BUILD_TAVERN_WOOD		5
#define BUILD_THIEVESGUILD_GOLD		750
#define BUILD_THIEVESGUILD_WOOD		5
#define BUILD_SHIPYARD_GOLD		2000
#define BUILD_SHIPYARD_WOOD		20
#define BUILD_STATUE_GOLD		1250
#define BUILD_STATUE_ORE		5
#define BUILD_MARKETPLACE_GOLD		500
#define BUILD_MARKETPLACE_WOOD		5
#define BUILD_WELL_GOLD			500
#define BUILD_MOAT_GOLD			750
#define BUILD_LEFTTURRET_GOLD		1500
#define BUILD_LEFTTURRET_ORE		5
#define BUILD_RIGHTTURRET_GOLD		1500
#define BUILD_RIGHTTURRET_ORE		5
#define BUILD_CAPTAIN_GOLD		500
#define BUILD_WEL2_GOLD			1000
// spec
#define BUILD_FORTIFICATION_GOLD	1500
#define BUILD_FORTIFICATION_ORE		15
#define BUILD_FORTIFICATION_WOOD	5
#define BUILD_COLISEUM_GOLD		2000
#define BUILD_COLISEUM_WOOD		10
#define BUILD_COLISEUM_ORE		10
#define BUILD_STORM_GOLD		1000
#define BUILD_STORM_MERCURY		10
#define BUILD_STORM_SULFUR		10
#define BUILD_RAINBOW_GOLD		1500
#define BUILD_RAINBOW_CRYSTAL		10
#define BUILD_DUNGEON_GOLD		3000
#define BUILD_DUNGEON_ORE		10
#define BUILD_DUNGEON_WOOD		5
#define BUILD_LIBRARY_GOLD		1500
#define BUILD_LIBRARY_WOOD		5
#define BUILD_LIBRARY_ORE		5
#define BUILD_LIBRARY_CRYSTAL		5
#define BUILD_LIBRARY_GEMS		5
#define BUILD_LIBRARY_MERCURY		5
#define BUILD_LIBRARY_SULFUR		5
#define BUILD_SHRINE_GOLD		4000
#define BUILD_SHRINE_WOOD		10
#define BUILD_SHRINE_CRYSTAL		10
// dwelling level 1 
#define BUILD_THATCHEDHUT_GOLD          200 
#define BUILD_EXCAVATION_GOLD           400 
#define BUILD_HUT_GOLD                  300 
#define BUILD_TREEHOUSE_GOLD            500 
#define BUILD_TREEHOUSE_WOOD            5 
#define BUILD_HABITAT_GOLD              400 
#define BUILD_CAVE_GOLD                 500 
// dwelling level 2
#define BUILD_ARCHERYRANGE_GOLD		1000
#define BUILD_STICKHUT_GOLD		800
#define BUILD_STICKHUT_WOOD		5
#define BUILD_GRAVEYARD_GOLD		1000
#define BUILD_COTTAGE_GOLD		1000
#define BUILD_COTTAGE_WOOD		5
#define BUILD_PEN_GOLD			800
#define BUILD_CRYPT_GOLD		1000
#define BUILD_CRYPT_ORE			10
// dwelling level 3
#define BUILD_BLACKSMITH_GOLD		1000
#define BUILD_BLACKSMITH_ORE		5
#define BUILD_DEN_GOLD			1000
#define BUILD_NECPYRAMID_GOLD		1500
#define BUILD_NECPYRAMID_ORE		10
#define BUILD_ELVESRANGE_GOLD		1500
#define BUILD_FOUNDRY_GOLD		1500
#define BUILD_FOUNDRY_WOOD		5
#define BUILD_FOUNDRY_ORE		5
#define BUILD_NEST_GOLD			2000
// dwelling level 4
#define BUILD_ARMORY_GOLD		2000
#define BUILD_ARMORY_WOOD		10
#define BUILD_ARMORY_ORE		10
#define BUILD_ADOBE_GOLD		2000
#define BUILD_ADOBE_WOOD		10
#define BUILD_ADOBE_ORE			10
#define BUILD_MANSION_GOLD		3000
#define BUILD_MANSION_WOOD		10
#define BUILD_STONEHENGE_GOLD		1500
#define BUILD_STONEHENGE_ORE		10
#define BUILD_CLIFFNEST_GOLD		3000
#define BUILD_CLIFFNEST_WOOD		5
#define BUILD_MAZE_GOLD			3000
#define BUILD_MAZE_GEMS			10
// dwelling level 5
#define BUILD_ARENA_GOLD		3000
#define BUILD_ARENA_WOOD		20
#define BUILD_BRIDGE_GOLD		4000
#define BUILD_BRIDGE_ORE		20
#define BUILD_MAUSOLEUM_GOLD		4000
#define BUILD_MAUSOLEUM_WOOD		10
#define BUILD_MAUSOLEUM_SULFUR		10
#define BUILD_FENCED_GOLD		3000
#define BUILD_FENCED_WOOD		10
#define BUILD_FENCED_GEMS		10
#define BUILD_IVORYTOWER_GOLD		3500
#define BUILD_IVORYTOWER_WOOD		5
#define BUILD_IVORYTOWER_ORE		5
#define BUILD_IVORYTOWER_CRYSTAL	5
#define BUILD_IVORYTOWER_MERCURY	5
#define BUILD_IVORYTOWER_SULFUR		5
#define BUILD_IVORYTOWER_GEMS		5
#define BUILD_SWAMP_GOLD		4000
#define BUILD_SWAMP_SULFUR		10
// dwelling level 6
#define BUILD_CATHEDRAL_GOLD		5000
#define BUILD_CATHEDRAL_WOOD		20
#define BUILD_CATHEDRAL_CRYSTAL		20
#define BUILD_PYRAMID_GOLD		6000
#define BUILD_PYRAMID_ORE		20
#define BUILD_PYRAMID_CRYSTAL		20
#define BUILD_LABORATORY_GOLD		10000
#define BUILD_LABORATORY_WOOD		10
#define BUILD_LABORATORY_ORE		10
#define BUILD_LABORATORY_CRYSTAL	5
#define BUILD_LABORATORY_SULFUR		5
#define BUILD_LABORATORY_MERCURY	5
#define BUILD_LABORATORY_GEMS		5
#define BUILD_REDTOWER_GOLD		10000
#define BUILD_REDTOWER_ORE		30
#define BUILD_REDTOWER_MERCURY		20
#define BUILD_CLOUDCASTLE_GOLD		12500
#define BUILD_CLOUDCASTLE_ORE		5
#define BUILD_CLOUDCASTLE_WOOD		5
#define BUILD_CLOUDCASTLE_GEMS		20
#define BUILD_GREENDRAGON_GOLD		15000
#define BUILD_GREENDRAGON_ORE		30
#define BUILD_GREENDRAGON_SULFUR	20
// upgrade level 2
#define UPGRADE_ARCHERYRANGE_GOLD	1500
#define UPGRADE_ARCHERYRANGE_WOOD	5
#define UPGRADE_STICKHUT_GOLD		1200
#define UPGRADE_STICKHUT_WOOD		5
#define UPGRADE_GRAVEYARD_GOLD		1000
#define UPGRADE_COTTAGE_GOLD		1500
#define UPGRADE_COTTAGE_WOOD		5
// upgrade level 3
#define UPGRADE_BLACKSMITH_GOLD		1500
#define UPGRADE_BLACKSMITH_ORE		5
#define UPGRADE_NECPYRAMID_GOLD		1500
#define UPGRADE_NECPYRAMID_ORE		5
#define UPGRADE_ELVESRANGE_GOLD		1500
#define UPGRADE_ELVESRANGE_WOOD		5
#define UPGRADE_FOUNDRY_GOLD		1500
#define UPGRADE_FOUNDRY_MERCURY		5
// upgrade level 4
#define UPGRADE_ARMORY_GOLD		2000
#define UPGRADE_ARMORY_WOOD		5
#define UPGRADE_ARMORY_ORE		5
#define UPGRADE_ADOBE_GOLD		3000
#define UPGRADE_ADOBE_WOOD		5
#define UPGRADE_ADOBE_ORE		5
#define UPGRADE_MANSION_GOLD		4000
#define UPGRADE_MANSION_WOOD		5
#define UPGRADE_MANSION_CRYSTAL		10
#define UPGRADE_MANSION_GEMS		10
#define UPGRADE_STONEHENGE_GOLD		1500
#define UPGRADE_STONEHENGE_MERCURY	5
#define UPGRADE_MAZE_GOLD		2000
#define UPGRADE_MAZE_GEMS		5
// upgrade level 5
#define UPGRADE_ARENA_GOLD		3000
#define UPGRADE_ARENA_WOOD		10
#define UPGRADE_BRIDGE_GOLD		2000
#define UPGRADE_BRIDGE_ORE		10
#define UPGRADE_MAUSOLEUM_GOLD		3000
#define UPGRADE_MAUSOLEUM_ORE		5
#define UPGRADE_MAUSOLEUM_CRYSTAL	5
#define UPGRADE_IVORYTOWER_GOLD		4000
#define UPGRADE_IVORYTOWER_WOOD		5
#define UPGRADE_IVORYTOWER_ORE		5
// upgrade level 6
#define UPGRADE_CATHEDRAL_GOLD		5000
#define UPGRADE_CATHEDRAL_WOOD		10
#define UPGRADE_CATHEDRAL_CRYSTAL	10
#define UPGRADE_CLOUDCASTLE_GOLD	12500
#define UPGRADE_CLOUDCASTLE_ORE		5
#define UPGRADE_CLOUDCASTLE_WOOD	5
#define UPGRADE_CLOUDCASTLE_GEMS	20
#define UPGRADE_REDDRAGON_GOLD		5000
#define UPGRADE_REDDRAGON_ORE		5
#define UPGRADE_REDDRAGON_SULFUR	10
#define UPGRADE_BLACKDRAGON_GOLD	5000
#define UPGRADE_BLACKDRAGON_ORE		5
#define UPGRADE_BLACKDRAGON_SULFUR	10

#define BUY_SPELL_BOOK_GOLD		500

#define BUY_BOAT_GOLD			1000
#define BUY_BOAT_WOOD			10

typedef Resource::funds_t payment_t;

namespace PaymentConditions
{
    class BuyBuilding : public payment_t
    {
    public:
	BuyBuilding(const u8 race, const u32 build);
    };

    class BuyMonster : public payment_t
    {
    public:
	BuyMonster(u8 monster);
    };
    
    class UpgradeMonster : public payment_t
    {
    public:
	UpgradeMonster(u8 monster);
    };
};

#endif
