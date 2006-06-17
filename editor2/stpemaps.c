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
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "SDL.h"
#include "gamedefs.h"
#include "agg.h"
#include "tools.h"
#include "cursor.h"
#include "actionevent.h"
#include "element.h"
#include "config.h"
#include "draw.h"
#include "stpemaps.h"
#include "mp2maps.h"

typedef struct {
    MP2TILEINFO		*info;
    SDL_Surface		*tile;
} CELLMAPS;

void	ShowEditDisplay(void);
void	RedrawMapsArea(void);
void	DrawSingleTile(Uint8, Uint8);
void 	RedrawRadar(void);
void	RedrawRadarCursor(void);
void	FreeMaps(void);

void	InitRadar(void);
void	FreeRadar(void);

Uint8	GetAreaWidth(void);	// возвращает ширину видимой карты на экране
Uint8	GetAreaHeight(void);	// возвращает высоту видимой карты на экране

ACTION ActionPressScrollTopLeft(void);
ACTION ActionPressScrollTopRight(void);
ACTION ActionPressScrollTop(void);
ACTION ActionPressScrollPanelRight(void);
ACTION ActionPressScrollBottom(void);
ACTION ActionPressScrollBottomRight(void);
ACTION ActionPressScrollLeft(void);
ACTION ActionPressScrollPanelBottom(void);
ACTION ActionPressScrollRight(void);
ACTION ActionPressScrollBottomLeft(void);
ACTION ActionClickRadarArea(void);
ACTION ActionPressPreferences(void);
ACTION ActionPressOptions(void);
ACTION ActionPressSpecs(void);
ACTION ActionPressNewEdit(void);
ACTION ActionPressUndo(void);
ACTION ActionPressZoom(void);
ACTION ActionPressGround(void);
ACTION ActionPressTree(void);
ACTION ActionPressInfo(void);
ACTION ActionPressRiver(void);
ACTION ActionPressRoad(void);
ACTION ActionPressClear(void);
ACTION ActionClickMapsArea(void);
ACTION ActionSelectSmallClear(void);
ACTION ActionSelectMediumClear(void);
ACTION ActionSelectLargeClear(void);
ACTION ActionSelectManualClear(void);
ACTION ActionSelectSmallGround(void);
ACTION ActionSelectMediumGround(void);
ACTION ActionSelectLargeGround(void);
ACTION ActionSelectManualGround(void);
ACTION ActionSelectOceanGround(void);
ACTION ActionSelectGrassGround(void);
ACTION ActionSelectSnowGround(void);
ACTION ActionSelectSwampGround(void);
ACTION ActionSelectLavaGround(void);
ACTION ActionSelectDesertGround(void);
ACTION ActionSelectDirtGround(void);
ACTION ActionSelectWastelandGround(void);
ACTION ActionSelectBeachGround(void);
ACTION ActionSelectOceanObject(void);
ACTION ActionSelectGrassObject(void);
ACTION ActionSelectSnowObject(void);
ACTION ActionSelectSwampObject(void);
ACTION ActionSelectLavaObject(void);
ACTION ActionSelectDesertObject(void);
ACTION ActionSelectDirtObject(void);
ACTION ActionSelectWastelandObject(void);
ACTION ActionSelectBeachObject(void);
ACTION ActionSelectTownObject(void);
ACTION ActionSelectMonsterObject(void);
ACTION ActionSelectHeroesObject(void);
ACTION ActionSelectArtifactObject(void);
ACTION ActionSelectResourceObject(void);

ACTION PreferencePressNewMap(void);
ACTION PreferencePressLoadMap(void);
ACTION PreferencePressSaveMap(void);
ACTION PreferencePressQuit(void);
ACTION PreferencePressCancel(void);

ACTION OptionsClickAnimation(void);
ACTION OptionsClickCycling(void);
ACTION OptionsClickGrid(void);
ACTION OptionsClickCursor(void);
ACTION OptionsClickOkay(void);

INTERFACEACTION *stpemaps = NULL;

MP2HEADER	*ptrMP2Header = NULL;
CELLMAPS	*ptrMaps = NULL;

MP2ADDONTAIL	*tailAddon = NULL;
Uint32 		countTail = 0;

SDL_Surface	*radarArea = NULL;
SDL_Surface	*radarCursor = NULL;
SDL_Surface	*backgroundArea = NULL;

Uint8		offsetX	= 0;
Uint8		offsetY = 0;
Uint8		lastOffsetX = 0;
Uint8		lastOffsetY = 0;

ACTION DrawEditDisplay(){

    SDL_Rect dest;
    SDL_Surface *image;
    SDL_Surface *video;
	    
    INTERFACEACTION action;
    AGGSPRITE sprite;
    stpemaps = NULL;

    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue("fullscreen")) flag = flag | SDL_FULLSCREEN;

    // select video mode

    switch(GetIntValue("videomode")){

	default:
	case 0:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 640 || video->h != 480)
		video = SDL_SetVideoMode(640, 480, 16, flag);
	    break;

	case 1:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 800 || video->h != 576)
		video = SDL_SetVideoMode(800, 576, 16, flag);
	    break;
	
	case 2:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 1024 || video->h != 768)
		video = SDL_SetVideoMode(1024, 768, 16, flag);
	    break;
    }

    if(NULL == video){
	fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
	return EXIT;
    }

    SDL_Surface *formatSurface;
    Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xF000;
    gmask = 0x0F00;
    bmask = 0x00F0;
    amask = 0x000F;
#else
    rmask = 0x000F;
    gmask = 0x00F0;
    bmask = 0x0F00;
    amask = 0xF000;
