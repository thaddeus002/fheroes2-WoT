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

void AddActionEvent(INTERFACEACTION **pointer, INTERFACEACTION *action){

    INTERFACEACTION *ptr;

    if(NULL == *pointer){
	if(NULL == (*pointer = (INTERFACEACTION *) malloc(sizeof(INTERFACEACTION)))){
	    fprintf(stderr, "AddActionEvent: error malloc: %d\n", sizeof(INTERFACEACTION));
    	    return;
	}
	ptr = *pointer;

    }else{
    
	ptr = *pointer;
	while(ptr->next) ptr = (INTERFACEACTION *) ptr->next;

	if(NULL == (ptr->next = malloc(sizeof(INTERFACEACTION)))){
	    fprintf(stderr, "AddActionEvent: error malloc: %d\n", sizeof(INTERFACEACTION));
    	    return;
	}

	ptr = (INTERFACEACTION *) ptr->next;
    }

    *ptr = *action;

    ptr->next = NULL;
}

void RemoveActionLevelEvent(INTERFACEACTION *headAction, Uint8 level){

    INTERFACEACTION *head = headAction;

    if(head->level == level || level == LEVELEVENT_ROOT){
	fprintf(stderr, "RemoveActionLevelEvent: First element ACTIONEVENT always only LEVELEVENT_ROOT, and not remove!\n");
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
    action->pf = NULL;
    action->level = LEVELEVENT_ROOT;
    action->next = NULL;
}

ACTION ActionPressCANCEL(void){
    
    return CANCEL;
}

ACTION ActionPressYES(void){
    
    return YES;
}

ACTION ActionPressOK(void){
    
    return OK;
}

ACTION ActionPressNO(void){
    
    return NO;
}

ACTION ActionPressLEFT(void){
    
    return LEFT;
}

ACTION ActionPressRIGHT(void){
    
    return RIGHT;
}

ACTION ActionPressEXIT(void){
    
    return EXIT;
}

ACTION ActionPressESC(void){
    
    return ESC;
}

ACTION ActionPressDISMISS(void){
    
    return DISMISS;
}

ACTION ActionPressUPGRADE(void){
    
    return UPGRADE;
}

ACTION ActionPressMAX(void){
    
    return MAX;
}

ACTION ActionPressUP(void){
    
    return UP;
}

ACTION ActionPressDOWN(void){
    
    return DOWN;
}

ACTION ActionPressSELECT(void){
    
    return SELECT;
}

ACTION ActionCycle(INTERFACEACTION *action, void (*pfanim)()){

    SDL_Event event;
    SDL_Surface *video = SDL_GetVideoSurface();;
    ACTION exit = NONE;
    S_OLDOBJECT old;
    memset(old.object.name, 0, AGGSIZENAME);
    old.object.number = 0xFFFF;
    old.pushRect.x = 0;
    old.pushRect.y = 0;
    old.pushRect.w = 0;
    old.pushRect.h = 0;
    old.presRect = old.pushRect;
    old.flagPush = FALSE;
    old.flagPres = FALSE;
    INTERFACEACTION *ptr = NULL;

    Uint32 ticket = 0;

    // цикл по событиям
    while(exit == NONE){
	while(SDL_PollEvent(&event)){

	    switch(event.type){
	    
		case SDL_QUIT:

		    // close windows
		    exit = ActionPressEXIT();
		    break;

		case SDL_KEYDOWN:

		    // нажатия клавиатуры
		    switch(event.key.keysym.sym){

			case SDLK_ESCAPE:

			    exit = ActionPressESC();
			    break;

			case SDLK_RETURN:

			    exit = ENTER;
			    break;

			// F4 switch to full screen
                	case SDLK_F4:

			    SDL_WM_ToggleFullScreen(video);

			    GetIntValue(FULLSCREEN) ? SetIntValue(FULLSCREEN, FALSE) : SetIntValue(FULLSCREEN, TRUE);

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
				    old.pushRect = ptr->rect;
				    old.object = ptr->objectUp;
				    old.flagPush = TRUE;
				    DrawSprite(&ptr->rect, &ptr->objectPush);
				}
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			case SDL_BUTTON_RIGHT:
			    
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_RCLICK) && ptr->pf) exit = (*ptr->pf)();
		    
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    if(GetIntValue(DEBUG)) fprintf(stderr, "x: %d, y: %d\n", event.button.x, event.button.y);
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
			    if(old.flagPush){
				DrawSprite(&old.pushRect, &old.object);
				old.flagPush = FALSE;
			    }

			    while(ptr){
				if(ValidPoint(&old.pushRect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK) && CompareRect(&ptr->rect, &old.pushRect) && ptr->pf )
					exit = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;
			    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEMOTION:
		
		    CursorShow();
		    break;

		default:
    		    break;
	    }

            if(exit != EXIT && 0 == ticket % (GetIntValue(ANIMATIONDELAY) / 2) && pfanim) (*pfanim)();
            ++ticket;
        }

	if(0 == ticket % GetIntValue(ANIMATIONDELAY) && pfanim) (*pfanim)();
        else if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);

        ++ticket;
    }

    return exit;
}
