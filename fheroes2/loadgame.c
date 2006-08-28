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
#include "debug.h"
#include "draw.h"
#include "radar.h"
#include "kingdom.h"
#include "loadgame.h"
#include "mp2maps.h"

#define  MONSTERFIXTURE 26;

void	ShowStaticMainDisplay(void);
void	RedrawMapsArea(void);
void	DrawRectAreaMaps(SDL_Rect *);
void	DrawCellAreaMapsFlags(Uint8, Uint8);
void	DrawCellAreaMapsMonster(Uint8, Uint8);
void	DrawCellAreaMapsTile(Uint8, Uint8);
void	DrawCellAreaMapsLevel1(Uint8, Uint8);
void	DrawCellAreaMapsLevel2(Uint8, Uint8);
void	DrawCellStaticAnimation(Uint8, Uint8);
void	RedrawMapsAnimation(void);
void	CheckCursorAreaAction(E_OBJECT);
ACTION	ClickCursorAreaAction(E_OBJECT);

void	InitCursorFocus(void);
void	RedrawFocusPanel(S_FOCUS *);
void	FreeCursorFocus(void);
void	SetGameFocus(void *, E_OBJECT);

void   ComputerStep(E_COLORS);
ACTION ActionGAMELOOP(void);
ACTION ActionHUMANLOOP(INTERFACEACTION *);

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
ACTION ActionClickFocusHeroes(void);
ACTION ActionClickFocusCastle(void);

ACTION ActionButtonHeroes(void);
ACTION ActionButtonAction(void);
ACTION ActionButtonCastle(void);
ACTION ActionButtonMagic(void);
ACTION ActionButtonCloseDay(void);
ACTION ActionButtonInfo(void);
ACTION ActionButtonMenu(void);
ACTION ActionButtonSettings(void);

ACTION InfoClickWorld(void);
ACTION InfoClickPuzzle(void);
ACTION InfoClickInfo(void);
ACTION InfoClickDig(void);
ACTION InfoClickCancel(void);

ACTION MenuClickNew(void);
ACTION MenuClickLoad(void);
ACTION MenuClickSave(void);
ACTION MenuClickQuit(void);
ACTION MenuClickCancel(void);

ACTION SettingsClickMusic(void);
ACTION SettingsClickSound(void);
ACTION SettingsClickType(void);
ACTION SettingsClickHeroesSpeed(void);
ACTION SettingsClickEnemySpeed(void);
ACTION SettingsClickPath(void);
ACTION SettingsClickInterface(void);
ACTION SettingsClickVideo(void);
ACTION SettingsClickCursor(void);
ACTION SettingsClickOkay(void);

INTERFACEACTION *stpemaindisplay = NULL;

SDL_Surface	*backgroundArea = NULL;

S_DISPLAY       display;

S_FOCUS		gameFocus;

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
    if(TRUE == GetIntValue(FULLSCREEN)) flag = flag | SDL_FULLSCREEN;

    // select video mode

    switch(GetIntValue(VIDEOMODE)){

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

    if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, GetWidthArea() * TILEWIDTH, GetHeightArea() * TILEWIDTH, 16, rmask, gmask, bmask, 0))){
	fprintf(stderr, "DrawMainDisplay: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), GetWidthArea() * TILEWIDTH, GetHeightArea() * TILEWIDTH);
	return EXIT;
    }

    backgroundArea = SDL_DisplayFormat(formatSurface);
    SDL_FreeSurface(formatSurface);

    // click по области карты
    if(GetIntValue(DEBUG)){
	dest.x = BORDERWIDTH;
	dest.y = BORDERWIDTH;
	dest.w = GetWidthArea() * TILEWIDTH;
	dest.h = GetHeightArea() * TILEWIDTH;
	// обнуляем
	ZeroINTERFACEACTION(&action);
	// заполняем
	action.rect = dest;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickMapsArea;
	// регистрируем
	AddActionEvent(&stpemaindisplay, &action);
    }

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
    action.pf = ActionScrollRight;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // scroll mouse верхний
    dest.x = BORDERWIDTH;
    dest.y = 0;
    dest.w = (GetWidthArea() - 1) * TILEWIDTH;
    dest.h = BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionScrollTop;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // scroll mouse нижний
    dest.x = 2 * BORDERWIDTH;
    dest.y = video->h - BORDERWIDTH;
    dest.w = (GetWidthArea() - 1) * TILEWIDTH;
    dest.h = BORDERWIDTH;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_PRESENT;
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
    if(GetIntValue(EVILINTERFACE))
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
    action.pf = ActionButtonCloseDay;
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
    action.pf = ActionButtonMenu;
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
    if(GetIntValue(EVILINTERFACE))
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

    switch(GetIntValue(VIDEOMODE)){

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
    FillSPRITE(&sprite, icnname, 2);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 57;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 2);
    FillSPRITE(&action.objectPush, icnname, 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollHeroesDown;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // scroll castle down
    FillSPRITE(&sprite, icnname, 2);
    image = GetICNSprite(&sprite);
    dest.x = video->w - RADARWIDTH - BORDERWIDTH + 115 + image->w;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, icnname, 2);
    FillSPRITE(&action.objectPush, icnname, 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionScrollCastleDown;
    // регистрируем
    AddActionEvent(&stpemaindisplay, &action);

    // инициализируем радар
    InitRadar();
    InitCursorFocus();
    
    S_CASTLE *castle = GetFirstCastle(GetIntValue(HUMANCOLORS));

    if(castle)
	SetGameFocus(castle, OBJ_CASTLE);
    else
	// alter focus heroes
	return EXIT;


    display.lastOffsetX = 0xFF;
    display.lastOffsetY = 0xFF;

    // отображаем картинку
    ShowStaticMainDisplay();
    RedrawMapsArea();
    RedrawFocusPanel(&gameFocus);

    // идем в игровой цикл
    ACTION result = ActionGAMELOOP();

    // овобождаем данные
    FreeActionEvent(stpemaindisplay);
    FreeRadar();
    FreeCursorFocus();

    if(backgroundArea) SDL_FreeSurface(backgroundArea);

    return result;
}

void ShowStaticMainDisplay(void){

    SDL_Surface *video = NULL;
    SDL_Surface *image = NULL;
    SDL_Rect dst, src;

    AGGSPRITE sprite;
    INTERFACEACTION action;
        
    video = SDL_GetVideoSurface();
    char * icnname = NULL;
    Uint8 i;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    SDL_FillRect(video, NULL, SDL_MapRGB(video->format, 0x00, 0x00, 0x00));

    // show border
    if(GetIntValue(EVILINTERFACE))
	icnname = "ADVBORDE.ICN";
    else
	icnname = "ADVBORD.ICN";

    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);

    // ниже, собирается интерфейс из кусочков под видеорежим
    switch(GetIntValue(VIDEOMODE)){

	// 640x480 draw all
	case 0:
	    src.x = 0;
	    src.y = 0;
	    src.w = image->w;
	    src.h = image->h;
	    dst = src;
	    SDL_BlitSurface(image, &src, video, &dst);
	    // TOP PANEL BACKGROUND ELEMENT
	    if(GetIntValue(EVILINTERFACE))
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
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusHeroes;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 4; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusCastle;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue(EVILINTERFACE))
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
	    if(GetIntValue(EVILINTERFACE))
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
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusHeroes;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 7; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusCastle;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue(EVILINTERFACE))
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
	    if(GetIntValue(EVILINTERFACE))
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
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusHeroes;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusCastle;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue(EVILINTERFACE))
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
	    if(GetIntValue(EVILINTERFACE))
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
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusHeroes;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;	// background panel castle
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    for(i = 0; i < 8; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		// регистрируем область
		ZeroINTERFACEACTION(&action);
		action.rect = dst;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickFocusCastle;
		AddActionEvent(&stpemaindisplay, &action);
		dst.y += 32;
	    }

	    // BOTTOM PANEL BACKGROUND ELEMENT
	    if(GetIntValue(EVILINTERFACE))
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

    SetCursor(CURSOR_SCROLL_TOP);

    if(display.offsetY){
	display.lastOffsetX = display.offsetX;
	display.lastOffsetY = display.offsetY;
	--display.offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollBottom(void){

    SetCursor(CURSOR_SCROLL_BOTTOM);

    if(display.offsetY < GetHeightMaps() - GetHeightArea()){
	display.lastOffsetX = display.offsetX;
	display.lastOffsetY = display.offsetY;
	++display.offsetY;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollRight(void){

    SetCursor(CURSOR_SCROLL_RIGHT);

    if(display.offsetX < GetWidthMaps() - GetWidthArea()){
	display.lastOffsetY = display.offsetY;
	display.lastOffsetX = display.offsetX;
	++display.offsetX;
	RedrawMapsArea();
    }

    return NONE;
}

ACTION ActionScrollLeft(void){

    SetCursor(CURSOR_SCROLL_LEFT);

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
    if(0 > display.offsetX - GetWidthArea() / 2)
	display.offsetX = 0;
    else
	display.offsetX = display.offsetX - GetWidthArea() / 2;
    

    if(0 > display.offsetY - GetHeightArea() / 2)
	display.offsetY = 0;
    else
	display.offsetY = display.offsetY - GetHeightArea() / 2;
    

    // проверка за границы
    if(display.offsetY > GetHeightMaps() - GetHeightArea())
	display.offsetY = GetHeightMaps() - GetHeightArea();
    

    if(display.offsetX > GetWidthMaps() - GetWidthArea())
	display.offsetX = GetWidthMaps() - GetWidthArea();
    
    RedrawMapsArea();

    return NONE;
}

void RedrawMapsArea(){

    // проверка границ экрана
    if(display.offsetX > GetWidthMaps() - GetWidthArea() || display.offsetY > GetHeightMaps() - GetHeightArea()) return;

    SetIntValue(ANIM2, FALSE);
    SetIntValue(CYCLELOOP, FALSE);

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest, src;

    CursorOff();

    Uint8 areaHeight = GetHeightArea();
    Uint8 areaWidth = GetWidthArea();

    // копируем видимую область area для сдвига при одинарном скроллинге
    src.x = BORDERWIDTH;
    src.y = BORDERWIDTH;
    src.w = GetWidthArea() * TILEWIDTH;
    src.h = GetHeightArea() * TILEWIDTH;
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

    // redraw radar
    RedrawRadarCursor();

    CursorOn();

    SDL_Flip(video);

    SetIntValue(CYCLELOOP, TRUE);
    SetIntValue(ANIM2, TRUE);

    return;
}

void DrawRectAreaMaps(SDL_Rect *rect){

    Uint8 x, y;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    S_CELLMAPS *ptrCell = NULL;

    // циклы разделены bugs (объекты больше чем tile, следующий tile зарисовывает объект)

    // сначала отрисовываем все tile
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x){

	    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

	    if(! ptrCell->animation) DrawCellAreaMapsTile(x, y);
    }

    // отрисовываем все нижние объекты
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x){

	    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

	    if(! ptrCell->animation) DrawCellAreaMapsLevel1(x, y);
    }

    // отрисовываем всех монстров
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x)
	    DrawCellAreaMapsMonster(x, y);

    // отрисовываем все верхние объекты
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x){

	    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

	    if(! ptrCell->animation) DrawCellAreaMapsLevel2(x, y);
    }

    // отрисовываем все флаги
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x)
	    DrawCellAreaMapsFlags(x, y);

    // если присутствует анимация на клетке, отрисовываем отдельно, пару статичных спрайтов
    for(y = rect->y; y < rect->y + rect->h; ++y)
	for(x = rect->x; x < rect->x + rect->w; ++x)
	    DrawCellStaticAnimation(x, y);

    // и рисуем сетку
    if(GetIntValue(DEBUG)){

	dest.w = TILEWIDTH;
	dest.h = TILEWIDTH;

	for(y = rect->y; y < rect->y + rect->h; ++y)
	    for(x = rect->x; x < rect->x + rect->w; ++x){

		dest.x = BORDERWIDTH + x * TILEWIDTH;
		dest.y = BORDERWIDTH + y * TILEWIDTH;

		LockSurface(video);
		PutPixel(video, dest.x + dest.w - 1, dest.y + dest.h - 1, 0xFF00);
		UnlockSurface(video);
    }}
}

