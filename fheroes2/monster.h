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
#ifndef H2MONSTER_H
#define H2MONSTER_H

#include <vector>
#include <string>
#include "mp2.h"
#include "maps_tiles.h"
#include "race.h"
#include "speed.h"
#include "gamedefs.h"

namespace Monster
{
    typedef enum {
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4
    } level_t;

    typedef enum {
	PEASANT		= 0,
	ARCHER		= 1,
	RANGER		= 2,
	PIKEMAN		= 3,
	VETERAN_PIKEMAN	= 4,
	SWORDSMAN	= 5,
	MASTER_SWORDSMAN= 6,
	CAVALRY		= 7,
	CHAMPION	= 8,
	PALADIN		= 9,
	CRUSADER	= 10,
	GOBLIN		= 11,
	ORC		= 12,
	CHIEF_ORC	= 13,
	WOLF		= 14,
	OGRE		= 15,
	LORD_OGRE	= 16,
	TROLL		= 17,
	WAR_TROLL	= 18,
	CYCLOPS		= 19,
	SPRITE		= 20,
	DWARF		= 21,
	BATTLE_DWARF	= 22,
	ELF		= 23,
	GRAND_ELF	= 24,
	DRUID		= 25,
	GREATER_DRUID	= 26,
	UNICORN		= 27,
	PHOENIX		= 28,
	CENTAUR		= 29,
	GARGOYLE	= 30,
	GRIFFIN		= 31,
	MINOTAUR	= 32,
	KNIGHT_MINOTAUR	= 33,
	HYDRA		= 34,
	GREEN_DRAGON	= 35,
	RED_DRAGON	= 36,
	BLACK_DRAGON	= 37,
	HALFLING	= 38,
	BOAR		= 39,
	IRON_GOLEM	= 40,
	STEEL_GOLEM	= 41,
	ROC		= 42,
	MAGE		= 43,
	ARCHMAGE	= 44,
	GIANT		= 45,
	TITAN		= 46,
	SKELETON	= 47,
	ZOMBIE		= 48,
	MUTANT_ZOMBIE	= 49,
	MUMMY		= 50,
	ROYAL_MUMMY	= 51,
	VAMPIRE		= 52,
	LORD_VAMPIRE	= 53,
	LICH		= 54,
	POWER_LICH	= 55,
	BONE_DRAGON	= 56,

	ROGUE		= 57,
	NOMAD		= 58,
	GHOST		= 59,
	GENIE		= 60,
	MEDUSA		= 61,
	EARTH_ELEMENT	= 62,
	AIR_ELEMENT	= 63,
	FIRE_ELEMENT	= 64,
	WATER_ELEMENT	= 65,

	UNKNOWN		= 66,

	MONSTER_RND1,
	MONSTER_RND2,
	MONSTER_RND3,
	MONSTER_RND4,
	MONSTER_RND,

    } monster_t;

    typedef struct {
	monster_t monster;
	level_t level;
	u8 attack;
	u8 defence;
	u8 shots;
	u8 damageMin;
	u8 damageMax;
	u16 hp;
	Speed::speed_t speed;
	u8 grown;
	const std::string name;
	const std::string file;
    } stats_t;

    const std::string & String(monster_t monster);
    const stats_t & GetStats(monster_t monster);
    u8 GetGrown(monster_t monster);
    Race::race_t GetRace(monster_t monster);

    monster_t Upgrade(monster_t monster);
    bool AllowUpgrade(monster_t monster);

    monster_t Monster(u8 num);
    monster_t Monster(Race::race_t race, u32 dwelling);

    monster_t Rand(void);
    monster_t Rand1(void);
    monster_t Rand2(void);
    monster_t Rand3(void);
    monster_t Rand4(void);

    u16 RandCount(monster_t monster);
    
    void ChangeTileWithRNDMonster(std::vector<Maps::Tiles *> & vector, u16 center);
};

#endif
