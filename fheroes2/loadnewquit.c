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
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "draw.h"
#include "animation.h"
#include "actionevent.h"
#include "loadnewquit.h"
#include "newselectgame.h"
#include "loadselectgame.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"


typedef struct{
    SDL_Rect    pushRect;
    SDL_Rect    presRect;
    AGGSPRITE   object;
    BOOL        flagPres;
    BOOL        flagPush;
} S_OLDOBJECT2;

void ShowNewLoadQuit(void);
void RedrawMenuAnimation(void);

ACTION ActionPressNewGame(void);
ACTION ActionPressLoadGame(void);
ACTION ActionPressQuitGame(void);
ACTION ActionPressCredits(void);
ACTION ActionPressHighScores(void);

INTERFACEACTION *sthemain = NULL;
S_ANIMATION	*stheanim = NULL;

ACTION DrawNewLoadQuit(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    sthemain = NULL;

    // new game present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 483;
    dest.y = 175;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 0);
    FillSPRITE(&action.objectMotion, "BTNSHNGL.ICN", 2);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // new game click
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 483;
    dest.y = 175;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 0);
    FillSPRITE(&action.objectPush, "BTNSHNGL.ICN", 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewGame;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // load game present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x = 195;
    dest.y = 180;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 4);
    FillSPRITE(&action.objectMotion, "BTNSHNGL.ICN", 6);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // load game click
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x = 195;
    dest.y = 180;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 4);
    FillSPRITE(&action.objectPush, "BTNSHNGL.ICN", 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoadGame;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // quit game present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 16);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.y = 425;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 16);
    FillSPRITE(&action.objectMotion, "BTNSHNGL.ICN", 18);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    // регистрируем
    AddActionEvent(&sthemain, &action);
    
    // quit game click
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 16);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.y = 425;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 16);
    FillSPRITE(&action.objectPush, "BTNSHNGL.ICN", 19);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressQuitGame;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // high scores present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 8);
    image = GetICNSprite(&sprite);
    dest.x = 407;
    dest.y = 107;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 8);
    FillSPRITE(&action.objectMotion, "BTNSHNGL.ICN", 10);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionPressHighScores;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // high scores click
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 8);
    image = GetICNSprite(&sprite);
    dest.x = 407;
    dest.y = 107;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 8);
    FillSPRITE(&action.objectPush, "BTNSHNGL.ICN", 11);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressHighScores;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // credits present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.x = 303;
    dest.y = 137;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 12);
    FillSPRITE(&action.objectMotion, "BTNSHNGL.ICN", 14);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // credits present
    FillSPRITE(&sprite, "BTNSHNGL.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.x = 303;
    dest.y = 137;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNSHNGL.ICN", 12);
    FillSPRITE(&action.objectPush, "BTNSHNGL.ICN", 15);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCredits;
    // регистрируем
    AddActionEvent(&sthemain, &action);

    // анимация фонарей
    ICNHEADER *header;
    FillSPRITE(&sprite, "SHNGANIM.ICN", 0);
    header = GetICNHeader(&sprite);
    dest.x = 0;
    dest.y = 0;
    dest.w = 0;
    dest.h = 0;
    AddAnimationEvent(&stheanim, &dest, header, 41);
    
    // отображаем всю картинку
    ShowNewLoadQuit();

    // цикл событий
    SDL_Event event;
    SDL_Surface *video = SDL_GetVideoSurface();;
    ACTION result = NONE;
    S_OLDOBJECT2 old;
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
    while(result == NONE){
	while(SDL_PollEvent(&event))

	    switch(event.type){
	    
		case SDL_QUIT:

		    // close windows
		    result = EXIT;
		    break;

		case SDL_KEYDOWN:

		    // нажатия клавиатуры
		    switch(event.key.keysym.sym){

			case SDLK_ESCAPE:

			    result = ESC;
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
			    ptr = sthemain;
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
			    break;
			
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONUP:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка up
			    ptr = sthemain;
			    if(old.flagPush){
				DrawSprite(&old.pushRect, &old.object);
				old.flagPush = FALSE;
			    }

			    while(ptr){
				if(ValidPoint(&old.pushRect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK) && CompareRect(&ptr->rect, &old.pushRect) && ptr->pf )
					result = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;
			    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEMOTION:
		
		    ptr = sthemain;
		    
		    if(old.flagPres && !ValidPoint(&old.presRect, event.motion.x, event.motion.y)){
			DrawSprite(&old.presRect, &old.object);
			old.flagPres = FALSE;

		    }else
			while(ptr){
			    if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, event.motion.x, event.motion.y)){
				DrawSprite(&ptr->rect, &ptr->objectMotion);
				old.object = ptr->objectUp;
				old.presRect = ptr->rect;
				old.flagPres = TRUE;
			    }

    			    ptr = (INTERFACEACTION *) ptr->next;
		    }

		    CursorShow();

		    break;

		default:
    		    break;
	    }

	if(result != EXIT && 0 == ticket % GetIntValue(ANIMATIONDELAY)) RedrawMenuAnimation();

	if(GetIntValue(CYCLELOOP) && CYCLEDELAY) SDL_Delay(CYCLEDELAY);
    }

    // освобождаем данные
    FreeAnimationEvent(stheanim);

    FreeActionEvent(sthemain);

    return result;
}