/* функция реализующая алгоритм отрисовки нижних объектов одной клетки */
void DrawCellAreaMapsLevel1(Uint8 x, Uint8 y){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);
    ICNHEADER  *icn = ptrCell->level1;

    // все нижние объекты
    while(icn){

	dest.x = icn->offsetX + BORDERWIDTH + x * TILEWIDTH;
	dest.y = icn->offsetY + BORDERWIDTH + y * TILEWIDTH;
	dest.w = icn->surface->w;
	dest.h = icn->surface->h;

        SDL_BlitSurface(icn->surface, NULL, video, &dest);

        icn = icn->next;
    }

}

void DrawCellAreaMapsTile(Uint8 x, Uint8 y){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

    dest.x = BORDERWIDTH + x * TILEWIDTH;
    dest.y = BORDERWIDTH + y * TILEWIDTH;
    dest.w = TILEWIDTH;
    dest.h = TILEWIDTH;

    SDL_BlitSurface(ptrCell->tile, NULL, video, &dest);
}

/* функция реализующая алгоритм отрисовки монстров одной клетки */
void DrawCellAreaMapsMonster(Uint8 x, Uint8 y){


    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

    if(! ptrCell->monster) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest, src;

    // monster 1 кадр
    dest.x = ptrCell->monster->rect[0].x + x * TILEWIDTH + TILEWIDTH;
    dest.y = ptrCell->monster->rect[0].y + BORDERWIDTH + y * TILEWIDTH + MONSTERFIXTURE;
    dest.w = ptrCell->monster->rect[0].w;
    dest.h = ptrCell->monster->rect[0].h;

    src = dest;
    src.x = 0;
    src.y = 0;

    if(x == 0 && dest.x < BORDERWIDTH){
	src.x += BORDERWIDTH - dest.x;
	dest.x = BORDERWIDTH;
    }else if(y == 0 && dest.y < BORDERWIDTH){
	src.y += BORDERWIDTH - dest.y;
	dest.y = BORDERWIDTH;
    }else if(x == GetWidthArea() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
	src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
    }

    SDL_BlitSurface(ptrCell->monster->surface[0], &src, video, &dest);

    // monster 2 кадр
    dest.x = ptrCell->monster->rect[1].x + x * TILEWIDTH + TILEWIDTH;
    dest.y = ptrCell->monster->rect[1].y + BORDERWIDTH + y * TILEWIDTH + MONSTERFIXTURE;
    dest.w = ptrCell->monster->rect[1].w;
    dest.h = ptrCell->monster->rect[1].h;

    src = dest;
    src.x = 0;
    src.y = 0;

    if(x == 0 && dest.x < BORDERWIDTH){
	src.x += BORDERWIDTH - dest.x;
	dest.x = BORDERWIDTH;
    }else if(y == 0 && dest.y < BORDERWIDTH){
	src.y += BORDERWIDTH - dest.y;
	dest.y = BORDERWIDTH;
    }else if(x == GetWidthArea() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
	src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
    }

    SDL_BlitSurface(ptrCell->monster->surface[1], &src, video, &dest);
}

/* функция реализующая алгоритм отрисовки флагов */
void DrawCellAreaMapsFlags(Uint8 x, Uint8 y){

    if(0 == x && 0 == y) return;

    S_CELLMAPS *ptrCellLeft;
    S_CELLMAPS *ptrCellRight;

    ptrCellLeft  = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x + 1);
    ptrCellRight = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x - 1);
    if(OBJ_CASTLE != ptrCellLeft->type && OBJ_CASTLE != ptrCellRight->type) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    AGGSPRITE	sprite;
    ICNHEADER	*flagLeft = NULL;
    ICNHEADER	*flagRight = NULL;
    S_CASTLE	*castle;

    castle = GetStatCastlePos(display.offsetX + x, display.offsetY + y);

    if(! castle){
	if(GetIntValue(DEBUG)) fprintf(stderr, "DrawCellAreaMapsFlags: castle NULL, ax: %d, ay: %d\n", display.offsetX + x, display.offsetY + y);
	return;
    }
    
    switch(castle->color){
    
	case BLUE:
	    FillSPRITE(&sprite, "FLAG32.ICN", 0);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 1);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case GREEN:
	    FillSPRITE(&sprite, "FLAG32.ICN", 2);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 3);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case RED:
	    FillSPRITE(&sprite, "FLAG32.ICN", 4);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 5);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case YELLOW:
	    FillSPRITE(&sprite, "FLAG32.ICN", 6);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 7);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case ORANGE:
	    FillSPRITE(&sprite, "FLAG32.ICN", 8);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 9);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case PURPLE:
	    FillSPRITE(&sprite, "FLAG32.ICN", 10);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 11);
	    flagRight = GetICNHeader(&sprite);
	    break;
    
	case GRAY:
	    FillSPRITE(&sprite, "FLAG32.ICN", 12);
	    flagLeft = GetICNHeader(&sprite);
	    FillSPRITE(&sprite, "FLAG32.ICN", 13);
	    flagRight = GetICNHeader(&sprite);
	    break;
    }

    // левый флаг
    if(flagLeft && OBJ_CASTLE == ptrCellLeft->type){
	dest.x = BORDERWIDTH + TILEWIDTH * x + flagLeft->offsetX;
	dest.y = BORDERWIDTH + TILEWIDTH * y + flagLeft->offsetY;
	dest.w = flagLeft->surface->w;
	dest.h = flagLeft->surface->h;
	SDL_BlitSurface(flagLeft->surface, NULL, video, &dest);
    }

    // правый флаг
    if(flagRight && OBJ_CASTLE == ptrCellRight->type){
	dest.x = BORDERWIDTH + TILEWIDTH * x + flagRight->offsetX;
	dest.y = BORDERWIDTH + TILEWIDTH * y + flagRight->offsetY;
	dest.w = flagRight->surface->w;
	dest.h = flagRight->surface->h;
	SDL_BlitSurface(flagRight->surface, NULL, video, &dest);
    }
}

