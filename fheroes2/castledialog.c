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
#include "castle.h"
#include "castledialog.h"

ACTION BuildingInfoYESBox(E_BUILDINGCASTLE build){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    Uint32 cursor = GetCursor();
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    Uint8 i;
    
    const S_CASTLE *castle = GetCurrentCastle();

    const char *info = NULL;
    const char *header = NULL;
    const char *buybuild = "BUYBUILD.ICN";
    const char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    switch(build){
	case BUILD_STATUE:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_STATUE);
	    header = GetStringBuilding(castle->race, BUILD_STATUE);
	    break;
	case BUILD_CAPTAIN:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_CAPTAIN);
	    header = GetStringBuilding(castle->race, BUILD_CAPTAIN);
	    break;
	case BUILD_MOAT:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_MOAT);
	    header = GetStringBuilding(castle->race, BUILD_MOAT);
	    break;
	case BUILD_WEL2:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_WEL2);
	    header = GetStringBuilding(castle->race, BUILD_WEL2);
	    break;
	case BUILD_SPEC:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_SPEC);
	    header = GetStringBuilding(castle->race, BUILD_SPEC);
	    break;
	default:
	    CursorOn();
	    return NONE;
	break;
    }
    
    Uint16 height = 0;
    Uint16 width = 0;
    Uint8 count = 0;
    Uint16 max = 0;
    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    width += image->w;
    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    width += image->w;
    // вычисляем динамически высоту диалога
    max += 30;	// начало текста инфо
    max += FONT_HEIGHTBIG * (GetLengthText(info, FONT_BIG) / (width - 70)) + 1;
    max += 10;	// отступ

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    // количество средних блоков
    count = (max - height) / image->h;
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
	fprintf(stderr, "MessageBox: CreateRGBSurface failed: %s\n", SDL_GetError());
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
    if(GetIntValue(EVILINTERFACE)) rectCur.y = rectBack.y + 70; else rectCur.y = rectBack.y + 85;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 90;
    PrintAlignText(video, &rectCur, info, FONT_BIG);

    // кнопка YES
    FillSPRITE(&sprite, system, 5);
    image = GetICNSprite(&sprite);
    rectCur.x = (video->w - image->w) / 2;
    rectCur.y = rectBack.y + rectBack.h - 70;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, system, 5);
    FillSPRITE(&action.objectPush, system, 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressYES;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    ActionCycle(dialog, NULL);
                                    
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

    return NONE;
}

ACTION BuildingInfoBox(E_BUILDINGCASTLE build){

    CursorOff();

    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);

    SDL_Event event;
    BOOL exit = FALSE;
    
    Uint32 cursor = GetCursor();
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    INTERFACEACTION *dialog = NULL;        
    AGGSPRITE sprite;
    Uint8 i;
    
    const S_CASTLE *castle = GetCurrentCastle();

    const char *info = NULL;
    const char *header = NULL;
    const char *buybuild = "BUYBUILD.ICN";
    const char *system = "SYSTEM.ICN";
    if(GetIntValue(EVILINTERFACE)){
	buybuild = "BUYBUILE.ICN";
	system = "SYSTEME.ICN";
    }
    
    switch(build){
	case BUILD_TAVERN:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_TAVERN);
	    header = GetStringBuilding(castle->race, BUILD_TAVERN);
	    break;
	case BUILD_MAGEGUILD1:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_MAGEGUILD1);
	    header = GetStringBuilding(castle->race, BUILD_MAGEGUILD1);
	    break;
	case BUILD_THIEVESGUILD:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_THIEVESGUILD);
	    header = GetStringBuilding(castle->race, BUILD_THIEVESGUILD);
	    break;
	case BUILD_STATUE:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_STATUE);
	    header = GetStringBuilding(castle->race, BUILD_STATUE);
	    break;
	case BUILD_CAPTAIN:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_CAPTAIN);
	    header = GetStringBuilding(castle->race, BUILD_CAPTAIN);
	    break;
	case BUILD_CASTLE:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_CASTLE);
	    header = GetStringBuilding(castle->race, BUILD_CASTLE);
	    break;
	case BUILD_TENT:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_TENT);
	    header = GetStringBuilding(castle->race, BUILD_TENT);
	    break;
	case BUILD_SHIPYARD:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_SHIPYARD);
	    header = GetStringBuilding(castle->race, BUILD_SHIPYARD);
	    break;
	case BUILD_MARKETPLACE:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_MARKETPLACE);
	    header = GetStringBuilding(castle->race, BUILD_MARKETPLACE);
	    break;
	case BUILD_WELL:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_WELL);
	    header = GetStringBuilding(castle->race, BUILD_WELL);
	    break;
	case BUILD_MOAT:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_MOAT);
	    header = GetStringBuilding(castle->race, BUILD_MOAT);
	    break;
	case BUILD_WEL2:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_WEL2);
	    header = GetStringBuilding(castle->race, BUILD_WEL2);
	    break;
	case BUILD_SPEC:
	    info = GetStringDescriptionsBuilding(castle->race, BUILD_SPEC);
	    header = GetStringBuilding(castle->race, BUILD_SPEC);
	    break;
	default:
	    CursorOn();
	    return NONE;
	break;
    }
    
    Uint16 height = 0;
    Uint16 width = 0;
    Uint8 count = 0;
    Uint16 max = 0;
    // получаем левый верхний спрайт
    FillSPRITE(&sprite, buybuild, 4);
    image = GetICNSprite(&sprite);
    width += image->w;
    // получаем правый верхний спрайт
    FillSPRITE(&sprite, buybuild, 0);
    width += image->w;
    // вычисляем динамически высоту диалога
    max += 30;	// начало текста инфо
    max += FONT_HEIGHTBIG * (GetLengthText(info, FONT_BIG) / (width - 70)) + 1;

    // получаем левый средний спрайт
    FillSPRITE(&sprite, buybuild, 5);
    image = GetICNSprite(&sprite);
    // количество средних блоков
    count = (max - height) / image->h;
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
	fprintf(stderr, "MessageBox: CreateRGBSurface failed: %s\n", SDL_GetError());
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
    rectCur.y = rectBack.y + 70;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 70;
    PrintAlignText(video, &rectCur, header, FONT_BIG);

    // text info
    rectCur.x = rectBack.x + 35;
    rectCur.y = rectBack.y + 100;
    rectCur.w = rectBack.w - 70;
    rectCur.h = rectBack.h - 90;
    PrintAlignText(video, &rectCur, info, FONT_BIG);

    // Отрисовка диалога
    SDL_Flip(video);

    SetCursor(CURSOR_POINTER);

    CursorOn();

    // цикл событий
    while(! exit){
    
        while(SDL_PollEvent(&event))
            if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;
                        
        if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
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

    return NONE;
}
