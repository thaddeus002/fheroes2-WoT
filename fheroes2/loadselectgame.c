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

/*
    load select game: standart campain multi
*/

#include <stdlib.h>
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "loadselectgame.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"

void ShowLoadSelectGame(void);
ACTION ActionPressLoadStandardGame(void);
ACTION ActionPressLoadCampainGame(void);
ACTION ActionPressLoadMultiGame(void);
ACTION ActionPressLoadCancelGame(void);

INTERFACEACTION *stpeloadselect = NULL;

ACTION DrawLoadSelectGame(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    stpeloadselect = NULL;

    // регистрируем события

    // кнопка standard game
    FillSPRITE(&sprite, "BTNNEWGM.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 45;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNNEWGM.ICN", 0);
    FillSPRITE(&action.objectPush, "BTNNEWGM.ICN", 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoadStandardGame;
    // регистрируем
    AddActionEvent(&stpeloadselect, &action);

    // кнопка campain game
    FillSPRITE(&sprite, "BTNNEWGM.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 110;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNNEWGM.ICN", 2);
    FillSPRITE(&action.objectPush, "BTNNEWGM.ICN", 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoadCampainGame;
    // регистрируем
    AddActionEvent(&stpeloadselect, &action);

    // кнопка multi game
    FillSPRITE(&sprite, "BTNNEWGM.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 175;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNNEWGM.ICN", 4);
    FillSPRITE(&action.objectPush, "BTNNEWGM.ICN", 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoadMultiGame;
    // регистрируем
    AddActionEvent(&stpeloadselect, &action);

    // кнопка cancel
    FillSPRITE(&sprite, "BTNNEWGM.ICN", 6);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 375;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNNEWGM.ICN", 6);
    FillSPRITE(&action.objectPush, "BTNNEWGM.ICN", 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoadCancelGame;
    // регистрируем
    AddActionEvent(&stpeloadselect, &action);

    // отображаем всю картинку
    ShowLoadSelectGame();

    // цикл событий
    ACTION result = ActionCycle(stpeloadselect);

    // освобождаем данные
    FreeActionEvent(stpeloadselect);
    
    return result;
}

void ShowLoadSelectGame(void){

    SDL_Surface *video; 
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    // на данной картинке работаем только в 640x480
    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue("fullscreen")) flag |= SDL_FULLSCREEN;
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

    // загружаем панель
    FillSPRITE(&sprite, "REDBACK.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 405;
    dest.y = 5;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    INTERFACEACTION *ptr = stpeloadselect;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressLoadStandardGame(void){

    fprintf(stderr, "action: LoadStandardGame\n");

    return NONE;
}

ACTION ActionPressLoadCampainGame(void){

    fprintf(stderr, "action: LoadCampainGame\n");

    return NONE;
}

ACTION ActionPressLoadMultiGame(void){

    fprintf(stderr, "action: LoadMultiGame\n");

    return NONE;
}

ACTION ActionPressLoadCancelGame(void){

    return CANCEL;
}
