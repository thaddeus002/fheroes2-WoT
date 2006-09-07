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
#include "config.h"
#include "actionevent.h"
#include "payment.h"
#include "castleaction.h"

void RedrawDinamicRecrutMonster(SDL_Surface *, SDL_Rect *, E_MONSTER, Uint16, Uint16, Uint16, Uint32);

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
    sprintf(str, "Recrut %s", GetStringMonster(emonster));
    rectCur.x = rectBack.x + 160;
    rectCur.y = rectBack.y + 25;
    rectCur.w = GetLengthText(str, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    rectCur.x = rectCur.x - rectCur.w / 2;
    PrintText(video, &rectCur, str, FONT_BIG);

    S_PAYMENT payment = *(PaymentConditionsMonster(emonster));
    // cost per troop
    if( payment.wood ||
        payment.ore ||
	payment.crystal ||
        payment.sulfur ||
        payment. mercury ||
        payment.gems ){

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
	sprintf(str, "%d", payment.gold);
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
	sprintf(str, "%d", payment.gold);
	rectCur.x = rectBack.x + 205;
	rectCur.y = rectBack.y + 103;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }

    if(payment.crystal){

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
	    sprintf(str, "%d", payment.crystal);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.mercury){

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
	    sprintf(str, "%d", payment.mercury);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.wood){

	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 0);
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
	    sprintf(str, "%d", payment.wood);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.ore){

	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
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
	    sprintf(str, "%d", payment.ore);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.sulfur){

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
	    sprintf(str, "%d", payment.sulfur);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.gems){

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
	    sprintf(str, "%d", payment.gems);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
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
    action.pf = ActionPressUP;
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
    action.pf = ActionPressDOWN;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // кнопка MAX
    if(available){
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
	action.pf = ActionPressMAX;
	AddActionEvent(&dialog, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }else{
	FillSPRITE(&sprite, "RECRUIT.ICN", 5);
	image = GetICNSprite(&sprite);
	rectCur.x = rectBack.x + 230;
	rectCur.y = rectBack.y + 155;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }
    // кнопка OKAY
    if(available){
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
	action.pf = ActionPressOK;
	AddActionEvent(&dialog, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }else{
	FillSPRITE(&sprite, "RECRUIT.ICN", 9);
	image = GetICNSprite(&sprite);
	rectCur.x = rectBack.x + 34;
	rectCur.y = rectBack.y + 249;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }
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
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    if(available){
	// WEEL UP
	rectCur.x = rectBack.x + 20;
	rectCur.y = rectBack.y + 20;
	rectCur.w = rectBack.w - 20;
	rectCur.h = rectBack.h - 20;
	action.rect = rectCur;
	action.mouseEvent = MOUSE_UWHEEL;
	action.pf = ActionPressUP;
	AddActionEvent(&dialog, &action);
	// WEEL DOWN
	action.rect = rectCur;
	action.mouseEvent = MOUSE_DWHEEL;
	action.pf = ActionPressDOWN;
	AddActionEvent(&dialog, &action);
    }

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionCycle(dialog, NULL)){

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
		    if(payment.wood)
			resultCountResource += payment.wood;
		    else if(payment.ore)
			resultCountResource += payment.ore;
		    else if(payment.mercury)
			resultCountResource += payment.mercury;
		    else if(payment.crystal)
			resultCountResource += payment.crystal;
		    else if(payment.sulfur)
			resultCountResource += payment.sulfur;
		    else if(payment.gems)
			resultCountResource += payment.gems;
		    resultCountGold += payment.gold;
		    CursorOff();
		    RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);
		    CursorOn();
		}
    		break;

    	    case DOWN:
		if(resultRecrutMonster){
		    resultRecrutMonster--;
		    countAvailable++;
		    if(payment.wood)
			resultCountResource -= payment.wood;
		    else if(payment.ore)
			resultCountResource -= payment.ore;
		    else if(payment.mercury)
			resultCountResource -= payment.mercury;
		    else if(payment.crystal)
			resultCountResource -= payment.crystal;
		    else if(payment.sulfur)
			resultCountResource -= payment.sulfur;
		    else if(payment.gems)
			resultCountResource -= payment.gems;
		    resultCountGold -= payment.gold;
		    CursorOff();
		    RedrawDinamicRecrutMonster(stat, &rectBack, emonster, countAvailable, resultRecrutMonster, resultCountResource, resultCountGold);
		    CursorOn();
		}
    		break;

    	    case MAX:
		while(countAvailable && CheckBuyMonsterFromCastle(castle, levelMonster, resultRecrutMonster + 1)){
		    resultRecrutMonster++;
		    countAvailable--;
		    if(payment.wood)
			resultCountResource += payment.wood;
		    else if(payment.ore)
			resultCountResource += payment.ore;
		    else if(payment.mercury)
			resultCountResource += payment.mercury;
		    else if(payment.crystal)
			resultCountResource += payment.crystal;
		    else if(payment.sulfur)
			resultCountResource += payment.sulfur;
		    else if(payment.gems)
			resultCountResource += payment.gems;
		    resultCountGold += payment.gold;
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

    S_PAYMENT payment = *(PaymentConditionsMonster(emonster));
    // cost per troop
    if( payment.wood ||
        payment.ore ||
	payment.crystal ||
        payment.sulfur ||
        payment. mercury ||
        payment.gems ){
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

void DialogRecrutMonsterInfo(E_MONSTER emonster, Uint16 available){

    CursorOff();
    SetIntValue(ANIM3, FALSE);
    
    SDL_Surface *back, *image, *video;
    SDL_Event event;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    BOOL exit = FALSE;
    char str[32];

    S_PAYMENT payment = *(PaymentConditionsMonster(emonster));

    FillSPRITE(&sprite, "RECR2BKG.ICN", 0);
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
	return;
    }

    // сохраняем бакграунд
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // отображаем фон диалога
    SDL_BlitSurface(image, NULL, video, &rectBack);

    // картинка монстра
    FillSPRITE(&sprite, MonsterBigNamePortrait(emonster), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 70 - image->w / 2;
    rectCur.y = rectBack.y + 120 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text available
    sprintf(str, "Available: %d", available);
    rectCur.y = rectBack.y + 130;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    rectCur.x = rectBack.x + 70 - rectCur.w / 2;
    PrintAlignText(video, &rectCur, str, FONT_SMALL);

    // cost per troop
    if( payment.wood ||
        payment.ore ||
	payment.crystal ||
        payment.sulfur ||
        payment. mercury ||
        payment.gems ){

	// спрайт золото (условия)
	FillSPRITE(&sprite, "RESOURCE.ICN", 6);
	image = GetICNSprite(&sprite);
	rectCur.x = rectBack.x + 150;
	rectCur.y = rectBack.y + 75;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// количество золото (условия)
	sprintf(str, "%d", payment.gold);
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
	// количество золото (условия)
	sprintf(str, "%d", payment.gold);
	rectCur.x = rectBack.x + 205;
	rectCur.y = rectBack.y + 103;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	rectCur.x = rectCur.x - rectCur.w / 2;
	PrintText(video, &rectCur, str, FONT_SMALL);
    }

    if(payment.crystal){

	    // спрайт crystal (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 4);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество crystal (условия)
	    sprintf(str, "%d", payment.crystal);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.mercury){

	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 72;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество mercury (условия)
	    sprintf(str, "%d", payment.mercury);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.wood){

	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 0);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 72;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество mercury (условия)
	    sprintf(str, "%d", payment.wood);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.ore){

	    // спрайт mercury (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 72;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество mercury (условия)
	    sprintf(str, "%d", payment.ore);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.sulfur){

	    // спрайт sulfur (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество sulfur (условия)
	    sprintf(str, "%d", payment.sulfur);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);

    }else if(payment.gems){

	    // спрайт gems (условия)
	    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
	    image = GetICNSprite(&sprite);
	    rectCur.x = rectBack.x + 225;
	    rectCur.y = rectBack.y + 75;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    // количество gems (условия)
	    sprintf(str, "%d", payment.gems);
	    rectCur.x = rectBack.x + 240;
	    rectCur.y = rectBack.y + 103;
	    rectCur.w = GetLengthText(str, FONT_SMALL);
	    rectCur.h = FONT_HEIGHTSMALL;
	    rectCur.x = rectCur.x - rectCur.w / 2;
	    PrintText(video, &rectCur, str, FONT_SMALL);
    }

    // Отрисовка диалога
    //SDL_Flip(video);
    CursorOn();
    
    // цикл событий
    while(! exit){
    
        while(SDL_PollEvent(&event))
            if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;
                        
        if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
    }
    
    CursorOff();
                                                                    
    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    SDL_FreeSurface(back);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);

    CursorOn();
}
