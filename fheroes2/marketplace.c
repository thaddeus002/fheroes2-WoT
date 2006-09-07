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
#include "spliter.h"
#include "box.h"
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

struct {
    SDL_Surface		*surface;
    SDL_Rect		rect;
}backgroundAction;

void	RedrawTradeFromResource(void);
void	RedrawTradeToResource(void);
void	RedrawActionTrade(void);
void	RedrawSpriteResource(void);
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
S_SPLITER 	*spliterMarket = NULL;
E_COLORS	colorKingdom;

ACTION ShowMarketplace(E_COLORS color){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rect;
    BOOL exit = FALSE;
    INTERFACEACTION action;
    ACTION result = NONE;
    AGGSPRITE sprite;
    S_PAYMENT resource;
    S_BOX box;
    Uint16 countFrom = 0;
    Uint16 countTo = 0;

    Uint8 i;
    Uint16 cx, cy;

    actionTrade = NULL;
    backgroundAction.surface = NULL;
    colorKingdom = color;

    const char *trade = NULL;
    trade = (GetIntValue(EVILINTERFACE) ? "TRADPOSE.ICN" : "TRADPOST.ICN");

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

    // инициализация сплитера
    FillSPRITE(&sprite, trade, 2);
    spliterMarket = InitSpliter(&sprite);
    if(NULL == spliterMarket) return EXIT;
    

    if( ! InitBox(&box, 250, &actionTrade, EXIT)) return NONE;

    // второй бекграунд для затирки
    rect = box.rectArea;
    backgroundAction.rect = rect;
    backgroundAction.rect.x -= 5;
    backgroundAction.rect.w += 10;
    backgroundAction.rect.h = 180;
    if(NULL == (backgroundAction.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, backgroundAction.rect.w, backgroundAction.rect.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "ShowMarketplace: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }
    SDL_BlitSurface(video, &backgroundAction.rect, backgroundAction.surface, NULL);


    // ACTION
    cx = video->w / 2 - 115;
    cy = video->h / 2;
    // from action: wood ore mercury
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 7 + i);
	image = GetICNSprite(&sprite);
	rect.w = image->w;
	rect.h = image->h;
	rect.x = cx + i * (rect.w + 6);
	rect.y = cy + 20;
	ZeroINTERFACEACTION(&action);
	action.rect = rect;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = (1 == i ? ActionPressFromCostly : ActionPressFromUncostly);
	AddActionEvent(&actionTrade, &action);
    }
    // from action: sulfur crytal gems
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 10 + i);
	image = GetICNSprite(&sprite);
	rect.w = image->w;
	rect.h = image->h;
	rect.x = cx + i * (rect.w + 6);
	rect.y = cy + 60;
	ZeroINTERFACEACTION(&action);
	action.rect = rect;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressFromCostly;
	AddActionEvent(&actionTrade, &action);
    }
    // from action: gold
    FillSPRITE(&sprite, trade, 10 + i);
    image = GetICNSprite(&sprite);
    rect.w = image->w;
    rect.h = image->h;
    rect.x = cx + rect.w + 6;
    rect.y = cy + 100;
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressFromGold;
    AddActionEvent(&actionTrade, &action);
    //
    cx = video->w / 2 + 10;
    cy = video->h / 2;
    // to action: wood ore mercury
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 7 + i);
	image = GetICNSprite(&sprite);
	rect.w = image->w;
	rect.h = image->h;
	rect.x = cx + i * (rect.w + 6);
	rect.y = cy + 20;
	ZeroINTERFACEACTION(&action);
	action.rect = rect;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = (1 == i ? ActionPressToCostly : ActionPressToUncostly);
	AddActionEvent(&actionTrade, &action);
    }
    // to action: sulfur crytal gems
    for(i = 0; i < 3; ++i){
	FillSPRITE(&sprite, trade, 10 + i);
	image = GetICNSprite(&sprite);
	rect.w = image->w;
	rect.h = image->h;
	rect.x = cx + i * (rect.w + 6);
	rect.y = cy + 60;
	ZeroINTERFACEACTION(&action);
	action.rect = rect;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressToCostly;
	AddActionEvent(&actionTrade, &action);
    }
    // to action: gold
    FillSPRITE(&sprite, trade, 10 + i);
    image = GetICNSprite(&sprite);
    rect.w = image->w;
    rect.h = image->h;
    rect.x = cx + rect.w + 6;
    rect.y = cy + 100;
    ZeroINTERFACEACTION(&action);
    action.rect = rect;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressToGold;
    AddActionEvent(&actionTrade, &action);

    RedrawActionTrade();
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
	    case LEFT:
		MoveBackwardSpliter(spliterMarket);
		RedrawSpriteResource();
		break;

	    case RIGHT:
		MoveForwardSpliter(spliterMarket);
		RedrawSpriteResource();
		break;
		
	    case YES:
		if(tradeTo.resource != GOLDS){
		    countFrom = GetCurrentSpliter(spliterMarket) * GetTradeFactor(tradeFrom.select, tradeTo.select);
		    countTo = GetCurrentSpliter(spliterMarket);
		}else{
		    countTo = GetCurrentSpliter(spliterMarket) * GetTradeFactor(tradeFrom.select, tradeTo.select);
		    countFrom = GetCurrentSpliter(spliterMarket);
		}
		resource.ore = 0;
		resource.wood = 0;
		resource.mercury = 0;
		resource.sulfur = 0;
		resource.crystal = 0;
		resource.gems = 0;
		resource.gold = 0;
		switch(tradeFrom.resource){
    		    case ORE:
			resource.ore = countFrom;
			break;
    		    case WOOD:
			resource.wood = countFrom;
			break;
    		    case MERCURY:
			resource.mercury = countFrom;
			break;
    		    case SULFUR:
			resource.sulfur = countFrom;
			break;
    		    case CRYSTAL:
			resource.crystal = countFrom;
			break;
    		    case GEMS:
			resource.gems = countFrom;
			break;
    		    case GOLDS:
			resource.gold = countFrom;
			break;
		    default:
			break;
		}
		KingdomWasteResource(colorKingdom, &resource);
		resource.ore = 0;
		resource.wood = 0;
		resource.mercury = 0;
		resource.sulfur = 0;
		resource.crystal = 0;
		resource.gems = 0;
		resource.gold = 0;
		switch(tradeTo.resource){
    		    case ORE:
			resource.ore = countTo;
			break;
    		    case WOOD:
			resource.wood = countTo;
			break;
    		    case MERCURY:
			resource.mercury = countTo;
			break;
    		    case SULFUR:
			resource.sulfur = countTo;
			break;
    		    case CRYSTAL:
			resource.crystal = countTo;
			break;
    		    case GEMS:
			resource.gems = countTo;
			break;
    		    case GOLDS:
			resource.gold = countTo;
			break;
		    default:
			break;
		}
		KingdomProfitResource(colorKingdom, &resource);
                exit = TRUE;
                result = NONE;
		break;

            default:
                break;

        }
                                    
    // востанавливаем бакгроунд
    CursorOff();

    FreeBox(&box);

    if(tradeFrom.background) SDL_FreeSurface(tradeFrom.background);
    if(tradeTo.background) SDL_FreeSurface(tradeTo.background);
    if(backgroundAction.surface) SDL_FreeSurface(backgroundAction.surface);
    FreeSpliter(spliterMarket);

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
    Uint8 i;
    Uint16 cx, cy;
    S_PAYMENT resource;
    GetKingdomAllResource(colorKingdom, &resource);
    
    const char *header = "Your Resources";
    const char *trade = NULL;
    char number[8];
    trade = (GetIntValue(EVILINTERFACE) ? "TRADPOSE.ICN" : "TRADPOST.ICN");

    cx = video->w / 2 - 115;
    cy = video->h / 2;
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
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // cost
    sprintf(number, "%d", resource.gold);
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
    Uint8 i;
    Uint16 cx, cy;
    S_PAYMENT resource;
    GetKingdomAllResource(colorKingdom, &resource);
    
    const char *header = "Available Trades";
    const char *trade = NULL;
    char string[8];
    trade = (GetIntValue(EVILINTERFACE) ? "TRADPOSE.ICN" : "TRADPOST.ICN");

    cx = video->w / 2 + 10;
    cy = video->h / 2;
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
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i && tradeFrom.resource != WOOD)
	    TradeCostToString(string, tradeFrom.select, UNCOSTLY);
	else if(1 == i && tradeFrom.resource != MERCURY)
	    TradeCostToString(string, tradeFrom.select, COSTLY);
	else if(2 == i && tradeFrom.resource != ORE)
	    TradeCostToString(string, tradeFrom.select, UNCOSTLY);
	else
	    sprintf(string, "n/a");
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
	SDL_BlitSurface(image, NULL, video, &rectCur);

	// cost
	if(0 == i && tradeFrom.resource != SULFUR)
	    TradeCostToString(string, tradeFrom.select, COSTLY);
	else if(1 == i && tradeFrom.resource != CRYSTAL)
	    TradeCostToString(string, tradeFrom.select, COSTLY);
	else if(2 == i && tradeFrom.resource != GEMS)
	    TradeCostToString(string, tradeFrom.select, COSTLY);
	else
	    sprintf(string, "n/a");
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
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // cost
    if(tradeFrom.resource != SULFUR)
	TradeCostToString(string, tradeFrom.select, GOLD);
    else
	sprintf(string, "n/a");
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
    if(UNFOCUS != tradeTo.select) RedrawActionTrade();
    
    CursorOn();

    return NONE;
}

