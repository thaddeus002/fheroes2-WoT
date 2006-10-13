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
#include "mp2maps.h"
#include "gamedefs.h"
#include "debug.h"
#include "draw.h"
#include "config.h"
#include "tools.h"
#include "castle.h"
#include "kingdom.h"
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

Uint8           *ptrMP2Castle = NULL;

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

		while(ptrMaps[i].upgrade){

		    next = (ICNHEADER *) ptrMaps[i].upgrade->next;
		    free(ptrMaps[i].upgrade);
		    ptrMaps[i].upgrade = next;
		}

		if(ptrMaps[i].animation)
			    FreeAnimationEvent(ptrMaps[i].animation);

		if(ptrMaps[i].monster)
			    FreeAnimationEvent(ptrMaps[i].monster);

	    }
	    free(ptrMaps);
	}

	free(ptrMP2Header);
    }
    if(ptrMP2Castle) free(ptrMP2Castle);
    
    ptrMP2Castle	= NULL;
    ptrMP2Header	= NULL;
    ptrMaps		= NULL;
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

    //идем в хвост с инфо о замках
    if(NULL == (ptrMP2Castle = (Uint8 *) malloc(3 * 72))){
	printf("InitMaps malloc: error size %d\n", 3 * 72);
        return EXIT;
    }
    fread(ptrMP2Castle, 1, 3 * 72, fd);

    // здесь инфо и ресурсах, пока пропускаем
    fseek(fd, 3 * 144 + 1, SEEK_CUR);

    // те самые несколько байт, пропускаем
    fread(&i, sizeof(i), 1, fd);
    while(i) fread(&i, sizeof(i), 1, fd);

    // позиция хвоста
    Uint32 fdTail = ftell(fd);

    // возможно еще что то прочитаем


    // инициализируем королевства
    InitKingdom();

    // цикл конвертации в новый формат
    Uint8  *ptrTILData = GetTILData("GROUND32.TIL");

    Sint8		j = 0;
    Uint8 		castle = 0;
    Uint16 		indexAddon = 0;

    MP2ADDONTAIL	*ptrAddon = NULL;
    ICNHEADER		*tail = NULL;
    ICNHEADER		*current = NULL;
    Uint16		quantity;

    // цикл по заполнению основной информации
    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

        if(NULL == (ptrMaps[i].tile = SDL_CreateRGBSurface(SDL_SWSURFACE, TILEWIDTH, TILEWIDTH, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "InitMaps: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), TILEWIDTH, TILEWIDTH);
	    return EXIT;
	}

	ptrMaps[i].ground = GetTypeGround(&ptrMapsInfo[i]);
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMapsInfo[i].tileIndex, ptrMapsInfo[i].shape);

	ptrMaps[i].ax		= i % ptrMP2Header->widthMaps;
	ptrMaps[i].ay		= i / ptrMP2Header->widthMaps;

	ptrMaps[i].level1	= NULL;
	ptrMaps[i].level2	= NULL;
	ptrMaps[i].upgrade	= NULL;

	ptrMaps[i].count	= 0;
	ptrMaps[i].animation	= NULL;
	ptrMaps[i].monster	= NULL;

	ptrMaps[i].type = CheckValidObject(ptrMapsInfo[i].generalObject);

	// все рандом замки переопределены
	if(OBJ_RNDCASTLE == ptrMaps[i].type || OBJ_RNDTOWN == ptrMaps[i].type) ptrMaps[i].type = OBJ_CASTLE;
	if(OBJN_RNDCASTLE == ptrMaps[i].type || OBJN_RNDTOWN == ptrMaps[i].type) ptrMaps[i].type = OBJN_CASTLE;

	// добавляем замки
	if(OBJ_CASTLE == ptrMaps[i].type){
	    quantity = PackUint16(ptrMapsInfo[i].quantity2, ptrMapsInfo[i].quantity1);
	    if(GetIntValue(DEBUG) && (0 == quantity || quantity % 0x08)) fprintf(stderr, "InitMaps: hmm.. unknown castle position, quantity: %hX\n", quantity);
	    fseek(fd, fdTail, SEEK_SET);
	    if(! AddCastle(fd, quantity / 8 - 1, ptrMP2Castle[castle * 3], ptrMP2Castle[castle * 3 + 1])) fprintf(stderr, "ax: %d, ay: %d, quantity: %hX\n", ptrMaps[i].ax, ptrMaps[i].ay, quantity);
	    ++castle;
	}


	switch(ptrMaps[i].type){
	
            case OBJ_STONES:
            case OBJ_OILLAKE:
            case OBJ_BIGCRACK:
            case OBJ_MOUNTS:
            case OBJ_TREES:
            case OBJ_FIRTREES:
            case OBJN_WAGONCAMP:
            case OBJN_SAWMILL:
            case OBJN_MINES:
            case OBJ_WATERLAKE:
            case OBJN_ALCHEMYTOWER:
            case OBJN_EXCAVATION:
            case OBJN_FORT:
            case OBJN_DRAGONCITY:
		ptrMaps[i].move = FALSE;
		break;
	
	    default:
		ptrMaps[i].move = TRUE;
		break;
	}
    }

    // цикл по заполнению спрайтов и анимации
    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

	// init level1
	for(j = 3; j >= 0; --j){

    	    if(0xFF != ptrMapsInfo[i].indexName1 && j == (ptrMapsInfo[i].quantity1 % 4)){

		MapsRescanObject(ptrMapsInfo[i].objectName1, &ptrMapsInfo[i], NULL, &ptrMaps[i]);

		if(NULL != (current = GetICNHEADERCellObject(ptrMapsInfo[i].objectName1, ptrMapsInfo[i].indexName1, ptrMaps[i].ax, ptrMaps[i].ay))){

		    tail = AddLEVELDRAW(&ptrMaps[i].level1);

		    tail->surface = current->surface;
		    tail->offsetX = current->offsetX;
		    tail->offsetY = current->offsetY;
		    tail->next = NULL;
		    
		    StoreAnimationFrame(ptrMapsInfo[i].objectName1, ptrMapsInfo[i].indexName1, &ptrMaps[i]);
		}
	    }

    	    indexAddon = ptrMapsInfo[i].indexAddon;
        
    	    while(indexAddon){
        	ptrAddon = GetADDONTAIL(indexAddon);

        	if(0xFF != ptrAddon->indexNameN1 && j == (ptrAddon->quantityN % 4)){
	
		    MapsRescanObject(ptrAddon->objectNameN1 * 2, &ptrMapsInfo[i], ptrAddon, &ptrMaps[i]);

		    if(NULL != (current = GetICNHEADERCellObject(ptrAddon->objectNameN1 * 2, ptrAddon->indexNameN1, ptrMaps[i].ax, ptrMaps[i].ay))){

			tail = AddLEVELDRAW(&ptrMaps[i].level1);

		        tail->surface = current->surface;
			tail->offsetX = current->offsetX;
			tail->offsetY = current->offsetY;
			tail->next = NULL;
		    
			StoreAnimationFrame(ptrAddon->objectNameN1 * 2, ptrAddon->indexNameN1, &ptrMaps[i]);
		    }
		}

        	indexAddon = ptrAddon->indexAddon;
	    }
	}

	indexAddon = 0;

	// init level2
	for(j = 3; j >= 0; --j){
            
    	    if(0xFF != ptrMapsInfo[i].indexName2 && j == (ptrMapsInfo[i].quantity1 % 4)){

		if(NULL != (current = GetICNHEADERCellObject(ptrMapsInfo[i].objectName2, ptrMapsInfo[i].indexName2, ptrMaps[i].ax, ptrMaps[i].ay))){

		    tail = AddLEVELDRAW(&ptrMaps[i].level2);

		    tail->surface = current->surface;
		    tail->offsetX = current->offsetX;
		    tail->offsetY = current->offsetY;
		    tail->next = NULL;

		    StoreAnimationFrame(ptrMapsInfo[i].objectName2, ptrMapsInfo[i].indexName2, &ptrMaps[i]);
		}
	    }

    	    indexAddon = ptrMapsInfo[i].indexAddon;
        
    	    while(indexAddon){
        	ptrAddon = GetADDONTAIL(indexAddon);

        	if(0xFF != ptrAddon->indexNameN2 && j == (ptrAddon->quantityN % 4)){

		    if(NULL != (current = GetICNHEADERCellObject(ptrAddon->objectNameN2, ptrAddon->indexNameN2, ptrMaps[i].ax, ptrMaps[i].ay))){

			tail = AddLEVELDRAW(&ptrMaps[i].level2);

			tail->surface = current->surface;
			tail->offsetX = current->offsetX;
			tail->offsetY = current->offsetY;
			tail->next = NULL;

			StoreAnimationFrame(ptrAddon->objectNameN2, ptrAddon->indexNameN2, &ptrMaps[i]);
		    }
		}

        	indexAddon = ptrAddon->indexAddon;
    	    }
	}

	// monster
	if(0x2F < ptrMapsInfo[i].objectName1 && 0x34 > ptrMapsInfo[i].objectName1)
	    StoreAnimationFrame(ptrMapsInfo[i].objectName1, ptrMapsInfo[i].indexName1, &ptrMaps[i]);

    }

    // close file
    fclose(fd);

    // Освобождаем загруженные объекты TIL
    FreeObject("GROUND32.TIL");

    // Освобождаем использованную информацию
    free(ptrMapsInfo);
    free(tailAddon);

    fprintf(stderr, "InitMaps: %s\n", filename);

    // Рисуем экран и в цикл событий
    ACTION result = DrawMainDisplay();

    FreeCastle();
    FreeKingdom();
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
	    info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER1:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL1);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;
	    info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER2:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL2);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;
	    info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER3:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL3);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;
	    info->indexName1 = maps->object.monster;

	    break;

	case OBJ_RNDMONSTER4:

	    // монстры только в первой секции
	    if(addon) return;

	    maps->type = OBJ_MONSTER;
	    maps->object.monster = GetRNDMonster(MNS_LEVEL4);
	    maps->count = GetRNDSizeLevelMonster(GetLevelMonster(maps->object.monster));

	    info->generalObject = OBJ_MONSTER;
	    info->indexName1 = maps->object.monster;

	    break;

	    default:
		break;
	}

    return;
}

