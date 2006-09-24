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
#include "element.h"
#include "config.h"
#include "animation.h"
#include "draw.h"
#include "cursor.h"
#include "loadgame.h"
#include "mp2maps.h"
#include "kingdom.h"
#include "heroes.h"
#include "heroesinfo.h"
#include "debug.h"
#include "payment.h"
#include "spell.h"
#include "magictower.h"
#include "castle.h"
#include "armyinfo.h"
#include "selector.h"
#include "castleaction.h"
#include "castle_kngt.h"
#include "castle_brbn.h"
#include "castle_scrs.h"
#include "castle_ncrm.h"
#include "castle_wrlk.h"
#include "castle_wzrd.h"

#define NUMLEN	8

void RedrawCastleAnimation(void);
void RedrawBottomBar(void);
void RedrawRamkaCastleName(void);
void RedrawHeroesMonster(void);
void UpdateCastleBuilding(void);
void RedrawNNNNCastle(const S_CASTLE *);

ACTION ActionCASTLELOOP(INTERFACEACTION *); 

ACTION ActionExitCastle(void); 
ACTION ActionLeftCastle(void); 
ACTION ActionRightCastle(void); 
ACTION ActionClickRedistributeMonster(Uint8, BOOL);
ACTION ActionClickCastleMonster(void);
ACTION ActionClickRCastleMonster(void);
ACTION ActionClickCastleMonsterEmpty(Uint8);
ACTION ActionClickHeroesMonster(void);
ACTION ActionClickRHeroesMonster(void);
ACTION ActionClickHeroesMonsterEmpty(Uint8);
ACTION ActionClickHeroes(void);

ACTION ActionOverHeroesArmy(void);
ACTION ActionOverCastleArmy(void);

static 	S_CASTLE	*ptrCastle	= NULL;
static 	Uint8		countCastle	= 0;

static	S_CASTLE	*currentCastle	= NULL;

	S_ANIMATION    	*castanim	= NULL;
	INTERFACEACTION *castlact	= NULL;

static	BOOL		flagUpdateBuilding = FALSE;

struct {
    Uint8		select;
    BOOL		castle;
    S_SELECT		*selector;
} selectCastleArmy;

BOOL	AddCastle(FILE *fd, Uint8 seek, Uint8 ax, Uint8 ay){

    if(! fd){
	fprintf(stderr, "AddCastle: descriptor NULL\n");
	return FALSE;
    }

    Uint16 countBlock = 0;
    MP2CASTLE *ptr = NULL;

    while(seek){
	fread(&countBlock, sizeof(Uint16), 1, fd);
	fseek(fd, countBlock, SEEK_CUR);
	--seek;
    }

    fread(&countBlock, sizeof(Uint16), 1, fd);
    if(0x0046 != countBlock){
	fprintf(stderr, "AddCastle: error magic: %hX\n", countBlock);
	return FALSE;
    }

    if(NULL == (ptr = (MP2CASTLE *) malloc(sizeof(MP2CASTLE)))){
	fprintf(stderr, "AddCastle: error malloc: %d\n", sizeof(MP2CASTLE));
	return FALSE;
    }

    memset(ptr, 0, sizeof(MP2CASTLE));

    fread((char *) &ptr->color, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customBuilding, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->building, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->dwelling, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->magicTower, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customTroops, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster1, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster2, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster3, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster4, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster5, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->count1, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count2, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count3, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count4, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count5, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->capitan, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customCastleName, 1, sizeof(Uint8), fd);
    fread((char *) ptr->castleName, 1, 13, fd);
    fread((char *) &ptr->type, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->castle, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->allowCastle, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->unknown, 1, 29, fd);

    if(NULL == (ptrCastle = (S_CASTLE *) realloc(ptrCastle, sizeof(S_CASTLE) * (countCastle + 1)))){
	fprintf(stderr, "AddCastle: error malloc: %d\n", sizeof(S_CASTLE) * (countCastle + 1));
	return FALSE;
    }

	switch(ptr->color){
	
	    case 0:
		ptrCastle[countCastle].color = BLUE;
		break;
		
	    case 1:
		ptrCastle[countCastle].color = GREEN;
		break;
		
	    case 2:
		ptrCastle[countCastle].color = RED;
		break;
		
	    case 3:
		ptrCastle[countCastle].color = YELLOW;
		break;
		
	    case 4:
		ptrCastle[countCastle].color = ORANGE;
		break;
		
	    case 5:
		ptrCastle[countCastle].color = PURPLE;
		break;

	    case 0xff:
		ptrCastle[countCastle].color = GRAY;
		break;

	    default:
		ptrCastle[countCastle].color = GRAY;
		fprintf(stderr, "AddCastle: unknown colors, default GRAY\n");
		break;
	}

	switch(ptr->type){
	
	    case 0:
		ptrCastle[countCastle].race = KNIGHT;
		break;

	    case 1:
		ptrCastle[countCastle].race = BARBARIAN;
		break;

	    case 2:
		ptrCastle[countCastle].race = SORCERESS;
		break;

	    case 3:
		ptrCastle[countCastle].race = WARLOCK;
		break;

	    case 4:
		ptrCastle[countCastle].race = WIZARD;
		break;

	    case 5:
		ptrCastle[countCastle].race = NECROMANCER;
		break;

	    // rnd
	    case 6:
		ptrCastle[countCastle].race = rand() % BOMG;
		break;
	
	    default:
		ptrCastle[countCastle].race = KNIGHT;
		fprintf(stderr, "AddCastle: unknown race, default KNIGHT\n");
		break;
	}

	strncpy(ptrCastle[countCastle].name, ptr->castleName, 13);

	if(ptr->customBuilding){
	    ptrCastle[countCastle].building = ptr->building;
	    ptrCastle[countCastle].dwelling = ptr->dwelling;
	}else{
	    ptrCastle[countCastle].building = BUILD_TAVERN; // BUILD_THIEVESGUILD BUILD_TAVERN BUILD_SHIPYARD BUILD_WELL BUILD_STATUE BUILD_LEFTTURRET BUILD_RIGHTTURRET BUILD_MARKETPLACE BUILD_MOAT BUILD_WEL2 BUILD_SPEC
	    ptrCastle[countCastle].dwelling = DWELLING_MONSTER1 | DWELLING_MONSTER2; // DWELLING_MONSTER1 DWELLING_MONSTER2 DWELLING_MONSTER3 DWELLING_MONSTER4 DWELLING_MONSTER5 DWELLING_MONSTER6 DWELLING_UPGRADE2 DWELLING_UPGRADE3 DWELLING_UPGRADE4 DWELLING_UPGRADE5 DWELLING_UPGRADE6
	}

    Uint8 i;
    
    for(i = 0; i < CASTLESPELL_LEVEL1; ++i) ptrCastle[countCastle].mageGuild.level1[i] = SPELLNONE;
    for(i = 0; i < CASTLESPELL_LEVEL2; ++i) ptrCastle[countCastle].mageGuild.level2[i] = SPELLNONE;
    for(i = 0; i < CASTLESPELL_LEVEL3; ++i) ptrCastle[countCastle].mageGuild.level3[i] = SPELLNONE;
    for(i = 0; i < CASTLESPELL_LEVEL4; ++i) ptrCastle[countCastle].mageGuild.level4[i] = SPELLNONE;
    for(i = 0; i < CASTLESPELL_LEVEL5; ++i) ptrCastle[countCastle].mageGuild.level5[i] = SPELLNONE;

    if(0 < ptr->magicTower){
	FillSpellLevel1(ptrCastle[countCastle].race, ptrCastle[countCastle].mageGuild.level1);
	ptrCastle[countCastle].building |= BUILD_MAGEGUILD1;
    }
    if(1 < ptr->magicTower){
	FillSpellLevel2(ptrCastle[countCastle].race, ptrCastle[countCastle].mageGuild.level2);
	ptrCastle[countCastle].building |= BUILD_MAGEGUILD2;
    }
    if(2 < ptr->magicTower){
	FillSpellLevel3(ptrCastle[countCastle].race, ptrCastle[countCastle].mageGuild.level3);
	ptrCastle[countCastle].building |= BUILD_MAGEGUILD3;
    }
    if(3 < ptr->magicTower){
	FillSpellLevel4(ptrCastle[countCastle].race, ptrCastle[countCastle].mageGuild.level4);
	ptrCastle[countCastle].building |= BUILD_MAGEGUILD4;
    }
    if(4 < ptr->magicTower){
	FillSpellLevel5(ptrCastle[countCastle].race, ptrCastle[countCastle].mageGuild.level5);
	ptrCastle[countCastle].building |= BUILD_MAGEGUILD5;
    }
    // дополняем магией из библиотеки
    if(WIZARD == ptrCastle[countCastle].race && BUILD_SPEC & ptrCastle[countCastle].building){
	if(0 < ptr->magicTower) FillSpellLevel1(WIZARD, ptrCastle[countCastle].mageGuild.level1);
	if(1 < ptr->magicTower) FillSpellLevel2(WIZARD, ptrCastle[countCastle].mageGuild.level2);
	if(2 < ptr->magicTower) FillSpellLevel3(WIZARD, ptrCastle[countCastle].mageGuild.level3);
	if(3 < ptr->magicTower) FillSpellLevel4(WIZARD, ptrCastle[countCastle].mageGuild.level4);
	if(4 < ptr->magicTower) FillSpellLevel5(WIZARD, ptrCastle[countCastle].mageGuild.level5);
    }
	if(ptr->customTroops){
	    ptrCastle[countCastle].army[0].monster = ptr->monster1;
	    ptrCastle[countCastle].army[1].monster = ptr->monster2;
	    ptrCastle[countCastle].army[2].monster = ptr->monster3;
	    ptrCastle[countCastle].army[3].monster = ptr->monster4;
	    ptrCastle[countCastle].army[4].monster = ptr->monster5;
	    ptrCastle[countCastle].army[0].count = ptr->count1;
	    ptrCastle[countCastle].army[1].count = ptr->count2;
	    ptrCastle[countCastle].army[2].count = ptr->count3;
	    ptrCastle[countCastle].army[3].count = ptr->count4;
	    ptrCastle[countCastle].army[4].count = ptr->count5;

	}else{	
	    ptrCastle[countCastle].army[0].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1));
	    ptrCastle[countCastle].army[1].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2));
	    switch(ptrCastle[countCastle].race){
	
		case KNIGHT:
		    ptrCastle[countCastle].army[0].monster = PEASANT;
		    ptrCastle[countCastle].army[1].monster = ARCHER;
		    break;

		case BARBARIAN:
		    ptrCastle[countCastle].army[0].monster = GOBLIN;
		    ptrCastle[countCastle].army[1].monster = ORC;
		    break;

		case SORCERESS:
		    ptrCastle[countCastle].army[0].monster = SPRITE;
		    ptrCastle[countCastle].army[1].monster = DWARF;
		    break;

		case WARLOCK:
		    ptrCastle[countCastle].army[0].monster = CENTAUR;
		    ptrCastle[countCastle].army[1].monster = GARGOYLE;
		    break;

		case WIZARD:
		    ptrCastle[countCastle].army[0].monster = HALFLING;
		    ptrCastle[countCastle].army[1].monster = BOAR;
		    break;

		case NECROMANCER:
		    ptrCastle[countCastle].army[0].monster = SKELETON;
		    ptrCastle[countCastle].army[1].monster = ZOMBIE;
		    break;
	
		default:
		    break;
	    }

	    ptrCastle[countCastle].army[0].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1));
	    ptrCastle[countCastle].army[1].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2));

	    ptrCastle[countCastle].army[2].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[3].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[4].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[2].count = 0;
	    ptrCastle[countCastle].army[3].count = 0;
	    ptrCastle[countCastle].army[4].count = 0;
	}

    ptrCastle[countCastle].monster[0] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER1 ?
	 GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1)) : 0);

    ptrCastle[countCastle].monster[1] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER2 ?
	GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2)) : 0);

    ptrCastle[countCastle].monster[2] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER3 ?
	GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 3)) : 0);

    ptrCastle[countCastle].monster[3] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER4 ?
	GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 4)) : 0);

    ptrCastle[countCastle].monster[4] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER5 ?
	GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 5)) : 0);

    ptrCastle[countCastle].monster[5] = (ptrCastle[countCastle].dwelling & DWELLING_MONSTER6 ?
	GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 6)) : 0);

    if(ptr->capitan) ptrCastle[countCastle].building |= BUILD_CAPTAIN;

    if(ptr->castle) ptrCastle[countCastle].building |= BUILD_CASTLE;

    ptrCastle[countCastle].allowCastle = (ptr->allowCastle ? FALSE : TRUE);
    
    ptrCastle[countCastle].boat = FALSE;
    ptrCastle[countCastle].af = SPREAD;

    ptrCastle[countCastle].pos.x = (ax < 2 ? 0 : ax - 2);
    ptrCastle[countCastle].pos.y = (ay < 3 ? 0 : ay - 3);

    ptrCastle[countCastle].ax = ax;
    ptrCastle[countCastle].ay = ay;
    ptrCastle[countCastle].pos.w = 5;
    ptrCastle[countCastle].pos.h = 5;


    ptrCastle[countCastle].next = NULL;

    ++countCastle;

    free(ptr);

    return TRUE;
}

