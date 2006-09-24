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
#include "gamedefs.h"

#include "config.h"
#include "cursor.h"
#include "draw.h"
#include "tools.h"
#include "heroes.h"
#include "spell.h"

#include "magicbook.h"

#define STRLEN	42

ACTION	ActionBookOverPointInfo(void);
ACTION	ActionBookOverAdventureSpell(void);
ACTION	ActionBookOverCombatSpell(void);
ACTION	ActionBookOverNextPage(void);
ACTION	ActionBookOverPrevPage(void);
ACTION	ActionBookOverClose(void);

ACTION	ActionBookInfoLOOP(INTERFACEACTION *);
void	PrintBookInfoPrompt(const char *);


const	S_HEROES *currentHero = NULL;

ACTION  ShowMagicBook(const S_HEROES *hero){

    if(! hero) return NONE;
    currentHero = hero;

    SetIntValue(ANIM2, FALSE);

    SDL_Surface *back, *leftList, *rightList, *image, *video;
    SDL_Rect rectBack, rect;
    Uint16 cx, cy;
    AGGSPRITE sprite;
    BOOL exit = FALSE;

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;
    ACTION result = NONE;

    video = SDL_GetVideoSurface();

    CursorOff();

    // правый лист книги
    FillSPRITE(&sprite, "BOOK.ICN", 0);
    rightList = GetICNSprite(&sprite);
   // отрисовка диалога по центру экрана
    rectBack.x = (video->w - 2 * rightList->w) / 2;
    rectBack.y = (video->h - rightList->h) / 2;
    rectBack.w = 2 * rightList->w;
    rectBack.h = rightList->h + 50; // увеличили на высоту закладки
    cx = rectBack.x;
    cy = rectBack.y;

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "ShowMagicBook: CreateRGBSurface failed: %s\n", SDL_GetError());
        return NONE;
    }
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // правый лист книги
    rect.x = cx + rightList->w;
    rect.y = cy;
    rect.w = rightList->w;
    rect.h = rightList->h;
    SDL_BlitSurface(rightList, NULL, video, &rect);

    // левый лист книги
    if(NULL == (leftList = CopySurface(rightList, FLIP_HORIZONTAL))) return NONE;
    rect.x = cx;
    rect.y = cy;
    rect.w = leftList->w;
    rect.h = leftList->h;
    SDL_BlitSurface(leftList, NULL, video, &rect);

    // закладка info spell points
    FillSPRITE(&sprite, "BOOK.ICN", 6);
    image = GetICNSprite(&sprite);
    rect.x = cx + 125;
    rect.y = cy + 275;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverPointInfo;
    AddActionEvent(&dialog, &action);

    // закладка adventure spell
    FillSPRITE(&sprite, "BOOK.ICN", 3);
    image = GetICNSprite(&sprite);
    rect.x = cx + 270;
    rect.y = cy + 270;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverAdventureSpell;
    AddActionEvent(&dialog, &action);

    // закладка combat spell
    FillSPRITE(&sprite, "BOOK.ICN", 4);
    image = GetICNSprite(&sprite);
    rect.x = cx + 304;
    rect.y = cy + 278;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverCombatSpell;
    AddActionEvent(&dialog, &action);

    // закладка close
    FillSPRITE(&sprite, "BOOK.ICN", 5);
    image = GetICNSprite(&sprite);
    rect.x = cx + 420;
    rect.y = cy + 284;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverClose;
    AddActionEvent(&dialog, &action);

    // prev
    rect.x = cx + 30;
    rect.y = cy + 8;
    rect.w = 30;
    rect.h = 25;
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverPrevPage;
    AddActionEvent(&dialog, &action);

    // next
    rect.x = cx + 410;
    rect.y = cy + 8;
    rect.w = 30;
    rect.h = 25;
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionBookOverNextPage;
    AddActionEvent(&dialog, &action);
    
    PrintBookInfoPrompt(NULL);
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionBookInfoLOOP(dialog)){

            case EXIT:
                exit = TRUE;
                result = EXIT;
                break;

            case ESC:
            case CANCEL:
                exit = TRUE;
                result = NONE;
                break;

            default:
                break;

        }

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(dialog);
    SDL_FreeSurface(leftList);
    SDL_FreeSurface(back);

    CursorOn();
    SetIntValue(ANIM2, TRUE);
    
    return result;
}

void PrintBookInfoPrompt(const char *string){

    AGGSPRITE sprite;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rect;

    if(! string) string = "View Spells";

    FillSPRITE(&sprite, "HSBTNS.ICN", 8);
    image = GetICNSprite(&sprite);
    rect.x = (video->w - image->w) / 2;
    rect.y = video->h / 2 + 240 - image->h;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    PrintAlignText(video, &rect, string, FONT_BIG);
}

ACTION ActionBookInfoLOOP(INTERFACEACTION *action){

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

    Sint32 	cx, cy;

    // цикл по событиям
    while(exit == NONE){

	ptr = action;

	SDL_GetMouseState(&cx, &cy);

        if(old.flagPres && !ValidPoint(&old.presRect, cx, cy)){
            PrintBookInfoPrompt(NULL);
            old.flagPres = FALSE;
        }else
            while(ptr){
                if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, cx, cy) && ptr->pf && !old.flagPres){
                    old.presRect = ptr->rect;
                    old.flagPres = TRUE;
		    exit = (*ptr->pf)();
            	}
        	ptr = (INTERFACEACTION *) ptr->next;
	}

	while(SDL_PollEvent(&event)){

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
			    
			    if(GetIntValue(DEBUG)) fprintf(stderr, "x: %d, y: %d\n", event.button.x, event.button.y);

			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_RCLICK) && ptr->pf )
					exit = (*ptr->pf)();

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
	}

	if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);
    }

    return exit;
}

ACTION	ActionBookOverPointInfo(void){

    char str[STRLEN + 1];

    snprintf(str, STRLEN, "You Hero has %d spell points remaining", currentHero->magicPoint);
    PrintBookInfoPrompt(str);

    return NONE;
}

ACTION	ActionBookOverAdventureSpell(void){

    PrintBookInfoPrompt("View adventure Spells");

    return NONE;
}

ACTION	ActionBookOverCombatSpell(void){

    PrintBookInfoPrompt("View combat Spells");

    return NONE;
}

ACTION	ActionBookOverPrevPage(void){

    PrintBookInfoPrompt("View previous Page");

    return NONE;
}

ACTION	ActionBookOverNextPage(void){

    PrintBookInfoPrompt("View next Page");

    return NONE;
}

ACTION	ActionBookOverClose(void){

    PrintBookInfoPrompt("Close Spellbook");

    return NONE;
}
