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

#ifndef _HEROES_H
#define _HEROES_H

#include "SDL.h"
#include "gamedefs.h"

#define HEROESMAXSKILL		8
#define HEROESMAXARTIFACT	14
#define HEROESMAXARMY		5

typedef enum {
		// увеличивает урон стрелков 10%
		ARCHERY_BASIC,
		// 25%
		ARCHERY_ADVANCED,
		// 50%
		ARCHERY_EXPERT,
		// увеличивает вероятность катапульты
		BALLISTICS_BASIC,
		// стреляет 2 раза
		BALLISTICS_ADVANCED,
		// стреляет 2 раза + пробивает стену спервого раза
		BALLISTICS_EXPERT,
		// присоединение нейтралов
		DIPLOMACY_BASIC,
		// присоединение нейтралов
		DIPLOMACY_ADVANCED,
		// присоединение нейтралов
		DIPLOMACY_EXPERT,
		// 20% 2 уровня
		EAGLEEYE_BASIC,
		// 30% 3 уровня
		EAGLEEYE_ADVANCED,
		// 40% 4 уровня
		EAGLEEYE_EXPERT,
		// 100 gold
		ESTATES_BASIC,
		// 250 gold
		ESTATES_ADVANCED,
		// 500 gold
		ESTATES_EXPERT,
		// +1 мораль
		LEADERSHIP_BASIC,
		// +2 мораль
		LEADERSHIP_ADVANCED,
		// +3 мораль
		LEADERSHIP_EXPERT,
		// увеличивает число ходов TP
		LOGISTICS_BASIC,
		// увеличивает число ходов TP
		LOGISTICS_ADVANCED,
		// увеличивает число ходов TP
		LOGISTICS_EXPERT,
		// +1 удачи
		LUCK_BASIC,
		// +2 удачи
		LUCK_ADVANCED,
		// +3 удачи
		LUCK_EXPERT,
		// +1 MP
		MYSTICISM_BASIC,
		// +2 MP
		MYSTICISM_ADVANCED,
		// +3 MP
		MYSTICISM_EXPERT,
		// увеличивает число ходов на море TP
		NAVIGATION_BASIC,
		// увеличивает число ходов на море TP
		NAVIGATION_ADVANCED,
		// увеличивает число ходов на море TP
		NAVIGATION_EXPERT,
		//
		NECROMANCY_BASIC,
		//
		NECROMANCY_ADVANCED,
		//
		NECROMANCY_EXPERT,
		// увеличивает скорость прохождения
		PATHFINDING_BASIC,
		// увеличивает скорость прохождения
		PATHFINDING_ADVANCED,
		// увеличивает скорость прохождения
		PATHFINDING_EXPERT,
		// +1 к обзору
		SCOUTING_BASIC,
		// +2 к обзору
		SCOUTING_ADVANCED,
		// +3 к обзору
		SCOUTING_EXPERT,
		// 3 magic level
		WISDOM_BASIC,
		// 4 magic level
		WISDOM_ADVANCED,
		// 5 magic level
		WISDOM_EXPERT

	    } E_SKILL;

typedef enum {
		BARBARIAN, 
		KNIGHT, 
		NECROMANCER, 
		SORCERESS, 
		WARLOCK, 
		WIZARD

	    } E_RACE;

typedef enum {
		SPREAD,
		GROUPED
	    } E_ARMYFORMAT;

typedef struct { 
    E_RACE              race;
    Uint8               attack;
    Uint8               defence;
    Uint8               spell;
    Uint8               knowledge;
    E_MORALE            morale;
    E_LUCK              luck;
    Uint32              experience;
    Uint16              mp;				// magic point
    Uint8               movement;
    Uint8               cf;				// ballista count fire
    E_SKILL		skill[HEROESMAXSKILL];
    E_ARTIFACT		artifact[HEROESMAXARTIFACT];
    S_ARMY		army[HEROESMAXARMY];
    E_ARMYFORMAT	af;				// army format
} S_HEROES;

#endif
