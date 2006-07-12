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

#ifndef _ACTIONEVENT_H
#define _ACTIONEVENT_H

#include "SDL.h"
#include "agg.h"

#define CYCLEDELAY 1

#define LEVELEVENT_ROOT		0x00

#define LEVELEVENT_EDITPANEL	0x05

typedef enum {NONE=0, YES=1, OK=2, NO=3, CANCEL=4, EXIT=5, ESC=6} ACTION;

typedef struct{
    SDL_Rect    pushRect;
    SDL_Rect    presRect;
    AGGSPRITE   object;
    BOOL        flagPres;
    BOOL        flagPush;
} S_OLDOBJECT;

typedef struct {
    AGGSPRITE	objectPush;		// спрайт нажатия
    AGGSPRITE	objectMotion;		// спрайт наведения
    AGGSPRITE	objectUp;		// спрайт отжатия
    Uint16	mouseEvent;		// реагируем на MOUSE_RCLICK MOUSE_LCLICK MOUSE_UWHEEL MOUSE_DWHEEL или MOUSE_PRESENT
    ACTION	(*pf)();		// функция действия, если возвращает 1 то выход из цикла
    SDL_Rect	rect;			// квадрат действия
    Uint8	level;			// для контроля уровеней вложенности
    void	*next;
} INTERFACEACTION;

void AddActionEvent(INTERFACEACTION **, INTERFACEACTION *);
void RemoveActionLevelEvent(INTERFACEACTION *, Uint8);
void FreeActionEvent(INTERFACEACTION *);
void ZeroINTERFACEACTION(INTERFACEACTION *);
ACTION ActionCycle(INTERFACEACTION *);

ACTION ActionPressCancel(void);

#endif
