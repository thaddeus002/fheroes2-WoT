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

#include "mp2maps.h"
#include "gamedefs.h"
#include "debug.h"
#include "draw.h"
#include "config.h"
#include "monster.h"
#include "artifact.h"
#include "resource.h"
#include "loadgame.h"

void		RescanRNDObject();
E_GROUND	GetTypeGround(Uint16);
ICNHEADER	*AddLEVELDRAW(ICNHEADER**);
MP2ADDONTAIL   *GetADDONTAIL(Uint16);

MP2HEADER       *ptrMP2Header = NULL;
S_CELLMAPS      *ptrMaps = NULL;

MP2ADDONTAIL    *tailAddon = NULL;
static Uint32   countTail = 0;


Uint8   GetWidthMaps(void){

    if(ptrMP2Header)
        return ptrMP2Header->widthMaps;

    return 0;
}

Uint8   GetHeightMaps(void){

    if(ptrMP2Header)
        return ptrMP2Header->heightMaps;

    return 0;
}

MP2ADDONTAIL *GetADDONTAIL(Uint16 index){

    if(tailAddon)
        return &tailAddon[index];

    return NULL;
}

S_CELLMAPS *GetCELLMAPS(Uint16 index){

    if(ptrMaps)
        return &ptrMaps[index];

    return NULL;
}

void FreeMaps(void){

    int i;
    ICNHEADER *next = NULL;

    if(ptrMP2Header){

	if(ptrMaps){

	    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

		if(ptrMaps[i].tile) SDL_FreeSurface(ptrMaps[i].tile);

		while(ptrMaps[i].level1){

		    next = (ICNHEADER *) ptrMaps[i].level1->next;
		    free(ptrMaps[i].level1);
		    ptrMaps[i].level1 = next;
		}

		while(ptrMaps[i].level2){

		    next = (ICNHEADER *) ptrMaps[i].level2->next;
		    free(ptrMaps[i].level2);
		    ptrMaps[i].level2 = next;
		}
	    }
	    free(ptrMaps);
	}

	free(ptrMP2Header);
    }
}

