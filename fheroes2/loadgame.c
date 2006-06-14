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
#include "radar.h"
#include "loadgame.h"
#include "mp2maps.h"

void	ShowStaticMainDisplay(void);
void	RedrawMapsArea(void);
void	DrawRectAreaMaps(SDL_Rect *);
void	DrawCellAreaMapsMonster(Uint8, Uint8);
void	DrawCellAreaMapsLevel1(Uint8, Uint8);
void	DrawCellAreaMapsLevel2(Uint8, Uint8);

ACTION ActionClickMapsArea(void);
ACTION ActionClickRadarArea(void);

ACTION ActionScrollRight(void);
ACTION ActionScrollLeft(void);
ACTION ActionScrollTop(void);
ACTION ActionScrollBottom(void);

ACTION ActionScrollHeroesUp(void);
ACTION ActionScrollHeroesDown(void);
ACTION ActionScrollCastleUp(void);
ACTION ActionScrollCastleDown(void);

ACTION ActionButtonHeroes(void);
ACTION ActionButtonAction(void);
ACTION ActionButtonCastle(void);
ACTION ActionButtonMagic(void);
ACTION ActionButtonTur(void);
ACTION ActionButtonInfo(void);
ACTION ActionButtonOptions(void);
ACTION ActionButtonSettings(void);

ACTION ActionPressPreferences(void);
ACTION ActionPressOptions(void);

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

INTERFACEACTION *stpemaindisplay = NULL;

SDL_Surface	*backgroundArea = NULL;
SDL_Surface	*frameAreaLeft = NULL;
SDL_Surface	*frameAreaBottom = NULL;

S_DISPLAY       display;

