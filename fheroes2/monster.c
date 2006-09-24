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

#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "config.h"
#include "kingdom.h"
#include "payment.h"
#include "castle.h"
#include "monster.h"

static S_MONSTER *ptrMonster = NULL;
static char monsterPortrait[13];

void	FreeMonster(void){

    if(ptrMonster) free(ptrMonster);

    ptrMonster = NULL;
}

E_MONSTER CheckValidMonster(Uint8 type){

    E_MONSTER a;
                
    for(a = PEASANT; a < MONSTERNONE; ++a)
	if(a == type)
    	    return a;

    if(GetIntValue(DEBUG))
        fprintf(stderr, "CheckValidMonster: unknown monster: 0x%hhX\n", type);

    return PEASANT;
}

S_MONSTER *GetStatMonster(E_MONSTER monster){

    if(ptrMonster) return &ptrMonster[monster];
    
    return NULL;
}

E_ARMYSIZE GetSizeArmy(Uint16 count){

    if(count < SEVERAL) return FEW;

    else if(count < PACK) return SEVERAL;

    else if(count < LOTS) return PACK;

    else if(count < HORDE) return LOTS;

    else if(count < THRONG) return HORDE;

    else if(count < SWARM) return THRONG;

    else if(count < ZOUNDS) return SWARM;

    else if(count < LEGION) return ZOUNDS;
    
    else return LEGION;
}

Uint16 GetRNDSizeLevelMonster(E_LEVELMONSTER level){

    Uint16 res;
    
    switch(level){
    
	case MNS_LEVEL1:
	    res = rand() % (THRONG - LOTS) + LOTS;
	break;

	case MNS_LEVEL2:
	    res = rand() % (HORDE - PACK) + PACK;
	break;

	case MNS_LEVEL3:
	    res = rand() % (LOTS - SEVERAL) + SEVERAL;
	break;

	case MNS_LEVEL4:
	    res = rand() % (PACK - FEW) + FEW;
	break;

	default:
	    res = 1;
	break;
    }

    return res;
}

const char *GetStringSizeArmy(E_ARMYSIZE size){

    const char *string = NULL;

    switch(size){
    
	case FEW:
	    string = "Few";
	    break;

	case SEVERAL:
	    string = "Several";
	    break;

	case PACK:
	    string = "Pack";
	    break;

	case LOTS:
	    string = "Lots";
	    break;

	case HORDE:
	    string = "Horde";
	    break;

	case THRONG:
	    string = "Throng";
	    break;

	case SWARM:
	    string = "Swarm";
	    break;

	case ZOUNDS:
	    string = "Zounds";
	    break;

	case LEGION:
	    string = "Legion";
	    break;
    }
    
    return string;
}


E_RACE	GetRaceMonster(E_MONSTER monster){

    switch(monster){
    
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

	    return NECROMANCER;
	    break;

	case GOBLIN:
	case ORC:
	case CHIEF_ORC:
	case WOLF:
	case OGRE:
	case LORD_OGRE:
	case TROLL:
	case WAR_TROLL:
	case CYCLOPS:

	    return BARBARIAN;
	    break;

	case PEASANT:
	case ARCHER:
	case RANGER:
	case PIKEMAN:
	case VETERAN_PIKEMAN:
	case SWORDSMAN:
	case MASTER_SWORDSMAN:
	case CAVALIRY:
	case CHAMPION:
	case PALADIN:
	case CRUSADER:

	    return KNIGHT;
	    break;

	case SPRITE:
	case DWARF:
	case BATTLE_DWARF:
	case ELF:
	case GRAND_ELF:
	case DRUID:
	case GREATER_DRUID:
	case UNICORN:
	case PHOENIX:

	    return SORCERESS;
	    break;

	case CENTAUR:
	case GARGOYLE:
	case GRIFFIN:
	case MINOTAUR:
	case KNIGHT_MINOTAUR:
	case HIDRA:
	case GREEN_DRAGON:
	case RED_DRAGON:
	case BLACK_DRAGON:

	    return WARLOCK;
	    break;

	case HALFLING:
	case BOAR:
	case IRON_GOLEM:
	case STEEL_GOLEM:
	case ROC:
	case MAGE:
	case ARCHMAGE:
	case GIANT:
	case TITAN:

	    return WIZARD;
	    break;

	default:
	    return BOMG;
	    break;
    }
}

BOOL	TheFlyMonster(E_MONSTER monster){

    switch(monster){
    
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
	case GENIE:

	    return TRUE;
	    break;
	
	default:

	    return FALSE;
	    break;
    }
}

E_LEVELMONSTER GetLevelMonster(E_MONSTER monster){

    switch(monster){
    
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
	    return MNS_LEVEL1;
	    break;
	    
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
	    return MNS_LEVEL2;
	    break;

	case SWORDSMAN:
	case MASTER_SWORDSMAN:
	case CAVALIRY:
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
	    return MNS_LEVEL3;
	    break;

	case PALADIN:
	case CRUSADER:
	case CYCLOPS:
	case UNICORN:
	case PHOENIX:
	case HIDRA:
	case GREEN_DRAGON:
	case RED_DRAGON:
	case BLACK_DRAGON:
	case GIANT:
	case TITAN:
	case POWER_LICH:
	case BONE_DRAGON:
	case GENIE:
	    return MNS_LEVEL3;
	    break;

	default:
	    break;
    }
    
    return MNS_ALL;
}

E_MONSTER GetRNDMonster(E_LEVELMONSTER level){

    E_MONSTER level1[] = { PEASANT, ARCHER, GOBLIN, ORC, SPRITE, CENTAUR, HALFLING, SKELETON, ZOMBIE, ROGUE };
    E_MONSTER level2[] = { RANGER, PIKEMAN, VETERAN_PIKEMAN, CHIEF_ORC, WOLF, DWARF, BATTLE_DWARF, ELF, GRAND_ELF, GARGOYLE, BOAR, IRON_GOLEM, MUTANT_ZOMBIE, MUMMY, NOMAD };
    E_MONSTER level3[] = { SWORDSMAN, MASTER_SWORDSMAN, CAVALIRY, CHAMPION, OGRE, LORD_OGRE, TROLL, WAR_TROLL, DRUID, GREATER_DRUID, GRIFFIN, MINOTAUR, KNIGHT_MINOTAUR, STEEL_GOLEM, ROC, MAGE, ARCHMAGE, ROYAL_MUMMY, VAMPIRE, LORD_VAMPIRE, LICH, GHOST, MEDUSA, EARTH_ELEMENT ,AIR_ELEMENT, FIRE_ELEMENT, WATER_ELEMENT };
    E_MONSTER level4[] = { PALADIN, CRUSADER, CYCLOPS, UNICORN, PHOENIX, HIDRA, GREEN_DRAGON, RED_DRAGON, BLACK_DRAGON, GIANT, TITAN, POWER_LICH, BONE_DRAGON, GENIE };

    switch(level){
    
	case MNS_LEVEL1:
		return level1[rand() % MONSTERCOUNT_LEVEL1];
		break;

	case MNS_LEVEL2:
		return level2[rand() % MONSTERCOUNT_LEVEL2];
		break;

	case MNS_LEVEL3:
		return level3[rand() % MONSTERCOUNT_LEVEL3];
		break;

	case MNS_LEVEL4:
		return level4[rand() % MONSTERCOUNT_LEVEL4];
		break;

	default:
		break;
    }

    // MNS_ALL
    return rand() % MONSTERMAXCOUNT;
}

