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
#include "tools.h"
#include "draw.h"
#include "cursor.h"
#include "debug.h"
#include "castle.h"
#include "config.h"
#include "element.h"
#include "actionevent.h"
#include "armyinfo.h"
#include "castle.h"
#include "monster.h"
#include "selector.h"
#include "magicbook.h"

#include "heroesinfo.h"

#define STRLEN	64
#define NUMLEN	8

ACTION	ActionHeroesInfoLClickPrimarySkill(void);
ACTION	ActionHeroesInfoRClickPrimarySkill(void);
ACTION	ActionHeroesInfoOverPrimarySkill(void);
ACTION	ActionHeroesInfoLClickSecondarySkill(void);
ACTION	ActionHeroesInfoRClickSecondarySkill(void);
ACTION	ActionHeroesInfoOverSecondarySkill(void);
ACTION	ActionHeroesInfoLClickMorale(void);
ACTION	ActionHeroesInfoRClickMorale(void);
ACTION	ActionHeroesInfoOverMorale(void);
ACTION	ActionHeroesInfoLClickLuck(void);
ACTION	ActionHeroesInfoRClickLuck(void);
ACTION	ActionHeroesInfoOverLuck(void);
ACTION	ActionHeroesInfoLClickSpreadFormat(void);
ACTION	ActionHeroesInfoLClickGroupedFormat(void);
ACTION	ActionHeroesInfoRClickSpreadFormat(void);
ACTION	ActionHeroesInfoRClickGroupedFormat(void);
ACTION	ActionHeroesInfoOverSpreadFormat(void);
ACTION	ActionHeroesInfoOverGroupedFormat(void);
ACTION	ActionHeroesInfoLClickExperience(void);
ACTION	ActionHeroesInfoRClickExperience(void);
ACTION	ActionHeroesInfoOverExperience(void);
ACTION	ActionHeroesInfoLClickPoints(void);
ACTION	ActionHeroesInfoRClickPoints(void);
ACTION	ActionHeroesInfoOverPoints(void);
ACTION	ActionHeroesInfoLClickArmy(void);
ACTION	ActionHeroesInfoRClickArmy(void);
ACTION	ActionHeroesInfoOverArmy(void);
ACTION	ActionHeroesInfoLClickArtifact(void);
ACTION	ActionHeroesInfoRClickArtifact(void);
ACTION	ActionHeroesInfoOverArtifact(void);

ACTION	ActionHeroesInfoLOOP(INTERFACEACTION *);

void	PrintHeroesInfoPrompt(const char *);
void	RedrawHeroesInfoMonster(void);

S_HEROES	*heroes = NULL;
S_SELECT 	*selectHeroesFormat = NULL;
S_SELECT 	*selectHeroesArmy = NULL;

