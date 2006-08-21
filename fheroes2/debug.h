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

#ifndef _DEBUG_H
#define _DEBUG_H

#include "SDL.h"
#include "object.h"
#include "mp2maps.h"
#include "monster.h"
#include "castle.h"

const char * GetStringObject(E_OBJECT);
const char * GetStringRace(E_RACE);
const char * GetStringColor(E_COLORS);
const char * GetStringLuck(E_LUCK);
const char * GetStringMorale(E_MORALE);
const char * GetStringSpeed(E_MONSTERSPEED);
const char * GetStringGround(E_GROUND);
const char * GetStringWel2(E_RACE);
const char * GetStringSpec(E_RACE);
const char * GetStringDwelling(E_RACE, E_DWELLINGCASTLE);

const char * GetStringMageGuild(void);
const char * GetStringLevelMageGuild(E_MAGICLEVEL level);
const char * GetStringTavern(void);
const char * GetStringThievesGuild(void);
const char * GetStringWell(void);
const char * GetStringShipyard(void);
const char * GetStringStatue(void);
const char * GetStringMarketplace(void);
const char * GetStringLeftTurret(void);
const char * GetStringRightTurret(void);
const char * GetStringMoat(void);
const char * GetStringCaptain(void);

void         PrintCellInfo(Uint16);

#endif
