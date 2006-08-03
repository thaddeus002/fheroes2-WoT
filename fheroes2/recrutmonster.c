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

#include "gamedefs.h"
#include "tools.h"
#include "cursor.h"
#include "debug.h"
#include "castle.h"
#include "config.h"
#include "actionevent.h"
#include "castle.h"
#include "monster.h"
#include "castleaction.h"

void RedrawDinamicRecrutMonster(SDL_Surface *, SDL_Rect *, E_MONSTER, Uint16, Uint16, Uint16, Uint32);

ACTION RecrutMonsterPressUp(void);
ACTION RecrutMonsterPressDown(void);
ACTION RecrutMonsterPressMax(void);
ACTION RecrutMonsterPressOkay(void);
ACTION RecrutMonsterPressCancel(void);

Uint8 DialogRecrutMonster(E_MONSTER emonster, Uint8 levelMonster, Uint16 available){

    CursorOff();
    SetIntValue(ANIM3, FALSE);
    
    SDL_Surface *back, *image, *video, *stat;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    BOOL exit = FALSE;
    char str[32];

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    
    S_MONSTER *monster = GetStatMonster(emonster);
    const S_CASTLE *castle = GetCurrentCastle();

    Uint16 countAvailable = available;
    Uint16 resultRecrutMonster = 0;
    Uint32 resultCountGold = 0;
    Uint16 resultCountResource = 0;

    FillSPRITE(&sprite, "RECRBKG.ICN", 0);
    image = GetICNSprite(&sprite);

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    rectBack.x = (video->w - image->w) / 2;
    rectBack.y = (video->h - image->h) / 2;
    rectBack.w = image->w;
    rectBack.h = image->h;

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "DialogRecrutMonster: CreateRGBSurface failed: %s\n", SDL_GetError());
	return 0;
    }

    // сохраняем бакграунд
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // отображаем фон диалога
    SDL_BlitSurface(image, NULL, video, &rectBack);

    // название монстра
    sprintf(str, "Recrut %s", monster->descriptions);
    rectCur.x = rectBack.x + 160;
    rectCur.y = rectBack.y + 25;
    rectCur.w = GetLengthText(str, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x = rectCur.x - rectCur.w / 2;
    PrintText(video, &rectCur, str, FONT_BIG);

    // cost per troop
    if( emonster == CYCLOPS ||
        emonster == PHOENIX ||
	emonster == GREEN_DRAGON ||
        emonster == RED_DRAGON ||
        emonster == BLACK_DRAGON ||
        emonster == GIANT ||
        emonster == TITAN ||
        emonster == GENIE ){

	// спрайт золото (условия)
	FillSPRITE(&sprite, "RESOURCE.ICN", 6);
	image = GetICNSprite(&sprite);
	rectCur.x = rectBack.x + 150;
	rectCur.y = rectBack.y + 75;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// спрайт золото (всего)
	rectCur.x = rectBack.x + 105;
	rectCur.y = rectBack.y + 200;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// количество золото (условия)
	sprintf(str, "%d", monster->cost);
	rectCur.x = rectBack.x + 183;
	rectCur.y = rectBack.y + 103;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }else{
	// спрайт золото (условия)
	FillSPRITE(&sprite, "RESOURCE.ICN", 6);
	image = GetICNSprite(&sprite);
	rectCur.x = rectBack.x + 175;
	rectCur.y = rectBack.y + 75;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// спрайт золото (всего)
	rectCur.x = rectBack.x + 130;
	rectCur.y = rectBack.y + 200;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// количество золото (условия)
	sprintf(str, "%d", monster->cost);
	rectCur.x = rectBack.x + 205;
	rectCur.y = rectBack.y + 103;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }

    switch(emonster){

        case CYCLOPS:
	    // спрайт crystal (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 4);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт crystal (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество crystal (условия)
	    sprintf(str, "%d", BUY_CYCLOPS_CRYSTAL);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case PHOENIX:
	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 72;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт mercury (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 197;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество mercury (условия)
	    sprintf(str, "%d", BUY_PHOENIX_MERCURY);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case GREEN_DRAGON:
	    // спрайт sulfur (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт sulfur (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество sulfur (условия)
	    sprintf(str, "%d", BUY_GREENDRAGON_SULFUR);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case RED_DRAGON:
	    // спрайт sulfur (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт sulfur (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество sulfur (условия)
	    sprintf(str, "%d", BUY_REDDRAGON_SULFUR);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case BLACK_DRAGON:
	    // спрайт sulfur (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт sulfur (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество sulfur (условия)
	    sprintf(str, "%d", BUY_BLACKDRAGON_SULFUR);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case GIANT:
	    // спрайт gems (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт gems (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество gems (условия)
	    sprintf(str, "%d", BUY_GIANT_GEMS);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case TITAN:
	    // спрайт gems (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт gems (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество gems (условия)
	    sprintf(str, "%d", BUY_TITAN_GEMS);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        case GENIE:
	    // спрайт gems (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // спрайт gems (всего)
	    rectCur.x = rectBack.x + 180;
	    rectCur.y = rectBack.y + 200;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество gems (условия)
	    sprintf(str, "%d", BUY_GENIE_GEMS);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
            break;

        default:
            break;
    }

    // картинка монстра
    FillSPRITE(&sprite, MonsterBigNamePortrait(emonster), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 50;
    rectCur.y = rectBack.y + 45;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // text number to buy
    sprintf(str, "Number to buy:");
    rectCur.x = rectBack.x + 30;
    rectCur.y = rectBack.y + 163;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);

    // сохраняем статичную картинку
    if(NULL == (stat = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "DialogRecrutMonster: CreateRGBSurface failed: %s\n", SDL_GetError());
	return 0;
    }
    SDL_BlitSurface(video, &rectBack, stat, NULL);
    
    // далее динамичные елементы
    RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);

    // рисуем кнопки
    // кнопка UP
    FillSPRITE(&sprite, "RECRUIT.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 208;
    rectCur.y = rectBack.y + 156;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "RECRUIT.ICN", 0);
    FillSPRITE(&action.objectPush, "RECRUIT.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = RecrutMonsterPressUp;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка DOWN
    FillSPRITE(&sprite, "RECRUIT.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 208;
    rectCur.y = rectBack.y + 171;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "RECRUIT.ICN", 2);
    FillSPRITE(&action.objectPush, "RECRUIT.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = RecrutMonsterPressDown;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка MAX
    FillSPRITE(&sprite, "RECRUIT.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 230;
    rectCur.y = rectBack.y + 155;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "RECRUIT.ICN", 4);
    FillSPRITE(&action.objectPush, "RECRUIT.ICN", 5);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = RecrutMonsterPressMax;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка OKAY
    FillSPRITE(&sprite, "RECRUIT.ICN", 8);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 34;
    rectCur.y = rectBack.y + 249;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "RECRUIT.ICN", 8);
    FillSPRITE(&action.objectPush, "RECRUIT.ICN", 9);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = RecrutMonsterPressOkay;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка CANCEL
    FillSPRITE(&sprite, "RECRUIT.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 187;
    rectCur.y = rectBack.y + 249;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "RECRUIT.ICN", 6);
    FillSPRITE(&action.objectPush, "RECRUIT.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = RecrutMonsterPressCancel;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionCycle(dialog)){

    	    case EXIT:
    	    case ESC:
    	    case CANCEL:
    		resultRecrutMonster = 0;
    		exit = TRUE;
    		break;

    	    case OK:
    		exit = TRUE;
    		break;

    	    case UP:
		if(countAvailable && CheckBuyMonsterFromCastle(castle, levelMonster, resultRecrutMonster + 1)){
		    resultRecrutMonster++;
		    countAvailable--;
		    if(emonster == CYCLOPS)
			resultCountResource += BUY_CYCLOPS_CRYSTAL;
		    else if(emonster == PHOENIX)
			resultCountResource += BUY_PHOENIX_MERCURY;
		    else if(emonster == GREEN_DRAGON)
			resultCountResource += BUY_GREENDRAGON_SULFUR;
		    else if(emonster == RED_DRAGON)
			resultCountResource += BUY_REDDRAGON_SULFUR;
		    else if(emonster == BLACK_DRAGON)
			resultCountResource += BUY_BLACKDRAGON_SULFUR;
		    else if(emonster == GIANT)
			resultCountResource += BUY_GIANT_GEMS;
		    else if(emonster == TITAN)
			resultCountResource += BUY_TITAN_GEMS;
		    else if(emonster == GENIE)
			resultCountResource += BUY_GENIE_GEMS;
		    resultCountGold += monster->cost;
		    CursorOff();
		    RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);
		    CursorOn();
		}
    		break;

    	    case DOWN:
		if(resultRecrutMonster){
		    resultRecrutMonster--;
		    countAvailable++;
		    if(emonster == CYCLOPS)
			resultCountResource -= BUY_CYCLOPS_CRYSTAL;
		    else if(emonster == PHOENIX)
			resultCountResource -= BUY_PHOENIX_MERCURY;
		    else if(emonster == GREEN_DRAGON)
			resultCountResource -= BUY_GREENDRAGON_SULFUR;
		    else if(emonster == RED_DRAGON)
			resultCountResource -= BUY_REDDRAGON_SULFUR;
		    else if(emonster == BLACK_DRAGON)
			resultCountResource -= BUY_BLACKDRAGON_SULFUR;
		    else if(emonster == GIANT)
			resultCountResource -= BUY_GIANT_GEMS;
		    else if(emonster == TITAN)
			resultCountResource -= BUY_TITAN_GEMS;
		    else if(emonster == GENIE)
			resultCountResource -= BUY_GENIE_GEMS;
		    resultCountGold -= monster->cost;
		    CursorOff();
		    RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);
		    CursorOn();
		}
    		break;

    	    case MAX:
		while(countAvailable && CheckBuyMonsterFromCastle(castle, levelMonster, resultRecrutMonster + 1)){
		    resultRecrutMonster++;
		    countAvailable--;
		    if(emonster == CYCLOPS)
			resultCountResource += BUY_CYCLOPS_CRYSTAL;
		    else if(emonster == PHOENIX)
			resultCountResource += BUY_PHOENIX_MERCURY;
		    else if(emonster == GREEN_DRAGON)
			resultCountResource += BUY_GREENDRAGON_SULFUR;
		    else if(emonster == RED_DRAGON)
			resultCountResource += BUY_REDDRAGON_SULFUR;
		    else if(emonster == BLACK_DRAGON)
			resultCountResource += BUY_BLACKDRAGON_SULFUR;
		    else if(emonster == GIANT)
			resultCountResource += BUY_GIANT_GEMS;
		    else if(emonster == TITAN)
			resultCountResource += BUY_TITAN_GEMS;
		    else if(emonster == GENIE)
			resultCountResource += BUY_GENIE_GEMS;
		    resultCountGold += monster->cost;
		}
		CursorOff();
		RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);
		CursorOn();
		break;

    	    default:
    		break;

	}

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(dialog);
    SDL_FreeSurface(stat);
    SDL_FreeSurface(back);

    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return resultRecrutMonster;
}