void	FreeCastle(void){

    if(ptrCastle) free(ptrCastle);
    countCastle = 0;
}

E_RACE GetRaceRNDCastle(Uint8 ax, Uint8 ay){

    Uint8 i;
    SDL_Rect rect;
    
    rect.w = 8;
    rect.h = 5;

    for(i = 0; i < countCastle; ++i){
	rect.x = (ptrCastle[i].pos.x > 3 ? ptrCastle[i].pos.x - 3 : 0);
	rect.y = ptrCastle[i].pos.y;
	if(ValidPoint(&rect, ax, ay))   return ptrCastle[i].race;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRaceRNDCastle: unknown castle, ax: %d, ay: %d\n", ax, ay);
    return BOMG;
}

const S_CASTLE  *GetCurrentCastle(void){

    return currentCastle;
}

S_CASTLE *GetStatCastlePos(Uint8 ax, Uint8 ay){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)

	if(ValidPoint(&ptrCastle[i].pos, ax, ay))

	    return &ptrCastle[i];

    return NULL;
}

S_CASTLE *GetStatCastle(Uint8 index){

    if(index >= countCastle) return NULL;

    return &ptrCastle[index];
}

ACTION EnterCastle(Uint8 ax, Uint8 ay, E_NAMEHEROES castleHeroes){

    // определяем тип замка
    SDL_Surface *format, *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    ACTION result;
    S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_HEROES *heroes = NULL;
    flagUpdateBuilding = FALSE;

    // в серые замки не заходим
    if(GRAY == castle->color) return NONE;

    currentCastle = castle;
    
    if(HEROESNULL == castleHeroes){
	if(HEROESNULL > castle->hero){
	    heroes = GetStatHeroes(castle->hero);
	    if(heroes->ax != castle->ax || heroes->ay != castle->ay) castle->hero = HEROESNULL;
	}else
	    castle->hero = HEROESNULL;
    }else
	castle->hero = castleHeroes;

    heroes = GetStatHeroes(castle->hero);

    // инициализируем backgroundCursor
    FillSPRITE(&sprite, "STRIP.ICN", 1);
    selectCastleArmy.selector = InitSelector(&sprite, 0, 0, FALSE);
    selectCastleArmy.select = 0xFF;

    // переопределяем курсор и выключаем анимацию карты
    SetIntValue(ANIM1, FALSE);
    CursorOff();
    Uint32 cursor = GetCursor();
    INTERFACEACTION action;

    Uint16 cx, cy;
    Uint8 i;

    // рисуем бакгроунд 640х480
    // если videomode более то рисуем бордюр + тень и по центру экрана
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
	cx = rectBack.x;
	cy = rectBack.y;
    }
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "EnterCastle: CreateRGBSurface failed: %s\n", SDL_GetError());
        return EXIT;
    }

    // сохраняем бакгроунд
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // рисуем бордюр
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);

    // рисуем фон хозяйства
    switch(castle->race){

	case KNIGHT:
	    FillSPRITE(&sprite, "TOWNBKG0.ICN", 0);
	    break;

	case BARBARIAN:
	    FillSPRITE(&sprite, "TOWNBKG1.ICN", 0);
	    break;

	case SORCERESS:
	    FillSPRITE(&sprite, "TOWNBKG2.ICN", 0);
	    break;

	case WARLOCK:
	    FillSPRITE(&sprite, "TOWNBKG3.ICN", 0);
	    break;

	case WIZARD:
	    FillSPRITE(&sprite, "TOWNBKG4.ICN", 0);
	    break;

	case NECROMANCER:
	    FillSPRITE(&sprite, "TOWNBKG5.ICN", 0);
	    break;
	
	default:
	    break;
    }
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // левый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 480 - image->h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SMALLBAR.ICN", 1);
    FillSPRITE(&action.objectPush, "SMALLBAR.ICN", 2);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionLeftCastle;
    AddActionEvent(&castlact, &action);

    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);


    // правый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 3);
    image = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SMALLBAR.ICN", 3);
    FillSPRITE(&action.objectPush, "SMALLBAR.ICN", 4);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionRightCastle;
    AddActionEvent(&castlact, &action);

    // рисуем цветовой знак
    switch(castle->color){
    
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

	default:
	    break;
    }
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 5;
    rectCur.y = cy + 262;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // action from castle monster
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    for(i = 0; i < CASTLEMAXARMY; ++i){
	rectCur.x = cx + 112 + (image->w + 6) * i;
	rectCur.y = cy + 262;
	rectCur.w = image->w;
	rectCur.h = image->h;
	// регистрируем событие на left click
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickCastleMonster;
	AddActionEvent(&castlact, &action);
	// регистрируем событие на right click
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_RCLICK;
	action.pf = ActionClickRCastleMonster;
	AddActionEvent(&castlact, &action);
	// наведение мыши
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverCastleArmy;
	AddActionEvent(&castlact, &action);
    }

    // портрет героя
    if(heroes){
	FillSPRITE(&sprite, HeroesBigNamePortrait(castleHeroes), 0);
	// наведение мыши
	rectCur.x = cx + 5;
	rectCur.y = cy + 350;
	rectCur.w = image->w;
	rectCur.h = image->h;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverHeroes;
	AddActionEvent(&castlact, &action);
	// клик
	ZeroINTERFACEACTION(&action);
        action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickHeroes;
	AddActionEvent(&castlact, &action);

    // рисуем капитана
    }else if(castle->building & BUILD_CAPTAIN)
	FillSPRITE(&sprite, CapitanBigNamePortrait(castle->race), 0);
    else
	FillSPRITE(&sprite, "STRIP.ICN", 3);

    image = GetICNSprite(&sprite);
    rectCur.x = cx + 5;
    rectCur.y = cy + 361;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // action from castle monster
    if(heroes){
	FillSPRITE(&sprite, "STRIP.ICN", 2);
	image = GetICNSprite(&sprite);
	for( i = 0; i < HEROESMAXARMY; ++i){
	    rectCur.x = cx + 112 + (image->w + 6) * i;
	    rectCur.y = cy + 361;
	    rectCur.w = image->w;
	    rectCur.h = image->h;

	    // регистрируем событие на click
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionClickHeroesMonster;
	    AddActionEvent(&castlact, &action);
	    // регистрируем событие на click
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_RCLICK;
	    action.pf = ActionClickRHeroesMonster;
	    AddActionEvent(&castlact, &action);
	    // наведение мыши
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_PRESENT;
	    action.pf = ActionOverHeroesArmy;
	    AddActionEvent(&castlact, &action);
	}
    }

    RedrawCastleMonster();
    RedrawHeroesMonster();
    RedrawCastleInfoResource();

    // кнопка exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 553;
    rectCur.y = cy + 428;
    rectCur.w = image->w;
    rectCur.h = image->h;
    // наведение мыши
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverExit;
    AddActionEvent(&castlact, &action);
    // клик
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SWAPBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "SWAPBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionExitCastle;
    AddActionEvent(&castlact, &action);

    RedrawNNNNCastle(castle);

    RedrawRamkaCastleName();

    SetCursor(CURSOR_POINTER);
    CursorOn();

    result = ActionCASTLELOOP(castlact);

    CursorOff();
    // чистим данные
    FreeAnimationEvent(castanim);
    FreeActionEvent(castlact);
    FreeSelector(selectCastleArmy.selector);

    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_FreeSurface(back);

    castlact = NULL;
    castanim = NULL;
    currentCastle = NULL;

    // востанавливаем курсор и анимацию карты
    SetCursor(cursor);
    CursorOn();

    SetIntValue(ANIM1, TRUE);
    
    return result;
}

