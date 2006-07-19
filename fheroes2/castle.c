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
#include "monster.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"
#include "castleaction.h"
#include "castle_kngt.h"
#include "castle_brbn.h"
#include "castle_scrs.h"
#include "castle_ncrm.h"
#include "castle_wrlk.h"
#include "castle_wzrd.h"

ACTION ActionClickCastleMonster(void);
ACTION ActionClickHeroesMonster(void);
ACTION ActionOverHeroes(void);
ACTION ActionViewHeroes(void);
ACTION ActionOverExit(void);
void RedrawCastleAnimation(void);
void RedrawBottomBar(void);
ACTION ActionExitCastle(void); 
ACTION ActionLeftCastle(void); 
ACTION ActionRightCastle(void); 
ACTION ActionCASTLELOOP(INTERFACEACTION *); 


static 	S_CASTLE	*ptrCastle	= NULL;
static 	Uint8		countCastle	= 0;
	S_ANIMATION    	*castanim	= NULL;
const	S_CASTLE	*currentCastle	= NULL;
	INTERFACEACTION *dialogCastle	= NULL;

struct {
    Uint8		select;
    SDL_Rect            rect;
    SDL_Surface         *surface;
    BOOL                use;
} backMonsterCursor;

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
	    ptrCastle[countCastle].building = BUILD_TAVERN; // BUILD_THIEVEGUILD BUILD_TAVERN BUILD_SHIPYARD BUILD_WELL BUILD_STATUE BUILD_LEFTTURRET BUILD_RIGHTTURRET BUILD_MARKETPLACE BUILD_MOAT BUILD_EXT1 BUILD_SPEC
	    ptrCastle[countCastle].dwelling = DWELLING_MONSTER1 | DWELLING_MONSTER2; // DWELLING_MONSTER1 DWELLING_MONSTER2 DWELLING_MONSTER3 DWELLING_MONSTER4 DWELLING_MONSTER5 DWELLING_MONSTER6 DWELLING_UPGRADE2 DWELLING_UPGRADE3 DWELLING_UPGRADE4 DWELLING_UPGRADE5 DWELLING_UPGRADE6
	}

	ptrCastle[countCastle].magicTower = ptr->magicTower;

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
	    switch(ptrCastle[countCastle].race){
	
		case KNIGHT:
		    ptrCastle[countCastle].army[0].monster = PEASANT;
		    ptrCastle[countCastle].army[1].monster = ARCHER;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;

		case BARBARIAN:
		    ptrCastle[countCastle].army[0].monster = GOBLIN;
		    ptrCastle[countCastle].army[1].monster = ORC;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;

		case SORCERESS:
		    ptrCastle[countCastle].army[0].monster = SPRITE;
		    ptrCastle[countCastle].army[1].monster = DWARF;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;

		case WARLOCK:
		    ptrCastle[countCastle].army[0].monster = CENTAUR;
		    ptrCastle[countCastle].army[1].monster = GARGOYLE;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;

		case WIZARD:
		    ptrCastle[countCastle].army[0].monster = HALFLING;
		    ptrCastle[countCastle].army[1].monster = BOAR;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;

		case NECROMANCER:
		    ptrCastle[countCastle].army[0].monster = SKELETON;
		    ptrCastle[countCastle].army[1].monster = ZOMBIE;
		    ptrCastle[countCastle].army[0].count = 28;
		    ptrCastle[countCastle].army[1].count = 7;
		    break;
	
		default:
		    break;
	    }

	    ptrCastle[countCastle].army[2].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[3].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[4].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[2].count = 0;
	    ptrCastle[countCastle].army[3].count = 0;
	    ptrCastle[countCastle].army[4].count = 0;
	}

    ptrCastle[countCastle].capitan = ptr->capitan;

    if(ptr->castle) ptrCastle[countCastle].castle = TRUE; else ptrCastle[countCastle].castle = FALSE;

    if(ptr->allowCastle)
        ptrCastle[countCastle].allowCastle = FALSE;
    else
        ptrCastle[countCastle].allowCastle = TRUE;

    if(ax < 5) ptrCastle[countCastle].pos.x = 0; else ptrCastle[countCastle].pos.x = ax - 5;
    if(ay < 3) ptrCastle[countCastle].pos.y = 0; else ptrCastle[countCastle].pos.y = ay - 3;

    ptrCastle[countCastle].ax = ax;
    ptrCastle[countCastle].ay = ay;
    ptrCastle[countCastle].pos.w = 8;
    ptrCastle[countCastle].pos.h = 5;
    
    Uint8 i;
    for(i = 0; i < CASTLEMAXMONSTER; ++i) ptrCastle[countCastle].monster[i] = 0;

    ptrCastle[countCastle].next = NULL;

    ++countCastle;

    KingdomAddCastle(ptrCastle[countCastle - 1].color, countCastle - 1);

    free(ptr);

    return TRUE;
}

