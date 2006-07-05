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
#include "config.h"
#include "mp2maps.h"
#include "monster.h"
#include "castle.h"

static S_CASTLE		*ptrCastle = NULL;

BOOL	InitCastle(FILE *fd){

    if(! fd){
	fprintf(stderr, "InitCastle: descriptor NULL\n");
	return FALSE;
    }

    Uint32 pos = ftell(fd);
    fseek(fd, 0, SEEK_END);
    Uint32 size = ftell(fd);
    
    fseek(fd, pos, SEEK_SET);
    Uint16 countBlock = 0;
    Uint8  countCastle = 0;
    MP2CASTLE *ptr = NULL;

    while((pos = ftell(fd)) < size - 4){

	fread(&countBlock, sizeof(Uint16), 1, fd);

	if(0x0046 != countBlock){
	    fseek(fd, countBlock, SEEK_CUR);
	    continue;
	}
	
	if(NULL == (ptr = (MP2CASTLE *) malloc(sizeof(MP2CASTLE)))){
	    fprintf(stderr, "InitCastle: error malloc: %d\n", sizeof(MP2CASTLE));
	    return FALSE;
	}

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
	
	if(NULL == (ptrCastle = realloc(ptrCastle, sizeof(S_CASTLE) * (countCastle + 1)))){
	    fprintf(stderr, "InitCastle: error malloc: %d\n", sizeof(S_CASTLE) * (countCastle + 1));
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
		fprintf(stderr, "InitCastle: unknown colors, default GRAY\n");
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

	    case 6:
		ptrCastle[countCastle].race = NECROMANCER;
		break;
	
	    default:
		ptrCastle[countCastle].race = KNIGHT;
		fprintf(stderr, "InitCastle: unknown race, default KNIGHT\n");
		break;
	}

	strncpy(ptrCastle[countCastle].name, ptr->castleName, 13);

	if(ptr->customBuilding)
	    ptrCastle[countCastle].building = ptr->building;
	else
	    ptrCastle[countCastle].building = BUILD_TAVERN; // BUILD_THIEVEGUILD BUILD_TAVERN BUILD_SHIPYARD BUILD_WELL BUILD_STATUE BUILD_LEFTTURRET BUILD_RIGHTTURRET BUILD_MARKETPLACE BUILD_MOAT BUILD_EXT1 BUILD_EXT2

	if(ptr->customBuilding)
	    ptrCastle[countCastle].dwelling = ptr->dwelling;
	else
	    ptrCastle[countCastle].dwelling = DWELLING_MONSTER1 | DWELLING_MONSTER2; // DWELLING_MONSTER1 DWELLING_MONSTER2 DWELLING_MONSTER3 DWELLING_MONSTER4 DWELLING_MONSTER5 DWELLING_MONSTER6 DWELLING_UPGRADE2 DWELLING_UPGRADE3 DWELLING_UPGRADE4 DWELLING_UPGRADE5 DWELLING_UPGRADE6

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
	ptrCastle[countCastle].castle = ptr->castle;

	if(ptr->allowCastle)
	    ptrCastle[countCastle].allowCastle = FALSE;
	else
	    ptrCastle[countCastle].allowCastle = TRUE;

	ptrCastle[countCastle].next = NULL;

	//ptrCastle[countCastle].pos.x;
	//ptrCastle[countCastle].pos.y;
	//ptrCastle[countCastle].pos.w;
	//ptrCastle[countCastle].pos.h;

	free(ptr);
	++countCastle;
    }
    if(GetIntValue(DEBUG)) fprintf(stderr, "InitCastle: total %d\n", countCastle);

    return TRUE;
}

void	FreeCastle(void){

    if(ptrCastle) free(ptrCastle);
}