/* функция реализующая алгоритм отрисовки первых 2 кадров анимации */
void DrawCellStaticAnimation(Uint8 x, Uint8 y){

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

    if(! ptrCell->animation) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    DrawCellAreaMapsTile(x, y);

    // все нижние объекты
    DrawCellAreaMapsLevel1(x, y);

    // анимация 1 кадр
    dest.x = ptrCell->animation->rect[0].x + BORDERWIDTH + x * TILEWIDTH;
    dest.y = ptrCell->animation->rect[0].y + BORDERWIDTH + y * TILEWIDTH;
    dest.w = ptrCell->animation->rect[0].w;
    dest.h = ptrCell->animation->rect[0].h;
    SDL_BlitSurface(ptrCell->animation->surface[0], NULL, video, &dest);

    // анимация 2 кадр
    dest.x = ptrCell->animation->rect[1].x + BORDERWIDTH + x * TILEWIDTH;
    dest.y = ptrCell->animation->rect[1].y + BORDERWIDTH + y * TILEWIDTH;
    dest.w = ptrCell->animation->rect[0].w;
    dest.h = ptrCell->animation->rect[0].h;
    SDL_BlitSurface(ptrCell->animation->surface[1], NULL, video, &dest);

    // все верхние объекты
    DrawCellAreaMapsLevel2(x, y);
}

/* функция реализующая алгоритм отрисовки верхних объектов одной клетки */
void DrawCellAreaMapsLevel2(Uint8 x, Uint8 y){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect dest;

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);
    ICNHEADER  *icn = ptrCell->level2;

    while(icn){

	dest.x = icn->offsetX + BORDERWIDTH + x * TILEWIDTH;
	dest.y = icn->offsetY + BORDERWIDTH + y * TILEWIDTH;
	dest.w = icn->surface->w;
	dest.h = icn->surface->h;

        SDL_BlitSurface(icn->surface, NULL, video, &dest);

        icn = icn->next;
    }

}

void RedrawMapsAnimation(void){

    if(! GetIntValue(ANIMATION)) return;
    if(! GetIntValue(ANIM2)) return;

    static Uint32 animationFrame = 0;
    Uint8 x, y;

    S_CELLMAPS	*ptrCell = NULL;
    S_ANIMATION *ptrFrame = NULL;
    SDL_Surface	*video = SDL_GetVideoSurface();

    SDL_Rect dest, src;
    Uint8 frame;

    Sint32 cx, cy;

    SDL_GetMouseState(&cx, &cy);
    
    for(y = 0; y < GetHeightArea(); ++y)

	for(x = 0; x < GetWidthArea(); ++x){

	    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

	    if(! ptrCell->animation && ! ptrCell->monster) continue;

	    dest.w = 3 * TILEWIDTH + CURSOR_WIDTH;
	    dest.h = 2 * TILEWIDTH + CURSOR_HEIGHT;

	    // если курсор над клеткой с анимацией то выключаем
	    if(BORDERWIDTH + x * TILEWIDTH < CURSOR_WIDTH * 2) dest.x = 0; else dest.x = BORDERWIDTH + x * TILEWIDTH - CURSOR_WIDTH * 2;
	    if(BORDERWIDTH + y * TILEWIDTH < CURSOR_HEIGHT * 2) dest.y = 0; else dest.y = BORDERWIDTH + y * TILEWIDTH - CURSOR_HEIGHT * 2;
	    if(ValidPoint(&dest, cx, cy)) CursorOff();

	    // TILE
	    DrawCellAreaMapsTile(x, y);

	    // все нижние объекты
	    DrawCellAreaMapsLevel1(x, y);


	    // multi анимация
	    if(ptrCell->animation){

		ptrFrame = ptrCell->animation;
		while(ptrFrame){
	    
		    frame = 1 + animationFrame % (ptrFrame->count - 1);
		    dest.x = ptrFrame->rect[frame].x + BORDERWIDTH + x * TILEWIDTH;
		    dest.y = ptrFrame->rect[frame].y + BORDERWIDTH + y * TILEWIDTH;
		    dest.w = ptrFrame->rect[frame].w;
		    dest.h = ptrFrame->rect[frame].h;
		    SDL_BlitSurface(ptrFrame->surface[frame], NULL, video, &dest);

		    ptrFrame = (S_ANIMATION *) ptrFrame->next;
		}

		if(OBJ_MOUNTS == ptrCell->type) DrawCellAreaMapsLevel1(x, y);
	    }

	    // monster анимация
	    if(ptrCell->monster){

		// перерисовать клетки
		// справа - тень монстра
		if(x){
		    DrawCellAreaMapsTile(x - 1, y);
		    DrawCellAreaMapsLevel1(x - 1, y);
		    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x - 1);
		    // multi анимация
		    if(ptrCell->animation){
			ptrFrame = ptrCell->animation;
			while(ptrFrame){
	    
			    frame = 1 + animationFrame % (ptrFrame->count - 1);
			    dest.x = ptrFrame->rect[frame].x + BORDERWIDTH + (x - 1) * TILEWIDTH;
			    dest.y = ptrFrame->rect[frame].y + BORDERWIDTH + y * TILEWIDTH;
			    dest.w = ptrFrame->rect[frame].w;
			    dest.h = ptrFrame->rect[frame].h;
			    SDL_BlitSurface(ptrFrame->surface[frame], NULL, video, &dest);

			    ptrFrame = (S_ANIMATION *) ptrFrame->next;
			}
		    }
		}
		// слева - части монстра
		if(x < GetWidthArea() - 1){
		    DrawCellAreaMapsTile(x + 1, y);
		    DrawCellAreaMapsLevel1(x + 1, y);
		    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x + 1);
		    // multi анимация
		    if(ptrCell->animation){
			ptrFrame = ptrCell->animation;
			while(ptrFrame){
	    
			    frame = 1 + animationFrame % (ptrFrame->count - 1);
			    dest.x = ptrFrame->rect[frame].x + BORDERWIDTH + (x + 1) * TILEWIDTH;
			    dest.y = ptrFrame->rect[frame].y + BORDERWIDTH + y * TILEWIDTH;
			    dest.w = ptrFrame->rect[frame].w;
			    dest.h = ptrFrame->rect[frame].h;
			    SDL_BlitSurface(ptrFrame->surface[frame], NULL, video, &dest);

			    ptrFrame = (S_ANIMATION *) ptrFrame->next;
			}
		    }
		}
		// сверху - части монстра
		if(y){
		    DrawCellAreaMapsTile(x, y - 1);
		    DrawCellAreaMapsLevel1(x, y - 1);
		    ptrCell = GetCELLMAPS((display.offsetY + y - 1) * GetWidthMaps() + display.offsetX + x);
		    // multi анимация
		    if(ptrCell->animation){
			ptrFrame = ptrCell->animation;
			while(ptrFrame){
	    
			    frame = 1 + animationFrame % (ptrFrame->count - 1);
			    dest.x = ptrFrame->rect[frame].x + BORDERWIDTH + x * TILEWIDTH;
			    dest.y = ptrFrame->rect[frame].y + BORDERWIDTH + (y - 1) * TILEWIDTH;
			    dest.w = ptrFrame->rect[frame].w;
			    dest.h = ptrFrame->rect[frame].h;
			    SDL_BlitSurface(ptrFrame->surface[frame], NULL, video, &dest);

			    ptrFrame = (S_ANIMATION *) ptrFrame->next;
			}
		    }
		}

		// основная клетка с монстром
		ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);
		ptrFrame = ptrCell->monster;
		while(ptrFrame){
		    dest.x = ptrFrame->rect[0].x + x * TILEWIDTH + TILEWIDTH;
		    dest.y = ptrFrame->rect[0].y + BORDERWIDTH + y * TILEWIDTH + MONSTERFIXTURE;
		    dest.w = ptrFrame->rect[0].w;
		    dest.h = ptrFrame->rect[0].h;
		    
		    src = dest;
		    src.x = 0;
		    src.y = 0;

		    if(x == 0 && dest.x < BORDERWIDTH){
			src.x += BORDERWIDTH - dest.x;
			dest.x = BORDERWIDTH;
		    }else if(y == 0 && dest.y < BORDERWIDTH){
			src.y += BORDERWIDTH - dest.y;
			dest.y = BORDERWIDTH;
		    }else if(x == GetWidthArea() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
		        src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
		    }

		    SDL_BlitSurface(ptrFrame->surface[0], &src, video, &dest);

		    frame = 1 + animationFrame % (ptrFrame->count - 1);
		    dest.x = ptrFrame->rect[frame].x + x * TILEWIDTH + TILEWIDTH;
		    dest.y = ptrFrame->rect[frame].y + BORDERWIDTH + y * TILEWIDTH + MONSTERFIXTURE;
		    dest.w = ptrFrame->rect[frame].w;
		    dest.h = ptrFrame->rect[frame].h;
		    
		    src = dest;
		    src.x = 0;
		    src.y = 0;

		    if(x == 0 && dest.x < BORDERWIDTH){
			src.x += BORDERWIDTH - dest.x;
			dest.x = BORDERWIDTH;
		    }else if(y == 0 && dest.y < BORDERWIDTH){
			src.y += BORDERWIDTH - dest.y;
			dest.y = BORDERWIDTH;
		    }else if(x == GetWidthArea() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
		        src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
		    }

		    SDL_BlitSurface(ptrFrame->surface[frame], &src, video, &dest);

		    ptrFrame = (S_ANIMATION *) ptrFrame->next;
		}

		// перерисовать клетки
		// справа - тень монстра
		if(x) DrawCellAreaMapsLevel2(x - 1, y);
		// слева - части монстра
		if(x < GetWidthArea() - 1) DrawCellAreaMapsLevel2(x + 1, y);
		// сверху - части монстра
		if(y) DrawCellAreaMapsLevel2(x, y - 1);
	    }

	    // все верхние объекты
	    DrawCellAreaMapsLevel2(x, y);
    }

    CursorOn();
    SDL_Flip(video);

    ++animationFrame;

    return;
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

