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
#include "loadgame.h"
#include "mp2maps.h"

#define  MONSTERFIXTURE 26;

void	ShowStaticMainDisplay(void);
void	RedrawMapsArea(void);
void	DrawRectAreaMaps(SDL_Rect *);
void	DrawCellAreaMapsMonster(Uint8, Uint8);
void	DrawCellAreaMapsTile(Uint8, Uint8);
void	DrawCellAreaMapsLevel1(Uint8, Uint8);
void	DrawCellAreaMapsLevel2(Uint8, Uint8);
void	DrawCellStaticAnimation(Uint8, Uint8);
Uint32	RedrawMapsAnimation(Uint32, void *);

ACTION ActionGAMELOOP(INTERFACEACTION *);

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
SDL_Surface	*frameAreaLeft = NULL;
SDL_Surface	*frameAreaBottom = NULL;

SDL_TimerID	timerAnime = NULL;

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

    // включаем анимацию
    timerAnime = SDL_AddTimer(GetIntValue(ANIMATIONDELAY) * 10, RedrawMapsAnimation, NULL);

    // идем в цикл сообщений
    ACTION result;

    while(! (EXIT == (result = ActionGAMELOOP(stpemaindisplay)) || (ESC == result && YES == MessageBox("Are you sure you want to\n\t\t\t quit?", FONT_BIG))) );

    SDL_RemoveTimer(timerAnime);

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
    }else if(x == GetAreaWidth() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
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
    }else if(x == GetAreaWidth() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
	src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
    }

    SDL_BlitSurface(ptrCell->monster->surface[1], &src, video, &dest);
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

Uint32 RedrawMapsAnimation(Uint32 interval, void *param){

    if(! GetIntValue(ANIMATION)) return interval;
    if(! GetIntValue(ANIM2)) return interval;

    static Uint32 animationFrame = 0;
    Uint8 x, y;

    S_CELLMAPS	*ptrCell = NULL;
    S_ANIMATION *ptrFrame = NULL;
    SDL_Surface	*video = SDL_GetVideoSurface();

    SDL_Rect dest, src;
    Uint8 frame;

    Sint32 cx, cy;

    SDL_GetMouseState(&cx, &cy);
    
    for(y = 0; y < GetAreaHeight(); ++y)

	for(x = 0; x < GetAreaWidth(); ++x){

	    ptrCell = GetCELLMAPS((display.offsetY + y) * GetWidthMaps() + display.offsetX + x);

	    if(! ptrCell->animation && ! ptrCell->monster) continue;

	    dest.w = TILEWIDTH + CURSOR_WIDTH;
	    dest.h = TILEWIDTH + CURSOR_HEIGHT;

	    // если курсор над клеткой с анимацией то выключаем
	    if(BORDERWIDTH + x * TILEWIDTH < CURSOR_WIDTH) dest.x = 0; else dest.x = BORDERWIDTH + x * TILEWIDTH - CURSOR_WIDTH;
	    if(BORDERWIDTH + y * TILEWIDTH < CURSOR_HEIGHT) dest.y = 0; else dest.y = BORDERWIDTH + y * TILEWIDTH - CURSOR_HEIGHT;
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
		if(x < GetAreaWidth() - 1){
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
		    }else if(x == GetAreaWidth() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
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
		    }else if(x == GetAreaWidth() - 1 && dest.w > TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x){
		        src.w = TILEWIDTH + BORDERWIDTH + x * TILEWIDTH - dest.x;
		    }

		    SDL_BlitSurface(ptrFrame->surface[frame], &src, video, &dest);

		    ptrFrame = (S_ANIMATION *) ptrFrame->next;
		}

		// перерисовать клетки
		// справа - тень монстра
		if(x) DrawCellAreaMapsLevel2(x - 1, y);
		// слева - части монстра
		if(x < GetAreaWidth() - 1) DrawCellAreaMapsLevel2(x + 1, y);
		// сверху - части монстра
		if(y) DrawCellAreaMapsLevel2(x, y - 1);
	    }

	    // все верхние объекты
	    DrawCellAreaMapsLevel2(x, y);
    }

    SDL_Flip(video);

    CursorOn();

    ++animationFrame;

    return interval;
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

    return NONE;
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
    result = ActionCycle(dialogInfo);

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
    result = ActionCycle(dialogMenu);

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
    result = ActionCycle(dialogSettings);

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

ACTION ActionGAMELOOP(INTERFACEACTION *action){

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

    // цикл по событиям
    while(exit == NONE){

	while(SDL_PollEvent(&event))

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
			    ptr = stpemaindisplay;
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
			    ptr = stpemaindisplay;
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

		default:
    		    break;
	    }

	ptr = stpemaindisplay;

	while(ptr){

	    if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, event.motion.x, event.motion.y)){

		if(ptr->cursorMotion) SetCursor(ptr->cursorMotion);
		if(ptr->pf) exit = (*ptr->pf)();
	    
	    }

    	    ptr = (INTERFACEACTION *) ptr->next;
	}

	if(CYCLEDELAY) SDL_Delay(CYCLEDELAY);
    }

    return exit;
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

