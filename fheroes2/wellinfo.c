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
#include "element.h"
#include "castle.h"
#include "recrutmonster.h"
#include "monster.h"
#include "wellinfo.h"

ACTION	ActionWellClickMonster1(void);
ACTION	ActionWellClickMonster2(void);
ACTION	ActionWellClickMonster3(void);
ACTION	ActionWellClickMonster4(void);
ACTION	ActionWellClickMonster5(void);
ACTION	ActionWellClickMonster6(void);
void	RedrawInfoAvailable(void);

#define STRLEN		32
#define	WELLINFOWIDTH	640
#define	WELLINFOHEIGHT	480

SDL_Surface	*backWellInfo = NULL;

ACTION ShowWellInfo(void){

    CursorOff();
    SetIntValue(ANIM3, FALSE);

    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy, mx, my;
    Uint8 level;
    AGGSPRITE sprite;
    const char *icnname = NULL;
    const char *message = NULL;
    char str[STRLEN + 1];
    BOOL exit = FALSE;

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    ACTION result = NONE;

    const S_CASTLE *castle = GetCurrentCastle();
    const S_MONSTER *monster = NULL;
    backWellInfo = NULL;

    FillSPRITE(&sprite, "WELLBKG.ICN", 0);
    image = GetICNSprite(&sprite);

    // отрисовка диалога по центру экрана
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
        rectBack.x = video->w / 2 - 320 - BORDERWIDTH - SHADOWWIDTH;
        rectBack.y = video->h / 2 - 240 - BORDERWIDTH;
	rectBack.w = 640 + 2 * BORDERWIDTH + SHADOWWIDTH;
        rectBack.h = 480 + 2 * BORDERWIDTH + SHADOWWIDTH;
    }else{
        rectBack.x = 0;
        rectBack.y = 0;
        rectBack.w = 640;
        rectBack.h = 480;
    }

    // сохраняем бакгроунд
    if(NULL == (back = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
	fprintf(stderr, "WellInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
	return 0;
    }
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // отображаем фон диалога
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);
    rectCur.x = 0;
    rectCur.y = 0;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(GetIntValue(VIDEOMODE)){
        rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH;
        rectCur.y = rectBack.y + BORDERWIDTH;
    }
    SDL_FillRect(video, &rectCur, 0);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y;

    // Text
    message = "Town Population Information and Statistics";
    rectCur.x = video->w / 2;
    rectCur.y = video->h / 2 + 240 - BORDERWIDTH;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_BIG) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_BIG);
    rectCur.h = FONT_HEIGHTBIG;
    PrintText(video, &rectCur, message, FONT_BIG);
                            
    // кнопка Exit
    FillSPRITE(&sprite, "WELLXTRA.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 578;
    rectCur.y = cy + 461;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "WELLXTRA.ICN", 0);
    FillSPRITE(&action.objectPush, "WELLXTRA.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    switch(castle->race){

        default:
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

    // LEVEL 1
    level = 1;
    mx = cx;
    my = cy;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    FillSPRITE(&sprite, icnname, 19);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    message = GetStringBuilding(castle->race, DWELLING_MONSTER1);
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER1){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 12;
	rectCur.y = cy + 12;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster1;
	AddActionEvent(&dialog, &action);
    }

    // LEVEL 2
    level = 2;
    mx = cx;
    my = cy + 150;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    castle->building & DWELLING_UPGRADE2 ? FillSPRITE(&sprite, icnname, 23 + level) : FillSPRITE(&sprite, icnname, 18 + level);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    message = (castle->building & DWELLING_UPGRADE2 ? GetStringBuilding(castle->race, DWELLING_UPGRADE2) : GetStringBuilding(castle->race, DWELLING_MONSTER2));
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER2 || castle->building & DWELLING_UPGRADE2){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 12;
	rectCur.y = cy + 160;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster2;
	AddActionEvent(&dialog, &action);
    }

    // LEVEL 3
    level = 3;
    mx = cx;
    my = cy + 300;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    castle->building & DWELLING_UPGRADE3 ? FillSPRITE(&sprite, icnname, 23 + level) : FillSPRITE(&sprite, icnname, 18 + level);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    message = (castle->building & DWELLING_UPGRADE3 ? GetStringBuilding(castle->race, DWELLING_UPGRADE3) : GetStringBuilding(castle->race, DWELLING_MONSTER3));
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER3 || castle->building & DWELLING_UPGRADE3){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 12;
	rectCur.y = cy + 312;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster3;
	AddActionEvent(&dialog, &action);
    }

    // LEVEL 4
    level = 4;
    mx = cx + 315;
    my = cy;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    castle->building & DWELLING_UPGRADE4 ? FillSPRITE(&sprite, icnname, 23 + level) : FillSPRITE(&sprite, icnname, 18 + level);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    message = (castle->building & DWELLING_UPGRADE4 ? GetStringBuilding(castle->race, DWELLING_UPGRADE4) : GetStringBuilding(castle->race, DWELLING_MONSTER4));
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER4 || castle->building & DWELLING_UPGRADE4){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 330;
	rectCur.y = cy + 12;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster4;
	AddActionEvent(&dialog, &action);
    }

    // LEVEL 5
    level = 5;
    mx = cx + 315;
    my = cy + 150;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    castle->building & DWELLING_UPGRADE5 ? FillSPRITE(&sprite, icnname, 23 + level) : FillSPRITE(&sprite, icnname, 18 + level);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    message = (castle->building & DWELLING_UPGRADE5 ? GetStringBuilding(castle->race, DWELLING_UPGRADE5) : GetStringBuilding(castle->race, DWELLING_MONSTER5));
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER5 || castle->building & DWELLING_UPGRADE5){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 330;
	rectCur.y = cy + 160;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster5;
	AddActionEvent(&dialog, &action);
    }

    // LEVEL 6
    level = 6;
    mx = cx + 315;
    my = cy + 300;
    monster = GetStatMonster(GetMonsterFromCastle(castle, level));
    // image dwelling
    if(castle->building & DWELLING_UPGRADE7 && WARLOCK == castle->race)
	FillSPRITE(&sprite, icnname, 24 + level);
    else if(castle->building & DWELLING_UPGRADE6)
	FillSPRITE(&sprite, icnname, 23 + level);
    else
	FillSPRITE(&sprite, icnname, 18 + level);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 21;
    rectCur.y = my + 35;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text dwelling
    if(castle->building & DWELLING_UPGRADE7 && WARLOCK == castle->race)
	message = GetStringBuilding(castle->race, DWELLING_UPGRADE7);
    else if(castle->building & DWELLING_UPGRADE6)
	message = GetStringBuilding(castle->race, DWELLING_UPGRADE6);
    else
	message = GetStringBuilding(castle->race, DWELLING_MONSTER6);
    rectCur.x = mx + 86;
    rectCur.y = my + 106;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // image monster
    FillSPRITE(&sprite, MonsterBigNamePortrait(GetMonsterFromCastle(castle, level)), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = mx + 195 - image->w / 2;
    rectCur.y = my + 130 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    //text monster
    message = GetStringMonster(GetMonsterFromCastle(castle, level));
    rectCur.x = mx + 120;
    rectCur.y = my + 21;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // attack
    snprintf(str, STRLEN, "Attack: %d", monster->attack);
    rectCur.x = mx + 270;
    rectCur.y = my + 24;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // defense
    snprintf(str, STRLEN, "Defense: %d", monster->defence);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // damage
    snprintf(str, STRLEN, "Damg: %d-%d", monster->damageMin, monster->damageMax);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 2;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // hp
    snprintf(str, STRLEN, "HP: %d", monster->hp);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 3;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    // speed
    snprintf(str, STRLEN, "Speed:");
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 5;
    rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(str, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, str, FONT_SMALL);
    message = GetStringSpeed(monster->speed);
    rectCur.x = mx + 270;
    rectCur.y = my + 24 + FONT_HEIGHTSMALL * 6;
    rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
    rectCur.y = rectCur.y - 3;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // growth
    if(castle->building & DWELLING_MONSTER6 || castle->building & DWELLING_UPGRADE6 || castle->building & DWELLING_UPGRADE7){
	snprintf(str, STRLEN, "Growth");
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 8;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	snprintf(str, STRLEN, "+ %d / week", GetMonsterGrownCastle(castle, GetMonsterFromCastle(castle, level)));
	rectCur.x = mx + 270;
	rectCur.y = my + 24 + FONT_HEIGHTSMALL * 9;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, str, FONT_SMALL);
	// action
	rectCur.x = cx + 330;
	rectCur.y = cy + 312;
	rectCur.w = 300;
	rectCur.h = 135;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionWellClickMonster6;
	AddActionEvent(&dialog, &action);
    }

    // сохраняем бакгроунд
    if(NULL == (backWellInfo = SDL_CreateRGBSurface(SDL_SWSURFACE, WELLINFOWIDTH, WELLINFOHEIGHT, 16, 0, 0, 0, 0))){
        fprintf(stderr, "ShowWellInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
        return NONE;
    }
    rectCur.x = (video->w - WELLINFOWIDTH) / 2;
    rectCur.y = (video->h - WELLINFOHEIGHT) / 2;
    rectCur.w = WELLINFOWIDTH;
    rectCur.h = WELLINFOHEIGHT;
    SDL_BlitSurface(video, &rectCur, backWellInfo, NULL);

    RedrawInfoAvailable();

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionCycle(dialog, NULL)){

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

    FreeActionEvent(dialog);
    SDL_FreeSurface(backWellInfo);
    SDL_FreeSurface(back);

    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

void RedrawInfoAvailable(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rectCur;
    Uint16 cx, cy, mx, my;
    Uint8 level;
    const char *message = NULL;
    char str[STRLEN + 1];
    const S_CASTLE *castle = GetCurrentCastle();

    if(GetIntValue(VIDEOMODE)){
        cx = video->w / 2 - 320;
        cy = video->h / 2 - 240;
    }else{
        cx = 0;
        cy = 0;
    }

    CursorOff();

    // restore back
    rectCur.x = (video->w - WELLINFOWIDTH) / 2;
    rectCur.y = (video->h - WELLINFOHEIGHT) / 2;
    rectCur.w = WELLINFOWIDTH;
    rectCur.h = WELLINFOHEIGHT;
    SDL_BlitSurface(backWellInfo, NULL, video, &rectCur);

    // available level 1
    level = 1;
    mx = cx;
    my = cy;
    if(castle->building & DWELLING_MONSTER1 && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
    // available level 2
    level = 2;
    mx = cx;
    my = cy + 150;
    if((castle->building & DWELLING_MONSTER2 || castle->building & DWELLING_UPGRADE2) && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
    // available level 3
    level = 3;
    mx = cx;
    my = cy + 300;
    if((castle->building & DWELLING_MONSTER3 || castle->building & DWELLING_UPGRADE3) && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
    // available level 4
    level = 4;
    mx = cx + 315;
    my = cy;
    if((castle->building & DWELLING_MONSTER4 || castle->building & DWELLING_UPGRADE4) && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
    // available level 5
    level = 5;
    mx = cx + 315;
    my = cy + 150;
    if((castle->building & DWELLING_MONSTER5 || castle->building & DWELLING_UPGRADE5) && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
    // available level 6
    level = 6;
    mx = cx + 315;
    my = cy + 300;
    if((castle->building & DWELLING_MONSTER6 || castle->building & DWELLING_UPGRADE6 || castle->building & DWELLING_UPGRADE7) && castle->monster[level - 1]){
	message = "Available:";
	rectCur.x = mx + 70;
	rectCur.y = my + 122;
	rectCur.x = rectCur.x - GetLengthText(message, FONT_SMALL) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(message, FONT_SMALL);
	rectCur.h = FONT_HEIGHTSMALL;
	PrintText(video, &rectCur, message, FONT_SMALL);

        snprintf(str, STRLEN, "%d", castle->monster[level - 1]);
	rectCur.x = mx + 55 + GetLengthText(message, FONT_SMALL);
	rectCur.y = my + 118;
	rectCur.x = rectCur.x - GetLengthText(str, FONT_BIG) / 2;
	rectCur.y = rectCur.y - 3;
	rectCur.w = GetLengthText(str, FONT_BIG);
	rectCur.h = FONT_HEIGHTBIG;
	PrintText(video, &rectCur, str, FONT_BIG);
    }
}

ACTION	ActionWellClickMonster1(void){

    const Uint8 levelMonster = 1;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}

ACTION	ActionWellClickMonster2(void){

    const Uint8 levelMonster = 2;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}

ACTION	ActionWellClickMonster3(void){

    const Uint8 levelMonster = 3;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}

ACTION	ActionWellClickMonster4(void){

    const Uint8 levelMonster = 4;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}

ACTION	ActionWellClickMonster5(void){

    const Uint8 levelMonster = 5;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}

ACTION	ActionWellClickMonster6(void){

    const Uint8 levelMonster = 6;
    const S_CASTLE *castle = GetCurrentCastle();

    if(! castle) return NONE;

    if( BuyMonsterFromCastle(castle, levelMonster,
        DialogRecrutMonster(GetMonsterFromCastle(castle, levelMonster), levelMonster, castle->monster[levelMonster - 1])) )
	RedrawInfoAvailable();

    CursorOn();

    return NONE;
}
