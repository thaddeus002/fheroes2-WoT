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

#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "cursor.h"
#include "mp2maps.h"
#include "loadgame.h"
#include "object.h"
#include "config.h"
#include "debug.h"
#include "element.h"

#define BOXWIDTH 306
#define BOXHEIGHT 226

ACTION MessageBox(const char *message, ENUMFONT font){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    
    Uint32 cursor = GetCursor();
    
    char *buybuild = "BUYBUILD.ICN";
    char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    // отрисовка диалога по центру экрана
    SDL_Surface *format, *back, *elem, *video;
    SDL_Rect rectBack, rectCur;
    BOOL exit;
    ACTION result = NONE;
    AGGSPRITE sprite;
            
    video = SDL_GetVideoSurface();
    rectBack.x = (video->w - BOXWIDTH) / 2;
    rectBack.y = (video->h - BOXHEIGHT) / 2;
    rectBack.w = BOXWIDTH;
    rectBack.h = BOXHEIGHT;

    // сохраняем бакгроунд
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, BOXWIDTH, BOXHEIGHT, 16, 0, 0, 0, 0))){
	fprintf(stderr, "MessageBox: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }    

    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // рисуем текст
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + 70;
    rectCur.w = BOXWIDTH - 70;
    rectCur.h = BOXHEIGHT - 150;
    PrintText(video, &rectCur, message, font);

    // рисуем кнопки
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        

    // кнопка YES
    FillSPRITE(&sprite, system, 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + BOXHEIGHT - 70;
    if(GetIntValue(EVILINTERFACE))
        rectCur.y = rectBack.y + BOXHEIGHT - 85;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 5);
    FillSPRITE(&action.objectPush, system, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressYES;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // кнопка NO
    FillSPRITE(&sprite, system, 7);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + BOXWIDTH - elem->w - 25;
    if(GetIntValue(EVILINTERFACE))
	rectCur.y = rectBack.y + BOXHEIGHT - 85;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 7);
    FillSPRITE(&action.objectPush, system, 8);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNO;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    exit = FALSE;
    while(! exit)
        switch(ActionCycle(dialog)){

            case ENTER:
            case YES:
                exit = TRUE;
                result = YES;
            break;

            case NO:
                exit = TRUE;
                result = NO;
            break;

            case EXIT:
                exit = TRUE;
                result = EXIT;
            break;

            case ESC:
            case CANCEL:
                exit = TRUE;
                result = CANCEL;
            break;
	    
	    default:
		break;
	}

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    //SDL_Flip(video);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    CursorOn();

    return result;
}

Uint16 MessageSelectCountBox(Uint16 max, const char *message, ENUMFONT font){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    
    char *buybuild = "BUYBUILD.ICN";
    char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    // отрисовка диалога по центру экрана
    SDL_Surface *format, *back, *elem, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 result;
    BOOL exit = FALSE;
    AGGSPRITE sprite;
            
    video = SDL_GetVideoSurface();
    rectBack.x = (video->w - BOXWIDTH) / 2;
    rectBack.y = (video->h - BOXHEIGHT) / 2;
    rectBack.w = BOXWIDTH;
    rectBack.h = BOXHEIGHT;

    // сохраняем бакгроунд
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, BOXWIDTH, BOXHEIGHT, 16, 0, 0, 0, 0))){
	fprintf(stderr, "MessageBox: CreateRGBSurface failed: %s\n", SDL_GetError());
	return EXIT;
    }    

    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, buybuild, 6);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, buybuild, 1);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, buybuild, 2);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // рисуем текст
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + 70;
    rectCur.w = BOXWIDTH - 70;
    rectCur.h = BOXHEIGHT - 150;
    PrintText(video, &rectCur, message, font);

    // рисуем кнопки
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        

    // кнопка YES
    FillSPRITE(&sprite, system, 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + BOXHEIGHT - 70;
    if(GetIntValue(EVILINTERFACE))
        rectCur.y = rectBack.y + BOXHEIGHT - 85;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 5);
    FillSPRITE(&action.objectPush, system, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressYES;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // кнопка NO
    FillSPRITE(&sprite, system, 7);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + BOXWIDTH - elem->w - 25;
    if(GetIntValue(EVILINTERFACE))
	rectCur.y = rectBack.y + BOXHEIGHT - 85;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 7);
    FillSPRITE(&action.objectPush, system, 8);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNO;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // UP
    FillSPRITE(&sprite, "TOWNWIND.ICN", 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 50;
    rectCur.y = rectBack.y + BOXHEIGHT - 50;
    if(GetIntValue(EVILINTERFACE))
        rectCur.y = rectBack.y + BOXHEIGHT - 65;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "TOWNWIND.ICN", 5);
    FillSPRITE(&action.objectPush, "TOWNWIND.ICN", 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressUP;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // DOWN
    FillSPRITE(&sprite, "TOWNWIND.ICN", 7);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 50;
    rectCur.y = rectBack.y + BOXHEIGHT - 60;
    if(GetIntValue(EVILINTERFACE))
        rectCur.y = rectBack.y + BOXHEIGHT - 75;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "TOWNWIND.ICN", 7);
    FillSPRITE(&action.objectPush, "TOWNWIND.ICN", 8);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressDOWN;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    result = 0;
    while(! exit)

        switch(ActionCycle(dialog)){

            case EXIT:
                exit = TRUE;
                result = 0xFFFF;
                break;

            case ESC:
            case YES:
            case NO:
                exit = TRUE;
                break;

	    case UP:
		if(result < max) ++result;
		break;

	    case DOWN:
		if(result) --result;
		break;

            default:
                break;
        }

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    //SDL_Flip(video);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetCursor(cursor);

    SetIntValue(ANIM2, TRUE);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

