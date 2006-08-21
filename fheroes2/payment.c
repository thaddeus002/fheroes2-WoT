/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include "SDL.h"

#include "gamedefs.h"
#include "debug.h"
#include "config.h"
#include "payment.h"

static S_PAYMENT paymentObject;

const S_PAYMENT * PaymentConditionsBuilding(E_RACE race, E_BUILDINGCASTLE build){
 
    paymentObject.gold = 0;
    paymentObject. wood = 0;
    paymentObject.ore = 0;
    paymentObject.mercury = 0;
    paymentObject.crystal = 0;
    paymentObject.sulfur = 0;
    paymentObject.gems = 0;

    switch(build){
    
	case BUILD_CASTLE:
	    paymentObject.gold = BUILD_CASTLE_GOLD;
	    paymentObject.wood = BUILD_CASTLE_WOOD;
	    paymentObject.ore = BUILD_CASTLE_ORE;
	    break;

	case BUILD_THIEVESGUILD:
	    paymentObject.gold = BUILD_THIEVESGUILD_GOLD;
	    paymentObject.wood = BUILD_THIEVESGUILD_WOOD;
	    break;

	case BUILD_TAVERN:
	    paymentObject.gold = BUILD_TAVERN_GOLD;
	    paymentObject.wood = BUILD_TAVERN_WOOD;
	    break;

	case BUILD_SHIPYARD:
	    paymentObject.gold = BUILD_SHIPYARD_GOLD;
	    paymentObject.wood = BUILD_SHIPYARD_WOOD;
	    break;

	case BUILD_WELL:
	    paymentObject.gold = BUILD_WELL_GOLD;
	    break;

	case BUILD_CAPTAIN:
	    paymentObject.gold = BUILD_CAPTAIN_GOLD;
	    break;

	case BUILD_STATUE:
	    paymentObject.gold = BUILD_STATUE_GOLD;
	    paymentObject.ore = BUILD_STATUE_ORE;
	    break;

	case BUILD_LEFTTURRET:
	    paymentObject.gold = BUILD_LEFTTURRET_GOLD;
	    paymentObject.ore = BUILD_LEFTTURRET_ORE;
	    break;

	case BUILD_RIGHTTURRET:
	    paymentObject.gold = BUILD_RIGHTTURRET_GOLD;
	    paymentObject.ore = BUILD_RIGHTTURRET_ORE;
	    break;

	case BUILD_MARKETPLACE:
	    paymentObject.gold = BUILD_MARKETPLACE_GOLD;
	    paymentObject.wood = BUILD_MARKETPLACE_WOOD;
	    break;

	case BUILD_WEL2:
	    paymentObject.gold = BUILD_WEL2_GOLD;
	    break;

	case BUILD_MOAT:
	    paymentObject.gold = BUILD_MOAT_GOLD;
	    break;

	case BUILD_BOAT:
	    paymentObject.gold = BUILD_BOAT_GOLD;
	    paymentObject.wood = BUILD_BOAT_WOOD;
	    break;

	case BUILD_MAGEGUILD1:
	    paymentObject.gold = BUILD_MAGEGUILD1_GOLD;
	    paymentObject.ore = BUILD_MAGEGUILD1_ORE;
	    paymentObject.wood = BUILD_MAGEGUILD1_WOOD;
	    break;

	case BUILD_MAGEGUILD2:
	    paymentObject.gold = BUILD_MAGEGUILD2_GOLD;
	    paymentObject.ore = BUILD_MAGEGUILD2_ORE;
	    paymentObject.wood = BUILD_MAGEGUILD2_WOOD;
	    paymentObject.crystal = BUILD_MAGEGUILD2_CRYSTAL;
	    paymentObject.gems = BUILD_MAGEGUILD2_GEMS;
	    paymentObject.mercury = BUILD_MAGEGUILD2_MERCURY;
	    paymentObject.sulfur = BUILD_MAGEGUILD2_SULFUR;
	    break;

	case BUILD_MAGEGUILD3:
	    paymentObject.gold = BUILD_MAGEGUILD3_GOLD;
	    paymentObject.ore = BUILD_MAGEGUILD3_ORE;
	    paymentObject.wood = BUILD_MAGEGUILD3_WOOD;
	    paymentObject.crystal = BUILD_MAGEGUILD3_CRYSTAL;
	    paymentObject.gems = BUILD_MAGEGUILD3_GEMS;
	    paymentObject.mercury = BUILD_MAGEGUILD3_MERCURY;
	    paymentObject.sulfur = BUILD_MAGEGUILD3_SULFUR;
	    break;

	case BUILD_MAGEGUILD4:
	    paymentObject.gold = BUILD_MAGEGUILD4_GOLD;
	    paymentObject.ore = BUILD_MAGEGUILD4_ORE;
	    paymentObject.wood = BUILD_MAGEGUILD4_WOOD;
	    paymentObject.crystal = BUILD_MAGEGUILD4_CRYSTAL;
	    paymentObject.gems = BUILD_MAGEGUILD4_GEMS;
	    paymentObject.mercury = BUILD_MAGEGUILD4_MERCURY;
	    paymentObject.sulfur = BUILD_MAGEGUILD4_SULFUR;
	    break;

	case BUILD_MAGEGUILD5:
	    paymentObject.gold = BUILD_MAGEGUILD5_GOLD;
	    paymentObject.ore = BUILD_MAGEGUILD5_ORE;
	    paymentObject.wood = BUILD_MAGEGUILD5_WOOD;
	    paymentObject.crystal = BUILD_MAGEGUILD5_CRYSTAL;
	    paymentObject.gems = BUILD_MAGEGUILD5_GEMS;
	    paymentObject.mercury = BUILD_MAGEGUILD5_MERCURY;
	    paymentObject.sulfur = BUILD_MAGEGUILD5_SULFUR;
	    break;

	case BUILD_SPEC:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_FORTIFICATION_GOLD;
		    paymentObject.ore = BUILD_FORTIFICATION_ORE;
		    paymentObject.wood = BUILD_FORTIFICATION_WOOD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_COLISEUM_GOLD;
		    paymentObject.wood = BUILD_COLISEUM_WOOD;
		    paymentObject.ore = BUILD_COLISEUM_ORE;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_RAINBOW_GOLD;
		    paymentObject.crystal = BUILD_RAINBOW_CRYSTAL;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_STORM_GOLD;
		    paymentObject.mercury = BUILD_STORM_MERCURY;
		    paymentObject.sulfur = BUILD_STORM_SULFUR;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_DUNGEON_GOLD;
		    paymentObject.ore = BUILD_DUNGEON_ORE;
		    paymentObject.wood = BUILD_DUNGEON_WOOD;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_LIBRARY_GOLD;
		    paymentObject.wood = BUILD_LIBRARY_WOOD;
		    paymentObject.ore = BUILD_LIBRARY_ORE;
	    	    paymentObject.crystal = BUILD_LIBRARY_CRYSTAL;
		    paymentObject.gems = BUILD_LIBRARY_GEMS;
		    paymentObject.mercury = BUILD_LIBRARY_MERCURY;
		    paymentObject.sulfur = BUILD_LIBRARY_SULFUR;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsBuilding: return ZERO\n");
		    break;
	    }
	    break;
    }
    
    return &paymentObject;
}