ACTION DrawMainDisplay(){

    SDL_Rect dest;
    SDL_Surface *image;
    SDL_Surface *video;

    INTERFACEACTION action;
    AGGSPRITE sprite;
    stpemaindisplay = NULL;
    
    char *icnname;
    Uint16 dy = 0;

    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue("fullscreen")) flag = flag | SDL_FULLSCREEN;

    // select video mode

    switch(GetIntValue("videomode")){

	default:
	case 0:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 640 || video->h != 480)
		video = SDL_SetVideoMode(640, 480, 16, flag);

	    dy = 320;
	    break;

	case 1:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 800 || video->h != 576)
		video = SDL_SetVideoMode(800, 576, 16, flag);
	    
	    dy = 416;
	    break;
	
	case 2:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 1024 || video->h != 768)
		video = SDL_SetVideoMode(1024, 768, 16, flag);

	    dy = 448;
	    break;
	
	case 3:
	    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 1280 || video->h != 1024)
		video = SDL_SetVideoMode(1280, 1024, 16, flag);

	    dy = 448;
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

    display.offsetX = 0;
    display.offsetY = 0;
    display.lastOffsetX = 0;
    display.lastOffsetY = 0;

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
    AddActionEvent(&stpemaindisplay, &action);

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
    AddActionEvent(&stpemaindisplay, &action);

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
    action.pf = ActionScrollLeft;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

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
    action.pf = ActionScrollRight;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

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
    action.pf = ActionScrollTop;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

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
    action.pf = ActionScrollBottom;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

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
    AddActionEvent(&stpemaindisplay, &action);

    // button
    if(GetIntValue("evilinterface"))
	icnname = "ADVEBTNS.ICN";
    else
	icnname = "ADVBTNS.ICN";
    // next heroes
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH;
    dest.y = dy;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 0);
    FillSPRITE(&action.objectPush, icnname, 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonHeroes;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // action
    FillSPRITE(&sprite, icnname, 2);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 2);
    FillSPRITE(&action.objectPush, icnname, 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonAction;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // castle
    FillSPRITE(&sprite, icnname, 4);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 4);
    FillSPRITE(&action.objectPush, icnname, 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonCastle;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);
    
    // magic
    FillSPRITE(&sprite, icnname, 6);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 6);
    FillSPRITE(&action.objectPush, icnname, 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonMagic;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);
    
    // end tur
    FillSPRITE(&sprite, icnname, 8);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH;
    dest.y = dy + image->h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 8);
    FillSPRITE(&action.objectPush, icnname, 9);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonTur;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // info
    FillSPRITE(&sprite, icnname, 10);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy + image->h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 10);
    FillSPRITE(&action.objectPush, icnname, 11);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonInfo;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // options
    FillSPRITE(&sprite, icnname, 12);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy + image->h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 12);
    FillSPRITE(&action.objectPush, icnname, 13);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonOptions;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // settings
    FillSPRITE(&sprite, icnname, 14);
    image = GetICNSprite(&sprite);
    dest.x += image->w;
    dest.y = dy + image->h;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 14);
    FillSPRITE(&action.objectPush, icnname, 15);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionButtonSettings;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // scroll element
    if(GetIntValue("evilinterface"))
	icnname = "SCROLLE.ICN";
    else
	icnname = "SCROLL.ICN";

    // scroll heroes up
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 57;
    dest.y = RADARWIDTH + 2 * BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 0);
    FillSPRITE(&action.objectPush, icnname, 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollHeroesUp;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // scroll castle up
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 115 + image->w;
    dest.y = RADARWIDTH + 2 * BORDERWIDTH;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 0);
    FillSPRITE(&action.objectPush, icnname, 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollCastleUp;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    switch(GetIntValue("videomode")){

	// 640x480
	default:
	case 0:
	    dest.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 4 - image->h;
	    break;

	// 800x600
	case 1:
	    dest.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 7 - image->h;
	    break;

	// 1024x760
	case 2:
	// 1280x1024
	case 3:
	    dest.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * 8 - image->h;
	    break;
    }
    // scroll heroes down
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 57;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 0);
    FillSPRITE(&action.objectPush, icnname, 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollHeroesDown;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);
    // scroll castle Down
    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 115 + image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 0);
    FillSPRITE(&action.objectPush, icnname, 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollCastleDown;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);






    // инициализируем радар
    InitRadar();

    // отображаем картинку
    ShowStaticMainDisplay();
    RedrawMapsArea();
    
    // идем в цикл сообщений
    ACTION result;

    while(! (EXIT == (result = ActionCycle(stpemaindisplay)) || (ESC == result && YES == MessageBox("Are you sure you want to\n\t\t\t quit?", FONT_BIG))) );

    result = EXIT;

    // овобождаем данные
    FreeActionEvent(stpemaindisplay);
    FreeRadar();

    if(backgroundArea) SDL_FreeSurface(backgroundArea);
    if(frameAreaLeft) SDL_FreeSurface(frameAreaLeft);
    if(frameAreaBottom) SDL_FreeSurface(frameAreaBottom);
    
    return result;
}

