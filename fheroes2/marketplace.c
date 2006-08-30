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
#include "payment.h"
#include "kingdom.h"
#include "config.h"
#include "resource.h"
#include "actionevent.h"
#include "marketplace.h"

typedef enum {UNFOCUS, COSTLY, UNCOSTLY, GOLD} E_FOCUSTRADE;

struct {
    E_FOCUSTRADE        select;
    E_RESOURCE		resource;
    SDL_Rect            rect;
    SDL_Surface         *background;
    SDL_Surface         *cursor;
    BOOL		use;
} tradeFrom, tradeTo;

void	RedrawTradeFromResource(void);
void	RedrawTradeToResource(void);
void	TradeRestoreFromCursor(void);
void	TradeRestoreToCursor(void);
void	TradeSaveFromCursor(void);
void	TradeSaveToCursor(void);

void	TradeCostToString(char *, E_FOCUSTRADE, E_FOCUSTRADE);
Uint16	GetTradeFactor(E_FOCUSTRADE, E_FOCUSTRADE);

ACTION	ActionPressFromUncostly(void);
ACTION	ActionPressFromCostly(void);
ACTION	ActionPressFromGold(void);

ACTION	ActionPressToUncostly(void);
ACTION	ActionPressToCostly(void);
ACTION	ActionPressToGold(void);

INTERFACEACTION *actionTrade = NULL;
E_COLORS	colorKingdom;