BOOL	InitMonster(void){

    if(NULL == (ptrMonster = (S_MONSTER *) malloc(sizeof(S_MONSTER) * MONSTERMAXCOUNT))){
	fprintf(stderr, "InitMonster: malloc error: %d\n", sizeof(S_MONSTER) * MONSTERMAXCOUNT);
	return FALSE;
    }

	ptrMonster[PEASANT].attack       = 1;
	ptrMonster[PEASANT].defence      = 1;
	ptrMonster[PEASANT].shots        = 0;
	ptrMonster[PEASANT].damageMin    = 1;
	ptrMonster[PEASANT].damageMax    = 1;
	ptrMonster[PEASANT].hp           = 1;
	ptrMonster[PEASANT].speed        = VERYSLOW;
	ptrMonster[PEASANT].cost         = 20;
	ptrMonster[PEASANT].grown        = GROWN_WEEK_PEASANT;
	ptrMonster[PEASANT].descriptions = "Peasant";
	ptrMonster[PEASANT].filename = "PEASANT.ICN";

	ptrMonster[ARCHER].attack       = 5;
	ptrMonster[ARCHER].defence      = 3;
	ptrMonster[ARCHER].shots        = 12;
	ptrMonster[ARCHER].damageMin    = 2;
	ptrMonster[ARCHER].damageMax    = 3;
	ptrMonster[ARCHER].hp           = 10;
	ptrMonster[ARCHER].speed        = VERYSLOW;
	ptrMonster[ARCHER].cost         = 150;
	ptrMonster[ARCHER].grown        = GROWN_WEEK_ARCHER;
	ptrMonster[ARCHER].descriptions = "Archer";
	ptrMonster[ARCHER].filename = "ARCHER.ICN";

	ptrMonster[RANGER].attack       = 5;
	ptrMonster[RANGER].defence      = 3;
	ptrMonster[RANGER].shots        = 24;
	ptrMonster[RANGER].damageMin    = 2;
	ptrMonster[RANGER].damageMax    = 3;
	ptrMonster[RANGER].hp           = 10;
	ptrMonster[RANGER].speed        = AVERAGE;
	ptrMonster[RANGER].cost         = 200;
	ptrMonster[RANGER].grown        = GROWN_WEEK_RANGER;
	ptrMonster[RANGER].descriptions = "Ranger";
	ptrMonster[RANGER].filename = "ARCHER2.ICN";

	ptrMonster[PIKEMAN].attack       = 5;
	ptrMonster[PIKEMAN].defence      = 9;
	ptrMonster[PIKEMAN].shots        = 0;
	ptrMonster[PIKEMAN].damageMin    = 3;
	ptrMonster[PIKEMAN].damageMax    = 4;
	ptrMonster[PIKEMAN].hp           = 15;
	ptrMonster[PIKEMAN].speed        = AVERAGE;
	ptrMonster[PIKEMAN].cost         = 200;
	ptrMonster[PIKEMAN].grown        = GROWN_WEEK_PIKEMAN;
	ptrMonster[PIKEMAN].descriptions = "Pikeman";
	ptrMonster[PIKEMAN].filename = "PIKEMAN.ICN";

	ptrMonster[VETERAN_PIKEMAN].attack       = 5;
	ptrMonster[VETERAN_PIKEMAN].defence      = 9;
	ptrMonster[VETERAN_PIKEMAN].shots        = 0;
	ptrMonster[VETERAN_PIKEMAN].damageMin    = 3;
	ptrMonster[VETERAN_PIKEMAN].damageMax    = 4;
	ptrMonster[VETERAN_PIKEMAN].hp           = 20;
	ptrMonster[VETERAN_PIKEMAN].speed        = FAST;
	ptrMonster[VETERAN_PIKEMAN].cost         = 250;
	ptrMonster[VETERAN_PIKEMAN].grown        = GROWN_WEEK_VETERAN_PIKEMAN;
	ptrMonster[VETERAN_PIKEMAN].descriptions = "Veteran Pikeman";
	ptrMonster[VETERAN_PIKEMAN].filename = "PIKEMAN2.ICN";

	ptrMonster[SWORDSMAN].attack       = 7;
	ptrMonster[SWORDSMAN].defence      = 9;
	ptrMonster[SWORDSMAN].shots        = 0;
	ptrMonster[SWORDSMAN].damageMin    = 4;
	ptrMonster[SWORDSMAN].damageMax    = 6;
	ptrMonster[SWORDSMAN].hp           = 25;
	ptrMonster[SWORDSMAN].speed        = AVERAGE;
	ptrMonster[SWORDSMAN].cost         = 250;
	ptrMonster[SWORDSMAN].grown        = GROWN_WEEK_SWORDSMAN;
	ptrMonster[SWORDSMAN].descriptions = "Swordsman";
	ptrMonster[SWORDSMAN].filename = "SWORDSMN.ICN";

	ptrMonster[MASTER_SWORDSMAN].attack       = 7;
	ptrMonster[MASTER_SWORDSMAN].defence      = 9;
	ptrMonster[MASTER_SWORDSMAN].shots        = 0;
	ptrMonster[MASTER_SWORDSMAN].damageMin    = 4;
	ptrMonster[MASTER_SWORDSMAN].damageMax    = 6;
	ptrMonster[MASTER_SWORDSMAN].hp           = 30;
	ptrMonster[MASTER_SWORDSMAN].speed        = FAST;
	ptrMonster[MASTER_SWORDSMAN].cost         = 300;
	ptrMonster[MASTER_SWORDSMAN].grown        = GROWN_WEEK_MASTER_SWORDSMAN;
	ptrMonster[MASTER_SWORDSMAN].descriptions = "Master Swordsman";
	ptrMonster[MASTER_SWORDSMAN].filename = "SWORDSM2.ICN";

	ptrMonster[CAVALIRY].attack       = 10;
	ptrMonster[CAVALIRY].defence      = 9;
	ptrMonster[CAVALIRY].shots        = 0;
	ptrMonster[CAVALIRY].damageMin    = 5;
	ptrMonster[CAVALIRY].damageMax    = 9;
	ptrMonster[CAVALIRY].hp           = 30;
	ptrMonster[CAVALIRY].speed        = VERYFAST;
	ptrMonster[CAVALIRY].cost         = 300;
	ptrMonster[CAVALIRY].grown        = GROWN_WEEK_CAVALIRY;
	ptrMonster[CAVALIRY].descriptions = "Cavalry";
	ptrMonster[CAVALIRY].filename = "CAVALRYR.ICN";

	ptrMonster[CHAMPION].attack       = 10;
	ptrMonster[CHAMPION].defence      = 9;
	ptrMonster[CHAMPION].shots        = 0;
	ptrMonster[CHAMPION].damageMin    = 5;
	ptrMonster[CHAMPION].damageMax    = 10;
	ptrMonster[CHAMPION].hp           = 40;
	ptrMonster[CHAMPION].speed        = ULTRAFAST;
	ptrMonster[CHAMPION].cost         = 375;
	ptrMonster[CHAMPION].grown        = GROWN_WEEK_CHAMPION;
	ptrMonster[CHAMPION].descriptions = "Champion";
	ptrMonster[CHAMPION].filename = "CAVALRYB.ICN";

	ptrMonster[PALADIN].attack       = 11;
	ptrMonster[PALADIN].defence      = 12;
	ptrMonster[PALADIN].shots        = 0;
	ptrMonster[PALADIN].damageMin    = 10;
	ptrMonster[PALADIN].damageMax    = 20;
	ptrMonster[PALADIN].hp           = 50;
	ptrMonster[PALADIN].speed        = FAST;
	ptrMonster[PALADIN].cost         = 600;
	ptrMonster[PALADIN].grown        = GROWN_WEEK_PALADIN;
	ptrMonster[PALADIN].descriptions = "Paladin";
	ptrMonster[PALADIN].filename = "PALADIN.ICN";

	ptrMonster[CRUSADER].attack       = 11;
	ptrMonster[CRUSADER].defence      = 12;
	ptrMonster[CRUSADER].shots        = 0;
	ptrMonster[CRUSADER].damageMin    = 10;
	ptrMonster[CRUSADER].damageMax    = 20;
	ptrMonster[CRUSADER].hp           = 65;
	ptrMonster[CRUSADER].speed        = VERYFAST;
	ptrMonster[CRUSADER].cost         = 1000;
	ptrMonster[CRUSADER].grown        = GROWN_WEEK_CRUSADER;
	ptrMonster[CRUSADER].descriptions = "Crusader";
	ptrMonster[CRUSADER].filename = "PALADIN2.ICN";

	ptrMonster[GOBLIN].attack       = 3;
	ptrMonster[GOBLIN].defence      = 1;
	ptrMonster[GOBLIN].shots        = 0;
	ptrMonster[GOBLIN].damageMin    = 1;
	ptrMonster[GOBLIN].damageMax    = 2;
	ptrMonster[GOBLIN].hp           = 3;
	ptrMonster[GOBLIN].speed        = AVERAGE;
	ptrMonster[GOBLIN].cost         = 40;
	ptrMonster[GOBLIN].grown        = GROWN_WEEK_GOBLIN;
	ptrMonster[GOBLIN].descriptions = "Goblin";
	ptrMonster[GOBLIN].filename = "GOBLIN.ICN";

	ptrMonster[ORC].attack       = 3;
	ptrMonster[ORC].defence      = 4;
	ptrMonster[ORC].shots        = 8;
	ptrMonster[ORC].damageMin    = 2;
	ptrMonster[ORC].damageMax    = 3;
	ptrMonster[ORC].hp           = 10;
	ptrMonster[ORC].speed        = VERYSLOW;
	ptrMonster[ORC].cost         = 140;
	ptrMonster[ORC].grown        = GROWN_WEEK_ORC;
	ptrMonster[ORC].descriptions = "Orc";
	ptrMonster[ORC].filename = "ORC.ICN";

	ptrMonster[CHIEF_ORC].attack       = 3;
	ptrMonster[CHIEF_ORC].defence      = 4;
	ptrMonster[CHIEF_ORC].shots        = 16;
	ptrMonster[CHIEF_ORC].damageMin    = 3;
	ptrMonster[CHIEF_ORC].damageMax    = 4;
	ptrMonster[CHIEF_ORC].hp           = 15;
	ptrMonster[CHIEF_ORC].speed        = SLOW;
	ptrMonster[CHIEF_ORC].cost         = 175;
	ptrMonster[CHIEF_ORC].grown        = GROWN_WEEK_CHIEF_ORC;
	ptrMonster[CHIEF_ORC].descriptions = "Orc Chief";
	ptrMonster[CHIEF_ORC].filename = "ORC2.ICN";

	ptrMonster[WOLF].attack       = 6;
	ptrMonster[WOLF].defence      = 2;
	ptrMonster[WOLF].shots        = 0;
	ptrMonster[WOLF].damageMin    = 3;
	ptrMonster[WOLF].damageMax    = 5;
	ptrMonster[WOLF].hp           = 20;
	ptrMonster[WOLF].speed        = VERYFAST;
	ptrMonster[WOLF].cost         = 200;
	ptrMonster[WOLF].grown        = GROWN_WEEK_WOLF;
	ptrMonster[WOLF].descriptions = "Wolf";
	ptrMonster[WOLF].filename = "WOLF.ICN";

	ptrMonster[OGRE].attack       = 9;
	ptrMonster[OGRE].defence      = 5;
	ptrMonster[OGRE].shots        = 0;
	ptrMonster[OGRE].damageMin    = 4;
	ptrMonster[OGRE].damageMax    = 6;
	ptrMonster[OGRE].hp           = 40;
	ptrMonster[OGRE].speed        = VERYSLOW;
	ptrMonster[OGRE].cost         = 300;
	ptrMonster[OGRE].grown        = GROWN_WEEK_OGRE;
	ptrMonster[OGRE].descriptions = "Ogre";
	ptrMonster[OGRE].filename = "OGRE.ICN";

	ptrMonster[LORD_OGRE].attack       = 9;
	ptrMonster[LORD_OGRE].defence      = 5;
	ptrMonster[LORD_OGRE].shots        = 0;
	ptrMonster[LORD_OGRE].damageMin    = 5;
	ptrMonster[LORD_OGRE].damageMax    = 7;
	ptrMonster[LORD_OGRE].hp           = 60;
	ptrMonster[LORD_OGRE].speed        = AVERAGE;
	ptrMonster[LORD_OGRE].cost         = 500;
	ptrMonster[LORD_OGRE].grown        = GROWN_WEEK_LORD_OGRE;
	ptrMonster[LORD_OGRE].descriptions = "Ogre Lord";
	ptrMonster[LORD_OGRE].filename = "OGRE2.ICN";

	ptrMonster[TROLL].attack       = 10;
	ptrMonster[TROLL].defence      = 5;
	ptrMonster[TROLL].shots        = 8;
	ptrMonster[TROLL].damageMin    = 5;
	ptrMonster[TROLL].damageMax    = 7;
	ptrMonster[TROLL].hp           = 40;
	ptrMonster[TROLL].speed        = AVERAGE;
	ptrMonster[TROLL].cost         = 600;
	ptrMonster[TROLL].grown        = GROWN_WEEK_TROLL;
	ptrMonster[TROLL].descriptions = "Troll";
	ptrMonster[TROLL].filename = "TROLL.ICN";

	ptrMonster[WAR_TROLL].attack       = 10;
	ptrMonster[WAR_TROLL].defence      = 5;
	ptrMonster[WAR_TROLL].shots        = 16;
	ptrMonster[WAR_TROLL].damageMin    = 7;
	ptrMonster[WAR_TROLL].damageMax    = 9;
	ptrMonster[WAR_TROLL].hp           = 40;
	ptrMonster[WAR_TROLL].speed        = FAST;
	ptrMonster[WAR_TROLL].cost         = 700;
	ptrMonster[WAR_TROLL].grown        = GROWN_WEEK_WAR_TROLL;
	ptrMonster[WAR_TROLL].descriptions = "War Troll";
	ptrMonster[WAR_TROLL].filename = "TROLL2.ICN";

	ptrMonster[CYCLOPS].attack       = 12;
	ptrMonster[CYCLOPS].defence      = 9;
	ptrMonster[CYCLOPS].shots        = 0;
	ptrMonster[CYCLOPS].damageMin    = 12;
	ptrMonster[CYCLOPS].damageMax    = 24;
	ptrMonster[CYCLOPS].hp           = 80;
	ptrMonster[CYCLOPS].speed        = FAST;
	ptrMonster[CYCLOPS].cost         = 750;
	ptrMonster[CYCLOPS].grown        = GROWN_WEEK_CYCLOPS;
	ptrMonster[CYCLOPS].descriptions = "Cyclops";
	ptrMonster[CYCLOPS].filename = "CYCLOPS.ICN";

	ptrMonster[SPRITE].attack       = 4;
	ptrMonster[SPRITE].defence      = 2;
	ptrMonster[SPRITE].shots        = 0;
	ptrMonster[SPRITE].damageMin    = 1;
	ptrMonster[SPRITE].damageMax    = 2;
	ptrMonster[SPRITE].hp           = 2;
	ptrMonster[SPRITE].speed        = AVERAGE;
	ptrMonster[SPRITE].cost         = 50;
	ptrMonster[SPRITE].grown        = GROWN_WEEK_SPRITE;
	ptrMonster[SPRITE].descriptions = "Sprite";
	ptrMonster[SPRITE].filename = "SPRITE.ICN";

	ptrMonster[DWARF].attack       = 6;
	ptrMonster[DWARF].defence      = 5;
	ptrMonster[DWARF].shots        = 0;
	ptrMonster[DWARF].damageMin    = 2;
	ptrMonster[DWARF].damageMax    = 4;
	ptrMonster[DWARF].hp           = 20;
	ptrMonster[DWARF].speed        = VERYSLOW;
	ptrMonster[DWARF].cost         = 200;
	ptrMonster[DWARF].grown        = GROWN_WEEK_DWARF;
	ptrMonster[DWARF].descriptions = "Dwarf";
	ptrMonster[DWARF].filename = "DWARF.ICN";

	ptrMonster[BATTLE_DWARF].attack       = 6;
	ptrMonster[BATTLE_DWARF].defence      = 6;
	ptrMonster[BATTLE_DWARF].shots        = 0;
	ptrMonster[BATTLE_DWARF].damageMin    = 2;
	ptrMonster[BATTLE_DWARF].damageMax    = 4;
	ptrMonster[BATTLE_DWARF].hp           = 20;
	ptrMonster[BATTLE_DWARF].speed        = AVERAGE;
	ptrMonster[BATTLE_DWARF].cost         = 250;
	ptrMonster[BATTLE_DWARF].grown        = GROWN_WEEK_BATTLE_DWARF;
	ptrMonster[BATTLE_DWARF].descriptions = "Battle Dwarf";
	ptrMonster[BATTLE_DWARF].filename = "DWARF2.ICN";

	ptrMonster[ELF].attack       = 4;
	ptrMonster[ELF].defence      = 3;
	ptrMonster[ELF].shots        = 24;
	ptrMonster[ELF].damageMin    = 2;
	ptrMonster[ELF].damageMax    = 3;
	ptrMonster[ELF].hp           = 15;
	ptrMonster[ELF].speed	     = AVERAGE;
	ptrMonster[ELF].cost         = 250;
	ptrMonster[ELF].grown        = GROWN_WEEK_ELF;
	ptrMonster[ELF].descriptions = "Elf";
	ptrMonster[ELF].filename = "ELF.ICN";

	ptrMonster[GRAND_ELF].attack       = 5;
	ptrMonster[GRAND_ELF].defence      = 5;
	ptrMonster[GRAND_ELF].shots        = 24;
	ptrMonster[GRAND_ELF].damageMin    = 2;
	ptrMonster[GRAND_ELF].damageMax    = 3;
	ptrMonster[GRAND_ELF].hp           = 15;
	ptrMonster[GRAND_ELF].speed        = VERYFAST;
	ptrMonster[GRAND_ELF].cost         = 300;
	ptrMonster[GRAND_ELF].grown        = GROWN_WEEK_GRAND_ELF;
	ptrMonster[GRAND_ELF].descriptions = "Grand Elf";
	ptrMonster[GRAND_ELF].filename = "ELF2.ICN";

	ptrMonster[DRUID].attack       = 7;
	ptrMonster[DRUID].defence      = 5;
	ptrMonster[DRUID].shots        = 8;
	ptrMonster[DRUID].damageMin    = 5;
	ptrMonster[DRUID].damageMax    = 8;
	ptrMonster[DRUID].hp           = 25;
	ptrMonster[DRUID].speed        = FAST;
	ptrMonster[DRUID].cost         = 350;
	ptrMonster[DRUID].grown        = GROWN_WEEK_DRUID;
	ptrMonster[DRUID].descriptions = "Druid";
	ptrMonster[DRUID].filename = "DRUID.ICN";

	ptrMonster[GREATER_DRUID].attack       = 7;
	ptrMonster[GREATER_DRUID].defence      = 7;
	ptrMonster[GREATER_DRUID].shots        = 16;
	ptrMonster[GREATER_DRUID].damageMin    = 5;
	ptrMonster[GREATER_DRUID].damageMax    = 8;
	ptrMonster[GREATER_DRUID].hp           = 25;
	ptrMonster[GREATER_DRUID].speed        = VERYFAST;
	ptrMonster[GREATER_DRUID].cost         = 400;
	ptrMonster[GREATER_DRUID].grown        = GROWN_WEEK_GREATER_DRUID;
	ptrMonster[GREATER_DRUID].descriptions = "Greater Druid";
	ptrMonster[GREATER_DRUID].filename = "DRUID2.ICN";

	ptrMonster[UNICORN].attack       = 10;
	ptrMonster[UNICORN].defence      = 9;
	ptrMonster[UNICORN].shots        = 0;
	ptrMonster[UNICORN].damageMin    = 7;
	ptrMonster[UNICORN].damageMax    = 14;
	ptrMonster[UNICORN].hp           = 40;
	ptrMonster[UNICORN].speed        = FAST;
	ptrMonster[UNICORN].cost         = 500;
	ptrMonster[UNICORN].grown        = GROWN_WEEK_UNICORN;
	ptrMonster[UNICORN].descriptions = "Unicorn";
	ptrMonster[UNICORN].filename = "UNICORN.ICN";

	ptrMonster[PHOENIX].attack       = 12;
	ptrMonster[PHOENIX].defence      = 10;
	ptrMonster[PHOENIX].shots        = 0;
	ptrMonster[PHOENIX].damageMin    = 20;
	ptrMonster[PHOENIX].damageMax    = 40;
	ptrMonster[PHOENIX].hp           = 100;
	ptrMonster[PHOENIX].speed        = ULTRAFAST;
	ptrMonster[PHOENIX].cost         = 1500;
	ptrMonster[PHOENIX].grown        = GROWN_WEEK_PHOENIX;
	ptrMonster[PHOENIX].descriptions = "Phoenix";
	ptrMonster[PHOENIX].filename = "PHOENIX.ICN";

	ptrMonster[CENTAUR].attack       = 3;
	ptrMonster[CENTAUR].defence      = 1;
	ptrMonster[CENTAUR].shots        = 8;
	ptrMonster[CENTAUR].damageMin    = 1;
	ptrMonster[CENTAUR].damageMax    = 2;
	ptrMonster[CENTAUR].hp           = 5;
	ptrMonster[CENTAUR].speed        = AVERAGE;
	ptrMonster[CENTAUR].cost         = 60;
	ptrMonster[CENTAUR].grown        = GROWN_WEEK_CENTAUR;
	ptrMonster[CENTAUR].descriptions = "Centaur";
	ptrMonster[CENTAUR].filename = "CENTAUR.ICN";

	ptrMonster[GARGOYLE].attack       = 4;
	ptrMonster[GARGOYLE].defence      = 7;
	ptrMonster[GARGOYLE].shots        = 0;
	ptrMonster[GARGOYLE].damageMin    = 2;
	ptrMonster[GARGOYLE].damageMax    = 3;
	ptrMonster[GARGOYLE].hp           = 15;
	ptrMonster[GARGOYLE].speed        = VERYFAST;
	ptrMonster[GARGOYLE].cost         = 200;
	ptrMonster[GARGOYLE].grown        = GROWN_WEEK_GARGOYLE;
	ptrMonster[GARGOYLE].descriptions = "Gargoyle";
	ptrMonster[GARGOYLE].filename = "GARGOYLE.ICN";

	ptrMonster[GRIFFIN].attack       = 6;
	ptrMonster[GRIFFIN].defence      = 6;
	ptrMonster[GRIFFIN].shots        = 0;
	ptrMonster[GRIFFIN].damageMin    = 3;
	ptrMonster[GRIFFIN].damageMax    = 5;
	ptrMonster[GRIFFIN].hp           = 25;
	ptrMonster[GRIFFIN].speed        = AVERAGE;
	ptrMonster[GRIFFIN].cost         = 300;
	ptrMonster[GRIFFIN].grown        = GROWN_WEEK_GRIFFIN;
	ptrMonster[GRIFFIN].descriptions = "Griffin";
	ptrMonster[GRIFFIN].filename = "GRIFFIN.ICN";

	ptrMonster[MINOTAUR].attack       = 9;
	ptrMonster[MINOTAUR].defence      = 8;
	ptrMonster[MINOTAUR].shots        = 0;
	ptrMonster[MINOTAUR].damageMin    = 5;
	ptrMonster[MINOTAUR].damageMax    = 10;
	ptrMonster[MINOTAUR].hp           = 35;
	ptrMonster[MINOTAUR].speed        = AVERAGE;
	ptrMonster[MINOTAUR].cost         = 400;
	ptrMonster[MINOTAUR].grown        = GROWN_WEEK_MINOTAUR;
	ptrMonster[MINOTAUR].descriptions = "Minotaur";
	ptrMonster[MINOTAUR].filename = "MINOTAUR.ICN";

	ptrMonster[KNIGHT_MINOTAUR].attack       = 9;
	ptrMonster[KNIGHT_MINOTAUR].defence      = 8;
	ptrMonster[KNIGHT_MINOTAUR].shots        = 0;
	ptrMonster[KNIGHT_MINOTAUR].damageMin    = 5;
	ptrMonster[KNIGHT_MINOTAUR].damageMax    = 10;
	ptrMonster[KNIGHT_MINOTAUR].hp           = 45;
	ptrMonster[KNIGHT_MINOTAUR].speed        = VERYFAST;
	ptrMonster[KNIGHT_MINOTAUR].cost         = 500;
	ptrMonster[KNIGHT_MINOTAUR].grown        = GROWN_WEEK_KNIGHT_MINOTAUR;
	ptrMonster[KNIGHT_MINOTAUR].descriptions = "Minotaur King";
	ptrMonster[KNIGHT_MINOTAUR].filename = "MINOTAU2.ICN";

	ptrMonster[HIDRA].attack       = 8;
	ptrMonster[HIDRA].defence      = 9;
	ptrMonster[HIDRA].shots        = 0;
	ptrMonster[HIDRA].damageMin    = 6;
	ptrMonster[HIDRA].damageMax    = 12;
	ptrMonster[HIDRA].hp           = 75;
	ptrMonster[HIDRA].speed        = VERYSLOW;
	ptrMonster[HIDRA].cost         = 800;
	ptrMonster[HIDRA].grown        = GROWN_WEEK_HIDRA;
	ptrMonster[HIDRA].descriptions = "Hydra";
	ptrMonster[HIDRA].filename = "HYDRA.ICN";

	ptrMonster[GREEN_DRAGON].attack       = 12;
	ptrMonster[GREEN_DRAGON].defence      = 12;
	ptrMonster[GREEN_DRAGON].shots        = 0;
	ptrMonster[GREEN_DRAGON].damageMin    = 25;
	ptrMonster[GREEN_DRAGON].damageMax    = 50;
	ptrMonster[GREEN_DRAGON].hp           = 200;
	ptrMonster[GREEN_DRAGON].speed        = AVERAGE;
	ptrMonster[GREEN_DRAGON].cost         = 3000;
	ptrMonster[GREEN_DRAGON].grown        = GROWN_WEEK_GREEN_DRAGON;
	ptrMonster[GREEN_DRAGON].descriptions = "Green Dragon";
	ptrMonster[GREEN_DRAGON].filename = "DRAGGREE.ICN";

	ptrMonster[RED_DRAGON].attack       = 13;
	ptrMonster[RED_DRAGON].defence      = 13;
	ptrMonster[RED_DRAGON].shots        = 0;
	ptrMonster[RED_DRAGON].damageMin    = 25;
	ptrMonster[RED_DRAGON].damageMax    = 50;
	ptrMonster[RED_DRAGON].hp           = 250;
	ptrMonster[RED_DRAGON].speed        = FAST;
	ptrMonster[RED_DRAGON].cost         = 3500;
	ptrMonster[RED_DRAGON].grown        = GROWN_WEEK_RED_DRAGON;
	ptrMonster[RED_DRAGON].descriptions = "Red Dragon";
	ptrMonster[RED_DRAGON].filename = "DRAGRED.ICN";

	ptrMonster[BLACK_DRAGON].attack       = 14;
	ptrMonster[BLACK_DRAGON].defence      = 14;
	ptrMonster[BLACK_DRAGON].shots        = 0;
	ptrMonster[BLACK_DRAGON].damageMin    = 25;
	ptrMonster[BLACK_DRAGON].damageMax    = 50;
	ptrMonster[BLACK_DRAGON].hp           = 300;
	ptrMonster[BLACK_DRAGON].speed        = VERYFAST;
	ptrMonster[BLACK_DRAGON].cost         = 4000;
	ptrMonster[BLACK_DRAGON].grown        = GROWN_WEEK_BLACK_DRAGON;
	ptrMonster[BLACK_DRAGON].descriptions = "Black Dragon";
	ptrMonster[BLACK_DRAGON].filename = "DRAGBLAK.ICN";

	ptrMonster[HALFLING].attack       = 2;
	ptrMonster[HALFLING].defence      = 1;
	ptrMonster[HALFLING].shots        = 12;
	ptrMonster[HALFLING].damageMin    = 1;
	ptrMonster[HALFLING].damageMax    = 3;
	ptrMonster[HALFLING].hp           = 3;
	ptrMonster[HALFLING].speed        = SLOW;
	ptrMonster[HALFLING].cost         = 50;
	ptrMonster[HALFLING].grown        = GROWN_WEEK_HALFLING;
	ptrMonster[HALFLING].descriptions = "Halfling";
	ptrMonster[HALFLING].filename = "HALFLING.ICN";

	ptrMonster[BOAR].attack       = 5;
	ptrMonster[BOAR].defence      = 4;
	ptrMonster[BOAR].shots        = 0;
	ptrMonster[BOAR].damageMin    = 2;
	ptrMonster[BOAR].damageMax    = 3;
	ptrMonster[BOAR].hp           = 15;
	ptrMonster[BOAR].speed        = VERYFAST;
	ptrMonster[BOAR].cost         = 150;
	ptrMonster[BOAR].grown        = GROWN_WEEK_BOAR;
	ptrMonster[BOAR].descriptions = "Boar";
	ptrMonster[BOAR].filename = "BOAR.ICN";

	ptrMonster[IRON_GOLEM].attack       = 5;
	ptrMonster[IRON_GOLEM].defence      = 10;
	ptrMonster[IRON_GOLEM].shots        = 0;
	ptrMonster[IRON_GOLEM].damageMin    = 4;
	ptrMonster[IRON_GOLEM].damageMax    = 5;
	ptrMonster[IRON_GOLEM].hp           = 30;
	ptrMonster[IRON_GOLEM].speed        = VERYSLOW;
	ptrMonster[IRON_GOLEM].cost         = 300;
	ptrMonster[IRON_GOLEM].grown        = GROWN_WEEK_IRON_GOLEM;
	ptrMonster[IRON_GOLEM].descriptions = "Iron Golem";
	ptrMonster[IRON_GOLEM].filename = "GOLEM.ICN";

	ptrMonster[STEEL_GOLEM].attack       = 7;
	ptrMonster[STEEL_GOLEM].defence      = 10;
	ptrMonster[STEEL_GOLEM].shots        = 0;
	ptrMonster[STEEL_GOLEM].damageMin    = 4;
	ptrMonster[STEEL_GOLEM].damageMax    = 5;
	ptrMonster[STEEL_GOLEM].hp           = 35;
	ptrMonster[STEEL_GOLEM].speed        = SLOW;
	ptrMonster[STEEL_GOLEM].cost         = 350;
	ptrMonster[STEEL_GOLEM].grown        = GROWN_WEEK_STEEL_GOLEM;
	ptrMonster[STEEL_GOLEM].descriptions = "Steel Golem";
	ptrMonster[STEEL_GOLEM].filename = "GOLEM2.ICN";

	ptrMonster[ROC].attack       = 7;
	ptrMonster[ROC].defence      = 7;
	ptrMonster[ROC].shots        = 0;
	ptrMonster[ROC].damageMin    = 4;
	ptrMonster[ROC].damageMax    = 8;
	ptrMonster[ROC].hp           = 40;
	ptrMonster[ROC].speed        = AVERAGE;
	ptrMonster[ROC].cost         = 400;
	ptrMonster[ROC].grown        = GROWN_WEEK_ROC;
	ptrMonster[ROC].descriptions = "Roc";
	ptrMonster[ROC].filename = "ROC.ICN";

	ptrMonster[MAGE].attack       = 11;
	ptrMonster[MAGE].defence      = 7;
	ptrMonster[MAGE].shots        = 12;
	ptrMonster[MAGE].damageMin    = 7;
	ptrMonster[MAGE].damageMax    = 9;
	ptrMonster[MAGE].hp           = 30;
	ptrMonster[MAGE].speed        = FAST;
	ptrMonster[MAGE].cost         = 600;
	ptrMonster[MAGE].grown        = GROWN_WEEK_MAGE;
	ptrMonster[MAGE].descriptions = "Mage";
	ptrMonster[MAGE].filename = "MAGE1.ICN";

	ptrMonster[ARCHMAGE].attack       = 12;
	ptrMonster[ARCHMAGE].defence      = 8;
	ptrMonster[ARCHMAGE].shots        = 24;
	ptrMonster[ARCHMAGE].damageMin    = 7;
	ptrMonster[ARCHMAGE].damageMax    = 9;
	ptrMonster[ARCHMAGE].hp           = 35;
	ptrMonster[ARCHMAGE].speed        = VERYFAST;
	ptrMonster[ARCHMAGE].cost         = 700;
	ptrMonster[ARCHMAGE].grown        = GROWN_WEEK_ARCHMAGE;
	ptrMonster[ARCHMAGE].descriptions = "Archmage";
	ptrMonster[ARCHMAGE].filename = "MAGE2.ICN";

	ptrMonster[GIANT].attack       = 13;
	ptrMonster[GIANT].defence      = 10;
	ptrMonster[GIANT].shots        = 0;
	ptrMonster[GIANT].damageMin    = 20;
	ptrMonster[GIANT].damageMax    = 30;
	ptrMonster[GIANT].hp           = 150;
	ptrMonster[GIANT].speed        = AVERAGE;
	ptrMonster[GIANT].cost         = 2000;
	ptrMonster[GIANT].grown        = GROWN_WEEK_GIANT;
	ptrMonster[GIANT].descriptions = "Giant";
	ptrMonster[GIANT].filename = "TITANBLU.ICN";

	ptrMonster[TITAN].attack       = 15;
	ptrMonster[TITAN].defence      = 15;
	ptrMonster[TITAN].shots        = 24;
	ptrMonster[TITAN].damageMin    = 20;
	ptrMonster[TITAN].damageMax    = 30;
	ptrMonster[TITAN].hp           = 300;
	ptrMonster[TITAN].speed        = VERYFAST;
	ptrMonster[TITAN].cost         = 5000;
	ptrMonster[TITAN].grown        = GROWN_WEEK_TITAN;
	ptrMonster[TITAN].descriptions = "Titan";
	ptrMonster[TITAN].filename = "TITANBLA.ICN";

	ptrMonster[SKELETON].attack       = 4;
	ptrMonster[SKELETON].defence      = 3;
	ptrMonster[SKELETON].shots        = 0;
	ptrMonster[SKELETON].damageMin    = 2;
	ptrMonster[SKELETON].damageMax    = 3;
	ptrMonster[SKELETON].hp           = 4;
	ptrMonster[SKELETON].speed        = AVERAGE;
	ptrMonster[SKELETON].cost         = 75;
	ptrMonster[SKELETON].grown        = GROWN_WEEK_SKELETON;
	ptrMonster[SKELETON].descriptions = "Skeleton";
	ptrMonster[SKELETON].filename = "SKELETON.ICN";
	
	ptrMonster[ZOMBIE].attack       = 5;
	ptrMonster[ZOMBIE].defence      = 2;
	ptrMonster[ZOMBIE].shots        = 0;
	ptrMonster[ZOMBIE].damageMin    = 2;
	ptrMonster[ZOMBIE].damageMax    = 3;
	ptrMonster[ZOMBIE].hp           = 15;
	ptrMonster[ZOMBIE].speed        = VERYSLOW;
	ptrMonster[ZOMBIE].cost         = 150;
	ptrMonster[ZOMBIE].grown        = GROWN_WEEK_ZOMBIE;
	ptrMonster[ZOMBIE].descriptions = "Zombie";
	ptrMonster[ZOMBIE].filename = "ZOMBIE.ICN";

	ptrMonster[MUTANT_ZOMBIE].attack       = 5;
	ptrMonster[MUTANT_ZOMBIE].defence      = 2;
	ptrMonster[MUTANT_ZOMBIE].shots        = 0;
	ptrMonster[MUTANT_ZOMBIE].damageMin    = 2;
	ptrMonster[MUTANT_ZOMBIE].damageMax    = 3;
	ptrMonster[MUTANT_ZOMBIE].hp           = 20;
	ptrMonster[MUTANT_ZOMBIE].speed        = AVERAGE;
	ptrMonster[MUTANT_ZOMBIE].cost         = 200;
	ptrMonster[MUTANT_ZOMBIE].grown        = GROWN_WEEK_MUTANT_ZOMBIE;
	ptrMonster[MUTANT_ZOMBIE].descriptions = "Mutant Zombie";
	ptrMonster[MUTANT_ZOMBIE].filename = "ZOMBIE2.ICN";

	ptrMonster[MUMMY].attack       = 6;
	ptrMonster[MUMMY].defence      = 6;
	ptrMonster[MUMMY].shots        = 0;
	ptrMonster[MUMMY].damageMin    = 3;
	ptrMonster[MUMMY].damageMax    = 4;
	ptrMonster[MUMMY].hp           = 25;
	ptrMonster[MUMMY].speed        = AVERAGE;
	ptrMonster[MUMMY].cost         = 250;
	ptrMonster[MUMMY].grown        = GROWN_WEEK_MUMMY;
	ptrMonster[MUMMY].descriptions = "Mummy";
	ptrMonster[MUMMY].filename = "MUMMYW.ICN";

	ptrMonster[ROYAL_MUMMY].attack       = 6;
	ptrMonster[ROYAL_MUMMY].defence      = 6;
	ptrMonster[ROYAL_MUMMY].shots        = 0;
	ptrMonster[ROYAL_MUMMY].damageMin    = 3;
	ptrMonster[ROYAL_MUMMY].damageMax    = 4;
	ptrMonster[ROYAL_MUMMY].hp           = 30;
	ptrMonster[ROYAL_MUMMY].speed        = FAST;
	ptrMonster[ROYAL_MUMMY].cost         = 300;
	ptrMonster[ROYAL_MUMMY].grown        = GROWN_WEEK_ROYAL_MUMMY;
	ptrMonster[ROYAL_MUMMY].descriptions = "Royal Mummy";
	ptrMonster[ROYAL_MUMMY].filename = "MUMMY2.ICN";

	ptrMonster[VAMPIRE].attack       = 8;
	ptrMonster[VAMPIRE].defence      = 6;
	ptrMonster[VAMPIRE].shots        = 0;
	ptrMonster[VAMPIRE].damageMin    = 5;
	ptrMonster[VAMPIRE].damageMax    = 7;
	ptrMonster[VAMPIRE].hp           = 30;
	ptrMonster[VAMPIRE].speed        = AVERAGE;
	ptrMonster[VAMPIRE].cost         = 500;
	ptrMonster[VAMPIRE].grown        = GROWN_WEEK_VAMPIRE;
	ptrMonster[VAMPIRE].descriptions = "Vampire";
	ptrMonster[VAMPIRE].filename = "VAMPIRE.ICN";

	ptrMonster[LORD_VAMPIRE].attack       = 8;
	ptrMonster[LORD_VAMPIRE].defence      = 6;
	ptrMonster[LORD_VAMPIRE].shots        = 0;
	ptrMonster[LORD_VAMPIRE].damageMin    = 5;
	ptrMonster[LORD_VAMPIRE].damageMax    = 7;
	ptrMonster[LORD_VAMPIRE].hp           = 40;
	ptrMonster[LORD_VAMPIRE].speed        = FAST;
	ptrMonster[LORD_VAMPIRE].cost         = 650;
	ptrMonster[LORD_VAMPIRE].grown        = GROWN_WEEK_LORD_VAMPIRE;
	ptrMonster[LORD_VAMPIRE].descriptions = "Vampire Lord";
	ptrMonster[LORD_VAMPIRE].filename = "VAMPIRE2.ICN";

	ptrMonster[LICH].attack       = 7;
	ptrMonster[LICH].defence      = 12;
	ptrMonster[LICH].shots        = 12;
	ptrMonster[LICH].damageMin    = 8;
	ptrMonster[LICH].damageMax    = 10;
	ptrMonster[LICH].hp           = 25;
	ptrMonster[LICH].speed        = FAST;
	ptrMonster[LICH].cost         = 750;
	ptrMonster[LICH].grown        = GROWN_WEEK_LICH;
	ptrMonster[LICH].descriptions = "Lich";
	ptrMonster[LICH].filename = "LICH.ICN";

	ptrMonster[POWER_LICH].attack       = 7;
	ptrMonster[POWER_LICH].defence      = 13;
	ptrMonster[POWER_LICH].shots        = 24;
	ptrMonster[POWER_LICH].damageMin    = 8;
	ptrMonster[POWER_LICH].damageMax    = 10;
	ptrMonster[POWER_LICH].hp           = 35;
	ptrMonster[POWER_LICH].speed        = VERYFAST;
	ptrMonster[POWER_LICH].cost         = 900;
	ptrMonster[POWER_LICH].grown        = GROWN_WEEK_POWER_LICH;
	ptrMonster[POWER_LICH].descriptions = "Power Lich";
	ptrMonster[POWER_LICH].filename = "LICH2.ICN";

	ptrMonster[BONE_DRAGON].attack       = 11;
	ptrMonster[BONE_DRAGON].defence      = 9;
	ptrMonster[BONE_DRAGON].shots        = 0;
	ptrMonster[BONE_DRAGON].damageMin    = 25;
	ptrMonster[BONE_DRAGON].damageMax    = 45;
	ptrMonster[BONE_DRAGON].hp           = 150;
	ptrMonster[BONE_DRAGON].speed        = AVERAGE;
	ptrMonster[BONE_DRAGON].cost         = 1500;
	ptrMonster[BONE_DRAGON].grown        = GROWN_WEEK_BONE_DRAGON;
	ptrMonster[BONE_DRAGON].descriptions = "Bone Dragon";
	ptrMonster[BONE_DRAGON].filename = "DRAGBONE.ICN";

	ptrMonster[ROGUE].attack       = 6;
	ptrMonster[ROGUE].defence      = 1;
	ptrMonster[ROGUE].shots        = 0;
	ptrMonster[ROGUE].damageMin    = 1;
	ptrMonster[ROGUE].damageMax    = 2;
	ptrMonster[ROGUE].hp           = 4;
	ptrMonster[ROGUE].speed        = FAST;
	ptrMonster[ROGUE].cost         = 50;
	ptrMonster[ROGUE].grown        = GROWN_WEEK_ROGUE;
	ptrMonster[ROGUE].descriptions = "Rogue";
	ptrMonster[ROGUE].filename = "ROGUE.ICN";

	ptrMonster[NOMAD].attack       = 7;
	ptrMonster[NOMAD].defence      = 6;
	ptrMonster[NOMAD].shots        = 0;
	ptrMonster[NOMAD].damageMin    = 2;
	ptrMonster[NOMAD].damageMax    = 5;
	ptrMonster[NOMAD].hp           = 20;
	ptrMonster[NOMAD].speed        = VERYFAST;
	ptrMonster[NOMAD].cost         = 200;
	ptrMonster[NOMAD].grown        = GROWN_WEEK_NOMAD;
	ptrMonster[NOMAD].descriptions = "Nomad";
	ptrMonster[NOMAD].filename = "NOMAD.ICN";

	ptrMonster[GHOST].attack       = 8;
	ptrMonster[GHOST].defence      = 7;
	ptrMonster[GHOST].shots        = 0;
	ptrMonster[GHOST].damageMin    = 4;
	ptrMonster[GHOST].damageMax    = 6;
	ptrMonster[GHOST].hp           = 20;
	ptrMonster[GHOST].speed        = FAST;
	ptrMonster[GHOST].cost         = 1000;
	ptrMonster[GHOST].grown        = GROWN_WEEK_GHOST;
	ptrMonster[GHOST].descriptions = "Ghost";
	ptrMonster[GHOST].filename = "GHOST.ICN";

	ptrMonster[GENIE].attack       = 10;
	ptrMonster[GENIE].defence      = 9;
	ptrMonster[GENIE].shots        = 0;
	ptrMonster[GENIE].damageMin    = 20;
	ptrMonster[GENIE].damageMax    = 30;
	ptrMonster[GENIE].hp           = 50;
	ptrMonster[GENIE].speed        = VERYFAST;
	ptrMonster[GENIE].cost         = 650;
	ptrMonster[GENIE].grown        = GROWN_WEEK_GENIE;
	ptrMonster[GENIE].descriptions = "Genie";
	ptrMonster[GENIE].filename = "GENIE.ICN";

	ptrMonster[MEDUSA].attack       = 8;
	ptrMonster[MEDUSA].defence      = 9;
	ptrMonster[MEDUSA].shots        = 0;
	ptrMonster[MEDUSA].damageMin    = 6;
	ptrMonster[MEDUSA].damageMax    = 10;
	ptrMonster[MEDUSA].hp           = 35;
	ptrMonster[MEDUSA].speed        = AVERAGE;
	ptrMonster[MEDUSA].cost         = 500;
	ptrMonster[MEDUSA].grown        = GROWN_WEEK_MEDUSA;
	ptrMonster[MEDUSA].descriptions = "Medusa";
	ptrMonster[MEDUSA].filename = "MEDUSA.ICN";

	ptrMonster[EARTH_ELEMENT].attack       = 8;
	ptrMonster[EARTH_ELEMENT].defence      = 8;
	ptrMonster[EARTH_ELEMENT].shots        = 0;
	ptrMonster[EARTH_ELEMENT].damageMin    = 4;
	ptrMonster[EARTH_ELEMENT].damageMax    = 5;
	ptrMonster[EARTH_ELEMENT].hp           = 50;
	ptrMonster[EARTH_ELEMENT].speed        = SLOW;
	ptrMonster[EARTH_ELEMENT].cost         = 500;
	ptrMonster[EARTH_ELEMENT].grown        = GROWN_WEEK_EARTH_ELEMENT;
	ptrMonster[EARTH_ELEMENT].descriptions = "Earth Element";
	ptrMonster[EARTH_ELEMENT].filename = "EELEM.ICN";

	ptrMonster[AIR_ELEMENT].attack       = 7;
	ptrMonster[AIR_ELEMENT].defence      = 7;
	ptrMonster[AIR_ELEMENT].shots        = 0;
	ptrMonster[AIR_ELEMENT].damageMin    = 2;
	ptrMonster[AIR_ELEMENT].damageMax    = 8;
	ptrMonster[AIR_ELEMENT].hp           = 35;
	ptrMonster[AIR_ELEMENT].speed        = VERYFAST;
	ptrMonster[AIR_ELEMENT].cost         = 500;
	ptrMonster[AIR_ELEMENT].grown	     = GROWN_WEEK_AIR_ELEMENT;
	ptrMonster[AIR_ELEMENT].descriptions = "Air Element";
	ptrMonster[AIR_ELEMENT].filename = "AELEM.ICN";

	ptrMonster[FIRE_ELEMENT].attack       = 8;
	ptrMonster[FIRE_ELEMENT].defence      = 6;
	ptrMonster[FIRE_ELEMENT].shots        = 0;
	ptrMonster[FIRE_ELEMENT].damageMin    = 4;
	ptrMonster[FIRE_ELEMENT].damageMax    = 6;
	ptrMonster[FIRE_ELEMENT].hp           = 40;
	ptrMonster[FIRE_ELEMENT].speed        = FAST;
	ptrMonster[FIRE_ELEMENT].cost         = 500;
	ptrMonster[FIRE_ELEMENT].grown        = GROWN_WEEK_FIRE_ELEMENT;
	ptrMonster[FIRE_ELEMENT].descriptions = "Fire Element";
	ptrMonster[FIRE_ELEMENT].filename = "FELEM.ICN";

	ptrMonster[WATER_ELEMENT].attack       = 6;
	ptrMonster[WATER_ELEMENT].defence      = 8;
	ptrMonster[WATER_ELEMENT].shots        = 0;
	ptrMonster[WATER_ELEMENT].damageMin    = 3;
	ptrMonster[WATER_ELEMENT].damageMax    = 7;
	ptrMonster[WATER_ELEMENT].hp           = 45;
	ptrMonster[WATER_ELEMENT].speed        = AVERAGE;
	ptrMonster[WATER_ELEMENT].cost         = 500;
	ptrMonster[WATER_ELEMENT].grown        = GROWN_WEEK_WATER_ELEMENT;
	ptrMonster[WATER_ELEMENT].descriptions = "Water Element";
	ptrMonster[WATER_ELEMENT].filename = "WELEM.ICN";

    fprintf(stderr, "Init monster.\n");

    return TRUE;
}