void ShowStaticMainDisplay(void){

    SDL_Surface *video = NULL;
    SDL_Surface *image = NULL;
    SDL_Surface *formatSurface = NULL;
    SDL_Rect dst, src;

    AGGSPRITE sprite;
        
    video = SDL_GetVideoSurface();
    char * icnname = NULL;
    Uint8 i;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    SDL_FillRect(video, NULL, SDL_MapRGB(video->format, 0x00, 0x00, 0x00));

    // show border
    if(GetIntValue("evilinterface"))
	icnname = "ADVBORDE.ICN";
    else
	icnname = "ADVBORD.ICN";

    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);

    // ниже, собирается интерфейс из кусочков под видеорежим
    switch(GetIntValue("videomode")){

	// 640x480 draw all
	case 0:
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "LOCATORE.ICN";
	    else
		icnname = "LOCATORS.ICN";
	    FillSPRITE(&sprite, icnname, 1);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 5;	// background panel heroes
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 4; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 4; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "STONBAKE.ICN";
	    else
		icnname = "STONBACK.ICN";
	    FillSPRITE(&sprite, icnname, 0);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    dst.y = 392;
	    SDL_BlitSurface(image, &src, video, &dst);
	    break;

	// 800x600
	case 1:
	    // TOP BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = 196;
	    src.h = BORDERWIDTH;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 80;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // BOTTOM BORDER
	    src.x = 0;
	    src.y = image->h - BORDERWIDTH; 
	    src.w = 196;
	    src.h = BORDERWIDTH;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 80;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // LEFT BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 176;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // RIGHT BORDER
	    src.x = image->w - BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 176;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // MIDDLE BORDER
	    src.x = image->w - RADARWIDTH - 2 * BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 176;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL
	    src.x = image->w - RADARWIDTH - BORDERWIDTH;
	    src.y = RADARWIDTH + BORDERWIDTH;
	    src.w = RADARWIDTH;
	    src.h = 64;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.y += 32;
	    src.h = 64;
	    dst.h = src.h;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "LOCATORE.ICN";
	    else
		icnname = "LOCATORS.ICN";
	    FillSPRITE(&sprite, icnname, 1);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 5;	// background panel heroes
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 7; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 7; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "STONBAKE.ICN";
	    else
		icnname = "STONBACK.ICN";
	    FillSPRITE(&sprite, icnname, 0);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    dst.y = 488;
	    SDL_BlitSurface(image, &src, video, &dst);
	    break;

	// 1024x768
	case 2:
	    // TOP BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = 140;
	    src.h = BORDERWIDTH;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 192;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // BOTTOM BORDER
	    src.x = 0;
	    src.y = image->h - BORDERWIDTH;
	    src.w = 140;
	    src.h = BORDERWIDTH;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 192;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // LEFT BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 166;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 144;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // RIGHT BORDER
	    src.x = image->w - BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 166;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 144;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // MIDDLE BORDER
	    src.x = image->w - RADARWIDTH - 2 * BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 166;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 144;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL
	    src.x = image->w - RADARWIDTH - BORDERWIDTH;
	    src.y = RADARWIDTH + BORDERWIDTH;
	    src.w = RADARWIDTH;
	    src.h = 64;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.y += 32;
	    src.h = 64;
	    dst.h = src.h;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "LOCATORE.ICN";
	    else
		icnname = "LOCATORS.ICN";
	    FillSPRITE(&sprite, icnname, 1);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 5;	// background panel heroes
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "STONBAKE.ICN";
	    else
		icnname = "STONBACK.ICN";
	    FillSPRITE(&sprite, icnname, 0);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    dst.y = 520;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    break;

	case 3:
	    // TOP BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = 128;
	    src.h = BORDERWIDTH;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 214;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w - 1;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // BOTTOM BORDER
	    src.x = 0;
	    src.y = image->h - BORDERWIDTH;
	    src.w = 140;
	    src.h = BORDERWIDTH;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.x += src.w;
	    src.w = 214;
	    dst = src;
	    dst.y = video->h - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.x += src.w - 1;
	    src.w = image->w - src.x;
	    dst.w = src.w;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // LEFT BORDER
	    src.x = 0;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 160;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 181;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // RIGHT BORDER
	    src.x = image->w - BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 160;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 181;
	    dst = src;
	    dst.x = video->w - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // MIDDLE BORDER
	    src.x = image->w - RADARWIDTH - 2 * BORDERWIDTH;
	    src.y = 0; 
	    src.w = BORDERWIDTH;
	    src.h = 160;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 181;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - 2 * BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.h = image->h - src.y;
	    dst.h = src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL
	    src.x = image->w - RADARWIDTH - BORDERWIDTH;
	    src.y = RADARWIDTH + BORDERWIDTH;
	    src.w = RADARWIDTH;
	    src.h = 64;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    src.y += src.h;
	    src.h = 32;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += src.h;
	    src.y += 32;
	    src.h = 64;
	    dst.h = src.h;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "LOCATORE.ICN";
	    else
		icnname = "LOCATORS.ICN";
	    FillSPRITE(&sprite, icnname, 1);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 5;	// background panel heroes
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }

	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue("evilinterface"))
		icnname = "STONBAKE.ICN";
	    else
		icnname = "STONBACK.ICN";
	    FillSPRITE(&sprite, icnname, 0);
	    image = GetICNSprite(&sprite);
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH;
	    dst.y = 520;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);
	    dst.y += image->h;
	    SDL_BlitSurface(image, &src, video, &dst);

	    break;
	
	default:
	    break;
    }

    // сохраняем рамку (левую) для перерисовки поверх всех
    if(NULL == frameAreaLeft){
	src.x = BORDERWIDTH + GetAreaWidth() * TILEWIDTH;
	src.y = BORDERWIDTH;
	src.w = BORDERWIDTH;
	src.h = GetAreaWidth() * TILEWIDTH;

	if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, src.w, src.h, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "RedrawMapsArea: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), src.w, src.h);
	    return;
	}

	SDL_BlitSurface(video, &src, formatSurface, NULL);
	
	frameAreaLeft = SDL_DisplayFormat(formatSurface);
	SDL_FreeSurface(formatSurface);
    }

    // сохраняем рамку (нижнюю) для перерисовки поверх всех
    if(NULL == frameAreaBottom){
	src.x = BORDERWIDTH;
	src.y = BORDERWIDTH + GetAreaHeight() * TILEWIDTH;
	src.w = GetAreaHeight() * TILEWIDTH;
	src.h = BORDERWIDTH;

	if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, src.w, src.h, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "RedrawMapsArea: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), src.w, src.h);
	    return;
	}

	SDL_BlitSurface(video, &src, formatSurface, NULL);

	frameAreaBottom = SDL_DisplayFormat(formatSurface);
	SDL_FreeSurface(formatSurface);
    }

    // динамические элементы
    INTERFACEACTION *ptr = stpemaindisplay;
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

