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

#define LEVELEVENT_ROOT			0x00
#define LEVELEVENT_EDITPANEL		0x05
#define LEVELEVENT_SELECTCOLOR		0x06
#define LEVELEVENT_CASTLEUPDATEBUILD	0x07
#define LEVELEVENT_CASTLEINFOBUILD	0x08
#define LEVELEVENT_MARKETPLACE1		0x09
#define LEVELEVENT_MARKETPLACE2		0x0A
#define LEVELEVENT_MARKETTRADE		0x0B

typedef enum {

		NONE	=	0x00000000,
		YES	=	0x00000001,
		OK	=	0x00000002,
		NO	=	0x00000004,
		CANCEL	=	0x00000008,
		EXIT	=	0x00000010,
		ESC	=	0x00000020,
		DISMISS	=	0x00000040,
		UPGRADE	=	0x00000080,
		ENDTUR	=	0x00000100,
		MAX	=	0x00000200,
		UP	=	0x00000400,
		DOWN	=	0x00000800,
		SELECT	=	0x00001000,
		ENTER	=	0x00002000,
		LEFT	=	0x00004000,
		RIGHT	=	0x00008000

	    } ACTION;

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
ACTION ActionCycle(INTERFACEACTION *, void (*pf)());

ACTION ActionPressYES(void);
ACTION ActionPressOK(void);
ACTION ActionPressNO(void);
ACTION ActionPressCANCEL(void);
ACTION ActionPressEXIT(void);
ACTION ActionPressESC(void);
ACTION ActionPressDISMISS(void);
ACTION ActionPressUPGRADE(void);
ACTION ActionPressMAX(void);
ACTION ActionPressUP(void);
ACTION ActionPressDOWN(void);
ACTION ActionPressSELECT(void);
ACTION ActionPressLEFT(void);
ACTION ActionPressRIGHT(void);

#endif