#endif

    if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, GetAreaWidth() * TILEWIDTH, GetAreaHeight() * TILEWIDTH, 16, rmask, gmask, bmask, 0))){
	fprintf(stderr, "RedrawMapsArea: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), GetAreaWidth() * TILEWIDTH, GetAreaHeight() * TILEWIDTH);
	return EXIT;
    }

    backgroundArea = SDL_DisplayFormat(formatSurface);
    SDL_FreeSurface(formatSurface);

    offsetX = 0;
    offsetY = 0;
    lastOffsetX = 0;
    lastOffsetY = 0;

    // востановим курсор рабочей области
    dest.x = BORDERWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = video->w - 2 * BORDERWIDTH;
    dest.h = video->h - 2 * BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click по области карты
    dest.x = BORDERWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = GetAreaWidth() * TILEWIDTH;
    dest.h = GetAreaHeight() * TILEWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionClickMapsArea;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click scroll угловой верхний левый
    FillSPRITE(&sprite, "ESCROLL.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 12);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 13);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollTopLeft;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    FillSPRITE(&sprite, "EDITBTNS.ICN", 34);
    image = GetICNSprite(&sprite);
    dest.x += dest.w + image->w * GetAreaWidth();

    // click scroll угловой верхний правый
    FillSPRITE(&sprite, "ESCROLL.ICN", 14);
    image = GetICNSprite(&sprite);
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 14);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 15);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollTopRight;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click scroll верхний
    FillSPRITE(&sprite, "ESCROLL.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.y = BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 4);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollTop;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    /*
    // click scroll панель правый
    dest.y = ?;
    dest.y = ?;
    dest.w = BORDERWIDTH;
    dest.h = video->h - 2 * BORDERWIDTH - dest.y;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollPanelRight;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    */

    // click scroll нижний
    FillSPRITE(&sprite, "ESCROLL.ICN", 6);
    image = GetICNSprite(&sprite);
    dest.y = video->h - 2 * BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 6);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollBottom;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click scroll угловой правый нижний 
    FillSPRITE(&sprite, "ESCROLL.ICN", 16);
    image = GetICNSprite(&sprite);
    dest.y = video->h - BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 16);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 17);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollBottomRight;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click scroll правый
    FillSPRITE(&sprite, "ESCROLL.ICN", 10);
    image = GetICNSprite(&sprite);
    dest.x -= image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 10);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 11);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollRight;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    /*
    // click панель нижний
    dest.x = ?;
    dest.y = ?;
    dest.w = dest.x - 2 * BORDERWIDTH;
    dest.h = BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollPanelBottom;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    */

    // click scroll левый
    FillSPRITE(&sprite, "ESCROLL.ICN", 8);
    image = GetICNSprite(&sprite);
    dest.x = BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 8);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 9);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollLeft;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click scroll угловой левый нижний
    FillSPRITE(&sprite, "ESCROLL.ICN", 18);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_POINTER;
    // регистрируем
    AddActionEvent(&stpemaps, &action);
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "ESCROLL.ICN", 18);
    FillSPRITE(&action.objectPush, "ESCROLL.ICN", 19);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollBottomLeft;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // scroll mouse левый
    dest.x = 0;
    dest.y = BORDERWIDTH;
    dest.w = BORDERWIDTH;
    dest.h = video->h - 2 * BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_SCROLL_LEFT;
    action.pf = ActionPressScrollLeft;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // scroll mouse правый
    dest.x = video->w - BORDERWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = BORDERWIDTH;
    dest.h = video->h - 2 * BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_SCROLL_RIGHT;
    action.pf = ActionPressScrollRight;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // scroll mouse верхний
    dest.x = BORDERWIDTH;
    dest.y = 0;
    dest.w = (GetAreaWidth() - 1) * TILEWIDTH;
    dest.h = BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_SCROLL_TOP;
    action.pf = ActionPressScrollTop;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // scroll mouse нижний
    dest.x = 2 * BORDERWIDTH;
    dest.y = video->h - BORDERWIDTH;
    dest.w = (GetAreaWidth() - 1) * TILEWIDTH;
    dest.h = BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.cursorMotion = CURSOR_SCROLL_BOTTOM;
    action.pf = ActionPressScrollBottom;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button zoom
    FillSPRITE(&sprite, "EDITPANL.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.h = image->h;
    FillSPRITE(&sprite, "EDITBTNS.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.x = video->w - BORDERWIDTH - RADARWIDTH;
    dest.y = BORDERWIDTH + RADARWIDTH + 2 * image->h + dest.h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 12);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 13);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressZoom;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button undo
    FillSPRITE(&sprite, "EDITBTNS.ICN", 14);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 14);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 15);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressUndo;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button new
    FillSPRITE(&sprite, "EDITBTNS.ICN", 16);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 16);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 17);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewEdit;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button specs
    FillSPRITE(&sprite, "EDITPANL.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.h = image->h;
    FillSPRITE(&sprite, "EDITBTNS.ICN", 18);
    image = GetICNSprite(&sprite);
    dest.x = video->w - BORDERWIDTH - RADARWIDTH;
    dest.y = BORDERWIDTH + RADARWIDTH + 3 * image->h + dest.h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 18);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 19);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressSpecs;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button preferences
    FillSPRITE(&sprite, "EDITBTNS.ICN", 20);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 20);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 21);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressPreferences;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button options
    FillSPRITE(&sprite, "EDITBTNS.ICN", 22);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 22);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 23);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressOptions;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // radar
    dest.x = video->w - BORDERWIDTH - RADARWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = RADARWIDTH;
    dest.h = RADARWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionClickRadarArea;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button ground
    FillSPRITE(&sprite, "EDITBTNS.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.y += RADARWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 0);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button tree
    FillSPRITE(&sprite, "EDITBTNS.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 2);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressTree;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button info
    FillSPRITE(&sprite, "EDITBTNS.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 4);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressInfo;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button clear
    FillSPRITE(&sprite, "EDITBTNS.ICN", 10);
    image = GetICNSprite(&sprite);
    dest.y += image->h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 10);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 11);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressClear;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button road
    FillSPRITE(&sprite, "EDITBTNS.ICN", 8);
    image = GetICNSprite(&sprite);
    dest.x -= image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 8);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 9);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressRoad;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // button river
    FillSPRITE(&sprite, "EDITBTNS.ICN", 6);
    image = GetICNSprite(&sprite);
    dest.x -= image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 6);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressRiver;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // инициализируем радар
    InitRadar();

    // отображаем картинку
    ShowEditDisplay();
    RedrawMapsArea();
    
    // идем в цикл сообщений
    ACTION result;

    while(! (EXIT == (result = ActionCycle(stpemaps)) || (ESC == result && YES == MessageBox("Are you sure you want to\n\t\t\t quit?", FONT_BIG))) );

    result = EXIT;

    // овобождаем данные
    FreeActionEvent(stpemaps);
    FreeRadar();
    SDL_FreeSurface(backgroundArea);
    
    return result;
}