ACTION InitMaps(char *filename){

    FILE        *fd = NULL;
    MP2TILEINFO *ptrMapsInfo = NULL;

    // open file
    if(NULL == (fd = fopen(filename, "rb"))){
        fprintf(stderr, "InitMaps: error fopen: %s\n", filename);
	return EXIT;
    }

    Uint32 filesize;
    fseek(fd, 0, SEEK_END);
    filesize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // read buffer MP2HEADER
    if(NULL == (ptrMP2Header = (MP2HEADER *) malloc(sizeof(MP2HEADER)))){
	printf("InitMaps: malloc: error size %d\n", sizeof(MP2HEADER));
        return EXIT;
    }
    fread((char *) ptrMP2Header, sizeof(MP2HEADER), 1, fd);

    if(ptrMP2Header->headerMagic != 0x0000005C){
	fprintf(stderr, "InitMaps: load error: incorrect MP2 maps!\n");
	return EXIT;
    }

    // read buffer CELLMAPS
    if(NULL == (ptrMaps = (S_CELLMAPS *) malloc(sizeof(S_CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps))){
	fprintf(stderr, "InitMaps: malloc: error size %d\n", sizeof(S_CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps);
        return EXIT;
    }

    // read buffer MP2TILEINFO
    if(NULL == (ptrMapsInfo = (MP2TILEINFO *) malloc(sizeof(MP2TILEINFO) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps))){
	fprintf(stderr, "InitMaps: malloc: error size %d\n", sizeof(MP2TILEINFO) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps);
        return EXIT;
    }

    fread((char *) ptrMapsInfo, ptrMP2Header->heightMaps * ptrMP2Header->widthMaps, sizeof(MP2TILEINFO), fd);

    fread((char *) &countTail, 1, sizeof(Uint32), fd);

    if(filesize < (ftell(fd) + countTail * 15)){
	printf("InitMaps error: incorrect format maps.\n");
        return EXIT;
    }
    
    if(NULL == (tailAddon = (MP2ADDONTAIL *) malloc(countTail * sizeof(MP2ADDONTAIL)))){
	printf("InitMaps malloc: error size %lu\n", (unsigned long) countTail * sizeof(MP2ADDONTAIL));
        return EXIT;
    }

    Uint16 i;
    // read TailADDON
    for(i = 0; i < countTail; ++i){
        
        fread((char *) &tailAddon[i].indexAddon, sizeof(Uint16), 1, fd);
        fread((char *) &tailAddon[i].objectNameN1, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].indexNameN1, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].quantityN, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].objectNameN2, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].indexNameN2, sizeof(Uint8), 1, fd);
        fread((char *) &tailAddon[i].uniqNumberN1, sizeof(Uint32), 1, fd);
        fread((char *) &tailAddon[i].uniqNumberN2, sizeof(Uint32), 1, fd);
    }

    // close file
    fclose(fd);

    // цикл конвертации в новый формат
    Uint8  *ptrTILData = GetTILData("GROUND32.TIL");

    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

        if(NULL == (ptrMaps[i].tile = SDL_CreateRGBSurface(SDL_SWSURFACE, TILEWIDTH, TILEWIDTH, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "InitMaps: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), TILEWIDTH, TILEWIDTH);
	    return EXIT;
	}

	ptrMaps[i].ground = GetTypeGround(ptrMapsInfo[i].tileIndex);
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMapsInfo[i].tileIndex, ptrMapsInfo[i].shape);

	Sint8			j = 0;
	Uint16 			indexAddon = 0;

	MP2ADDONTAIL		*ptrAddon = NULL;
	ICNHEADER		*tail = NULL;
	ICNHEADER		*current = NULL;

	ptrMaps[i].level1	= NULL;
	ptrMaps[i].level2	= NULL;

	// init level1
	for(j = 3; j >= 0; --j){

    	    if(0xFF != ptrMapsInfo[i].indexName1 && j == (ptrMapsInfo[i].quantity1 % 4)){

		if(NULL != (current = GetICNHEADERCellObject(ptrMapsInfo[i].objectName1, ptrMapsInfo[i].indexName1))){
		
		    tail = AddLEVELDRAW(&ptrMaps[i].level1);

		    tail->surface = current->surface;
		    tail->offsetX = current->offsetX;
		    tail->offsetY = current->offsetY;
		    tail->next = NULL;
		}
	    }

    	    indexAddon = ptrMapsInfo[i].indexAddon;
        
    	    while(indexAddon){
        	ptrAddon = GetADDONTAIL(indexAddon);

        	if(0xFF != ptrAddon->indexNameN1 && j == (ptrAddon->quantityN % 4)){

		    if(NULL != (current = GetICNHEADERCellObject(ptrAddon->objectNameN1 * 2, ptrAddon->indexNameN1))){

			tail = AddLEVELDRAW(&ptrMaps[i].level1);

		        tail->surface = current->surface;
			tail->offsetX = current->offsetX;
			tail->offsetY = current->offsetY;
			tail->next = NULL;
		    }
		}

        	indexAddon = ptrAddon->indexAddon;
    	    }
	}

	indexAddon = 0;
	
	// init level2
	for(j = 3; j >= 0; --j){
            
    	    if(0xFF != ptrMapsInfo[i].indexName2 && j == (ptrMapsInfo[i].quantity1 % 4)){

		if(NULL != (current = GetICNHEADERCellObject(ptrMapsInfo[i].objectName2, ptrMapsInfo[i].indexName2))){

		    tail = AddLEVELDRAW(&ptrMaps[i].level2);

		    tail->surface = current->surface;
		    tail->offsetX = current->offsetX;
		    tail->offsetY = current->offsetY;
		    tail->next = NULL;
		}
	    }

    	    indexAddon = ptrMapsInfo[i].indexAddon;
        
    	    while(indexAddon){
        	ptrAddon = GetADDONTAIL(indexAddon);

        	if(0xFF != ptrAddon->indexNameN2 && j == (ptrAddon->quantityN % 4)){

		    if(NULL != (current = GetICNHEADERCellObject(ptrAddon->objectNameN2, ptrAddon->indexNameN2))){

			tail = AddLEVELDRAW(&ptrMaps[i].level2);

			tail->surface = current->surface;
			tail->offsetX = current->offsetX;
			tail->offsetY = current->offsetY;
			tail->next = NULL;
		    }
		}

        	indexAddon = ptrAddon->indexAddon;
    	    }
	}

	ptrMaps[i].move = TRUE;
	ptrMaps[i].count = 0;
	ptrMaps[i].type = CheckCorrectObject(ptrMapsInfo[i].generalObject);

	ptrMaps[i].object = NULL;
	ptrMaps[i].heroes = NULL;
/*
	E_MONSTER	rndMonster;
	// монстры
	switch(ptrMaps[i].type){

	    case OBJ_MONSTER: 

		// раскодируем количество
                ptrMaps[i].count = ptrMapsInfo[i].quantity2;
                ptrMaps[i].count <<= 8;
                ptrMaps[i].count |=  ptrMapsInfo[i].quantity1;
                ptrMaps[i].count >>= 3;
             
                ptrMaps[i].object = GetStatMonster(CheckCorrectMonster(ptrMapsInfo[i].indexName1));
                if(0 == ptrMaps[i].count) ptrMaps[i].count = rand() % RNDMONSTERMAXCOUNT;
                break;
                
            case OBJ_RNDMONSTER1:
        	rndMonster = GetRNDMonster(MNS_LEVEL1);
        	ptrMaps[i].type = OBJ_MONSTER;
                ptrMaps[i].object = GetStatMonster(rndMonster);
                ptrMaps[i].count = rand() % RNDMONSTERMAXCOUNT;
                
                current = GetICNHEADERCellObject(0x30, rndMonster);
                ptrMaps[i].level1->surface = current->surface;
                ptrMaps[i].level1->offsetX = current->offsetX;
                ptrMaps[i].level1->offsetY = current->offsetY;
                break;

            case OBJ_RNDMONSTER2:
        	rndMonster = GetRNDMonster(MNS_LEVEL2);
        	ptrMaps[i].type = OBJ_MONSTER;
                ptrMaps[i].object = GetStatMonster(rndMonster);
                ptrMaps[i].count = rand() % RNDMONSTERMAXCOUNT;
                
                current = GetICNHEADERCellObject(0x30, rndMonster);
                ptrMaps[i].level1->surface = current->surface;
                ptrMaps[i].level1->offsetX = current->offsetX;
                ptrMaps[i].level1->offsetY = current->offsetY;
                break;

            case OBJ_RNDMONSTER3:
        	rndMonster = GetRNDMonster(MNS_LEVEL3);
        	ptrMaps[i].type = OBJ_MONSTER;
                ptrMaps[i].object = GetStatMonster(rndMonster);
                ptrMaps[i].count = rand() % RNDMONSTERMAXCOUNT;
                
                current = GetICNHEADERCellObject(0x30, rndMonster);
                ptrMaps[i].level1->surface = current->surface;
                ptrMaps[i].level1->offsetX = current->offsetX;
                ptrMaps[i].level1->offsetY = current->offsetY;
                break;

            case OBJ_RNDMONSTER4:
        	rndMonster = GetRNDMonster(MNS_LEVEL4);
        	ptrMaps[i].type = OBJ_MONSTER;
                ptrMaps[i].object = GetStatMonster(rndMonster);
                ptrMaps[i].count = rand() % RNDMONSTERMAXCOUNT;
                
                current = GetICNHEADERCellObject(0x30, rndMonster);
                ptrMaps[i].level1->surface = current->surface;
                ptrMaps[i].level1->offsetX = current->offsetX;
                ptrMaps[i].level1->offsetY = current->offsetY;
                break;
            
            default:
        	break;
        }
*/
    }

    // Освобождаем загруженные объекты TIL
    FreeObject("GROUND32.TIL");

    // Освобождаем использованную информацию
    free(ptrMapsInfo);
    free(tailAddon);
    
    fprintf(stderr, "InitMaps: %s\n", filename);

    // переопределяем rnd объекты
    RescanRNDObject();
    
    // Рисуем экран и в цикл событий
    ACTION result = DrawMainDisplay();

    FreeMaps();

    return result;
}

