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
#include "actionevent.h"
#include "stpemain.h"
#include "stpesize.h"
#include "stpeload.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"

void ShowNewLoadQuit(void);
ACTION ActionPressNew(void);
ACTION ActionPressLoad(void);

INTERFACEACTION *stpemain = NULL;

ACTION DrawNewLoadQuit(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    stpemain = NULL;

    // регистрируем события

    // кнопка new map 
    FillSPRITE(&sprite, "BTNEMAIN.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 45;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNEMAIN.ICN", 0);
    FillSPRITE(&action.objectPush, "BTNEMAIN.ICN", 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNew;
    // регистрируем
    AddActionEvent(&stpemain, &action);

    // кнопка load map
    FillSPRITE(&sprite, "BTNEMAIN.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 110;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNEMAIN.ICN", 2);
    FillSPRITE(&action.objectPush, "BTNEMAIN.ICN", 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLoad;
    // регистрируем
    AddActionEvent(&stpemain, &action);

    // кнопка cancel
    FillSPRITE(&sprite, "BTNEMAIN.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 375;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNEMAIN.ICN", 4);
    FillSPRITE(&action.objectPush, "BTNEMAIN.ICN", 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCancel;
    // регистрируем
    AddActionEvent(&stpemain, &action);

    // отображаем всю картинку
    ShowNewLoadQuit();

    // цикл событий
    ACTION result = ActionCycle(stpemain);

    // освобождаем данные
    FreeActionEvent(stpemain);
    
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
    if(TRUE == GetIntValue("fullscreen")) flag |= SDL_FULLSCREEN;
    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 640 || video->h !=480)
        video = SDL_SetVideoMode(640, 480, 16, flag);

    if(NULL == video){ 
        fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError()); 
	return;
    }

    // загружаем фон
    FillSPRITE(&sprite, "EDITOR.ICN", 0);
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

    INTERFACEACTION *ptr = stpemain;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressNew(){

    if(EXIT == DrawSelectSize()) return EXIT;

    ShowNewLoadQuit();

    return NONE;
}

ACTION ActionPressLoad(){

    if(EXIT == DrawSelectLoad()) return EXIT;

    PreloadObject("REQUESTS.ICN");
    PreloadObject("REQSBKG.ICN");

    ShowNewLoadQuit();

    return NONE;
}