const char * GetStringMonster(E_MONSTER name){

    return ptrMonster[name].descriptions;
}

const char * MonsterBigNamePortrait(E_MONSTER name){

    char num[5];
    char *ptr;

    snprintf(num, 5, "%4d", name);

    for(ptr = num; *ptr; ++ptr) if(0 == strncmp(ptr, " ", 1)) *ptr = '0';

    snprintf(monsterPortrait, 13, "MONH%4s.ICN", num);

    return monsterPortrait;
}

Uint8 GetMonsterGrown(E_MONSTER name){

    return ptrMonster[name].grown;
}

E_MONSTER GetUpgradeMonster(E_MONSTER name){

    E_MONSTER result = name;

    switch(name){
    
	case ARCHER:
	    result = RANGER;
	    break;

	case PIKEMAN:
	    result = VETERAN_PIKEMAN;
	    break;

	case SWORDSMAN:
	    result = MASTER_SWORDSMAN;
	    break;

	case CAVALIRY:
	    result = CHAMPION;
	    break;

	case PALADIN:
	    result = CRUSADER;
	    break;
	
	case ORC:
	    result = CHIEF_ORC;
	    break;

	case OGRE:
	    result = LORD_OGRE;
	    break;

	case TROLL:
	    result = WAR_TROLL;
	    break;

	case DWARF:
	    result = BATTLE_DWARF;
	    break;
	
	case ELF:
	    result = GRAND_ELF;
	    break;

	case DRUID:
	    result = GREATER_DRUID;
	    break;

	case ZOMBIE:
	    result = MUTANT_ZOMBIE;
	    break;
	
	case MUMMY:
	    result = ROYAL_MUMMY;
	    break;

	case VAMPIRE:
	    result = LORD_VAMPIRE;
	    break;

	case LICH:
	    result = POWER_LICH;
	    break;

	case MINOTAUR:
	    result = KNIGHT_MINOTAUR;
	    break;
	
	case GREEN_DRAGON:
	    result = RED_DRAGON;
	    break;

	case RED_DRAGON:
	    result = BLACK_DRAGON;
	    break;

	case IRON_GOLEM:
	    result = STEEL_GOLEM;
	    break;

	case MAGE:
	    result = ARCHMAGE;
	    break;

	case GIANT:
	    result = TITAN;
	    break;

	default:
	    break;
    }
    
    return result;
}

