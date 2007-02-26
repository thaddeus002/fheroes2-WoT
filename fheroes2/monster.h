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

#include <string>
#include "sprite.h"
#include "gamedefs.h"

namespace Monster
{
    typedef enum {
	CRAWLING,
        VERYSLOW,
        SLOW,
        AVERAGE,
        FAST,
        VERYFAST,
        ULTRAFAST,
        BLAZING,
        INSTANT
    } speed_t;

    typedef enum {
	PEASANT,
	ARCHER,
	RANGER,
	PIKEMAN,
	VETERAN_PIKEMAN,
	SWORDSMAN,
	MASTER_SWORDSMAN,
	CAVALRY,
	CHAMPION,
	PALADIN,
	CRUSADER,
	GOBLIN,
	ORC,
	CHIEF_ORC,
	WOLF,
	OGRE,
	LORD_OGRE,
	TROLL,
	WAR_TROLL,
	CYCLOPS,
	SPRITE,
	DWARF,
	BATTLE_DWARF,
	ELF,
	GRAND_ELF,
	DRUID,
	GREATER_DRUID,
	UNICORN,
	PHOENIX,
	CENTAUR,
	GARGOYLE,
	GRIFFIN,
	MINOTAUR,
	KNIGHT_MINOTAUR,
	HIDRA,
	GREEN_DRAGON,
	RED_DRAGON,
	BLACK_DRAGON,
	HALFLING,
	BOAR,
	IRON_GOLEM,
	STEEL_GOLEM,
	ROC,
	MAGE,
	ARCHMAGE,
	GIANT,
	TITAN,
	SKELETON,
	ZOMBIE,
	MUTANT_ZOMBIE,
	MUMMY,
	ROYAL_MUMMY,
	VAMPIRE,
	LORD_VAMPIRE,
	LICH,
	POWER_LICH,
	BONE_DRAGON,

	ROGUE,
	NOMAD,
	GHOST,
	GENIE,
	MEDUSA,
	EARTH_ELEMENT,
	AIR_ELEMENT,
	FIRE_ELEMENT,
	WATER_ELEMENT,

	MONSTER_RND1,
	MONSTER_RND2,
	MONSTER_RND3,
	MONSTER_RND4,
	MONSTER_RND,
    } monster_t;

    typedef struct {
	u8 attack;
	u8 defence;
	u8 shots;
	u8 damageMin;
	u8 damageMax;
	u8 hp;
	speed_t speed;
	u8 grown;
	u8 cost;
    } stats_t;


    const std::string & String(monster_t monster);

    //const Sprite & GetSprite(monster_t monster);

};

#endif