void	FreeCastle(void){

    if(ptrCastle) free(ptrCastle);
    countCastle = 0;
}

E_RACE GetRaceRNDCastle(Uint8 ax, Uint8 ay){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)
	if(ValidPoint(&ptrCastle[i].pos, ax, ay))
	    return ptrCastle[i].race;
	    
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

void EnterCastle(Uint8 ax, Uint8 ay, E_NAMEHEROES nameHeroes){

    // определяем тип замка
    SDL_Surface *format, *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    S_KINGDOM *kingdom = NULL;
    S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_HEROES *heroes = GetStatHeroes(nameHeroes);

    // в серые замки не заходим
    if(GRAY == castle->color) return;
    currentCastle = castle;

    // инициализируем backgroundCursor
    FillSPRITE(&sprite, "STRIP.ICN", 1);
    image = GetICNSprite(&sprite);
    backMonsterCursor.select = 0xFF;
    backMonsterCursor.surface = NULL;
    backMonsterCursor.use = FALSE;
    backMonsterCursor.rect.x = 0;
    backMonsterCursor.rect.y = 0;
    backMonsterCursor.rect.w = image->w;
    backMonsterCursor.rect.h = image->h;

    // переопределяем курсор и выключаем анимацию карты
    SetIntValue(ANIM1, FALSE);
    CursorOff();
    Uint32 cursor = GetCursor();
    INTERFACEACTION action;

    char message[8];
    char number[5];
    char icnstring[13];

    Uint16 cx, cy;
    Uint8 i, j;

    // рисуем бакгроунд 640х480
    // если videomode более то рисуем бордюр + тень и по центру экрана
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
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "EnterCastle: CreateRGBSurface failed: %s\n", SDL_GetError());
        return;
    }

    // сохраняем бакгроунд
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // регистрируем пустое событие

    ZeroINTERFACEACTION(&action);
    action.rect.x = 0;
    action.rect.y = 0;
    action.rect.w = 1;
    action.rect.h = 1;
    action.mouseEvent = MOUSE_LCLICK;
    AddActionEvent(&dialogCastle, &action);

    // рисуем бордюр
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);

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
    rectCur.x = 0;
    rectCur.y = 0;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH;
	rectCur.y = rectBack.y + BORDERWIDTH;
    }
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рисуем сетку
    FillSPRITE(&sprite, "STRIP.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y;

    // левый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SMALLBAR.ICN", 1);
    FillSPRITE(&action.objectPush, "SMALLBAR.ICN", 2);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionLeftCastle;
    AddActionEvent(&dialogCastle, &action);

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
    AddActionEvent(&dialogCastle, &action);

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
    if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
    rectCur.y = cy + 6;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // рисуем фон ячеек для монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    cx = rectCur.x + rectCur.w + 6;
    cy = rectCur.y;
    for(i = 0; i < CASTLEMAXARMY; ++i){

	// если есть в замке монстры
	if(castle->army[i].count){
		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(castle->army[i].monster)){
		
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

		// регистрируем событие на click
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickCastleMonster;
		//action.level = LEVELEVENT_CASTLEMONSTER;
		AddActionEvent(&dialogCastle, &action);

		// рисуем монстров
		memset(number, 0, strlen(number) + 1);
		memset(icnstring, 0, strlen(icnstring) + 1);
    		sprintf(number, "%4d", castle->army[i].monster);
    		for(j = 0; j < 4; j++)
		    if(0 == strncmp(&number[j], " ", 1)) number[j] = '0';
    		sprintf(icnstring, "MONH%4s.ICN", number);
		FillSPRITE(&sprite, icnstring, 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
	        memset(message, 0, strlen(message));
		sprintf(message, "%5d", castle->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = FONT_WIDTHSMALL * strlen(message);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
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

    // рисуем фон ячейки героя
    if(heroes){
	memset(number, 0, strlen(number) + 1);
	memset(icnstring, 0, strlen(icnstring) + 1);
        sprintf(number, "%4d", nameHeroes);
        for(i = 0; i < 4; i++)
	    if(0 == strncmp(&number[i], " ", 1)) number[i] = '0';
        sprintf(icnstring, "PORT%4s.ICN", number);
	FillSPRITE(&sprite, icnstring, 0);

	// наведение мыши
	if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
	rectCur.y = cy + image->w - 2;
	rectCur.w = image->w;
	rectCur.h = image->h;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverHeroes;
	AddActionEvent(&dialogCastle, &action);
	// клик
	ZeroINTERFACEACTION(&action);
        action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionViewHeroes;
	AddActionEvent(&dialogCastle, &action);

    }else if(castle->capitan){
	memset(number, 0, strlen(number) + 1);
	memset(icnstring, 0, strlen(icnstring) + 1);
        sprintf(number, "%4d", castle->race + 89);
        for(i = 0; i < 4; i++)
	    if(0 == strncmp(&number[i], " ", 1)) number[i] = '0';
        sprintf(icnstring, "PORT%4s.ICN", number);
	FillSPRITE(&sprite, icnstring, 0);
    }else
	FillSPRITE(&sprite, "STRIP.ICN", 3);

    image = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
    rectCur.y = cy + image->w - 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = rectCur.x + rectCur.w + 6;
	cy = rectCur.y;

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

		// регистрируем событие на click
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickHeroesMonster;
		//action.level = LEVELEVENT_HEROESMONSTER;
		AddActionEvent(&dialogCastle, &action);

		// рисуем монстров
		memset(number, 0, strlen(number) + 1);
		memset(icnstring, 0, strlen(icnstring) + 1);
    		sprintf(number, "%4d", heroes->army[i].monster);
    		for(j = 0; j < 4; j++)
		    if(0 == strncmp(&number[j], " ", 1)) number[j] = '0';
    		sprintf(icnstring, "MONH%4s.ICN", number);
		FillSPRITE(&sprite, icnstring, 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
	        memset(message, 0, strlen(message));
		sprintf(message, "%5d", heroes->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = FONT_WIDTHSMALL * strlen(message);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);

	    }else{
		FillSPRITE(&sprite, "STRIP.ICN", 2);
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);
	    }
    }else{
	FillSPRITE(&sprite, "STRIP.ICN", 11);
	image = GetICNSprite(&sprite);
	rectCur.x += rectCur.w + 6;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->wood);
    rectCur.x = cx + 8;
    rectCur.y = cy + 32;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->sulfur);
    rectCur.x = cx + 48;
    rectCur.y = cy + 32;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->crystal);
    rectCur.x = cx + 8;
    rectCur.y = cy + 78;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->mercury);
    rectCur.x = cx + 48;
    rectCur.y = cy + 78;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->ore);
    rectCur.x = cx + 8;
    rectCur.y = cy + 118;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%5d", kingdom->gems);
    rectCur.x = cx + 48;
    rectCur.y = cy + 118;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
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
    memset(message, 0, strlen(message));
    sprintf(message, "%7d", kingdom->gold);
    rectCur.x = cx + 24;
    rectCur.y = cy + 154;
    rectCur.w = FONT_WIDTHSMALL * strlen(message);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);

    // кнопка exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx +  1;
    rectCur.y = cy + 166;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение мыши
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverExit;
    AddActionEvent(&dialogCastle, &action);
    // клик
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SWAPBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "SWAPBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionExitCastle;
    AddActionEvent(&dialogCastle, &action);

    switch(castle->race){

	case KNIGHT:
	    if(castle->capitan) DrawKNGTCapitan(&castanim, &dialogCastle);
	    DrawKNGTCastle(&castanim, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawKNGTLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawKNGTRTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawKNGTMoat(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawKNGTMarketplace(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawKNGTUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawKNGTDwelling2(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawKNGTThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawKNGTTavern(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawKNGTMageGuild(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawKNGTUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawKNGTDwelling5(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawKNGTUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawKNGTDwelling6(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawKNGTDwelling1(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawKNGTUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawKNGTDwelling3(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawKNGTUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawKNGTDwelling4(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawKNGTWell(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawKNGTStatue(&castanim, &dialogCastle);
	    // если рядом море
	    if(castle->building & BUILD_SHIPYARD) DrawKNGTShipyard(&castanim, &dialogCastle);
	    else DrawKNGTExt0(&castanim, &dialogCastle);
	    break;

	case BARBARIAN:
	    if(castle->building & BUILD_SPEC) DrawBRBNSpec(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawBRBNUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawBRBNDwelling6(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawBRBNMageGuild(&castanim, &dialogCastle);
	    if(castle->capitan) DrawBRBNCapitan(&castanim, &dialogCastle);
	    DrawBRBNCastle(&castanim, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBRBNLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBRBNRTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawBRBNMoat(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawBRBNUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawBRBNDwelling3(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBRBNThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawBRBNTavern(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawBRBNDwelling1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBRBNMarketplace(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawBRBNUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawBRBNDwelling2(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawBRBNUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawBRBNUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawBRBNWell(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawBRBNStatue(&castanim, &dialogCastle);
	    // учесть что анимация перерисовывает мост и таверну
	    //if(castle->building & BUILD_EXT1) DrawBRBNExt1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SHIPYARD) DrawBRBNShipyard(&castanim, &dialogCastle);
	    else DrawBRBNExt0(&castanim, &dialogCastle);
	    break;

	case SORCERESS:
	    if(castle->building & BUILD_SPEC) DrawSCRSSpec(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawSCRSUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawSCRSDwelling6(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawSCRSMageGuild(&castanim, &dialogCastle);
	    if(castle->capitan) DrawSCRSCapitan(&castanim, &dialogCastle);
	    DrawSCRSCastle(&castanim, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawSCRSLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawSCRSRTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawSCRSMoat(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawSCRSUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawSCRSDwelling3(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SHIPYARD) DrawSCRSShipyard(&castanim, &dialogCastle);
	    else DrawSCRSExt0(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawSCRSMarketplace(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawSCRSDwelling1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawSCRSTavern(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawSCRSUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawSCRSDwelling2(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawSCRSThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawSCRSStatue(&castanim, &dialogCastle);
	    if(castle->building & BUILD_EXT1) DrawSCRSExt1(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawSCRSUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawSCRSDwelling4(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawSCRSWell(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawSCRSUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawSCRSDwelling5(&castanim, &dialogCastle);
	break;

	case NECROMANCER:
	    if(castle->building & BUILD_SPEC) DrawNCRMSpec(&castanim, &dialogCastle);
	    DrawNCRMCastle(&castanim, &dialogCastle);
	    if(castle->capitan) DrawNCRMCapitan(&castanim, &dialogCastle);
    	    if(castle->building & BUILD_LEFTTURRET) DrawNCRMLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawNCRMRTurret(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawNCRMUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawNCRMDwelling6(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawNCRMMoat(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SHIPYARD) DrawNCRMShipyard(&castanim, &dialogCastle);
	    else DrawNCRMExt0(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawNCRMThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawNCRMTavern(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawNCRMUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawNCRMDwelling3(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawNCRMUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawNCRMDwelling5(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawNCRMUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawNCRMDwelling2(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawNCRMUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawNCRMDwelling4(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawNCRMDwelling1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawNCRMMarketplace(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawNCRMMageGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_EXT1) DrawNCRMExt1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawNCRMStatue(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawNCRMWell(&castanim, &dialogCastle);
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawWRLKUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawWRLKDwelling5(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawWRLKUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawWRLKDwelling3(&castanim, &dialogCastle);
	    DrawWRLKCastle(&castanim, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawWRLKLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWRLKRTurret(&castanim, &dialogCastle);
	    if(castle->capitan) DrawWRLKCapitan(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawWRLKMoat(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SHIPYARD) DrawWRLKShipyard(&castanim, &dialogCastle);
	    else DrawWRLKExt0(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawWRLKMageGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawWRLKThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawWRLKTavern(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawWRLKStatue(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWRLKDwelling1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_EXT1) DrawWRLKExt1(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SPEC) DrawWRLKSpec(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawWRLKUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawWRLKDwelling4(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawWRLKUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawWRLKDwelling2(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawWRLKUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawWRLKWell(&castanim, &dialogCastle);
	break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE6) DrawWZRDUpgrade6(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER6) DrawWZRDDwelling6(&castanim, &dialogCastle);
	    DrawWZRDCastle(&castanim, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawWZRDLTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWZRDRTurret(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MOAT) DrawWZRDMoat(&castanim, &dialogCastle);
	    if(castle->capitan) DrawWZRDCapitan(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE2) DrawWZRDUpgrade2(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER2) DrawWZRDDwelling2(&castanim, &dialogCastle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawWZRDThievesGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_TAVERN) DrawWZRDTavern(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SHIPYARD) DrawWZRDShipyard(&castanim, &dialogCastle);
	    else DrawWZRDExt0(&castanim, &dialogCastle);
	    if(castle->building & BUILD_WELL) DrawWZRDWell(&castanim, &dialogCastle);
	    if(castle->building & BUILD_SPEC) DrawWZRDSpec(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE3) DrawWZRDUpgrade3(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER3) DrawWZRDDwelling3(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE5) DrawWZRDUpgrade5(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER5) DrawWZRDDwelling5(&castanim, &dialogCastle);
	    if(castle->magicTower) DrawWZRDMageGuild(&castanim, &dialogCastle);
	    if(castle->building & BUILD_STATUE) DrawWZRDStatue(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWZRDDwelling1(&castanim, &dialogCastle);
	    if(castle->dwelling & DWELLING_UPGRADE4) DrawWZRDUpgrade4(&castanim, &dialogCastle);
	    else if(castle->dwelling & DWELLING_MONSTER4) DrawWZRDDwelling4(&castanim, &dialogCastle);
	    if(castle->building & BUILD_MARKETPLACE) DrawWZRDMarketplace(&castanim, &dialogCastle);
	    if(castle->building & BUILD_EXT1) DrawWZRDExt1(&castanim, &dialogCastle);
	break;
	
	default:
	return;
	break;
    }

    // наименование замка
    FillSPRITE(&sprite, "TOWNNAME.ICN", 0);
    image = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + 264;
	rectCur.y = rectBack.y + 264;
    }else{
	rectCur.x = rectBack.x + 264 - BORDERWIDTH * 3;
	rectCur.y = rectBack.y + 264 - BORDERWIDTH;
    }
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    rectCur.x = rectCur.x + 90 - strlen(castle->name) * FONT_WIDTHSMALL / 2 ;
    rectCur.y = rectCur.y + 1;
    rectCur.w = FONT_WIDTHSMALL * strlen(castle->name);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, castle->name, FONT_SMALL);

    SetCursor(CURSOR_POINTER);
    CursorOn();

    ActionCASTLELOOP(dialogCastle);

    CursorOff();
    // чистим данные
    FreeAnimationEvent(castanim);
    FreeActionEvent(dialogCastle);
    if(backMonsterCursor.surface) SDL_FreeSurface(backMonsterCursor.surface);

    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_FreeSurface(back);

    dialogCastle = NULL;
    castanim = NULL;

    // востанавливаем курсор и анимацию карты
    SetCursor(cursor);
    CursorOn();

    SetIntValue(ANIM1, TRUE);
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
 
    while(ptr){ 

        if(ValidPoint(&ptr->rect[animationFrame % ptr->count], x, y)) CursorOff();
 
	// востановить фон предыдущего спрайта
	if(ptr->background){
    	    SDL_BlitSurface(ptr->background, NULL, video, &ptr->rectBack);
	    SDL_FreeSurface(ptr->background);
	    ptr->background = NULL;
	}
	
	if(NULL == (ptr->background = SDL_CreateRGBSurface(SDL_SWSURFACE, ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "RedrawMenuAnimation: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h);
            return;
        }

	// сохраняем фон нового спрайта
	ptr->rectBack = ptr->rect[animationFrame % ptr->count];
    	SDL_BlitSurface(video, &ptr->rectBack, ptr->background, NULL);

	// рисуем спрайт
        SDL_BlitSurface(ptr->surface[animationFrame % ptr->count], NULL, video, &ptr->rectBack);

        CursorOn(); 
        ptr = ptr->next; 
    } 
 
    SDL_Flip(video); 
    SDL_Delay(GetIntValue(ANIMATIONDELAY));     

    ++animationFrame;

    return;
}

void RedrawBottomBar(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;

    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.x = video->w / 2 - 320 + BORDERWIDTH + 4;
    rect.y = video->h / 2 + 240 - BORDERWIDTH - 3;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    SDL_Flip(video);
}