ACTION  ActionPressFromCostly(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    tradeFrom.select = COSTLY;

    RedrawTradeToResource();
    if(UNFOCUS != tradeTo.select) RedrawActionTrade();

    CursorOn();

    return NONE;
}

ACTION  ActionPressFromGold(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    tradeFrom.select = GOLD;

    RedrawTradeToResource();
    if(UNFOCUS != tradeTo.select) RedrawActionTrade();

    CursorOn();

    return NONE;
}

ACTION  ActionPressToUncostly(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = UNCOSTLY;

    RedrawTradeToResource();
    RedrawActionTrade();

    CursorOn();

    return NONE;
}

ACTION  ActionPressToCostly(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = COSTLY;

    RedrawTradeToResource();
    RedrawActionTrade();

    CursorOn();

    return NONE;
}

ACTION  ActionPressToGold(void){

    CursorOff();

    TradeRestoreToCursor();
    TradeSaveToCursor();

    tradeTo.select = GOLD;

    RedrawTradeToResource();
    RedrawActionTrade();

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

    rect.x = video->w / 2 - 115;
    rect.y = video->h / 2 + 20;
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
    rect.x = video->w / 2 - 115;
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
    rect.x = video->w / 2 - 75;
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
    rect.y = video->h / 2 + 20;
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
	tradeTo.resource = WOOD;
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
	tradeTo.resource = MERCURY;
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
	tradeTo.resource = ORE;
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
	tradeTo.resource = SULFUR;
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
	tradeTo.resource = CRYSTAL;
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
	tradeTo.resource = GEMS;
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
	tradeTo.resource = GOLDS;
	return;
    }
}