void ShowEditDisplay(void){

    SDL_Surface *video;
    SDL_Surface *image;
    SDL_Rect dest, src;
    Uint16 i;
    AGGSPRITE sprite;
        
    video = SDL_GetVideoSurface();

    SetCursor(CURSOR_POINTER);
    CursorOff();
    SDL_FillRect(video, NULL, SDL_MapRGB(video->format, 0x00, 0x00, 0x00));

    // show border
    FillSPRITE(&sprite, "ADVBORD.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - image->w;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);
    if(GetIntValue("videomode")){
	dest.y = video->h - image->h;
	SDL_BlitSurface(image, NULL, video, &dest);
    }

    // fill black
    dest.x = BORDERWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = video->w - 2 * BORDERWIDTH;
    dest.h = video->h - 2 * BORDERWIDTH;
    SDL_FillRect(video, &dest, SDL_MapRGB(video->format, 0x00 ,0x00, 0x00));

    // scroll панель правый
    FillSPRITE(&sprite, "ESCROLL.ICN", 1);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
    dest.y = 2 * BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);
    if(GetIntValue("videomode")){
	src.x = 0;
	src.y = 5;
	src.w = dest.w;
	src.h = dest.h - src.y;
	dest.y = video->h - image->h - 2 * BORDERWIDTH + src.y;
	SDL_BlitSurface(image, &src, video, &dest);
    }

    // scroll панель нижний
    FillSPRITE(&sprite, "ESCROLL.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 2 * BORDERWIDTH;
    dest.y = video->h - BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);
    if(GetIntValue("videomode")){
	src.x = 5;
	src.y = 0;
	src.w = dest.w - src.x;
	src.h = dest.h;
	dest.x = video->w - RADARWIDTH - image->w - 3 * BORDERWIDTH + src.x;
	SDL_BlitSurface(image, &src, video, &dest);
    }

    // show top cubic
    FillSPRITE(&sprite, "ESCROLL.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.x = image->w;
    FillSPRITE(&sprite, "EDITBTNS.ICN", 34);
    image = GetICNSprite(&sprite);
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;
    for(i = 0; i < GetAreaWidth(); ++i){
	SDL_BlitSurface(image, NULL, video, &dest);
	dest.x += image->w;
    }

    // show left cubic
    FillSPRITE(&sprite, "ESCROLL.ICN", 12);
    image = GetICNSprite(&sprite);
    dest.y = image->h;
    FillSPRITE(&sprite, "EDITBTNS.ICN", 33);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.w = image->w;
    dest.h = image->h;
    for(i = 0; i < GetAreaHeight(); ++i){
	SDL_BlitSurface(image, NULL, video, &dest);
	dest.y += image->h;
    }

    // фон панели editpanl
    FillSPRITE(&sprite, "EDITPANL.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // заполняем пустое пространство правый нижний квадрат
    if(GetIntValue("videomode")){
	FillSPRITE(&sprite, "EDITBTNS.ICN", 18);
	image = GetICNSprite(&sprite);
	dest.h = image->h;
	FillSPRITE(&sprite, "EDITPANL.ICN", 3);
	image = GetICNSprite(&sprite);
	// top
	dest.x = video->w - BORDERWIDTH - RADARWIDTH;
	dest.y = BORDERWIDTH + RADARWIDTH + 4 * dest.h + image->h;
	dest.w = image->w;
	dest.h = 32;
	src.x = 0;
	src.y = 0;
	src.w = image->w;
	src.h = 32;
	SDL_BlitSurface(image, &src, video, &dest);
	// middle
	dest.x = video->w - BORDERWIDTH - RADARWIDTH;
	src.y = 5;
	while( video->h - BORDERWIDTH - 64 > dest.y){
	    dest.y += 32;
	    SDL_BlitSurface(image, &src, video, &dest);
	}
	// bottom
	dest.y += 32;
	src.y = image->h - 32;
	SDL_BlitSurface(image, &src, video, &dest);
    }
    // динамические элементы
    INTERFACEACTION *ptr = stpemaps;
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
    	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);

    // перерисовываем радар
    RedrawRadar();

    CursorOn();
}

void InitRadar(void){

    if(radarArea || radarCursor) return;

    int i, j;
    Uint16 index = 0;

    SDL_Surface *tiles = NULL;
    SDL_Surface *image = NULL;

    Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xF000;
    gmask = 0x0F00;
    bmask = 0x00F0;
    amask = 0x000F;
#else
    rmask = 0x000F;
    gmask = 0x00F0;
    bmask = 0x0F00;
    amask = 0xF000;
#endif

    SDL_Rect dest, dst, src;
    dest.x = 0;
    dest.y = 0;

    if(NULL == (radarArea = SDL_CreateRGBSurface(SDL_SWSURFACE, RADARWIDTH, RADARWIDTH, 16, rmask, gmask, bmask, 0))){
	fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), RADARWIDTH, RADARWIDTH);
	return;
    }
    for(j = 0; j < ptrMP2Header->heightMaps; ++j){

	// scale radar heigth (MAPS_LARGE)
	if(ptrMP2Header->heightMaps == MAPS_LARGE && 2 == j % 3){
	    src.x = 0;
	    src.y = dest.y - dest.h;
	    src.w = RADARWIDTH;
	    src.h = dest.h;
	    dst = src;
	    dst.y += dest.h;
	    SDL_BlitSurface(radarArea, &src, radarArea, &dst);
	    dest.y += dest.h;
	}

	for(i = 0; i < ptrMP2Header->widthMaps; ++i){

	    tiles = ptrMaps[index].tile;

    	    if(NULL == (image = SDL_CreateRGBSurface(SDL_SWSURFACE, RADARWIDTH / ptrMP2Header->widthMaps, RADARWIDTH / ptrMP2Header->heightMaps, 16, rmask, gmask, bmask, 0))){
		fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), RADARWIDTH / ptrMP2Header->widthMaps, RADARWIDTH / ptrMP2Header->heightMaps);
		return;
	    }

	    ScaleSurface(tiles, image);
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, radarArea, &dest);

	    // scale radar width (MAPS_LARGE)
	    if(ptrMP2Header->widthMaps == MAPS_LARGE && 2 == i % 3){
	    	dest.x += dest.w;
		SDL_BlitSurface(image, NULL, radarArea, &dest);
	    }

	    SDL_FreeSurface(image);

	    dest.x += dest.w;
	    ++index;
	}
	dest.x = 0;
	dest.y += dest.h;
    }

    // рисуем курсор радара
    float scale = 0;
    scale = RADARWIDTH / (float) ptrMP2Header->widthMaps;
    Uint8 widthCursor = GetAreaWidth() * scale;
    scale = RADARWIDTH / (float) ptrMP2Header->heightMaps;
    Uint8 heightCursor = GetAreaHeight() * scale;

    if(NULL == (radarCursor = SDL_CreateRGBSurface(SDL_SWSURFACE, widthCursor, heightCursor, 16, rmask, gmask, bmask, 0))){
	fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), widthCursor, heightCursor);
	return;
    }

    SDL_FillRect(radarCursor, NULL, COLORKEY);
    SDL_SetColorKey(radarCursor, SDL_SRCCOLORKEY|SDL_RLEACCEL, COLORKEY);
    // индекс цвета из палитры
    Uint16 color = RADARCOLOR;

    j = 0;
    for(i = 0; i < widthCursor; ++i){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i + 1, j, color);
	i += 3;
    }
    j = heightCursor - 1;
    for(i = 0; i < widthCursor; ++i){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i + 1, j, color);
	i += 3;
    }
    i = 0;
    for(j = 0; j < heightCursor; ++j){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i, j + 1, color);
	j += 3;
    }
    i = widthCursor - 1;
    for(j = 0; j < heightCursor; ++j){
	DrawPixel(radarCursor, i, j, color);
	DrawPixel(radarCursor, i, j + 1, color);
	j += 3;
    }

    return;
}

void FreeRadar(void){

    if(NULL != radarArea) SDL_FreeSurface(radarArea);
    if(NULL != radarCursor) SDL_FreeSurface(radarCursor);

    radarArea = NULL;
    radarCursor = NULL;
}

void RedrawRadar(void){

    if(NULL == radarArea) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_Rect dest;
    dest.x = video->w - BORDERWIDTH - RADARWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = radarArea->w;
    dest.h = radarArea->h;
    
    SDL_BlitSurface(radarArea, NULL, video, &dest);

    dest.x = video->w - BORDERWIDTH - RADARWIDTH + offsetX * RADARWIDTH / ptrMP2Header->widthMaps;
    dest.y = BORDERWIDTH + offsetY * RADARWIDTH / ptrMP2Header->heightMaps;
    dest.w = radarCursor->w;
    dest.h = radarCursor->h;

    SDL_BlitSurface(radarCursor, NULL, video, &dest);

    SDL_Flip(video);
}

void RedrawRadarCursor(void){

    if(NULL == radarArea) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_Rect dest, src;

    // востанавливаем старое положение
    src.x = lastOffsetX * RADARWIDTH / ptrMP2Header->widthMaps;
    src.y = lastOffsetY * RADARWIDTH / ptrMP2Header->heightMaps;
    src.w = radarCursor->w;
    src.h = radarCursor->h;

    dest.x = video->w - BORDERWIDTH - RADARWIDTH + src.x;
    dest.y = BORDERWIDTH + src.y;
    dest.w = src.w;
    dest.h = src.h;
    
    SDL_BlitSurface(radarArea, &src, video, &dest);

    // рисуем новое положение
    dest.x = video->w - BORDERWIDTH - RADARWIDTH + offsetX * RADARWIDTH / ptrMP2Header->widthMaps;
    dest.y = BORDERWIDTH + offsetY * RADARWIDTH / ptrMP2Header->heightMaps;
    dest.w = radarCursor->w;
    dest.h = radarCursor->h;

    SDL_BlitSurface(radarCursor, NULL, video, &dest);

    SDL_Flip(video);
}

ACTION ActionPressScrollTopLeft(void){

    BOOL redraw = FALSE;

    if(offsetX){
	lastOffsetX = offsetX;
	--offsetX;
	redraw = TRUE;
    }

    if(offsetY){
	lastOffsetY = offsetY;
	--offsetY;
	redraw = TRUE;
    }

    if(redraw) RedrawMapsArea();

    return NONE;
}

