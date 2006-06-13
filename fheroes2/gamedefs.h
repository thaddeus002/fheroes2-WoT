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

#define VERSION 20060613

typedef enum { FALSE=0, TRUE=1 } BOOL;

#define COLORKEY	0xFF
#define COLORCHANGE	0xFE

#define RADARCOLOR	0x10

#define RADARWIDTH      144
#define BORDERWIDTH     16 
#define TILEWIDTH       32

typedef enum {
	    BLUE,
	    RED,
	    GREEN,
	    YELLOW,
	    ORANGE,
	    PURPLE
	    } E_COLORS;

typedef enum {
		MAPS_ALL	= 0, 
		MAPS_SMALL	= 36, 
		MAPS_MEDIUM	= 72, 
		MAPS_LARGE	= 108, 
		MAPS_XLARGE	= 144

	    } E_SIZEMAP;

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

	    } E_SURFACE;
                
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