ACTION ShowMarketplace(E_COLORS color){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    INTERFACEACTION action;
    BOOL exit = FALSE;
    ACTION result = NONE;
    AGGSPRITE sprite;
    Uint8 i;
    
    actionTrade = NULL;
    colorKingdom = color;

    const char *buybuild = "BUYBUILD.ICN";
    const char *trade = "TRADPOST.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	trade = "TRADPOSE.ICN";
    }

    // инициализация курсора
    FillSPRITE(&sprite, trade, 14);
    tradeFrom.select = UNFOCUS;
    tradeFrom.background = NULL;
    tradeFrom.cursor = GetICNSprite(&sprite);
    tradeFrom.use = FALSE;
    tradeFrom.rect.x = 0;
    tradeFrom.rect.y = 0;
    tradeFrom.rect.w = tradeFrom.cursor->w;
    tradeFrom.rect.h = tradeFrom.cursor->h;
    tradeTo.select = UNFOCUS;
    tradeTo.background = NULL;
    tradeTo.cursor = GetICNSprite(&sprite);
    tradeTo.use = FALSE;
    tradeTo.rect.x = 0;
    tradeTo.rect.y = 0;
    tradeTo.rect.w = tradeTo.cursor->w;
    tradeTo.rect.h = tradeTo.cursor->h;
    if(NULL == (tradeFrom.background = SDL_CreateRGBSurface(SDL_SWSURFACE, tradeFrom.cursor->w, tradeFrom.cursor->h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "ShowMarketplace: CreateRGBSurface failed: %s\n", SDL_GetError());
            return EXIT;
    }
    if(NULL == (tradeTo.background = SDL_CreateRGBSurface(SDL_SWSURFACE, tradeTo.cursor->w, tradeTo.cursor->h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "ShowMarketplace: CreateRGBSurface failed: %s\n", SDL_GetError());
            return EXIT;
    }
    
    const char *info = "Please inspect our fine wares. If you feel like offering a trade, click on the items you wish to trade with and for.";
    const char *header = GetStringBuilding(KNIGHT, BUILD_MARKETPLACE);
    
    Uint16 height = 0;
    Uint16 width = 0;
    Uint8 count = 0;
    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    width += image->w;
    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    width += image->w;

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    // количество средних блоков
    count = 6;
    height = height + count * image->h;

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    rectBack.w = width;
    rectBack.h = height;
    // поправка на верхний и нижний
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectBack.h += image->h;
    rectBack.x = (video->w - rectBack.w) / 2;
    rectBack.y = (video->h - rectBack.h) / 2;

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "ShowMarketplace: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }

    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    if(GetIntValue(EVILINTERFACE)) rectCur.x = rectBack.x; else rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.y += rectCur.h;
    rectCur.h = image->h;
    for(i = 0; i < count; ++i){
	SDL_BlitSurface(image, NULL, video, &rectCur);
	rectCur.y += rectCur.h;
    }

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    
    // text header
    rectCur.x = rectBack.x + 35;
    if(GetIntValue(EVILINTERFACE)) rectCur.y = rectBack.y + 50; else rectCur.y = rectBack.y + 65;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 70;
    PrintAlignText(video, &rectCur, header, FONT_BIG);

    // text info
    rectCur.x = rectBack.x + 35;
    if(GetIntValue(EVILINTERFACE)) rectCur.y = rectBack.y + 80; else rectCur.y = rectBack.y + 95;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 90;
    PrintAlignText(video, &rectCur, info, FONT_BIG);

    // кнопка EXIT
    FillSPRITE(&sprite, trade, 17);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = rectBack.y + rectBack.h - 70;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, trade, 17);
    FillSPRITE(&action.objectPush, trade, 18);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // Отрисовка диалога
    SDL_Flip(video);

    RedrawTradeFromResource();
    RedrawTradeToResource();

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    while(! exit)
    
        switch(ActionCycle(actionTrade, NULL)){
    
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
    //SDL_Flip(video);

    FreeActionEvent(actionTrade);
    SDL_FreeSurface(back);

    if(tradeFrom.background) SDL_FreeSurface(tradeFrom.background);
    if(tradeTo.background) SDL_FreeSurface(tradeTo.background);

    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

void RedrawTradeFromResource(void){

    SDL_Surface *image;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rectCur;
    AGGSPRITE sprite;
    INTERFACEACTION action;
    Uint8 i;
    Uint16 cx, cy;
    S_PAYMENT resource;
    GetKingdomAllResource(colorKingdom, &resource);
    
    const char *header = "Your Resources";
    const char *trade = "TRADPOST.ICN";
    char number[8];
    if(GetIntValue(EVILINTERFACE)) trade = "TRADPOSE.ICN";

    RemoveActionLevelEvent(actionTrade, LEVELEVENT_MARKETPLACE1);

    cx = video->w / 2 - 120;
    cy = video->h / 2 + 10;
    rectCur.x = cx;
    rectCur.y = cy;
    rectCur.w = 120;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintAlignText(video, &rectCur, header, FONT_SMALL);

    // image wood ore mercury
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 7 + i);
	image = GetICNSprite(&sprite);
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = cx + i * (rectCur.w + 6);
	rectCur.y = cy + 20;

	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.level = LEVELEVENT_MARKETPLACE1;
	if(1 == i)
	    action.pf = ActionPressFromCostly;
	else
	    action.pf = ActionPressFromUncostly;
	AddActionEvent(&actionTrade, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i)
	    sprintf(number, "%d", resource.wood);
	else if(1 == i)
	    sprintf(number, "%d", resource.mercury);
	else
	    sprintf(number, "%d", resource.ore);
	rectCur.x += (image->w - GetLengthText(number, FONT_SMALL)) / 2;
	rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
	rectCur.w = GetLengthText(number, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintAlignText(video, &rectCur, number, FONT_SMALL);
    }

    // image sulfur crytal gems
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 10 + i);
	image = GetICNSprite(&sprite);
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = cx + i * (rectCur.w + 6);
	rectCur.y = cy + 60;

	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.level = LEVELEVENT_MARKETPLACE1;
	action.pf = ActionPressFromCostly;
	AddActionEvent(&actionTrade, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i)
	    sprintf(number, "%d", resource.sulfur);
	else if(1 == i)
	    sprintf(number, "%d", resource.crystal);
	else
	    sprintf(number, "%d", resource.gems);
	rectCur.x += (image->w - GetLengthText(number, FONT_SMALL)) / 2;
	rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
	rectCur.w = GetLengthText(number, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintAlignText(video, &rectCur, number, FONT_SMALL);
    }

    // image gold
    FillSPRITE(&sprite, trade, 10 + i);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    rectCur.x = cx + rectCur.w + 6;
    rectCur.y = cy + 100;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.level = LEVELEVENT_MARKETPLACE1;
    action.pf = ActionPressFromGold;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // cost
    sprintf(number, "%d", resource.gold);
    tradeFrom.resource = GOLDS;
    rectCur.x += (image->w - GetLengthText(number, FONT_SMALL)) / 2;
    rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
    rectCur.w = GetLengthText(number, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintAlignText(video, &rectCur, number, FONT_SMALL);
}

void RedrawTradeToResource(void){

    SDL_Surface *image;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rectCur;
    AGGSPRITE sprite;
    INTERFACEACTION action;
    Uint8 i;
    Uint16 cx, cy;
    S_PAYMENT resource;
    GetKingdomAllResource(colorKingdom, &resource);
    
    const char *header = "Available Trades";
    const char *trade = "TRADPOST.ICN";
    char string[8];
    if(GetIntValue(EVILINTERFACE)) trade = "TRADPOSE.ICN";

    RemoveActionLevelEvent(actionTrade, LEVELEVENT_MARKETPLACE2);

    cx = video->w / 2 + 10;
    cy = video->h / 2 + 10;
    rectCur.x = cx;
    rectCur.y = cy;
    rectCur.w = 120;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintAlignText(video, &rectCur, header, FONT_SMALL);

    // image wood ore mercury
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 7 + i);
	image = GetICNSprite(&sprite);
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = cx + i * (rectCur.w + 6);
	rectCur.y = cy + 20;

	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.level = LEVELEVENT_MARKETPLACE2;
	if(1 == i)
	    action.pf = ActionPressToCostly;
	else
	    action.pf = ActionPressToUncostly;
	AddActionEvent(&actionTrade, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i){
	    tradeTo.resource = WOOD;
	    TradeCostToString(string, tradeFrom.select, UNCOSTLY);
	}else if(1 == i){
	    tradeTo.resource = MERCURY;
	    TradeCostToString(string, tradeFrom.select, COSTLY);
	}else{
	    tradeTo.resource = ORE;
	    TradeCostToString(string, tradeFrom.select, UNCOSTLY);
	}
	rectCur.x += (image->w - GetLengthText(string, FONT_SMALL)) / 2;
	rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
	rectCur.w = GetLengthText(string, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintAlignText(video, &rectCur, string, FONT_SMALL);
    }

    // image sulfur crytal gems
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 10 + i);
	image = GetICNSprite(&sprite);
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = cx + i * (rectCur.w + 6);
	rectCur.y = cy + 60;

	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.level = LEVELEVENT_MARKETPLACE2;
	action.pf = ActionPressToCostly;
	AddActionEvent(&actionTrade, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i)
	    tradeTo.resource = SULFUR;
	else if(1 == i)
	    tradeTo.resource = CRYSTAL;
	else if(2 == i)
	    tradeTo.resource = GEMS;
	TradeCostToString(string, tradeFrom.select, COSTLY);
	rectCur.x += (image->w - GetLengthText(string, FONT_SMALL)) / 2;
	rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
	rectCur.w = GetLengthText(string, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintAlignText(video, &rectCur, string, FONT_SMALL);
    }

    // image gold
    FillSPRITE(&sprite, trade, 10 + i);
    image = GetICNSprite(&sprite);
    rectCur.w = image->w;
    rectCur.h = image->h;
    rectCur.x = cx + rectCur.w + 6;
    rectCur.y = cy + 100;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.level = LEVELEVENT_MARKETPLACE2;
    action.pf = ActionPressToGold;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // cost
    tradeTo.resource = GOLDS;
    TradeCostToString(string, tradeFrom.select, GOLD);
    rectCur.x += (image->w - GetLengthText(string, FONT_SMALL)) / 2;
    rectCur.y += image->h - FONT_HEIGHTSMALL - 2;
    rectCur.w = GetLengthText(string, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintAlignText(video, &rectCur, string, FONT_SMALL);
}