void TradeCostToString(char *string, E_FOCUSTRADE from, E_FOCUSTRADE to){

    if(! string) return;

    if(0 == GetTradeFactor(from, to)){
	sprintf(string, "n/a");
	return;
    }
    Uint8 market = GetKingdomCountMarketplace(colorKingdom);

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

    Uint8 market = GetKingdomCountMarketplace(colorKingdom);

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

void RedrawActionTrade(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rectCur = backgroundAction.rect;
    const char *info = "Please inspect our fine wares. If you feel like offering a trade, click on the items you wish to trade with and for.";
    const char *header = GetStringBuilding(KNIGHT, BUILD_MARKETPLACE);
    const char *trade = "TRADPOST.ICN";
    if(GetIntValue(EVILINTERFACE)) trade = "TRADPOSE.ICN";

    char str[64];

    INTERFACEACTION action;
    AGGSPRITE sprite;
    
    if(backgroundAction.surface) SDL_BlitSurface(backgroundAction.surface, NULL , video, &backgroundAction.rect);

    PrintAlignText(video, &rectCur, header, FONT_BIG);
    rectCur.y += 30;

    if(UNFOCUS != tradeFrom.select && UNFOCUS != tradeTo.select) RemoveActionLevelEvent(actionTrade, LEVELEVENT_MARKETTRADE);

    if(! GetTradeFactor(tradeFrom.select, tradeTo.select)){
	PrintAlignText(video, &rectCur, info, FONT_BIG);
	return;
    }
    
    sprintf(str, "I can offer you 1 unit of %s for %d units of %s.", GetStringResource(tradeTo.resource), GetTradeFactor(tradeFrom.select, tradeTo.select), GetStringResource(tradeFrom.resource));
    PrintAlignText(video, &rectCur, str, FONT_BIG);

    // splite
    FillSPRITE(&sprite, trade, 1);
    image = GetICNSprite(&sprite);
    rectCur.y += 93;
    rectCur.x = (video->w - image->w) / 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // LEFT
    FillSPRITE(&sprite, trade, 3);
    image = GetICNSprite(&sprite);
    rectCur.x = video->w / 2 - 109;
    rectCur.y += 1;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, trade, 3);
    FillSPRITE(&action.objectPush, trade, 4);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLEFT;
    action.level = LEVELEVENT_MARKETTRADE;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // RIGHT
    FillSPRITE(&sprite, trade, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = video->w / 2 + 100;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, trade, 5);
    FillSPRITE(&action.objectPush, trade, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressRIGHT;
    action.level = LEVELEVENT_MARKETTRADE;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    
    // TRADE
    FillSPRITE(&sprite, trade, 15);
    image = GetICNSprite(&sprite);
    rectCur.y += 21;
    rectCur.x = (video->w - image->w) / 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, trade, 15);
    FillSPRITE(&action.objectPush, trade, 16);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressYES;
    action.level = LEVELEVENT_MARKETTRADE;
    AddActionEvent(&actionTrade, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // init spliterMarket
    rectCur.x = video->w / 2 - 91;
    rectCur.y = (GetIntValue(EVILINTERFACE) ? video->h / 2 - 35 : video->h / 2 - 25);
    rectCur.w = 188;
    rectCur.h = 10;

    Uint16 max = 0;
    if(GetTradeFactor(tradeFrom.select, tradeTo.select))
	max = (tradeTo.resource != GOLDS ?
	    GetKingdomResource(colorKingdom, tradeFrom.resource) / GetTradeFactor(tradeFrom.select, tradeTo.select) :
	    GetKingdomResource(colorKingdom, tradeFrom.resource) );

    SetRangeSpliter(spliterMarket, &rectCur, max);
    RedrawSpliter(spliterMarket);
    RedrawSpriteResource();
}

void RedrawSpriteResource(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rectCur, rectFrom;
    AGGSPRITE sprite;
    Uint8 index;
    Uint16 countFrom = 0;
    Uint16 countTo = 0;
    const char *text = NULL;
    char str[8];

    CursorOff();

    // background
    rectCur = backgroundAction.rect;
    rectCur.y = video->h / 2 - 80;
    rectCur.h = 40;
    rectFrom = rectCur;
    rectFrom.x = 0;
    rectFrom.y = 0;
    
    if(backgroundAction.surface) SDL_BlitSurface(backgroundAction.surface, &rectFrom, video, &rectCur);

    switch(tradeFrom.resource){

        case ORE:
	    index = 2;
	    break;

        case WOOD:
	    index = 0;
	    break;

        case MERCURY:
	    index = 1;
	    break;

        case SULFUR:
	    index = 3;
	    break;

        case CRYSTAL:
	    index = 4;
	    break;

        case GEMS:
	    index = 5;
	    break;

        case GOLDS:
	    index = 6;
	    break;
	
	default:
	    return;
	    break;
    }

    // sprite From resource
    FillSPRITE(&sprite, "RESOURCE.ICN", index);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2 - 75;
    rectCur.y = video->h / 2 - 55 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    switch(tradeTo.resource){

        case ORE:
	    index = 2;
	    break;

        case WOOD:
	    index = 0;
	    break;

        case MERCURY:
	    index = 1;
	    break;

        case SULFUR:
	    index = 3;
	    break;

        case CRYSTAL:
	    index = 4;
	    break;

        case GEMS:
	    index = 5;
	    break;

        case GOLDS:
	    index = 6;
	    break;
	
	default:
	    return;
	    break;
    }

    // sprite To resource
    FillSPRITE(&sprite, "RESOURCE.ICN", index);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2 + 75;
    rectCur.y = video->h / 2 - 55 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // sprite arrow
    FillSPRITE(&sprite, "TRADPOST.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = video->h / 2 - 60 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // text
    text = "Qty to trade";
    rectCur.x = (video->w - GetLengthText(text, FONT_SMALL)) / 2;
    rectCur.y = video->h / 2 - 53;
    rectCur.w = GetLengthText(text, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, text, FONT_SMALL);
    
    if(tradeTo.resource != GOLDS){
	countFrom = GetCurrentSpliter(spliterMarket) * GetTradeFactor(tradeFrom.select, tradeTo.select);
	countTo = GetCurrentSpliter(spliterMarket);
    }else{
	countTo = GetCurrentSpliter(spliterMarket) * GetTradeFactor(tradeFrom.select, tradeTo.select);
	countFrom = GetCurrentSpliter(spliterMarket);
    }

    sprintf(str, "%d", countFrom);
    rectCur.x = (video->w - GetLengthText(str, FONT_SMALL)) / 2 - 75;
    rectCur.y = video->h / 2 - 53;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);

    sprintf(str, "%d", countTo);
    rectCur.x = (video->w - GetLengthText(str, FONT_SMALL)) / 2 + 75;
    rectCur.y = video->h / 2 - 53;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    
    CursorOn();
}