ACTION ShowHeroesInfo(E_NAMEHEROES name, BOOL ro){

    CursorOff();
    SetIntValue(ANIM2, FALSE);
    SetIntValue(ANIM3, FALSE);
    
    SDL_Surface *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    Uint16 cx, cy;
    AGGSPRITE sprite;
    BOOL exit = FALSE;
    
    Uint8 count = 0;
    Uint8 i;
    char str[STRLEN + 1];
    char num[NUMLEN + 1];

    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        
    ACTION result;

    video = SDL_GetVideoSurface();
    heroes = GetStatHeroes(name);

    // init cursor format
    FillSPRITE(&sprite, "HSICONS.ICN", 11);
    selectHeroesFormat = InitSelector(&sprite, 0, 0, FALSE);

    // init cursor army
    FillSPRITE(&sprite, "STRIP.ICN", 1);
    selectHeroesArmy = InitSelector(&sprite, video->w / 2 - 164, 88, FALSE);

    FillSPRITE(&sprite, "HEROBKG.ICN", 0);
    image = GetICNSprite(&sprite);

    // отрисовка диалога по центру экрана
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
	fprintf(stderr, "HeroesInfo: CreateRGBSurface failed: %s\n", SDL_GetError());
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

    // признак интерфейса evil
    GetIntValue(EVILINTERFACE) ? FillSPRITE(&sprite, "HEROEXTE.ICN", 0) : FillSPRITE(&sprite, "HEROEXTG.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // prev heroes
    FillSPRITE(&sprite, "HSBTNS.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 480 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(! ro){
	ZeroINTERFACEACTION(&action);
	FillSPRITE(&action.objectUp, "HSBTNS.ICN", 4);
	FillSPRITE(&action.objectPush, "HSBTNS.ICN", 5);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressLEFT;
        AddActionEvent(&dialog, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }
    // next heroes
    FillSPRITE(&sprite, "HSBTNS.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 640 - image->w;
    rectCur.y = cy + 480 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(! ro){
	ZeroINTERFACEACTION(&action);
	FillSPRITE(&action.objectUp, "HSBTNS.ICN", 6);
	FillSPRITE(&action.objectPush, "HSBTNS.ICN", 7);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressLEFT;
	AddActionEvent(&dialog, &action);
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }

    PrintHeroesInfoPrompt(NULL);

    // портрет героя PORT00XX
    FillSPRITE(&sprite, HeroesBigNamePortrait(name), 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 49;
    rectCur.y = cy + 31;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // имя героя
    snprintf(str, STRLEN, "%s the %s ( Level %d )", HeroesGetStringName(name), GetStringRace(heroes->race), GetHeroesLevel(heroes));
    rectCur.x = cx + 45;
    rectCur.y = cy;
    rectCur.w = 550;
    rectCur.h = 18;
    PrintAlignText(video, &rectCur, str, FONT_BIG);

    // attack skill
    rectCur.x = cx + 156;
    rectCur.y = cy + 30;
    rectCur.w = 80;
    rectCur.h = 92;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverPrimarySkill;
    AddActionEvent(&dialog, &action);
    rectCur.y += 3;
    PrintAlignText(video, &rectCur, GetStringAttackSkill(), FONT_SMALL);
    snprintf(num, NUMLEN, "%d", CalculateHeroesAttack(heroes));
    rectCur.y += 72;
    PrintAlignText(video, &rectCur, num, FONT_BIG);    
    // defense skill
    rectCur.x = cx + 156 + 1 * (8 + 80);
    rectCur.y = cy + 30;
    rectCur.w = 80;
    rectCur.h = 92;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverPrimarySkill;
    AddActionEvent(&dialog, &action);
    rectCur.y += 3;
    PrintAlignText(video, &rectCur, GetStringDefenseSkill(), FONT_SMALL);
    snprintf(num, NUMLEN, "%d", CalculateHeroesDefence(heroes));
    rectCur.y += 72;
    PrintAlignText(video, &rectCur, num, FONT_BIG);    
    // spell power
    rectCur.x = cx + 156 + 2 * (8 + 80);
    rectCur.y = cy + 30;
    rectCur.w = 80;
    rectCur.h = 92;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverPrimarySkill;
    AddActionEvent(&dialog, &action);
    rectCur.y += 3;
    PrintAlignText(video, &rectCur, GetStringSpellPower(), FONT_SMALL);
    snprintf(num, NUMLEN, "%d", CalculateHeroesPower(heroes));
    rectCur.y += 72;
    PrintAlignText(video, &rectCur, num, FONT_BIG);    
    // knowledge
    rectCur.x = cx + 156 + 3 * (8 + 80);
    rectCur.y = cy + 30;
    rectCur.w = 80;
    rectCur.h = 92;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickPrimarySkill;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverPrimarySkill;
    AddActionEvent(&dialog, &action);
    rectCur.y += 3;
    PrintAlignText(video, &rectCur, GetStringKnowledge(), FONT_SMALL);
    snprintf(num, NUMLEN, "%d", CalculateHeroesKnowledge(heroes));
    rectCur.y += 72;
    PrintAlignText(video, &rectCur, num, FONT_BIG);

    // morale
    switch(CalculateHeroesMorale(heroes)){
	case MORALE_TREASON:
	    count = 3;
            FillSPRITE(&sprite, "HSICONS.ICN", 5);
	    break;
        case MORALE_AWFUL:
	    count = 2;
            FillSPRITE(&sprite, "HSICONS.ICN", 5);
	    break;
        case MORALE_POOR:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 5);
	    break;
        case MORALE_NORMAL:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 7);
	    break;
        case MORALE_GOOD:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 4);
	    break;
        case MORALE_GREAT:
	    count = 2;
            FillSPRITE(&sprite, "HSICONS.ICN", 4);
	    break;
        case MORALE_BLOOD:
	    count = 3;
            FillSPRITE(&sprite, "HSICONS.ICN", 4);
	    break;
    }
    rectCur.x = cx + 515;
    rectCur.y = cy + 35;
    rectCur.w = 34;
    rectCur.h = 26;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickMorale;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickMorale;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverMorale;
    AddActionEvent(&dialog, &action);
    image = GetICNSprite(&sprite);
    rectCur.y += (26 - image->h) / 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(1 == count){
	rectCur.x += (34 - image->w) / 2;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }else
	for(i = 0; i < count; ++i){
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    rectCur.x += (count == 2 ? 7 : 5);
    }

    // luck
    switch(CalculateHeroesLuck(heroes)){
	case LUCK_CURSED:
	    count = 3;
            FillSPRITE(&sprite, "HSICONS.ICN", 3);
	    break;
        case LUCK_AWFUL:
	    count = 2;
            FillSPRITE(&sprite, "HSICONS.ICN", 3);
	    break;
        case LUCK_BAD:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 3);
	    break;
        case LUCK_NORMAL:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 6);
	    break;
        case LUCK_GOOD:
	    count = 1;
            FillSPRITE(&sprite, "HSICONS.ICN", 2);
	    break;
        case LUCK_GREAT:
	    count = 2;
            FillSPRITE(&sprite, "HSICONS.ICN", 2);
	    break;
        case LUCK_IRISH:
	    count = 3;
            FillSPRITE(&sprite, "HSICONS.ICN", 2);
	    break;
    }
    rectCur.x = cx + 552;
    rectCur.y = cy + 35;
    rectCur.w = 34;
    rectCur.h = 26;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickLuck;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickLuck;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverLuck;
    AddActionEvent(&dialog, &action);
    image = GetICNSprite(&sprite);
    rectCur.y += (26 - image->h) / 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(1 == count){
	rectCur.x += (34 - image->w) / 2;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }else
	for(i = 0; i < count; ++i){
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	    rectCur.x += (count == 2 ? 7 : 5);
    }

    // army format spread
    FillSPRITE(&sprite, "HSICONS.ICN", 9);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 515;
    rectCur.y = cy + 63;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    if(! ro){
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionHeroesInfoLClickSpreadFormat;
	AddActionEvent(&dialog, &action);
    }
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickSpreadFormat;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverSpreadFormat;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    if(SPREAD == heroes->af && !ro){
	rectCur.x = rectCur.x - 1;
	rectCur.y = rectCur.y - 1;
	RedrawSelector(selectHeroesFormat, &rectCur);
	CursorOff();
    }

    // army format grouped
    FillSPRITE(&sprite, "HSICONS.ICN", 10);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 552;
    rectCur.y = cy + 63;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    if(! ro){
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionHeroesInfoLClickGroupedFormat;
	AddActionEvent(&dialog, &action);
    }
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickGroupedFormat;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverGroupedFormat;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    if(GROUPED == heroes->af && !ro){
	rectCur.x = rectCur.x - 1;
	rectCur.y = rectCur.y - 1;
	RedrawSelector(selectHeroesFormat, &rectCur);
	CursorOff();
    }

    // exp
    FillSPRITE(&sprite, "HSICONS.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 512;
    rectCur.y = cy + 85;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickExperience;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickExperience;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverExperience;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    rectCur.x = cx + 513;
    rectCur.y = cy + 107;
    rectCur.w = 32;
    rectCur.h = FONT_HEIGHTSMALL;
    snprintf(num, NUMLEN, "%d", heroes->experience);
    PrintAlignText(video, &rectCur, num, FONT_SMALL);

    // points
    FillSPRITE(&sprite, "HSICONS.ICN", 8);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 550;
    rectCur.y = cy + 87;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionHeroesInfoLClickPoints;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_RCLICK;
    action.pf = ActionHeroesInfoRClickPoints;
    AddActionEvent(&dialog, &action);
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionHeroesInfoOverPoints;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);
    rectCur.x = cx + 551;
    rectCur.y = cy + 107;
    rectCur.w = 32;
    rectCur.h = FONT_HEIGHTSMALL;
    snprintf(num, NUMLEN, "%d/%d", heroes->magicPoint, CalculateHeroesMagicPoint(heroes));
    PrintAlignText(video, &rectCur, num, FONT_SMALL);

    // знак
    switch(heroes->color){
        default:
        case BLUE:
            FillSPRITE(&sprite, "CREST.ICN", 0);
            break;
        case GREEN:
            FillSPRITE(&sprite, "CREST.ICN", 1);
            break;
        case RED:
            FillSPRITE(&sprite, "CREST.ICN", 2);
            break;
        case YELLOW:
            FillSPRITE(&sprite, "CREST.ICN", 3);
    	    break;
        case ORANGE:
            FillSPRITE(&sprite, "CREST.ICN", 4);
    	    break;
        case PURPLE:
            FillSPRITE(&sprite, "CREST.ICN", 5);
            break;
    }
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 49;
    rectCur.y = cy + 130;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // фон монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.y = cy + 130;
    rectCur.w = image->w;
    rectCur.h = image->h;
    for(i = 0; i < HEROESMAXARMY; i++){
	rectCur.x = cx + 156 + i * (rectCur.w + 6);
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	if(!ro){
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionHeroesInfoLClickArmy;
	    AddActionEvent(&dialog, &action);
	}
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionHeroesInfoRClickArmy;
	AddActionEvent(&dialog, &action);
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionHeroesInfoOverArmy;
	AddActionEvent(&dialog, &action);
    }
    RedrawHeroesInfoMonster();

    // фон спец SECSKILL
    for(i = 0; i < HEROESMAXSKILL; i++){

	switch(heroes->skill[i].type){
	    case SKILLNONE:
		count = 0;
		break;
	    case PATHFINDING:
		count = 1;
		break;
	    case ARCHERY:
		count = 2;
		break;
	    case LOGISTICS:
		count = 3;
		break;
	    case SCOUTING:
		count = 4;
		break;
	    case DIPLOMACY:
		count = 5;
		break;
	    case NAVIGATION:
		count = 6;
		break;
	    case LEADERSHIP:
		count = 7;
		break;
	    case WISDOM:
		count = 8;
		break;
	    case MYSTICISM:
		count = 9;
		break;
	    case LUCK:
		count = 10;
		break;
	    case BALLISTICS:
		count = 11;
		break;
	    case EAGLEEYE:
		count = 12;
		break;
	    case NECROMANCY:
		count = 13;
		break;
	    case ESTATES:
		count = 14;
		break;
	}
	// image
	FillSPRITE(&sprite, "SECSKILL.ICN", count);
	image = GetICNSprite(&sprite);
	rectCur.y = cy + 233;
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = cx + 3 + i * (rectCur.w + 5);
	SDL_BlitSurface(image, NULL, video, &rectCur);
	if(heroes->skill[i].type){
	    // action
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionHeroesInfoLClickSecondarySkill;
	    AddActionEvent(&dialog, &action);
	    action.mouseEvent = MOUSE_RCLICK;
	    action.pf = ActionHeroesInfoRClickSecondarySkill;
	    AddActionEvent(&dialog, &action);
	    action.mouseEvent = MOUSE_PRESENT;
	    action.pf = ActionHeroesInfoOverSecondarySkill;
	    AddActionEvent(&dialog, &action);
	    // text skill
	    PrintAlignText(video, &rectCur, GetStringSkill(heroes->skill[i].type), FONT_SMALL);
	    // text level
	    rectCur.y += 50;
	    PrintAlignText(video, &rectCur, GetStringLevelSkill(heroes->skill[i].level), FONT_SMALL);
	}
    }

    // ARTIFACT
    for(i = 0; i < HEROESMAXARTIFACT; i++){
	// sprite
	FillSPRITE(&sprite, "ARTIFACT.ICN", heroes->artifact[i]);
	image = GetICNSprite(&sprite);
	rectCur.y = (i < HEROESMAXARTIFACT / 2 ? cy + 308 : cy + 387);
	rectCur.w = image->w;
	rectCur.h = image->h;
	rectCur.x = (i < HEROESMAXARTIFACT / 2 ? cx + 51 + i * (rectCur.w + 15) : cx + 51 + (i - HEROESMAXARTIFACT / 2) * (rectCur.w + 15));
	SDL_BlitSurface(image, NULL, video, &rectCur);
	// action
	if(heroes->artifact[i]){
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionHeroesInfoLClickArtifact;
	    AddActionEvent(&dialog, &action);
	    action.mouseEvent = MOUSE_RCLICK;
	    action.pf = ActionHeroesInfoRClickArtifact;
	    AddActionEvent(&dialog, &action);
	    action.mouseEvent = MOUSE_PRESENT;
	    action.pf = ActionHeroesInfoOverArtifact;
	    AddActionEvent(&dialog, &action);
	}
    }

    // кнопка Dismiss
    if(ro){
	FillSPRITE(&sprite, "HSBTNS.ICN", 1);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 5;
	rectCur.y = cy + 318;
	rectCur.w = image->w;
	rectCur.h = image->h;
    }else{
	FillSPRITE(&sprite, "HSBTNS.ICN", 0);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 5;
	rectCur.y = cy + 318;
	rectCur.w = image->w;
	rectCur.h = image->h;
	ZeroINTERFACEACTION(&action);
	FillSPRITE(&action.objectUp, "HSBTNS.ICN", 0);
	FillSPRITE(&action.objectPush, "HSBTNS.ICN", 1);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionPressDISMISS;
	AddActionEvent(&dialog, &action);
    }
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // кнопка Exit
    FillSPRITE(&sprite, "HSBTNS.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 603;
    rectCur.y = cy + 318;
    rectCur.w = image->w;
    rectCur.h = image->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "HSBTNS.ICN", 2);
    FillSPRITE(&action.objectPush, "HSBTNS.ICN", 3);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCANCEL;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // Отрисовка диалога
    CursorOn();

    // цикл событий
    while(! exit)

        switch(ActionHeroesInfoLOOP(dialog)){

    	    case EXIT:
    		exit = TRUE;
    		result = EXIT;
    		break;

    	    case ESC:
    	    case CANCEL:
    		exit = TRUE;
    		result = NONE;
    		break;

    	    case DISMISS:
    		exit = TRUE;
    		result = DISMISS;
    		break;

    	    default:
		result = NONE;
    		break;

	}

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);
    FreeSelector(selectHeroesFormat);
    FreeSelector(selectHeroesArmy);

    SetIntValue(ANIM3, TRUE);
    SetIntValue(ANIM2, TRUE);
    CursorOn();

    return result;
}

