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

#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "object.h"
#include "gamedefs.h"

#define RESOURCEMAXCOUNT	15
#define RESOURCEMINCOUNT	5

//#define OBJ_MINE_ORE            0x00 
//#define OBJ_MINE_SULFUR         0x01 
//#define OBJ_MINE_CRYSTAL        0x02 
//#define OBJ_MINE_GEMS           0x03 
//#define OBJ_MINE_GOLD           0x04 

typedef enum {
                WOOD		= 1,
                MERCURY		= 3,
                ORE		= 5,
                SULFUR		= 7,
                CRYSTAL		= 9,
                GEMS		= 11,
                GOLDS		= 13,
		LAMP		= 15,
		RNDRES		= 17,
		CHEST		= 19,
		RES_NONE	= 20
            } E_RESOURCE;

typedef struct {
		Uint8		ax;
		Uint8		ay;
		E_OBJECT	type;
		E_COLORS	color;
	    } S_BUILDING;

Uint16	    GetCountResource(E_RESOURCE);
E_RESOURCE  GetRNDResource(void);
E_RESOURCE  CheckValidResource(Uint8);
const char *GetDescriptionsResource(E_RESOURCE);

#endif
