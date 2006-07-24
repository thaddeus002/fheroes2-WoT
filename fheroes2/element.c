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

ACTION DialogPressYES(void);
ACTION DialogPressNO(void);

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
    action.pf = DialogPressYES;
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
    action.pf = DialogPressNO;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    result = ActionCycle(dialog);

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

ACTION DialogPressYES(){

    return YES;
}

ACTION DialogPressNO(){

    return NO;
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

    SDL_Surface *format, *back, *ram, *video;
    Sint32 cx, cy;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    const char *message;

    SDL_GetMouseState(&cx, &cy);

    // выборка по герою замку или объекту
    switch((GetCELLMAPS(index))->type){
    
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

    // выборка надписи по герою замку или объекту
    E_OBJECT obj = (GetCELLMAPS(index))->type;
    switch(obj){

	case OBJN_CASTLE:
	case OBJ_CASTLE:
	    message = "Castle Info";
	    break;
	    
	case OBJ_HEROES:
	    message = "Heroes Info";
	    break;
	
	default:
	    message = PrintObjectType(obj, 0);
	    break;
    }
    // здесь левый верхний угол после бордюра
    rectCur.x = rectBack.x + 24;
    rectCur.y = rectBack.y + 10;
    rectCur.w = rectBack.w - 40;
    rectCur.h = rectBack.h - 40;

    rectCur.x = rectCur.x + rectCur.w / 2 - strlen(message) * FONT_WIDTHSMALL / 2 ;

    PrintText(video, &rectCur, message, FONT_SMALL);
    SDL_Flip(video);

    while(! exit){

	while(SDL_PollEvent(&event))
	    if(SDL_RELEASED == event.button.state) exit = TRUE;
    
	if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
    }

    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);

    SDL_FreeSurface(back);

    SetCursor(cursor);
    SetIntValue(ANIM2, TRUE);
    CursorOn();
}