Uint8 GetWidthArea(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    return (video->w - RADARWIDTH - 3 * BORDERWIDTH) / TILEWIDTH;
}

Uint8 GetHeightArea(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    return (video->h - 2 * BORDERWIDTH) / TILEWIDTH;
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

ACTION ActionButtonCloseDay(void){

    return ENDTUR;
}

ACTION ActionButtonInfo(void){

    SetIntValue(ANIM2, FALSE);
    CursorOff();

    // Сохраняем курсор
    Uint32 cursor = GetCursor();    

    AGGSPRITE sprite;
    SDL_Rect rectCur, rectBack;

    INTERFACEACTION action;
    INTERFACEACTION *dialogInfo = NULL;

    ACTION result = NONE;

    SDL_Surface *format = NULL;
    SDL_Surface *back = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    // фон панели cpanbkg
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANBKGE.ICN", 0) : FillSPRITE(&sprite, "APANBKG.ICN", 0);
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
    // картинка
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANBKGE.ICN", 0) : FillSPRITE(&sprite, "APANBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + widthshadow;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // world
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANELE.ICN", 0) : FillSPRITE(&sprite, "APANEL.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 78;
    rectCur.y = rectBack.y + 30;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "APANELE.ICN", 0) : FillSPRITE(&action.objectUp, "APANEL.ICN", 0);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "APANELE.ICN", 1) : FillSPRITE(&action.objectPush, "APANEL.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = InfoClickWorld;
    AddActionEvent(&dialogInfo, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // puzzle
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANELE.ICN", 2) : FillSPRITE(&sprite, "APANEL.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 211;
    rectCur.y = rectBack.y + 30;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "APANELE.ICN", 2) : FillSPRITE(&action.objectUp, "APANEL.ICN", 2);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "APANELE.ICN", 3) : FillSPRITE(&action.objectPush, "APANEL.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = InfoClickPuzzle;
    AddActionEvent(&dialogInfo, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // info
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANELE.ICN", 4) : FillSPRITE(&sprite, "APANEL.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 78;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "APANELE.ICN", 4) : FillSPRITE(&action.objectUp, "APANEL.ICN", 4);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "APANELE.ICN", 5) : FillSPRITE(&action.objectPush, "APANEL.ICN", 5);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = InfoClickInfo;
    AddActionEvent(&dialogInfo, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // dig
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANELE.ICN", 6) : FillSPRITE(&sprite, "APANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 211;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "APANELE.ICN", 6) : FillSPRITE(&action.objectUp, "APANEL.ICN", 6);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "APANELE.ICN", 7) : FillSPRITE(&action.objectPush, "APANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = InfoClickDig;
    AddActionEvent(&dialogInfo, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // cancel
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "APANELE.ICN", 8) : FillSPRITE(&sprite, "APANEL.ICN", 8);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 144;
    rectCur.y = rectBack.y + 184;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "APANELE.ICN", 8) : FillSPRITE(&action.objectUp, "APANEL.ICN", 8);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "APANELE.ICN", 9) : FillSPRITE(&action.objectPush, "APANEL.ICN", 9);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = InfoClickCancel;
    AddActionEvent(&dialogInfo, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // отрисовка диалога
    SDL_Flip(video);

    // Стандартный курсор
    SetCursor(CURSOR_POINTER);

    CursorOn();

    // в цикл событий
    result = ActionCycle(dialogInfo, NULL);

    if(EXIT != result) result = NONE;

    // востанавливаем background
    CursorOff();

    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialogInfo);
    SDL_FreeSurface(back);

    // Востанавливаем курсор
    SetCursor(cursor);    

    CursorOn();
    SetIntValue(ANIM2, TRUE);

    return result;
}

ACTION ActionButtonMenu(void){

    SetIntValue(ANIM2, FALSE);
    CursorOff();

    // Сохраняем курсор
    Uint32 cursor = GetCursor();    

    AGGSPRITE sprite;
    SDL_Rect rectCur, rectBack;

    INTERFACEACTION action;
    INTERFACEACTION *dialogMenu = NULL;

    ACTION result = NONE;

    SDL_Surface *format = NULL;
    SDL_Surface *back = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    // фон панели cpanbkg
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANBKGE.ICN", 0) : FillSPRITE(&sprite, "CPANBKG.ICN", 0);
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
    // картинка
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANBKGE.ICN", 0) : FillSPRITE(&sprite, "CPANBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + widthshadow;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // new
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANELE.ICN", 0) : FillSPRITE(&sprite, "CPANEL.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 78;
    rectCur.y = rectBack.y + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "CPANELE.ICN", 0) : FillSPRITE(&action.objectUp, "CPANEL.ICN", 0);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "CPANELE.ICN", 1) : FillSPRITE(&action.objectPush, "CPANEL.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = MenuClickNew;
    AddActionEvent(&dialogMenu, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // load
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANELE.ICN", 2) : FillSPRITE(&sprite, "CPANEL.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 211;
    rectCur.y = rectBack.y + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "CPANELE.ICN", 2) : FillSPRITE(&action.objectUp, "CPANEL.ICN", 2);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "CPANELE.ICN", 3) : FillSPRITE(&action.objectPush, "CPANEL.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = MenuClickLoad;
    AddActionEvent(&dialogMenu, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // save
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANELE.ICN", 4) : FillSPRITE(&sprite, "CPANEL.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 78;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "CPANELE.ICN", 4) : FillSPRITE(&action.objectUp, "CPANEL.ICN", 4);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "CPANELE.ICN", 5) : FillSPRITE(&action.objectPush, "CPANEL.ICN", 5);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = MenuClickSave;
    AddActionEvent(&dialogMenu, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // quit
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANELE.ICN", 6) : FillSPRITE(&sprite, "CPANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 211;
    rectCur.y = rectBack.y + 107;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "CPANELE.ICN", 6) : FillSPRITE(&action.objectUp, "CPANEL.ICN", 6);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "CPANELE.ICN", 7) : FillSPRITE(&action.objectPush, "CPANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = MenuClickQuit;
    AddActionEvent(&dialogMenu, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // cancel
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "CPANELE.ICN", 8) : FillSPRITE(&sprite, "CPANEL.ICN", 8);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 144;
    rectCur.y = rectBack.y + 184;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "CPANELE.ICN", 8) : FillSPRITE(&action.objectUp, "CPANEL.ICN", 8);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "CPANELE.ICN", 9) : FillSPRITE(&action.objectPush, "CPANEL.ICN", 9);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = MenuClickCancel;
    AddActionEvent(&dialogMenu, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // отрисовка диалога
    SDL_Flip(video);

    // Стандартный курсор
    SetCursor(CURSOR_POINTER);

    CursorOn();

    // в цикл событий
    result = ActionCycle(dialogMenu, NULL);

    if(EXIT != result) result = NONE;

    // востанавливаем background
    CursorOff();

    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialogMenu);
    SDL_FreeSurface(back);

    // Востанавливаем курсор
    SetCursor(cursor);    

    CursorOn();
    SetIntValue(ANIM2, TRUE);

    return result;
}

