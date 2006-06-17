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

void		MapsRescanObject(Uint8, MP2TILEINFO *, MP2ADDONTAIL *, S_CELLMAPS *);
E_GROUND	GetTypeGround(MP2TILEINFO*);
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

	ptrMaps[i].ground = GetTypeGround(&ptrMapsInfo[i]);
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMapsInfo[i].tileIndex, ptrMapsInfo[i].shape);

	Sint8			j = 0;
	Uint16 			indexAddon = 0;

	MP2ADDONTAIL		*ptrAddon = NULL;
	ICNHEADER		*tail = NULL;
	ICNHEADER		*current = NULL;

	ptrMaps[i].level1	= NULL;
	ptrMaps[i].level2	= NULL;

	ptrMaps[i].move		= TRUE;
	ptrMaps[i].count	= 0;
	ptrMaps[i].type		= OBJ_ZERO;

	// init level1
	for(j = 3; j >= 0; --j){

    	    if(0xFF != ptrMapsInfo[i].indexName1 && j == (ptrMapsInfo[i].quantity1 % 4)){

		MapsRescanObject(ptrMapsInfo[i].objectName1, &ptrMapsInfo[i], NULL, &ptrMaps[i]);

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
	
		    MapsRescanObject(ptrAddon->objectNameN1 * 2, &ptrMapsInfo[i], ptrAddon, &ptrMaps[i]);

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

	if(OBJ_ZERO == ptrMaps[i].type) ptrMaps[i].type = CheckValidObject(ptrMapsInfo[i].generalObject);

    }

    // Освобождаем загруженные объекты TIL
    FreeObject("GROUND32.TIL");

    // Освобождаем использованную информацию
    free(ptrMapsInfo);
    free(tailAddon);
    
    fprintf(stderr, "InitMaps: %s\n", filename);

    // Рисуем экран и в цикл событий
    ACTION result = DrawMainDisplay();

    FreeMaps();

    return result;
}

// объекты которые необходимо дополнить инфомацией
void MapsRescanObject(Uint8 type, MP2TILEINFO *info, MP2ADDONTAIL *addon, S_CELLMAPS *maps){

    switch(type){
        // артифакты
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:

        // монстры
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:

        // ресурсы
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
	    
            break;

	default:
	
	    return;
	    break;
    }

    switch(info->generalObject){

	case OBJ_RESOURCE:

	    // ресурсы только в первой секции
	    if(addon) return;

	    maps->type = OBJ_RESOURCE;
	    maps->object.resource = CheckValidResource(info->indexName1);
	    maps->count = GetCountResource(maps->object.resource);

	    break;

	case OBJ_RNDRESOURCE:

	    // ресурсы только в первой секции
	    if(addon) return;

	    maps->type = OBJ_RESOURCE;
	    maps->object.resource = GetRNDResource();
	    maps->count = GetCountResource(maps->object.resource);
	    
	    info->generalObject = OBJ_RESOURCE;

	    if(RNDRES == CheckValidResource(info->indexName1))
		info->indexName1 = maps->object.resource;

	    break;

	case OBJ_ARTIFACT:

	    // артифакты только в первой секции
	    if(addon) return;

	    maps->type = OBJ_ARTIFACT;
	    maps->object.artifact = CheckValidArtifact((info->indexName1 + 1) / 2 - 1);
	    
	    break;

	case OBJ_RNDARTIFACT:

	    // артифакты только в первой секции
	    if(addon) return;

	    maps->type = OBJ_ARTIFACT;
	    maps->object.artifact = GetRNDArtifact(ART_ALL);
	    
	    info->generalObject = OBJ_ARTIFACT;

	    if(RND_ARTIFACT == CheckValidArtifact(info->indexName1))
		info->indexName1 = maps->object.artifact * 2 + 1;

	    break;

	case OBJ_RNDARTIFACT1:

	    // артифакты только в первой секции
	    if(addon) return;

	    maps->type = OBJ_ARTIFACT;
	    maps->object.artifact = GetRNDArtifact(ART_LEVEL1);
	    
	    info->generalObject = OBJ_ARTIFACT;

	    if(RND_ARTIFACT1 == CheckValidArtifact(info->indexName1))
		info->indexName1 = maps->object.artifact * 2 +1;

	    break;

	case OBJ_RNDARTIFACT2:

	    // артифакты только в первой секции
	    if(addon) return;

	    maps->type = OBJ_ARTIFACT;
	    maps->object.artifact = GetRNDArtifact(ART_LEVEL2);
	    
	    info->generalObject = OBJ_ARTIFACT;

	    if(RND_ARTIFACT2 == CheckValidArtifact(info->indexName1))
		info->indexName1 = maps->object.artifact * 2 +1;

	    break;

	case OBJ_RNDARTIFACT3:

	    // артифакты только в первой секции
	    if(addon) return;

	    maps->type = OBJ_ARTIFACT;
	    maps->object.artifact = GetRNDArtifact(ART_LEVEL3);
	    
	    info->generalObject = OBJ_ARTIFACT;

	    if(RND_ARTIFACT3 == CheckValidArtifact(info->indexName1))
		info->indexName1 = maps->object.artifact * 2 +1;

	    break;

	case OBJ_MONSTER:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = CheckValidMonster(info->indexName1);
	    maps->count = info->quantity2;
            maps->count <<= 8;
            maps->count |= info->quantity1;
            maps->count >>= 3;

	    if(! maps->count) maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    break;

	case OBJ_RNDMONSTER:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_ALL);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;

	    if(MONSTER_RND == CheckValidMonster(info->indexName1))
		info->indexName1 = maps->object.monster;


	    break;

	case OBJ_RNDMONSTER1:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL1);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;

	    if(MONSTER_RND1 == CheckValidMonster(info->indexName1))
		info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER2:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL2);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;

	    if(MONSTER_RND2 == CheckValidMonster(info->indexName1))
		info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER3:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL3);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;

	    if(MONSTER_RND3 == CheckValidMonster(info->indexName1))
		info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER4:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL4);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;

	    if(MONSTER_RND4 == CheckValidMonster(info->indexName1))
		info->indexName1 = maps->object.monster;

	    break;


	    default:
		break;
	}


    return;
}

E_GROUND GetTypeGround(MP2TILEINFO *info){

    Uint16 index = info->tileIndex;
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
