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
#include "gamedefs.h"
#include "config.h"
#include "cursor.h"
#include "tools.h"
#include "debug.h"
#include "armyinfo.h"

#define STRLEN	64
#define NUMLEN	8

ACTION ShowArmyInfo(const S_ARMY *army, const S_HEROES *heroes, const S_CASTLE *castle){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Surface *background = NULL;
    AGGSPRITE sprite;
    const char *icnname = NULL;
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;
    ACTION result = NONE;
    SDL_Rect rectBack, rect;
    char str[STRLEN + 1];
    char num[NUMLEN + 1];
    BOOL exit;

    if(MONSTERNONE == army->monster) return NONE;
    if(GetIntValue(EVILINTERFACE)){ icnname = "VIEWARME.ICN"; }else{  icnname = "VIEWARMY.ICN"; }

    CursorOff();
    SetIntValue(ANIM3, FALSE);

    Uint32 cursor = GetCursor();

    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    rectBack.x = video->w / 2 - image->w / 2;
    rectBack.y = video->h / 2 - image->h / 2;
    rectBack.w = image->w;
    rectBack.h = image->h;
    
    // сохраняем background
    if(NULL == (background = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "ShowArmyInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
        return NONE;
    }
    SDL_BlitSurface(video, &rectBack, background, NULL);

    // рисуем картинку диалога
    SDL_BlitSurface(image, NULL, video , &rectBack);

    // рисуем картинку монстра
    S_MONSTER *monster = GetStatMonster(army->monster);
    FillSPRITE(&sprite, MonsterBigNamePortrait(army->monster), 0);
    image = GetICNSprite(&sprite);
    rect.x = rectBack.x + 150 - image->w / 2;
    rect.y = rectBack.y + 100;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video , &rect);

    // наименование
    rect.x = rectBack.x + 120;
    rect.y = rectBack.y + 35;
    rect.w = GetLengthText(monster->descriptions, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    PrintText(video, &rect, monster->descriptions, FONT_BIG);

    // количество
    snprintf(num, NUMLEN, "%5d", army->count);
    rect.x = rectBack.x + 120;
    rect.y = rectBack.y + 224;
    rect.w = GetLengthText(num, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    PrintText(video, &rect, num, FONT_BIG);

    // attack
    snprintf(str, STRLEN, "%s: %d", GetStringAttackSkill(), monster->attack);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // defence
    snprintf(str, STRLEN, "%s: %d", GetStringDefenseSkill(), monster->defence);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG + FONT_HEIGHTBIG / 4;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // shot
    snprintf(str, STRLEN, "Shots: %d", monster->shots);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 2 + FONT_HEIGHTBIG / 4 * 2;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // damage
    snprintf(str, STRLEN, "Damage: %d - %d", monster->damageMin, monster->damageMax);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 3 + FONT_HEIGHTBIG / 4 * 3;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // hp
    snprintf(str, STRLEN, "Hit Points: %d", monster->hp);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 4 + FONT_HEIGHTBIG / 4 * 4;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // speed
    snprintf(str, STRLEN, "Speed: %s", GetStringSpeed(monster->speed));
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 5 + FONT_HEIGHTBIG / 4 * 5;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // morale
    heroes ? snprintf(str, STRLEN, "Morale: %s", GetStringMorale(CalculateHeroesMorale(heroes))) : snprintf(str, STRLEN, "Morale: Normal");
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 6 + FONT_HEIGHTBIG / 4 * 6;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // luck
    heroes ? snprintf(str, STRLEN, "Luck: %s", GetStringLuck(CalculateHeroesLuck(heroes))) : snprintf(str, STRLEN, "Luck: Normal");
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 7 + FONT_HEIGHTBIG / 4 * 7;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // click upgrade
    switch(UpgradableArmy(army, GetIntValue(HUMANCOLORS), castle)){
    
	case YES:
	    FillSPRITE(&sprite, icnname, 5);
	    image = GetICNSprite(&sprite);
	    rect.x = rectBack.x + 290;
	    rect.y = rectBack.y + 192;
	    rect.w = image->w;
	    rect.h = image->h;
	    // click
	    ZeroINTERFACEACTION(&action);
	    FillSPRITE(&action.objectUp, icnname, 5);
	    FillSPRITE(&action.objectPush, icnname, 6);
	    action.rect = rect;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionPressUPGRADE;
	    AddActionEvent(&dialog, &action);
	    SDL_BlitSurface(image, NULL, video, &rect);
	    break;
	
	case NO:
	    FillSPRITE(&sprite, icnname, 6);
	    image = GetICNSprite(&sprite);
	    rect.x = rectBack.x + 290;
	    rect.y = rectBack.y + 192;
	    rect.w = image->w;
	    rect.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rect);
	    break;
	
	default:
	    break;
    }

    // click dismiss
    if(heroes && 1 == HeroesCountArmy(heroes)){
	FillSPRITE(&sprite, icnname, 2);
	image = GetICNSprite(&sprite);
	rect.x = rectBack.x + 290;
	rect.y = rectBack.y + 220;
	rect.w = image->w;
	rect.h = image->h;
    }else{
	FillSPRITE(&sprite, icnname, 1);
	image = GetICNSprite(&sprite);
	rect.x = rectBack.x + 290;
	rect.y = rectBack.y + 220;
	rect.w = image->w;
	rect.h = image->h;
	// click
	ZeroINTERFACEACTION(&action);
	FillSPRITE(&action.objectUp, icnname, 1);
	FillSPRITE(&action.objectPush, icnname, 2);
	action.rect = rect;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressDISMISS;
	AddActionEvent(&dialog, &action);
    }
    SDL_BlitSurface(image, NULL, video, &rect);

    // click exit
    FillSPRITE(&sprite, icnname, 3);
    image = GetICNSprite(&sprite);
    rect.x = rectBack.x + 420;
    rect.y = rectBack.y + 220;
    rect.w = image->w;
    rect.h = image->h;
    // click
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, icnname, 3);
    FillSPRITE(&action.objectPush, icnname, 4);
    action.rect = rect;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);
    SetCursor(CURSOR_POINTER);
            
    CursorOn();

    exit = FALSE;
    while(! exit)
        switch(ActionCycle(dialog, NULL)){

            case UPGRADE:
                exit = TRUE;
                result = UPGRADE;
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
        
    CursorOff();

    SDL_BlitSurface(background, NULL, video, &rectBack);
    FreeActionEvent(dialog);
    SDL_FreeSurface(background);

    SetCursor(cursor);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

ACTION ShowArmyInfoRight(const S_ARMY *army, const S_HEROES *heroes){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Surface *background = NULL;
    AGGSPRITE sprite;
    const char *icnname = NULL;
    INTERFACEACTION *dialog = NULL;
    ACTION result = NONE;
    SDL_Rect rectBack, rect;
    SDL_Event event;
    char str[STRLEN + 1];
    char num[NUMLEN + 1];
    BOOL exit;

    if(MONSTERNONE == army->monster) return NONE;
    if(GetIntValue(EVILINTERFACE)){ icnname = "VIEWARME.ICN"; }else{  icnname = "VIEWARMY.ICN"; }

    CursorOff();
    SetIntValue(ANIM3, FALSE);

    Uint32 cursor = GetCursor();

    FillSPRITE(&sprite, icnname, 0);
    image = GetICNSprite(&sprite);
    rectBack.x = video->w / 2 - image->w / 2;
    rectBack.y = video->h / 2 - image->h / 2;
    rectBack.w = image->w;
    rectBack.h = image->h;
    
    // сохраняем background
    if(NULL == (background = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "ShowArmyInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
        return NONE;
    }
    SDL_BlitSurface(video, &rectBack, background, NULL);

    // рисуем картинку диалога
    SDL_BlitSurface(image, NULL, video , &rectBack);

    // рисуем картинку монстра
    S_MONSTER *monster = GetStatMonster(army->monster);
    FillSPRITE(&sprite, MonsterBigNamePortrait(army->monster), 0);
    image = GetICNSprite(&sprite);
    rect.x = rectBack.x + 150 - image->w / 2;
    rect.y = rectBack.y + 100;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video , &rect);

    // наименование
    rect.x = rectBack.x + 120;
    rect.y = rectBack.y + 35;
    rect.w = GetLengthText(monster->descriptions, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    PrintText(video, &rect, monster->descriptions, FONT_BIG);

    // количество
    snprintf(num, NUMLEN, "%5d", army->count);
    rect.x = rectBack.x + 120;
    rect.y = rectBack.y + 224;
    rect.w = GetLengthText(num, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    PrintText(video, &rect, num, FONT_BIG);

    // attack
    snprintf(str, STRLEN, "%s: %d", GetStringAttackSkill(), monster->attack);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // defence
    snprintf(str, STRLEN, "%s: %d", GetStringDefenseSkill(), monster->defence);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG + FONT_HEIGHTBIG / 2;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // shot
    snprintf(str, STRLEN, "Shots: %d", monster->shots);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 2 + FONT_HEIGHTBIG / 2 * 2;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // damage
    snprintf(str, STRLEN, "Damage: %d - %d", monster->damageMin, monster->damageMax);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 3 + FONT_HEIGHTBIG / 2 * 3;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // hp
    snprintf(str, STRLEN, "Hit Points: %d", monster->hp);
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 4 + FONT_HEIGHTBIG / 2 * 4;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // speed
    snprintf(str, STRLEN, "Speed: %s", GetStringSpeed(monster->speed));
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 5 + FONT_HEIGHTBIG / 2 * 5;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // morale
    heroes ? snprintf(str, STRLEN, "Morale: %s", GetStringMorale(CalculateHeroesMorale(heroes))) : snprintf(str, STRLEN, "Morale: Normal");
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 6 + FONT_HEIGHTBIG / 2 * 6;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    // luck
    heroes ? snprintf(str, STRLEN, "Luck: %s", GetStringLuck(CalculateHeroesLuck(heroes))) : snprintf(str, STRLEN, "Luck: Normal");
    rect.x = rectBack.x + 400;
    rect.y = rectBack.y + 35 + FONT_HEIGHTBIG * 7 + FONT_HEIGHTBIG / 2 * 7;
    rect.w = GetLengthText(str, FONT_BIG);
    rect.h = FONT_HEIGHTBIG;
    rect.x = rect.x - rect.w / 2;
    PrintText(video, &rect, str, FONT_BIG);

    SDL_Flip(video);
    SetCursor(CURSOR_POINTER);
            
    CursorOn();

    exit = FALSE;
            
    while(! exit){
            
        while(SDL_PollEvent(&event))
            if( SDL_BUTTON_RIGHT == event.button.button && SDL_RELEASED == event.button.state) exit = TRUE;
            
        if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY * 10);
    }
                    
    CursorOff();

    SDL_BlitSurface(background, NULL, video, &rectBack);
    FreeActionEvent(dialog);
    SDL_FreeSurface(background);

    SetCursor(cursor);
    SetIntValue(ANIM3, TRUE);
    CursorOn();

    return result;
}