ACTION ActionButtonSettings(void){

    SetIntValue(ANIM2, FALSE);
    CursorOff();

    // Сохраняем курсор
    Uint32 cursor = GetCursor();    

    AGGSPRITE sprite;
    SDL_Rect rectCur, rectBack;

    INTERFACEACTION action;
    INTERFACEACTION *dialogSettings = NULL;

    ACTION result = NONE;

    SDL_Surface *format = NULL;
    SDL_Surface *back = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();

    // фон панели spanbkg
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "SPANBKGE.ICN", 0) : FillSPRITE(&sprite, "SPANBKG.ICN", 0);
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
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "SPANBKGE.ICN", 1) : FillSPRITE(&sprite, "SPANBKG.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.y = rectBack.y + widthshadow;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // картинка
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "SPANBKGE.ICN", 0) : FillSPRITE(&sprite, "SPANBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + widthshadow;
    rectCur.y = rectBack.y;
    rectCur.w = image->w;
    rectCur.h = image->h;    
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // music
    FillSPRITE(&sprite, "SPANEL.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 52;
    rectCur.y = rectBack.y + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 0);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickMusic;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 67;
    rectCur.y = rectBack.y + 34;
    rectCur.w = FONT_WIDTHSMALL * 6;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Music", FONT_SMALL);

    // sound
    FillSPRITE(&sprite, "SPANEL.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 144;
    rectCur.y = rectBack.y + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 2);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickSound;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 155;
    rectCur.y = rectBack.y + 34;
    rectCur.w = FONT_WIDTHSMALL * 8;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Effects", FONT_SMALL);

    // type
    FillSPRITE(&sprite, "SPANEL.ICN", 11);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 236;
    rectCur.y = rectBack.y + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 11);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 12);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickType;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 235;
    rectCur.y = rectBack.y + 34;
    rectCur.w = FONT_WIDTHSMALL * 12;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Music Type", FONT_SMALL);

    // heroes speed
    FillSPRITE(&sprite, "SPANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 52;
    rectCur.y = rectBack.y + 157;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 6);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickHeroesSpeed;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 68;
    rectCur.y = rectBack.y + 144;
    rectCur.w = FONT_WIDTHSMALL * 6;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Speed", FONT_SMALL);

    // enemy speed
    FillSPRITE(&sprite, "SPANEL.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 144;
    rectCur.y = rectBack.y + 157;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 6);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 7);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickEnemySpeed;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 138;
    rectCur.y = rectBack.y + 144;
    rectCur.w = FONT_WIDTHSMALL * 14;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Enemy Speed", FONT_SMALL);

    // path
    FillSPRITE(&sprite, "SPANEL.ICN", 13);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 236;
    rectCur.y = rectBack.y + 157;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 13);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 14);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickPath;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 236;
    rectCur.y = rectBack.y + 144;
    rectCur.w = FONT_WIDTHSMALL * 11;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Show Path", FONT_SMALL);

    // interface
    FillSPRITE(&sprite, "SPANEL.ICN", 16);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 52;
    rectCur.y = rectBack.y + 267;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 16);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 17);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickInterface;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 58;
    rectCur.y = rectBack.y + 254;
    rectCur.w = FONT_WIDTHSMALL * 10;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Interface", FONT_SMALL);

    // video
    FillSPRITE(&sprite, "SPANEL.ICN", 18);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 144;
    rectCur.y = rectBack.y + 267;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 18);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 19);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickVideo;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 160;
    rectCur.y = rectBack.y + 254;
    rectCur.w = FONT_WIDTHSMALL * 6;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Video", FONT_SMALL);

    // cursor
    FillSPRITE(&sprite, "SPANEL.ICN", 20);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 236;
    rectCur.y = rectBack.y + 267;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SPANEL.ICN", 20);
    FillSPRITE(&action.objectPush, "SPANEL.ICN", 21);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickCursor;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text
    rectCur.x = rectBack.x + 228;
    rectCur.y = rectBack.y + 254;
    rectCur.w = FONT_WIDTHSMALL * 15;
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, "Mouse Cursor", FONT_SMALL);

    // okay
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "SPANBTNE.ICN", 0) : FillSPRITE(&sprite, "SPANBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 129;
    rectCur.y = rectBack.y + 361;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectUp, "SPANBTNE.ICN", 0) : FillSPRITE(&action.objectUp, "SPANBTN.ICN", 0);
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&action.objectPush, "SPANBTNE.ICN", 1) : FillSPRITE(&action.objectPush, "SPANBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = SettingsClickOkay;
    AddActionEvent(&dialogSettings, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // отрисовка диалога
    SDL_Flip(video);

    // Стандартный курсор
    SetCursor(CURSOR_POINTER);

    CursorOn();

    // в цикл событий
    result = ActionCycle(dialogSettings, NULL);

    if(EXIT != result) result = NONE;

    // востанавливаем background
    CursorOff();

    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialogSettings);
    SDL_FreeSurface(back);

    // Востанавливаем курсор
    SetCursor(cursor);    

    CursorOn();
    SetIntValue(ANIM2, TRUE);

    return result;
}

ACTION ActionScrollHeroesUp(void){

    RedrawFocusPanel(&gameFocus);
    return NONE;
}

ACTION ActionScrollHeroesDown(void){

    RedrawFocusPanel(&gameFocus);
    return NONE;
}

ACTION ActionScrollCastleUp(void){

    Uint8 maxCount;

    switch(GetIntValue(VIDEOMODE)){
        default:
        case 0:
            maxCount = 4;
            break;

        case 1:
            maxCount = 7;
            break;

        case 2:
        case 3:
            maxCount = 8;
            break;
    }

    if(GetCountCastle(GetIntValue(HUMANCOLORS)) < maxCount) return NONE;

    S_CASTLE *castle = GetFirstCastle(GetIntValue(HUMANCOLORS));
    while(castle && castle != gameFocus.firstCastle) castle = GetNextCastle(GetIntValue(HUMANCOLORS));

    gameFocus.firstCastle = GetPrevCastle(GetIntValue(HUMANCOLORS));
    if(! gameFocus.firstCastle) gameFocus.firstCastle = GetFirstCastle(GetIntValue(HUMANCOLORS));

    RedrawFocusPanel(&gameFocus);

    return NONE;
}

ACTION ActionScrollCastleDown(void){

    Uint8 maxCount, count;

    switch(GetIntValue(VIDEOMODE)){
        default:
        case 0:
            maxCount = 4;
            break;

        case 1:
            maxCount = 7;
            break;

        case 2:
        case 3:
            maxCount = 8;
            break;
    }

    if(GetCountCastle(GetIntValue(HUMANCOLORS)) < maxCount) return NONE;

    S_CASTLE *castle = GetFirstCastle(GetIntValue(HUMANCOLORS));

    count = 0;
    while(castle && castle != gameFocus.firstCastle){
	castle = GetNextCastle(GetIntValue(HUMANCOLORS));
	++count;
    }

    if(count < GetCountCastle(GetIntValue(HUMANCOLORS)) - maxCount)
	gameFocus.firstCastle = GetNextCastle(GetIntValue(HUMANCOLORS));

    RedrawFocusPanel(&gameFocus);

    return NONE;
}

ACTION ActionClickFocusHeroes(void){

    display.lastOffsetX = 0xFF;
    display.lastOffsetY = 0xFF;

    return NONE;
}

ACTION ActionClickFocusCastle(void){

    Sint32 mx, my;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (my - RADARWIDTH - BORDERWIDTH - 21) / 32;

    S_CASTLE *castle = GetFirstCastle(GetIntValue(HUMANCOLORS));
    
    while(castle && castle != gameFocus.firstCastle) castle = GetNextCastle(GetIntValue(HUMANCOLORS));

    while(index){
	castle = GetNextCastle(GetIntValue(HUMANCOLORS));
	--index;
    }

    if(castle){
	if(ValidPoint(&gameFocus.back, mx, my) &&
	    EXIT == EnterCastle(gameFocus.ax, gameFocus.ay, SANDYSANDY))
		return EXIT;

	SetGameFocus(castle, OBJ_CASTLE);
	RedrawFocusPanel(&gameFocus);

	display.lastOffsetX = 0xFF;
	display.lastOffsetY = 0xFF;

	RedrawMapsArea();
	RedrawRadar();
    }

    return NONE;
}

S_DISPLAY *GetDisplayPos(void){

    return &display;
}

ACTION InfoClickWorld(void){

    return NONE;
}

ACTION InfoClickPuzzle(void){

    return NONE;
}

ACTION InfoClickInfo(void){

    return NONE;
}

ACTION InfoClickDig(void){

    return NONE;
}

ACTION InfoClickCancel(void){

    return CANCEL;
}

ACTION MenuClickNew(void){

    return NONE;
}

ACTION MenuClickLoad(void){

    return NONE;
}

ACTION MenuClickSave(void){

    return NONE;
}

