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
    load new quit game
*/

#include <stdlib.h>
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "loadnewquit.h"
#include "newselectgame.h"
#include "loadselectgame.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"

void ShowNewLoadQuit(void);
ACTION ActionPressNewGame(void);
ACTION ActionPressLoadGame(void);
ACTION ActionPressQuitGame(void);
ACTION ActionPressCredits(void);
ACTION ActionPressHighScores(void);

INTERFACEACTION *sthemain = NULL;

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

    // отображаем всю картинку
    ShowNewLoadQuit();

    // цикл событий
    ACTION result = ActionCycle(sthemain);

    // освобождаем данные
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

    if(EXIT == DrawNewSelectGame()) return EXIT;
    
    ShowNewLoadQuit();
        
    return NONE;
}

ACTION ActionPressLoadGame(){

    PreloadObject("BTNNEWGM.ICN");
    PreloadObject("REDBACK.ICN");

    if(EXIT == DrawLoadSelectGame()) return EXIT;

    ShowNewLoadQuit();
        
    return NONE;
}

ACTION ActionPressQuitGame(){

    return EXIT;
}

ACTION ActionPressCredits(void){

    return NONE;
}

ACTION ActionPressHighScores(void){

    return NONE;
}
