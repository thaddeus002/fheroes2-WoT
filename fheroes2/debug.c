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
#include "monster.h"
#include "castle.h"
#include "object.h"
#include "gamedefs.h"

const char * GetStringObject(E_OBJECT type){

    const char	*string;

    switch(type){

        // пусто
        case OBJ_ZERO:
            string = "OBJ_ZERO";
            break;

        // башня алхимика, mercury
        case OBJN_ALCHEMYTOWER:
        case OBJ_ALCHEMYTOWER:
            string = "Alchemy Tower";
            break;

        // пещера демона
        case OBJN_DAEMONCAVE:
        case OBJ_DAEMONCAVE:
            string = "Daemon Cave";
            break;

        // холм с мухоморами (+1 удача)
        case OBJN_FAERIERING:
        case OBJ_FAERIERING:
            string = "Faerie Ring";
            break;

        // кладбище
        case OBJN_GRAVEYARD:
        case OBJ_GRAVEYARD:
            string = "Graveyard";
            break;

        // город драконов
        case OBJN_DRAGONCITY:
        case OBJ_DRAGONCITY:
            string = "Dragon City";
            break;

        // маяк
        case OBJN_LIGHTHOUSE:
        case OBJ_LIGHTHOUSE:
            string = "Lighthouse";
            break;

        // водяная мельница
        case OBJN_WATERMILL:
        case OBJ_WATERMILL:
            string = "Watermill";
            break;

        // шахта
        case OBJN_MINES:
        case OBJ_MINES:
            string = "Mines";
            break;

        // обелиск
        case OBJN_OBELISK:
        case OBJ_OBELISK:
            string = "Obelisk";
            break;

        // оазис
        case OBJN_OASIS:
        case OBJ_OASIS:
            string = "Oasis";
            break;

        // побережье (граница между морем и сушей)
        case OBJ_COAST:
            string = "Coast";
            break;

        // лесопилка
        case OBJN_SAWMILL:
        case OBJ_SAWMILL:
            string = "Sawmill";
            break;

        // оракул
        case OBJN_ORACLE:
        case OBJ_ORACLE:
            string = "Oracle";
            break;

        // разрушенный корабль (правый)
        case OBJN_SHIPWRECK:
        case OBJ_SHIPWRECK:
            string = "Ship Wreck";
            break;

        // тент кочевников nomads
        case OBJN_DESERTTENT:
        case OBJ_DESERTTENT:
            string = "Desert Tent";
            break;

        // замок
        case OBJN_CASTLE:
        case OBJ_CASTLE:
            string = "Castle";
            break;

        // вагоны на поляне (rogues)
        case OBJN_WAGONCAMP:
        case OBJ_WAGONCAMP:
            string = "Wagon Camp";
            break;

        // мельница
        case OBJN_WINDMILL:
        case OBJ_WINDMILL:
            string = "Windmill";
            break;

        // random город
        case OBJN_RNDTOWN:
        case OBJ_RNDTOWN:
            string = "RND Town";
            break;

        // random замок
        case OBJN_RNDCASTLE:
        case OBJ_RNDCASTLE:
            string = "RND Castle";
            break;

        // трещина малая
        case OBJ_SMALLCRACK:
            string = "Small Crack";
            break;

        // тройной дом спрайтов
        case OBJN_TREECITY:
        case OBJ_TREECITY:
            string = "Tree City";
            break;

        // medusa hunt
        case OBJN_RUINS:
        case OBJ_RUINS:
            string = "Ruins";
            break;

        // повышение навыка (+1 defence)
        case OBJN_FORT:
        case OBJ_FORT:
            string = "Fort";
            break;

        // рынок
        case OBJN_TRADINGPOST:
        case OBJ_TRADINGPOST:
            string = "Trading Post";
            break;

        // заброшенная шахта
        case OBJN_ABANDONEDMINE:
        case OBJ_ABANDONEDMINE:
            string = "Abandone Mine";
            break;

        // дерево знаний
        case OBJN_TREEKNOWLEDGE:
        case OBJ_TREEKNOWLEDGE:
            string = "Tree Knowledge";
            break;

        // хижина повышения знаний (+1 knowledge)
        case OBJN_DOCTORHUNT:
        case OBJ_DOCTORHUNT:
            string = "Doctor Hunt";
            break;

        // храм
        case OBJN_TEMPLE:
        case OBJ_TEMPLE:
            string = "Temple";
            break;

        // герой
        case OBJ_HEROES:
            string = "Heroes";
            break;

        // табличка
        case OBJ_SIGN:
            string = "Sign";
            break;

        // форт для upgrade orc ogre
        case OBJN_HILLFORT:
        case OBJ_HILLFORT:
            string = "Hill Fort";
            break;

        // нора halfling
        case OBJN_HALFLINGHOLE:
        case OBJ_HALFLINGHOLE:
            string = "Halfling Hole";
            break;

        // палатка повышения атаки (+1 attack)
        case OBJN_MERCENARYCAMP:
        case OBJ_MERCENARYCAMP:
            string = "Mercenary Camp";
            break;

        // пирамида
        case OBJN_PIRAMID:
        case OBJ_PIRAMID:
            string = "Piramid";
            break;

        // город мертвых, руины в песке
        case OBJN_CITYDEAD:
        case OBJ_CITYDEAD:
            string = "City Dead";
            break;

        // развалины дома, в песке
        case OBJN_EXCAVATION:
        case OBJ_EXCAVATION:
            string = "Excavation";
            break;

        // сфинкс
        case OBJN_SPHINX:
        case OBJ_SPHINX:
            string = "Sphinx";
            break;

        // озеро нефти
        case OBJ_OILLAKE:
            string = "Oil Lake";
            break;

        // мост троллей
        case OBJN_TROLLBRIDGE:
        case OBJ_TROLLBRIDGE:
            string = "Troll Bridge";
            break;

        // хижина приобретения вторичного навыка (skill)
        case OBJN_WITCHHUNT:
        case OBJ_WITCHHUNT:
            string = "Witch's Hunt";
            break;

        // замок дворецкого
        case OBJN_XANADU:
        case OBJ_XANADU:
            string = "Xanadu";
            break;

        // пещера кентавров
        case OBJN_CAVE:
        case OBJ_CAVE:
            string = "Cave";
            break;

        // лодочник
        case OBJN_MAGELLANMAPS:
        case OBJ_MAGELLANMAPS:
            string = "Magellan Maps";
            break;

        // разрушенный корабль (левый)
        case OBJN_DERELICTSHIP:
        case OBJ_DERELICTSHIP:
            string = "Derelict Ship";
            break;

        // башня обзора
        case OBJN_OBSERVATIONTOWER:
        case OBJ_OBSERVATIONTOWER:
            string = "Observation Tower";
            break;

        // кузница upgrade
        case OBJN_FREEMANFOUNDRY:
        case OBJ_FREEMANFOUNDRY:
            string = "Freeman Foundry";
            break;

        // деревья
        case OBJ_TREES:
            string = "Trees";
            break;

        // горы
        case OBJ_MOUNTS:
            string = "Mounts";
            break;

        // вулкан
        case OBJ_VOLCANO:
            string = "Volcano";
            break;

        // камни
        case OBJ_STONES:
            string = "OBJ_STONES";
            break;

        // цветы, растительность
        case OBJ_VEGETATION1:
            string = "Vegetation";
            break;

        // озеро воды
        case OBJ_WATERLAKE:
            string = "Water Lake";
            break;

        // цветы, растительность
        case OBJ_VEGETATION2:
            string = "Vegetation";
            break;

        // трещина большая
        case OBJ_BIGCRACK:
            string = "Big Crack";
            break;

        // озеро лавы
        case OBJ_LAVALAKE:
            string = "Lava Lake";
            break;

        // цветы, растительность
        case OBJ_VEGETATION3:
            string = "Vegetation";
            break;

        // буй на воде
        case OBJ_BUOY:
            string = "Buoy";
            break;

        // скелет странника в пустыне
        case OBJ_SKELETON:
            string = "Skeleton";
            break;

        // сундук с сокровищами
        case OBJ_TREASURECHEST:
            string = "Treasure Chest";
            break;

        // брошенный костер
        case OBJ_CAMPFIRE:
            string = "Camp Fire";
            break;

        // фонтан +luck
        case OBJ_FOUNTAIN:
            string = "Fountain";
            break;

        // беседка (+1000 exp)
        case OBJ_GAZEBO:
            string = "Gazebo";
            break;

        // лампа джина
        case OBJ_ANCIENTLAMP:
            string = "Ancient Lamp";
            break;

        // хижина лучников
        case OBJ_ARCHERHOUSE:
            string = "Archer House";
            break;

        // хижина гоблинов
        case OBJ_GOBLINHUNT:
            string = "Goblin Hunt";
            break;

        // хижина dwarf
        case OBJ_DWARFCOTT:
            string = "Dwarf Cott";
            break;

        // хижина крестьянина
        case OBJ_PEASANTHUNT:
            string = "Peasant Hunt";
            break;

        // монстры
        case OBJ_MONSTER:
	    string = "Monster";
            break;

        // ресурсы
        case OBJ_RESOURCE:
	    string = "Resource";
            break;


        // телепорт
        case OBJ_STONELITHS:
            string = "Stonelights";
            break;

        // водоворот
        case OBJ_WHIRLPOOL:
            string = "Whirlpool";
            break;

        // артифакт
        case OBJ_ARTIFACT:
	    string = "Artifact";
            break;

        // лодка
        case OBJ_BOAT:
            string = "Boat";
            break;

        // random артифакт
        case OBJ_RNDARTIFACT:
            string = "RND Artifact";
            break;

        // random ресурс
        case OBJ_RNDRESOURCE:
            string = "RND Resource";
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
            string = "Watch Tower";
            break;

        // домик спрайтов (маленький)
        case OBJ_TREEHOUSE:
            string = "Tree House";
            break;

        // стоунхедж, (+1 power)
        case OBJ_STANDINGSTONES:
            string = "Standing Stones";
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
            string = "Idol";
            break;

        // хижина магии 1 уровня
        case OBJ_SHRINE1:
            string = "Shrine of Level 1";
            break;

        // хижина магии 2 уровня
        case OBJ_SHRINE2:
            string = "Shrine of Level 2";
            break;

        // хижина магии 3 уровня
        case OBJ_SHRINE3:
            string = "Shrine of Level 3";
            break;

        // брошенная тачка
        case OBJ_WAGON:
            string = "Wagon";
            break;

        // родник
        case OBJ_ARTESIANSPRING:
            string = "Artesian Spring";
            break;

        // озеро на cracked
        case OBJ_CRAKEDLAKE:
            string = "Cracked Lake";
            break;

        // загон (зимовка)
        case OBJ_LEANTO:
            string = "Leanto";
            break;

        // бревна на воде
        case OBJ_FLOTSAM:
            string = "Flotsam";
            break;

        // матрос в воде
        case OBJ_SHIPWRECKSURVIROR:
            string = "Shipwreck Surviror";
            break;

        // бутылка в воде
        case OBJ_BOTTLE:
            string = "Bottle";
            break;

        // колодец
        case OBJ_MAGICWELL:
            string = "Magic Well";
            break;

        // гном под грибами (+5 gems)
        case OBJ_MAGICGARDEN:
            string = "Magic Garden";
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
    fprintf(stderr, "ground  : %s\n", GetStringGround(ptrCell->ground));

    
    fprintf(stderr, "move    : ");
    ptrCell->move ? fprintf(stderr, "TRUE\n") : fprintf(stderr, "FALSE\n");

    fprintf(stderr, "type    : %s\n", GetStringObject(ptrCell->type));

    S_CASTLE *castle;

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

	case OBJN_CASTLE:
	case OBJ_CASTLE:
	    castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
	    fprintf(stderr, "object  : ");
	    castle->castle ? fprintf(stderr, "Castle: ") : fprintf(stderr, "Tower: ");
    	    fprintf(stderr, "%s, ", castle->name);
    	    fprintf(stderr, "Kingdom: %s, ", GetStringColor(castle->color));
    	    fprintf(stderr, "Race: %s\n", GetStringRace(castle->race));
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

const char * GetStringRace(E_RACE race){

    const char  *string;

    switch(race){
    
	case KNIGHT:
	    string = "Knight";
	    break;

	case BARBARIAN:
	    string = "Barbarian";
	    break;

	case SORCERESS:
	    string = "Sorceress";
	    break;

	case WARLOCK:
	    string = "Warlock";
	    break;

	case WIZARD:
	    string = "Wizard";
	    break;

	case NECROMANCER:
	    string = "Necromancer";
	    break;

	default:
	    string = "Bomg";
	    break;

    }
    
    return string;
}

const char * GetStringColor(E_COLORS color){

    const char  *string;

    switch(color){
    
	case GREEN:
	    string = "Green";
	    break;

	case BLUE:
	    string = "Blue";
	    break;

	case RED:
	    string = "Red";
	    break;
	case YELLOW:
	    string = "Yellow";
	    break;

	case ORANGE:
	    string = "Orange";
	    break;

	default:
	case PURPLE:
	    string = "Purple";
	    break;
    }
    
    return string;
}

const char * GetStringLuck(E_LUCK luck){

    const char  *string;

    switch(luck){
        case LUCK_AWFUL:
            string = "Awful";
            break;

        case LUCK_BAD:
            string = "Bad";
            break;

        default:
	case LUCK_NORMAL:
            string = "Normal";
            break;

        case LUCK_GOOD:
            string = "Good";
            break;

        case LUCK_GREAT:
            string = "Great";
            break;

        case LUCK_IRISH:
            string = "Irish";
            break;
    }

    return string;
}

const char * GetStringMorale(E_MORALE morale){

    const char  *string;

    switch(morale){
        case MORALE_TREASON:
            string = "Treason";
            break;

        case MORALE_AWFUL:
            string = "Awful";
            break;

        case MORALE_POOR:
            string = "Poor";
            break;

        default:
	case MORALE_NORMAL:
            string = "Normal";
            break;

        case MORALE_GOOD:
            string = "Good";
            break;

        case MORALE_GREAT:
            string = "Great";
            break;

        case MORALE_IRISH:
            string = "Irish";
            break;
    }

    return string;
}

const char * GetStringSpeed(E_MONSTERSPEED speed){

    const char  *string;

    switch(speed){

        case CRAWLING:
            string = "Crawling";
            break;

        case VERYSLOW:
            string = "Very Slow";
            break;

        case SLOW:
            string = "Slow";
            break;

        default:
	case AVERAGE:
            string = "Average";
            break;

        case FAST:
            string = "Fast";
            break;

        case VERYFAST:
            string = "Very Fast";
            break;

        case ULTRAFAST:
            string = "Ultra Fast";
            break;

        case BLAZING:
            string = "Brazing";
            break;

        case INSTANT:
            string = "Instant";
            break;
    }
    
    return string;
}

const char * GetStringGround(E_GROUND ground){

    const char *string;
    
    switch(ground){
    
	case DESERT:
	    string = "Desert";
	    break;
	
	case SNOW:
	    string = "Snow";
	    break;
	    
	case SWAMP:
	    string = "Swamp";
	    break;
	    
	case WASTELAND:
	    string = "Wasteland";
	    break;
	
	case BEACH:
	    string = "Beach";
	    break;
	    
	case LAVA:
	    string = "Lava";
	    break;
	    
	case DIRT:
	    string = "Dirt";
	    break;
	
	case GRASS:
	    string = "Grass";
	    break;
	
	case WATER:
	    string = "Ocean";
	    break;

	default:
	case ROAD:
	    string = "Road";
	    break;
    }
    
    return string;
}

const char * GetStringWel2(E_RACE race){

    const char  *string;

    switch(race){
    
	case KNIGHT:
	    string = "Farm";
	    break;

	case BARBARIAN:
	    string = "Garbage Heap";
	    break;

	case SORCERESS:
	    string = "Crystal Garden";
	    break;

	case WARLOCK:
	    string = "Waterfall";
	    break;

	case WIZARD:
	    string = "Orchard";
	    break;

	case NECROMANCER:
	    string = "Skull Pile";
	    break;

	default:
	    string = NULL;
	    break;

    }
    
    return string;
}

const char * GetStringSpec(E_RACE race){

    const char  *string;

    switch(race){
    
	case KNIGHT:
	    string = "Fortifications";
	    break;

	case BARBARIAN:
	    string = "Coliseum";
	    break;

	case SORCERESS:
	    string = "Rainbow";
	    break;

	case WARLOCK:
	    string = "Dungeon";
	    break;

	case WIZARD:
	    string = "Library";
	    break;

	case NECROMANCER:
	    string = "Storm";
	    break;

	default:
	    string = NULL;
	    break;

    }
    
    return string;
}

const char * GetStringDwelling(E_RACE race, E_DWELLINGCASTLE level){

    const char  *string = NULL;

    switch(race){
    
	case KNIGHT:
	    if(DWELLING_MONSTER1 == level) string = "Thatched Hut";
	    if(DWELLING_MONSTER2 == level) string = "Archery Range";
	    if(DWELLING_MONSTER3 == level) string = "Blacksmith";
	    if(DWELLING_MONSTER4 == level) string = "Armory";
	    if(DWELLING_MONSTER5 == level) string = "Jousting Arena";
	    if(DWELLING_MONSTER6 == level) string = "Cathedral";
	    if(DWELLING_UPGRADE2 == level) string = "Upg. Archery Range";
	    if(DWELLING_UPGRADE3 == level) string = "Upg. Blacksmith";
	    if(DWELLING_UPGRADE4 == level) string = "Upg. Armory";
	    if(DWELLING_UPGRADE5 == level) string = "Upg. Jousting Arena";
	    if(DWELLING_UPGRADE6 == level) string = "Upg. Cathedral";
	    if(DWELLING_UPGRADE7 == level) string = NULL;
	    break;

	case BARBARIAN:
	    if(DWELLING_MONSTER1 == level) string = "Hut";
	    if(DWELLING_MONSTER2 == level) string = "Stick Hut";
	    if(DWELLING_MONSTER3 == level) string = "Den";
	    if(DWELLING_MONSTER4 == level) string = "Adobe";
	    if(DWELLING_MONSTER5 == level) string = "Bridge";
	    if(DWELLING_MONSTER6 == level) string = "Pyramid";
	    if(DWELLING_UPGRADE2 == level) string = "Upg. Stick Hut";
	    if(DWELLING_UPGRADE4 == level) string = "Upg. Adobe";
	    if(DWELLING_UPGRADE3 == level) string = NULL;
	    if(DWELLING_UPGRADE5 == level) string = "Upg. Bridge";
	    if(DWELLING_UPGRADE6 == level) string = NULL;
	    if(DWELLING_UPGRADE7 == level) string = NULL;
	    break;

	case SORCERESS:
	    if(DWELLING_MONSTER1 == level) string = "Treehouse";
	    if(DWELLING_MONSTER2 == level) string = "Cottage";
	    if(DWELLING_MONSTER3 == level) string = "Archery Range";
	    if(DWELLING_MONSTER4 == level) string = "Stonehenge";
	    if(DWELLING_MONSTER5 == level) string = "Fenced Meadow";
	    if(DWELLING_MONSTER6 == level) string = "Red Tower";
	    if(DWELLING_UPGRADE2 == level) string = "Upg. Cottage";
	    if(DWELLING_UPGRADE3 == level) string = "Upg. Archery Range";
	    if(DWELLING_UPGRADE4 == level) string = "Upg. Stonehenge";
	    if(DWELLING_UPGRADE5 == level) string = NULL;
	    if(DWELLING_UPGRADE6 == level) string = NULL;
	    if(DWELLING_UPGRADE7 == level) string = NULL;
	    break;

	case WIZARD:
	    if(DWELLING_MONSTER1 == level) string = "Habitat";
	    if(DWELLING_MONSTER2 == level) string = "Pen";
	    if(DWELLING_MONSTER3 == level) string = "Foundry";
	    if(DWELLING_MONSTER4 == level) string = "Cliff Nest";
	    if(DWELLING_MONSTER5 == level) string = "Ivory Tower";
	    if(DWELLING_MONSTER6 == level) string = "Cloud Castle";
	    if(DWELLING_UPGRADE2 == level) string = NULL;
	    if(DWELLING_UPGRADE3 == level) string = "Upg. Foundry";
	    if(DWELLING_UPGRADE4 == level) string = NULL;
	    if(DWELLING_UPGRADE5 == level) string = "Upg. Ivory Tower";
	    if(DWELLING_UPGRADE6 == level) string = "Upg. Cloud Castle";
	    break;

	case WARLOCK:
	    if(DWELLING_MONSTER1 == level) string = "Cave";
	    if(DWELLING_MONSTER2 == level) string = "Crypt";
	    if(DWELLING_MONSTER3 == level) string = "Nest";
	    if(DWELLING_MONSTER4 == level) string = "Maze";
	    if(DWELLING_MONSTER5 == level) string = "Swamp";
	    if(DWELLING_MONSTER6 == level) string = "Green Tower";
	    if(DWELLING_UPGRADE2 == level) string = NULL;
	    if(DWELLING_UPGRADE3 == level) string = NULL;
	    if(DWELLING_UPGRADE4 == level) string = "Upg. Maze";
	    if(DWELLING_UPGRADE5 == level) string = "Red Tower";
	    if(DWELLING_UPGRADE6 == level) string = "Black Tower";
	    if(DWELLING_UPGRADE7 == level) string = NULL;
	    break;

	case NECROMANCER:
	    if(DWELLING_MONSTER1 == level) string = "Excavation";
	    if(DWELLING_MONSTER2 == level) string = "Graveyard";
	    if(DWELLING_MONSTER3 == level) string = "Pyramid";
	    if(DWELLING_MONSTER4 == level) string = "Mansion";
	    if(DWELLING_MONSTER5 == level) string = "Mausoleum";
	    if(DWELLING_MONSTER6 == level) string = "Laboratory";
	    if(DWELLING_UPGRADE2 == level) string = "Upg. Graveyard";
	    if(DWELLING_UPGRADE3 == level) string = "Upg. Pyramid";
	    if(DWELLING_UPGRADE4 == level) string = "Upg. Mansion";
	    if(DWELLING_UPGRADE5 == level) string = "Upg. Mausoleum";
	    if(DWELLING_UPGRADE6 == level) string = NULL;
	    if(DWELLING_UPGRADE7 == level) string = NULL;
	    break;

	default:
	    string = NULL;
	    break;

    }
    
    return string;
}

const char * GetStringMageGuild(void){

    const char *string = "Mage Guild";
    
    return string;
}

const char * GetStringLevelMageGuild(Uint8 level){

    const char *string;

    switch(level){
	case 1:
	    string = "Mage Guild, Level 1";
	    break;
	case 2:
	    string = "Mage Guild, Level 2";
	    break;
	case 3:
	    string = "Mage Guild, Level 3";
	    break;
	case 4:
	    string = "Mage Guild, Level 4";
	    break;
	case 5:
	    string = "Mage Guild, Level 5";
	    break;
	default:
	    string = NULL;
	    break;
    }

    return string;
}

const char * GetStringTavern(void){

    const char *string = "Tavern";
    
    return string;
}

const char * GetStringThievesGuild(void){

    const char *string = "Thieves Guild";
    
    return string;
}

const char * GetStringWell(void){

    const char *string = "Well";
    
    return string;
}

const char * GetStringShipyard(void){

    const char *string = "Shipyard";
    
    return string;
}

const char * GetStringStatue(void){

    const char *string = "Statue";
    
    return string;
}

const char * GetStringMarketplace(void){

    const char *string = "Marketplace";
    
    return string;
}

const char * GetStringLeftTurret(void){

    const char *string = "LeftTurret";
    
    return string;
}

const char * GetStringRightTurret(void){

    const char *string = "Right Turret";
    
    return string;
}

const char * GetStringMoat(void){

    const char *string = "Moat";
    
    return string;
}

const char * GetStringCapitan(void){

    const char *string = "Captains Quarters";
    
    return string;
}
