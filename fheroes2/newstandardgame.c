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
#include <string.h>
#include "SDL.h"

#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"
#include "newstandardgame.h"
#include "mp2maps.h"
#include "selector.h"
#include "newselectmaps.h"

void	ShowNewStandardGame(void);
void    DrawMapsOpponents(void);
void    DrawMapsClass(void);

ACTION	ActionSelectDifficulty(E_GAMELEVEL);
ACTION	ActionSelectDifficultyEasy(void);
ACTION	ActionSelectDifficultyNormal(void);
ACTION	ActionSelectDifficultyHard(void);
ACTION	ActionSelectDifficultyExpert(void);
ACTION	ActionSelectDifficultyImpossible(void);
ACTION	ActionClickSelectColor(void);
ACTION	ActionClickSelectRaceBlue(void);
ACTION	ActionClickSelectRaceGreen(void);
ACTION	ActionClickSelectRaceRed(void);
ACTION	ActionClickSelectRaceYellow(void);
ACTION	ActionClickSelectRaceOrange(void);
ACTION	ActionClickSelectRacePurple(void);

INTERFACEACTION *stpenewstandard = NULL;

S_SELECT *gameDifficulty = NULL;

ACTION DrawSelectNewStandardGame(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;
    ACTION result = NONE;
    BOOL exit;

    stpenewstandard = NULL;
    
    // инициализируем selector Game Difficulty
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    if(NULL == (gameDifficulty = InitSelector(&sprite, 225, 77, FALSE))) return EXIT;

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
    action.pf = ActionPressSELECT;
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
    action.pf = ActionPressOK;
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
    action.pf = ActionPressCANCEL;
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
    exit = FALSE;
    while(! exit)
        switch(ActionCycle(stpenewstandard, NULL)){

            case EXIT:
                exit = TRUE;
                result = EXIT;
                break;

            case ESC:
            case CANCEL:
                exit = TRUE;
                result = CANCEL;
                break;

            case SELECT:
		PreloadObject("REQUESTS.ICN");
		PreloadObject("REQSBKG.ICN");
		if(EXIT == DrawNewSelectMaps()){
		    exit = TRUE;
		    result = EXIT;
		}else{
		    FreeObject("REQUESTS.ICN");
		    FreeObject("REQSBKG.ICN");
		    ShowNewStandardGame();
		    DrawMapsOpponents();
		    DrawMapsClass();
		    ActionSelectDifficultyNormal();
		}
        	break;
    
            case OK:
		if(strlen(GetStrValue(FILEMAPSPATH))){
            	    exit = TRUE;
		    result = InitMaps(GetStrValue(FILEMAPSPATH));
		}
                break;

            default:
        	break;
	}

    // освобождаем данные
    FreeActionEvent(stpenewstandard);
    FreeSelector(gameDifficulty);

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

void DrawMapsOpponents(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;
    INTERFACEACTION action;

    RemoveActionLevelEvent(stpenewstandard, LEVELEVENT_SELECTCOLOR);
    
    // text
    dest.x = 368;
    dest.y = 210;
    dest.w = FONT_WIDTHBIG * 11;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Opponents:", FONT_BIG);

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    Uint8 human = 0;
    
    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01){

    	    if((GetIntValue(ALLOWCOLORS) >> i) & 0x01 && !human){
		FillSPRITE(&sprite, "NGEXTRA.ICN", 9 + i);
		human = i + 1;
    	    }else if((GetIntValue(ALLOWCOLORS) >> i) & 0x01)
		FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + i);
	    else
		FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + i);

	    image = GetICNSprite(&sprite);

	    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
	    dest.y = 234;
	    dest.w = image->w;
	    dest.h = image->h;

    	    if((GetIntValue(ALLOWCOLORS) >> i) & 0x01){
		// обнуляем структуру
		ZeroINTERFACEACTION(&action);
		// заполняем
		action.rect = dest;
		action.mouseEvent = MOUSE_LCLICK;
		action.level = LEVELEVENT_SELECTCOLOR;
		action.pf = ActionClickSelectColor;
		// регистрируем
		AddActionEvent(&stpenewstandard, &action);
	    }

	    SDL_BlitSurface(image, NULL, video, &dest);
	    ++current;
    }
    
    SetIntValue(HUMANCOLORS, human - 1);
}