ACTION ActionPressScrollTopRight(void){

    BOOL redraw = FALSE;
    
    if(offsetX < ptrMP2Header->widthMaps){
	lastOffsetX = offsetX;
	++offsetX;
	redraw = TRUE;
    }
    
    if(offsetY){
	lastOffsetY = offsetY;
	--offsetY;
	redraw = TRUE;
    }

    if(redraw) RedrawMapsArea();

    return NONE;
}

ACTION ActionPressScrollTop(void){

    if(offsetY){
	lastOffsetX = offsetX;
	lastOffsetY = offsetY;
	--offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionPressScrollPanelRight(void){

    printf("scroll: panel right\n");
    return NONE;
}

ACTION ActionPressScrollBottom(void){

    if(offsetY < ptrMP2Header->heightMaps - GetAreaHeight()){
	lastOffsetX = offsetX;
	lastOffsetY = offsetY;
	++offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionPressScrollBottomRight(void){

    BOOL redraw = FALSE;
    
    if(offsetY < ptrMP2Header->heightMaps - GetAreaHeight()){
	lastOffsetY = offsetY;
	++offsetY;
	redraw = TRUE;
    }
    
    if(offsetX < ptrMP2Header->widthMaps - GetAreaWidth()){
	lastOffsetX = offsetX;
	++offsetX;
	redraw = TRUE;
    }
    
    if(redraw) RedrawMapsArea();

    return NONE;
}

ACTION ActionPressScrollRight(void){

    if(offsetX < ptrMP2Header->widthMaps - GetAreaWidth()){
	lastOffsetY = offsetY;
	lastOffsetX = offsetX;
	++offsetX;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionPressScrollPanelBottom(void){

    printf("scroll: panel bottom\n");
    return NONE;
}

ACTION ActionPressScrollLeft(void){

    if(offsetX){
	lastOffsetY = offsetY;
	lastOffsetX = offsetX;
	--offsetX;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionPressScrollBottomLeft(void){

    BOOL redraw = FALSE;
    
    if(offsetY < ptrMP2Header->heightMaps - GetAreaHeight()){
	lastOffsetY = offsetY;
	++offsetY;
	redraw = TRUE;
    }
    
    if(offsetX){
	lastOffsetX = offsetX;
	--offsetX;
	redraw = TRUE;
    }

    if(redraw) RedrawMapsArea();

    return NONE;
}

ACTION ActionClickRadarArea(void){

    int x, y;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_GetMouseState(&x, &y);
    
    // верхний правый угол отображения
    lastOffsetX = offsetX;
    lastOffsetY = offsetY;
    offsetX = (x - (video->w - BORDERWIDTH - RADARWIDTH)) * ptrMP2Header->widthMaps / RADARWIDTH;
    offsetY = (y - BORDERWIDTH) * ptrMP2Header->heightMaps / RADARWIDTH;
    
    // центрируем
    if(0 > offsetX - GetAreaWidth() / 2)
	offsetX = 0;
    else
	offsetX = offsetX - GetAreaWidth() / 2;
    

    if(0 > offsetY - GetAreaHeight() / 2)
	offsetY = 0;
    else
	offsetY = offsetY - GetAreaHeight() / 2;
    

    // проверка за границы
    if(offsetY > ptrMP2Header->heightMaps - GetAreaHeight())
	offsetY = ptrMP2Header->heightMaps - GetAreaHeight();
    

    if(offsetX > ptrMP2Header->widthMaps - GetAreaWidth())
	offsetX = ptrMP2Header->widthMaps - GetAreaWidth();
    
    RedrawMapsArea();

    return NONE;
}

ACTION ActionPressPreferences(void){

    CursorOff();

    // Сохраняем курсор
    Uint32 cursor = GetCursor();    

    AGGSPRITE sprite;
    SDL_Rect rectCur, rectBack;
    ACTION result = NONE;

    INTERFACEACTION action;
    INTERFACEACTION *dialogPreferences = NULL;

    SDL_Surface *format = NULL;
    SDL_Surface *back = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    // фон панели cpanbkg
    FillSPRITE(&sprite, "CPANBKG.ICN", 0);
    SDL_Surface *image = GetICNSprite(&sprite);

    // сохраняем background
    rectBack.x = (video->w - image->w) / 2;
    rectBack.y = (video->h - image->h) / 2;
    rectBack.w = image->w;
    rectBack.h = image->h;
    format = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0, 0, 0, 0);
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    //  по центру
    SDL_BlitSurface(image, NULL, video, &rectBack);

    // New Game
    FillSPRITE(&sprite, "CPANEL.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 62;
    rectCur.y = rectBack.y + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "CPANEL.ICN", 0);
    FillSPRITE(&action.objectPush, "CPANEL.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = PreferencePressNewMap;
    AddActionEvent(&dialogPreferences, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Load Game
    FillSPRITE(&sprite, "CPANEL.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 195;
    rectCur.y = rectBack.y + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "CPANEL.ICN", 2);
    FillSPRITE(&action.objectPush, "CPANEL.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = PreferencePressLoadMap;
    AddActionEvent(&dialogPreferences, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Save Game
    FillSPRITE(&sprite, "CPANEL.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 62;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "CPANEL.ICN", 4);
    FillSPRITE(&action.objectPush, "CPANEL.ICN", 5);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = PreferencePressSaveMap;
    AddActionEvent(&dialogPreferences, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Quit Game
    FillSPRITE(&sprite, "CPANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 195;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "CPANEL.ICN", 6);
    FillSPRITE(&action.objectPush, "CPANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = PreferencePressQuit;
    AddActionEvent(&dialogPreferences, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // cancel
    FillSPRITE(&sprite, "CPANEL.ICN", 8);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 128;
    rectCur.y = rectBack.y + 183;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "CPANEL.ICN", 8);
    FillSPRITE(&action.objectPush, "CPANEL.ICN", 9);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = PreferencePressCancel;
    AddActionEvent(&dialogPreferences, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // отрисовка диалога
    SDL_Flip(video);

    // Стандартный курсор
    SetCursor(CURSOR_POINTER);

    CursorOn();

    // в цикл событий
    result = ActionCycle(dialogPreferences);

    if(ESC == result) result = NONE;

    // востанавливаем background
    CursorOff();

    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialogPreferences);
    SDL_FreeSurface(back);

    // Востанавливаем курсор
    SetCursor(cursor);    

    CursorOn();

    return result;
}

ACTION ActionPressOptions(void){

    CursorOff();

    // Сохраняем курсор
    Uint32 cursor = GetCursor();    

    AGGSPRITE sprite;
    SDL_Rect rectCur, rectBack;

    INTERFACEACTION action;
    INTERFACEACTION *dialogOptions = NULL;

    ACTION result = NONE;

    SDL_Surface *format = NULL;
    SDL_Surface *back = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    // фон панели espanbkg
    FillSPRITE(&sprite, "ESPANBKG.ICN", 0);
    SDL_Surface *image = GetICNSprite(&sprite);

    Uint8 widthshadow = 16;
    
    // сохраняем background
    rectBack.x = (video->w - image->w) / 2 - widthshadow;
    rectBack.y = (video->h - image->h) / 2;
    rectBack.w = image->w + widthshadow;
    rectBack.h = image->h + widthshadow;
    format = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0);
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // по центру
    rectCur = rectBack;
    // тень
    FillSPRITE(&sprite, "ESPANBKG.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.y = rectBack.y + widthshadow;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // картинка
    FillSPRITE(&sprite, "ESPANBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + widthshadow;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // animation
    FillSPRITE(&sprite, "ESPANEL.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 85;
    rectCur.y = rectBack.y + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "ESPANEL.ICN", 0);
    FillSPRITE(&action.objectPush, "ESPANEL.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = OptionsClickAnimation;
    AddActionEvent(&dialogOptions, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // cycling
    FillSPRITE(&sprite, "ESPANEL.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 203;
    rectCur.y = rectBack.y + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "ESPANEL.ICN", 2);
    FillSPRITE(&action.objectPush, "ESPANEL.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = OptionsClickCycling;
    AddActionEvent(&dialogOptions, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    
    // grid
    FillSPRITE(&sprite, "ESPANEL.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 85;
    rectCur.y = rectBack.y + 157;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "ESPANEL.ICN", 4);
    FillSPRITE(&action.objectPush, "ESPANEL.ICN", 5);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = OptionsClickGrid;
    AddActionEvent(&dialogOptions, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    
    // cursor
    FillSPRITE(&sprite, "ESPANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 203;
    rectCur.y = rectBack.y + 157;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "ESPANEL.ICN", 6);
    FillSPRITE(&action.objectPush, "ESPANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = OptionsClickCursor;
    AddActionEvent(&dialogOptions, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    
    // okay
    FillSPRITE(&sprite, "ESPANBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 128;
    rectCur.y = rectBack.y + 252;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "ESPANBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "ESPANBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = OptionsClickOkay;
    AddActionEvent(&dialogOptions, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // отрисовка диалога
    SDL_Flip(video);

    // Стандартный курсор
    SetCursor(CURSOR_POINTER);

    CursorOn();

    // в цикл событий
    result = ActionCycle(dialogOptions);

    if(OK == result){
	
	fprintf(stderr, "save options\n");
    }

    if(EXIT != result) result = NONE;

    // востанавливаем background
    CursorOff();

    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialogOptions);
    SDL_FreeSurface(back);

    // Востанавливаем курсор
    SetCursor(cursor);    

    CursorOn();

    return result;
}

ACTION ActionPressSpecs(void){

    printf("push: specs\n");
    return NONE;
}

ACTION ActionPressNewEdit(void){

    printf("push: new\n");
    return NONE;
}

ACTION ActionPressUndo(void){

    printf("push: undo\n");
    return NONE;
}

ACTION ActionPressZoom(void){

    printf("push: zoom\n");
    return NONE;
}

ACTION ActionPressGround(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;
    INTERFACEACTION action;
    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 0);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // select small ground
    FillSPRITE(&sprite, "EDITBTNS.ICN", 24);
    image = GetICNSprite(&sprite);
    dest.x += 15;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 24);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 25);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSmallGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select medium ground
    FillSPRITE(&sprite, "EDITBTNS.ICN", 26);
    image = GetICNSprite(&sprite);
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 26);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 27);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectMediumGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select large ground
    FillSPRITE(&sprite, "EDITBTNS.ICN", 28);
    image = GetICNSprite(&sprite);
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 28);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 29);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectLargeGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select manual ground
    FillSPRITE(&sprite, "EDITBTNS.ICN", 30);
    image = GetICNSprite(&sprite);
    //dest.x = 586;
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 30);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 31);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectMediumGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select ocean
    dest.x = video->w - BORDERWIDTH - RADARWIDTH + 31;
    dest.y = 244;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectOceanGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select grass
    dest.x += 28;
    dest.y = 244;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectGrassGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select snow
    dest.x += 28;
    dest.y = 244;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSnowGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select swamp
    dest.x = video->w - BORDERWIDTH - RADARWIDTH + 31;
    dest.y = 272;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSwampGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select lava
    dest.x += 28;
    dest.y = 272;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectLavaGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select desert
    dest.x += 28;
    dest.y = 272;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDesertGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select dirt
    dest.x = video->w - BORDERWIDTH - RADARWIDTH + 31;
    dest.y = 300;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDirtGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select wasteland
    dest.x += 28;
    dest.y = 300;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectWastelandGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select beach
    dest.x += 28;
    dest.y = 300;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectBeachGround;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // отрисовываем элементы
    INTERFACEACTION *ptr = stpemaps;
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)) && ptr->level == LEVELEVENT_EDITPANEL)
    	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);

    return NONE;
}

ACTION ActionPressTree(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;
    INTERFACEACTION action;
    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 1);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // select ocean object
    //dest.x = 498;
    dest.x += 17;
    dest.y = 246;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectOceanObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select grass object
    //dest.x = 526;
    dest.x += 28;
    dest.y = 246;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectGrassObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select snow object
    //dest.x = 554;
    dest.x += 28;
    dest.y = 246;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSnowObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select swamp object
    //dest.x = 582;
    dest.x += 28;
    dest.y = 246;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSwampObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select lava object
    //dest.x = 498;
    dest.x = video->w - BORDERWIDTH - image->w + 17;
    dest.y = 274;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectLavaObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select desert object
    //dest.x = 526;
    dest.x += 28;
    dest.y = 274;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDesertObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select dirt object
    //dest.x = 554;
    dest.x += 28;
    dest.y = 274;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDirtObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select wasteland object
    //dest.x = 582;
    dest.x += 28;
    dest.y = 274;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectWastelandObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select beach object
    //dest.x = 498;
    dest.x = video->w - BORDERWIDTH - image->w + 17;
    dest.y = 302;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectBeachObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select town object
    //dest.x = 526;
    dest.x += 28;
    dest.y = 302;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectTownObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select monster object
    //dest.x = 554;
    dest.x += 28;
    dest.y = 302;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectMonsterObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select heroes object
    //dest.x = 582;
    dest.x += 28;
    dest.y = 302;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectHeroesObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select artifact object
    //dest.x = 498;
    dest.x = video->w - BORDERWIDTH - image->w + 17;
    dest.y = 330;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectArtifactObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // select resource object
    //dest.x = 582;
    dest.x = video->w - BORDERWIDTH - image->w + 101;
    dest.y = 330;
    dest.w = 26;
    dest.h = 26;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectResourceObject;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    SDL_Flip(video);

    return NONE;
}

ACTION ActionPressInfo(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;

    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 2);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    DrawSprite(&dest, &sprite);

    return NONE;
}

ACTION ActionPressRiver(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;

    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 3);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    DrawSprite(&dest, &sprite);

    return NONE;
}

ACTION ActionPressRoad(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;

    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 4);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    DrawSprite(&dest, &sprite);

    return NONE;
}

ACTION ActionPressClear(void){

    // фон панели editpanl
    AGGSPRITE sprite;
    SDL_Rect dest;
    INTERFACEACTION action;
    SDL_Surface *video = SDL_GetVideoSurface();

    RemoveActionLevelEvent(stpemaps, LEVELEVENT_EDITPANEL);

    FillSPRITE(&sprite, "EDITPANL.ICN", 5);
    SDL_Surface *image = GetICNSprite(&sprite);
    //dest.x = 481;
    dest.x = video->w - BORDERWIDTH - image->w;
    dest.y = 232;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // click small clear
    FillSPRITE(&sprite, "EDITBTNS.ICN", 24);
    image = GetICNSprite(&sprite);
    //dest.x = 496;
    dest.x += 15;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 24);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 25);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectSmallClear;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click medium clear
    FillSPRITE(&sprite, "EDITBTNS.ICN", 26);
    image = GetICNSprite(&sprite);
    //dest.x = 526;
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 26);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 27);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectMediumClear;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click large clear
    FillSPRITE(&sprite, "EDITBTNS.ICN", 28);
    image = GetICNSprite(&sprite);
    //dest.x = 556;
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 28);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 29);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectLargeClear;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // click manual clear
    FillSPRITE(&sprite, "EDITBTNS.ICN", 30);
    image = GetICNSprite(&sprite);
    //dest.x = 586;
    dest.x += 30;
    dest.y = 360;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "EDITBTNS.ICN", 30);
    FillSPRITE(&action.objectPush, "EDITBTNS.ICN", 31);
    action.level = LEVELEVENT_EDITPANEL;
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectMediumClear;
    // регистрируем
    AddActionEvent(&stpemaps, &action);

    // отрисовываем элементы
    INTERFACEACTION *ptr = stpemaps;
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)) && ptr->level == LEVELEVENT_EDITPANEL)
    	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);

    return NONE;
}