ACTION ActionExitCastle(void){

    return CANCEL;
}

ACTION ActionLeftCastle(void){

    return NONE;
}

ACTION ActionRightCastle(void){

    return NONE;
}

ACTION ActionCASTLELOOP(INTERFACEACTION *action){

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

	SDL_GetMouseState(&cx, &cy);

	if(flagUpdateBuilding)  UpdateCastleBuilding();

        if(old.flagPres && !ValidPoint(&old.presRect, cx, cy)){
            RedrawBottomBar();
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

            if(exit != EXIT && 0 == ticket % (GetIntValue(ANIMATIONDELAY) / 2)) RedrawCastleAnimation();
            ++ticket;

	}

	if(0 == ticket % GetIntValue(ANIMATIONDELAY)) RedrawCastleAnimation();
	else if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);

	++ticket;
    }

    return exit;
}

void RedrawCastleAnimation(void){

    if(! GetIntValue(ANIMATION)) return;
    if(! GetIntValue(ANIM3)) return;

    static Uint32 animationFrame = 0;
 
    Sint32 x; 
    Sint32 y; 
     
    SDL_Surface *video = SDL_GetVideoSurface(); 
    SDL_GetMouseState(&x, &y); 
    S_ANIMATION *ptr = castanim; 

    CursorOff();
 
    while(ptr){ 
 
	// востановить фон предыдущего спрайта
	if(ptr->background){
    	    SDL_BlitSurface(ptr->background, NULL, video, &ptr->rectBack);
	    SDL_FreeSurface(ptr->background);
	    ptr->background = NULL;
	}
	
	if(NULL == (ptr->background = SDL_CreateRGBSurface(SDL_SWSURFACE, ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "RedrawCastleAnimation: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h);
            return;
        }

	// сохраняем фон нового спрайта
	ptr->rectBack = ptr->rect[animationFrame % ptr->count];
    	SDL_BlitSurface(video, &ptr->rectBack, ptr->background, NULL);

	// рисуем спрайт
        SDL_BlitSurface(ptr->surface[animationFrame % ptr->count], NULL, video, &ptr->rectBack);

        ptr = ptr->next; 
    } 
 
    // в зависимости от замка дорисовываем постройки
    if(currentCastle)
	switch(currentCastle->race){

	    case WARLOCK:
		if(currentCastle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(NULL, NULL);
		if(currentCastle->building & BUILD_STATUE) DrawWRLKStatue(NULL, NULL);
        	if(currentCastle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(NULL, NULL);
            	break;

	    case BARBARIAN:
		if(currentCastle->building & BUILD_THIEVESGUILD) DrawBRBNThievesGuild(NULL, NULL);
		if(currentCastle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(NULL, NULL);
		if(currentCastle->building & BUILD_STATUE) DrawBRBNStatue(NULL, NULL);
		if(currentCastle->building & BUILD_TAVERN) DrawBRBNTavern(NULL, NULL);
		if(currentCastle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(NULL, NULL);
		if(currentCastle->building & BUILD_WELL) DrawBRBNWell(NULL, NULL);
		// необходимо перерисовать имя замка
		RedrawRamkaCastleName();
		break;

	    default:
		break;
	}
                            
    CursorOn(); 
    //SDL_Flip(video); 
    SDL_Delay(GetIntValue(ANIMATIONDELAY));

    ++animationFrame;

    return;
}

void RedrawBottomBar(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;

    CursorOff();
    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.x = video->w / 2 - 320 + BORDERWIDTH + 5;
    rect.y = video->h / 2 + 240 - BORDERWIDTH - 3;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    CursorOn();
}

ACTION ActionClickRCastleMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    const S_CASTLE *castle = GetCurrentCastle();
    ACTION result;
    
    SDL_GetMouseState(&mx, &my);

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;

    Uint8 index = (Uint16) (mx - cx) / 88;
    //selectCastleArmy.selector->use = TRUE;

    // если монстр показываем инфо
    if(MONSTERNONE != castle->army[index].monster) result =  ShowArmyInfoRight(&castle->army[index], NULL);
    // иначе диалог деления
    else result = ActionClickRedistributeMonster(index, FALSE);

    ResetSelector(selectCastleArmy.selector);

    RedrawCastleMonster();
    RedrawHeroesMonster();

    CursorOn();

    return result;
}

ACTION ActionClickRHeroesMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));
    ACTION result;

    SDL_GetMouseState(&mx, &my);

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;

    Uint8 index = (Uint16) (mx - cx) / 88;

    // если монстр показываем инфо
    if(MONSTERNONE != heroes->army[index].monster) result =  ShowArmyInfoRight(&heroes->army[index], heroes);
    // иначе диалог деления
    else result = ActionClickRedistributeMonster(index, TRUE);

    ResetSelector(selectCastleArmy.selector);

    RedrawCastleMonster();
    RedrawHeroesMonster();

    CursorOn();

    return result;
}

ACTION ActionClickRedistributeMonster(Uint8 index, BOOL toHero){

    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));
    
    // верхний левый угол начала
    Uint16 res = 0;

    if(! selectCastleArmy.selector->use) return NONE;

    CursorOff();

    if(selectCastleArmy.castle){
    
	// from castle to castle
	if(! toHero){

	    if(currentCastle->army[index].monster != MONSTERNONE && currentCastle->army[index].monster != currentCastle->army[selectCastleArmy.select].monster){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    res = SelectCountBox(currentCastle->army[selectCastleArmy.select].count);
	    if(0xFFFF == res) return EXIT;
	    if(0 == res){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    currentCastle->army[selectCastleArmy.select].count -= res;

	    currentCastle->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
	    currentCastle->army[index].count += res;

	// from castle to hero
	}else if(heroes){

	    if(heroes->army[index].monster != MONSTERNONE && heroes->army[index].monster != currentCastle->army[selectCastleArmy.select].monster){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }
	    
	    res = SelectCountBox(currentCastle->army[selectCastleArmy.select].count);
	    if(0xFFFF == res) return EXIT;
	    if(0 == res){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    currentCastle->army[selectCastleArmy.select].count -= res;
	    heroes->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
	    heroes->army[index].count += res;
	}

    }else if(heroes){

	// from hero to castle
	if(! toHero){

	    if(currentCastle->army[index].monster != MONSTERNONE && currentCastle->army[index].monster != heroes->army[selectCastleArmy.select].monster){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }
	
	    res = SelectCountBox(heroes->army[selectCastleArmy.select].count);
	    if(0xFFFF == res) return EXIT;
	    if(0 == res){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    heroes->army[selectCastleArmy.select].count -= res;
	    currentCastle->army[index].monster = heroes->army[selectCastleArmy.select].monster;
	    currentCastle->army[index].count += res;

	}else{

	    if(heroes->army[index].monster != MONSTERNONE && heroes->army[index].monster != heroes->army[selectCastleArmy.select].monster){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    res = SelectCountBox(heroes->army[selectCastleArmy.select].count);
	    if(0xFFFF == res) return EXIT;
	    if(0 == res){
		ResetSelector(selectCastleArmy.selector);
		CursorOn();
		return NONE;
	    }

	    heroes->army[selectCastleArmy.select].count -= res;
	    heroes->army[index].monster = heroes->army[selectCastleArmy.select].monster;
	    heroes->army[index].count += res;
	}
    }

    return NONE;
}

ACTION ActionClickCastleMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;

    CursorOff();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    Uint16 cy = video->h / 2 + 22;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint16) (mx - cx) / 88;
    Uint16 count = 0;
    E_MONSTER monster = MONSTERNONE;
    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));

    if(MONSTERNONE == currentCastle->army[index].monster) return ActionClickCastleMonsterEmpty(index);

    // двойной клик - инфо монстра
    if(selectCastleArmy.select == index && selectCastleArmy.castle && selectCastleArmy.selector->use){
	ResetSelector(selectCastleArmy.selector);
	switch(ShowArmyInfo(&currentCastle->army[index], NULL, currentCastle)){
	    case DISMISS:
		currentCastle->army[index].monster = MONSTERNONE;
		currentCastle->army[index].count = 0;
		RedrawCastleMonster();
		break;

	    case UPGRADE:
		UpgradeArmy(&currentCastle->army[index], currentCastle->color);
		RedrawCastleMonster();
		RedrawCastleInfoResource();
		break;
	
	    case EXIT:
		return EXIT;
		break;

	    default:
		break;
	}
    // обмен
    }else if(selectCastleArmy.selector->use){
	ResetSelector(selectCastleArmy.selector);
	monster = currentCastle->army[index].monster;
	count = currentCastle->army[index].count;
	// в замке
	if(selectCastleArmy.castle){
	    // одинаковых объединяем
	    if(currentCastle->army[index].monster == currentCastle->army[selectCastleArmy.select].monster){
		currentCastle->army[index].count += currentCastle->army[selectCastleArmy.select].count;
		currentCastle->army[selectCastleArmy.select].monster = MONSTERNONE;
		currentCastle->army[selectCastleArmy.select].count = 0;
	    }else{
		currentCastle->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
		currentCastle->army[index].count = currentCastle->army[selectCastleArmy.select].count;
		currentCastle->army[selectCastleArmy.select].monster = monster;
		currentCastle->army[selectCastleArmy.select].count = count;
	    }
	    RedrawCastleMonster();
	// с героем
	}else if(heroes){

	    // одинаковых объединяем
	    if(currentCastle->army[index].monster == heroes->army[selectCastleArmy.select].monster){
		currentCastle->army[index].count += heroes->army[selectCastleArmy.select].count;
		heroes->army[selectCastleArmy.select].monster = MONSTERNONE;
		heroes->army[selectCastleArmy.select].count = 0;
	    }else{
		currentCastle->army[index].monster = heroes->army[selectCastleArmy.select].monster;
		currentCastle->army[index].count = heroes->army[selectCastleArmy.select].count;
		heroes->army[selectCastleArmy.select].monster = monster;
		heroes->army[selectCastleArmy.select].count = count;
	    }
	    RedrawCastleMonster();
	    RedrawHeroesMonster();
	}
    // первый клик рисуем рамку
    }else{
	rect.x = cx + index * (selectCastleArmy.selector->pos.w + 6);
	rect.y = cy;
	selectCastleArmy.select = index;
	selectCastleArmy.castle = TRUE;
	RedrawSelector(selectCastleArmy.selector, &rect);
    }

    CursorOn();

    return NONE;
}

ACTION ActionClickCastleMonsterEmpty(Uint8 index){

    if(! selectCastleArmy.selector->use) return NONE;

    CursorOff();

    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));
    ResetSelector(selectCastleArmy.selector);

    if(! selectCastleArmy.castle && 2 > HeroesCountArmy(heroes)){
	CursorOn();
	return NONE;
    }

    // перемещаем на пустую клетку
    if(selectCastleArmy.castle){
	currentCastle->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
	currentCastle->army[index].count = currentCastle->army[selectCastleArmy.select].count;
	currentCastle->army[selectCastleArmy.select].monster = MONSTERNONE;
	currentCastle->army[selectCastleArmy.select].count = 0;
    }else{
	currentCastle->army[index].monster = heroes->army[selectCastleArmy.select].monster;
	currentCastle->army[index].count = heroes->army[selectCastleArmy.select].count;
	heroes->army[selectCastleArmy.select].monster = MONSTERNONE;
	heroes->army[selectCastleArmy.select].count = 0;
    }

    RedrawCastleMonster();
    RedrawHeroesMonster();

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroesMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;

    CursorOff();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    Uint16 cy = video->h / 2 + 121;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint16) (mx - cx) / 88;
    Uint16 count = 0;
    E_MONSTER monster = MONSTERNONE;

    const S_CASTLE *castle = GetCurrentCastle();
    S_HEROES *heroes = GetStatHeroes(castle->hero);
    if(! heroes) return NONE;

    if(MONSTERNONE == heroes->army[index].monster) return ActionClickHeroesMonsterEmpty(index);

    // двойной клик - инфо монстра
    if(selectCastleArmy.select == index && !selectCastleArmy.castle && selectCastleArmy.selector->use){
	ResetSelector(selectCastleArmy.selector);
	switch(ShowArmyInfo(&heroes->army[index], heroes, castle)){
	    case DISMISS:
		CursorOff();
		heroes->army[index].monster = MONSTERNONE;
		heroes->army[index].count = 0;
		RedrawHeroesMonster();
		CursorOn();
		break;

	    case UPGRADE:
		CursorOff();
		UpgradeArmy(&heroes->army[index], heroes->color);
		RedrawHeroesMonster();
		RedrawCastleInfoResource();
		CursorOn();
		break;

	    case EXIT:
		return EXIT;
		break;

	    default:
		break;
	}
	CursorOff();
    // обмен
    }else if(selectCastleArmy.selector->use){
	ResetSelector(selectCastleArmy.selector);
	monster = heroes->army[index].monster;
	count = heroes->army[index].count;
	// с замком
	if(selectCastleArmy.castle){
	    // одинаковых объединяем
	    if(heroes->army[index].monster == currentCastle->army[selectCastleArmy.select].monster){
		heroes->army[index].count += currentCastle->army[selectCastleArmy.select].count;
		currentCastle->army[selectCastleArmy.select].monster = MONSTERNONE;
		currentCastle->army[selectCastleArmy.select].count = 0;
	    }else{
		heroes->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
		heroes->army[index].count = currentCastle->army[selectCastleArmy.select].count;
		currentCastle->army[selectCastleArmy.select].monster = monster;
		currentCastle->army[selectCastleArmy.select].count = count;
	    }
	    RedrawHeroesMonster();
	    RedrawCastleMonster();
	// в герое
	}else{
	    // одинаковых объединяем
	    if(heroes->army[index].monster == heroes->army[selectCastleArmy.select].monster){
		heroes->army[index].count += heroes->army[selectCastleArmy.select].count;
		heroes->army[selectCastleArmy.select].monster = MONSTERNONE;
		heroes->army[selectCastleArmy.select].count = 0;
	    }else{
		heroes->army[index].monster = heroes->army[selectCastleArmy.select].monster;
		heroes->army[index].count = heroes->army[selectCastleArmy.select].count;
		heroes->army[selectCastleArmy.select].monster = monster;
		heroes->army[selectCastleArmy.select].count = count;
	    }
	    RedrawHeroesMonster();
	}
    // первый клик рисуем рамку
    }else{
	rect.x = cx + index * (selectCastleArmy.selector->pos.w + 6);
	rect.y = cy;
	selectCastleArmy.select = index;
	selectCastleArmy.castle = FALSE;
	RedrawSelector(selectCastleArmy.selector, &rect);
    }

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroesMonsterEmpty(Uint8 index){

    if(! selectCastleArmy.selector->use) return NONE;

    CursorOff();

    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));
    ResetSelector(selectCastleArmy.selector);
    
    // перемещаем на пустую клетку
    if(selectCastleArmy.castle){
	heroes->army[index].monster = currentCastle->army[selectCastleArmy.select].monster;
	heroes->army[index].count = currentCastle->army[selectCastleArmy.select].count;
	currentCastle->army[selectCastleArmy.select].monster = MONSTERNONE;
	currentCastle->army[selectCastleArmy.select].count = 0;
    }else{
	heroes->army[index].monster = heroes->army[selectCastleArmy.select].monster;
	heroes->army[index].count = heroes->army[selectCastleArmy.select].count;
	heroes->army[selectCastleArmy.select].monster = MONSTERNONE;
	heroes->army[selectCastleArmy.select].count = 0;
    }

    RedrawCastleMonster();
    RedrawHeroesMonster();

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroes(void){

    ResetSelector(selectCastleArmy.selector);
    //ShowHeroesInfo(GetHeroesFromCastle(GetCurrentCastle()), FALSE);
    ShowHeroesInfo(SANDYSANDY, FALSE);
    RedrawHeroesMonster();

    return NONE;
}