E_GROUND GetTypeGround(MP2TILEINFO *info){

    Uint16 index = info->tileIndex;

    // сканируем дорогу ROAD
    if(0x7A == info->objectName1) return ROAD;
    
    Uint16 indexAddon = info->indexAddon;
    MP2ADDONTAIL	*ptrAddon = NULL;
        
    while(indexAddon){

        ptrAddon = GetADDONTAIL(indexAddon);

	if(0x7A == ptrAddon->objectNameN1 * 2) return ROAD;

        indexAddon = ptrAddon->indexAddon;
    }

    // список поверхностей по индексу из GROUND32.TIL
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

E_GROUND GetGroundMaps(Uint8 ax, Uint8 ay){

    const S_CELLMAPS *maps = GetCELLMAPS(ay * GetWidthMaps() + ax);
    
    if(maps) return maps->ground;
    
    return WATER;
}

void MapsUpgradeTown2Castle(Uint8 ax, Uint8 ay){

/* 
 sprite castle OBJNTOWN.ICN 32x32 kngt barb srcr wrlc wizrd necr 
 sprite ground OBJNTWBA.ICN 32x32 grass snow swamp lava desert dirt wast beach 
 
             XXX XXX C00 XXX XXX     XXX XXX XXX XXX XXX 
     TTT TTT C01 C02 C03 C04 C05     XXX XXX XXX XXX XXX 
 TTT TTT TTT C06 C07 C08 C09 C10     XXX XXX XXX XXX XXX 
     TTT TTT C11 C12 AAA C14 C15     G00 G01 AAA G03 G04 
         TTT XXX XXX XXX XXX XXX     G05 G06 G07 G08 G09 
*/

    Uint8 index, i;
    Uint16 cx, cy;
    ICNHEADER *header = NULL;
    ICNHEADER *tail = NULL;
    AGGSPRITE sprite;

    const S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_CELLMAPS *maps = GetCELLMAPS(ay * GetWidthMaps() + ax);
 
    switch(maps->ground){
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
    // castle ground
    for(i = 0; i < 5; ++i){
        FillSPRITE(&sprite, "OBJNTWBA.ICN", index + i);
        header = GetICNHeader(&sprite);
	cx = ax - 2 + i;
	cy = ay;
	maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
	tail = AddLEVELDRAW(&maps->upgrade);
        tail->surface = header->surface;
        tail->offsetX = header->offsetX;
        tail->offsetY = header->offsetY;
        tail->next = NULL;
    }
    for(i = 0; i < 5; ++i){
        FillSPRITE(&sprite, "OBJNTWBA.ICN", index + 5 + i);
        header = GetICNHeader(&sprite);
	cx = ax - 2 + i;
	cy = ay + 1;
	maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
	tail = AddLEVELDRAW(&maps->upgrade);
        tail->surface = header->surface;
        tail->offsetX = header->offsetX;
        tail->offsetY = header->offsetY;
        tail->next = NULL;
    }

    switch(castle->race){
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
    FillSPRITE(&sprite, "OBJNTOWN.ICN", index);
    header = GetICNHeader(&sprite);
    cx = ax;
    cy = ay - 3;
    maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
    tail = AddLEVELDRAW(&maps->upgrade);
    tail->surface = header->surface;
    tail->offsetX = header->offsetX;
    tail->offsetY = header->offsetY;
    tail->next = NULL;
    for(i = 0; i < 5; ++i){
        FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 1 + i);
        header = GetICNHeader(&sprite);
	cx = ax - 2 + i;
	cy = ay - 2;
	maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
	tail = AddLEVELDRAW(&maps->upgrade);
        tail->surface = header->surface;
        tail->offsetX = header->offsetX;
        tail->offsetY = header->offsetY;
        tail->next = NULL;
    }
    for(i = 0; i < 5; ++i){
        FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 6 + i);
        header = GetICNHeader(&sprite);
	cx = ax - 2 + i;
	cy = ay - 1;
	maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
	tail = AddLEVELDRAW(&maps->upgrade);
        tail->surface = header->surface;
        tail->offsetX = header->offsetX;
        tail->offsetY = header->offsetY;
        tail->next = NULL;
    }
    for(i = 0; i < 5; ++i){
        FillSPRITE(&sprite, "OBJNTOWN.ICN", index + 11 + i);
        header = GetICNHeader(&sprite);
	cx = ax - 2 + i;
	cy = ay;
	maps = GetCELLMAPS(cy * GetWidthMaps() + cx);
	tail = AddLEVELDRAW(&maps->upgrade);
        tail->surface = header->surface;
        tail->offsetX = header->offsetX;
        tail->offsetY = header->offsetY;
        tail->next = NULL;
    }

    return;
}
