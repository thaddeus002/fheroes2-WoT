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

#include "SDL.h"

#define VERSION 20060803

typedef enum { FALSE=0, TRUE=1 } BOOL;

#define COLORKEY	0xFF
#define COLORCHANGE	0xFE

#define RADARCOLOR	0x10
#define FOCUSCOLOR	0x40

#define RADARWIDTH      144
#define BORDERWIDTH     16
#define TILEWIDTH       32

#define COLORBLUE	0x01
#define COLORGREEN	0x02
#define COLORRED	0x04
#define COLORYELLOW	0x08
#define COLORORANGE	0x10
#define COLORPURPLE	0x20
#define COLORGRAY	0xFF

#define BUY_CYCLOPS_CRYSTAL     1
#define BUY_PHOENIX_MERCURY     1
#define BUY_GREENDRAGON_SULFUR  1
#define BUY_REDDRAGON_SULFUR    1
#define BUY_BLACKDRAGON_SULFUR  3
#define BUY_GIANT_GEMS          1
#define BUY_TITAN_GEMS          2
#define BUY_GENIE_GEMS          1

#define GOLD_CASTLE_DAY		1000
#define GOLD_TOWN_DAY		750
#define GOLD_STATUE_DAY		250
#define GOLD_DUNGEON_DAY	500

typedef enum {
		GAME_EASY,
		GAME_NORMAL,
		GAME_HARD,
		GAME_EXPERT,
		GAME_IMPOSSIBLE

	    } E_GAMELEVEL;

typedef enum {
        	BLUE,
        	GREEN,
        	RED,
        	YELLOW,
        	ORANGE,
        	PURPLE,
		GRAY,

            } E_COLORS;

typedef enum {
                KNIGHT,
                BARBARIAN,
                SORCERESS,
                WARLOCK,
                WIZARD,
                NECROMANCER,
                BOMG

            } E_RACE;

typedef enum {
		MAPS_ALL	= 0, 
		MAPS_SMALL	= 36, 
		MAPS_MEDIUM	= 72, 
		MAPS_LARGE	= 108, 
		MAPS_XLARGE	= 144

	    } E_SIZEMAP;

typedef enum {
                LUCK_AWFUL      = -2,
                LUCK_BAD        = -1,
                LUCK_NORMAL     = 0,
                LUCK_GOOD       = 1,
                LUCK_GREAT      = 2,
                LUCK_IRISH      = 3
                
            } E_LUCK;
                
typedef enum {
                MORALE_TREASON  = -3,
                MORALE_AWFUL    = -2,
                MORALE_POOR     = -1,
                MORALE_NORMAL   = 0,
                MORALE_GOOD     = 1,
                MORALE_GREAT    = 2,
                MORALE_IRISH    = 3
                
            } E_MORALE;
                                                                                                                                                                                                            
#endif
