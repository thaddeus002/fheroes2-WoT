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

#ifndef _GAMEDEFS_H
#define _GAMEDEFS_H

#define VERSION 20060609

typedef enum { FALSE=0, TRUE=1 } BOOL;

#define COLORKEY	0xFF
#define COLORCHANGE	0xFE

#define RADARCOLOR	0x10

#define RADARWIDTH      144
#define BORDERWIDTH     16 
#define TILEWIDTH       32

typedef enum {
		MAPS_ALL=0, 
		MAPS_SMALL=36, 
		MAPS_MEDIUM=72, 
		MAPS_LARGE=108, 
		MAPS_XLARGE=144

	    } SIZEMAP;

typedef enum {
		SPEED_CRAWLING=1, 
		SPEED_VERYSLOW=2, 
		SPEED_SLOW=3, 
		SPEED_AVERAGE=4, 
		SPEED_FAST=5, 
		SPEED_VERYFAST=6, 
		SPEED_ULTRAFAST=7, 
		SPEED_BLAZING=8, 
		SPEED_INSTANT=9
		
	    } ARMYSPEED;

typedef enum {
		// 1 - 4
		SIZE_FEW=1, 
		// 5 - 9
		SIZE_SEVERAL=5, 
		// 10 - 19
		SIZE_PACK=10, 
		// 20 - 49
		SIZE_LOTS=20, 
		// 50 - 99
		SIZE_HORDE=50, 
		// 100 - 249
		SIZE_THRONG=100, 
		// 250 - 499
		SIZE_SWARM=250, 
		// 500 - 999
		SIZE_ZOUNDS=500, 
		// +1000
		SIZE_LEGION=1000
		
	    } ARMYSIZE;

typedef enum {
		LUCK_AWFUL=-2, 
		LUCK_BAD=-1, 
		LUCK_NORMAL=0, 
		LUCK_GOOD=1, 
		LUCK_GREAT=2, 
		LUCK_IRISH=3
		
	    } LUCKLEVEL;

typedef enum {
		MORALE_TREASON=-2, 
		MORALE_AWFUL=-2, 
		MORALE_POOR=-1, 
		MORALE_NORMAL=0, 
		MORALE_GOOD=1, 
		MORALE_GREAT=2, 
		MORALE_IRISH=3
		
	    } MORALELEVEL;

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

	    } SECONDARYSKILL;

typedef enum {
		BARBARIAN, 
		KNIGHT, 
		NECROMANCER, 
		SORCERESS, 
		WARLOCK, 
		WIZARD

	    } RACEHEROES;

typedef enum {
		DESERT, 
		SNOW, 
		SWAMP, 
		CRACKED, 
		BEACH, 
		LAVA, 
		DIRT, 
		GRASS, 
		WATER, 
		ROAD

	    } TYPESURFACE;


#endif
