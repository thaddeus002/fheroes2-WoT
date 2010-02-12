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

#include "castle.h"
#include "settings.h"
#include "race.h"
#include "monster.h"
#include "payment.h"

PaymentConditions::BuyMonster::BuyMonster(u8 monster)
{
    Monster::GetCost(monster, *this);
}

PaymentConditions::UpgradeMonster::UpgradeMonster(u8 monster)
{
    Monster::GetUpgradeCost(monster, *this);
}

PaymentConditions::BuyBuilding::BuyBuilding(const u8 race, const u32 build)
{
    gold	= 0;
    wood	= 0;
    ore		= 0;
    mercury	= 0;
    crystal	= 0;
    sulfur	= 0;
    gems 	= 0;
    
    switch(build)
    {
        case Castle::BUILD_CASTLE:
            gold = BUILD_CASTLE_GOLD;
            wood = BUILD_CASTLE_WOOD;
            ore = BUILD_CASTLE_ORE;
            break;

        case Castle::BUILD_THIEVESGUILD:
            gold = BUILD_THIEVESGUILD_GOLD;
            wood = BUILD_THIEVESGUILD_WOOD;
            break;

        case Castle::BUILD_TAVERN:
            if(Race::NECR == race && Settings::Get().PriceLoyaltyVersion())
            {
		gold = BUILD_SHRINE_GOLD;
		wood = BUILD_SHRINE_WOOD;
		crystal = BUILD_SHRINE_CRYSTAL;
            }
            else
            if(Race::NECR != race)
            {
        	gold = BUILD_TAVERN_GOLD;
        	wood = BUILD_TAVERN_WOOD;
            }
            break;

        case Castle::BUILD_SHIPYARD:
            gold = BUILD_SHIPYARD_GOLD;
            wood = BUILD_SHIPYARD_WOOD;
            break;

        case Castle::BUILD_WELL:
            gold = BUILD_WELL_GOLD;
            break;

        case Castle::BUILD_CAPTAIN:
    	    gold = BUILD_CAPTAIN_GOLD;
	    break;

        case Castle::BUILD_STATUE:
            gold = BUILD_STATUE_GOLD;
            ore = BUILD_STATUE_ORE;
            break;

        case Castle::BUILD_LEFTTURRET:
            gold = BUILD_LEFTTURRET_GOLD;
            ore = BUILD_LEFTTURRET_ORE;
            break;

        case Castle::BUILD_RIGHTTURRET:
            gold = BUILD_RIGHTTURRET_GOLD;
            ore = BUILD_RIGHTTURRET_ORE;
            break;

        case Castle::BUILD_MARKETPLACE:
            gold = BUILD_MARKETPLACE_GOLD;
            wood = BUILD_MARKETPLACE_WOOD;
            break;

        case Castle::BUILD_WEL2:
            gold = BUILD_WEL2_GOLD;
            break;

        case Castle::BUILD_MOAT:
            gold = BUILD_MOAT_GOLD;
            break;

        case Castle::BUILD_MAGEGUILD1:
            gold = BUILD_MAGEGUILD1_GOLD;
            ore = BUILD_MAGEGUILD1_ORE;
            wood = BUILD_MAGEGUILD1_WOOD;
            break;

        case Castle::BUILD_MAGEGUILD2:
            gold = BUILD_MAGEGUILD2_GOLD;
            ore = BUILD_MAGEGUILD2_ORE;
            wood = BUILD_MAGEGUILD2_WOOD;
            crystal = BUILD_MAGEGUILD2_CRYSTAL;
            gems = BUILD_MAGEGUILD2_GEMS;
            mercury = BUILD_MAGEGUILD2_MERCURY;
            sulfur = BUILD_MAGEGUILD2_SULFUR;
            break;
			
        case Castle::BUILD_MAGEGUILD3:
            gold = BUILD_MAGEGUILD3_GOLD;
            ore = BUILD_MAGEGUILD3_ORE;
            wood = BUILD_MAGEGUILD3_WOOD;
            crystal = BUILD_MAGEGUILD3_CRYSTAL;
            gems = BUILD_MAGEGUILD3_GEMS;
            mercury = BUILD_MAGEGUILD3_MERCURY;
            sulfur = BUILD_MAGEGUILD3_SULFUR;
            break;

        case Castle::BUILD_MAGEGUILD4:
            gold = BUILD_MAGEGUILD4_GOLD;
            ore = BUILD_MAGEGUILD4_ORE;
            wood = BUILD_MAGEGUILD4_WOOD;
            crystal = BUILD_MAGEGUILD4_CRYSTAL;
            gems = BUILD_MAGEGUILD4_GEMS;
            mercury = BUILD_MAGEGUILD4_MERCURY;
            sulfur = BUILD_MAGEGUILD4_SULFUR;
            break;

        case Castle::BUILD_MAGEGUILD5:
            gold = BUILD_MAGEGUILD5_GOLD;
            ore = BUILD_MAGEGUILD5_ORE;
            wood = BUILD_MAGEGUILD5_WOOD;
            crystal = BUILD_MAGEGUILD5_CRYSTAL;
            gems = BUILD_MAGEGUILD5_GEMS;
            mercury = BUILD_MAGEGUILD5_MERCURY;
            sulfur = BUILD_MAGEGUILD5_SULFUR;
            break;

        case Castle::BUILD_SPEC:
            switch(race)
	    {
                case Race::KNGT:
                    gold = BUILD_FORTIFICATION_GOLD;
                    ore = BUILD_FORTIFICATION_ORE;
                    wood = BUILD_FORTIFICATION_WOOD;
                    break;

                case Race::BARB:
                    gold = BUILD_COLISEUM_GOLD;
                    wood = BUILD_COLISEUM_WOOD;
                    ore = BUILD_COLISEUM_ORE;
                    break;

                case Race::SORC:
                    gold = BUILD_RAINBOW_GOLD;
                    crystal = BUILD_RAINBOW_CRYSTAL;
                    break;
            
                case Race::NECR:
                    gold = BUILD_STORM_GOLD;
                    mercury = BUILD_STORM_MERCURY;
                    sulfur = BUILD_STORM_SULFUR;
                    break;

                case Race::WRLK:
                    gold = BUILD_DUNGEON_GOLD;
                    ore = BUILD_DUNGEON_ORE;
                    wood = BUILD_DUNGEON_WOOD;
                    break;

        	case Race::WZRD:
                    gold = BUILD_LIBRARY_GOLD;
                    wood = BUILD_LIBRARY_WOOD;
                    ore = BUILD_LIBRARY_ORE;
                    crystal = BUILD_LIBRARY_CRYSTAL;
                    gems = BUILD_LIBRARY_GEMS;
                    mercury = BUILD_LIBRARY_MERCURY;
            	    sulfur = BUILD_LIBRARY_SULFUR;
                    break;
		
		default: break;
	    }
	    break;

        case Castle::DWELLING_MONSTER1:
            switch(race)
	    {
                case Race::KNGT:
                    gold = BUILD_THATCHEDHUT_GOLD;
                    break;

                case Race::BARB:
                    gold = BUILD_HUT_GOLD;
                    break;

                case Race::SORC:
                    gold = BUILD_TREEHOUSE_GOLD;
                    wood = BUILD_TREEHOUSE_WOOD;
                    break;

                case Race::NECR:
                    gold = BUILD_EXCAVATION_GOLD;
                    break;

                case Race::WRLK:
                    gold = BUILD_CAVE_GOLD;
                    break;

                case Race::WZRD:
                    gold = BUILD_HABITAT_GOLD;
                    break;

		default: break;
            }
            break;

        case Castle::DWELLING_MONSTER2:
            switch(race)
	    {
                case Race::KNGT:
                    gold = BUILD_ARCHERYRANGE_GOLD;
                    break;

                case Race::BARB:
                    gold = BUILD_STICKHUT_GOLD;
                    wood = BUILD_STICKHUT_WOOD;
                    break;

                case Race::SORC:
                    gold = BUILD_COTTAGE_GOLD;
                    wood = BUILD_COTTAGE_WOOD;
                    break;

                case Race::NECR:
                    gold = BUILD_GRAVEYARD_GOLD;
                    break;

                case Race::WRLK:
                    gold = BUILD_CRYPT_GOLD;
                    ore = BUILD_CRYPT_ORE;
                    break;

                case Race::WZRD:
                    gold = BUILD_PEN_GOLD;
                    break;

                default: break;
            }
            break;

        case Castle::DWELLING_MONSTER3:
            switch(race)
	    {
                case Race::KNGT:
                    gold = BUILD_BLACKSMITH_GOLD;
                    ore = BUILD_BLACKSMITH_ORE;
                    break;

                case Race::BARB:
                    gold = BUILD_DEN_GOLD;
                    break;

                case Race::SORC:
                    gold = BUILD_ELVESRANGE_GOLD;
                    break;

                case Race::NECR:
                    gold = BUILD_NECPYRAMID_GOLD;
                    ore = BUILD_NECPYRAMID_ORE;
                    break;

                case Race::WRLK:
                    gold = BUILD_NEST_GOLD;
                    break;

                case Race::WZRD:
		    gold = BUILD_FOUNDRY_GOLD;
		    wood = BUILD_FOUNDRY_WOOD;
		    ore = BUILD_FOUNDRY_ORE;
		    break;

                default: break;
            }
            break;

        case Castle::DWELLING_MONSTER4:
            switch(race)
	    {
                case Race::KNGT:
                    gold = BUILD_ARMORY_GOLD;
                    wood = BUILD_ARMORY_WOOD;
                    ore = BUILD_ARMORY_ORE;
                    break;

                case Race::BARB:
                    gold = BUILD_ADOBE_GOLD;
                    wood = BUILD_ADOBE_WOOD;
                    ore = BUILD_ADOBE_ORE;
                    break;

                case Race::SORC:
                    gold = BUILD_STONEHENGE_GOLD;
                    ore = BUILD_STONEHENGE_ORE;
                    break;

                case Race::NECR:
                    gold = BUILD_MANSION_GOLD;
                    wood = BUILD_MANSION_WOOD;
                    break;

                case Race::WRLK:
                    gold = BUILD_MAZE_GOLD;
                    gems = BUILD_MAZE_GEMS;
                    break;

                case Race::WZRD:
                    gold = BUILD_CLIFFNEST_GOLD;
                    wood = BUILD_CLIFFNEST_WOOD;
                    break;

                default: break;
	    }
	    break;

	case Castle::DWELLING_MONSTER5:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = BUILD_ARENA_GOLD;
		    wood = BUILD_ARENA_WOOD;
		    break;
		
		case Race::BARB:
		    gold = BUILD_BRIDGE_GOLD;
		    ore = BUILD_BRIDGE_ORE;
		    break;
		
		case Race::SORC:
		    gold = BUILD_FENCED_GOLD;
		    wood = BUILD_FENCED_WOOD;
		    gems = BUILD_FENCED_GEMS;
		    break;
		
		case Race::NECR:
		    gold = BUILD_MAUSOLEUM_GOLD;
		    wood = BUILD_MAUSOLEUM_WOOD;
		    sulfur = BUILD_MAUSOLEUM_SULFUR;
		    break;
		
		case Race::WRLK:
		    gold = BUILD_SWAMP_GOLD;
		    sulfur = BUILD_SWAMP_SULFUR;
		    break;
		
		case Race::WZRD:
		    gold = BUILD_IVORYTOWER_GOLD;
		    wood = BUILD_IVORYTOWER_WOOD;
		    ore = BUILD_IVORYTOWER_ORE;
		    crystal = BUILD_IVORYTOWER_CRYSTAL;
		    mercury = BUILD_IVORYTOWER_MERCURY;
		    sulfur = BUILD_IVORYTOWER_SULFUR;
		    gems = BUILD_IVORYTOWER_GEMS;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_MONSTER6:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = BUILD_CATHEDRAL_GOLD;
		    wood = BUILD_CATHEDRAL_WOOD;
		    crystal = BUILD_CATHEDRAL_CRYSTAL;
		    break;
		
		case Race::BARB:
		    gold = BUILD_PYRAMID_GOLD;
		    ore = BUILD_PYRAMID_ORE;
		    crystal = BUILD_PYRAMID_CRYSTAL;
		    break;
		
		case Race::SORC:
		    gold = BUILD_REDTOWER_GOLD;
		    ore = BUILD_REDTOWER_ORE;
		    mercury = BUILD_REDTOWER_MERCURY;
		    break;
		
		case Race::NECR:
		    gold = BUILD_LABORATORY_GOLD;
		    wood = BUILD_LABORATORY_WOOD;
		    ore = BUILD_LABORATORY_ORE;
		    crystal = BUILD_LABORATORY_CRYSTAL;
		    sulfur = BUILD_LABORATORY_SULFUR;
		    mercury = BUILD_LABORATORY_MERCURY;
		    gems = BUILD_LABORATORY_GEMS;
		    break;
		
		case Race::WRLK:
		    gold = BUILD_GREENDRAGON_GOLD;
		    ore = BUILD_GREENDRAGON_ORE;
		    sulfur = BUILD_GREENDRAGON_SULFUR;
		    break;
		
		case Race::WZRD:
		    gold = BUILD_CLOUDCASTLE_GOLD;
		    ore = BUILD_CLOUDCASTLE_ORE;
		    wood = BUILD_CLOUDCASTLE_WOOD;
		    gems = BUILD_CLOUDCASTLE_GEMS;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE2:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = UPGRADE_ARCHERYRANGE_GOLD;
		    wood = UPGRADE_ARCHERYRANGE_WOOD;
		    break;
		
		case Race::BARB:
		    gold = UPGRADE_STICKHUT_GOLD;
		    wood = UPGRADE_STICKHUT_WOOD;
		    break;
		
		case Race::SORC:
		    gold = UPGRADE_COTTAGE_GOLD;
		    wood = UPGRADE_COTTAGE_WOOD;
		    break;
		
		case Race::NECR:
		    gold = UPGRADE_GRAVEYARD_GOLD;
		    break;
		
		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE3:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = UPGRADE_BLACKSMITH_GOLD;
		    ore = UPGRADE_BLACKSMITH_ORE;
		    break;
		
		case Race::SORC:
		    gold = UPGRADE_ELVESRANGE_GOLD;
		    wood = UPGRADE_ELVESRANGE_WOOD;
		    break;
		
		case Race::NECR:
		    gold = UPGRADE_NECPYRAMID_GOLD;
		    ore = UPGRADE_NECPYRAMID_ORE;
		    break;
		
		case Race::WZRD:
		    gold = UPGRADE_FOUNDRY_GOLD;
		    mercury = UPGRADE_FOUNDRY_MERCURY;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE4:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = UPGRADE_ARMORY_GOLD;
		    wood = UPGRADE_ARMORY_WOOD;
		    ore = UPGRADE_ARMORY_ORE;
		    break;
		
		case Race::BARB:
		    gold = UPGRADE_ADOBE_GOLD;
		    wood = UPGRADE_ADOBE_WOOD;
		    ore = UPGRADE_ADOBE_ORE;
		    break;
		
		case Race::SORC:
		    gold = UPGRADE_STONEHENGE_GOLD;
		    mercury = UPGRADE_STONEHENGE_MERCURY;
		    break;
		
		case Race::NECR:
		    gold = UPGRADE_MANSION_GOLD;
		    wood = UPGRADE_MANSION_WOOD;
		    crystal = UPGRADE_MANSION_CRYSTAL;
		    gems = UPGRADE_MANSION_GEMS;
		    break;
		
		case Race::WRLK:
		    gold = UPGRADE_MAZE_GOLD;
		    gems = UPGRADE_MAZE_GEMS;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE5:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = UPGRADE_ARENA_GOLD;
		    wood = UPGRADE_ARENA_WOOD;
		    break;
		
		case Race::BARB:
		    gold = UPGRADE_BRIDGE_GOLD;
		    ore = UPGRADE_BRIDGE_ORE;
		    break;
		
		case Race::NECR:
		    gold = UPGRADE_MAUSOLEUM_GOLD;
		    ore = UPGRADE_MAUSOLEUM_ORE;
		    crystal = UPGRADE_MAUSOLEUM_CRYSTAL;
		    break;
		
		case Race::WZRD:
		    gold = UPGRADE_IVORYTOWER_GOLD;
		    wood = UPGRADE_IVORYTOWER_WOOD;
		    ore = UPGRADE_IVORYTOWER_ORE;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE6:
	    switch(race)
	    {
		case Race::KNGT:
		    gold = UPGRADE_CATHEDRAL_GOLD;
		    wood = UPGRADE_CATHEDRAL_WOOD;
		    crystal = UPGRADE_CATHEDRAL_CRYSTAL;
		    break;
		
		case Race::WRLK:
		    gold = UPGRADE_REDDRAGON_GOLD;
		    ore = UPGRADE_REDDRAGON_ORE;
		    sulfur = UPGRADE_REDDRAGON_SULFUR;
		    break;
		
		case Race::WZRD:
		    gold = UPGRADE_CLOUDCASTLE_GOLD;
		    ore = UPGRADE_CLOUDCASTLE_ORE;
		    wood = UPGRADE_CLOUDCASTLE_WOOD;
		    gems = UPGRADE_CLOUDCASTLE_GEMS;
		    break;

		default: break;
	    }
	    break;

	case Castle::DWELLING_UPGRADE7:
	    switch(race)
	    {
		case Race::WRLK:
		    gold = UPGRADE_BLACKDRAGON_GOLD;
		    ore = UPGRADE_BLACKDRAGON_ORE;
		    sulfur = UPGRADE_BLACKDRAGON_SULFUR;
		    break;
		
		default: break;
	    }
	    break;
																						
	default: DEBUG(DBG_GAME , DBG_WARN, "PaymentConditions::BuyBuilding: unknown building."); break;
    }
}
