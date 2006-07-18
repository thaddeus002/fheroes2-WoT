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

void DrawBuildingCastle(S_CASTLE *, INTERFACEACTION **);
void DrawBuildingCapitan(S_CASTLE *);
void DrawBuildingMagicTower(S_CASTLE *);
void DrawBuildingStatue(S_CASTLE *);
void DrawBuildingWell(S_CASTLE *);
void DrawBuildingThieveGuild(S_CASTLE *);
void DrawBuildingTavern(S_CASTLE *);
void DrawBuildingShipyard(S_CASTLE *);
void DrawBuildingLeftTurret(S_CASTLE *);
void DrawBuildingRightTurret(S_CASTLE *);
void DrawBuildingMarketplace(S_CASTLE *);
void DrawBuildingMoat(S_CASTLE *);
void DrawBuildingSpec(S_CASTLE *);
void DrawBuildingExt0(S_CASTLE *);
void DrawBuildingExt1(S_CASTLE *);
void DrawDwelling1(S_CASTLE *);
void DrawDwelling2(S_CASTLE *);
void DrawDwelling3(S_CASTLE *);
void DrawDwelling4(S_CASTLE *);
void DrawDwelling5(S_CASTLE *);
void DrawDwelling6(S_CASTLE *);
void DrawDwellingUpgrade2(S_CASTLE *);
void DrawDwellingUpgrade3(S_CASTLE *);
void DrawDwellingUpgrade4(S_CASTLE *);
void DrawDwellingUpgrade5(S_CASTLE *);
void DrawDwellingUpgrade6(S_CASTLE *);
void DrawDwellingUpgrade7(S_CASTLE *);


void RedrawCastleAnimation(void);

ACTION ActionExitCastle(void);
ACTION ActionLeftCastle(void);
ACTION ActionRightCastle(void);
ACTION ActionCASTLELOOP(INTERFACEACTION *);

static S_CASTLE		*ptrCastle = NULL;
static Uint8		countCastle = 0;
       S_ANIMATION      *castanim = NULL;


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
    S_KINGDOM *kingdom = NULL;
    S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_HEROES *heroes = GetStatHeroes(nameHeroes);

    // в серые замки не заходим
    if(GRAY == castle->color) return;

    // переопределяем курсор и выключаем анимацию карты
    SetIntValue(ANIM1, FALSE);
    CursorOff();
    Uint32 cursor = GetCursor();
    INTERFACEACTION *dialogCastle = NULL;
    INTERFACEACTION action;

    SDL_Surface *format, *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    ICNHEADER *header = NULL;

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
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SWAPBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "SWAPBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionExitCastle;
    AddActionEvent(&dialogCastle, &action);

    switch(castle->race){

	case KNIGHT:
	    if(castle->capitan) DrawBuildingCapitan(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    // если рядом море
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    break;

	case BARBARIAN:
	    if(castle->building & BUILD_SPEC) DrawBuildingSpec(castle);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->capitan) DrawBuildingCapitan(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    // учесть что анимация перерисовывает мост и таверну
	    //if(castle->building & BUILD_EXT1) DrawBuildingExt1(castle);
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    break;

	case SORCERESS:
	    if(castle->building & BUILD_SPEC) DrawBuildingSpec(castle);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->capitan) DrawBuildingCapitan(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    if(castle->building & BUILD_EXT1) DrawBuildingExt1(castle);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	break;

	case NECROMANCER:
	    if(castle->building & BUILD_SPEC) DrawBuildingSpec(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->capitan) DrawBuildingCapitan(castle);
    	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->building & BUILD_EXT1) DrawBuildingExt1(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->capitan) DrawBuildingCapitan(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->building & BUILD_EXT1) DrawBuildingExt1(castle);
	    if(castle->building & BUILD_SPEC) DrawBuildingSpec(castle);
    	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_MONSTER6) DrawDwelling6(castle);
	    DrawBuildingCastle(castle, &dialogCastle);
	    if(castle->building & BUILD_LEFTTURRET) DrawBuildingLeftTurret(castle);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBuildingRightTurret(castle);
	    if(castle->building & BUILD_MOAT) DrawBuildingMoat(castle);
	    if(castle->capitan) DrawBuildingCapitan(castle);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawDwelling2(castle);
	    if(castle->building & BUILD_THIEVEGUILD) DrawBuildingThieveGuild(castle);
	    if(castle->building & BUILD_TAVERN) DrawBuildingTavern(castle);
	    if(castle->building & BUILD_SHIPYARD) DrawBuildingShipyard(castle);
	    else DrawBuildingExt0(castle);
	    if(castle->building & BUILD_WELL) DrawBuildingWell(castle);
	    if(castle->building & BUILD_SPEC) DrawBuildingSpec(castle);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawDwelling3(castle);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawDwelling5(castle);
	    if(castle->magicTower) DrawBuildingMagicTower(castle);
	    if(castle->building & BUILD_STATUE) DrawBuildingStatue(castle);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawDwelling1(castle);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawDwelling4(castle);
	    if(castle->building & BUILD_MARKETPLACE) DrawBuildingMarketplace(castle);
	    if(castle->building & BUILD_EXT1) DrawBuildingExt1(castle);
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

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_FreeSurface(back);
    FreeActionEvent(dialogCastle);
    FreeAnimationEvent(castanim);

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