ACTION  ActionPressFromUncostly(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    tradeFrom.select = UNCOSTLY;

    RedrawTradeToResource();

    CursorOn();

    return NONE;
}

ACTION  ActionPressFromCostly(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    tradeFrom.select = COSTLY;

    RedrawTradeToResource();

    CursorOn();

    return NONE;
}

ACTION  ActionPressFromGold(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    tradeFrom.select = GOLD;

    RedrawTradeToResource();

    CursorOn();

    return NONE;
}

ACTION  ActionPressToUncostly(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = UNCOSTLY;

    CursorOn();

    return NONE;
}

ACTION  ActionPressToCostly(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = COSTLY;

    CursorOn();

    return NONE;
}

ACTION  ActionPressToGold(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = GOLD;

    CursorOn();

    return NONE;
}

void TradeRestoreFromCursor(void){

    SDL_Surface *video = SDL_GetVideoSurface();

    if(tradeFrom.use) SDL_BlitSurface(tradeFrom.background, NULL, video, &tradeFrom.rect);
    tradeFrom.select = UNFOCUS;
    tradeFrom.use = FALSE;
}

void TradeRestoreToCursor(void){

    SDL_Surface *video = SDL_GetVideoSurface();

    if(tradeTo.use) SDL_BlitSurface(tradeTo.background, NULL, video, &tradeTo.rect);
    tradeTo.select = UNFOCUS;
    tradeTo.use = FALSE;
}