void FreeMaps(void){

    int i;

    if(ptrMP2Header){

	if(ptrMaps){
	    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){
		if(ptrMaps[i].info) free(ptrMaps[i].info);
		if(ptrMaps[i].tile) SDL_FreeSurface(ptrMaps[i].tile);
	    }

	    free(ptrMaps);
	}

	free(ptrMP2Header);
    }
    
    if(tailAddon) free(tailAddon);
}

ACTION LoadMaps(char *filename){

    char        pwd[PATH_MAX];
    FILE        *fd = NULL;

    if(NULL == getcwd(pwd, PATH_MAX)){
        fprintf(stderr, "error getcwd\n");
        return EXIT;
    }
    chdir("maps");

    // open file
    if(0 > (fd = fopen(filename, "rb"))){
        fprintf(stderr, "error fopen: %s\n", filename);
	return EXIT;
    }

    Uint32 filesize;
    fseek(fd, 0, SEEK_END);
    filesize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // read buffer MP2HEADER
    if(NULL == (ptrMP2Header = (MP2HEADER *) malloc(sizeof(MP2HEADER)))){
	printf("malloc: error size %d\n", sizeof(MP2HEADER));
        return EXIT;
    }
    fread((char *) ptrMP2Header, sizeof(MP2HEADER), 1, fd);

    if(ptrMP2Header->headerMagic != 0x0000005C){
	fprintf(stderr, "load error: incorrect MP2 maps!\n");
	return EXIT;
    }

    // read buffer CELLMAPS
    if(NULL == (ptrMaps = (CELLMAPS *) malloc(sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps))){
	fprintf(stderr, "malloc: error size %d\n", sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps);
        return EXIT;
    }

    // читаем все ячейки карты
    Uint8  *ptrTILData = GetTILData("GROUND32.TIL");

    int i;

    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

	if(NULL == (ptrMaps[i].info = (MP2TILEINFO *) malloc(sizeof(MP2TILEINFO)))){
	    fprintf(stderr, "malloc: error size %d\n", sizeof(MP2TILEINFO));
	    return EXIT;
	}
	
	fread((char *) ptrMaps[i].info, 1, sizeof(MP2TILEINFO), fd);

        Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xF000;
        gmask = 0x0F00;
	bmask = 0x00F0;
	amask = 0x000F;
#else
        rmask = 0x000F;
        gmask = 0x00F0;
        bmask = 0x0F00;
        amask = 0xF000;
#endif
        if(NULL == (ptrMaps[i].tile = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, TILEWIDTH, TILEWIDTH, 16, rmask, gmask, bmask, 0))){
	    fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), TILEWIDTH, TILEWIDTH);
	    return EXIT;
	}
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMaps[i].info->tileIndex, ptrMaps[i].info->shape);
    }


    fread((char *) &countTail, 1, sizeof(Uint32), fd);

    if(filesize < (ftell(fd) + countTail * 15)){
	printf("LoadMaps error: incorrect format maps.\n");
        return EXIT;
    }
    
    if(NULL == (tailAddon = (MP2ADDONTAIL *) malloc(countTail * sizeof(MP2ADDONTAIL)))){
	printf("LoadMaps malloc: error size %lu\n", (unsigned long) countTail * sizeof(MP2ADDONTAIL));
        return EXIT;
    }

    // read TailADDON
    for(i = 0; i < countTail; ++i){
        
        fread((char *) &tailAddon[i].indexAddon, sizeof(Uint16), 1, fd);
        fread((char *) &tailAddon[i].objectNameN1, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].indexNameN1, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].quantityN, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].objectNameN2, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].indexNameN2, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].uniqNumberN1, sizeof(Uint32), 1, fd);
        fread((char *) &tailAddon[i].uniqNumberN2, sizeof(Uint32), 1, fd);
    }

    // close file
    fclose(fd);
    chdir(pwd);

    // Освобождаем загруженные объекты TIL
    FreeObject("GROUND32.TIL");

    fprintf(stderr, "LoadMaps: %s\n", filename);

    // Рисуем экран и в цикл событий
    ACTION result = DrawEditDisplay();

    FreeMaps();

    return result;
}