/* функция переопределяет все RND объекты */
void RescanRNDObject(void){
/*
    Uint16 index = 0;
    Uint16 max = GetWidthMaps() * GetHeightMaps();
    CELLMAPS *ptrCell = NULL;
    
    while(index != max){

	ptrCell = GetCELLMAPS(index);
	
	switch(ptrCell->info->generalObject){
	
	    case OBJ_RNDARTIFACT:
	    case OBJ_RNDARTIFACT1:
	    case OBJ_RNDARTIFACT2:
	    case OBJ_RNDARTIFACT3:
	    
	    default:
		break;
	}
	
	++index;
    }
*/
}

E_GROUND GetTypeGround(Uint16 index){

    // список поверхностей по индексу из GROUND32.TIL
    // найти место для ROAD!!

    if(30 > index)
        return WATER;
    
    else if(92 > index)
	return GRASS;

    else if(146 > index)
	return SNOW;

    else if(208 > index)
	return SWAMP;

    else if(262 > index)
	return LAVA;
	
    else if(321 > index)
	return DESERT;

    else if(361 > index)
	return DIRT;
	
    else if(415 > index)
	return WASTELAND;

    else if(432 > index)
	return BEACH;

    else
	fprintf(stderr, "GetTypeGround: unknown type tiles: %d\n", index);

    return WATER;
}

ICNHEADER *AddLEVELDRAW(ICNHEADER **ptr){

    if(NULL == *ptr){
        *ptr = (ICNHEADER *) malloc(sizeof(ICNHEADER));
	
	if(NULL == *ptr){
	    fprintf(stderr, "AddLEVELDRAW: error malloc: %d\n", sizeof(ICNHEADER));
	    return NULL;
	}

        (*ptr)->offsetX	= 0;
        (*ptr)->offsetY	= 0;
        (*ptr)->surface = NULL;
        (*ptr)->next    = NULL;

        return (ICNHEADER *) *ptr;
    }

    // иначе ищем хвост
    ICNHEADER *tail = *ptr;
    
    while(tail->next) tail = (ICNHEADER *) tail->next;

    if(NULL == (tail->next = (ICNHEADER *) malloc(sizeof(ICNHEADER)))){
        fprintf(stderr, "AddLEVELDRAW: error malloc: %d\n", sizeof(ICNHEADER));
        return NULL;
    }
    
    tail = (ICNHEADER *) tail->next;

    tail->offsetX = 0;
    tail->offsetY = 0;
    tail->surface = NULL;
    tail->next    = NULL;

    return tail;
}