ACTION ActionHeroesInfoLClickPrimarySkill(void){

    Sint32 mx, my;
    const char *header = NULL;
    const char *string = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;
    
    switch(index){
	case 0:
	    header = GetStringAttackSkill();
	    string = GetStringDescriptionsAttackSkill();
	    break;
	case 1:
	    header = GetStringDefenseSkill();
	    string = GetStringDescriptionsDefenseSkill();
	    break;
	case 2:
	    header = GetStringSpellPower();
	    string = GetStringDescriptionsSpellPower();
	    break;
	case 3:
	    header = GetStringKnowledge();
	    string = GetStringDescriptionsKnowledge();
	    break;
	default:
	    break;
    }

    return MessageBox(header, string, FONT_BIG, OK);
}

ACTION ActionHeroesInfoRClickPrimarySkill(void){

    Sint32 mx, my;
    const char *header = NULL;
    const char *string = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;
    
    switch(index){
	case 0:
	    header = GetStringAttackSkill();
	    string = GetStringDescriptionsAttackSkill();
	    break;
	case 1:
	    header = GetStringDefenseSkill();
	    string = GetStringDescriptionsDefenseSkill();
	    break;
	case 2:
	    header = GetStringSpellPower();
	    string = GetStringDescriptionsSpellPower();
	    break;
	case 3:
	    header = GetStringKnowledge();
	    string = GetStringDescriptionsKnowledge();
	    break;
	default:
	    break;
    }

    return MessageBox(header, string, FONT_BIG, NONE);
}