ACTION MenuClickQuit(void){

    return EXIT;
}

ACTION MenuClickCancel(void){

    return CANCEL;
}

ACTION SettingsClickMusic(void){

    return NONE;
}

ACTION SettingsClickSound(void){

    return NONE;
}

ACTION SettingsClickType(void){

    return NONE;
}

ACTION SettingsClickHeroesSpeed(void){

    return NONE;
}

ACTION SettingsClickEnemySpeed(void){

    return NONE;
}

ACTION SettingsClickPath(void){

    return NONE;
}

ACTION SettingsClickInterface(void){

    return NONE;
}

ACTION SettingsClickVideo(void){

    return NONE;
}

ACTION SettingsClickCursor(void){

    return NONE;
}

ACTION SettingsClickOkay(void){

    fprintf(stderr, "save options\n");

    return OK;
}

void CheckCursorAreaAction(E_OBJECT f){

    Sint32 x, y;
    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_GetMouseState(&x, &y);

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + (y - BORDERWIDTH) / TILEWIDTH) * GetWidthMaps() + display.offsetX + (x - BORDERWIDTH) / TILEWIDTH);
    S_CASTLE *castle = NULL;

    // если над областью арены то по свойствам
    if(x > BORDERWIDTH && x < BORDERWIDTH + GetWidthArea() * TILEWIDTH && y > BORDERWIDTH && y < video->h - BORDERWIDTH)

	switch(f){
	
	    // фокус на лодке
	    case OBJ_BOAT:

		switch(ptrCell->type){

		    case OBJ_TREASURECHEST:
			WATER == ptrCell->ground ? SetCursor(CURSOR_REDBOAT) : SetCursor(CURSOR_POINTER);
			break;

		    case OBJ_SHIPWRECK:
		    case OBJ_WHIRLPOOL:
		    case OBJ_BUOY:
		    case OBJ_BOTTLE:
		    case OBJ_SHIPWRECKSURVIROR:
		    case OBJ_FLOTSAM:
		    case OBJ_MAGELLANMAPS:
			SetCursor(CURSOR_REDBOAT);
			break;
		    
		    case OBJ_COAST:
			SetCursor(CURSOR_ANCHOR);
			break;

		    case OBJN_CASTLE:
		    case OBJ_CASTLE:
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(castle->color == GetIntValue(HUMANCOLORS))
			    SetCursor(CURSOR_CASTLE);
			else
			    SetCursor(CURSOR_POINTER);
			break;

		    case OBJ_HEROES:
			SetCursor(CURSOR_HEROES);
			break;
		
		    case OBJN_SHIPWRECK:
		    case OBJN_DERELICTSHIP:
		    case OBJN_MAGELLANMAPS:
		    case OBJ_STONES:
			SetCursor(CURSOR_POINTER);
			break;

		    default:
			WATER == ptrCell->ground ? SetCursor(CURSOR_BOAT) : SetCursor(CURSOR_POINTER);
			break;

		}
		break;
		
	    // фокус на герое
	    case OBJ_HEROES:

		switch(ptrCell->type){
		
		    case OBJ_MONSTER:
			SetCursor(CURSOR_FIGHT);
			break;

		    case OBJN_CASTLE:
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(castle->color == GetIntValue(HUMANCOLORS))
			    SetCursor(CURSOR_CASTLE);
			else
			    SetCursor(CURSOR_FIGHT);
			break;

		    case OBJ_CASTLE:
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(castle->color == GetIntValue(HUMANCOLORS))
			    SetCursor(CURSOR_ACTION);
			else
			    SetCursor(CURSOR_FIGHT);
			break;

		    case OBJ_BOAT:
			SetCursor(CURSOR_BOAT);
			break;

		    case OBJ_TREASURECHEST:
			WATER == ptrCell->ground ? SetCursor(CURSOR_POINTER) : SetCursor(CURSOR_ACTION);
			break;

		    case OBJ_STONES:
		    case OBJ_OILLAKE:
		    case OBJ_BIGCRACK:
		    case OBJ_MOUNTS:
		    case OBJ_TREES:
		    case OBJN_WAGONCAMP:
		    case OBJN_SAWMILL:
		    case OBJN_MINES:
		    case OBJ_WATERLAKE:
		    case OBJN_ALCHEMYTOWER:
		    case OBJN_EXCAVATION:
		    case OBJN_FORT:
		    case OBJN_DRAGONCITY:
			SetCursor(CURSOR_POINTER);
			break;

		    case OBJ_ALCHEMYTOWER:
		    case OBJ_SIGN:
    	    	    case OBJ_SKELETON:
    		    case OBJ_DAEMONCAVE:
		    case OBJ_FAERIERING:
		    case OBJ_CAMPFIRE:
	    	    case OBJ_FOUNTAIN:
		    case OBJ_GAZEBO:
		    case OBJ_ANCIENTLAMP:
		    case OBJ_GRAVEYARD:
		    case OBJ_ARCHERHOUSE:
		    case OBJ_GOBLINHUNT:
		    case OBJ_DWARFCOTT:
		    case OBJ_PEASANTHUNT:
		    case OBJ_DRAGONCITY:
		    case OBJ_LIGHTHOUSE:
		    case OBJ_WATERMILL:
		    case OBJ_MINES:
		    case OBJ_OBELISK:
		    case OBJ_OASIS:
		    case OBJ_RESOURCE:
		    case OBJ_SAWMILL:
		    case OBJ_ORACLE:
		    case OBJ_SHRINE1:
		    case OBJ_DERELICTSHIP:
		    case OBJ_DESERTTENT:
		    case OBJ_STONELITHS:
		    case OBJ_WAGONCAMP:
		    case OBJ_WINDMILL:
		    case OBJ_ARTIFACT:
		    case OBJ_WATCHTOWER:
		    case OBJ_TREEHOUSE:
		    case OBJ_TREECITY:
		    case OBJ_RUINS:
		    case OBJ_FORT:
		    case OBJ_TRADINGPOST:
		    case OBJ_ABANDONEDMINE:
		    case OBJ_STANDINGSTONES:
		    case OBJ_IDOL:
		    case OBJ_TREEKNOWLEDGE:
		    case OBJ_DOCTORHUNT:
		    case OBJ_TEMPLE:
		    case OBJ_HILLFORT:
		    case OBJ_HALFLINGHOLE:
		    case OBJ_MERCENARYCAMP:
		    case OBJ_SHRINE2:
		    case OBJ_SHRINE3:
		    case OBJ_PIRAMID:
		    case OBJ_CITYDEAD:
		    case OBJ_EXCAVATION:
		    case OBJ_SPHINX:
		    case OBJ_WAGON:
		    case OBJ_ARTESIANSPRING:
		    case OBJ_TROLLBRIDGE:
		    case OBJ_WITCHHUNT:
		    case OBJ_XANADU:
		    case OBJ_CAVE:
		    case OBJ_LEANTO:
		    case OBJ_MAGICWELL:
		    case OBJ_MAGICGARDEN:
		    case OBJ_OBSERVATIONTOWER:
		    case OBJ_FREEMANFOUNDRY:
			SetCursor(CURSOR_ACTION);
			break;

		    default:
			WATER == ptrCell->ground ? SetCursor(CURSOR_POINTER) : SetCursor(CURSOR_MOVE);
			break;
		}
		break;
		
	    // фокус на замке
	    case OBJ_CASTLE:

		switch(ptrCell->type){

		    case OBJ_HEROES:
			SetCursor(CURSOR_HEROES);
			break;

		    case OBJN_CASTLE:
		    case OBJ_CASTLE:
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(castle->color == GetIntValue(HUMANCOLORS))
			    SetCursor(CURSOR_CASTLE);
			else
			    SetCursor(CURSOR_POINTER);
			break;

		    default:
			SetCursor(CURSOR_POINTER);
			break;
		}
		break;
	
	    default:
		return;
		break;
	}

    // если за областью арены то обычный курсор
    else if(x < video->w - BORDERWIDTH && x > BORDERWIDTH + GetWidthArea() * TILEWIDTH && y > BORDERWIDTH && y < video->h - BORDERWIDTH)
	SetCursor(CURSOR_POINTER);
}