const S_PAYMENT * PaymentConditionsDwelling(E_RACE race, E_DWELLINGCASTLE dwelling){

    paymentObject.gold = 0;
    paymentObject. wood = 0;
    paymentObject.ore = 0;
    paymentObject.mercury = 0;
    paymentObject.crystal = 0;
    paymentObject.sulfur = 0;
    paymentObject.gems = 0;

    switch(dwelling){

	case DWELLING_MONSTER1:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_THATCHEDHUT_GOLD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_HUT_GOLD;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_TREEHOUSE_GOLD;
		    paymentObject.wood = BUILD_TREEHOUSE_WOOD;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_EXCAVATION_GOLD;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_CAVE_GOLD;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_HABITAT_GOLD;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_MONSTER2:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_ARCHERYRANGE_GOLD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_STICKHUT_GOLD;
		    paymentObject.wood = BUILD_STICKHUT_WOOD;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_COTTAGE_GOLD;
		    paymentObject.wood = BUILD_COTTAGE_WOOD;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_GRAVEYARD_GOLD;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_CRYPT_GOLD;
		    paymentObject.ore = BUILD_CRYPT_ORE;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_PEN_GOLD;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_MONSTER3:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_BLACKSMITH_GOLD;
		    paymentObject.ore = BUILD_BLACKSMITH_ORE;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_DEN_GOLD;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_ELVESRANGE_GOLD;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_NECPYRAMID_GOLD;
		    paymentObject.ore = BUILD_NECPYRAMID_ORE;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_NEST_GOLD;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_FOUNDRY_GOLD;
		    paymentObject.wood = BUILD_FOUNDRY_WOOD;
		    paymentObject.ore = BUILD_FOUNDRY_ORE;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_MONSTER4:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_ARMORY_GOLD;
		    paymentObject.wood = BUILD_ARMORY_WOOD;
		    paymentObject.ore = BUILD_ARMORY_ORE;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_ADOBE_GOLD;
		    paymentObject.wood = BUILD_ADOBE_WOOD;
		    paymentObject.ore = BUILD_ADOBE_ORE;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_STONEHENGE_GOLD;
		    paymentObject.ore = BUILD_STONEHENGE_ORE;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_MANSION_GOLD;
		    paymentObject.wood = BUILD_MANSION_WOOD;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_MAZE_GOLD;
		    paymentObject.gems = BUILD_MAZE_GEMS;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_CLIFFNEST_GOLD;
		    paymentObject.wood = BUILD_CLIFFNEST_WOOD;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_MONSTER5:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_ARENA_GOLD;
		    paymentObject.wood = BUILD_ARENA_WOOD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_BRIDGE_GOLD;
		    paymentObject.ore = BUILD_BRIDGE_ORE;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_FENCED_GOLD;
		    paymentObject.wood = BUILD_FENCED_WOOD;
		    paymentObject.gems = BUILD_FENCED_GEMS;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_MAUSOLEUM_GOLD;
		    paymentObject.wood = BUILD_MAUSOLEUM_WOOD;
		    paymentObject.sulfur = BUILD_MAUSOLEUM_SULFUR;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_SWAMP_GOLD;
		    paymentObject.sulfur = BUILD_SWAMP_SULFUR;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_IVORYTOWER_GOLD;
		    paymentObject.wood = BUILD_IVORYTOWER_WOOD;
		    paymentObject.ore = BUILD_IVORYTOWER_ORE;
		    paymentObject.crystal = BUILD_IVORYTOWER_CRYSTAL;
		    paymentObject.mercury = BUILD_IVORYTOWER_MERCURY;
		    paymentObject.sulfur = BUILD_IVORYTOWER_SULFUR;
		    paymentObject.gems = BUILD_IVORYTOWER_GEMS;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_MONSTER6:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = BUILD_CATHEDRAL_GOLD;
		    paymentObject.wood = BUILD_CATHEDRAL_WOOD;
		    paymentObject.crystal = BUILD_CATHEDRAL_CRYSTAL;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = BUILD_PYRAMID_GOLD;
		    paymentObject.ore = BUILD_PYRAMID_ORE;
		    paymentObject.crystal = BUILD_PYRAMID_CRYSTAL;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = BUILD_REDTOWER_GOLD;
		    paymentObject.ore = BUILD_REDTOWER_ORE;
		    paymentObject.mercury = BUILD_REDTOWER_MERCURY;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = BUILD_LABORATORY_GOLD;
		    paymentObject.wood = BUILD_LABORATORY_WOOD;
		    paymentObject.ore = BUILD_LABORATORY_ORE;
		    paymentObject.crystal = BUILD_LABORATORY_CRYSTAL;
		    paymentObject.sulfur = BUILD_LABORATORY_SULFUR;
		    paymentObject.mercury = BUILD_LABORATORY_MERCURY;
		    paymentObject.gems = BUILD_LABORATORY_GEMS;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = BUILD_GREENDRAGON_GOLD;
		    paymentObject.ore = BUILD_GREENDRAGON_ORE;
		    paymentObject.sulfur = BUILD_GREENDRAGON_SULFUR;
		    break;
		
		case WIZARD:
		    paymentObject.gold = BUILD_CLOUDCASTLE_GOLD;
		    paymentObject.ore = BUILD_CLOUDCASTLE_ORE;
		    paymentObject.wood = BUILD_CLOUDCASTLE_WOOD;
		    paymentObject.gems = BUILD_CLOUDCASTLE_GEMS;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE2:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = UPGRADE_ARCHERYRANGE_GOLD;
		    paymentObject.wood = UPGRADE_ARCHERYRANGE_WOOD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = UPGRADE_STICKHUT_GOLD;
		    paymentObject.wood = UPGRADE_STICKHUT_WOOD;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = UPGRADE_COTTAGE_GOLD;
		    paymentObject.wood = UPGRADE_COTTAGE_WOOD;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = UPGRADE_GRAVEYARD_GOLD;
		    break;
		
		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: upgrade return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE3:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = UPGRADE_BLACKSMITH_GOLD;
		    paymentObject.ore = UPGRADE_BLACKSMITH_ORE;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = UPGRADE_ELVESRANGE_GOLD;
		    paymentObject.wood = UPGRADE_ELVESRANGE_WOOD;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = UPGRADE_NECPYRAMID_GOLD;
		    paymentObject.ore = UPGRADE_NECPYRAMID_ORE;
		    break;
		
		case WIZARD:
		    paymentObject.gold = UPGRADE_FOUNDRY_GOLD;
		    paymentObject.mercury = UPGRADE_FOUNDRY_MERCURY;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: upgrade return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE4:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = UPGRADE_ARMORY_GOLD;
		    paymentObject.wood = UPGRADE_ARMORY_WOOD;
		    paymentObject.ore = UPGRADE_ARMORY_ORE;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = UPGRADE_ADOBE_GOLD;
		    paymentObject.wood = UPGRADE_ADOBE_WOOD;
		    paymentObject.ore = UPGRADE_ADOBE_ORE;
		    break;
		
		case SORCERESS:
		    paymentObject.gold = UPGRADE_STONEHENGE_GOLD;
		    paymentObject.mercury = UPGRADE_STONEHENGE_MERCURY;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = UPGRADE_MANSION_GOLD;
		    paymentObject.wood = UPGRADE_MANSION_WOOD;
		    paymentObject.crystal = UPGRADE_MANSION_CRYSTAL;
		    paymentObject.gems = UPGRADE_MANSION_GEMS;
		    break;
		
		case WIZARD:
		    paymentObject.gold = UPGRADE_MAZE_GOLD;
		    paymentObject.gems = UPGRADE_MAZE_GEMS;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE5:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = UPGRADE_ARENA_GOLD;
		    paymentObject.wood = UPGRADE_ARENA_WOOD;
		    break;
		
		case BARBARIAN:
		    paymentObject.gold = UPGRADE_BRIDGE_GOLD;
		    paymentObject.ore = UPGRADE_BRIDGE_ORE;
		    break;
		
		case NECROMANCER:
		    paymentObject.gold = UPGRADE_MAUSOLEUM_GOLD;
		    paymentObject.ore = UPGRADE_MAUSOLEUM_ORE;
		    paymentObject.crystal = UPGRADE_MAUSOLEUM_CRYSTAL;
		    break;
		
		case WIZARD:
		    paymentObject.gold = UPGRADE_IVORYTOWER_GOLD;
		    paymentObject.wood = UPGRADE_IVORYTOWER_WOOD;
		    paymentObject.ore = UPGRADE_IVORYTOWER_ORE;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE6:
	    switch(race){
	    
		case KNIGHT:
		    paymentObject.gold = UPGRADE_CATHEDRAL_GOLD;
		    paymentObject.wood = UPGRADE_CATHEDRAL_WOOD;
		    paymentObject.crystal = UPGRADE_CATHEDRAL_CRYSTAL;
		    break;
		
		case WARLOCK:
		    paymentObject.gold = UPGRADE_REDDRAGON_GOLD;
		    paymentObject.ore = UPGRADE_REDDRAGON_ORE;
		    paymentObject.sulfur = UPGRADE_REDDRAGON_SULFUR;
		    break;
		
		case WIZARD:
		    paymentObject.gold = UPGRADE_CLOUDCASTLE_GOLD;
		    paymentObject.ore = UPGRADE_CLOUDCASTLE_ORE;
		    paymentObject.wood = UPGRADE_CLOUDCASTLE_WOOD;
		    paymentObject.gems = UPGRADE_CLOUDCASTLE_GEMS;
		    break;

		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;

	case DWELLING_UPGRADE7:
	    switch(race){
	    
		case WARLOCK:
		    paymentObject.gold = UPGRADE_BLACKDRAGON_GOLD;
		    paymentObject.ore = UPGRADE_BLACKDRAGON_ORE;
		    paymentObject.sulfur = UPGRADE_BLACKDRAGON_SULFUR;
		    break;
		
		default:
		    if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsDwelling: return ZERO\n");
		    break;
	    }
	    break;
    }

    return &paymentObject;
}

const S_PAYMENT * PaymentConditionsMonster(E_MONSTER monster){

    paymentObject.gold = 0;
    paymentObject. wood = 0;
    paymentObject.ore = 0;
    paymentObject.mercury = 0;
    paymentObject.crystal = 0;
    paymentObject.sulfur = 0;
    paymentObject.gems = 0;

    switch(monster){

		case PEASANT:
			paymentObject.gold = BUY_PEASANT_GOLD;
			break;

		case ARCHER:
			paymentObject.gold = BUY_ARCHER_GOLD;
			break;

		case RANGER:
			paymentObject.gold = BUY_RANGER_GOLD;
			break;

		case PIKEMAN:
			paymentObject.gold = BUY_PIKEMAN_GOLD;
			break;

		case VETERAN_PIKEMAN:
			paymentObject.gold = BUY_VETERAN_PIKEMAN_GOLD;
			break;

		case SWORDSMAN:
			paymentObject.gold = BUY_SWORDSMAN_GOLD;
			break;

		case MASTER_SWORDSMAN:
			paymentObject.gold = BUY_MASTER_SWORDSMAN_GOLD;
			break;

		case CAVALIRY:
			paymentObject.gold = BUY_CAVALIRY_GOLD;
			break;

		case CHAMPION:
			paymentObject.gold = BUY_CHAMPION_GOLD;
			break;

		case PALADIN:
			paymentObject.gold = BUY_PALADIN_GOLD;
			break;

		case CRUSADER:
			paymentObject.gold = BUY_CRUSADER_GOLD;
			break;

		case GOBLIN:
			paymentObject.gold = BUY_GOBLIN_GOLD;
			break;

		case ORC:
			paymentObject.gold = BUY_ORC_GOLD;
			break;

		case CHIEF_ORC:
			paymentObject.gold = BUY_CHIEF_ORC_GOLD;
			break;

		case WOLF:
			paymentObject.gold = BUY_WOLF_GOLD;
			break;

		case OGRE:
			paymentObject.gold = BUY_OGRE_GOLD;
			break;

		case LORD_OGRE:
			paymentObject.gold = BUY_LORD_OGRE_GOLD;
			break;

		case TROLL:
			paymentObject.gold = BUY_TROLL_GOLD;
			break;

		case WAR_TROLL:
			paymentObject.gold = BUY_WAR_TROLL_GOLD;
			break;

		case CYCLOPS:
			paymentObject.gold = BUY_CYCLOPS_GOLD;
			paymentObject.crystal = BUY_CYCLOPS_CRYSTAL;
			break;

		case SPRITE:
			paymentObject.gold = BUY_SPRITE_GOLD;
			break;

		case DWARF:
			paymentObject.gold = BUY_DWARF_GOLD;
			break;

		case BATTLE_DWARF:
			paymentObject.gold = BUY_BATTLE_DWARF_GOLD;
			break;

		case ELF:
			paymentObject.gold = BUY_ELF_GOLD;
			break;

		case GRAND_ELF:
			paymentObject.gold = BUY_GRAND_ELF_GOLD;
			break;

		case DRUID:
			paymentObject.gold = BUY_DRUID_GOLD;
			break;

		case GREATER_DRUID:
			paymentObject.gold = BUY_GREATER_DRUID_GOLD;
			break;

		case UNICORN:
			paymentObject.gold = BUY_UNICORN_GOLD;
			break;

		case PHOENIX:
			paymentObject.gold = BUY_PHOENIX_GOLD;
			paymentObject.mercury = BUY_PHOENIX_MERCURY;
			break;

		case CENTAUR:
			paymentObject.gold = BUY_CENTAUR_GOLD;
			break;

		case GARGOYLE:
			paymentObject.gold = BUY_GARGOYLE_GOLD;
			break;

		case GRIFFIN:
			paymentObject.gold = BUY_GRIFFIN_GOLD;
			break;

		case MINOTAUR:
			paymentObject.gold = BUY_MINOTAUR_GOLD;
			break;

		case KNIGHT_MINOTAUR:
			paymentObject.gold = BUY_KNIGHT_MINOTAUR_GOLD;
			break;

		case HIDRA:
			paymentObject.gold = BUY_HIDRA_GOLD;
			break;

		case GREEN_DRAGON:
			paymentObject.gold = BUY_GREEN_DRAGON_GOLD;
			paymentObject.sulfur = BUY_GREENDRAGON_SULFUR;
			break;

		case RED_DRAGON:
			paymentObject.gold = BUY_RED_DRAGON_GOLD;
			paymentObject.sulfur = BUY_REDDRAGON_SULFUR;
			break;

		case BLACK_DRAGON:
			paymentObject.gold = BUY_BLACK_DRAGON_GOLD;
			paymentObject.sulfur = BUY_BLACKDRAGON_SULFUR;
			break;

		case HALFLING:
			paymentObject.gold = BUY_HALFLING_GOLD;
			break;

		case BOAR:
			paymentObject.gold = BUY_BOAR_GOLD;
			break;

		case IRON_GOLEM:
			paymentObject.gold = BUY_IRON_GOLEM_GOLD;
			break;

		case STEEL_GOLEM:
			paymentObject.gold = BUY_STEEL_GOLEM_GOLD;
			break;

		case ROC:
			paymentObject.gold = BUY_ROC_GOLD;
			break;

		case MAGE:
			paymentObject.gold = BUY_MAGE_GOLD;
			break;

		case ARCHMAGE:
			paymentObject.gold = BUY_ARCHMAGE_GOLD;
			break;

		case GIANT:
			paymentObject.gold = BUY_GIANT_GOLD;
			paymentObject.gems = BUY_GIANT_GEMS;
			break;

		case TITAN:
			paymentObject.gold = BUY_TITAN_GOLD;
			paymentObject.gems = BUY_TITAN_GEMS;
			break;

		case SKELETON:
			paymentObject.gold = BUY_SKELETON_GOLD;
			break;

		case ZOMBIE:
			paymentObject.gold = BUY_ZOMBIE_GOLD;
			break;

		case MUTANT_ZOMBIE:
			paymentObject.gold = BUY_MUTANT_ZOMBIE_GOLD;
			break;

		case MUMMY:
			paymentObject.gold = BUY_MUMMY_GOLD;
			break;

		case ROYAL_MUMMY:
			paymentObject.gold = BUY_ROYAL_MUMMY_GOLD;
			break;

		case VAMPIRE:
			paymentObject.gold = BUY_VAMPIRE_GOLD;
			break;

		case LORD_VAMPIRE:
			paymentObject.gold = BUY_LORD_VAMPIRE_GOLD;
			break;

		case LICH:
			paymentObject.gold = BUY_LICH_GOLD;
			break;

		case POWER_LICH:
			paymentObject.gold = BUY_POWER_LICH_GOLD;
			break;

		case BONE_DRAGON:
			paymentObject.gold = BUY_BONE_DRAGON_GOLD;
			break;

		case ROGUE:
			paymentObject.gold = BUY_ROGUE_GOLD;
			break;

		case NOMAD:
			paymentObject.gold = BUY_NOMAD_GOLD;
			break;

		case GHOST:
			paymentObject.gold = BUY_GHOST_GOLD;
			break;

		case GENIE:
			paymentObject.gold = BUY_GENIE_GOLD;
			paymentObject.gems = BUY_GENIE_GEMS;
			break;

		case MEDUSA:
			paymentObject.gold = BUY_MEDUSA_GOLD;
			break;

		case EARTH_ELEMENT:
			paymentObject.gold = BUY_EARTH_ELEMENT_GOLD;
			break;

		case AIR_ELEMENT:
			paymentObject.gold = BUY_AIR_ELEMENT_GOLD;
			break;

		case FIRE_ELEMENT:
			paymentObject.gold = BUY_FIRE_ELEMENT_GOLD;
			break;

		case WATER_ELEMENT:
			paymentObject.gold = BUY_WATER_ELEMENT_GOLD;
			break;

		default:
			if(GetIntValue(DEBUG)) fprintf(stderr, "PaymentConditionsMonster: return ZERO\n");
			break;
	}

    return &paymentObject;
}

const S_PAYMENT * GetMultiPayment(const S_PAYMENT *payment, Uint16 count){

    if(payment->gold) paymentObject.gold = payment->gold * count;
    if(payment->wood) paymentObject.wood = payment->wood * count;
    if(payment->ore) paymentObject.ore = payment->ore * count;
    if(payment->mercury) paymentObject.mercury = payment->mercury * count;
    if(payment->crystal) paymentObject.crystal = payment->crystal * count;
    if(payment->sulfur) paymentObject.sulfur = payment->sulfur * count;
    if(payment->gems) paymentObject.gems = payment->gems * count;

    return &paymentObject;
}