void RedrawCastleMonster(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    SDL_Rect rectCur;
    Uint16 cx, cy;
    Uint8 i;
    ICNHEADER *header = NULL;

    char num[NUMLEN + 1];
    CursorOff();

    // рисуем фон ячеек для монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    cx = video->w / 2 - 208;
    cy = video->h / 2 + 22;
    for(i = 0; i < CASTLEMAXARMY; ++i){

	// если есть в замке монстры
	if(currentCastle->army[i].count){
		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(currentCastle->army[i].monster)){
		
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
		FillSPRITE(&sprite, MonsterBigNamePortrait(currentCastle->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		snprintf(num, NUMLEN, "%5d", currentCastle->army[i].count);
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

void RedrawHeroesMonster(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    SDL_Rect rectCur;
    Uint16 cx, cy;
    Uint8 i;
    ICNHEADER *header = NULL;
    S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));

    if(! heroes) return;

    CursorOff();

    char num[NUMLEN + 1];

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = video->w / 2 - 208;
	cy = video->h / 2 + 121;

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

S_CASTLE *GetFirstCastle(E_COLORS color){

    Uint8 i;
    currentCastle = NULL;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color){
	    currentCastle = &ptrCastle[i];
	    return currentCastle;
	}

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetFirstCastle: return NULL\n");

    return NULL;
}

S_CASTLE *GetNextCastle(E_COLORS color){

    Uint8 i;
    BOOL  flag = FALSE;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color){
	    if(flag){
		currentCastle = &ptrCastle[i];
		return currentCastle;
	    }else if(currentCastle == &ptrCastle[i]) flag = TRUE;
	}

    return NULL;
}

S_CASTLE *GetPrevCastle(E_COLORS color){

    Uint8 i;
    S_CASTLE *result = NULL;
    
    for(i = 0; i < countCastle; ++i)

	if(ptrCastle[i].color == color){
	    if(currentCastle == &ptrCastle[i]){
		currentCastle = result;
		return result;
	    }else
		result = &ptrCastle[i];
	}

    return NULL;
}

S_CASTLE *GetEndCastle(E_COLORS color){

    Uint8 i;
    currentCastle = NULL;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color) currentCastle = &ptrCastle[i];

    if(!currentCastle && GetIntValue(DEBUG)) fprintf(stderr, "GetEndCastle: return NULL\n");

    return currentCastle;
}

E_MONSTER GetMonsterFromCastle(const S_CASTLE *castle, Uint8 level){

    switch(castle->race){
	
	case BARBARIAN:
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return WAR_TROLL;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return LORD_OGRE;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return CHIEF_ORC;
	    if(level == 6) return CYCLOPS;
	    if(level == 5) return TROLL;
	    if(level == 4) return OGRE;
	    if(level == 3) return WOLF;
	    if(level == 2) return ORC;
	    if(level == 1) return GOBLIN;
	    break;

	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return CRUSADER;
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return CHAMPION;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return MASTER_SWORDSMAN;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return VETERAN_PIKEMAN;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return RANGER;
	    if(level == 6) return PALADIN;
	    if(level == 5) return CAVALIRY;
	    if(level == 4) return SWORDSMAN;
	    if(level == 3) return PIKEMAN;
	    if(level == 2) return ARCHER;
	    if(level == 1) return PEASANT;
	    break;

	case SORCERESS:
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return GREATER_DRUID;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return GRAND_ELF;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return BATTLE_DWARF;
	    if(level == 6) return PHOENIX;
	    if(level == 5) return UNICORN;
	    if(level == 4) return DRUID;
	    if(level == 3) return ELF;
	    if(level == 2) return DWARF;
	    if(level == 1) return SPRITE;
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_UPGRADE7 && level == 6) return BLACK_DRAGON;
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return RED_DRAGON;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return KNIGHT_MINOTAUR;
	    if(level == 6) return GREEN_DRAGON;
	    if(level == 5) return HIDRA;
	    if(level == 4) return MINOTAUR;
	    if(level == 3) return GRIFFIN;
	    if(level == 2) return GARGOYLE;
	    if(level == 1) return CENTAUR;
	    break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return TITAN;
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return ARCHMAGE;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return STEEL_GOLEM;
	    if(level == 6) return GIANT;
	    if(level == 5) return MAGE;
	    if(level == 4) return ROC;
	    if(level == 3) return IRON_GOLEM;
	    if(level == 2) return BOAR;
	    if(level == 1) return HALFLING;
	    break;

	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return POWER_LICH;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return LORD_VAMPIRE;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return ROYAL_MUMMY;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return MUTANT_ZOMBIE;
	    if(level == 6) return BONE_DRAGON;
	    if(level == 5) return LICH;
	    if(level == 4) return VAMPIRE;
	    if(level == 3) return MUMMY;
	    if(level == 2) return ZOMBIE;
	    if(level == 1) return SKELETON;
	    break;
				
	default:
	    break;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetMonsterFromCastle: return MONSTERNONE, level: %d, castle: %s, race: %s, ax: %d, ay: %d\n", level, castle->name, GetStringRace(castle->race), castle->ax, castle->ay);

    return MONSTERNONE;
}

BOOL BuyMonsterFromCastle(const S_CASTLE *castle, Uint8 level, Uint16 count){

    currentCastle = NULL;
    Uint8 i;
    BOOL free = FALSE;

    for(i = 0; i < countCastle; ++i)
	if(&ptrCastle[i] == castle) currentCastle = &ptrCastle[i];

    if(	! count ||
	! currentCastle ||
	! level ||
	level > CASTLEMAXMONSTER ||
	currentCastle->monster[level - 1] < count ) return FALSE;

    E_MONSTER emonster = GetMonsterFromCastle(castle, level);

    // проверяем свободный слот
    for(i = 0; i < CASTLEMAXARMY; ++i)
	if(! castle->army[i].count || castle->army[i].monster == emonster){ free = TRUE; break; }

    if( ! free ||
	MONSTERNONE == emonster ||
	! CheckBuyMonsterFromCastle(castle, level, count) ) return FALSE;
 
    KingdomWasteResource(castle->color, GetMultiPayment(PaymentConditionsMonster(emonster), count));

    currentCastle->monster[level - 1] -= count;
    currentCastle->army[i].monster = emonster;
    currentCastle->army[i].count += count;

    return TRUE;
}

BOOL CheckBuyMonsterFromCastle(const S_CASTLE *castle, Uint8 level, Uint16 count){

    E_MONSTER emonster = GetMonsterFromCastle(castle, level);

    if(MONSTERNONE == emonster) return FALSE;

    const S_PAYMENT *payment = PaymentConditionsMonster(emonster);

    // проверяем платежеспособность
    if( !KingdomAllowPayment(castle->color, GetMultiPayment(payment, count))) return FALSE;
    
    return TRUE;
}

void RedrawCastleInfoResource(void){

    SDL_Rect rectCur, rectBack;
    SDL_Surface *video, *image;
    const S_CASTLE *castle = GetCurrentCastle();
    S_KINGDOM *kingdom = NULL;
    AGGSPRITE sprite;
    char num[NUMLEN + 1];
    Uint16 cx, cy;
    
    CursorOff();

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

    // закрашиваем фон для инфо ресурсов
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + 584;
	rectCur.y = rectBack.y + 278;
	rectCur.w = 82;
	rectCur.h = 192;
    }else{
	rectCur.x = rectBack.x + 584 - BORDERWIDTH * 2;
	rectCur.y = rectBack.y + 278 - BORDERWIDTH;
	rectCur.w = 82;
	rectCur.h = 192;
    }
    SDL_FillRect(video, &rectCur, 0);
    cx = rectCur.x;
    cy = rectCur.y;
    // sprite wood
    FillSPRITE(&sprite, "RESOURCE.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 10;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count wood
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->wood);
    rectCur.x = cx + 8;
    rectCur.y = cy + 32;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite sulfur
    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 42;
    rectCur.y = cy + 6;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count sulfur
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->sulfur);
    rectCur.x = cx + 48;
    rectCur.y = cy + 32;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite crystal
    FillSPRITE(&sprite, "RESOURCE.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 45;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count crystal
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->crystal);
    rectCur.x = cx + 8;
    rectCur.y = cy + 78;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite mercury
    FillSPRITE(&sprite, "RESOURCE.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 44;
    rectCur.y = cy + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count mercury
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->mercury);
    rectCur.x = cx + 48;
    rectCur.y = cy + 78;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite ore
    FillSPRITE(&sprite, "RESOURCE.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 92;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count ore
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->ore);
    rectCur.x = cx + 8;
    rectCur.y = cy + 118;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite gems
    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 45;
    rectCur.y = cy + 92;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count gems
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%5d", kingdom->gems);
    rectCur.x = cx + 48;
    rectCur.y = cy + 118;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // sprite gold
    FillSPRITE(&sprite, "RESOURCE.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 130;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count gold
    kingdom = GetStatKingdom(castle->color);
    snprintf(num, NUMLEN, "%7d", kingdom->gold);
    rectCur.x = cx + 24;
    rectCur.y = cy + 154;
    rectCur.w = GetLengthText(num, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, num, FONT_SMALL);
    // exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx +  1;
    rectCur.y = cy + 166;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
}