void DrawBuildingCastle(S_CASTLE *castle, INTERFACEACTION **ptrAction){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    if(castle->castle){
    		// анимация
		cur.x = cx;
		cur.y = cy;
	        cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNBCSTL.ICN", 1);
	        header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 6);
	        // спрайт замка
		FillSPRITE(&sprite, "TWNBCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNBTENT.ICN", 0);
	break;
	
	case KNIGHT:
	    if(castle->castle){
		// анимация
		cur.x = cx;
	        cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNKCSTL.ICN", 1);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 5);
		// спрайт замка
		FillSPRITE(&sprite, "TWNKCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNKTENT.ICN", 0);
	break;
	
	case NECROMANCER:
	    if(castle->castle){
		// анимация
		cur.x = cx;
		cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNNCSTL.ICN", 1);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 5);
		// спрайт замка
		FillSPRITE(&sprite, "TWNNCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNNTENT.ICN", 0);
	break;

	case SORCERESS:
	    if(castle->castle){
		// анимация
		cur.x = cx;
		cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNSCSTL.ICN", 1);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 5);
		// спрайт замка
		FillSPRITE(&sprite, "TWNSCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNSTENT.ICN", 0);
	break;
	
	case WARLOCK:
	    if(castle->castle){
		// анимация
		cur.x = cx;
		cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNWCSTL.ICN", 1);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 5);
		// спрайт замка
		FillSPRITE(&sprite, "TWNWCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNWTENT.ICN", 0);
	break;
	
	case WIZARD:
	    if(castle->castle){
		// анимация
		cur.x = cx;
		cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNZCSTL.ICN", 1);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 5);
		// спрайт замка
		FillSPRITE(&sprite, "TWNZCSTL.ICN", 0);
	    }else FillSPRITE(&sprite, "TWNZTENT.ICN", 0);
	break;
	
	default:
	return;
	break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingCapitan(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBCAPT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBCAPT.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBCAPT.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSCAPT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSCAPT.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNCAPT.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWCAPT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWCAPT.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZCAPT.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingStatue(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBSTAT.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKSTAT.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNSTAT.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSSTAT.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWSTAT.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZSTAT.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingWell(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBWELL.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKWELL.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNWELL.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSWELL.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWWELL.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZWELL.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingThieveGuild(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBTHIE.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKTHIE.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKTHIE.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNTHIE.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSTHIE.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSTHIE.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWTHIE.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZTHIE.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZTHIE.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingTavern(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBTVRN.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKTVRN.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKTVRN.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // у некров нет таверны
	    return;
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSTVRN.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSTVRN.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWTVRN.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZTVRN.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZTVRN.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingShipyard(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDOCK.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDOCK.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDOCK.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDOCK.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDOCK.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZDOCK.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDOCK.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingLeftTurret(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBLTUR.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKLTUR.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKLTUR.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNLTUR.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSLTUR.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWLTUR.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZLTUR.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingRightTurret(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBRTUR.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKRTUR.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKRTUR.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNRTUR.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSRTUR.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWRTUR.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZRTUR.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingMarketplace(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBMARK.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKMARK.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNMARK.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSMARK.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWMARK.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZMARK.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingMoat(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBMOAT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBMOAT.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKMOAT.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNMOAT.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSMOAT.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWMOAT.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWMOAT.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZMOAT.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingSpec(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBSPEC.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKSPEC.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNSPEC.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSSPEC.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWSPEC.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZSPEC.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingExt1(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBEXT1.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBEXT1.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKEXT1.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNWEL2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNWEL2.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSEXT1.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSEXT1.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWWEL2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWWEL2.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZWEL2.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingExt0(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBEXT0.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKEXT0.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNEXT0.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSEXT0.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWEXT0.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZEXT0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZEXT0.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling1(S_CASTLE *castle){
    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_0.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKDW_0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_0.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_0.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSDW_0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_0.ICN", 0);
	    break;

	case WARLOCK:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNWDW_0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_0.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZDW_0.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_0.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling2(S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE2){
	DrawDwellingUpgrade2(castle);
	return;
    }

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_1.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_1.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_1.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSDW_1.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_1.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_1.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_1.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling3(S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE3){
	DrawDwellingUpgrade3(castle);
	return;
    }

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_2.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKDW_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_2.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNDW_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_2.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_2.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_2.ICN", 0);
	    break;
	    
	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZDW_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_2.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling4(S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE4){
	DrawDwellingUpgrade4(castle);
	return;
    }

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBDW_3.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_3.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_3.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_3.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_3.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_3.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_3.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling5(S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE5){
	DrawDwellingUpgrade5(castle);
	return;
    }

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBDW_4.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_4.ICN", 0);
	    break;

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKDW_4.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 7);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_4.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_4.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_4.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_4.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_4.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwelling6(S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE7){
	DrawDwellingUpgrade7(castle);
	return;
    }else if(castle->dwelling & DWELLING_UPGRADE6){
	DrawDwellingUpgrade6(castle);
	return;
    }

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){
	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBDW_5.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBDW_5.ICN", 0);
	    break;

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKDW_5.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNDW_5.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNDW_5.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSDW_5.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWDW_5.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZDW_5.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawBuildingMagicTower(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    if(castle->magicTower > 5 || castle->magicTower < 1) return;

    switch(castle->race){

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKMAGE.ICN", castle->magicTower);
	    break;

	case BARBARIAN:
	    if(5 == castle->magicTower){
		// анимация
		cur.x = cx;
		cur.y = cy;
		cur.w = 0;
		cur.h = 0;
		FillSPRITE(&sprite, "TWNBMAGE.ICN", 5);
		header = GetICNHeader(&sprite);
		AddAnimationEvent(&castanim, &cur, header, 8);
	    }
	    // спрайт
	    FillSPRITE(&sprite, "TWNBMAGE.ICN", castle->magicTower);
	    break;
	    
	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNMAGE.ICN", (castle->magicTower - 1) * 6 + 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNMAGE.ICN", (castle->magicTower - 1) * 6);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSMAGE.ICN", castle->magicTower);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWMAGE.ICN", castle->magicTower);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZMAGE.ICN", castle->magicTower);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade2(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_1.ICN", 0);
	    break;

	case BARBARIAN:
	    // спрайт
	    FillSPRITE(&sprite, "TWNBUP_1.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNUP_1.ICN", 0);
	    break;

	case SORCERESS:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNSUP_1.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_1.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade3(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKUP_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 6);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_2.ICN", 0);
	    break;

	case NECROMANCER:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNNUP_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNNUP_2.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSUP_2.ICN", 0);
	    break;

	case WIZARD:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNZUP_2.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNZUP_2.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade4(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_3.ICN", 0);
	    break;

	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBUP_3.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBUP_3.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNUP_3.ICN", 0);
	    break;

	case SORCERESS:
	    // спрайт
	    FillSPRITE(&sprite, "TWNSUP_3.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWUP_3.ICN", 0);
	    break;
	    
	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade5(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case KNIGHT:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNKUP_4.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 7);
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_4.ICN", 0);
	    break;

	case BARBARIAN:
	    // анимация
	    cur.x = cx;
	    cur.y = cy;
	    cur.w = 0;
	    cur.h = 0;
	    FillSPRITE(&sprite, "TWNBUP_4.ICN", 1);
	    header = GetICNHeader(&sprite);
	    AddAnimationEvent(&castanim, &cur, header, 5);
	    // спрайт
	    FillSPRITE(&sprite, "TWNBUP_4.ICN", 0);
	    break;
	    
	case NECROMANCER:
	    // спрайт
	    FillSPRITE(&sprite, "TWNNUP_4.ICN", 0);
	    break;

	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZUP_4.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade6(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case KNIGHT:
	    // спрайт
	    FillSPRITE(&sprite, "TWNKUP_5.ICN", 0);
	    break;

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWUP_5.ICN", 0);
	    break;
	    
	case WIZARD:
	    // спрайт
	    FillSPRITE(&sprite, "TWNZUP_5.ICN", 0);
	    break;

	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

void DrawDwellingUpgrade7(S_CASTLE *castle){

    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect cur;
    
    Uint16 cx = video->w / 2 - 640 / 2;
    Uint16 cy = video->h / 2 - 480 / 2;

    switch(castle->race){

	case WARLOCK:
	    // спрайт
	    FillSPRITE(&sprite, "TWNWUP5B.ICN", 0);
	    break;
	    
	default:
	    return;
	    break;
    }

    header = GetICNHeader(&sprite);
    cur.x = cx + header->offsetX;
    cur.y = cy + header->offsetY;
    cur.w = header->surface->w;
    cur.h = header->surface->h;
    SDL_BlitSurface(header->surface, NULL, video, &cur);
}