ACTION ClickCursorAreaAction(E_OBJECT f){

    Sint32 x, y;
    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_GetMouseState(&x, &y);

    S_CELLMAPS *ptrCell = GetCELLMAPS((display.offsetY + (y - BORDERWIDTH) / TILEWIDTH) * GetWidthMaps() + display.offsetX + (x - BORDERWIDTH) / TILEWIDTH);
    S_CASTLE	*castle = NULL;
    
    // если над областью арены то по свойствам
    if(! (x > BORDERWIDTH && x < BORDERWIDTH + GetWidthArea() * TILEWIDTH && y > BORDERWIDTH && y < video->h - BORDERWIDTH)) return NONE;

    switch(f){
/*	
	    // фокус на лодке
	    case OBJ_BOAT:

		switch(ptrCell->type){

		    case OBJ_TREASURECHEST:
			WATER == ptrCell->ground ? SetCursor(CURSOR_REDBOAT) : SetCursor(CURSOR_POINTER);
			break;

		    case OBJ_SHIPWRECK:
		    case OBJ_WHIRLPOOL:
		    case OBJ_BUOY:
		    case OBJ_BOTTLE:
		    case OBJ_SHIPWRECKSURVIROR:
		    case OBJ_FLOTSAM:
		    case OBJ_MAGELLANMAPS:
			SetCursor(CURSOR_REDBOAT);
			break;
		    
		    case OBJ_COAST:
			SetCursor(CURSOR_ANCHOR);
			break;

		    case OBJN_CASTLE:
		    case OBJ_CASTLE:
			SetCursor(CURSOR_CASTLE);
			break;

		    case OBJ_HEROES:
			SetCursor(CURSOR_HEROES);
			break;
		
		    case OBJN_SHIPWRECK:
		    case OBJN_DERELICTSHIP:
		    case OBJN_MAGELLANMAPS:
		    case OBJ_STONES:
			SetCursor(CURSOR_POINTER);
			break;

		    default:
			WATER == ptrCell->ground ? SetCursor(CURSOR_BOAT) : SetCursor(CURSOR_POINTER);
			break;

		}
		break;
*/		
	    // фокус на герое
	    case OBJ_HEROES:

		switch(ptrCell->type){
		
		    case OBJ_MONSTER:
//			SetCursor(CURSOR_FIGHT);
			break;

		    case OBJN_CASTLE:

			// фокус на замок
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(!castle) break;

			if(castle->color == GetIntValue(HUMANCOLORS)){
			    gameFocus.type = OBJ_CASTLE;
			    gameFocus.ax = castle->ax;
			    gameFocus.ay = castle->ay;
			    gameFocus.object = castle;

			    SetGameFocus(castle, OBJ_CASTLE);
			    RedrawFocusPanel(&gameFocus);

			    display.lastOffsetX = 0xFF;
			    display.lastOffsetY = 0xFF;

			    RedrawRadar();
			    RedrawMapsArea();

			    if(EXIT == EnterCastle(gameFocus.ax, gameFocus.ay, HEROESNULL))
				return EXIT;
			}
			break;
/*
		    case OBJ_BOAT:
			SetCursor(CURSOR_BOAT);
			break;

		    case OBJ_TREASURECHEST:
			WATER == ptrCell->ground ? SetCursor(CURSOR_POINTER) : SetCursor(CURSOR_ACTION);
			break;

		    case OBJ_STONES:
		    case OBJ_OILLAKE:
		    case OBJ_BIGCRACK:
		    case OBJ_MOUNTS:
		    case OBJ_TREES:
		    case OBJN_WAGONCAMP:
		    case OBJN_SAWMILL:
		    case OBJN_MINES:
		    case OBJ_WATERLAKE:
		    case OBJN_ALCHEMYTOWER:
		    case OBJN_EXCAVATION:
		    case OBJN_FORT:
		    case OBJN_DRAGONCITY:
			SetCursor(CURSOR_POINTER);
			break;

		    case OBJ_CASTLE:
		    case OBJ_ALCHEMYTOWER:
		    case OBJ_SIGN:
    	    	    case OBJ_SKELETON:
    		    case OBJ_DAEMONCAVE:
		    case OBJ_FAERIERING:
		    case OBJ_CAMPFIRE:
	    	    case OBJ_FOUNTAIN:
		    case OBJ_GAZEBO:
		    case OBJ_ANCIENTLAMP:
		    case OBJ_GRAVEYARD:
		    case OBJ_ARCHERHOUSE:
		    case OBJ_GOBLINHUNT:
		    case OBJ_DWARFCOTT:
		    case OBJ_PEASANTHUNT:
		    case OBJ_DRAGONCITY:
		    case OBJ_LIGHTHOUSE:
		    case OBJ_WATERMILL:
		    case OBJ_MINES:
		    case OBJ_OBELISK:
		    case OBJ_OASIS:
		    case OBJ_RESOURCE:
		    case OBJ_SAWMILL:
		    case OBJ_ORACLE:
		    case OBJ_SHRINE1:
		    case OBJ_DERELICTSHIP:
		    case OBJ_DESERTTENT:
		    case OBJ_STONELITHS:
		    case OBJ_WAGONCAMP:
		    case OBJ_WINDMILL:
		    case OBJ_ARTIFACT:
		    case OBJ_WATCHTOWER:
		    case OBJ_TREEHOUSE:
		    case OBJ_TREECITY:
		    case OBJ_RUINS:
		    case OBJ_FORT:
		    case OBJ_TRADINGPOST:
		    case OBJ_ABANDONEDMINE:
		    case OBJ_STANDINGSTONES:
		    case OBJ_IDOL:
		    case OBJ_TREEKNOWLEDGE:
		    case OBJ_DOCTORHUNT:
		    case OBJ_TEMPLE:
		    case OBJ_HILLFORT:
		    case OBJ_HALFLINGHOLE:
		    case OBJ_MERCENARYCAMP:
		    case OBJ_SHRINE2:
		    case OBJ_SHRINE3:
		    case OBJ_PIRAMID:
		    case OBJ_CITYDEAD:
		    case OBJ_EXCAVATION:
		    case OBJ_SPHINX:
		    case OBJ_WAGON:
		    case OBJ_ARTESIANSPRING:
		    case OBJ_TROLLBRIDGE:
		    case OBJ_WITCHHUNT:
		    case OBJ_XANADU:
		    case OBJ_CAVE:
		    case OBJ_LEANTO:
		    case OBJ_MAGICWELL:
		    case OBJ_MAGICGARDEN:
		    case OBJ_OBSERVATIONTOWER:
		    case OBJ_FREEMANFOUNDRY:
			SetCursor(CURSOR_ACTION);
			break;
*/
		    default:
//			WATER == ptrCell->ground ? SetCursor(CURSOR_POINTER) : SetCursor(CURSOR_MOVE);
			break;
		}
		break;


	// фокус на замке
	case OBJ_CASTLE:

	    switch(ptrCell->type){

		case OBJ_HEROES:
//			SetCursor(CURSOR_HEROES);
			break;

		case OBJN_CASTLE:
		case OBJ_CASTLE:

			// фокус на замок
			castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
			if(!castle) break;

			if(castle->color == GetIntValue(HUMANCOLORS)){

			    gameFocus.type = OBJ_CASTLE;
			    SetGameFocus(castle, OBJ_CASTLE);
			    RedrawFocusPanel(&gameFocus);

			    display.lastOffsetX = 0xFF;
			    display.lastOffsetY = 0xFF;

			    RedrawRadar();
			    RedrawMapsArea();

			    EnterCastle(gameFocus.ax, gameFocus.ay, HEROESNULL);
			}
			break;

		default:
//			SetCursor(CURSOR_POINTER);
			break;
	    }
	    break;
	
	default:
	    break;
    }

    return NONE;
}

ACTION ActionGAMELOOP(void){

    Uint8 i; 
    Uint32 cursor;
    
    ACTION exit = ENDTUR;

    while(1){
    
	// ход humans
	while(! (ENDTUR == (exit = ActionHUMANLOOP(stpemaindisplay)) || 
		EXIT == exit || 
		(ESC == exit && YES == MessageBox("Are you sure you want to quit?", FONT_BIG))) );

	if(EXIT == exit || ESC == exit){
	    exit = EXIT;
	    break;
	}

	CursorOff();
	cursor = GetCursor();
	SetCursor(CURSOR_WAIT);
	CursorOn();

	// ход оппонентов по цветам
	for(i = 0; i < 8; ++i) 
    	    if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01)
		if(GetIntValue(HUMANCOLORS) != i && ENDTUR == exit)
		    ComputerStep(i);

	// расчет даты
	if(7 == GetIntValue(DAY)){
	    SetIntValue(DAY, 1);

	    if(4 == GetIntValue(WEEK)){
		SetIntValue(WEEK, 1);
		SetIntValue(MONTH, GetIntValue(MONTH) + 1);

		// перегенерация месячных событий карты
		for(i = 0; i < 8; ++i) 
    		    if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01)
			RecalculateKingdomMonth(i);

	    }else
		SetIntValue(WEEK, GetIntValue(WEEK) + 1);

	    // перегенерация недельных событий карты
	    for(i = 0; i < 8; ++i) 
    		if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01)
		    RecalculateKingdomWeek(i);

	    // увеличиваем недельный прирост
	    AllCastleIncreaseArmy();

	}else
	    SetIntValue(DAY, GetIntValue(DAY) + 1);

	// перегенерация однодневных событий карты
	// расчет ресурсов всех королевств
	for(i = 0; i < 8; ++i) 
    	    if((GetIntValue(KINGDOMCOLORS) >> i) & 0x01)
		RecalculateKingdomDay(i);

	// расчет хода для героев
	
	if(GetIntValue(DEBUG)) fprintf(stderr, "Month: %d, Week: %d, Day: %d\n", GetIntValue(MONTH), GetIntValue(WEEK), GetIntValue(DAY));

	CursorOff();
	SetCursor(cursor);
	CursorOn();
    }
    
    return exit;
}

