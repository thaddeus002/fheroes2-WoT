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
#include "draw.h"
#include "config.h"
#include "loadgame.h"

MP2HEADER       *ptrMP2Header = NULL;
CELLMAPS        *ptrMaps = NULL;

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

CELLMAPS *GetCELLMAPS(Uint16 index){

    if(ptrMaps)
        return &ptrMaps[index];

    return NULL;
}

void FreeMaps(void){

    int i;

    if(ptrMP2Header){

	if(ptrMaps){
	    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){
		if(ptrMaps[i].info) free(ptrMaps[i].info);
		if(ptrMaps[i].tile) SDL_FreeSurface(ptrMaps[i].tile);
	    }

	    free(ptrMaps);
	}

	free(ptrMP2Header);
    }
    
    if(tailAddon) free(tailAddon);
}

ACTION InitMaps(char *filename){

    FILE        *fd = NULL;

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
    if(NULL == (ptrMaps = (CELLMAPS *) malloc(sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps))){
	fprintf(stderr, "InitMaps: malloc: error size %d\n", sizeof(CELLMAPS) * ptrMP2Header->widthMaps * ptrMP2Header->heightMaps);
        return EXIT;
    }

    // читаем все ячейки карты
    Uint8  *ptrTILData = GetTILData("GROUND32.TIL");

    int i;

    for(i = 0; i < ptrMP2Header->heightMaps * ptrMP2Header->widthMaps; ++i){

	if(NULL == (ptrMaps[i].info = (MP2TILEINFO *) malloc(sizeof(MP2TILEINFO)))){
	    fprintf(stderr, "InitMaps: malloc: error size %d\n", sizeof(MP2TILEINFO));
	    return EXIT;
	}
	
	fread((char *) ptrMaps[i].info, 1, sizeof(MP2TILEINFO), fd);

        Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xF000;
        gmask = 0x0F00;
	bmask = 0x00F0;
	amask = 0x000F;
#else
        rmask = 0x000F;
        gmask = 0x00F0;
        bmask = 0x0F00;
        amask = 0xF000;
#endif
        if(NULL == (ptrMaps[i].tile = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, TILEWIDTH, TILEWIDTH, 16, rmask, gmask, bmask, 0))){
	    fprintf(stderr, "InitMaps: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), TILEWIDTH, TILEWIDTH);
	    return EXIT;
	}
	DrawTILBmp(ptrMaps[i].tile, ptrTILData, ptrMaps[i].info->tileIndex, ptrMaps[i].info->shape);
    }


    fread((char *) &countTail, 1, sizeof(Uint32), fd);

    if(filesize < (ftell(fd) + countTail * 15)){
	printf("InitMaps error: incorrect format maps.\n");
        return EXIT;
    }
    
    if(NULL == (tailAddon = (MP2ADDONTAIL *) malloc(countTail * sizeof(MP2ADDONTAIL)))){
	printf("InitMaps malloc: error size %lu\n", (unsigned long) countTail * sizeof(MP2ADDONTAIL));
        return EXIT;
    }

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

    // Освобождаем загруженные объекты TIL
    FreeObject("GROUND32.TIL");

    fprintf(stderr, "InitMaps: %s\n", filename);

    // Рисуем экран и в цикл событий
    ACTION result = DrawMainDisplay();

    FreeMaps();

    return result;
}

void PrintCellInfo(Uint16 index){

    if(! GetIntValue("debug")) return;

    const char *string = "";

    Uint32 count = 0;
    Uint16 indexAddon = 0;

    CELLMAPS *ptrCell = GetCELLMAPS(index);
    MP2ADDONTAIL *ptrAddon = NULL;

    fprintf(stderr, "---------------------\n");
    fprintf(stderr, "tileIndex    : %6hX\n", ptrCell->info->tileIndex);
    fprintf(stderr, "objectName1  : %6hhX\n", ptrCell->info->objectName1);
    fprintf(stderr, "indexName1   : %6hhX\n", ptrCell->info->indexName1);
    fprintf(stderr, "quantity1    : %6hhX\n", ptrCell->info->quantity1);
    fprintf(stderr, "quantity2    : %6hhX\n", ptrCell->info->quantity2);
    fprintf(stderr, "objectName2  : %6hhX\n", ptrCell->info->objectName2);
    fprintf(stderr, "indexName2   : %6hhX\n", ptrCell->info->indexName2);
    fprintf(stderr, "shape        : %6hhX\n", ptrCell->info->shape);
    fprintf(stderr, "generalObject: %6hhX\t", ptrCell->info->generalObject);

    fprintf(stderr, "%s", PrintObjectType(ptrCell->info->generalObject, ptrCell->info->indexName1));

    if(OBJ_MONSTER == ptrCell->info->generalObject){

	count = ptrCell->info->quantity2;
	count <<= 8;
	count |= ptrCell->info->quantity1;
	count >>= 3;
	if(count) fprintf(stderr, "\tCOUNT: %d", count);
	else fprintf(stderr, "\tCOUNT: (RANDOM)");
    }

    fprintf(stderr, "\n");

    //fprintf(stderr, "indexAddon   : %6hX\n", ptrCell->info->indexAddon);
    //fprintf(stderr, "uniqNumber1  : %6lX\n", (long unsigned int) ptrCell->info->uniqNumber1);
    //fprintf(stderr, "uniqNumber2  : %6lX\n", (long unsigned int) ptrCell->info->uniqNumber2);

    if(ptrCell->info->indexAddon > countTail)

	fprintf(stderr, "Ooops...\n");

    else{
    
	indexAddon = ptrCell->info->indexAddon;

	ptrAddon = GetADDONTAIL(indexAddon);
	
	while(indexAddon){

	    fprintf(stderr, "---------------------\n");

	    //fprintf(stderr, "indexAddon   : %6hX\n", ptrAddon->indexAddon);

	    fprintf(stderr, "objectNameN1 : %6hhX\n", ptrAddon->objectNameN1);
	    fprintf(stderr, "indexNameN1  : %6hhX", ptrAddon->indexNameN1);

	    if(0x3A == ptrAddon->objectNameN1)
    
        	switch(ptrAddon->indexNameN1){

            	    case OBJ_MINE_ORE:
            		string = "OBJ_MINE_ORE";
                	break;
                
            	    case OBJ_MINE_SULFUR:
                	string = "OBJ_MINE_SULFUR";
            		break;

            	    case OBJ_MINE_CRYSTAL:
                	string = "OBJ_MINE_CRYSTAL";
                	break;

            	    case OBJ_MINE_GEMS:
                	string = "OBJ_MINE_GEMS";
                	break;

            	    case OBJ_MINE_GOLD:
                	string = "OBJ_MINE_GOLD";
                	break;

            	    default:
                	string = "";
                	break;
                }

	    fprintf(stderr, "\t%s\n", string);

	    fprintf(stderr, "quantityN    : %6hhX\n", ptrAddon->quantityN);
	    fprintf(stderr, "objectNameN2 : %6hhX\n", ptrAddon->objectNameN2);
    	    fprintf(stderr, "indexNameN2  : %6hhX\n", ptrAddon->indexNameN2);

	    //fprintf(stderr, "uniqNumberN1 : %6lX\n", (long unsigned int) ptrAddon->uniqNumberN1);
	    //fprintf(stderr, "uniqNumberN2 : %6lX\n", (long unsigned int) ptrAddon->uniqNumberN2);

	    indexAddon = ptrAddon->indexAddon;
	}
    }

    fprintf(stderr, "---------------------\n\n\n");

}