ACTION NewMaps(SIZEMAP sizeMap){

    // read buffer MP2HEADER
    if(NULL == (ptrMP2Header = (MP2HEADER *) malloc(sizeof(MP2HEADER)))){
	printf("malloc: error size %d\n", sizeof(MP2HEADER));
        return EXIT;
    }

    ptrMP2Header->headerMagic = 0x0000005C;
    ptrMP2Header->difficultLevel = 1;

    ptrMP2Header->widthMaps = sizeMap;
    ptrMP2Header->heightMaps = sizeMap;

    ptrMP2Header->mapsWidth = sizeMap;
    ptrMP2Header->mapsHeight = sizeMap;
    
    // read buffer CELLMAPS
    if(NULL == (ptrMaps = (CELLMAPS *) malloc(sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps))){
	fprintf(stderr, "malloc: error size %d\n", sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps);
        return EXIT;
    }

    // пишем все ячейки карты
    Uint8  *ptrTILData = GetTILData("GROUND32.TIL");

    int i;
    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

	if(NULL == (ptrMaps[i].info = (MP2TILEINFO *) malloc(sizeof(MP2TILEINFO)))){
	    fprintf(stderr, "malloc: error size %d\n", sizeof(MP2TILEINFO));
	    return EXIT;
	}
	
	memset(ptrMaps[i].info, 0, sizeof(MP2TILEINFO));
	
	ptrMaps[i].info->tileIndex = i % 4 + 0x0010;
	ptrMaps[i].info->indexName1 = 0xFF;
	ptrMaps[i].info->indexName2 = 0xFF;

        Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xF000;
        gmask = 0x0F00;
	bmask = 0x00F0;
	amask = 0x000F;
#else
        rmask = 0x000F;
        gmask = 0x00F0;
        bmask = 0x0F00;
        amask = 0xF000;
#endif
        if(NULL == (ptrMaps[i].tile = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, TILEWIDTH, TILEWIDTH, 16, rmask, gmask, bmask, 0))){
	    fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), TILEWIDTH, TILEWIDTH);
	    return EXIT;
	}
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMaps[i].info->tileIndex, ptrMaps[i].info->shape);
    }

    ACTION result = DrawEditDisplay();

    FreeMaps();

    return result;
}