ACTION RecrutMonsterPressUp(void){

    return UP;
}

ACTION RecrutMonsterPressDown(void){

    return DOWN;
}

ACTION RecrutMonsterPressMax(void){

    return MAX;
}

ACTION RecrutMonsterPressOkay(void){

    return OK;
}

ACTION RecrutMonsterPressCancel(void){

    return CANCEL;
}

void RedrawDinamicRecrutMonster(SDL_Surface *stat, SDL_Rect *rect, E_MONSTER emonster, Uint16 available, Uint16 resultRecrutMonster, Uint16 resultCountResource, Uint32 resultCountGold){

    SDL_Rect rectCur;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[64];

    // востанавливаем статичную картинку
    SDL_BlitSurface(stat, NULL, video, rect);

    // text available
    sprintf(str, "Available: %d", available);
    rectCur.x = rect->x + 40;
    rectCur.y = rect->y + 138;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // text count recrut
    sprintf(str, "%d", resultRecrutMonster);
    rectCur.x = rect->x + 167;
    rectCur.y = rect->y + 160;
    rectCur.w = GetLengthText(str, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x = rectCur.x - rectCur.w / 2;
    PrintText(video, &rectCur, str, FONT_BIG);
    if( emonster == CYCLOPS ||
        emonster == PHOENIX ||
	emonster == GREEN_DRAGON ||
        emonster == RED_DRAGON ||
        emonster == BLACK_DRAGON ||
        emonster == GIANT ||
        emonster == TITAN ||
        emonster == GENIE ){
	// text count gold
	sprintf(str, "%d", resultCountGold);
	rectCur.x = rect->x + 133;
	rectCur.y = rect->y + 228;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// text count resource
	sprintf(str, "%d", resultCountResource);
	rectCur.x = rect->x + 195;
	rectCur.y = rect->y + 228;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }else{
	// text count gold
	sprintf(str, "%d", resultCountGold);
	rectCur.x = rect->x + 160;
	rectCur.y = rect->y + 228;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }
}

void ErrorDialogRecrutMonster(const S_CASTLE *castle){
}