ACTION ActionScrollTop(void){

    if(display.offsetY){
	display.lastOffsetX = display.offsetX;
	display.lastOffsetY = display.offsetY;
	--display.offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollBottom(void){

    if(display.offsetY < GetHeightMaps() - GetAreaHeight()){
	display.lastOffsetX = display.offsetX;
	display.lastOffsetY = display.offsetY;
	++display.offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollRight(void){

    if(display.offsetX < GetWidthMaps() - GetAreaWidth()){
	display.lastOffsetY = display.offsetY;
	display.lastOffsetX = display.offsetX;
	++display.offsetX;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollLeft(void){

    if(display.offsetX){
	display.lastOffsetY = display.offsetY;
	display.lastOffsetX = display.offsetX;
	--display.offsetX;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionClickRadarArea(void){

    int x, y;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_GetMouseState(&x, &y);
    
    // верхний правый угол отображения
    display.lastOffsetX = display.offsetX;
    display.lastOffsetY = display.offsetY;
    display.offsetX = (x - (video->w - BORDERWIDTH - RADARWIDTH)) * GetWidthMaps() / RADARWIDTH;
    display.offsetY = (y - BORDERWIDTH) * GetHeightMaps() / RADARWIDTH;
    
    // центрируем
    if(0 > display.offsetX - GetAreaWidth() / 2)
	display.offsetX = 0;
    else
	display.offsetX = display.offsetX - GetAreaWidth() / 2;
    

    if(0 > display.offsetY - GetAreaHeight() / 2)
	display.offsetY = 0;
    else
	display.offsetY = display.offsetY - GetAreaHeight() / 2;
    

    // проверка за границы
    if(display.offsetY > GetHeightMaps() - GetAreaHeight())
	display.offsetY = GetHeightMaps() - GetAreaHeight();
    

    if(display.offsetX > GetWidthMaps() - GetAreaWidth())
	display.offsetX = GetWidthMaps() - GetAreaWidth();
    
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

void RedrawMapsArea(){

    // проверка границ экрана
    if(display.offsetX > GetWidthMaps() - GetAreaWidth() || display.offsetY > GetHeightMaps() - GetAreaHeight()) return;

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

    // переместить влево на 1
    if((display.lastOffsetX - display.offsetX) == 1 && display.lastOffsetY == display.offsetY){

	dest.x = BORDERWIDTH + TILEWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w - TILEWIDTH;
	dest.h = backgroundArea->h;
	src.x = 0;
	src.y = 0;
	src.w = backgroundArea->w - TILEWIDTH;
	src.h = backgroundArea->h;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);

	dest.x = 0;
	dest.y = 0;
	dest.w = 1;
	dest.h = areaHeight;

    // переместить вправо на 1
    }else if((display.offsetX - display.lastOffsetX) == 1 && display.lastOffsetY == display.offsetY){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w - TILEWIDTH;
	dest.h = backgroundArea->h;
	src.x = TILEWIDTH;
	src.y = 0;
	src.w = backgroundArea->w - TILEWIDTH;
	src.h = backgroundArea->h;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);

	dest.x = areaWidth - 1;
	dest.y = 0;
	dest.w = 1;
	dest.h = areaHeight;

    // переместить вниз на 1
    }else if((display.offsetY - display.lastOffsetY) == 1 && display.lastOffsetX == display.offsetX){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = backgroundArea->w;
	dest.h = backgroundArea->h - TILEWIDTH;
	src.x = 0;
	src.y = TILEWIDTH;
	src.w = backgroundArea->w;
	src.h = backgroundArea->h - TILEWIDTH;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);

	dest.x = 0;
	dest.y = areaHeight - 1;
	dest.w = areaWidth;
	dest.h = 1;

    // переместить вверх на 1
    }else if((display.lastOffsetY - display.offsetY) == 1 && display.lastOffsetX == display.offsetX){

	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH + TILEWIDTH;
	dest.w = backgroundArea->w;
	dest.h = backgroundArea->h - TILEWIDTH;
	src.x = 0;
	src.y = 0;
	src.w = backgroundArea->w;
	src.h = backgroundArea->h - TILEWIDTH;
	SDL_BlitSurface(backgroundArea, &src, video, &dest);

	dest.x = 0;
	dest.y = 0;
	dest.w = areaWidth;
	dest.h = 1;

    // иначе перерисовываем все!
    }else{

	dest.x = 0;
	dest.y = 0;
	dest.w = areaWidth;
	dest.h = areaHeight;
    }

    DrawRectAreaMaps(&dest);

    SDL_Flip(video);
    
    // redraw radar
    RedrawRadarCursor();

    CursorOn();

    return;
}

void DrawRectAreaMaps(SDL_Rect *rect){

    Uint8 x, y;

    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    Uint16 index = 0;
    SDL_Rect dest;

    CELLMAPS *ptrCell = NULL;
    // циклы разделены bug (объекты больше чем tile, следующий tile зарисовывает объект)

    // сначала отрисовываем все tile
    for(y = rect->y; y < rect->y + rect->h; ++y)

	for(x = rect->x; x < rect->x + rect->w; ++x){

	    dest.x = BORDERWIDTH + x * TILEWIDTH;
	    dest.y = BORDERWIDTH + y * TILEWIDTH;
	    dest.w = TILEWIDTH;
	    dest.h = TILEWIDTH;

	    index = (display.offsetY + y) * GetWidthMaps() + display.offsetX + x;
	    ptrCell = GetCELLMAPS(index);
	    image = ptrCell->tile;
	    SDL_BlitSurface(image, NULL, video, &dest);
	    
    }

    // отрисовываем все нижние объекты
    for(y = rect->y; y < rect->y + rect->h; ++y)

	for(x = rect->x; x < rect->x + rect->w; ++x)
	
	    DrawCellAreaMapsLevel1(x, y);

    // отрисовываем всех монстров
    for(y = rect->y; y < rect->y + rect->h; ++y)

	for(x = rect->x; x < rect->x + rect->w; ++x)
	
	    DrawCellAreaMapsMonster(x, y);

    // отрисовываем все верхние объекты
    for(y = rect->y; y < rect->y + rect->h; ++y)

	for(x = rect->x; x < rect->x + rect->w; ++x){

	    DrawCellAreaMapsLevel2(x, y);

	    // и рисуем сетку
	    if(GetIntValue("debug")){
		LockSurface(video);
		PutPixel(video, dest.x + dest.w - 1, dest.y + dest.h - 1, 0xFF00);
		UnlockSurface(video);
	    }
	}

    // востановим нестандартные спрайты при скроллинге вправо и вниз
    if(rect->x == (GetAreaWidth() - 1)){

	// отрисовываем всех монстров
	for(y = rect->y; y < rect->y + rect->h; ++y)

		DrawCellAreaMapsMonster(rect->x - 1, y);

	// отрисовываем все верхние объекты
	for(y = rect->y; y < rect->y + rect->h; ++y){

		DrawCellAreaMapsLevel2(rect->x - 1, y);
		DrawCellAreaMapsLevel2(rect->x, y);
	}

    }else if(rect->y == (GetAreaHeight() - 1)){

	// отрисовываем всех монстров
	for(x = rect->x; x < rect->x + rect->w; ++x)

		DrawCellAreaMapsMonster(x, rect->y - 1);

	// отрисовываем все верхние объекты
	for(y = rect->y; y < rect->y + rect->h; ++y){

		DrawCellAreaMapsLevel2(y, rect->y - 1);
		DrawCellAreaMapsLevel2(y, rect->y);
	}
    }

    // отрисовываем рамку area
    dest.x = BORDERWIDTH + GetAreaWidth() * TILEWIDTH;
    dest.y = BORDERWIDTH;
    dest.w = BORDERWIDTH;
    dest.h = GetAreaHeight() * TILEWIDTH;
    SDL_BlitSurface(frameAreaLeft, NULL, video, &dest);
    dest.x = BORDERWIDTH;
    dest.y = BORDERWIDTH + GetAreaHeight() * TILEWIDTH;
    dest.w = GetAreaWidth() * TILEWIDTH;
    dest.h = BORDERWIDTH;
    SDL_BlitSurface(frameAreaBottom, NULL, video, &dest);
}

/* функция реализующая алгоритм отрисовки нижних объектов одной клетки */
void DrawCellAreaMapsLevel1(Uint8 x, Uint8 y){

    SDL_Rect dest;
    dest.x = BORDERWIDTH + x * TILEWIDTH;
    dest.y = BORDERWIDTH + y * TILEWIDTH;
    dest.w = TILEWIDTH;
    dest.h = TILEWIDTH;

    Uint16 index = (display.offsetY + y) * GetWidthMaps() + display.offsetX + x;
    Uint16 indexAddon = 0;

    CELLMAPS *ptrCell = GetCELLMAPS(index);
    MP2ADDONTAIL *ptrAddon = NULL;
    Sint8 i = 0;

    for(i = 3; i >= 0; --i){
    
	// объект 1 уровня
	if(0xFF != ptrCell->info->indexName1 && i == (ptrCell->info->quantity1 % 4)) DrawMapObject(&dest, ptrCell->info->objectName1, ptrCell->info->indexName1);

	indexAddon = ptrCell->info->indexAddon;

	// все объекты N1
	while(indexAddon){
	    ptrAddon = GetADDONTAIL(indexAddon);
	    if(0xFF != ptrAddon->indexNameN1 && i == (ptrAddon->quantityN % 4)) DrawMapObject(&dest, ptrAddon->objectNameN1 * 2, ptrAddon->indexNameN1);
	    indexAddon = ptrAddon->indexAddon;
	}
    }
}

/* функция реализующая алгоритм отрисовки монстров одной клетки */
void DrawCellAreaMapsMonster(Uint8 x, Uint8 y){

    SDL_Rect dest;
    dest.x = BORDERWIDTH + x * TILEWIDTH;
    dest.y = BORDERWIDTH + y * TILEWIDTH;
    dest.w = TILEWIDTH;
    dest.h = TILEWIDTH;

    Uint16 index = (display.offsetY + y) * GetWidthMaps() + display.offsetX + x;
    Uint16 indexAddon = 0;

    CELLMAPS *ptrCell = GetCELLMAPS(index);
    MP2ADDONTAIL *ptrAddon = NULL;
    Sint8 i = 0;

    for(i = 3; i >= 0; --i){
    
	if(0x98 == ptrCell->info->generalObject){

	    // объект 1 уровня
	    if(0xFF != ptrCell->info->indexName1 && i == (ptrCell->info->quantity1 % 4)) DrawMapObject(&dest, ptrCell->info->objectName1, ptrCell->info->indexName1);

	    indexAddon = ptrCell->info->indexAddon;

	    // все объекты N1
	    while(indexAddon){
		ptrAddon = GetADDONTAIL(indexAddon);
		if(0xFF != ptrAddon->indexNameN1 && i == (ptrAddon->quantityN % 4)) DrawMapObject(&dest, ptrAddon->objectNameN1 * 2, ptrAddon->indexNameN1);
		indexAddon = ptrAddon->indexAddon;
	    }
	}
    }
}

/* функция реализующая алгоритм отрисовки верхних объектов одной клетки */
void DrawCellAreaMapsLevel2(Uint8 x, Uint8 y){

    SDL_Rect dest;
    dest.x = BORDERWIDTH + x * TILEWIDTH;
    dest.y = BORDERWIDTH + y * TILEWIDTH;
    dest.w = TILEWIDTH;
    dest.h = TILEWIDTH;

    Uint16 index = (display.offsetY + y) * GetWidthMaps() + display.offsetX + x;
    Uint16 indexAddon = 0;

    CELLMAPS *ptrCell = GetCELLMAPS(index);
    MP2ADDONTAIL *ptrAddon = NULL;
    Sint8 i = 0;

    for(i = 3; i >= 0; --i){

        // объект 2 уровня
	if(0xFF != ptrCell->info->indexName2 && i == (ptrCell->info->quantity1 % 4)) DrawMapObject(&dest, ptrCell->info->objectName2, ptrCell->info->indexName2);

	indexAddon = ptrCell->info->indexAddon;

        // все объекты N2
	while(indexAddon){
	    ptrAddon = GetADDONTAIL(indexAddon);
	    if(0xFF != ptrAddon->indexNameN2 && i == (ptrAddon->quantityN % 4)) DrawMapObject(&dest, ptrAddon->objectNameN2, ptrAddon->indexNameN2);
	    indexAddon = ptrAddon->indexAddon;
	}
    }
}

ACTION ActionClickMapsArea(void){

    int x, y;

    SDL_GetMouseState(&x, &y);
    
    // выравниваем по началу ячейки
    x -= BORDERWIDTH;
    y -= BORDERWIDTH;
    x /= TILEWIDTH;
    y /= TILEWIDTH;

    Uint16 index = (display.offsetY + y) * GetWidthMaps() + display.offsetX + x;
    
    PrintCellInfo(index);
    
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


ACTION ActionButtonHeroes(void){

    return NONE;
}

ACTION ActionButtonAction(void){

    return NONE;
}

ACTION ActionButtonCastle(void){

    return NONE;
}

ACTION ActionButtonMagic(void){

    return NONE;
}

ACTION ActionButtonTur(void){

    return NONE;
}

ACTION ActionButtonInfo(void){

    return NONE;
}

ACTION ActionButtonOptions(void){

    return NONE;
}

ACTION ActionButtonSettings(void){

    return NONE;
}

ACTION ActionScrollHeroesUp(void){

    return NONE;
}

ACTION ActionScrollHeroesDown(void){

    return NONE;
}

ACTION ActionScrollCastleUp(void){

    return NONE;
}

ACTION ActionScrollCastleDown(void){

    return NONE;
}

S_DISPLAY *GetDisplayPos(void){

    return &display;
}