BOOL CastleNearOcean(const S_CASTLE * castle){

    if( WATER == GetGroundMaps(castle->ax - 1, castle->ay + 2) ||
	WATER == GetGroundMaps(castle->ax, castle->ay + 2) ||
        WATER == GetGroundMaps(castle->ax + 1, castle->ay + 2) ) return TRUE;

    return FALSE;
}

BOOL CastleNearBoat(void){

    return FALSE;
}

BOOL CastleDwellingUpgradable(const S_CASTLE *castle, E_DWELLINGCASTLE level){

    switch(castle->race){
    
	case BARBARIAN:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ) return TRUE;
	    break;
	
	case KNIGHT:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ||
		(level & DWELLING_UPGRADE6) ) return TRUE;
	    break;

	case NECROMANCER:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ) return TRUE;
	    break;

	case SORCERESS:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ) return TRUE;
	    break;

	case WARLOCK:
	    if( (level & DWELLING_UPGRADE4) ||
	        (level & DWELLING_UPGRADE6) ||
		(level & DWELLING_UPGRADE7) ) return TRUE;
	    break;

	case WIZARD:
	    if( (level & DWELLING_UPGRADE3) ||
	        (level & DWELLING_UPGRADE5) ||
		(level & DWELLING_UPGRADE6) ) return TRUE;
	    break;

	default:
	    break;
    }

    return FALSE;
}

