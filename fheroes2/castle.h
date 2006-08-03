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

#ifndef _CASTLE_H
#define _CASTLE_H

#include "SDL.h"
#include "gamedefs.h"
#include "heroes.h"
#include "actionevent.h"
#include "monster.h"

#define CASTLEMAXARMY		5
#define CASTLEMAXMONSTER	6

typedef enum {
		BUILD_THIEVEGUILD	= 0x0002,
		BUILD_TAVERN		= 0x0004,
		BUILD_SHIPYARD		= 0x0008,
		BUILD_WELL		= 0x0010,
		BUILD_STATUE		= 0x0080,
		BUILD_LEFTTURRET	= 0x0100,
		BUILD_RIGHTTURRET	= 0x0200,
		BUILD_MARKETPLACE	= 0x0400,
		BUILD_MOAT		= 0x1000,
		BUILD_WEL2		= 0x0800,	// Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
		BUILD_SPEC		= 0x2000	// Fortification, Coliseum, Rainbow, Dungeon, Library, Storm

	    } E_BUILDINGCASTLE;

typedef enum {
                DWELLING_MONSTER1       = 0x0008,
                DWELLING_MONSTER2       = 0x0010,
                DWELLING_MONSTER3       = 0x0020,
                DWELLING_MONSTER4       = 0x0040,
                DWELLING_MONSTER5       = 0x0080,
                DWELLING_MONSTER6       = 0x0100,
                DWELLING_UPGRADE2       = 0x0200,
        	DWELLING_UPGRADE3       = 0x0400,
                DWELLING_UPGRADE4       = 0x0800,
                DWELLING_UPGRADE5       = 0x1000,
                DWELLING_UPGRADE6       = 0x2000,
                DWELLING_UPGRADE7       = 0x4000        // black dragon

            } E_DWELLINGCASTLE;
                                                                                                                                                                                                            
typedef struct {

    E_COLORS	color;
    E_RACE	race;
    char	name[13];
    Uint8	ax;
    Uint8	ay;
    SDL_Rect	pos;
    Uint16	building;
    Uint16	dwelling;
    Uint8	magicTower;
    Uint16	monster[CASTLEMAXMONSTER];
    S_ARMY	army[CASTLEMAXARMY];
    BOOL	capitan;
    BOOL	castle;
    BOOL	allowCastle;
    void	*next;

} S_CASTLE;

BOOL		AddCastle(FILE *, Uint8, Uint8, Uint8);
void		FreeCastle(void);
E_RACE		GetRaceRNDCastle(Uint8, Uint8);
S_CASTLE	*GetStatCastlePos(Uint8, Uint8);
S_CASTLE	*GetStatCastle(Uint8);
const S_CASTLE	*GetCurrentCastle(void);

void		EnterCastle(Uint8, Uint8, E_NAMEHEROES);

S_CASTLE	*GetFirstCastle(E_COLORS);
S_CASTLE 	*GetNextCastle(E_COLORS);
S_CASTLE 	*GetPrevCastle(E_COLORS);
S_CASTLE 	*GetEndCastle(E_COLORS);
Uint8     	GetCountCastle(E_COLORS);
E_MONSTER	GetMonsterFromCastle(const S_CASTLE *, Uint8);
BOOL		BuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);
BOOL		CheckBuyMonsterFromCastle(const S_CASTLE *, Uint8, Uint16);

void		RedrawCastleMonster(void);
void		RedrawCastleInfoResource(void);

#endif
