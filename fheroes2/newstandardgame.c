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
    new select standard game
*/

#include <stdlib.h>
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"
#include "newstandardgame.h"
#include "mp2maps.h"
#include "newselectmaps.h"

void	ShowNewStandardGame(void);
void	SelectCursorRestoreBackground(void);
void	SelectCursorSaveBackground(SDL_Rect *);

void    DrawMapsOpponents(void);
void    DrawMapsClass(void);

ACTION	ActionPressNewStandardSelect(void);
ACTION	ActionPressNewStandardOkay(void);
ACTION	ActionPressNewStandardCancel(void);

ACTION	ActionSelectDifficulty(E_GAMELEVEL);
ACTION	ActionSelectDifficultyEasy(void);
ACTION	ActionSelectDifficultyNormal(void);
ACTION	ActionSelectDifficultyHard(void);
ACTION	ActionSelectDifficultyExpert(void);
ACTION	ActionSelectDifficultyImpossible(void);


INTERFACEACTION *stpenewstandard = NULL;

struct {
    SDL_Rect		rect;
    SDL_Surface		*surface;
    BOOL		use;
} backCursor;

ACTION DrawSelectNewStandardGame(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    stpenewstandard = NULL;
    
    // инициализируем backgroundCursor
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);

    backCursor.surface = NULL;
    backCursor.use = FALSE;
    backCursor.rect.x = 0;
    backCursor.rect.y = 0;
    backCursor.rect.w = image->w;
    backCursor.rect.h = image->h;

    if(NULL == (backCursor.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "DrawSelectNewStandardGame: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), image->w, image->h);
            return EXIT;
    }

    // регистрируем события

    // кнопка select file
    FillSPRITE(&sprite, "NGEXTRA.ICN", 64);
    image = GetICNSprite(&sprite);
    dest.x = 513;
    dest.y = 77;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 64);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 65);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardSelect;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // кнопка okey select
    FillSPRITE(&sprite, "NGEXTRA.ICN", 66);
    image = GetICNSprite(&sprite);
    dest.x = 234;
    dest.y = 413;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 66);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 67);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardOkay;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // кнопка cancel select
    FillSPRITE(&sprite, "NGEXTRA.ICN", 68);
    image = GetICNSprite(&sprite);
    dest.x = 491;
    dest.y = 413;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 68);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 69);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardCancel;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty easy
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 225;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyEasy;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty normal
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 302;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyNormal;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty hard
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 378;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyHard;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty expert
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyExpert;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty impossible
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 532;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyImpossible;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);



                        
    // отображаем всю картинку
    ShowNewStandardGame();

    // цикл событий
    ACTION result = ActionCycle(stpenewstandard);

    // освобождаем данные
    FreeActionEvent(stpenewstandard);
    if(backCursor.surface) SDL_FreeSurface(backCursor.surface);

    return result;
}