BUILDACTION AllowBuildCastle(const S_CASTLE *castle){

    if(castle->building & BUILD_CASTLE) return ALREADY_BUILD;
    if(! castle->allowCastle) return CANNOT_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;
    
    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_CASTLE))) return BUILD_OK;
    
    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMageGuild(const S_CASTLE *castle){

    if(MAGIC_LEVEL5 == GetMageGuildLevel(castle)) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;
    
    switch(GetMageGuildLevel(castle)){
	case MAGIC_NONE:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD1))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL1:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD2))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL2:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD3))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL3:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD4))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL4:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD5))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL5:
	    return ALREADY_BUILD;
	    break;
    }

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildTavern(const S_CASTLE *castle){

    if(castle->building & BUILD_TAVERN) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_TAVERN))) return BUILD_OK;
    
    return CANNOT_BUILD;
}


BUILDACTION AllowBuildThievesGuild(const S_CASTLE *castle){

    if(castle->building & BUILD_THIEVESGUILD) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_THIEVESGUILD))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildShipyard(const S_CASTLE *castle){

    if(castle->building & BUILD_SHIPYARD) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(CastleNearOcean(castle) && KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_SHIPYARD))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildStatue(const S_CASTLE *castle){

    if(castle->building & BUILD_STATUE) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_STATUE))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMarketplace(const S_CASTLE *castle){

    if(castle->building & BUILD_MARKETPLACE) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MARKETPLACE))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildWell(const S_CASTLE *castle){

    if(castle->building & BUILD_WELL) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_WELL))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMoat(const S_CASTLE *castle){

    if(castle->building & BUILD_MOAT) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MOAT))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildLeftTurret(const S_CASTLE *castle){

    if(castle->building & BUILD_LEFTTURRET) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_LEFTTURRET))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildRightTurret(const S_CASTLE *castle){

    if(castle->building & BUILD_RIGHTTURRET) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_RIGHTTURRET))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildCaptain(const S_CASTLE *castle){

    if(castle->building & BUILD_CAPTAIN) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_CAPTAIN))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildBoat(const S_CASTLE *castle){

    if(castle->boat) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_BOAT))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildWel2(const S_CASTLE *castle){

    if(castle->building & BUILD_WEL2) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_WEL2))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildSpec(const S_CASTLE *castle){

    if(castle->building & BUILD_SPEC) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_SPEC))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling1(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_MONSTER1) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER1))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling2(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE2) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER2))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
	    case NECROMANCER:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2)) &&
		    castle->dwelling & DWELLING_MONSTER1  &&
		    castle->building & BUILD_TAVERN ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE2)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE2)) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_UPGRADE2)) &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling3(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE3) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER3))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER3)) &&
		    castle->dwelling & DWELLING_MONSTER1 &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
	    case NECROMANCER:
	    case SORCERESS:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER3)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_UPGRADE3)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE3)) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_UPGRADE3)) &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_UPGRADE3)) &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling4(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE4) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER4))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER1 &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->building & BUILD_TAVERN ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(BARBARIAN, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->building & BUILD_THIEVESGUILD ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;

	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER2 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 ) return BUILD_OK;
		break;

	    case NECROMANCER:
	    case SORCERESS:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4)) ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling5(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE5) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER5))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;
	
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WARLOCK, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER3 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE5)) ) return BUILD_OK;
		break;

	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE5)) &&
		    GetMageGuildLevel(castle) > MAGIC_LEVEL1) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_UPGRADE5)) &&
		    castle->building & BUILD_SPEC ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling6(const S_CASTLE *castle){

    if((castle->race != WARLOCK && castle->dwelling & DWELLING_UPGRADE6) || castle->dwelling & DWELLING_UPGRADE7) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER6))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
	    case NECROMANCER:
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER5 ) return BUILD_OK;
		break;

	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER4 &&
		    castle->dwelling & DWELLING_MONSTER5 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(castle->dwelling & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))

	switch(castle->race){

	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WARLOCK, DWELLING_UPGRADE7)) ) return BUILD_OK;
		break;


	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))

	switch(castle->race){

	    case KNIGHT:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE6)) ) return BUILD_OK;
		break;


	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowRecrutHeroes(const S_CASTLE *castle){

    // незабыть проверку на макс количество

    //если герой присутствет в замке
    if(CastlePresentHeroes(castle)) return ALREADY_BUILD;

    if(KingdomAllowPayment(castle->color, PaymentConditionsRecrutHeroes())) return BUILD_OK;

    return CANNOT_BUILD;
}

BOOL BuildMageGuild(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    switch(GetMageGuildLevel(&ptrCastle[i])){
	    
		case MAGIC_NONE:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD1));
		    ptrCastle[i].building |= BUILD_MAGEGUILD1;
		    FillSpellLevel1(ptrCastle[i].race, ptrCastle[i].mageGuild.level1);
		    if(WIZARD == castle->race && castle->building & BUILD_SPEC)
			FillSpellLevel1(WIZARD, ptrCastle[i].mageGuild.level1);
		    break;
		    
		case MAGIC_LEVEL1:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD2));
		    ptrCastle[i].building |= BUILD_MAGEGUILD2;
		    FillSpellLevel2(ptrCastle[i].race, ptrCastle[i].mageGuild.level2);
		    if(WIZARD == castle->race && castle->building & BUILD_SPEC)
			FillSpellLevel2(WIZARD, ptrCastle[i].mageGuild.level2);
		    break;

		case MAGIC_LEVEL2:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD3));
		    ptrCastle[i].building |= BUILD_MAGEGUILD3;
		    FillSpellLevel3(ptrCastle[i].race, ptrCastle[i].mageGuild.level3);
		    if(WIZARD == castle->race && castle->building & BUILD_SPEC)
			FillSpellLevel3(WIZARD, ptrCastle[i].mageGuild.level3);
		    break;

		case MAGIC_LEVEL3:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD4));
		    ptrCastle[i].building |= BUILD_MAGEGUILD4;
		    FillSpellLevel4(ptrCastle[i].race, ptrCastle[i].mageGuild.level4);
		    if(WIZARD == castle->race && castle->building & BUILD_SPEC)
			FillSpellLevel4(WIZARD, ptrCastle[i].mageGuild.level4);
		    break;

		case MAGIC_LEVEL4:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD5));
		    ptrCastle[i].building |= BUILD_MAGEGUILD5;
		    FillSpellLevel5(ptrCastle[i].race, ptrCastle[i].mageGuild.level5);
		    if(WIZARD == castle->race && castle->building & BUILD_SPEC)
			FillSpellLevel5(WIZARD, ptrCastle[i].mageGuild.level5);
		    break;
		    
		case MAGIC_LEVEL5:
		    return FALSE;
		    break;
	    }

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}
	
    return FALSE;
}

BOOL BuildRightTurret(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_RIGHTTURRET;
	    
	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_RIGHTTURRET));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildLeftTurret(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_LEFTTURRET;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_LEFTTURRET));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildTavern(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_TAVERN;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_TAVERN));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildStatue(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_STATUE;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_STATUE));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildMarketplace(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_MARKETPLACE;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MARKETPLACE));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildThievesGuild(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_THIEVESGUILD;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_THIEVESGUILD));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildWell(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_WELL;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_WELL));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildMoat(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_MOAT;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MOAT));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildWel2(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_WEL2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_WEL2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildSpec(const S_CASTLE *castle){

    Uint8 i, j;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_SPEC;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_SPEC));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);
	    
	        for(j = 0; j < countCastle; ++j)

		    if(&ptrCastle[j] == castle && WIZARD == castle->race && MAGIC_NONE != GetMageGuildLevel(castle))

			switch(GetMageGuildLevel(castle)){
			
			    case MAGIC_LEVEL5:
				FillSpellLevel1(WIZARD, ptrCastle[j].mageGuild.level1);
				FillSpellLevel2(WIZARD, ptrCastle[j].mageGuild.level2);
				FillSpellLevel3(WIZARD, ptrCastle[j].mageGuild.level3);
				FillSpellLevel4(WIZARD, ptrCastle[j].mageGuild.level4);
				FillSpellLevel5(WIZARD, ptrCastle[j].mageGuild.level5);
				break;

			    case MAGIC_LEVEL4:
				FillSpellLevel1(WIZARD, ptrCastle[j].mageGuild.level1);
				FillSpellLevel2(WIZARD, ptrCastle[j].mageGuild.level2);
				FillSpellLevel3(WIZARD, ptrCastle[j].mageGuild.level3);
				FillSpellLevel4(WIZARD, ptrCastle[j].mageGuild.level4);
				break;

			    case MAGIC_LEVEL3:
				FillSpellLevel1(WIZARD, ptrCastle[j].mageGuild.level1);
				FillSpellLevel2(WIZARD, ptrCastle[j].mageGuild.level2);
				FillSpellLevel3(WIZARD, ptrCastle[j].mageGuild.level3);
				break;

			    case MAGIC_LEVEL2:
				FillSpellLevel1(WIZARD, ptrCastle[j].mageGuild.level1);
				FillSpellLevel2(WIZARD, ptrCastle[j].mageGuild.level2);
				break;

			    case MAGIC_LEVEL1:
				FillSpellLevel1(WIZARD, ptrCastle[j].mageGuild.level1);
				break;
			
			    default:
				break;
			}

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildShipyard(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_SHIPYARD;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_SHIPYARD));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildCaptain(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_CAPTAIN;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_CAPTAIN));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildCastle(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_CASTLE;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_CASTLE));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling1(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER1;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER1));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling2(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling3(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER3;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER3));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling4(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER4;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER4));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling5(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER5;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER5));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling6(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER6;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade2(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade3(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE3;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE3));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade4(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE4;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade5(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE5;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE5));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade6(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE6;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE6));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade7(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE7;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE7));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