void DrawMapsClass(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;
    INTERFACEACTION action;

    RemoveActionLevelEvent(stpenewstandard, LEVELEVENT_SELECTRACE);
    
    // text
    dest.x = 386;
    dest.y = 290;
    dest.w = FONT_WIDTHBIG * 7;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Class:", FONT_BIG);

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;
    
    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return;

    for(color = BLUE; color < GRAY; ++color){
    
	ZeroINTERFACEACTION(&action);
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEBLUE);
		action.pf = ActionClickSelectRaceBlue;
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		action.pf = ActionClickSelectRaceGreen;
		break;
	    case RED:
		i = GetIntValue(RACERED);
		action.pf = ActionClickSelectRaceRed;
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		action.pf = ActionClickSelectRaceYellow;
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		action.pf = ActionClickSelectRaceOrange;
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		action.pf = ActionClickSelectRacePurple;
		break;
	    default:
		i = 0x07;
		break;
	}

	// 00 - kngt, 01 - barb, 02 - sorc, 03 - wrlk, 04 - wzrd, 05 - necr, 06 - mult, 07 - rnd, ff - none
	if(0xFF == i)
	    continue;
	else if(0x07 == i)
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	else
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 70 + i);

	image = GetICNSprite(&sprite);

	dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
	dest.y = 314;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, NULL, video, &dest);

	action.rect = dest;
	action.mouseEvent = MOUSE_LCLICK;
	action.level = LEVELEVENT_SELECTCOLOR;
	if(i == 0x07) AddActionEvent(&stpenewstandard, &action);

	++current;
    }
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
    
    RedrawSelector(gameDifficulty, &rect);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectColor(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;
    Uint8 i;

    Uint8 count = 0;
    Uint8 current = 0;
    Uint8 human = GetIntValue(HUMANCOLORS);
    
    Sint32 mx, my;
    SDL_GetMouseState(&mx, &my);    

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;
    
    CursorOff();
    
    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01){

	    FillSPRITE(&sprite, "NGEXTRA.ICN", 3);
	    image = GetICNSprite(&sprite);

	    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
	    dest.y = 234;
	    dest.w = image->w;
	    dest.h = image->h;
	    
    	    if(((GetIntValue(ALLOWCOLORS) >> i) & 0x01) && ValidPoint(&dest, mx, my)){
		FillSPRITE(&sprite, "NGEXTRA.ICN", 9 + i);
		human = i;
    	    }else if((GetIntValue(ALLOWCOLORS) >> i) & 0x01)
		FillSPRITE(&sprite, "NGEXTRA.ICN", 3 + i);
	    else
		FillSPRITE(&sprite, "NGEXTRA.ICN", 15 + i);

	    image = GetICNSprite(&sprite);
	    SDL_BlitSurface(image, NULL, video, &dest);

	    ++current;
    }
    
    CursorOn();
    SetIntValue(HUMANCOLORS, human);

    return NONE;
}

ACTION	ActionClickSelectRaceBlue(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < BLUE; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEBLUE);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACERED);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACEBLUE)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACEBLUE, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACEBLUE, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACEBLUE, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACEBLUE, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACEBLUE, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACEBLUE, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACEBLUE, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectRaceGreen(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < GREEN; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEGREEN);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACERED);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACEGREEN)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACEGREEN, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACEGREEN, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACEGREEN, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACEGREEN, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACEGREEN, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACEGREEN, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACEGREEN, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectRaceRed(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < RED; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACERED);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACERED);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACERED)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACERED, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACERED, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACERED, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACERED, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACERED, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACERED, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACERED, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectRaceYellow(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < YELLOW; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEYELLOW);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACEYELLOW);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACEYELLOW)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACEYELLOW, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACEYELLOW, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACEYELLOW, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACEYELLOW, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACEYELLOW, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACEYELLOW, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACEYELLOW, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectRaceOrange(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < ORANGE; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEORANGE);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACEORANGE);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACEORANGE)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACEORANGE, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACEORANGE, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACEORANGE, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACEORANGE, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACEORANGE, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACEORANGE, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACEORANGE, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}

ACTION	ActionClickSelectRacePurple(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    Uint8 i;
    Uint8 count = 0;
    Uint8 current = 0;
    E_COLORS color;

    for(i = 0; i < 8; ++i)
        if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01) ++count;

    if(! count) return NONE;

    for(color = BLUE; color < PURPLE; ++color){
	switch(color){
	    case BLUE:
		i = GetIntValue(RACEPURPLE);
		break;
	    case GREEN:
		i = GetIntValue(RACEGREEN);
		break;
	    case RED:
		i = GetIntValue(RACEPURPLE);
		break;
	    case YELLOW:
		i = GetIntValue(RACEYELLOW);
		break;
	    case ORANGE:
		i = GetIntValue(RACEORANGE);
		break;
	    case PURPLE:
		i = GetIntValue(RACEPURPLE);
		break;
	    default:
		i = 0x07;
		break;
	}
	if(0xFF == i) continue;
	++current;
    }

    switch(GetIntValue(RACEPURPLE)){

	case 0x07:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 51);
	    SetIntValue(RACEPURPLE, 0x00);
	    break;
	case 0x00:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 52);
	    SetIntValue(RACEPURPLE, 0x01);
	    break;
	case 0x01:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 53);
	    SetIntValue(RACEPURPLE, 0x02);
	    break;
	case 0x02:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 54);
	    SetIntValue(RACEPURPLE, 0x03);
	    break;
	case 0x03:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 55);
	    SetIntValue(RACEPURPLE, 0x04);
	    break;
	case 0x04:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 56);
	    SetIntValue(RACEPURPLE, 0x05);
	    break;
	case 0x05:
	    FillSPRITE(&sprite, "NGEXTRA.ICN", 58);
	    SetIntValue(RACEPURPLE, 0x07);
	    break;
	default:
	    return NONE;
	    break;
    }

    image = GetICNSprite(&sprite);

    dest.x = 228 + current * image->w * 6 / count + (image->w * (6 - count) / (2 * count));
    dest.y = 314;
    dest.w = image->w;
    dest.h = image->h;

    CursorOff();
    SDL_BlitSurface(image, NULL, video, &dest);
    CursorOn();

    return NONE;
}
