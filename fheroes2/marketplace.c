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
#include "actionevent.h"
#include "castle.h"
#include "castledialog.h"

typedef enum {UNFOCUS, COSTLY, UNCOSTLY, GOLD} E_FOCUSTRADE;

struct {
    E_FOCUSTRADE        select;
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

const char *GetStringTradeCost(E_FOCUSTRADE, E_FOCUSTRADE);

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
    GetKingdomResource(colorKingdom, &resource);
    
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
	else if(2 == i)
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
	else if(2 == i)
	    sprintf(number, "%d", resource.mercury);
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
    GetKingdomResource(colorKingdom, &resource);
    
    const char *header = "Available Trades";
    const char *trade = "TRADPOST.ICN";
    const char *string = NULL;
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
	if(0 == i || 2 == i)
	    string = GetStringTradeCost(tradeFrom.select, UNCOSTLY);
	else if(1 == i)
	    string = GetStringTradeCost(tradeFrom.select, COSTLY);
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
	string = GetStringTradeCost(tradeFrom.select, COSTLY);
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
    string = GetStringTradeCost(tradeFrom.select, GOLD);
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

    RedrawTradeToResource();

    tradeFrom.select = UNCOSTLY;

    CursorOn();

    return NONE;
}

ACTION  ActionPressFromCostly(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    RedrawTradeToResource();

    tradeFrom.select = COSTLY;

    CursorOn();

    return NONE;
}

ACTION  ActionPressFromGold(void){

    CursorOff();

    TradeRestoreFromCursor();
    TradeSaveFromCursor();

    RedrawTradeToResource();

    tradeFrom.select = GOLD;

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
	return;
    }
}

void TradeSaveToCursor(void){

    SDL_Surface *video =SDL_GetVideoSurface();
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

const char *GetStringTradeCost(E_FOCUSTRADE from, E_FOCUSTRADE to){

    const char *string = "n/a";

    switch(from){
    
	case UNFOCUS:
	    break;
	    
	case COSTLY:
	    if(GOLD == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "50";
		if(2 == GetCountMarketplace(colorKingdom)) string = "74";
		if(3 == GetCountMarketplace(colorKingdom)) string = "100";
		if(4 == GetCountMarketplace(colorKingdom)) string = "124";
		if(5 == GetCountMarketplace(colorKingdom)) string = "149";
		if(6 == GetCountMarketplace(colorKingdom)) string = "175";
		if(7 == GetCountMarketplace(colorKingdom)) string = "200";
		if(8 == GetCountMarketplace(colorKingdom)) string = "224";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "249";
	    }
	    if(COSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "10";
		if(2 == GetCountMarketplace(colorKingdom)) string = "7";
		if(3 == GetCountMarketplace(colorKingdom)) string = "5";
		if(4 == GetCountMarketplace(colorKingdom)) string = "4";
		if(5 == GetCountMarketplace(colorKingdom)) string = "4";
		if(6 == GetCountMarketplace(colorKingdom)) string = "3";
		if(7 == GetCountMarketplace(colorKingdom)) string = "3";
		if(8 == GetCountMarketplace(colorKingdom)) string = "3";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "2";
	    }
	    if(UNCOSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "1/5";
		if(2 == GetCountMarketplace(colorKingdom)) string = "1/4";
		if(3 == GetCountMarketplace(colorKingdom)) string = "1/3";
		if(4 == GetCountMarketplace(colorKingdom)) string = "1/2";
		if(5 == GetCountMarketplace(colorKingdom)) string = "1/2";
		if(6 == GetCountMarketplace(colorKingdom)) string = "1/2";
		if(7 == GetCountMarketplace(colorKingdom)) string = "1/2";
		if(8 == GetCountMarketplace(colorKingdom)) string = "1/2";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "1/1";
	    }
	    break;

	case UNCOSTLY:
	    if(GOLD == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "25";
		if(2 == GetCountMarketplace(colorKingdom)) string = "37";
		if(3 == GetCountMarketplace(colorKingdom)) string = "50";
		if(4 == GetCountMarketplace(colorKingdom)) string = "62";
		if(5 == GetCountMarketplace(colorKingdom)) string = "74";
		if(6 == GetCountMarketplace(colorKingdom)) string = "87";
		if(7 == GetCountMarketplace(colorKingdom)) string = "100";
		if(8 == GetCountMarketplace(colorKingdom)) string = "112";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "124";
	    }
	    if(UNCOSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "10";
		if(2 == GetCountMarketplace(colorKingdom)) string = "7";
		if(3 == GetCountMarketplace(colorKingdom)) string = "5";
		if(4 == GetCountMarketplace(colorKingdom)) string = "4";
		if(5 == GetCountMarketplace(colorKingdom)) string = "4";
		if(6 == GetCountMarketplace(colorKingdom)) string = "3";
		if(7 == GetCountMarketplace(colorKingdom)) string = "3";
		if(8 == GetCountMarketplace(colorKingdom)) string = "3";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "2";
	    }
	    if(COSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "1/20";
		if(2 == GetCountMarketplace(colorKingdom)) string = "1/14";
		if(3 == GetCountMarketplace(colorKingdom)) string = "1/10";
		if(4 == GetCountMarketplace(colorKingdom)) string = "1/8";
		if(5 == GetCountMarketplace(colorKingdom)) string = "1/7";
		if(6 == GetCountMarketplace(colorKingdom)) string = "1/6";
		if(7 == GetCountMarketplace(colorKingdom)) string = "1/5";
		if(8 == GetCountMarketplace(colorKingdom)) string = "1/5";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "1/4";
	    }
	    break;

	case GOLD:
	    if(UNCOSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "2500";
		if(2 == GetCountMarketplace(colorKingdom)) string = "1667";
		if(3 == GetCountMarketplace(colorKingdom)) string = "1250";
		if(4 == GetCountMarketplace(colorKingdom)) string = "1000";
		if(5 == GetCountMarketplace(colorKingdom)) string = "834";
		if(6 == GetCountMarketplace(colorKingdom)) string = "715";
		if(7 == GetCountMarketplace(colorKingdom)) string = "625";
		if(8 == GetCountMarketplace(colorKingdom)) string = "556";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "500";
	    }
	    if(COSTLY == to){
		if(1 == GetCountMarketplace(colorKingdom)) string = "5000";
		if(2 == GetCountMarketplace(colorKingdom)) string = "3334";
		if(3 == GetCountMarketplace(colorKingdom)) string = "2500";
		if(4 == GetCountMarketplace(colorKingdom)) string = "2000";
		if(5 == GetCountMarketplace(colorKingdom)) string = "1667";
		if(6 == GetCountMarketplace(colorKingdom)) string = "1429";
		if(7 == GetCountMarketplace(colorKingdom)) string = "1250";
		if(8 == GetCountMarketplace(colorKingdom)) string = "1112";
		if(8 <  GetCountMarketplace(colorKingdom)) string = "1000";
	    }
	    break;
    }
    
    return string;
}