void UpdateCastleBuilding(void){

    const S_CASTLE *castle = GetCurrentCastle();
    
    CursorOff();
    SetIntValue(ANIM3, FALSE);

    FreeAnimationEvent(castanim);
    castanim = NULL;
    RemoveActionLevelEvent(castlact, LEVELEVENT_CASTLEUPDATEBUILD);

    RedrawNNNNCastle(castle);

    // обновим рамку
    RedrawRamkaCastleName();
    
    // раз купили, так обновим инфо
    RedrawCastleInfoResource();

    flagUpdateBuilding = FALSE;
    SetIntValue(ANIM3, TRUE);
    CursorOn();
}

void EnableCastleUpdateBuilding(void){

    flagUpdateBuilding = TRUE;
}

void RedrawRamkaCastleName(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rectCur;
    AGGSPRITE sprite;
    Uint16 cx = 0;
    Uint16 cy = 0;
    
    const S_CASTLE *castle = GetCurrentCastle();

    if(GetIntValue(VIDEOMODE)){
        cx = video->w / 2 - 320;
	cy = video->h / 2 - 240;
    }

    // рисуем сетку
    FillSPRITE(&sprite, "STRIP.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 256;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рамка наименования
    FillSPRITE(&sprite, "TOWNNAME.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 320 - image->w / 2;
    rectCur.y = cy + 248;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // наименование замка
    rectCur.x = cx + 320 - GetLengthText(castle->name, FONT_SMALL) / 2 ;
    rectCur.y = cy + 248;
    rectCur.w = GetLengthText(castle->name, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, castle->name, FONT_SMALL);

    if(HEROESNULL == castle->hero){
	FillSPRITE(&sprite, "STRIP.ICN", 11);
	image = GetICNSprite(&sprite);
	rectCur.x = cx + 112;
	rectCur.y = cy + 361;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }
}

void CastleIncreaseArmy(const S_CASTLE *castle, E_DWELLINGCASTLE dwelling, Uint8 count){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    switch(dwelling){
	    
		case DWELLING_MONSTER1:
		    ptrCastle[i].monster[0] += count;
		    break;

		case DWELLING_MONSTER2:
		    ptrCastle[i].monster[1] += count;
		    break;

		case DWELLING_MONSTER3:
		    ptrCastle[i].monster[2] += count;
		    break;

		case DWELLING_MONSTER4:
		    ptrCastle[i].monster[3] += count;
		    break;

		case DWELLING_MONSTER5:
		    ptrCastle[i].monster[4] += count;
		    break;

		case DWELLING_MONSTER6:
		    ptrCastle[i].monster[5] += count;
		    break;
		
		default:
		    break;
	    }
	    
	    return;
	}
}

Uint8 GetMonsterGrownCastle(const S_CASTLE *castle, E_MONSTER name){

    Uint8 grown = GetMonsterGrown(name);

        if(castle){
            // well
            if(castle->building & BUILD_WELL) grown +=2;
            // wel2
            if(GOBLIN == name && castle->building & BUILD_WEL2) grown += 8;
            if(PEASANT == name && castle->building & BUILD_WEL2) grown += 8;
            if(SKELETON == name && castle->building & BUILD_WEL2) grown += 8;
            if(SPRITE == name  && castle->building & BUILD_WEL2) grown += 8;
            if(HALFLING == name && castle->building & BUILD_WEL2) grown += 8;
            if(CENTAUR == name && castle->building & BUILD_WEL2) grown += 8;
        }

    return grown;
}

void AllCastleIncreaseRecrut(void){

    Uint8 i;

    for(i = 0; i < countCastle; ++i){

	if(ptrCastle[i].dwelling & DWELLING_MONSTER1) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER1, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 1)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER2 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE2) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER2, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 2)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER3 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE3) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER3, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 3)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER4 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE4) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER4, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 4)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER5 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE5) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER5, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 5)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER6 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE6 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE7) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER6, GetMonsterGrownCastle(&ptrCastle[i], GetMonsterFromCastle(&ptrCastle[i], 6)));
    }
}

E_MAGICLEVEL GetMageGuildLevel(const S_CASTLE *castle){

    if(castle->building & BUILD_MAGEGUILD5) return MAGIC_LEVEL5;
    if(castle->building & BUILD_MAGEGUILD4) return MAGIC_LEVEL4;
    if(castle->building & BUILD_MAGEGUILD3) return MAGIC_LEVEL3;
    if(castle->building & BUILD_MAGEGUILD2) return MAGIC_LEVEL2;
    if(castle->building & BUILD_MAGEGUILD1) return MAGIC_LEVEL1;

    return MAGIC_NONE;
}

    
ACTION UpgradableArmy(const S_ARMY *army, E_COLORS color, const S_CASTLE *castle){
    
    E_MONSTER upgradeMonster = GetUpgradeMonster(army->monster);
    
    if(army->monster == upgradeMonster) return EXIT;
    
    // check upgrade building
    if(!castle) return EXIT;

    Uint8 i;
    BOOL upgrade = FALSE;

    for(i = 1; i < CASTLEMAXMONSTER; ++i)
	if(upgradeMonster == GetMonsterFromCastle(castle, i + 1))
	    switch(i + 1){
	    
		case 2:
		    if(castle->dwelling & DWELLING_UPGRADE2) upgrade = TRUE;
		    break;

		case 3:
		    if(castle->dwelling & DWELLING_UPGRADE3) upgrade = TRUE;
		    break;

		case 4:
		    if(castle->dwelling & DWELLING_UPGRADE4) upgrade = TRUE;
		    break;

		case 5:
		    if(castle->dwelling & DWELLING_UPGRADE5) upgrade = TRUE;
		    break;

		case 6:
		    if(castle->dwelling & DWELLING_UPGRADE7) upgrade = TRUE;
		    else if(GREEN_DRAGON == army->monster && castle->dwelling & DWELLING_UPGRADE6) upgrade = TRUE;
		    break;
		
		default:
		    break;
	    }

    // check resource
    if(upgrade && KingdomAllowPayment(color, GetMultiPayment(PaymentConditionsUpgradeMonster(army->monster, upgradeMonster), army->count * RATE_DIFFERENCE_UPGRADE))) return YES;

    return NO;
}

void UpgradeArmy(S_ARMY *army, E_COLORS color){

    if(! army) return;

    E_MONSTER upgradeMonster = GetUpgradeMonster(army->monster);
    
    KingdomWasteResource(color, GetMultiPayment(PaymentConditionsUpgradeMonster(army->monster, upgradeMonster), army->count * RATE_DIFFERENCE_UPGRADE));
    
    army->monster = upgradeMonster;
}

BOOL CastlePresentHeroes(const S_CASTLE *castle){

    return TRUE;
}

