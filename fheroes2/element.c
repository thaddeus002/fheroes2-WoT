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
#include "config.h"
#include "element.h"

#define BOXWIDTH 306
#define BOXHEIGHT 226

ACTION DialogPressYES(void);
ACTION DialogPressNO(void);

ACTION MessageBox(const char *message, ENUMFONT font){

    CursorOff();
    SetIntValue(ANIMATION, FALSE);
    
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
    format = SDL_CreateRGBSurface(SDL_SWSURFACE, BOXWIDTH, BOXHEIGHT, 16, 0, 0, 0, 0);
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
    SDL_Flip(video);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);

    SetCursor(cursor);

    SetIntValue(ANIMATION, TRUE);
    CursorOn();

    return result;
}

ACTION DialogPressYES(){

    return YES;
}

ACTION DialogPressNO(){

    return NO;
}
