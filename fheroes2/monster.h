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

#ifndef _MONSTER_H
#define _MONSTER_H

#include "SDL.h"
#include "gamedefs.h"
#include "castle.h"

#define RNDMONSTERMAXCOUNT	4000

#define	MONSTERMAXCOUNT		66
#define	MONSTERCOUNT_LEVEL1	10
#define	MONSTERCOUNT_LEVEL2	15
#define	MONSTERCOUNT_LEVEL3	27
#define	MONSTERCOUNT_LEVEL4	14

typedef enum {
		MNS_ALL		= 0xAF,
		MNS_LEVEL1	= 0xB3,
		MNS_LEVEL2	= 0xB4,
		MNS_LEVEL3	= 0xB5,
		MNS_LEVEL4	= 0xB6
	    } E_LEVELMONSTER;

typedef enum {
                FEW		= 1,                // 1 - 4
                SEVERAL		= 5,                // 5 - 9
                PACK		= 10,               // 10 - 19
                LOTS		= 20,               // 20 - 49
                HORDE		= 50,               // 50 - 99
                THRONG		= 100,              // 100 - 249
                SWARM		= 250,              // 250 - 499
                ZOUNDS		= 500,              // 500 - 999
                LEGION		= 1000              // 1000+

            } E_ARMYSIZE;

typedef enum {
                CRAWLING 	 = 1,
                VERYSLOW 	 = 2,
                SLOW     	 = 3,
                AVERAGE  	 = 4,
                FAST     	 = 5,
                VERYFAST 	 = 6,
                ULTRAFAST	 = 7,
                BLAZING  	 = 8,
                INSTANT  	 = 9

            } E_MONSTERSPEED;

typedef enum {
		PEASANT		= 0,
		ARCHER		= 1,
		RANGER		= 2,
		PIKEMAN		= 3,
		VETERAN_PIKEMAN = 4,
		SWORDSMAN	= 5,
		MASTER_SWORDSMAN = 6,
		CAVALIRY	= 7,
		CHAMPION	= 8,
		PALADIN		= 9,
		CRUSADER	= 10,
		GOBLIN 		= 11,
		ORC 		= 12,
		CHIEF_ORC 	= 13,
		WOLF 		= 14,
		OGRE 		= 15,
		LORD_OGRE 	= 16,
		TROLL 		= 17,
		WAR_TROLL 	= 18,
		CYCLOPS 	= 19,
		SPRITE 		= 20,
		DWARF 		= 21,
		BATTLE_DWARF 	= 22,
		ELF 		= 23,
		GRAND_ELF 	= 24,
		DRUID 		= 25,
		GREATER_DRUID	= 26,
		UNICORN 	= 27,
		PHOENIX 	= 28,
		CENTAUR 	= 29,
		GARGOYLE 	= 30,
		GRIFFIN 	= 31,
		MINOTAUR 	= 32,
		KNIGHT_MINOTAUR = 33,
		HIDRA 		= 34,
		GREEN_DRAGON 	= 35,
		RED_DRAGON 	= 36,
		BLACK_DRAGON 	= 37,
		HALFLING 	= 38,
		BOAR 		= 39,
		IRON_GOLEM 	= 40,
		STEEL_GOLEM 	= 41,
		ROC 		= 42,
		MAGE 		= 43,
		ARCHMAGE 	= 44,
		GIANT 		= 45,
		TITAN 		= 46,
		SKELETON 	= 47,
		ZOMBIE 		= 48,
		MUTANT_ZOMBIE 	= 49,
		MUMMY 		= 50,
		ROYAL_MUMMY 	= 51,
		VAMPIRE 	= 52,
		LORD_VAMPIRE 	= 53,
		LICH 		= 54,
		POWER_LICH 	= 55,
		BONE_DRAGON 	= 56,
		ROGUE 		= 57,
		NOMAD 		= 58,
		GHOST 		= 59,
		GENIE 		= 60,
		MEDUSA 		= 61,
		EARTH_ELEMENT 	= 62,
		AIR_ELEMENT 	= 63,
		FIRE_ELEMENT 	= 64,
		WATER_ELEMENT 	= 65,

		MONSTER_RND1 	= 66,
		MONSTER_RND2 	= 67,
		MONSTER_RND3 	= 68,
		MONSTER_RND4 	= 69,
		MONSTER_RND 	= 70,

		MONSTERNONE
	     } E_MONSTER;

typedef struct {
    Uint8       	attack;
    Uint8       	defence;
    Uint8		shots;
    Uint8       	damageMin;
    Uint8       	damageMax;
    Uint16		hp;
    E_MONSTERSPEED	speed;
    Uint16		cost;
    const char *	descriptions;
	    } S_MONSTER;

typedef struct {
    Uint16		count;
    E_MONSTER		monster;
	    } S_ARMY;

BOOL		InitMonster(void);
void		FreeMonster(void);

S_MONSTER *	GetStatMonster(E_MONSTER);
E_ARMYSIZE 	GetSizeArmy(Uint16);
const char *    GetStringSizeArmy(E_ARMYSIZE);
E_MONSTER	GetRNDMonster(E_LEVELMONSTER);
E_RACE		GetRaceMonster(E_MONSTER);
BOOL		TheFlyMonster(E_MONSTER);
E_MONSTER	CheckValidMonster(Uint8);
Uint16		GetRNDSizeLevelMonster(E_LEVELMONSTER);
E_LEVELMONSTER	GetLevelMonster(E_MONSTER);


#endif