// фуункция рисует составную картинку замка
void DrawAmountedCastle(SDL_Surface *surface, SDL_Rect *rect, E_RACE race, E_GROUND ground, BOOL castle){

/*
спрайт замка  OBJNTOWN.ICN 32x32 kngt barb srcr wrlc wizrd necr
спрайт грунта OBJNTWBA.ICN 32x32 grass snow swamp lava desert dirt wast beach

XXX XXX C00 XXX XXX     XXX XXX XXX XXX XXX
C01 C02 C03 C04 C05     XXX XXX XXX XXX XXX
C06 C07 C08 C09 C10     XXX XXX XXX XXX XXX
C11 C12 C13 C14 C15	G00 G01 G02 G03 G04
XXX XXX XXX XXX XXX     G05 G06 G07 G08 G09
*/

    SDL_Rect cur;
    AGGSPRITE sprite;
    ICNHEADER *header;
    Uint8 index, i;
    // невыполнимые условия (160 ширина нарисованой картинки)
    if(! surface || ! rect) return;

    // рисуем грунт под замком
    switch(ground){
	case GRASS:
	    index = 0;
	    break;
	case SNOW:
	    index = 10;
	    break;
	case SWAMP:
	    index = 20;
	    break;
	case LAVA:
	    index = 30;
	    break;
	case DESERT:
	    index = 40;
	    break;
	case DIRT:
	    index = 50;
	    break;
	case WASTELAND:
	    index = 60;
	    break;
	case BEACH:
	    index = 70;
	    break;
	default:
	    return;
	    break;
    }
    for(i = 0; i < 5; ++i){
	FillSPRITE(&sprite, "OBJNTWBA.ICN", index + i);
	header = GetICNHeader(&sprite);
	cur.x = rect->x + i * 32;
	cur.y = rect->y + 3 * 32;
	cur.x += header->offsetX;
	cur.y += header->offsetY;
	cur.w = header->surface->w;
	cur.h = header->surface->h;
	SDL_BlitSurface(header->surface, NULL, surface, &cur);
    }
    for(i = 0; i < 5; ++i){
	FillSPRITE(&sprite, "OBJNTWBA.ICN", index + 5 + i);
	header = GetICNHeader(&sprite);
	cur.x = rect->x + i * 32;
	cur.y = rect->y + 4 * 32;
	cur.x += header->offsetX;
	cur.y += header->offsetY;
	cur.w = header->surface->w;
	cur.h = header->surface->h;
	SDL_BlitSurface(header->surface, NULL, surface, &cur);
    }

    // рисуем замоk
    switch(race){
	case KNIGHT:
	    index = 0;
	    break;
	case BARBARIAN:
	    index = 32;
	    break;
	case SORCERESS:
	    index = 64;
	    break;
	case WARLOCK:
	    index = 96;
	    break;
	case WIZARD:
	    index = 128;
	    break;
	case NECROMANCER:
	    index = 160;
	    break;
	default:
	    break;
    }
    if(! castle) index += 16;

    cur.y = rect->y;
    cur.x = rect->x + 2 * 32;
    FillSPRITE(&sprite, "OBJNTOWN.ICN", index);
    header = GetICNHeader(&sprite);
    cur.x += header->offsetX;
    cur.y += header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, surface, &cur);
    for(i = 0; i < 5; ++i){
	FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 1 + i);
	header = GetICNHeader(&sprite);
	cur.x = rect->x + i * 32;
	cur.y = rect->y + 32;
	cur.x += header->offsetX;
	cur.y += header->offsetY;
	cur.w = header->surface->w;
	cur.h = header->surface->h;
	SDL_BlitSurface(header->surface, NULL, surface, &cur);
    }
    for(i = 0; i < 5; ++i){
	FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 6 + i);
	header = GetICNHeader(&sprite);
	cur.x = rect->x + i * 32;
	cur.y = rect->y + 2 * 32;
	cur.x += header->offsetX;
	cur.y += header->offsetY;
	cur.w = header->surface->w;
	cur.h = header->surface->h;
	SDL_BlitSurface(header->surface, NULL, surface, &cur);
    }
    for(i = 0; i < 5; ++i){
	FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 11 + i);
	header = GetICNHeader(&sprite);
	cur.x = rect->x + i * 32;
	cur.y = rect->y + 3 * 32;
	cur.x += header->offsetX;
	cur.y += header->offsetY;
	cur.w = header->surface->w;
	cur.h = header->surface->h;
	SDL_BlitSurface(header->surface, NULL, surface, &cur);
    }
}

void RedrawNNNNCastle(const S_CASTLE *castle){

    switch(castle->race){

	case KNIGHT:
	    DrawKNGTCastle(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawKNGTCapitan(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawKNGTWel2(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawKNGTLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawKNGTRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawKNGTMoat(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawKNGTMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawKNGTDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawKNGTThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawKNGTTavern(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawKNGTMageGuild(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawKNGTDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawKNGTDwelling6(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawKNGTDwelling1(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawKNGTDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawKNGTDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawKNGTWell(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawKNGTStatue(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawKNGTShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawKNGTExt0(&castanim, &castlact);
	    //DrawKNGTExt1(&castanim, &castlact); // развилка дорог?
	    //DrawKNGTExt2(&castanim, &castlact); // развилка дорог?
	    break;

	case BARBARIAN:
	    if(castle->building & BUILD_SPEC) DrawBRBNSpec(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawBRBNWel2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawBRBNDwelling6(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)){
		DrawBRBNMageGuild(&castanim, &castlact);
		DrawBRBNExt2(&castanim, &castlact);
	    }
	    if(castle->building & BUILD_CAPTAIN) DrawBRBNCapitan(&castanim, &castlact);
	    DrawBRBNCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawBRBNLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBRBNRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawBRBNMoat(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawBRBNDwelling3(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawBRBNThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawBRBNTavern(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawBRBNDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawBRBNMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawBRBNDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawBRBNWell(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawBRBNStatue(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawBRBNShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawBRBNExt0(&castanim, &castlact);
	    DrawBRBNExt1(&castanim, &castlact);
	    //DrawBRBNExt3(&castanim, &castlact); // развилка дорог?
	    break;

	case SORCERESS:
	    if(castle->building & BUILD_SPEC) DrawSCRSSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawSCRSDwelling6(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawSCRSMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawSCRSCapitan(&castanim, &castlact);
	    DrawSCRSCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawSCRSLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawSCRSRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawSCRSMoat(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawSCRSDwelling3(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawSCRSShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawSCRSExt0(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawSCRSMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawSCRSDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawSCRSThievesGuild(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawSCRSDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawSCRSTavern(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE && castle->building & BUILD_WEL2) DrawSCRSExt1(&castanim, &castlact);
	    else if(castle->building & BUILD_STATUE) DrawSCRSStatue(&castanim, &castlact);
	    else if(castle->building & BUILD_WEL2) DrawSCRSWel2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawSCRSDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawSCRSWell(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawSCRSDwelling5(&castanim, &castlact);
	break;

	case NECROMANCER:
	    if(castle->building & BUILD_SPEC) DrawNCRMSpec(&castanim, &castlact);
	    DrawNCRMCastle(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawNCRMCapitan(&castanim, &castlact);
    	    if(castle->building & BUILD_LEFTTURRET) DrawNCRMLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawNCRMRTurret(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawNCRMDwelling6(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawNCRMMoat(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawNCRMShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawNCRMExt0(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawNCRMThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawNCRMTavern(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawNCRMDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawNCRMDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawNCRMDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawNCRMDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawNCRMDwelling1(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawNCRMMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawNCRMWel2(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawNCRMMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawNCRMStatue(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawNCRMWell(&castanim, &castlact);
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWRLKDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWRLKDwelling3(&castanim, &castlact);
	    DrawWRLKCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWRLKLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWRLKRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWRLKCapitan(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWRLKMoat(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWRLKShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawWRLKExt0(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawWRLKMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWRLKTavern(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWRLKThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWRLKStatue(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWRLKDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWRLKWel2(&castanim, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWRLKSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWRLKDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWRLKDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawWRLKWell(&castanim, &castlact);
	break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWZRDDwelling6(&castanim, &castlact);
	    DrawWZRDCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWZRDLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWZRDRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWZRDMoat(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWZRDCapitan(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWZRDDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWZRDThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWZRDTavern(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWZRDShipyard(&castanim, &castlact);
	    else if(CastleNearOcean(castle)) DrawWZRDExt0(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawWZRDWell(&castanim, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWZRDSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWZRDDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWZRDDwelling5(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawWZRDMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWZRDStatue(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWZRDDwelling1(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWZRDDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWZRDMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWZRDWel2(&castanim, &castlact);
	break;
	
	default:
	return;
	break;
    }
}

ACTION ActionOverHeroesArmy(void){

    Sint32 mx;
    
    const char *message = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    const S_HEROES *heroes = GetStatHeroes(GetHeroesFromCastle(GetCurrentCastle()));
    if(! heroes) return NONE;
    
    SDL_GetMouseState(&mx, NULL);

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;

    Uint8 index = (Uint16) (mx - cx) / 88;

    if(MONSTERNONE != heroes->army[index].monster){
	if(! selectCastleArmy.selector->use)
	    message = "Select Army";
	else if(index == selectCastleArmy.select && !selectCastleArmy.castle)
	    message = "View Army";
	else
	    message = "Change Army";
    }else
	message = (selectCastleArmy.selector->use ? "Left click Move Army, Right click Redistribute Army" : "Empty");

    CursorOff();
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);
    CursorOn();

    return NONE;
}

ACTION ActionOverCastleArmy(void){

    Sint32 mx;
    
    const S_CASTLE *castle = GetCurrentCastle();
    const char *message = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;

    if( ! castle) return NONE;
    
    SDL_GetMouseState(&mx, NULL);

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;

    Uint8 index = (Uint16) (mx - cx) / 88;

    if(MONSTERNONE != castle->army[index].monster){
	if(! selectCastleArmy.selector->use)
	    message = "Select Army";
	else if(index == selectCastleArmy.select && selectCastleArmy.castle)
	    message = "View Army";
	else
	    message = "Change Army";
    }else
	message = (selectCastleArmy.selector->use ? "Left click Move Army, Right click Redistribute Army" : "Empty");

    CursorOff();
    cur.x = video->w / 2;
    cur.y = video->h / 2 + 240 - BORDERWIDTH;

    cur.x = cur.x - GetLengthText(message, FONT_BIG) / 2;
    cur.y = cur.y - 3;
    cur.w = GetLengthText(message, FONT_BIG);
    cur.h = FONT_HEIGHTBIG;
    PrintText(video, &cur, message, FONT_BIG);
    CursorOn();
    
    return NONE;
}

E_NAMEHEROES GetHeroesFromCastle(const S_CASTLE *castle){

    if(! castle) return HEROESNULL;
    
    return castle->hero;
}

const S_CASTLE *HeroesInCastle(const S_HEROES *heroes){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].ax == heroes->ax && ptrCastle[i].ay == heroes->ay)
	    return &ptrCastle[i];

    return NULL;
}

void ResetCastleSelector(void){

    ResetSelector(selectCastleArmy.selector);
}