ACTION ActionHUMANLOOP(INTERFACEACTION *action){

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

    Uint32	ticket = 0;
    Sint32 	cx, cy;

    // цикл по событиям
    while(exit == NONE){

	ptr = action;

	while(ptr){

	    SDL_GetMouseState(&cx, &cy);
	    if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, cx, cy) && ptr->pf)
		exit = (*ptr->pf)();

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

			    exit = ClickCursorAreaAction(gameFocus.type);

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
			    
			    SDL_GetMouseState(&cx, &cy);

			    // правая кнопка по арене, рисуем QuickInfo
			    if(cx > BORDERWIDTH && cy > BORDERWIDTH && cx < GetWidthArea() * TILEWIDTH + BORDERWIDTH && cy < GetHeightArea() * TILEWIDTH + BORDERWIDTH){
				cx -= BORDERWIDTH;

				cy -= BORDERWIDTH;
				cx /= TILEWIDTH;
				cy /= TILEWIDTH;
	
				ShowQuickInfo((display.offsetY + cy) * GetWidthMaps() + display.offsetX + cx);
			    }

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

	    if(exit == NONE && 0 == ticket % (GetIntValue(ANIMATIONDELAY) / 2)) RedrawMapsAnimation();
	    ++ticket;
	}

	CheckCursorAreaAction(gameFocus.type);

	if(0 == ticket % GetIntValue(ANIMATIONDELAY)) RedrawMapsAnimation();
	else if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);

	++ticket;
    }

    return exit;
}

void ComputerStep(E_COLORS color){

    printf("Computer %s: move\n", GetStringColor(color));
    return;
}

void InitCursorFocus(void){

    SDL_Surface *formatSurface = NULL;
    gameFocus.cursor = NULL;
    gameFocus.background = NULL;
    gameFocus.useBack = FALSE;
    gameFocus.firstCastle = GetFirstCastle(GetIntValue(HUMANCOLORS));

    if(NULL == (gameFocus.background = SDL_CreateRGBSurface(SDL_SWSURFACE, 56, 32, 16, 0, 0, 0, 0))){
        fprintf(stderr, "InitCursorFocus: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), 56, 32);
        return;
    }

    if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 56, 32, 16, 0, 0, 0, 0))){
        fprintf(stderr, "InitCursorFocus: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), 56, 32);
        return;
    }

    SDL_FillRect(formatSurface, NULL, COLORKEY);

    Uint8 i;
    for(i = 0; i < formatSurface->w; ++i){
        DrawPixel(formatSurface, i, 0, FOCUSCOLOR);
        DrawPixel(formatSurface, i, formatSurface->h - 1, FOCUSCOLOR);
    }
    for(i = 0; i < formatSurface->h; ++i){
        DrawPixel(formatSurface, 0, i, FOCUSCOLOR);
        DrawPixel(formatSurface, formatSurface->w - 1, i, FOCUSCOLOR);
    }

    SDL_SetColorKey(formatSurface, SDL_SRCCOLORKEY, COLORKEY);
    gameFocus.cursor = SDL_DisplayFormat(formatSurface);
    
    SDL_FreeSurface(formatSurface);
}

void FreeCursorFocus(void){

    if(gameFocus.cursor) SDL_FreeSurface(gameFocus.cursor);
    if(gameFocus.background) SDL_FreeSurface(gameFocus.background);
    gameFocus.cursor = NULL;
    gameFocus.background = NULL;
}

void RedrawFocusPanel(S_FOCUS *focus){

    // текущие кординаты
    SDL_Surface *video = NULL;
    SDL_Surface *image = NULL;
    SDL_Rect dst, cur;
    S_CASTLE *castle = GetFirstCastle(GetIntValue(HUMANCOLORS));

    AGGSPRITE sprite;

    video = SDL_GetVideoSurface();
    Uint8 i, seek, maxCount;

    switch(GetIntValue(VIDEOMODE)){
        default:
        case 0:
            maxCount = 4;
            break;

        case 1:
            maxCount = 7;
            break;

        case 2:
        case 3:
            maxCount = 8;
            break;
    }

    CursorOff();

    // heroes
/*
	    FillSPRITE(&sprite, icnname, 1);
	    image = GetICNSprite(&sprite);
	    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 5;
	    dst.y = RADARWIDTH + BORDERWIDTH + 21;
	    dst->w = image->w;
	    dst->h = image->h;
	    for(i = 0; i < 4; ++i){
		FillSPRITE(&sprite, icnname, i + 1);
		image = GetICNSprite(&sprite);
		SDL_BlitSurface(image, &src, video, &dst);
		dst.y += 32;
	    }
*/
    // castle
    i = 0;
    while(castle && castle != focus->firstCastle) castle = GetNextCastle(GetIntValue(HUMANCOLORS));

    if(gameFocus.useBack){
	SDL_BlitSurface(gameFocus.background, NULL, video, &gameFocus.back);
	gameFocus.useBack = FALSE;
    }

    dst.x = video->w - RADARWIDTH - BORDERWIDTH + 77;
    dst.y = RADARWIDTH + BORDERWIDTH + 21;
    dst.w = 46;
    dst.h = 22;
    while(castle && i < maxCount){
	if(castle->building & BUILD_CASTLE) seek = 9 + castle->race; else seek = 15 + castle->race;
	FillSPRITE(&sprite, "LOCATORS.ICN", seek);
	image = GetICNSprite(&sprite);
	SDL_BlitSurface(image, NULL, video, &dst);
	// маркируем
	if(focus->object == castle){
	    cur.x = dst.x - 5;
	    cur.y = dst.y - 5;
	    cur.w = gameFocus.background->w;
	    cur.h = gameFocus.background->h;
	    SDL_BlitSurface(video, &cur, gameFocus.background, NULL);
	    gameFocus.useBack = TRUE;
	    gameFocus.back = cur;
	    SDL_BlitSurface(gameFocus.cursor, NULL, video, &cur);
	}
	dst.y += 32;
	castle = GetNextCastle(GetIntValue(HUMANCOLORS));
	++i;
    }

    CursorOn();
}

void SetGameFocus(void *object, E_OBJECT type){

    S_CASTLE *castle = NULL;
    Sint32 mx, my;
    SDL_GetMouseState(&mx, &my);

    Uint8 first, maxCount, seek;

    switch(GetIntValue(VIDEOMODE)){
        default:
        case 0:
            maxCount = 4;
            break;

        case 1:
            maxCount = 7;
            break;

        case 2:
        case 3:
            maxCount = 8;
            break;
    }

    if(object && type == OBJ_CASTLE){

	castle = object;
	gameFocus.type = OBJ_CASTLE;
	gameFocus.ax = castle->ax;
	gameFocus.ay = castle->ay;
	gameFocus.object = castle;

	// позиция выбранного замка
	seek = 0;
	castle = GetFirstCastle(GetIntValue(HUMANCOLORS));
	while(castle != gameFocus.object){
	    castle = GetNextCastle(GetIntValue(HUMANCOLORS));
	    ++seek;
	}

	// позиция первого элемента
	if(GetCountCastle(GetIntValue(HUMANCOLORS)) > maxCount && seek < (GetCountCastle(GetIntValue(HUMANCOLORS)) - maxCount) && seek > maxCount)
	    gameFocus.firstCastle = gameFocus.object;
	else if(seek < maxCount)
	    gameFocus.firstCastle = GetFirstCastle(GetIntValue(HUMANCOLORS));
	else{
	    first = maxCount - 1;
	    gameFocus.firstCastle = GetEndCastle(GetIntValue(HUMANCOLORS));
	    while(first){
		gameFocus.firstCastle = GetPrevCastle(GetIntValue(HUMANCOLORS));
		--first;
	    }
	}

	gameFocus.firstHeroes = NULL;
    }

    if(gameFocus.ax < GetWidthArea() / 2) display.offsetX = 0;
    else if(GetWidthMaps() < gameFocus.ax + GetWidthArea() / 2) display.offsetX = GetWidthMaps() - GetWidthArea();
    else display.offsetX = gameFocus.ax - GetWidthArea() / 2;

    if(gameFocus.ay < GetHeightArea() / 2) display.offsetY = 0;
    else if(GetHeightMaps() < gameFocus.ay + GetHeightArea() / 2) display.offsetY = GetHeightMaps() - GetHeightArea();
    else display.offsetY = gameFocus.ay - GetHeightArea() / 2;
}