ACTION ActionHeroesInfoOverPrimarySkill(void){

    Sint32 mx, my;
    char str[STRLEN + 1];
    SDL_Surface *video = SDL_GetVideoSurface();
    
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;
    
    switch(index){
	case 0:
	    snprintf(str, STRLEN, "View %s Info", GetStringAttackSkill());
	    break;
	case 1:
	    snprintf(str, STRLEN, "View %s Info", GetStringDefenseSkill());
	    break;
	case 2:
	    snprintf(str, STRLEN, "View %s Info", GetStringSpellPower());
	    break;
	case 3:
	    snprintf(str, STRLEN, "View %s Info", GetStringKnowledge());
	    break;
	default:
	    break;
    }

    PrintHeroesInfoPrompt(str);

    return NONE;
}

ACTION	ActionHeroesInfoLClickMorale(void){

    const char *header = NULL;
    
    switch(CalculateHeroesMorale(heroes)){
	case MORALE_TREASON:
        case MORALE_AWFUL:
        case MORALE_POOR:
	    header = "Bad Morale";
	    break;
        case MORALE_NORMAL:
	    header = "Neutral Morale";
	    break;
        case MORALE_GOOD:
        case MORALE_GREAT:
        case MORALE_BLOOD:
	    header = "Good Morale";
	    break;
    }

    return MessageBox(header, GetStringDescriptionsMorale(CalculateHeroesMorale(heroes)), FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickMorale(void){

    const char *header = NULL;
    
    switch(CalculateHeroesMorale(heroes)){
	case MORALE_TREASON:
        case MORALE_AWFUL:
        case MORALE_POOR:
	    header = "Bad Morale";
	    break;
        case MORALE_NORMAL:
	    header = "Neutral Morale";
	    break;
        case MORALE_GOOD:
        case MORALE_GREAT:
        case MORALE_BLOOD:
	    header = "Good Morale";
	    break;
    }

    return MessageBox(header, GetStringDescriptionsMorale(CalculateHeroesMorale(heroes)), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverMorale(void){

    const char *header = NULL;
    char str[STRLEN + 1];
    
    switch(CalculateHeroesMorale(heroes)){
	case MORALE_TREASON:
        case MORALE_AWFUL:
        case MORALE_POOR:
	    header = "Bad Morale";
	    break;
        case MORALE_NORMAL:
	    header = "Neutral Morale";
	    break;
        case MORALE_GOOD:
        case MORALE_GREAT:
        case MORALE_BLOOD:
	    header = "Good Morale";
	    break;
    }
    
    snprintf(str, STRLEN, "View %s Info", header);
    PrintHeroesInfoPrompt(str);

    return NONE;
}

ACTION	ActionHeroesInfoLClickLuck(void){

    const char *header = NULL;
    
    switch(CalculateHeroesLuck(heroes)){
	case LUCK_CURSED:
        case LUCK_AWFUL:
        case LUCK_BAD:
	    header = "Bad Luck";
	    break;
        case LUCK_NORMAL:
	    header = "Neutral Luck";
	    break;
        case LUCK_GOOD:
        case LUCK_GREAT:
        case LUCK_IRISH:
	    header = "Good Luck";
	    break;
    }

    return MessageBox(header, GetStringDescriptionsLuck(CalculateHeroesLuck(heroes)), FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickLuck(void){

    const char *header = NULL;
    
    switch(CalculateHeroesLuck(heroes)){
	case LUCK_CURSED:
        case LUCK_AWFUL:
        case LUCK_BAD:
	    header = "Bad Luck";
	    break;
        case LUCK_NORMAL:
	    header = "Neutral Luck";
	    break;
        case LUCK_GOOD:
        case LUCK_GREAT:
        case LUCK_IRISH:
	    header = "Good Luck";
	    break;
    }

    return MessageBox(header, GetStringDescriptionsLuck(CalculateHeroesLuck(heroes)), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverLuck(void){

    const char *header = NULL;
    char str[STRLEN + 1];
    
    switch(CalculateHeroesLuck(heroes)){
	case LUCK_CURSED:
        case LUCK_AWFUL:
        case LUCK_BAD:
	    header = "Bad Luck";
	    break;
        case LUCK_NORMAL:
	    header = "Neutral Luck";
	    break;
        case LUCK_GOOD:
        case LUCK_GREAT:
        case LUCK_IRISH:
	    header = "Good Luck";
	    break;
    }
    
    snprintf(str, STRLEN, "View %s Info", header);
    PrintHeroesInfoPrompt(str);

    return NONE;
}

void PrintHeroesInfoPrompt(const char *string){

    AGGSPRITE sprite;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rect;

    if(! string) string = "Hero Screen";

    FillSPRITE(&sprite, "HSBTNS.ICN", 8);
    image = GetICNSprite(&sprite);
    rect.x = (video->w - image->w) / 2;
    rect.y = video->h / 2 + 240 - image->h;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    PrintAlignText(video, &rect, string, FONT_BIG);
}

ACTION ActionHeroesInfoLOOP(INTERFACEACTION *action){

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
            PrintHeroesInfoPrompt(NULL);
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

			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_RCLICK) && ptr->pf )
					exit = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }
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

ACTION	ActionHeroesInfoLClickSpreadFormat(void){

    SDL_Rect rect;

    if(SPREAD != heroes->af){
	rect = selectHeroesFormat->pos;
	rect.x -= 37;
	ResetSelector(selectHeroesFormat);
	RedrawSelector(selectHeroesFormat, &rect);
	CursorOn();
	heroes->af = SPREAD;
    }
    
    return NONE;
}

ACTION	ActionHeroesInfoLClickGroupedFormat(void){

    SDL_Rect rect;

    if(GROUPED != heroes->af){
	rect = selectHeroesFormat->pos;
	rect.x += 37;
	ResetSelector(selectHeroesFormat);
	RedrawSelector(selectHeroesFormat, &rect);
	CursorOn();
	heroes->af = GROUPED;
    }

    return NONE;
}

ACTION	ActionHeroesInfoRClickSpreadFormat(void){

    return MessageBox("Spread Formation", GetStringDescriptionsFormation(SPREAD), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoRClickGroupedFormat(void){

    return MessageBox("Grouped Formation", GetStringDescriptionsFormation(GROUPED), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverSpreadFormat(void){

    PrintHeroesInfoPrompt("Set army combat formation to 'Spread'");

    return NONE;
}

ACTION	ActionHeroesInfoOverGroupedFormat(void){

    PrintHeroesInfoPrompt("Set army combat formation to 'Grouped'");

    return NONE;
}

ACTION	ActionHeroesInfoLClickExperience(void){

    char str[STRLEN + 1];
    char num[10];

    snprintf(num, 9, "Level %d", GetHeroesLevel(heroes));
    snprintf(str, STRLEN, "Current experience %d\nNext level %d.", heroes->experience, GetHeroesRestLevel(heroes));

    return MessageBox(num, str, FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickExperience(void){

    char str[STRLEN + 1];
    char num[10];

    snprintf(num, 9, "Level %d", GetHeroesLevel(heroes));
    snprintf(str, STRLEN, "Current experience %d\nNext level %d.", heroes->experience, GetHeroesRestLevel(heroes));

    return MessageBox(num, str, FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverExperience(void){

    PrintHeroesInfoPrompt("View Experience Info");

    return NONE;
}

ACTION	ActionHeroesInfoLClickPoints(void){

    char str[256];
    snprintf(str, 255, "%s currently has %d spell points out of a maximum of %d. The maximum number of spell points is 10 times your knowledge. It is occasionally possible to have more than your maximum spell points via special events.", heroes->name, heroes->magicPoint, CalculateHeroesMagicPoint(heroes));

    return MessageBox("Spell Points", str, FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickPoints(void){

    char str[256];
    snprintf(str, 255, "%s currently has %d spell points out of a maximum of %d. The maximum number of spell points is 10 times your knowledge. It is occasionally possible to have more than your maximum spell points via special events.", heroes->name, heroes->magicPoint, CalculateHeroesMagicPoint(heroes));

    return MessageBox("Spell Points", str, FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverPoints(void){

    PrintHeroesInfoPrompt("View Spell Points Info");

    return NONE;
}

void RedrawHeroesInfoMonster(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    SDL_Rect rectCur;
    Uint16 cx, cy;
    Uint8 i;
    ICNHEADER *header = NULL;

    if(! heroes) return;

    CursorOff();

    char num[NUMLEN + 1];

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = video->w / 2 - 164;
	cy = video->h / 2 - 110;

	for( i = 0; i < HEROESMAXARMY; ++i)
	    // если есть у героя монстры
	    if(heroes->army[i].count){

		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(heroes->army[i].monster)){
		
		    case KNIGHT:
			FillSPRITE(&sprite, "STRIP.ICN", 4);
			break;

		    case BARBARIAN:
			FillSPRITE(&sprite, "STRIP.ICN", 5);
			break;

		    case SORCERESS:
			FillSPRITE(&sprite, "STRIP.ICN", 6);
			break;

		    case WARLOCK:
			FillSPRITE(&sprite, "STRIP.ICN", 7);
			break;

		    case WIZARD:
			FillSPRITE(&sprite, "STRIP.ICN", 8);
			break;

		    case NECROMANCER:
			FillSPRITE(&sprite, "STRIP.ICN", 9);
			break;

		    default:
			FillSPRITE(&sprite, "STRIP.ICN", 10);
			break;
		}
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// рисуем монстров
		FillSPRITE(&sprite, MonsterBigNamePortrait(heroes->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		snprintf(num, NUMLEN, "%5d", heroes->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = GetLengthText(num, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, num, FONT_SMALL);

	    }else{
		FillSPRITE(&sprite, "STRIP.ICN", 2);
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);
	    }
    }
}

ACTION	ActionHeroesInfoLClickArmy(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;
    Uint8 select = GetIndexSelected(selectHeroesArmy);
    Uint16 count = 0;
    E_MONSTER monster = MONSTERNONE;

    // select
    if(! select && MONSTERNONE != heroes->army[index].monster){
	rect.x = video->w / 2 - 164 + index * 88;
	rect.y = video->h / 2 - 110;
	RedrawSelector(selectHeroesArmy, &rect);
	CursorOn();
	return NONE;
    
    // show info
    }else if(index + 1 == select && MONSTERNONE != heroes->army[index].monster){
	ResetSelector(selectHeroesArmy);
	switch(ShowArmyInfo(&heroes->army[index], heroes, NULL)){
	    case EXIT:
		return EXIT;
		break;
	    case DISMISS:
		heroes->army[index].monster = MONSTERNONE;
		heroes->army[index].count = 0;
		break;
	    default:
		return NONE;
		break;
	}
	RedrawHeroesInfoMonster();
	CursorOn();

    // addon
    }else if(MONSTERNONE != heroes->army[index].monster && heroes->army[select - 1].monster == heroes->army[index].monster){
	heroes->army[index].count += heroes->army[select - 1].count;
	heroes->army[select - 1].monster = MONSTERNONE;
	heroes->army[select - 1].count = 0;
	ResetSelector(selectHeroesArmy);
	RedrawHeroesInfoMonster();
	CursorOn();

    // change
    }else if(MONSTERNONE != heroes->army[index].monster){
	monster = heroes->army[index].monster;
	count = heroes->army[index].count;
	heroes->army[index].monster = heroes->army[select - 1].monster;
	heroes->army[index].count = heroes->army[select - 1].count;
	heroes->army[select - 1].monster = monster;
	heroes->army[select - 1].count = count;
	ResetSelector(selectHeroesArmy);
	RedrawHeroesInfoMonster();
	CursorOn();

    // move
    }else if(select){
	heroes->army[index].monster = heroes->army[select - 1].monster;
	heroes->army[index].count = heroes->army[select - 1].count;
	heroes->army[select - 1].monster = MONSTERNONE;
	heroes->army[select - 1].count = 0;
	ResetSelector(selectHeroesArmy);
	RedrawHeroesInfoMonster();
	CursorOn();
    }
    
    return NONE;
}

ACTION	ActionHeroesInfoRClickArmy(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;
    Uint8 select = GetIndexSelected(selectHeroesArmy);
    Uint16 count = 0;

    CursorOff();

    if(select && index + 1 != select && (MONSTERNONE == heroes->army[index].monster || heroes->army[index].monster == heroes->army[select - 1].monster)){
	ResetSelector(selectHeroesArmy);

	count = SelectCountBox(heroes->army[select - 1].count);
	if(0xFFFF == count) return EXIT;

	heroes->army[index].monster = heroes->army[select - 1].monster;
	heroes->army[index].count += count;
	heroes->army[select - 1].count -= count;

	RedrawHeroesInfoMonster();
	CursorOn();

	return NONE;
    }

    ResetSelector(selectHeroesArmy);
    CursorOn();

    return ShowArmyInfoRight(&heroes->army[index], heroes);
}

ACTION	ActionHeroesInfoOverArmy(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[STRLEN + 1];
    
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 164)) / 88;

    CursorOff();
    if(MONSTERNONE == heroes->army[index].monster)
	GetIndexSelected(selectHeroesArmy) ? snprintf(str, STRLEN, "Move Army") : snprintf(str, STRLEN, "Empty");
    else
	snprintf(str, STRLEN, "Select %s", GetStringMonster(heroes->army[index].monster));
    
    PrintHeroesInfoPrompt(str);
    CursorOn();

    return NONE;
}

ACTION	ActionHeroesInfoLClickSecondarySkill(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[STRLEN + 1];
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 3)) / 80;

    if(index >= HEROESMAXSKILL || heroes->skill[index].type == SKILLNONE) return NONE;

    snprintf(str, STRLEN, "%s %s", GetStringLevelSkill(heroes->skill[index].level), GetStringSkill(heroes->skill[index].type));

    return MessageBox(str, GetStringDescriptionsSkill(heroes->skill[index].type, heroes->skill[index].level), FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickSecondarySkill(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[STRLEN + 1];
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 3)) / 80;

    if(index >= HEROESMAXSKILL || heroes->skill[index].type == SKILLNONE) return NONE;

    snprintf(str, STRLEN, "%s %s", GetStringLevelSkill(heroes->skill[index].level), GetStringSkill(heroes->skill[index].type));

    return MessageBox(str, GetStringDescriptionsSkill(heroes->skill[index].type, heroes->skill[index].level), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverSecondarySkill(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[STRLEN + 1];
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 3)) / 80;

    if(index >= HEROESMAXSKILL || heroes->skill[index].type == SKILLNONE) return NONE;

    snprintf(str, STRLEN, "View %s %s Info", GetStringLevelSkill(heroes->skill[index].level), GetStringSkill(heroes->skill[index].type));
    PrintHeroesInfoPrompt(str);

    return NONE;
}

ACTION	ActionHeroesInfoLClickArtifact(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 51)) / 79;

    if(my > video->h / 2 + 132) index += HEROESMAXARTIFACT / 2;

    if(index >= ARTIFACTMAXCOUNT || heroes->artifact[index] == ARTIFACTNONE) return NONE;

    if(MAGIC_BOOK == heroes->artifact[index]) return ShowMagicBook(heroes);

    return MessageBox(GetStringArtifact(heroes->artifact[index]), GetStringDescriptionsArtifact(heroes->artifact[index]), FONT_BIG, OK);
}

ACTION	ActionHeroesInfoRClickArtifact(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 51)) / 79;

    if(my > video->h / 2 + 132) index += HEROESMAXARTIFACT / 2;

    if(index >= ARTIFACTMAXCOUNT || heroes->artifact[index] == ARTIFACTNONE) return NONE;

    return MessageBox(GetStringArtifact(heroes->artifact[index]), GetStringDescriptionsArtifact(heroes->artifact[index]), FONT_BIG, NONE);
}

ACTION	ActionHeroesInfoOverArtifact(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    char str[STRLEN + 1];
            
    SDL_GetMouseState(&mx, &my);
    Uint8 index = (mx - (video->w / 2 - 320 + 51)) / 79;

    if(my > video->h / 2 + 132) index += HEROESMAXARTIFACT / 2;

    if(index >= ARTIFACTMAXCOUNT || heroes->artifact[index] == ARTIFACTNONE) return NONE;

    snprintf(str, STRLEN, "View %s Info", GetStringArtifact(heroes->artifact[index]));

    PrintHeroesInfoPrompt(str);

    return NONE;
}