void TradeSaveFromCursor(void){

    SDL_Surface *video =SDL_GetVideoSurface();
    Sint32 mx, my;
    SDL_Rect rect;
        
    SDL_GetMouseState(&mx, &my);

    rect.x = video->w / 2 - 120;
    rect.y = video->h / 2 + 30;
    rect.w = 34;
    rect.h = 34;

    //wood
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = UNCOSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = WOOD;
	return;
    }
    rect.x += 40;
    // mercury
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = COSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = MERCURY;
	return;
    }
    rect.x += 40;
    // ore
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = UNCOSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = ORE;
	return;
    }
    
    rect.y += 40;
    rect.x = video->w / 2 - 120;
    // sulfur
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = COSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = SULFUR;
	return;
    }
    rect.x += 40;
    // crystal
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = COSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = CRYSTAL;
	return;
    }
    rect.x += 40;
    // gems
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = COSTLY;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = GEMS;
	return;
    }

    rect.y += 40;
    rect.x = video->w / 2 - 80;
    // gold
    if(ValidPoint(&rect, mx, my)){
	tradeFrom.select = GOLD;
	tradeFrom.rect.x = rect.x - 2;
	tradeFrom.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeFrom.rect, tradeFrom.background, NULL);
	tradeFrom.use = TRUE;
	SDL_BlitSurface(tradeFrom.cursor, NULL, video, &tradeFrom.rect);
	tradeFrom.resource = GOLDS;
	return;
    }
}

void TradeSaveToCursor(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    Sint32 mx, my;
    SDL_Rect rect;
        
    SDL_GetMouseState(&mx, &my);

    rect.x = video->w / 2 + 10;
    rect.y = video->h / 2 + 30;
    rect.w = 34;
    rect.h = 34;

    //wood
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = UNCOSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
    rect.x += 40;
    // mercury
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = COSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
    rect.x += 40;
    // ore
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = UNCOSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
    
    rect.y += 40;
    rect.x = video->w / 2 + 10;
    // sulfur
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = COSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
    rect.x += 40;
    // crystal
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = COSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
    rect.x += 40;
    // gems
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = COSTLY;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }

    rect.y += 40;
    rect.x = video->w / 2 + 50;
    // gold
    if(ValidPoint(&rect, mx, my)){
	tradeTo.select = GOLD;
	tradeTo.rect.x = rect.x - 2;
	tradeTo.rect.y = rect.y - 2;
	SDL_BlitSurface(video, &tradeTo.rect, tradeTo.background, NULL);
	tradeTo.use = TRUE;
	SDL_BlitSurface(tradeTo.cursor, NULL, video, &tradeTo.rect);
	return;
    }
}

