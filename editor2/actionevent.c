#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "sound.h"
#include "draw.h"
#include "actionevent.h"
#include "cursor.h"
#include "config.h"

typedef struct{
    SDL_Rect    rect;
    AGGSPRITE   object;
    Uint8       use;
} OLDOBJECT;

void AddActionEvent(INTERFACEACTION **pointer, INTERFACEACTION *action){

    if(NULL == *pointer){
	*pointer = (INTERFACEACTION *) malloc(sizeof(INTERFACEACTION));
	**pointer = *action;
	(*pointer)->next = NULL;

	return;
    }
    
    INTERFACEACTION *ptr = *pointer;
    while(ptr->next) ptr = (INTERFACEACTION *) ptr->next;

    ptr->next = malloc(sizeof(INTERFACEACTION));
    if(ptr->next == NULL){
	fprintf(stderr, "error malloc: %d\n", sizeof(INTERFACEACTION));
        return;
    }
    ptr = (INTERFACEACTION *) ptr->next;
    *ptr = *action;
    ptr->next = NULL;
}

void RemoveActionLevelEvent(INTERFACEACTION *headAction, Uint8 level){

    INTERFACEACTION *head = headAction;

    if(head->level == level || level == LEVELEVENT_ROOT){
	fprintf(stderr, "First element ACTIONEVENT always only LEVELEVENT_ROOT, and not remove!\n");
	return;
    }

    INTERFACEACTION *tail;
    
    while(NULL != head->next){

	if(((INTERFACEACTION *) head->next)->level == level){

	    tail = (INTERFACEACTION *) ((INTERFACEACTION *) head->next)->next;

	    free(head->next);

	    head->next = tail;
	}else
	    head = (INTERFACEACTION *) head->next;
    }
}

void FreeActionEvent(INTERFACEACTION *headAction){

    INTERFACEACTION *ptr = headAction;

    while(ptr){
	headAction = (INTERFACEACTION *) headAction->next;
	free(ptr);
	ptr = headAction;
    }
    
    headAction = NULL;
}

void ZeroINTERFACEACTION(INTERFACEACTION *action){

    memset(action->objectPush.name, 0, AGGSIZENAME);
    action->objectPush.number = 0xFFFF;
    memset(action->objectMotion.name, 0, AGGSIZENAME);
    action->objectMotion.number = 0xFFFF;
    memset(action->objectUp.name, 0, AGGSIZENAME);
    action->objectUp.number = 0xFFFF;
    action->rect.x = 0;
    action->rect.y = 0;
    action->rect.w = 0;
    action->rect.h = 0;
    action->mouseEvent = MOUSE_NULL;
    action->cursorPush = CURSOR_NULL;
    action->cursorMotion = CURSOR_NULL;
    action->cursorUp = CURSOR_NULL;
    action->pf = NULL;
    action->level = LEVELEVENT_ROOT;
    action->next = NULL;
}

ACTION ActionPressCancel(void){
    
    return CANCEL;
}

ACTION ActionCycle(INTERFACEACTION *action){

    SDL_Event event;
    SDL_Surface *video = SDL_GetVideoSurface();;
    ACTION exit = NONE;
    OLDOBJECT old;
    memset(old.object.name, 0, AGGSIZENAME);
    old.object.number = 0xFFFF;
    old.rect.x = 0;
    old.rect.y = 0;
    old.rect.w = 0;
    old.rect.h = 0;
    old.use = 0;
    INTERFACEACTION *ptr = NULL;
    int mousex, mousey;

    // цикл по событиям
    while(exit == NONE){
	while(SDL_PollEvent(&event))

	    switch(event.type){
	    
		case SDL_QUIT:

		    // close windows
		    exit = EXIT;
		    break;

		case SDL_KEYDOWN:

		    // нажатия клавиатуры
		    switch(event.key.keysym.sym){

			case SDLK_ESCAPE:

			    exit = ESC;
			    break;

                	case SDLK_m:

			    fprintf(stderr, "%d\n", GetCurrentSizeMemory());
			    break;

			// F4 switch to full screen
                	case SDLK_F4:

			    SDL_WM_ToggleFullScreen(video);

			    if(GetIntValue("fullscreen")) SetIntValue("fullscreen", FALSE);
			    else SetIntValue("fullscreen", TRUE);

			    break;
								    
			default:
			    break;
		    }
		    break;
		
		case SDL_MOUSEBUTTONDOWN:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка down
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK)){
				    old.rect = ptr->rect;
				    old.object = ptr->objectUp;
				    old.use = 1;
				    DrawSprite(&ptr->rect, &ptr->objectPush);
				    SetCursor(ptr->cursorPush);
				}
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;
			
			case SDL_BUTTON_RIGHT:

			    // правая кнопка down
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_RCLICK) && ptr->pf) exit = (*ptr->pf)();
    
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
    			    fprintf(stderr, "x: %d, y: %d\n", event.motion.x, event.motion.y);
			    break;

			case SDL_BUTTON_WHEELUP:

			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_UWHEEL) && ptr->pf) exit = (*ptr->pf)();
		    
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			case SDL_BUTTON_WHEELDOWN:

			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_DWHEEL) && ptr->pf) exit = (*ptr->pf)();
		    
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONUP:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка up
			    ptr = action;
			    if(old.use){
				DrawSprite(&old.rect, &old.object);
				old.use = 0;
			    }

			    while(ptr){
				if(ValidPoint(&old.rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK)){

				    if(CompareRect(&ptr->rect, &old.rect) && ptr->pf)
					exit = (*ptr->pf)();
				    
				    SetCursor(ptr->cursorUp);
				}
				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;
			    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEMOTION:

		    // рисуем курсор
    		    CursorShow(event.motion.x, event.motion.y);
		    break;
		
		default:
    		    break;
	    }

	// Проверка на событие MOUSE_PRESENT
	SDL_GetMouseState(&mousex, &mousey);
	ptr = action;
	while(ptr){
	    if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, mousex, mousey)){
		DrawSprite(&ptr->rect, &ptr->objectMotion);
		SetCursor(ptr->cursorMotion);
		if(ptr->pf) exit = (*ptr->pf)();
	    }
    	    ptr = (INTERFACEACTION *) ptr->next;
	}

	if(CYCLEDELAY) SDL_Delay(CYCLEDELAY);
    }

    return exit;
}