void ShowBorder(SDL_Rect *rect){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect src, dst;
    AGGSPRITE sprite;
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "SURDRBKE.ICN", 0) : FillSPRITE(&sprite, "SURDRBKG.ICN", 0);
    SDL_Surface *image = GetICNSprite(&sprite);

    // top left angle
    src.x = 0;
    src.y = 0;
    src.w = SHADOWWIDTH + BORDERWIDTH * 3;
    src.h = BORDERWIDTH;
    dst.x = rect->x;
    dst.y = rect->y;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // top bar
    src.x = SHADOWWIDTH + BORDERWIDTH * 3;
    src.y = 0;
    src.w = image->w - SHADOWWIDTH - BORDERWIDTH * 6;
    src.h = BORDERWIDTH;
    dst.x = rect->x + SHADOWWIDTH + BORDERWIDTH * 3;
    dst.y = rect->y;
    dst.w = src.w;
    dst.h = src.h;
    while(dst.x + dst.w < rect->x + rect->w){
	SDL_BlitSurface(image, &src, video, &dst);
	dst.x += dst.w;
    }
    src.w = rect->x + rect->w - dst.x;
    dst.w = src.w;
    SDL_BlitSurface(image, &src, video, &dst);
    // top right angle
    src.x = image->w - BORDERWIDTH * 3;
    src.y = 0;
    src.w = BORDERWIDTH * 3;
    src.h = BORDERWIDTH;
    dst.x = rect->x + rect->w - src.w;
    dst.y = rect->y;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);

    // bottom left angle
    src.x = 0;
    src.y = image->h - SHADOWWIDTH - BORDERWIDTH;
    src.w = SHADOWWIDTH + BORDERWIDTH * 3;
    src.h = BORDERWIDTH + SHADOWWIDTH;
    dst.x = rect->x;
    dst.y = rect->y + rect->h - BORDERWIDTH - SHADOWWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // bottom bar
    src.x = SHADOWWIDTH + BORDERWIDTH * 3;
    src.y = image->h - SHADOWWIDTH - BORDERWIDTH;
    src.w = image->w - SHADOWWIDTH - BORDERWIDTH * 6;
    src.h = BORDERWIDTH + SHADOWWIDTH;
    dst.x = rect->x + SHADOWWIDTH + BORDERWIDTH * 3;
    dst.y = rect->y + rect->h - BORDERWIDTH - SHADOWWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    while(dst.x + dst.w < rect->x + rect->w){
	SDL_BlitSurface(image, &src, video, &dst);
	dst.x += dst.w;
    }
    src.w = rect->x + rect->w - dst.x - BORDERWIDTH * 3;
    dst.w = src.w;
    SDL_BlitSurface(image, &src, video, &dst);
    // bottom right angle
    src.x = image->w - BORDERWIDTH * 3;
    src.y = image->h - SHADOWWIDTH - BORDERWIDTH;
    src.w = BORDERWIDTH * 3;
    src.h = BORDERWIDTH + SHADOWWIDTH;
    dst.x = rect->x + rect->w - src.w;
    dst.y = rect->y + rect->h - BORDERWIDTH - SHADOWWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);

    // left top angle
    src.x = 0;
    src.y = BORDERWIDTH;
    src.w = SHADOWWIDTH + BORDERWIDTH;
    src.h = BORDERWIDTH * 2;
    dst.x = rect->x;
    dst.y = rect->y + BORDERWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // left bar
    src.x = 0;
    src.y = BORDERWIDTH * 3;
    src.w = SHADOWWIDTH + BORDERWIDTH;
    src.h = image->h - BORDERWIDTH * 6 - SHADOWWIDTH;
    dst.x = rect->x;
    dst.y = rect->y + BORDERWIDTH * 3;
    dst.w = src.w;
    dst.h = src.h;
    while(dst.y + dst.h < rect->y + rect->h - SHADOWWIDTH - BORDERWIDTH * 3){
	SDL_BlitSurface(image, &src, video, &dst);
	dst.y += dst.h;
    }
    src.h = rect->y + rect->h - dst.y - SHADOWWIDTH - BORDERWIDTH * 3;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // left bottom angle
    src.x = 0;
    src.y = image->h - SHADOWWIDTH - BORDERWIDTH * 3;
    src.w = SHADOWWIDTH + BORDERWIDTH;
    src.h = BORDERWIDTH * 2;
    dst.x = rect->x;
    dst.y = rect->y + rect->h - BORDERWIDTH * 3 - SHADOWWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);

    // right top angle
    src.x = image->w - BORDERWIDTH;
    src.y = BORDERWIDTH;
    src.w = BORDERWIDTH;
    src.h = BORDERWIDTH * 2;
    dst.x = rect->x + rect->w - BORDERWIDTH;
    dst.y = rect->y + BORDERWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // right bar
    src.x = image->w - BORDERWIDTH;
    src.y = BORDERWIDTH * 3;
    src.w = BORDERWIDTH;
    src.h = image->h - BORDERWIDTH * 6 - SHADOWWIDTH;
    dst.x = rect->x + rect->w - BORDERWIDTH;
    dst.y = rect->y + BORDERWIDTH * 3;
    dst.w = src.w;
    dst.h = src.h;
    while(dst.y + dst.h < rect->y + rect->h - SHADOWWIDTH - BORDERWIDTH * 3){
	SDL_BlitSurface(image, &src, video, &dst);
	dst.y += dst.h;
    }
    src.h = rect->y + rect->h - dst.y - SHADOWWIDTH - BORDERWIDTH * 3;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
    // right bottom angle
    src.x = image->w - BORDERWIDTH;
    src.y = image->h - SHADOWWIDTH - BORDERWIDTH * 3;
    src.w = BORDERWIDTH;
    src.h = BORDERWIDTH * 2;
    dst.x = rect->x + rect->w - BORDERWIDTH;
    dst.y = rect->y + rect->h - BORDERWIDTH * 3 - SHADOWWIDTH;
    dst.w = src.w;
    dst.h = src.h;
    SDL_BlitSurface(image, &src, video, &dst);
}