void TradeCostToString(char *string, E_FOCUSTRADE from, E_FOCUSTRADE to){

    if(! string) return;

    if(0 == GetTradeFactor(from, to)){
	sprintf(string, "n/a");
	return;
    }
    Uint8 market = GetCountMarketplace(colorKingdom);

    switch(from){

	default:
	    break;

	case COSTLY:
	    if(GOLD == to){
		if(1 == market) sprintf(string, "%d", SALE_COSTLY1);
		else if(2 == market) sprintf(string, "%d", SALE_COSTLY2);
		else if(3 == market) sprintf(string, "%d", SALE_COSTLY3);
		else if(4 == market) sprintf(string, "%d", SALE_COSTLY4);
		else if(5 == market) sprintf(string, "%d", SALE_COSTLY5);
		else if(6 == market) sprintf(string, "%d", SALE_COSTLY6);
		else if(7 == market) sprintf(string, "%d", SALE_COSTLY7);
		else if(8 == market) sprintf(string, "%d", SALE_COSTLY8);
		else if(8 <  market) sprintf(string, "%d", SALE_COSTLY9);
	    }
	    else if(COSTLY == to){
		if(1 == market) sprintf(string, "1/%d", COSTLY_COSTLY1);
		else if(2 == market) sprintf(string, "1/%d", COSTLY_COSTLY2);
		else if(3 == market) sprintf(string, "1/%d", COSTLY_COSTLY3);
		else if(4 == market) sprintf(string, "1/%d", COSTLY_COSTLY4);
		else if(5 == market) sprintf(string, "1/%d", COSTLY_COSTLY5);
		else if(6 == market) sprintf(string, "1/%d", COSTLY_COSTLY6);
		else if(7 == market) sprintf(string, "1/%d", COSTLY_COSTLY7);
		else if(8 == market) sprintf(string, "1/%d", COSTLY_COSTLY8);
		else if(8 <  market) sprintf(string, "1/%d", COSTLY_COSTLY9);
	    }
	    else if(UNCOSTLY == to){
		if(1 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY1);
		else if(2 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY2);
		else if(3 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY3);
		else if(4 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY4);
		else if(5 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY5);
		else if(6 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY6);
		else if(7 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY7);
		else if(8 == market) sprintf(string, "1/%d", COSTLY_UNCOSTLY8);
		else if(8 <  market) sprintf(string, "1/%d", COSTLY_UNCOSTLY9);
	    }
	    break;

	case UNCOSTLY:
	    if(GOLD == to){
		if(1 == market) sprintf(string, "%d", SALE_UNCOSTLY1);
		else if(2 == market) sprintf(string, "%d", SALE_UNCOSTLY2);
		else if(3 == market) sprintf(string, "%d", SALE_UNCOSTLY3);
		else if(4 == market) sprintf(string, "%d", SALE_UNCOSTLY4);
		else if(5 == market) sprintf(string, "%d", SALE_UNCOSTLY5);
		else if(6 == market) sprintf(string, "%d", SALE_UNCOSTLY6);
		else if(7 == market) sprintf(string, "%d", SALE_UNCOSTLY7);
		else if(8 == market) sprintf(string, "%d", SALE_UNCOSTLY8);
		else if(8 <  market) sprintf(string, "%d", SALE_UNCOSTLY9);
	    }
	    else if(UNCOSTLY == to){
		if(1 == market) sprintf(string, "1/%d", COSTLY_COSTLY1);
		else if(2 == market) sprintf(string, "1/%d", COSTLY_COSTLY2);
		else if(3 == market) sprintf(string, "1/%d", COSTLY_COSTLY3);
		else if(4 == market) sprintf(string, "1/%d", COSTLY_COSTLY4);
		else if(5 == market) sprintf(string, "1/%d", COSTLY_COSTLY5);
		else if(6 == market) sprintf(string, "1/%d", COSTLY_COSTLY6);
		else if(7 == market) sprintf(string, "1/%d", COSTLY_COSTLY7);
		else if(8 == market) sprintf(string, "1/%d", COSTLY_COSTLY8);
		else if(8 <  market) sprintf(string, "1/%d", COSTLY_COSTLY9);
	    }
	    else if(COSTLY == to){
		if(1 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY1);
		else if(2 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY2);
		else if(3 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY3);
		else if(4 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY4);
		else if(5 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY5);
		else if(6 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY6);
		else if(7 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY7);
		else if(8 == market) sprintf(string, "1/%d", UNCOSTLY_COSTLY8);
		else if(8 <  market) sprintf(string, "1/%d", UNCOSTLY_COSTLY9);
	    }
	    break;

	case GOLD:
	    if(UNCOSTLY == to){
		if(1 == market) sprintf(string, "%d", BUY_UNCOSTLY1);
		else if(2 == market) sprintf(string, "%d", BUY_UNCOSTLY2);
		else if(3 == market) sprintf(string, "%d", BUY_UNCOSTLY3);
		else if(4 == market) sprintf(string, "%d", BUY_UNCOSTLY4);
		else if(5 == market) sprintf(string, "%d", BUY_UNCOSTLY5);
		else if(6 == market) sprintf(string, "%d", BUY_UNCOSTLY6);
		else if(7 == market) sprintf(string, "%d", BUY_UNCOSTLY7);
		else if(8 == market) sprintf(string, "%d", BUY_UNCOSTLY8);
		else if(8 <  market) sprintf(string, "%d", BUY_UNCOSTLY9);
	    }
	    else if(COSTLY == to){
		if(1 == market) sprintf(string, "%d", BUY_COSTLY1);
		else if(2 == market) sprintf(string, "%d", BUY_COSTLY2);
		else if(3 == market) sprintf(string, "%d", BUY_COSTLY3);
		else if(4 == market) sprintf(string, "%d", BUY_COSTLY4);
		else if(5 == market) sprintf(string, "%d", BUY_COSTLY5);
		else if(6 == market) sprintf(string, "%d", BUY_COSTLY6);
		else if(7 == market) sprintf(string, "%d", BUY_COSTLY7);
		else if(8 == market) sprintf(string, "%d", BUY_COSTLY8);
		else if(8 <  market) sprintf(string, "%d", BUY_COSTLY9);
	    }
	    break;
    }
}