const char * PrintObjectType(Uint8 type, Uint8 ext){

    const char	*string;

    switch(type){

        // пусто
        case OBJ_ZERO:
            string = "OBJ_ZERO";
            break;

        // башня алхимика, mercury
        case OBJN_ALCHEMYTOWER:
            string = "OBJN_ALCHEMYTOWER";
            break;

        // пещера демона
        case OBJN_DAEMONCAVE:
            string = "OBJN_DAEMONCAVE";
            break;

        // холм с мухоморами (+1 удача)
        case OBJN_FAERIERING:
            string = "OBJN_FAERIERING";
            break;

        // кладбище
        case OBJN_GRAVEYARD:
            string = "OBJN_GRAVEYARD";
            break;

        // город драконов
        case OBJN_DRAGONCITY:
            string = "OBJN_DRAGONCITY";
            break;

        // маяк
        case OBJN_LIGHTHOUSE:
            string = "OBJN_LIGHTHOUSE";
            break;

        // водяная мельница
        case OBJN_WATERMILL:
            string = "OBJN_WATERMILL";
            break;

        // шахта
        case OBJN_MINES:
            string = "OBJN_MINES";
            break;

        // обелиск
        case OBJN_OBELISK:
            string = "OBJN_OBELISK";
            break;

        // оазис
        case OBJN_OASIS:
            string = "OBJN_OASIS";
            break;

        // побережье (граница между морем и сушей)
        case OBJ_COAST:
            string = "OBJ_COAST";
            break;

        // лесопилка
        case OBJN_SAWMILL:
            string = "OBJN_SAWMILL";
            break;

        // оракул
        case OBJN_ORACLE:
            string = "OBJN_ORACLE";
            break;

        // разрушенный корабль (правый)
        case OBJN_SHIPWRECK:
            string = "OBJN_SHIPWRECK";
            break;

        // тент кочевников nomads
        case OBJN_DESERTTENT:
            string = "OBJN_DESERTTENT";
            break;

        // замок
        case OBJN_CASTLE:
            string = "OBJN_CASTLE";
            break;

        // вагоны на поляне (rogues)
        case OBJN_WAGONCAMP:
            string = "OBJN_WAGONCAMP";
            break;

        // мельница
        case OBJN_WINDMILL:
            string = "OBJN_WINDMILL";
            break;

        // random город
        case OBJN_RNDTOWN:
            string = "OBJN_RNDTOWN";
            break;

        // random замок
        case OBJN_RNDCASTLE:
            string = "OBJN_RNDCASTLE";
            break;

        // трещина малая
        case OBJ_SMALLCRACK:
            string = "OBJ_SMALLCRACK";
            break;

        // тройной дом спрайтов
        case OBJN_TREECITY:
            string = "OBJN_TREECITY";
            break;

        // medusa hunt
        case OBJN_RUINS:
            string = "OBJN_RUINS";
            break;

        // повышение навыка (+1 defence)
        case OBJN_FORT:
            string = "OBJN_FORT";
            break;

        // рынок
        case OBJN_TRADINGPOST:
            string = "OBJN_TRADINGPOST";
            break;

        // заброшенная шахта
        case OBJN_ABANDONEDMINE:
            string = "OBJN_ABANDONEDMINE";
            break;

        // дерево знаний
        case OBJN_TREEKNOWLEDGE:
            string = "OBJN_TREEKNOWLEDGE";
            break;

        // хижина повышения знаний (+1 knowledge)
        case OBJN_DOCTORHUNT:
            string = "OBJN_DOCTORHUNT";
            break;

        // храм
        case OBJN_TEMPLE:
            string = "OBJN_TEMPLE";
            break;

        // форт для upgrade orc ogre
        case OBJN_HILLFORT:
            string = "OBJN_HILLFORT";
            break;

        // нора halfling
        case OBJN_HALFLINGHOLE:
            string = "OBJN_HALFLINGHOLE";
            break;

        // палатка повышения атаки (+1 attack)
        case OBJN_MERCENARYCAMP:
            string = "OBJN_MERCENARYCAMP";
            break;

        // пирамида
        case OBJN_PIRAMID:
            string = "OBJN_PIRAMID";
            break;

        // город мертвых, руины в песке
        case OBJN_CITYDEAD:
            string = "OBJN_CITYDEAD";
            break;

        // развалины дома, в песке
        case OBJN_EXCAVATION:
            string = "OBJN_EXCAVATION";
            break;

        // сфинкс
        case OBJN_SPHINX:
            string = "OBJN_SPHINX";
            break;

        // озеро нефти
        case OBJ_OILLAKE:
            string = "OBJ_OILLAKE";
            break;

        // мост троллей
        case OBJN_TROLLBRIDGE:
            string = "OBJN_TROLLBRIDGE";
            break;

        // хижина приобретения вторичного навыка (skill)
        case OBJN_WITCHHUNT:
            string = "OBJN_WITCHHUNT";
            break;

        // замок дворецкого
        case OBJN_XANADU:
            string = "OBJN_XANADU";
            break;

        // пещера кентавров
        case OBJN_CAVE:
            string = "OBJN_CAVE";
            break;

        // лодочник
        case OBJN_MAGELLANMAPS:
            string = "OBJN_MAGELLANMAPS";
            break;

        // разрушенный корабль (левый)
        case OBJN_DERELICTSHIP:
            string = "OBJN_DERELICTSHIP";
            break;

        // башня обзора
        case OBJN_OBSERVATIONTOWER:
            string = "OBJN_OBSERVATIONTOWER";
            break;

        // кузница upgrade
        case OBJN_FREEMANFOUNDRY:
            string = "OBJN_FREEMANFOUNDRY";
            break;

        // деревья
        case OBJ_TREES:
            string = "OBJ_TREES";
            break;

        // горы
        case OBJ_MOUNTS:
            string = "OBJ_MOUNTS";
            break;

        // камни
        case OBJ_STONES:
            string = "OBJ_STONES";
            break;

        // цветы, растительность
        case OBJ_VEGETATION1:
            string = "OBJ_VEGETATION1";
            break;

        // озеро воды
        case OBJ_WATERLAKE:
            string = "OBJ_WATERLAKE";
            break;

        // цветы, растительность
        case OBJ_VEGETATION2:
            string = "OBJ_VEGETATION2";
            break;

        // трещина большая
        case OBJ_BIGCRACK:
            string = "OBJ_BIGCRACK";
            break;

        // озеро лавы
        case OBJ_LAVALAKE:
            string = "OBJ_LAVALAKE";
            break;

        // цветы, растительность
        case OBJ_VEGETATION3:
            string = "OBJ_VEGETATION3";
            break;

        // башня алхимика mercury, точка входа
        case OBJ_ALCHEMYTOWER:
            string = "OBJ_ALCHEMYTOWER";
            break;

        // буй на воде
        case OBJ_BUOY:
            string = "OBJ_BUOY";
            break;

        // скелет странника в пустыне
        case OBJ_SKELETON:
            string = "OBJ_SKELETON";
            break;

        // пещера демона, точка входа
        case OBJ_DAEMONCAVE:
            string = "OBJ_DAEMONCAVE";
            break;

        // сундук с сокровищами
        case OBJ_TREASURECHEST:
            string = "OBJ_TREASURECHEST";
            break;

        // холм с мухоморами (+1 удача), точка входа
        case OBJ_FAERIERING:
            string = "OBJ_FAERIERING";
            break;

        // брошенный костер
        case OBJ_CAMPFIRE:
            string = "OBJ_CAMPFIRE";
            break;

        // фонтан +luck
        case OBJ_FOUNTAIN:
            string = "OBJ_FOUNTAIN";
            break;

        // беседка (+1000 exp)
        case OBJ_GAZEBO:
            string = "OBJ_GAZEBO";
            break;

        // лампа джина
        case OBJ_ANCIENTLAMP:
            string = "OBJ_ANCIENTLAMP";
            break;

        // кладбище, точка входа
        case OBJ_GRAVEYARD:
            string = "OBJ_GRAVEYARD";
            break;

        // хижина лучников
        case OBJ_ARCHERHOUSE:
            string = "OBJ_ARCHERHOUSE";
            break;

        // хижина гоблинов
        case OBJ_GOBLINHUNT:
            string = "OBJ_GOBLINHUNT";
            break;

        // хижина dwarf
        case OBJ_DWARFCOTT:
            string = "OBJ_DWARFCOTT";
            break;

        // хижина крестьянина
        case OBJ_PEASANTHUNT:
            string = "OBJ_PEASANTHUNT";
            break;

        // город драконов, точка входа
        case OBJ_DRAGONCITY:
            string = "OBJ_DRAGONCITY";
            break;

        // маяк, точка входа
        case OBJ_LIGHTHOUSE:
            string = "OBJ_LIGHTHOUSE";
            break;

        // водяная мельница, точка входа
        case OBJ_WATERMILL:
            string = "OBJ_WATERMILL";
            break;

        // шахта, точка входа
        case OBJ_MINES:
            string = "OBJ_MINES";
            break;

        // монстры
        case OBJ_MONSTER:

	    switch(ext){
	    
		case OBJ_MONSTER_PEASANT:
		    string = "OBJ_MONSTER_PEASANT";
		    break;

		case OBJ_MONSTER_ARCHER:
		    string = "OBJ_MONSTER_ARCHER";
		    break;

		case OBJ_MONSTER_RANGER:
		    string = "OBJ_MONSTER_RANGER";
		    break;

		case OBJ_MONSTER_PIKEMAN:
		    string = "OBJ_MONSTER_PIKEMAN";
		    break;


		case OBJ_MONSTER_VETERAN_PIKEMAN:
		    string = "OBJ_MONSTER_VETERAN_PIKEMAN";
		    break;


		case OBJ_MONSTER_SWORDSMAN:
		    string = "OBJ_MONSTER_SWORDSMAN";
		    break;


		case OBJ_MONSTER_MASTER_SWORDSMAN:
		    string = "OBJ_MONSTER_MASTER_SWORDSMAN";
		    break;


		case OBJ_MONSTER_CAVALIRY:
		    string = "OBJ_MONSTER_CAVALIRY";
		    break;


		case OBJ_MONSTER_CHAMPION:
		    string = "OBJ_MONSTER_CHAMPION";
		    break;


		case OBJ_MONSTER_PALADIN:
		    string = "OBJ_MONSTER_PALADIN";
		    break;


		case OBJ_MONSTER_CRUSADER:
		    string = "OBJ_MONSTER_CRUSADER";
		    break;


		case OBJ_MONSTER_GOBLIN:
		    string = "OBJ_MONSTER_GOBLIN";
		    break;


		case OBJ_MONSTER_ORC:
		    string = "OBJ_MONSTER_ORC";
		    break;


		case OBJ_MONSTER_CHIEF_ORC:
		    string = "OBJ_MONSTER_CHIEF_ORC";
		    break;


		case OBJ_MONSTER_WOLF:
		    string = "OBJ_MONSTER_WOLF";
		    break;


		case OBJ_MONSTER_OGRE:
		    string = "OBJ_MONSTER_OGRE";
		    break;


		case OBJ_MONSTER_LORD_OGRE:
		    string = "OBJ_MONSTER_LORD_OGRE";
		    break;


		case OBJ_MONSTER_TROLL:
		    string = "OBJ_MONSTER_TROLL";
		    break;


		case OBJ_MONSTER_WAR_TROLL:
		    string = "OBJ_MONSTER_WAR_TROLL";
		    break;


		case OBJ_MONSTER_CYCLOPS:
		    string = "OBJ_MONSTER_CYCLOPS";
		    break;


		case OBJ_MONSTER_SPRITE:
		    string = "OBJ_MONSTER_SPRITE";
		    break;


		case OBJ_MONSTER_DWARF:
		    string = "OBJ_MONSTER_DWARF";
		    break;


		case OBJ_MONSTER_BATTLE_DWARF:
		    string = "OBJ_MONSTER_BATTLE_DWARF";
		    break;


		case OBJ_MONSTER_ELF:
		    string = "OBJ_MONSTER_ELF";
		    break;


		case OBJ_MONSTER_GRAND_ELF:
		    string = "OBJ_MONSTER_GRAND_ELF";
		    break;


		case OBJ_MONSTER_DRUID:
		    string = "OBJ_MONSTER_DRUID";
		    break;


		case OBJ_MONSTER_GREATER_DRUID:
		    string = "OBJ_MONSTER_GREATER_DRUID";
		    break;


		case OBJ_MONSTER_UNICORN:
		    string = "OBJ_MONSTER_UNICORN";
		    break;


		case OBJ_MONSTER_PHOENIX:
		    string = "OBJ_MONSTER_PHOENIX";
		    break;


		case OBJ_MONSTER_CENTAUR:
		    string = "OBJ_MONSTER_CENTAUR";
		    break;


		case OBJ_MONSTER_GARGOYLE:
		    string = "OBJ_MONSTER_GARGOYLE";
		    break;


		case OBJ_MONSTER_GRIFFIN:
		    string = "OBJ_MONSTER_GRIFFIN";
		    break;


		case OBJ_MONSTER_MINOTAUR:
		    string = "OBJ_MONSTER_MINOTAUR";
		    break;


		case OBJ_MONSTER_KNIGHT_MINOTAUR:
		    string = "OBJ_MONSTER_KNIGHT_MINOTAUR";
		    break;


		case OBJ_MONSTER_HIDRA:
		    string = "OBJ_MONSTER_HIDRA";
		    break;


		case OBJ_MONSTER_GREEN_DRAGON:
		    string = "OBJ_MONSTER_GREEN_DRAGON";
		    break;


		case OBJ_MONSTER_RED_DRAGON:
		    string = "OBJ_MONSTER_RED_DRAGON";
		    break;


		case OBJ_MONSTER_BLACK_DRAGON:
		    string = "OBJ_MONSTER_BLACK_DRAGON";
		    break;


		case OBJ_MONSTER_HALFLING:
		    string = "OBJ_MONSTER_HALFLING";
		    break;


		case OBJ_MONSTER_BOAR:
		    string = "OBJ_MONSTER_BOAR";
		    break;


		case OBJ_MONSTER_IRON_GOLEM:
		    string = "OBJ_MONSTER_IRON_GOLEM";
		    break;


		case OBJ_MONSTER_STEEL_GOLEM:
		    string = "OBJ_MONSTER_STEEL_GOLEM";
		    break;


		case OBJ_MONSTER_ROC:
		    string = "OBJ_MONSTER_ROC";
		    break;


		case OBJ_MONSTER_MAGE:
		    string = "OBJ_MONSTER_MAGE";
		    break;


		case OBJ_MONSTER_ARCHMAGE:
		    string = "OBJ_MONSTER_ARCHMAGE";
		    break;


		case OBJ_MONSTER_GIANT:
		    string = "OBJ_MONSTER_GIANT";
		    break;


		case OBJ_MONSTER_TITAN:
		    string = "OBJ_MONSTER_TITAN";
		    break;


		case OBJ_MONSTER_SKELETON:
		    string = "OBJ_MONSTER_SKELETON";
		    break;


		case OBJ_MONSTER_ZOMBIE:
		    string = "OBJ_MONSTER_ZOMBIE";
		    break;


		case OBJ_MONSTER_MUTANT_ZOMBIE:
		    string = "OBJ_MONSTER_MUTANT_ZOMBIE";
		    break;


		case OBJ_MONSTER_MUMMY:
		    string = "OBJ_MONSTER_MUMMY";
		    break;


		case OBJ_MONSTER_ROYAL_MUMMY:
		    string = "OBJ_MONSTER_ROYAL_MUMMY";
		    break;


		case OBJ_MONSTER_VAMPIRE:
		    string = "OBJ_MONSTER_VAMPIRE";
		    break;


		case OBJ_MONSTER_LORD_VAMPIRE:
		    string = "OBJ_MONSTER_LORD_VAMPIRE";
		    break;


		case OBJ_MONSTER_LICH:
		    string = "OBJ_MONSTER_LICH";
		    break;


		case OBJ_MONSTER_POWER_LICH:
		    string = "OBJ_MONSTER_POWER_LICH";
		    break;


		case OBJ_MONSTER_BONE_DRAGON:
		    string = "OBJ_MONSTER_BONE_DRAGON";
		    break;


		case OBJ_MONSTER_ROGUE:
		    string = "OBJ_MONSTER_ROGUE";
		    break;


		case OBJ_MONSTER_NOMAD:
		    string = "OBJ_MONSTER_NOMAD";
		    break;


		case OBJ_MONSTER_GHOST:
		    string = "OBJ_MONSTER_GHOST";
		    break;


		case OBJ_MONSTER_GENIE:
		    string = "OBJ_MONSTER_GENIE";
		    break;


		case OBJ_MONSTER_MEDUSA:
		    string = "OBJ_MONSTER_MEDUSA";
		    break;


		case OBJ_MONSTER_EARTH_ELEMENT:
		    string = "OBJ_MONSTER_EARTH_ELEMENT";
		    break;


		case OBJ_MONSTER_AIR_ELEMENT:
		    string = "OBJ_MONSTER_AIR_ELEMENT";
		    break;


		case OBJ_MONSTER_FIRE_ELEMENT:
		    string = "OBJ_MONSTER_FIRE_ELEMENT";
		    break;


		case OBJ_MONSTER_WATER_ELEMENT:
		    string = "OBJ_MONSTER_WATER_ELEMENT";
		    break;

		case OBJ_MONSTER_RANDOM_LEVEL1:
		    string = "OBJ_MONSTER_RANDOM_LEVEL1";
		    break;

		case OBJ_MONSTER_RANDOM_LEVEL2:
		    string = "OBJ_MONSTER_RANDOM_LEVEL2";
		    break;

		case OBJ_MONSTER_RANDOM_LEVEL3:
		    string = "OBJ_MONSTER_RANDOM_LEVEL3";
		    break;

		case OBJ_MONSTER_RANDOM_LEVEL4:
		    string = "OBJ_MONSTER_RANDOM_LEVEL4";
		    break;

 		default:
		    string = "OBJ_MONSTER: UNKNOWN ?!";
		    break;
	    }
            break;

        // обелиск, точка входа
        case OBJ_OBELISK:
            string = "OBJ_OBELISK";
            break;

        // оазис, точка входа
        case OBJ_OASIS:
            string = "OBJ_OASIS";
            break;

        // ресурсы тип определяется по indexName1 (1: wood, 3: mercury, 5: ore, 7: sulfur, 8: crystal, B: gems, D: gold)
        case OBJ_RESOURCE:

	    switch(ext){
		
		case OBJ_RESOURCE_WOOD:
		    string = "OBJ_RESOURCE_WOOD";
		    break;
		
		case OBJ_RESOURCE_MERCURY:
		    string = "OBJ_RESOURCE_MERCURY";
		    break;
		
		case OBJ_RESOURCE_ORE:
		    string = "OBJ_RESOURCE_ORE";
		    break;
		
		case OBJ_RESOURCE_SULFUR:
		    string = "OBJ_RESOURCE_SULFUR";
		    break;
		
		case OBJ_RESOURCE_CRYSTAL:
		    string = "OBJ_RESOURCE_CRYSTAL";
		    break;
		    
		case OBJ_RESOURCE_GEMS:
		    string = "OBJ_RESOURCE_GEMS";
		    break;
		    
		case OBJ_RESOURCE_GOLD:
		    string = "OBJ_RESOURCE_GOLD";
		    break;
		    
		default:
		    string = "RESOURCE: UNKNOWN ?!";
	    }
            break;

        // лесопилка, точка входа
        case OBJ_SAWMILL:
            string = "OBJ_SAWMILL";
            break;

        // оракул
        case OBJ_ORACLE:
            string = "OBJ_ORACLE";
            break;

        // хижина магии 1 уровня
        case OBJ_SHRINE1:
            string = "OBJ_SHRINE1";
            break;

        // разрушенный корабль, точка входа
        case OBJ_SHIPWRECK:
            string = "OBJ_SHIPWRECK";
            break;

        // тент кочевников nomads, точка входа
        case OBJ_DESERTTENT:
            string = "OBJ_DESERTTENT";
            break;

        // замок, точка входа
        case OBJ_CASTLE:
            string = "OBJ_CASTLE";
            break;

        // телепорт
        case OBJ_STONELITHS:
            string = "OBJ_STONELITHS";
            break;

        // вагоны на поляне (rogues), точка входа
        case OBJ_WAGONCAMP:
            string = "OBJ_WAGONCAMP";
            break;

        // водоворот
        case OBJ_WHIRLPOOL:
            string = "OBJ_WHIRLPOOL";
            break;

        // мельница, точка входа
        case OBJ_WINDMILL:
            string = "OBJ_WINDMILL";
            break;

        // артифакт
        case OBJ_ARTIFACT:

	    switch(ext){

		case OBJ_ARTIFACT_ARCANE_NECKLACE:
		    string = "OBJ_ARTIFACT_ARCANE_NECKLACE";
		    break;

		case OBJ_ARTIFACT_CASTER_BRACELET:
		    string = "OBJ_ARTIFACT_CASTER_BRACELET";
		    break;

		case OBJ_ARTIFACT_MAGE_RING:
		    string = "OBJ_ARTIFACT_MAGE_RING";
		    break;

		case OBJ_ARTIFACT_WITCHES_BROACH:
		    string = "OBJ_ARTIFACT_WITCHES_BROACH";
		    break;

		case OBJ_ARTIFACT_MEDAL_VALOR:
		    string = "OBJ_ARTIFACT_MEDAL_VALOR";
		    break;

		case OBJ_ARTIFACT_MEDAL_COURAGE:
		    string = "OBJ_ARTIFACT_MEDAL_COURAGE";
		    break;

		case OBJ_ARTIFACT_MEDAL_HONOR:
		    string = "OBJ_ARTIFACT_MEDAL_HONOR";
		    break;

		case OBJ_ARTIFACT_MEDAL_DISTINCTION:
		    string = "OBJ_ARTIFACT_MEDAL_DISTINCTION";
		    break;

		case OBJ_ARTIFACT_FIZBIN_MISFORTUNE:
		    string = "OBJ_ARTIFACT_FIZBIN_MISFORTUNE";
		    break;

		case OBJ_ARTIFACT_THUNDER_MACE:
		    string = "OBJ_ARTIFACT_THUNDER_MACE";
		    break;

		case OBJ_ARTIFACT_ARMORED_GAUNTLETS:
		    string = "OBJ_ARTIFACT_ARMORED_GAUNTLETS";
		    break;

		case OBJ_ARTIFACT_DEFENDER_HELM:
		    string = "OBJ_ARTIFACT_DEFENDER_HELM";
		    break;

		case OBJ_ARTIFACT_GIANT_FLAIL:
		    string = "OBJ_ARTIFACT_GIANT_FLAIL";
		    break;

		case OBJ_ARTIFACT_BALLISTA:
		    string = "OBJ_ARTIFACT_BALLISTA";
		    break;

		case OBJ_ARTIFACT_STEALTH_SHIELD:
		    string = "OBJ_ARTIFACT_STEALTH_SHIELD";
		    break;

		case OBJ_ARTIFACT_DRAGON_SWORD:
		    string = "OBJ_ARTIFACT_DRAGON_SWORD";
		    break;

		case OBJ_ARTIFACT_POWER_AXE:
		    string = "OBJ_ARTIFACT_POWER_AXE";
		    break;

		case OBJ_ARTIFACT_DIVINE_BREASTPLATE:
		    string = "OBJ_ARTIFACT_DIVINE_BREASTPLATE";
		    break;

		case OBJ_ARTIFACT_MINOR_SCROLL:
		    string = "OBJ_ARTIFACT_MINOR_SCROLL";
		    break;

		case OBJ_ARTIFACT_MAJOR_SCROLL:
		    string = "OBJ_ARTIFACT_MAJOR_SCROLL";
		    break;

		case OBJ_ARTIFACT_SUPERIOR_SCROLL:
		    string = "OBJ_ARTIFACT_SUPERIOR_SCROLL";
		    break;

		case OBJ_ARTIFACT_FOREMOST_SCROLL:
		    string = "OBJ_ARTIFACT_FOREMOST_SCROLL";
		    break;

		case OBJ_ARTIFACT_ENDLESS_SACK_GOLD:
		    string = "OBJ_ARTIFACT_ENDLESS_SACK_GOLD";
		    break;

		case OBJ_ARTIFACT_ENDLESS_BAG_GOLD:
		    string = "OBJ_ARTIFACT_ENDLESS_BAG_GOLD";
		    break;

		case OBJ_ARTIFACT_ENDLESS_PURSE_GOLD:
		    string = "OBJ_ARTIFACT_ENDLESS_PURSE_GOLD";
		    break;

		case OBJ_ARTIFACT_NOMAD_BOOTS_MOBILITY:
		    string = "OBJ_ARTIFACT_NOMAD_BOOTS_MOBILITY";
		    break;

		case OBJ_ARTIFACT_TRAVELER_BOOTS_MOBILITY:
		    string = "OBJ_ARTIFACT_TRAVELER_BOOTS_MOBILITY";
		    break;

		case OBJ_ARTIFACT_RABBIT_FOOT:
		    string = "OBJ_ARTIFACT_RABBIT_FOOT";
		    break;

		case OBJ_ARTIFACT_GOLDEN_HORSESHOE:
		    string = "OBJ_ARTIFACT_GOLDEN_HORSESHOE";
				break;

		case OBJ_ARTIFACT_GAMBLER_LUCKY_COIN:
		    string = "OBJ_ARTIFACT_GAMBLER_LUCKY_COIN";
		    break;

		case OBJ_ARTIFACT_FOUR_LEAF_CLOVER:
		    string = "OBJ_ARTIFACT_FOUR_LEAF_CLOVER";
		    break;

		case OBJ_ARTIFACT_TRUE_COMPASS_MOBILITY:
		    string = "OBJ_ARTIFACT_TRUE_COMPASS_MOBILITY";
		    break;

		case OBJ_ARTIFACT_SAILORS_ASTROLABE_MOBILITY:
		    string = "OBJ_ARTIFACT_SAILORS_ASTROLABE_MOBILITY";
		    break;

		case OBJ_ARTIFACT_EVIL_EYE:
		    string = "OBJ_ARTIFACT_EVIL_EYE";
		    break;

		case OBJ_ARTIFACT_ENCHANTED_HOURGLASS:
		    string = "OBJ_ARTIFACT_ENCHANTED_HOURGLASS";
		    break;

		case OBJ_ARTIFACT_GOLD_WATCH:
		    string = "OBJ_ARTIFACT_GOLD_WATCH";
		    break;

		case OBJ_ARTIFACT_SKULLCAP:
		    string = "OBJ_ARTIFACT_SKULLCAP";
		    break;

		case OBJ_ARTIFACT_ICE_CLOAK:
		    string = "OBJ_ARTIFACT_ICE_CLOAK";
		    break;

		case OBJ_ARTIFACT_FIRE_CLOAK:
		    string = "OBJ_ARTIFACT_FIRE_CLOAK";
		    break;

		case OBJ_ARTIFACT_LIGHTNING_HELM:
		    string = "OBJ_ARTIFACT_LIGHTNING_HELM";
		    break;

		case OBJ_ARTIFACT_EVERCOLD_ICICLE:
		    string = "OBJ_ARTIFACT_EVERCOLD_ICICLE";
		    break;

		case OBJ_ARTIFACT_EVERHOT_LAVA_ROCK:
		    string = "OBJ_ARTIFACT_EVERHOT_LAVA_ROCK";
		    break;

		case OBJ_ARTIFACT_LIGHTNING_ROD:
		    string = "OBJ_ARTIFACT_LIGHTNING_ROD";
		    break;

		case OBJ_ARTIFACT_SNAKE_RING:
		    string = "OBJ_ARTIFACT_SNAKE_RING";
		    break;

		case OBJ_ARTIFACT_ANKH:
		    string = "OBJ_ARTIFACT_ANKH";
		    break;

		case OBJ_ARTIFACT_BOOK_ELEMENTS:
		    string = "OBJ_ARTIFACT_BOOK_ELEMENTS";
		    break;

		case OBJ_ARTIFACT_ELEMENTAL_RING:
		    string = "OBJ_ARTIFACT_ELEMENTAL_RING";
		    break;

		case OBJ_ARTIFACT_HOLY_PENDANT:
		    string = "OBJ_ARTIFACT_HOLY_PENDANT";
		    break;

		case OBJ_ARTIFACT_PENDANT_FREE_WILL:
		    string = "OBJ_ARTIFACT_PENDANT_FREE_WILL";
		    break;

		case OBJ_ARTIFACT_PENDANT_LIFE:
		    string = "OBJ_ARTIFACT_PENDANT_LIFE";
		    break;

		case OBJ_ARTIFACT_SERENITY_PENDANT:
		    string = "OBJ_ARTIFACT_SERENITY_PENDANT";
		    break;

		case OBJ_ARTIFACT_SEEING_EYE_PENDANT:
		    string = "OBJ_ARTIFACT_SEEING_EYE_PENDANT";
		    break;

		case OBJ_ARTIFACT_KINETIC_PENDANT:
		    string = "OBJ_ARTIFACT_KINETIC_PENDANT";
		    break;

		case OBJ_ARTIFACT_PENDANT_DEATH:
		    string = "OBJ_ARTIFACT_PENDANT_DEATH";
		    break;

		case OBJ_ARTIFACT_WAND_NEGATION:
		    string = "OBJ_ARTIFACT_WAND_NEGATION";
		    break;

		case OBJ_ARTIFACT_GOLDEN_BOW:
		    string = "OBJ_ARTIFACT_GOLDEN_BOW";
		    break;

		case OBJ_ARTIFACT_TELESCOPE:
		    string = "OBJ_ARTIFACT_TELESCOPE";
		    break;

		case OBJ_ARTIFACT_STATESMAN_QUILL:
		    string = "OBJ_ARTIFACT_STATESMAN_QUILL";
		    break;

		case OBJ_ARTIFACT_WIZARD_HAT:
		    string = "OBJ_ARTIFACT_WIZARD_HAT";
		    break;

		case OBJ_ARTIFACT_POWER_RING:
		    string = "OBJ_ARTIFACT_POWER_RING";
		    break;

		case OBJ_ARTIFACT_AMMO_CART:
		    string = "OBJ_ARTIFACT_AMMO_CART";
		    break;

		case OBJ_ARTIFACT_TAX_LIEN:
		    string = "OBJ_ARTIFACT_TAX_LIEN";
		    break;

		case OBJ_ARTIFACT_HIDEOUS_MASK:
		    string = "OBJ_ARTIFACT_HIDEOUS_MASK";
		    break;

		case OBJ_ARTIFACT_ENDLESS_POUCH_SULFUR:
		    string = "OBJ_ARTIFACT_ENDLESS_POUCH_SULFUR";
		    break;

		case OBJ_ARTIFACT_ENDLESS_VIAL_MERCURY:
		    string = "OBJ_ARTIFACT_ENDLESS_VIAL_MERCURY";
		    break;

		case OBJ_ARTIFACT_ENDLESS_POUCH_GEMS:
		    string = "OBJ_ARTIFACT_ENDLESS_POUCH_GEMS";
		    break;

		case OBJ_ARTIFACT_ENDLESS_CORD_WOOD:
		    string = "OBJ_ARTIFACT_ENDLESS_CORD_WOOD";
		    break;

		case OBJ_ARTIFACT_ENDLESS_CART_ORE:
		    string = "OBJ_ARTIFACT_ENDLESS_CART_ORE";
		    break;

		case OBJ_ARTIFACT_ENDLESS_POUCH_CRYSTAL:
		    string = "OBJ_ARTIFACT_ENDLESS_POUCH_CRYSTAL";
		    break;

		case OBJ_ARTIFACT_SPIKED_HELM:
		    string = "OBJ_ARTIFACT_SPIKED_HELM";
		    break;

		case OBJ_ARTIFACT_SPIKED_SHIELD:
		    string = "OBJ_ARTIFACT_SPIKED_SHIELD";
		    break;

		case OBJ_ARTIFACT_WHITE_PEARL:
		    string = "OBJ_ARTIFACT_WHITE_PEARL";
		    break;

		case OBJ_ARTIFACT_BLACK_PEARL:
		    string = "OBJ_ARTIFACT_BLACK_PEARL";
		    break;

		default:
		    string = "OBJ_ARTIFACT: UNKNOWN ?!";
	    }
            break;

        // лодка
        case OBJ_BOAT:
            string = "OBJ_BOAT";
            break;

        // random артифакт
        case OBJ_RNDARTIFACT:
            string = "OBJ_RNDARTIFACT";
            break;

        // random ресурс
        case OBJ_RNDRESOURCE:
            string = "OBJ_RNDRESOURCE";
            break;

        // random город, точка входа
        case OBJ_RNDTOWN:
            string = "OBJ_RNDTOWN";
            break;

        // random замок, точка входа
        case OBJ_RNDCASTLE:
            string = "OBJ_RNDCASTLE";
            break;

        // radom monster level1
        case OBJ_RNDMONSTER1:
            string = "OBJ_RNDMONSTER1";
            break;

        // radom monster level2
        case OBJ_RNDMONSTER2:
            string = "OBJ_RNDMONSTER2";
            break;

        // radom monster level3
        case OBJ_RNDMONSTER3:
            string = "OBJ_RNDMONSTER3";
            break;

        // radom monster level4
        case OBJ_RNDMONSTER4:
            string = "OBJ_RNDMONSTER4";
            break;

        // домик орков
        case OBJ_WATCHTOWER:
            string = "OBJ_WATCHTOWER";
            break;

        // домик спрайтов (маленький)
        case OBJ_TREEHOUSE:
            string = "OBJ_TREEHOUSE";
            break;

        // тройной дом спрайтов, точка входа
        case OBJ_TREECITY:
            string = "OBJ_TREECITY";
            break;

        // medusa hunt, точка входа
        case OBJ_RUINS:
            string = "OBJ_RUINS";
            break;

        // хижина повышение навыка (+1 defence), точка входа
        case OBJ_FORT:
            string = "OBJ_FORT";
            break;

        // рынок, точка входа
        case OBJ_TRADINGPOST:
            string = "OBJ_TRADINGPOST";
            break;

        // заброшенная шахта, точка входа
        case OBJ_ABANDONEDMINE:
            string = "OBJ_ABANDONEDMINE";
            break;

        // стоунхедж, (+1 power)
        case OBJ_STANDINGSTONES:
            string = "OBJ_STANDINGSTONES";
            break;

        // идол
        case OBJ_IDOL:
            string = "OBJ_IDOL";
            break;

        // дерево знаний, точка входа
        case OBJ_TREEKNOWLEDGE:
            string = "OBJ_TREEKNOWLEDGE";
            break;

        // хижина повышения знаний (+1 knowledge), точка входа
        case OBJ_DOCTORHUNT:
            string = "OBJ_DOCTORHUNT";
            break;

        // храм, точка входа
        case OBJ_TEMPLE:
            string = "OBJ_TEMPLE";
            break;

        // форт для upgrade orc ogre, точка входа
        case OBJ_HILLFORT:
            string = "OBJ_HILLFORT";
            break;

        // нора halfling, точка входа
        case OBJ_HALFLINGHOLE:
            string = "OBJ_HALFLINGHOLE";
            break;

        // палатка повышения атаки (+1 attack), точка входа
        case OBJ_MERCENARYCAMP:
            string = "OBJ_MERCENARYCAMP";
            break;

        // хижина магии 2 уровня
        case OBJ_SHRINE2:
            string = "OBJ_SHRINE2";
            break;

        // хижина магии 3 уровня
        case OBJ_SHRINE3:
            string = "OBJ_SHRINE3";
            break;

        // пирамида, точка входа
        case OBJ_PIRAMID:
            string = "OBJ_PIRAMID";
            break;

        // город мертвых, руины в песке, точка входа
        case OBJ_CITYDEAD:
            string = "OBJ_CITYDEAD";
            break;

        // развалины дома, в песке, точка входа
        case OBJ_EXCAVATION:
            string = "OBJ_EXCAVATION";
            break;

        // сфинкс, точка входа
        case OBJ_SPHINX:
            string = "OBJ_SPHINX";
            break;

        // брошенная тачка
        case OBJ_WAGON:
            string = "OBJ_WAGON";
            break;

        // родник
        case OBJ_ARTESIANSPRING:
            string = "OBJ_ARTESIANSPRING";
            break;

        // мост троллей, точка входа
        case OBJ_TROLLBRIDGE:
            string = "OBJ_TROLLBRIDGE";
            break;

        // озеро на cracked
        case OBJ_CRAKEDLAKE:
            string = "OBJ_CRAKEDLAKE";
            break;

        // хижина приобретения вторичного навыка (skill), точка входа
        case OBJ_WITCHHUNT:
            string = "OBJ_WITCHHUNT";
            break;

        // замок дворецкого, точка входа
        case OBJ_XANADU:
            string = "OBJ_XANADU";
            break;

        // пещера кентавров, точка входа
        case OBJ_CAVE:
            string = "OBJ_CAVE";
            break;

        // загон (зимовка)
        case OBJ_LEANTO:
            string = "OBJ_LEANTO";
            break;

        // лодочник, точка входа
        case OBJ_MAGELLANMAPS:
            string = "OBJ_MAGELLANMAPS";
            break;

        // бревна на воде
        case OBJ_FLOTSAM:
            string = "OBJ_FLOTSAM";
            break;

        // разрушенный корабль, точка входа
        case OBJ_DERELICTSHIP:
            string = "OBJ_DERELICTSHIP";
            break;

        // матрос в воде
        case OBJ_SHIPWRECKSURVIROR:
            string = "OBJ_SHIPWRECKSURVIROR";
            break;

        // бутылка в воде
        case OBJ_BOTTLE:
            string = "OBJ_BOTTLE";
            break;

        // колодец
        case OBJ_MAGICWELL:
            string = "OBJ_MAGICWELL";
            break;

        // гном под грибами (+5 gems)
        case OBJ_MAGICGARDEN:
            string = "OBJ_MAGICGARDEN";
            break;

        // башня обзора, точка входа
        case OBJ_OBSERVATIONTOWER:
            string = "OBJ_OBSERVATIONTOWER";
            break;

        // кузница upgrade, точка входа
        case OBJ_FREEMANFOUNDRY:
            string = "OBJ_FREEMANFOUNDRY";
            break;

        // random артифакт 1 уровня
        case OBJ_RNDARTIFACT1:
            string = "OBJ_RNDARTIFACT1";
            break;

        // random артифакт 2 уровня
        case OBJ_RNDARTIFACT2:
            string = "OBJ_RNDARTIFACT2";
            break;

        // random артифакт 3 уровня
        case OBJ_RNDARTIFACT3:
            string = "OBJ_RNDARTIFACT3";
            break;

	default:
	    string = "OBJECT: UNKNOWN ?!";
	    break;

    }
    
    return string;
}