void ShowNewStandardGame(void){

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

    // загружаем тень панели
    FillSPRITE(&sprite, "NGHSBKG.ICN", 1);
    image = GetICNSprite(&sprite);
    dest.x = 196;
    dest.y = 40;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    // загружаем панель
    FillSPRITE(&sprite, "NGHSBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 204;
    dest.y = 33;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    // основные надписи
    
    // Scenario
    dest.x = 376;
    dest.y = 53;
    dest.w = FONT_WIDTHBIG * 10;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Scenario:", FONT_BIG);
    // Name Scenario
    dest.x = 260;
    dest.y = 78;
    dest.w = FONT_WIDTHBIG * 16;
    dest.h = FONT_HEIGHTBIG / 2;
    PrintText(video, &dest, GetStrValue(MAPSLONGNAME), FONT_BIG);

    // Game Difficulty
    dest.x = 330;
    dest.y = 107;
    dest.w = FONT_WIDTHBIG * 17;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Game Difficulty:", FONT_BIG);
    // Easy
    dest.x = 248;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 5;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Easy", FONT_SMALL);
    // Normal
    dest.x = 316;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 9;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Normal", FONT_SMALL);
    // Hard
    dest.x = 395;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 5;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Hard", FONT_SMALL);
    // Expert
    dest.x = 472;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 7;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Expert", FONT_SMALL);
    // Impossible
    dest.x = 536;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 12;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Impossible", FONT_SMALL);

    INTERFACEACTION *ptr = stpenewstandard;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressNewStandardSelect(void){

    PreloadObject("REQUESTS.ICN");
    PreloadObject("REQSBKG.ICN");
    
    if(EXIT == DrawNewSelectMaps()) return EXIT;

    FreeObject("REQUESTS.ICN");
    FreeObject("REQSBKG.ICN");

    ShowNewStandardGame();

    DrawMapsOpponents();
    DrawMapsClass();

    ActionSelectDifficultyNormal();

    return NONE;
}

void DrawMapsOpponents(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;
    Uint8 seek = 0;
    
    // text
    dest.x = 368;
    dest.y = 210;
    dest.w = FONT_WIDTHBIG * 11;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Opponents:", FONT_BIG);

    if(GetIntValue(KINGDOMCOLORS) & COLORBLUE){

	seek = 0;

	GetIntValue(ALLOWCOLORS) & COLORBLUE ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORGREEN){

	seek = 1;

	GetIntValue(ALLOWCOLORS) & COLORGREEN ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORRED){

	seek = 2;

	GetIntValue(ALLOWCOLORS) & COLORRED ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORYELLOW){

	seek = 3;

	GetIntValue(ALLOWCOLORS) & COLORYELLOW ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORORANGE){

	seek = 4;

	GetIntValue(ALLOWCOLORS) & COLORORANGE ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORPURPLE){

	seek = 5;

	GetIntValue(ALLOWCOLORS) & COLORPURPLE ? FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + seek) : FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + seek);
	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 230;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
}

void DrawMapsClass(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;
    Uint8 seek = 0;
    
    // text
    dest.x = 386;
    dest.y = 284;
    dest.w = FONT_WIDTHBIG * 7;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Class:", FONT_BIG);

    if(GetIntValue(KINGDOMCOLORS) & COLORBLUE){

	seek = 0;

	if(GetIntValue(RNDCOLORS) & COLORBLUE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORBLUE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORGREEN){

	seek = 1;

	if(GetIntValue(RNDCOLORS) & COLORGREEN)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORGREEN)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORRED){

	seek = 2;

	if(GetIntValue(RNDCOLORS) & COLORRED)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORRED)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORYELLOW){

	seek = 3;

	if(GetIntValue(RNDCOLORS) & COLORYELLOW)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORYELLOW)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORORANGE){

	seek = 4;

	if(GetIntValue(RNDCOLORS) & COLORORANGE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORORANGE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
    if(GetIntValue(KINGDOMCOLORS) & COLORPURPLE){

	seek = 5;

	if(GetIntValue(RNDCOLORS) & COLORPURPLE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);

	else if(GetIntValue(ALLOWCOLORS) & COLORPURPLE)

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51 + seek);

	else

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + seek);

	image = GetICNSprite(&sprite);

	dest.x = 225 + seek * image->w;
	dest.y = 308;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

    }
}

ACTION ActionPressNewStandardOkay(void){

    if(strlen(GetStrValue(FILEMAPSPATH)))

	return InitMaps(GetStrValue(FILEMAPSPATH));
    else
	return NONE;
}

ACTION ActionPressNewStandardCancel(void){

    return CANCEL;
}

ACTION ActionSelectDifficultyEasy(void){

    ActionSelectDifficulty(GAME_EASY);
    SetIntValue(GAMEDIFFICULTY, GAME_EASY);

    return NONE;
}

ACTION ActionSelectDifficultyNormal(void){

    ActionSelectDifficulty(GAME_NORMAL);
    SetIntValue(GAMEDIFFICULTY, GAME_NORMAL);

    return NONE;
}

ACTION ActionSelectDifficultyHard(void){

    ActionSelectDifficulty(GAME_HARD);
    SetIntValue(GAMEDIFFICULTY, GAME_HARD);

    return NONE;
}

ACTION ActionSelectDifficultyExpert(void){

    ActionSelectDifficulty(GAME_EXPERT);
    SetIntValue(GAMEDIFFICULTY, GAME_EXPERT);

    return NONE;
}

ACTION ActionSelectDifficultyImpossible(void){

    ActionSelectDifficulty(GAME_IMPOSSIBLE);
    SetIntValue(GAMEDIFFICULTY, GAME_IMPOSSIBLE);

    return NONE;
}

ACTION ActionSelectDifficulty(E_GAMELEVEL level){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.w = image->w;
    rect.h = image->h;
    rect.y = 124;

    switch(level){
    
	default:
	case GAME_EASY:
	    rect.x = 225;
	    break;
	
	case GAME_NORMAL:
	    rect.x = 302;
	    break;

	case GAME_HARD:
	    rect.x = 378;
	    break;

	case GAME_EXPERT:
	    rect.x = 455;
	    break;

	case GAME_IMPOSSIBLE:
	    rect.x = 532;
	    break;
    }
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

void SelectCursorRestoreBackground(void){

    if(! backCursor.use) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_BlitSurface(backCursor.surface, NULL, video, &backCursor.rect);
    SDL_Flip(video);
    
    backCursor.use = FALSE;
}

void SelectCursorSaveBackground(SDL_Rect *rect){

    if(backCursor.use) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_BlitSurface(video, rect, backCursor.surface, NULL);
    
    backCursor.rect = *rect;
    backCursor.use = TRUE;
}
