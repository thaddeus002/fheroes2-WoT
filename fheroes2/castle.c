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
#include "config.h"
#include "cursor.h"
#include "mp2maps.h"
#include "monster.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"

static S_CASTLE		*ptrCastle = NULL;
static Uint8		countCastle = 0;

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
	    ptrCastle[countCastle].building = BUILD_TAVERN; // BUILD_THIEVEGUILD BUILD_TAVERN BUILD_SHIPYARD BUILD_WELL BUILD_STATUE BUILD_LEFTTURRET BUILD_RIGHTTURRET BUILD_MARKETPLACE BUILD_MOAT BUILD_EXT1 BUILD_EXT2
	    ptrCastle[countCastle].dwelling = DWELLING_MONSTER1 | DWELLING_MONSTER2; // DWELLING_MONSTER1 DWELLING_MONSTER2 DWELLING_MONSTER3 DWELLING_MONSTER4 DWELLING_MONSTER5 DWELLING_MONSTER6 DWELLING_UPGRADE2 DWELLING_UPGRADE3 DWELLING_UPGRADE4 DWELLING_UPGRADE5 DWELLING_UPGRADE6
	}

	ptrCastle[countCastle].magicTower = ptr->magicTower;

	if(ptr->customTroops){
	    ptrCastle[countCastle].army1.monster = ptr->monster1;
	    ptrCastle[countCastle].army2.monster = ptr->monster2;
	    ptrCastle[countCastle].army3.monster = ptr->monster3;
	    ptrCastle[countCastle].army4.monster = ptr->monster4;
	    ptrCastle[countCastle].army5.monster = ptr->monster5;
	    ptrCastle[countCastle].army1.count = ptr->count1;
	    ptrCastle[countCastle].army2.count = ptr->count2;
	    ptrCastle[countCastle].army3.count = ptr->count3;
	    ptrCastle[countCastle].army4.count = ptr->count4;
	    ptrCastle[countCastle].army5.count = ptr->count5;

	}else{	
	    switch(ptrCastle[countCastle].race){
	
		case KNIGHT:
		    ptrCastle[countCastle].army1.monster = PEASANT;
		    ptrCastle[countCastle].army2.monster = ARCHER;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;

		case BARBARIAN:
		    ptrCastle[countCastle].army1.monster = GOBLIN;
		    ptrCastle[countCastle].army2.monster = ORC;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;

		case SORCERESS:
		    ptrCastle[countCastle].army1.monster = SPRITE;
		    ptrCastle[countCastle].army2.monster = DWARF;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;

		case WARLOCK:
		    ptrCastle[countCastle].army1.monster = CENTAUR;
		    ptrCastle[countCastle].army2.monster = GARGOYLE;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;

		case WIZARD:
		    ptrCastle[countCastle].army1.monster = HALFLING;
		    ptrCastle[countCastle].army2.monster = BOAR;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;

		case NECROMANCER:
		    ptrCastle[countCastle].army1.monster = SKELETON;
		    ptrCastle[countCastle].army2.monster = ZOMBIE;
		    ptrCastle[countCastle].army1.count = 28;
		    ptrCastle[countCastle].army2.count = 7;
		    break;
	
		default:
		    break;
	    }

	    ptrCastle[countCastle].army3.monster = MONSTERNONE;
	    ptrCastle[countCastle].army4.monster = MONSTERNONE;
	    ptrCastle[countCastle].army5.monster = MONSTERNONE;
	    ptrCastle[countCastle].army3.count = 0;
	    ptrCastle[countCastle].army4.count = 0;
	    ptrCastle[countCastle].army5.count = 0;
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
    S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_HEROES *heroes = GetStatHeroes(nameHeroes);

    // в серые замки не заходим
    if(GRAY == castle->color) return;

    // переопределяем курсор и выключаем анимацию карты
    SetIntValue(ANIM1, FALSE);
    CursorOff();
    Uint32 cursor = GetCursor();
    INTERFACEACTION *dialogCastle = NULL;

    SDL_Surface *format, *back, *ram, *video;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    ICNHEADER *header = NULL;
    
    Uint16 cx, cy;
    Uint8 i, j;

    // рисуем бакгроунд 640х480 если videomode более то рисуем бордюр и по центру экрана
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
	rectBack.x = (video->w - 640) / 2 - BORDERWIDTH;
	rectBack.y = (video->h - 480) / 2 - BORDERWIDTH;
	rectBack.w = 640 + 2 * BORDERWIDTH;
	rectBack.h = 480 + 2 * BORDERWIDTH;
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

    // рисуем фон
    rectCur = rectBack;
    SDL_FillRect(video, &rectBack, 0);
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
    ram = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){
	// рисуем бордюр
	rectCur.x = rectBack.x + BORDERWIDTH;
	rectCur.y = rectBack.y + BORDERWIDTH;
	rectCur.w = ram->w;
	rectCur.h = ram->h;
    }
    SDL_BlitSurface(ram, NULL, video, &rectCur);

    // рисуем сетку
    FillSPRITE(&sprite, "STRIP.ICN", 0);
    ram = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y;

    // левый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 1);
    ram = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);

    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    ram = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);

    // правый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 3);
    ram = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);

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
    ram = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 6;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y + ram->h;

    // рисуем фон ячеек для монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    ram = GetICNSprite(&sprite);
    for(i = 0; i < 5; ++i){
	rectCur.x += rectCur.w + 6;
	rectCur.w = ram->w;
	rectCur.h = ram->h;
	SDL_BlitSurface(ram, NULL, video, &rectCur);
    }

    // рисуем фон ячейки героя
    char number[5];
    char icnstring[13];
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
    ram = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 6;
    rectCur.w = ram->w;
    rectCur.h = ram->h;
    SDL_BlitSurface(ram, NULL, video, &rectCur);

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = rectCur.x + rectCur.w + 6;
	cy = rectCur.y;

	for( i = 0; i < HEROESMAXARMY; ++i)
	    // если есть у героя монстры
	    if(MONSTERNONE != heroes->army[i].monster){

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
		ram = GetICNSprite(&sprite);
		rectCur.x = cx + (ram->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = ram->w;
		rectCur.h = ram->h;
		SDL_BlitSurface(ram, NULL, video, &rectCur);

		// рисуем монстров
		memset(number, 0, strlen(number) + 1);
		memset(icnstring, 0, strlen(icnstring) + 1);
    		sprintf(number, "%4d", heroes->army[i].monster);
    		for(j = 0; j < 4; j++)
		    if(0 == strncmp(&number[j], " ", 1)) number[j] = '0';
    		sprintf(icnstring, "MONH%4s.ICN", number);
		FillSPRITE(&sprite, icnstring, 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + (header->surface->w + 6) * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);

	    }else{
		FillSPRITE(&sprite, "STRIP.ICN", 2);
		ram = GetICNSprite(&sprite);
		rectCur.x = cx + (ram->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = ram->w;
		rectCur.h = ram->h;
		SDL_BlitSurface(ram, NULL, video, &rectCur);
	    }
    }else{
	FillSPRITE(&sprite, "STRIP.ICN", 11);
	ram = GetICNSprite(&sprite);
	rectCur.x += rectCur.w + 6;
	rectCur.w = ram->w;
	rectCur.h = ram->h;
	SDL_BlitSurface(ram, NULL, video, &rectCur);
    }






    SetCursor(CURSOR_POINTER);
    CursorOn();

    ActionCycle(dialogCastle);

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_FreeSurface(back);
            
    // востанавливаем курсор и анимацию карты
    SetCursor(cursor);
    CursorOn();

    SetIntValue(ANIM1, TRUE);
}