Uint16 GetTradeFactor(E_FOCUSTRADE from, E_FOCUSTRADE to){

    Uint8 market = GetCountMarketplace(colorKingdom);

    if( tradeFrom.resource == tradeTo.resource ||
	UNFOCUS == from ||
	!market ){
	return 0;
    }

    S_PAYMENT resource;
    GetKingdomAllResource(colorKingdom, &resource);

    switch(from){

	default:
	    break;

	case COSTLY:
	    if(GOLD == to){
		if(1 == market) return SALE_COSTLY1;
		else if(2 == market) return SALE_COSTLY2;
		else if(3 == market) return SALE_COSTLY3;
		else if(4 == market) return SALE_COSTLY4;
		else if(5 == market) return SALE_COSTLY5;
		else if(6 == market) return SALE_COSTLY6;
		else if(7 == market) return SALE_COSTLY7;
		else if(8 == market) return SALE_COSTLY8;
		else if(8 <  market) return SALE_COSTLY9;
	    }
	    else if(COSTLY == to){
		if(1 == market){
		    if(COSTLY_COSTLY1 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY1;
		}else if(2 == market){
		    if(COSTLY_COSTLY2 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY2;
		}else if(3 == market){
		    if(COSTLY_COSTLY3 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY3;
		}else if(4 == market){
		    if(COSTLY_COSTLY4 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY4;
		}else if(5 == market){
		    if(COSTLY_COSTLY5 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY5;
		}else if(6 == market){
		    if(COSTLY_COSTLY6 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY6;
		}else if(7 == market){
		    if(COSTLY_COSTLY7 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY7;
		}else if(8 == market){
		    if(COSTLY_COSTLY8 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY8;
		}else if(8 < market){
		    if(COSTLY_COSTLY9 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY9;
		}
	    }
	    else if(UNCOSTLY == to){
		if(1 == market){
		    if(COSTLY_UNCOSTLY1 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY1;
		}else if(2 == market){
		    if(COSTLY_UNCOSTLY2 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY2;
		}else if(3 == market){
		    if(COSTLY_UNCOSTLY3 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY3;
		}else if(4 == market){
		    if(COSTLY_UNCOSTLY4 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY4;
		}else if(5 == market){
		    if(COSTLY_UNCOSTLY5 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY5;
		}else if(6 == market){
		    if(COSTLY_UNCOSTLY6 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY6;
		}else if(7 == market){
		    if(COSTLY_UNCOSTLY7 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY7;
		}else if(8 == market){
		    if(COSTLY_UNCOSTLY8 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY8;
		}else if(8 < market){
		    if(COSTLY_UNCOSTLY9 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_UNCOSTLY9;
		}
	    }
	    break;

	case UNCOSTLY:
	    if(GOLD == to){
		if(1 == market) return SALE_UNCOSTLY1;
		else if(2 == market) return SALE_UNCOSTLY2;
		else if(3 == market) return SALE_UNCOSTLY3;
		else if(4 == market) return SALE_UNCOSTLY4;
		else if(5 == market) return SALE_UNCOSTLY5;
		else if(6 == market) return SALE_UNCOSTLY6;
		else if(7 == market) return SALE_UNCOSTLY7;
		else if(8 == market) return SALE_UNCOSTLY8;
		else if(8 < market) return SALE_UNCOSTLY9;
	    }
	    else if(UNCOSTLY == to){
		if(1 == market){
		    if(COSTLY_COSTLY1 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY1;
		}else if(2 == market){
		    if(COSTLY_COSTLY2 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY2;
		}else if(3 == market){
		    if(COSTLY_COSTLY3 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY3;
		}else if(4 == market){
		    if(COSTLY_COSTLY4 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY4;
		}else if(5 == market){
		    if(COSTLY_COSTLY5 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY5;
		}else if(6 == market){
		    if(COSTLY_COSTLY6 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY6;
		}else if(7 == market){
		    if(COSTLY_COSTLY7 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY7;
		}else if(8 == market){
		    if(COSTLY_COSTLY8 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY8;
		}else if(8 < market){
		    if(COSTLY_COSTLY9 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return COSTLY_COSTLY9;
		}
	    }
	    else if(COSTLY == to){
		if(1 == market){
		    if(UNCOSTLY_COSTLY1 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY1;
		}else if(2 == market){
		    if(UNCOSTLY_COSTLY2 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY2;
		}else if(3 == market){
		    if(UNCOSTLY_COSTLY3 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY3;
		}else if(4 == market){
		    if(UNCOSTLY_COSTLY4 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY4;
		}else if(5 == market){
		    if(UNCOSTLY_COSTLY5 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY5;
		}else if(6 == market){
		    if(UNCOSTLY_COSTLY6 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY6;
		}else if(7 == market){
		    if(UNCOSTLY_COSTLY7 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY7;
		}else if(8 == market){
		    if(UNCOSTLY_COSTLY8 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY8;
		}else if(8 < market){
		    if(UNCOSTLY_COSTLY9 > GetKingdomResource(colorKingdom, tradeFrom.resource)) return 0; else return UNCOSTLY_COSTLY9;
		}
	    }
	    break;

	case GOLD:
	    if(UNCOSTLY == to){
		if(1 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY1) return 0; else return BUY_UNCOSTLY1;
		}else if(2 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY2) return 0; else return BUY_UNCOSTLY2;
		}else if(3 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY3) return 0; else return BUY_UNCOSTLY3;
		}else if(4 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY4) return 0; else return BUY_UNCOSTLY4;
		}else if(5 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY5) return 0; else return BUY_UNCOSTLY5;
		}else if(6 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY6) return 0; else return BUY_UNCOSTLY6;
		}else if(7 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY7) return 0; else return BUY_UNCOSTLY7;
		}else if(8 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY8) return 0; else return BUY_UNCOSTLY8;
		}else if(8 < market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_UNCOSTLY9) return 0; else return BUY_UNCOSTLY9;
		}
	    }
	    else if(COSTLY == to){
		if(1 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY1) return 0; else return BUY_COSTLY1;
		}else if(2 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY2) return 0; else return BUY_COSTLY2;
		}else if(3 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY3) return 0; else return BUY_COSTLY3;
		}else if(4 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY4) return 0; else return BUY_COSTLY4;
		}else if(5 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY5) return 0; else return BUY_COSTLY5;
		}else if(6 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY6) return 0; else return BUY_COSTLY6;
		}else if(7 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY7) return 0; else return BUY_COSTLY7;
		}else if(8 == market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY8) return 0; else return BUY_COSTLY8;
		}else if(8 <  market){
		    if(GetKingdomResource(colorKingdom, tradeFrom.resource) < BUY_COSTLY9) return 0; else return BUY_COSTLY9;
		}
	    }
	    break;
    }

    return 0;
}