void ShowQuickInfo(Uint16 index){

    SDL_Event event;
    BOOL exit = FALSE;
    
    CursorOff();
    SetIntValue(ANIM2, FALSE);
    
    Uint32 cursor = GetCursor();
    Uint8 i;
    Uint8 count = 0;
    Uint8 current;

    SDL_Surface *format, *back, *ram, *video, *image;
    Sint32 cx, cy;
    SDL_Rect rectBack, rectCur, rect;
    AGGSPRITE sprite;
    const char *message;
    char string[64];

    const S_CELLMAPS *cell = NULL;
    const S_CASTLE *castle = NULL;
    
    SDL_GetMouseState(&cx, &cy);

    // выборка по герою замку или объекту
    cell = GetCELLMAPS(index);
    switch(cell->type){
    
	case OBJN_CASTLE:
	case OBJ_CASTLE:
	    FillSPRITE(&sprite, "QWIKTOWN.ICN", 0);
	    break;
	    
	case OBJ_HEROES:
	    FillSPRITE(&sprite, "QWIKHERO.ICN", 0);
	    break;
	
	default:
	    FillSPRITE(&sprite, "QWIKINFO.ICN", 0);
	    break;
    }

    ram = GetICNSprite(&sprite);

    // отрисовка диалога по центру курсора
    video = SDL_GetVideoSurface();

    rectBack.x = cx;
    rectBack.y = cy;
    rectBack.w = ram->w;
    rectBack.h = ram->h;
    if(cx + ram->w > BORDERWIDTH + GetWidthArea() * TILEWIDTH) rectBack.x = BORDERWIDTH + GetWidthArea() * TILEWIDTH - ram->w;
    if(cy + ram->h + BORDERWIDTH > video->h) rectBack.y = video->h - BORDERWIDTH - ram->h;
    
    // сохраняем бакгроунд
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "ShowQuickInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
	return;
    }    
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // рисуем рамку
    SDL_BlitSurface(ram, NULL, video, &rectBack);

    // здесь левый верхний угол после бордюра
    rectCur.x = rectBack.x + 25;
    rectCur.y = rectBack.y + 10;
    rectCur.w = rectBack.w - 40;
    rectCur.h = rectBack.h - 40;

    // выборка надписи по герою замку или объекту
    switch(cell->type){

	case OBJN_CASTLE:
	case OBJ_CASTLE:
	    castle = GetStatCastlePos(cell->ax, cell->ay);
	    if(castle == NULL){ printf("NULL\n"); break;}
	    message = castle->name;
	    rect.x = rectCur.x + (rectCur.w - GetLengthText(message, FONT_SMALL)) / 2 ;
	    rect.y = rectCur.y + 5;
	    rect.w = rectBack.w - 40;
            rect.h = FONT_HEIGHTSMALL;
            PrintText(video, &rect, message, FONT_SMALL);

    	    castle->building & BUILD_CASTLE ? FillSPRITE(&sprite, "LOCATORS.ICN", 9 + castle->race) : FillSPRITE(&sprite, "LOCATORS.ICN", 15 + castle->race);
            image = GetICNSprite(&sprite);
	    rect.x = rectCur.x + (rectCur.w - image->w) / 2 ;
	    rect.y = rect.y + rect.h + 5;
	    rect.w = image->w;
	    rect.h = image->h;
            SDL_BlitSurface(image, NULL, video, &rect);
                                
	    message = "Defenders:";
	    rect.x = rectCur.x + (rectCur.w - GetLengthText(message, FONT_SMALL)) / 2 ;
	    rect.y = rect.y + image->h + 5;
            rectCur.h = FONT_HEIGHTSMALL;
	    rect.w = GetLengthText(message, FONT_SMALL);
            PrintText(video, &rect, message, FONT_SMALL);
            
            for(i = 0; i < CASTLEMAXARMY; ++i)
        	if(MONSTERNONE != castle->army[i].monster && castle->army[i].count) ++count;
	    
	    if(! count){
		message = "None";
		rect.x = rectCur.x + (rectCur.w - GetLengthText(message, FONT_SMALL)) / 2 ;
		rect.y = rect.y + 45;
        	rectCur.h = FONT_HEIGHTSMALL;
		rect.w = GetLengthText(message, FONT_SMALL);
        	PrintText(video, &rect, message, FONT_SMALL);
    	    
    	    }else{
    	    // рисуем в одну строку
    		current = 0;
    		for(i = 0; i < CASTLEMAXARMY; ++i)
        	    if(MONSTERNONE != castle->army[i].monster && castle->army[i].count){
    			FillSPRITE(&sprite, "MONS32.ICN", castle->army[i].monster);
        		image = GetICNSprite(&sprite);
        		// выравнивание по количеству монстров
			rect.x = (rectCur.w / CASTLEMAXARMY - image->w) / 2 + rectCur.x + current * rectCur.w / count + ((rectCur.w / CASTLEMAXARMY) * (CASTLEMAXARMY - count) / (2 * count));
			rect.w = image->w;
			rect.h = image->h;
			rect.y = rectCur.y + 85;
			// выравнивание по высоте монстра
			if(image->h > 32) rect.y -= image->h - 32; else rect.y += 32 - image->h;
        		SDL_BlitSurface(image, NULL, video, &rect);
        		// текст количества
        		sprintf(string, "%d", castle->army[i].count);
        		rect.x += image->w / 2;
			rect.x = rect.x - GetLengthText(string, FONT_SMALL) / 2;
        		rect.y = rectCur.y + 118;
			rect.w = GetLengthText(string, FONT_SMALL);
        		rect.h = FONT_HEIGHTSMALL;
        		PrintText(video, &rect, string, FONT_SMALL);
        		
        		current++;
    		    }
    	    }
	    break;
	    
	case OBJ_HEROES:
	    message = "Heroes Info";
	    rectCur.x = rectCur.x + (rectCur.w - GetLengthText(message, FONT_SMALL)) / 2 ;
	    rectCur.y += 20;

	    PrintText(video, &rectCur, message, FONT_SMALL);
	    break;
	
	case OBJ_MONSTER:
    	    sprintf(string, "%s of %s", GetStringSizeArmy(GetSizeArmy(cell->count)), GetStringMonster((E_MONSTER) cell->object.monster));
	    rectCur.x = rectCur.x + (rectCur.w - GetLengthText(string, FONT_SMALL)) / 2 ;
	    rectCur.y += 20;
	    rectCur.w = GetLengthText(string, FONT_SMALL);
    	    rectCur.h = FONT_HEIGHTSMALL;
	    PrintText(video, &rectCur, string, FONT_SMALL);
	    break;

	default:
	    message = GetStringObject(cell->type);
	    rectCur.x = rectCur.x + (rectCur.w - GetLengthText(message, FONT_SMALL)) / 2 ;
	    rectCur.y += 20;

	    PrintText(video, &rectCur, message, FONT_SMALL);
	    break;
    }

    SDL_Flip(video);

    while(! exit){

	while(SDL_PollEvent(&event))
	    if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;

	if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
    }

    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);

    SDL_FreeSurface(back);

    SetCursor(cursor);
    SetIntValue(ANIM2, TRUE);
    CursorOn();
}
