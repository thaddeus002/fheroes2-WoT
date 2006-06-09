#ifndef _ACTIONEVENT_H
#define _ACTIONEVENT_H

#include "SDL.h"
#include "agg.h"

#define CYCLEDELAY 1

#define LEVELEVENT_ROOT		0x00

#define LEVELEVENT_EDITPANEL	0x05

typedef enum {NONE=0, YES=1, OK=2, NO=3, CANCEL=4, EXIT=5, ESC=6} ACTION;

typedef struct {
    AGGSPRITE	objectPush;		// спрайт нажатия
    AGGSPRITE	objectMotion;		// спрайт наведения
    AGGSPRITE	objectUp;		// спрайт отжатия
    Uint16	mouseEvent;		// реагируем на MOUSE_RCLICK MOUSE_LCLICK MOUSE_UWHEEL MOUSE_DWHEEL или MOUSE_PRESENT
    Uint32	cursorPush;		// имя курсора после нажатия
    Uint32	cursorMotion;		// имя курсора при наведении
    Uint32	cursorUp;		// имя курсора после отжатия
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