void RedrawMapsArea(){

    // проверка границ экрана
    if(offsetX > ptrMP2Header->widthMaps - GetAreaWidth() || offsetY > ptrMP2Header->heightMaps - GetAreaHeight()) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest, src;

    CursorOff();

    Uint8 areaHeight = GetAreaHeight();
    Uint8 areaWidth = GetAreaWidth();

    // копируем видимую область area для сдвига при одинарном скроллинге
    src.x = BORDERWIDTH;
    src.y = BORDERWIDTH;
    src.w = GetAreaWidth() * TILEWIDTH;
    src.h = GetAreaHeight() * TILEWIDTH;
    SDL_BlitSurface(video, &src, backgroundArea, NULL);

    Uint8 i, j;

    // переместить влево на 1
    if((lastOffsetX - offsetX) == 1 && lastOffsetY == offsetY){

	dest.x = BORDERWIDTH + TILEWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w - TILEWIDTH;
	dest.h = backgroundArea->h;
	src.x = 0;
	src.y = 0;
	src.w = backgroundArea->w - TILEWIDTH;
	src.h = backgroundArea->h;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);
	for(j = 0; j < areaHeight; ++j)
	    DrawSingleTile(0, j);

    // переместить вправо на 1
    }else if((offsetX - lastOffsetX) == 1 && lastOffsetY == offsetY){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w - TILEWIDTH;
	dest.h = backgroundArea->h;
	src.x = TILEWIDTH;
	src.y = 0;
	src.w = backgroundArea->w - TILEWIDTH;
	src.h = backgroundArea->h;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);
	for(j = 0; j < areaHeight; ++j)
	    DrawSingleTile(areaWidth - 1, j);

    // переместить вниз на 1
    }else if((offsetY - lastOffsetY) == 1 && lastOffsetX == offsetX){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w;
	dest.h = backgroundArea->h - TILEWIDTH;
	src.x = 0;
	src.y = TILEWIDTH;
	src.w = backgroundArea->w;
	src.h = backgroundArea->h - TILEWIDTH;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);
	for(i = 0; i < areaWidth; ++i)
	    DrawSingleTile(i, areaHeight - 1);

    // переместить вверх на 1
    }else if((lastOffsetY - offsetY) == 1 && lastOffsetX == offsetX){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH + TILEWIDTH;
	dest.w = backgroundArea->w;
	dest.h = backgroundArea->h - TILEWIDTH;
	src.x = 0;
	src.y = 0;
	src.w = backgroundArea->w;
	src.h = backgroundArea->h - TILEWIDTH;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);
	for(i = 0; i < areaWidth; ++i)
	    DrawSingleTile(i, 0);

    // иначе перерисовываем все!
    }else
	for(j = 0; j < areaHeight; ++j)
	    for(i = 0; i < areaWidth; ++i)
		DrawSingleTile(i, j);

    SDL_Flip(video);
    
    // redraw radar
    RedrawRadarCursor();

    CursorOn();

    return;
}

/* функция реализующая алгоритм отрисовки одной клетки */
void DrawSingleTile(Uint8 x, Uint8 y){

    SDL_Rect dest;
    dest.x = BORDERWIDTH + x * TILEWIDTH;
    dest.y = BORDERWIDTH + y * TILEWIDTH;
    dest.w = TILEWIDTH;
    dest.h = TILEWIDTH;

    SDL_Surface *video = SDL_GetVideoSurface();
    Uint16 index = (offsetY + y) * ptrMP2Header->widthMaps + offsetX + x;
    Uint16 indexAddon;

    // объект 0 уровня TILES
    SDL_Surface *image = ptrMaps[index].tile;
    SDL_BlitSurface(image, NULL, video, &dest);

    Sint8 i = 0;

    for(i = 3; i >= 0; --i){
    
	// объект 1 уровня
	if(0xFF != ptrMaps[index].info->indexName1 && i == (ptrMaps[index].info->quantity1 % 4)) DrawMapObject(&dest, ptrMaps[index].info->objectName1, ptrMaps[index].info->indexName1);

	indexAddon = ptrMaps[index].info->indexAddon;

	// все объекты N1
	while(indexAddon){
	    if(0xFF != tailAddon[indexAddon].indexNameN1 && i == (tailAddon[indexAddon].quantityN % 4)) DrawMapObject(&dest, tailAddon[indexAddon].objectNameN1 * 2, tailAddon[indexAddon].indexNameN1);
	    indexAddon = tailAddon[indexAddon].indexAddon;
	}
    }

    for(i = 3; i >= 0; --i){

        // объект 2 уровня
	if(0xFF != ptrMaps[index].info->indexName2 && i == (ptrMaps[index].info->quantity1 % 4)) DrawMapObject(&dest, ptrMaps[index].info->objectName2, ptrMaps[index].info->indexName2);
    
	indexAddon = ptrMaps[index].info->indexAddon;

        // все объекты N2
	while(indexAddon){
	    if(0xFF != tailAddon[indexAddon].indexNameN2 && i == (tailAddon[indexAddon].quantityN % 4)) DrawMapObject(&dest, tailAddon[indexAddon].objectNameN2, tailAddon[indexAddon].indexNameN2);
	    indexAddon = tailAddon[indexAddon].indexAddon;
	}
    }

    // рисуем сетку
    if(GetIntValue("debug")){
	LockSurface(video);
	PutPixel(video, dest.x + dest.w - 1, dest.y + dest.h - 1, 0xFF00);
	UnlockSurface(video);
    }
}

ACTION ActionClickMapsArea(void){

    if(! GetIntValue("debug")) return NONE;

    int x, y;
    const char *string = "";

    SDL_GetMouseState(&x, &y);
    x -= BORDERWIDTH;
    y -= BORDERWIDTH;
    x /= TILEWIDTH;
    y /= TILEWIDTH;

    Uint16 index = (offsetY + y) * ptrMP2Header->widthMaps + offsetX + x;
    Uint32 count = 0;
    Uint16 indexAddon = 0;

    fprintf(stderr, "---------------------\n");
    fprintf(stderr, "tileIndex    : %6hX\n", ptrMaps[index].info->tileIndex);
    fprintf(stderr, "objectName1  : %6hhX\n", ptrMaps[index].info->objectName1);
    fprintf(stderr, "indexName1   : %6hhX\n", ptrMaps[index].info->indexName1);
    fprintf(stderr, "quantity1    : %6hhX\n", ptrMaps[index].info->quantity1);
    fprintf(stderr, "quantity2    : %6hhX\n", ptrMaps[index].info->quantity2);
    fprintf(stderr, "objectName2  : %6hhX\n", ptrMaps[index].info->objectName2);
    fprintf(stderr, "indexName2   : %6hhX\n", ptrMaps[index].info->indexName2);
    fprintf(stderr, "shape        : %6hhX\n", ptrMaps[index].info->shape);
    fprintf(stderr, "generalObject: %6hhX\t", ptrMaps[index].info->generalObject);

    fprintf(stderr, "%s", PrintObjectType(ptrMaps[index].info->generalObject, ptrMaps[index].info->indexName1));

    if(OBJ_MONSTER == ptrMaps[index].info->generalObject){

	count = ptrMaps[index].info->quantity2;
	count <<= 8;
	count |= ptrMaps[index].info->quantity1;
	count >>= 3;
	if(count) fprintf(stderr, "\tCOUNT: %d", count);
	else fprintf(stderr, "\tCOUNT: (RANDOM)");
    }

    fprintf(stderr, "\n");

    //fprintf(stderr, "indexAddon   : %6hX\n", ptrMaps[index].info->indexAddon);
    fprintf(stderr, "uniqNumber1  : %6lX\n", (long unsigned int) ptrMaps[index].info->uniqNumber1);
    fprintf(stderr, "uniqNumber2  : %6lX\n", (long unsigned int) ptrMaps[index].info->uniqNumber2);

    if(ptrMaps[index].info->indexAddon > countTail)

	fprintf(stderr, "Ooops...\n");

    else{
    
	indexAddon = ptrMaps[index].info->indexAddon;
	
	while(indexAddon){

	    fprintf(stderr, "---------------------\n");

	    //fprintf(stderr, "indexAddon   : %6hX\n", tailAddon[indexAddon].indexAddon);

	    fprintf(stderr, "objectNameN1 : %6hhX\n", tailAddon[indexAddon].objectNameN1);
	    fprintf(stderr, "indexNameN1  : %6hhX", tailAddon[indexAddon].indexNameN1);

	    if(0x3A == tailAddon[indexAddon].objectNameN1)
    
        	switch(tailAddon[indexAddon].indexNameN1){

            	    case OBJ_MINE_ORE:
            		string = "OBJ_MINE_ORE";
                	break;
                
            	    case OBJ_MINE_SULFUR:
                	string = "OBJ_MINE_SULFUR";
            		break;

            	    case OBJ_MINE_CRYSTAL:
                	string = "OBJ_MINE_CRYSTAL";
                	break;

            	    case OBJ_MINE_GEMS:
                	string = "OBJ_MINE_GEMS";
                	break;

            	    case OBJ_MINE_GOLD:
                	string = "OBJ_MINE_GOLD";
                	break;

            	    default:
                	string = "";
                	break;
                }

	    fprintf(stderr, "\t%s\n", string);

	    fprintf(stderr, "quantityN    : %6hhX\n", tailAddon[indexAddon].quantityN);
	    fprintf(stderr, "objectNameN2 : %6hhX\n", tailAddon[indexAddon].objectNameN2);
    	    fprintf(stderr, "indexNameN2  : %6hhX\n", tailAddon[indexAddon].indexNameN2);

	    fprintf(stderr, "uniqNumberN1 : %6lX\n", (long unsigned int) tailAddon[indexAddon].uniqNumberN1);
	    fprintf(stderr, "uniqNumberN2 : %6lX\n", (long unsigned int) tailAddon[indexAddon].uniqNumberN2);

	    indexAddon = tailAddon[indexAddon].indexAddon;
	}
    }

    fprintf(stderr, "---------------------\n\n\n");

    return NONE;
}

