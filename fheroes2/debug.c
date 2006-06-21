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

#include "debug.h"
#include "agg.h"
#include "mp2maps.h"
#include "config.h"
#include "object.h"
#include "gamedefs.h"

const char * PrintObjectType(E_OBJECT type, Uint8 ext){

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
	    string = "OBJ_MONSTER";
            break;

        // обелиск, точка входа
        case OBJ_OBELISK:
            string = "OBJ_OBELISK";
            break;

        // оазис, точка входа
        case OBJ_OASIS:
            string = "OBJ_OASIS";
            break;

        // ресурсы
        case OBJ_RESOURCE:
	    string = "OBJ_RESOURCE";
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
	    string = "OBJ_ARTIFACT";
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

        // event
        case OBJ_EVENT:
            string = "OBJ_EVENT";
            break;

        //random monster
        case OBJ_RNDMONSTER:
            string = "OBJ_RNDMONSTER";
            break;

        //random ultimate artifact
        case OBJ_RNDULTIMATEARTIFACT:
            string = "OBJ_RNDULTIMATEARTIFACT";
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
	    if(GetIntValue(DEBUG))
		fprintf(stderr, "OBJECT: UNKNOWN: 0x%hhX\n", type);
	    string = "";
	    break;
    }
    
    return string;
}

void PrintCellInfo(Uint16 index){

    if(! GetIntValue(DEBUG)) return;

    S_CELLMAPS *ptrCell = GetCELLMAPS(index);

    fprintf(stderr, "---------------------\n");
    fprintf(stderr, "ax      : %d\n", ptrCell->ax);
    fprintf(stderr, "ay      : %d\n", ptrCell->ay);
    fprintf(stderr, "ground  : ");

    switch(ptrCell->ground){

	case DESERT:
	    fprintf(stderr, "DESERT\n");
	    break;

	case SNOW:
	    fprintf(stderr, "SNOW\n");
	    break;

	case SWAMP:
	    fprintf(stderr, "SWAMP\n");
	    break;

	case WASTELAND:
	    fprintf(stderr, "WASTELAND\n");
	    break;

	case BEACH:
	    fprintf(stderr, "BEACH\n");
	    break;

	case LAVA:
	    fprintf(stderr, "LAVA\n");
	    break;

	case DIRT:
	    fprintf(stderr, "DIRT\n");
	    break;

	case GRASS:
	    fprintf(stderr, "GRASS\n");
	    break;

	case WATER:
	    fprintf(stderr, "WATER\n");
	    break;

	case ROAD:
	    fprintf(stderr, "ROAD\n");
	    break;

	default:
	    fprintf(stderr, "\n");
	    break;
    }

    fprintf(stderr, "move    : ");
	if(ptrCell->move)
	    fprintf(stderr, "TRUE\n");
	else
	    fprintf(stderr, "FALSE\n");

    fprintf(stderr, "type    : %s\n", PrintObjectType(ptrCell->type, 0));

    switch(ptrCell->type){

	case OBJ_RESOURCE:
    	    fprintf(stderr, "object  : %s\n", GetDescriptionsResource(ptrCell->object.resource));
	    break;

	case OBJ_ARTIFACT:
    	    fprintf(stderr, "object  : %s\n", ((S_ARTIFACT *) GetStatArtifact(ptrCell->object.artifact))->name );
	    break;

	case OBJ_MONSTER:
    	    fprintf(stderr, "object  : %s\n", ((S_MONSTER *) GetStatMonster(ptrCell->object.monster))->descriptions );
	    break;

	default:
	    break;
    }

    fprintf(stderr, "count   : %d\n", ptrCell->count);

    ICNHEADER *level = NULL;
    Uint8 count = 0;
    level = ptrCell->level1;
    while(level){
	++count;
	level = level->next;
    }
    fprintf(stderr, "level1  : %d\n", count);

    count = 0;
    level = ptrCell->level2;
    while(level){
	++count;
	level = level->next;
    }
    fprintf(stderr, "level2  : %d\n", count);


    //fprintf(stderr, "heroes  : ");

    fprintf(stderr, "\n");
}