void ShowNewLoadQuit(void){

    SDL_Surface *video; 
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    // на данной картинке работаем только в 640x480
    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue(FULLSCREEN)) flag |= SDL_FULLSCREEN;
    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 640 || video->h !=480)
        video = SDL_SetVideoMode(640, 480, 16, flag);

    if(NULL == video){ 
        fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError()); 
	return;
    }

    // загружаем фон
    FillSPRITE(&sprite, "HEROES.ICN", 0);
    image = GetICNSprite(&sprite);

    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    INTERFACEACTION *ptr = sthemain;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressNewGame(){

    PreloadObject("BTNNEWGM.ICN");
    PreloadObject("REDBACK.ICN");

    SetIntValue(ANIM1, FALSE);

    if(EXIT == DrawNewSelectGame()) return EXIT;

    SetIntValue(ANIM1, TRUE);
    
    ShowNewLoadQuit();

    return NONE;
}

ACTION ActionPressLoadGame(){

    PreloadObject("BTNNEWGM.ICN");
    PreloadObject("REDBACK.ICN");

    SetIntValue(ANIM1, FALSE);

    if(EXIT == DrawLoadSelectGame()) return EXIT;

    SetIntValue(ANIM1, TRUE);

    ShowNewLoadQuit();

    return NONE;
}

ACTION ActionPressQuitGame(){

    SetIntValue(ANIM1, FALSE);

    return EXIT;
}

ACTION ActionPressCredits(void){

    return NONE;
}

ACTION ActionPressHighScores(void){

    return NONE;
}

void RedrawMenuAnimation(void){

    if(! GetIntValue(ANIMATION)) return;
    if(! GetIntValue(ANIM1)) return;

    static Uint32 animationFrame = 0;
 
    Sint32 x; 
    Sint32 y; 
     
    SDL_Surface *video = SDL_GetVideoSurface(); 
    SDL_GetMouseState(&x, &y); 
    S_ANIMATION *ptr = stheanim; 
 
    while(ptr){ 
 
        if(ValidPoint(&ptr->rect[animationFrame % ptr->count], x, y)) CursorOff(); 
 
        SDL_BlitSurface(ptr->surface[animationFrame % ptr->count], NULL, video, &ptr->rect[animationFrame % ptr->count]); 

        CursorOn(); 
        ptr = ptr->next; 
    } 
 
    SDL_Flip(video); 
    SDL_Delay(GetIntValue(ANIMATIONDELAY));     

    ++animationFrame;
    
     return;
}