Uint8 GetAreaWidth(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    return (video->w - RADARWIDTH - 3 * BORDERWIDTH) / TILEWIDTH;
}

Uint8 GetAreaHeight(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    return (video->h - 2 * BORDERWIDTH) / TILEWIDTH;
}

ACTION ActionSelectSmallClear(void){

    fprintf(stderr, "ActionSelectSmallClear\n");
    return NONE;
}

ACTION ActionSelectMediumClear(void){

    fprintf(stderr, "ActionSelectMediumClear\n");
    return NONE;
}

ACTION ActionSelectLargeClear(void){

    fprintf(stderr, "ActionSelectLargeClear\n");
    return NONE;
}

ACTION ActionSelectManualClear(void){

    fprintf(stderr, "ActionSelectManualClear\n");
    return NONE;
}

ACTION ActionSelectSmallGround(void){

    fprintf(stderr, "ActionSelectSmallGround\n");
    return NONE;
}

ACTION ActionSelectMediumGround(void){

    fprintf(stderr, "ActionSelectMediumGround\n");
    return NONE;
}

ACTION ActionSelectLargeGround(void){

    fprintf(stderr, "ActionSelectLargeGround\n");
    return NONE;
}

ACTION ActionSelectManualGround(void){

    fprintf(stderr, "ActionSelectManualGround\n");
    return NONE;
}

ACTION ActionSelectOceanGround(void){

    fprintf(stderr, "ActionSelectOceanGround\n");
    return NONE;
}

ACTION ActionSelectGrassGround(void){

    fprintf(stderr, "ActionSelectGrassGround\n");
    return NONE;
}

ACTION ActionSelectSnowGround(void){

    fprintf(stderr, "ActionSelectSnowGround\n");
    return NONE;
}

ACTION ActionSelectSwampGround(void){

    fprintf(stderr, "ActionSelectSwampGround\n");
    return NONE;
}

ACTION ActionSelectLavaGround(void){

    fprintf(stderr, "ActionSelectLavaGround\n");
    return NONE;
}

ACTION ActionSelectDesertGround(void){

    fprintf(stderr, "ActionSelectDesertGround\n");
    return NONE;
}

ACTION ActionSelectDirtGround(void){

    fprintf(stderr, "ActionSelectDirtGround\n");
    return NONE;
}

ACTION ActionSelectWastelandGround(void){

    fprintf(stderr, "ActionSelectWastelandGround\n");
    return NONE;
}

ACTION ActionSelectBeachGround(void){

    fprintf(stderr, "ActionSelectBeachGround\n");
    return NONE;
}

ACTION ActionSelectOceanObject(void){

    fprintf(stderr, "ActionSelectOceanObject\n");
    return NONE;
}

ACTION ActionSelectGrassObject(void){

    fprintf(stderr, "ActionSelectGrassObject\n");
    return NONE;
}

ACTION ActionSelectSnowObject(void){

    fprintf(stderr, "ActionSelectSnowObject\n");
    return NONE;
}

ACTION ActionSelectSwampObject(void){

    fprintf(stderr, "ActionSelectSwampObject\n");
    return NONE;
}

ACTION ActionSelectLavaObject(void){

    fprintf(stderr, "ActionSelectLavaObject\n");
    return NONE;
}

ACTION ActionSelectDesertObject(void){

    fprintf(stderr, "ActionSelectDesertObject\n");
    return NONE;
}

ACTION ActionSelectDirtObject(void){

    fprintf(stderr, "ActionSelectDirtObject\n");
    return NONE;
}

ACTION ActionSelectWastelandObject(void){

    fprintf(stderr, "ActionSelectWastelandObject\n");
    return NONE;
}

ACTION ActionSelectBeachObject(void){

    fprintf(stderr, "ActionSelectBeachObject\n");
    return NONE;
}

ACTION ActionSelectTownObject(void){

    fprintf(stderr, "ActionSelectTownObject\n");
    return NONE;
}

ACTION ActionSelectMonsterObject(void){

    fprintf(stderr, "ActionSelectMonsterObject\n");
    return NONE;
}

ACTION ActionSelectHeroesObject(void){

    fprintf(stderr, "ActionSelectHeroesObject\n");
    return NONE;
}

ACTION ActionSelectArtifactObject(void){

    fprintf(stderr, "ActionSelectArtifactObject\n");
    return NONE;
}

ACTION ActionSelectResourceObject(void){

    fprintf(stderr, "ActionSelectResourceObject\n");
    return NONE;
}

ACTION PreferencePressNewMap(void){

    fprintf(stderr, "PreferencePressNewMap\n");
    return NONE;
}


ACTION PreferencePressLoadMap(void){

    fprintf(stderr, "PreferencePressLoadMap\n");
    return NONE;
}


ACTION PreferencePressSaveMap(void){

    fprintf(stderr, "PreferencePressSaveMap\n");
    return NONE;
}


ACTION PreferencePressQuit(void){

    return EXIT;
}

ACTION PreferencePressCancel(void){

    return CANCEL;
}

ACTION OptionsClickAnimation(void){

    fprintf(stderr, "OptionsClickAnimation\n");
    return NONE;
}

ACTION OptionsClickCycling(void){

    fprintf(stderr, "OptionsClickCycling\n");
    return NONE;
}

ACTION OptionsClickGrid(void){

    fprintf(stderr, "OptionsClickGrid\n");
    return NONE;
}

ACTION OptionsClickCursor(void){

    fprintf(stderr, "OptionsClickCursor\n");
    return NONE;
}

ACTION OptionsClickOkay(void){

    return OK;
}
