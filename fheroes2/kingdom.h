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

#ifndef _KINGDOM_H
#define _KINGDOM_H

#include "SDL.h"
#include "gamedefs.h"
#include "resource.h"
#include "heroes.h"
#include "object.h"
#include "castle.h"

#define KINGDOMMAXHEROES	8
#define KINGDOMMAX		6

// объекты которые которые можно захватить (с флажками)
typedef enum {

		ALCHEMYTOWER	= OBJ_ALCHEMYTOWER,
		DRAGONCITY	= OBJ_DRAGONCITY,
		LIGHTHOUSE	= OBJ_LIGHTHOUSE,
		MINES		= OBJ_MINES,
		SAWMILL		= OBJ_SAWMILL,
		ABANDONEDMINE	= OBJ_ABANDONEDMINE,

	    } E_COLOROBJECT;

typedef struct {

		Uint8		ax;
		Uint8		ay;
		E_COLOROBJECT	type;
		E_RESOURCE	res;
		void		*object;

	    } S_BUILDING;
 
typedef struct {

		BOOL		play;
		S_BUILDING	*build;
		S_CASTLE	*castle;
		E_NAMEHEROES	nameheroes[KINGDOMMAXHEROES];

	    } S_KINGDOM;

BOOL    InitKingdom(void);
void	FreeKingdom(void);

void	KingdomAddHeroes(E_COLORS, E_NAMEHEROES);
void	KingdomRemoveHeroes(E_COLORS, E_NAMEHEROES);

void	KingdomAddCastle(E_COLORS, S_CASTLE *);
void	KingdomRemoveCastle(E_COLORS, S_CASTLE *);

//void	KingdomAddBuilding(Uint8, Uint8, E_COLOROBJECT);
//void	KingdomRemoveBuilding(Uint8, Uint8);

#endif
