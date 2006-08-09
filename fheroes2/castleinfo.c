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
#include "element.h"
#include "actionevent.h"
#include "castle.h"
#include "monster.h"
#include "draw.h"
#include "castleaction.h"
#include "castleinfo.h"

void RedrawInfoBottomBar(void);

ACTION ActionCASTLEINFOLOOP(INTERFACEACTION *);

ACTION ShowCastleInfo(void){

    CursorOff();
    SetIntValue(ANIM3, FALSE);

    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy;
    AGGSPRITE sprite;
    const char *icnname;
    const char *message;

    INTERFACEACTION action;
    INTERFACEACTION *castinfo = NULL;        

    const S_CASTLE *castle = GetCurrentCastle();

    FillSPRITE(&sprite, "CASLWIND.ICN", 0);
    image = GetICNSprite(&sprite);

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
        rectBack.x = video->w / 2 - 320 - BORDERWIDTH - SHADOWWIDTH;
        rectBack.y = video->h / 2 - 240 - BORDERWIDTH;
	rectBack.w = 640 + 2 * BORDERWIDTH + SHADOWWIDTH;
        rectBack.h = 480 + 2 * BORDERWIDTH + SHADOWWIDTH;
        cx = rectBack.x + BORDERWIDTH + SHADOWWIDTH;
        cy = rectBack.y + BORDERWIDTH;
    }else{
        rectBack.x = 0;
        rectBack.y = 0;
        rectBack.w = 640;
        rectBack.h = 480;
	cx = 0;
	cy = 0;
    }

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "DialogRecrutMonster: CreateRGBSurface failed: %s\n", SDL_GetError());
	return 0;
    }
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // отображаем фон диалога
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);
    rectCur.x = cx;
    rectCur.y = cy;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_FillRect(video, &rectCur, 0);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // нижний бар
    RedrawInfoBottomBar();

    // инфо о ресурсах
    RedrawCastleInfoResource();

    // кнопка exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 554;
    rectCur.y = cy + 428;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverExit;
    AddActionEvent(&castinfo, &action);
    // клик
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SWAPBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "SWAPBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionCastleClickExit;
    AddActionEvent(&castinfo, &action);

    // далее по расам
    switch(castle->race){
    
	default:
	    return NONE;
	    break;
	
	case BARBARIAN:
            icnname = "CSTLBARB.ICN";
	    break;

	case KNIGHT:
            icnname = "CSTLKNGT.ICN";
	    break;

	case NECROMANCER:
            icnname = "CSTLNECR.ICN";
	    break;

	case SORCERESS:
            icnname = "CSTLSORC.ICN";
	    break;

	case WARLOCK:
            icnname = "CSTLWRLK.ICN";
	    break;
	
	case WIZARD:
            icnname = "CSTLWZRD.ICN";
	    break;
    }

    // фон dwelling 1
    FillSPRITE(&sprite, icnname, 19);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 3;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling1;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling1(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling1;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringDwelling(castle->race, DWELLING_MONSTER1);
    rectCur.x = cx + 75;
    rectCur.y = cy + 60;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон dwelling 2
    if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE2) &&
	(castle->dwelling & DWELLING_MONSTER2)) FillSPRITE(&sprite, icnname, 25);
    else FillSPRITE(&sprite, icnname, 20);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 150;
    rectCur.y = cy + 3;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling2;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling2(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling2;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    if(castle->dwelling & DWELLING_MONSTER2 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE2);
    else
	message = GetStringDwelling(castle->race, DWELLING_MONSTER2);
    rectCur.x = cx + 225;
    rectCur.y = cy + 60;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон dwelling 3
    if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE3) &&
	(castle->dwelling & DWELLING_MONSTER3)) FillSPRITE(&sprite, icnname, 26);
    else FillSPRITE(&sprite, icnname, 21);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 3;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling3;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling3(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling3;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 60;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    if(castle->dwelling & DWELLING_MONSTER3 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE3);
    else
	message = GetStringDwelling(castle->race, DWELLING_MONSTER3);
    rectCur.x = cx + 370;
    rectCur.y = cy + 60;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон dwelling 4
    if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE4) &&
	(castle->dwelling & DWELLING_MONSTER4)) FillSPRITE(&sprite, icnname, 27);
    else FillSPRITE(&sprite, icnname, 22);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 78;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling4;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling4(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling4;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    if(castle->dwelling & DWELLING_MONSTER4 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE4);
    else
	message = GetStringDwelling(castle->race, DWELLING_MONSTER4);
    rectCur.x = cx + 75;
    rectCur.y = cy + 135;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон dwelling 5
    if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE5) &&
	(castle->dwelling & DWELLING_MONSTER5)) FillSPRITE(&sprite, icnname, 28);
    else FillSPRITE(&sprite, icnname, 23);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 150;
    rectCur.y = cy + 78;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling5;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling5(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling5;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    if(castle->dwelling & DWELLING_MONSTER5 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE5);
    else
	message = GetStringDwelling(castle->race, DWELLING_MONSTER5);
    rectCur.x = cx + 225;
    rectCur.y = cy + 135;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон dwelling 6
    if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE7) &&
	(castle->dwelling & DWELLING_UPGRADE6)) FillSPRITE(&sprite, icnname, 30);
    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE6) &&
	(castle->dwelling & DWELLING_MONSTER6)) FillSPRITE(&sprite, icnname, 29);
    else FillSPRITE(&sprite, icnname, 24);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 78;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverDwelling6;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildDwelling6(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickDwelling6;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 135;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    if(castle->dwelling & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE7);
    else if(castle->dwelling & DWELLING_MONSTER6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))
	message = GetStringDwelling(castle->race, DWELLING_UPGRADE6);
    else
	message = GetStringDwelling(castle->race, DWELLING_MONSTER6);
    rectCur.x = cx + 370;
    rectCur.y = cy + 135;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон magic tower
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 158;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverMageGuild;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildMageGuild(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickMageGuild;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringMageGuild();
    rectCur.x = cx + 75;
    rectCur.y = cy + 215;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон tawern
    if(NECROMANCER != castle->race){
	FillSPRITE(&sprite, icnname, 1);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 150;
	rectCur.y = cy + 158;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// наведение
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionCastleOverTavern;
	AddActionEvent(&castinfo, &action);
	// клик
	switch(AllowBuildTavern(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickTavern;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	}
	message = GetStringTavern();
	rectCur.x = cx + 225;
	rectCur.y = cy + 215;
	rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
	rectCur.y = rectCur.y + 1;
	rectCur.w = FONT_WIDTHSMALL * strlen(message);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);
    }
    
    // фон thieves guild
    NECROMANCER != castle->race ? FillSPRITE(&sprite, icnname, 2) : FillSPRITE(&sprite, icnname, 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 158;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverThievesGuild;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildThievesGuild(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickThievesGuild;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 215;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringThievesGuild();
    rectCur.x = cx + 370;
    rectCur.y = cy + 215;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон chip yard
    FillSPRITE(&sprite, icnname, 3);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 233;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverShipyard;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildShipyard(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickShipyard;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringShipyard();
    rectCur.x = cx + 75;
    rectCur.y = cy + 290;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон statue
    FillSPRITE(&sprite, icnname, 7);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 150;
    rectCur.y = cy + 233;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverStatue;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildStatue(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickStatue;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringStatue();
    rectCur.x = cx + 225;
    rectCur.y = cy + 290;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон market
    FillSPRITE(&sprite, icnname, 10);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 233;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverMarketplace;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildMarketplace(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickMarketplace;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 290;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringMarketplace();
    rectCur.x = cx + 370;
    rectCur.y = cy + 290;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон well
    FillSPRITE(&sprite, icnname, 4);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 308;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverWell;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildWell(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickWell;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringWell();
    rectCur.x = cx + 75;
    rectCur.y = cy + 365;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон wel2
    FillSPRITE(&sprite, icnname, 11);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 150;
    rectCur.y = cy + 308;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverWel2;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildWel2(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickWel2;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringWel2(castle->race);
    rectCur.x = cx + 225;
    rectCur.y = cy + 365;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон spec
    FillSPRITE(&sprite, icnname, 13);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 308;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverSpec;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildSpec(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickSpec;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 365;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringSpec(castle->race);
    rectCur.x = cx + 370;
    rectCur.y = cy + 365;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон l turret
    FillSPRITE(&sprite, icnname, 8);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 388;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverLeftTurret;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildLeftTurret(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickLeftTurret;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 5;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringLeftTurret();
    rectCur.x = cx + 75;
    rectCur.y = cy + 445;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон r turret
    FillSPRITE(&sprite, icnname, 9);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 150;
    rectCur.y = cy + 388;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverRightTurret;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildRightTurret(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickRightTurret;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 149;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringRightTurret();
    rectCur.x = cx + 225;
    rectCur.y = cy + 445;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // фон moat
    FillSPRITE(&sprite, icnname, 12);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 294;
    rectCur.y = cy + 388;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionCastleOverMoat;
    AddActionEvent(&castinfo, &action);
    // клик
    switch(AllowBuildMoat(castle)){
    	case BUILD_OK:
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionCastleClickMoat;
	    AddActionEvent(&castinfo, &action);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 1);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case CANNOT_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 12);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case END_TUR:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 13);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    FillSPRITE(&sprite, "CASLXTRA.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + 293;
	    rectCur.y = cy + 445;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
	case ALREADY_BUILD:
	    FillSPRITE(&sprite, "TOWNWIND.ICN", 11);
	    image = GetICNSprite(&sprite);
	    rectCur.x += 118;
	    rectCur.y += 40;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    break;
    }
    message = GetStringMoat();
    rectCur.x = cx + 370;
    rectCur.y = cy + 445;
    rectCur.x = rectCur.x - strlen(message) * FONT_WIDTHSMALL / 2;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    ActionCASTLEINFOLOOP(castinfo);

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(castinfo);
    SDL_FreeSurface(back);

    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return NONE;
}

ACTION ActionCASTLEINFOLOOP(INTERFACEACTION *action){

    SDL_Event event;
    SDL_Surface *video = SDL_GetVideoSurface();;
    ACTION exit = NONE;
    S_OLDOBJECT old;
    memset(old.object.name, 0, AGGSIZENAME);
    old.object.number = 0xFFFF;
    old.pushRect.x = 0;
    old.pushRect.y = 0;
    old.pushRect.w = 0;
    old.pushRect.h = 0;
    old.presRect = old.pushRect;
    old.flagPush = FALSE;
    old.flagPres = FALSE;
    INTERFACEACTION *ptr = NULL;

    Sint32 	cx, cy;

    // цикл по событиям
    while(exit == NONE){

	ptr = action;

	SDL_GetMouseState(&cx, &cy);

        if(old.flagPres && !ValidPoint(&old.presRect, cx, cy)){
            RedrawInfoBottomBar();
            old.flagPres = FALSE;
        }else
            while(ptr){
                if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, cx, cy) && ptr->pf && !old.flagPres){
                    old.presRect = ptr->rect;
                    old.flagPres = TRUE;
		    exit = (*ptr->pf)();
            	}
        	ptr = (INTERFACEACTION *) ptr->next;
	}

	while(SDL_PollEvent(&event)){

	    switch(event.type){
	    
		case SDL_QUIT:

		    // close windows
		    exit = EXIT;
		    break;

		case SDL_KEYDOWN:

		    // нажатия клавиатуры
		    switch(event.key.keysym.sym){

			case SDLK_ESCAPE:

			    exit = ESC;
			    break;

			// F4 switch to full screen
                	case SDLK_F4:

			    SDL_WM_ToggleFullScreen(video);

			    GetIntValue(FULLSCREEN) ? SetIntValue(FULLSCREEN, FALSE) : SetIntValue(FULLSCREEN, TRUE);

			    break;
								    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONDOWN:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка down
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK)){
				    old.pushRect = ptr->rect;
				    old.object = ptr->objectUp;
				    old.flagPush = TRUE;
				    DrawSprite(&ptr->rect, &ptr->objectPush);
				}
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			case SDL_BUTTON_RIGHT:
			    
			    if(GetIntValue(DEBUG)) fprintf(stderr, "x: %d, y: %d\n", event.button.x, event.button.y);
			    break;

			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONUP:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка up
			    ptr = action;
			    if(old.flagPush){
				DrawSprite(&old.pushRect, &old.object);
				old.flagPush = FALSE;
			    }

			    while(ptr){
				if(ValidPoint(&old.pushRect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK) && CompareRect(&ptr->rect, &old.pushRect) && ptr->pf )
					exit = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;
			    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEMOTION:

		    CursorShow();
		    break;

		default:
    		    break;
	    }

	}

	if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);

    }

    return exit;
}

void RedrawInfoBottomBar(void){

    AGGSPRITE sprite;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rectCur;

    FillSPRITE(&sprite, "CASLBAR.ICN", 0);
    image = GetICNSprite(&sprite);

    if(GetIntValue(VIDEOMODE)){
	rectCur.x =  video->w / 2 - 320;
	rectCur.y = 480 + 2 * BORDERWIDTH + SHADOWWIDTH - image->h;
    }else{
	rectCur.x = 0;
	rectCur.y = 480 - image->h;
    }

    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
}
