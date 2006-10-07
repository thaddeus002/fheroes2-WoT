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
#include "spell.h"
#include "gamedefs.h"

#define STRLEN	128

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
            string = "Mountains";
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
        case OBJ_PEASANTHUNT2:
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
    	    fprintf(stderr, "object  : %s\n", GetStringArtifact(ptrCell->object.artifact) );
	    break;

	case OBJ_MONSTER:
    	    fprintf(stderr, "object  : %s\n", GetStringMonster(ptrCell->object.monster) );
	    break;

	case OBJN_CASTLE:
	case OBJ_CASTLE:
	    castle = GetStatCastlePos(ptrCell->ax, ptrCell->ay);
	    fprintf(stderr, "object  : ");
	    castle->building & BUILD_CASTLE ? fprintf(stderr, "Castle: ") : fprintf(stderr, "Tower: ");
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

const char * GetStringGameLevel(E_GAMELEVEL level){

    const char  *string = NULL;

    switch(level){
	case GAME_EASY:
	    string = "Easy";
	    break;
	case GAME_NORMAL:
	    string = "Normal";
	    break;
	case GAME_HARD:
	    string = "Hard";
	    break;
	case GAME_EXPERT:
	    string = "Expert";
	    break;
	case GAME_IMPOSSIBLE:
	    string = "Impossible";
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

    const char  *string = NULL;

    switch(luck){
        case LUCK_CURSED:
            string = "Cursed";
            break;

        case LUCK_AWFUL:
            string = "Awful";
            break;

        case LUCK_BAD:
            string = "Bad";
            break;

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

const char * GetStringDescriptionsLuck(E_LUCK luck){

    const char  *string = NULL;

    switch(luck){
        case LUCK_CURSED:
        case LUCK_AWFUL:
        case LUCK_BAD:
            string = "Bad luck sometimes falls on your armies in combat, causing their attacks to only do half damage.";
            break;

	case LUCK_NORMAL:
            string = "Neutral luck means your armies will never get lucky or unlucky attacks on the enemy.";
            break;

        case LUCK_GOOD:
        case LUCK_GREAT:
        case LUCK_IRISH:
            string = "Good luck sometimes lets your armies get lucky attacks (double strength) in combat.";
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

        case MORALE_BLOOD:
            string = "Blood!";
            break;
    }

    return string;
}

const char * GetStringDescriptionsMorale(E_MORALE morale){

    const char  *string = NULL;

    switch(morale){
        case MORALE_TREASON:
        case MORALE_AWFUL:
        case MORALE_POOR:
            string = "Bad morale may cause your armies to freeze in combat.";
            break;

        default:
	case MORALE_NORMAL:
            string = "Neutral morale means your armies will never be blessed with extra attacks or freeze in combat.";
            break;

        case MORALE_GOOD:
        case MORALE_GREAT:
        case MORALE_BLOOD:
            string = "Good morale may give your armies extra attacks in combat.";
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

const char * GetStringLevelMageGuild(E_MAGICLEVEL level){

    const char *string = NULL;

    switch(level){
	case MAGIC_NONE:
	    break;
	case MAGIC_LEVEL1:
	    string = "Mage Guild, Level 1";
	    break;
	case MAGIC_LEVEL2:
	    string = "Mage Guild, Level 2";
	    break;
	case MAGIC_LEVEL3:
	    string = "Mage Guild, Level 3";
	    break;
	case MAGIC_LEVEL4:
	    string = "Mage Guild, Level 4";
	    break;
	case MAGIC_LEVEL5:
	    string = "Mage Guild, Level 5";
	    break;
    }

    return string;
}

const char * GetStringBuilding(E_RACE race, E_BUILDINGCASTLE level){

    const char  *string = NULL;

    switch(level){

	case BUILD_TAVERN:
		    string = "Tavern";
		    break;
	
	case BUILD_THIEVESGUILD:
		    string = "Thieves' Guild";
		    break;

	case BUILD_WELL:
		    string = "Well";
		    break;

	case BUILD_SHIPYARD:
		    string = "Shipyard";
		    break;
    
	case BUILD_STATUE:
		    string = "Statue";
		    break;

	case BUILD_LEFTTURRET:
		    string = "Left Turret";
		    break;

	case BUILD_RIGHTTURRET:
		    string = "Right Turret";
		    break;

	case BUILD_MOAT:
		    string = "Moat";
		    break;

	case BUILD_MARKETPLACE:
		    string = "Marketplace";
		    break;

	case BUILD_CASTLE:
		    string = "Castle";
		    break;

	case BUILD_TENT:
		    string = "Tent";
		    break;

	case BUILD_CAPTAIN:
		    string = "Captain";
		    break;

	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:
		    string = "Mage Guild";
		    break;

	default:
	    string = NULL;
	    break;
    }

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
	    if(BUILD_WEL2 == level) string = "Farm";
	    if(BUILD_SPEC == level) string = "Fortifications";
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
	    if(BUILD_WEL2 == level) string = "Garbage Heap";
	    if(BUILD_SPEC == level) string = "Coliseum";
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
	    if(BUILD_WEL2 == level) string = "Crystal Garden";
	    if(BUILD_SPEC == level) string = "Rainbow";
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
	    if(BUILD_WEL2 == level) string = "Orchard";
	    if(BUILD_SPEC == level) string = "Library";
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
	    if(DWELLING_UPGRADE5 == level) string = NULL;
	    if(DWELLING_UPGRADE6 == level) string = "Red Tower";
	    if(DWELLING_UPGRADE7 == level) string = "Black Tower";
	    if(BUILD_WEL2 == level) string = "Waterfall";
	    if(BUILD_SPEC == level) string = "Dungeon";
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
	    if(BUILD_WEL2 == level) string = "Skull Pile";
	    if(BUILD_SPEC == level) string = "Storm";
	    break;

	default:
	    string = NULL;
	    break;

    }
    
    return string;
}

const char * GetStringDescriptionsBuilding(E_RACE race, E_BUILDINGCASTLE build){

    const char  *string = NULL;

    switch(build){

	case BUILD_WEL2:
	    switch(race){
    		case KNIGHT:
		    string = "The Farm increases production of Peasants by 8 per week.";
		    break;
		case BARBARIAN:
		    string = "The Garbage Heap increases production of Goblins by 8 per week.";
		    break;
		case SORCERESS:
		    string = "The Crystal Garden increases production of Sprites by 8 per week.";
		    break;
		case WARLOCK:
		    string = "The Waterfall increases production of Centaurs by 8 per week.";
		    break;
		case WIZARD:
		    string = "The Orchard increases production of Halflings by 8 per week.";
		    break;
		case NECROMANCER:
		    string = "The Skull Pile increases production of Skeletons by 8 per week.";
		    break;
		default:
		    string = "";
		    break;
	    }
	    break;
	
	case BUILD_SPEC:
	    switch(race){
		case KNIGHT:
		    string = "The Fortifications increase the toughness of the walls, increasing the number of turns it takes to knock them down.";
		    break;
		case BARBARIAN:
		    string = "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two during combat.";
		    break;
		case SORCERESS:
		    string = "The Rainbow increases the luck of the defending units by two.";
		    break;
		case WARLOCK:
		    string = "The Dungeon increases the income of the town by 500 / day.";
		    break;
		case WIZARD:
		    string = "The Library increases the number of spells in the Guild by one for each level of the guild.";
		    break;
		case NECROMANCER:
		    string = "The Storm adds +2 to the power of spells of a defending spell caster.";
		    break;
		default:
		    string = "";
		    break;
	    }
	    break;

	case BUILD_TAVERN:
	    string = "The Tavern increases morale for troops defending the castle.";
	    break;
	
	case BUILD_THIEVESGUILD:
	    string = "The Thieves' Guild provides information on enemy players. Thieves' Guilds can also provide scouting information on enemy towns.";
	    break;

	case BUILD_WELL:
	    string = "The Well increases the growth rate of all dwellings by 2 creatures per week.";
	    break;

	case BUILD_SHIPYARD:
	    string = "The Shipyard allows ships to be built.";
	    break;
    
	case BUILD_STATUE:
	    string = "The Statue increases your town's income by 250 per day.";
	    break;

	case BUILD_LEFTTURRET:
	    string = "The Left Turret provides extra firepower during castle combat.";
	    break;

	case BUILD_RIGHTTURRET:
	    string = "The Right Turret provides extra firepower during castle combat.";
	    break;

	case BUILD_MOAT:
	    string = "The Moat slows attacking units. Any unit entering the moat must end its turn there and becomes more vulnerable to attack.";
	    break;

	case BUILD_CAPTAIN:
	    string = "The Captain's Quarters provides a captain to assist in the castle's defense when no hero is present.";
	    break;

	case BUILD_MARKETPLACE:
	    string = "The Marketplace can be used to convert one type of resource into another. The more marketplaces you control, the better the exchange rate.";
	    break;

	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:
	    string = "The Mage Guild allows heroes to learn spells and replenish their spell points.";
	    break;

	case BUILD_TENT:
	    string = "The Tent provides workers to build a castle, provided the materials and the gold are available.";
	    break;

	case BUILD_CASTLE:
	    string = "The Castle improves town defense and increases income to 1000 gold per day.";
	    break;

	default:
	    string = "";
	    break;
    }
    
    return string;
}

const char * GetStringResource(E_RESOURCE resource){

    const char *string = NULL;
    
    switch(resource){
	
    case WOOD:
	string = "Wood";
	break;
	
    case MERCURY:
	string = "Mercury";
	break;

    case ORE:
	string = "Ore";
	break;

    case SULFUR:
	string = "Sulfur";
	break;

    case CRYSTAL:
	string = "Crystal";
	break;

    case GEMS:
	string = "Gems";
	break;

    case GOLDS:
	string = "Gold";
	break;

    default:
	string = "";
	break;
    }

    return string;
}


const char * GetStringDepenceDwelling(E_RACE race,  E_BUILDINGCASTLE dwelling){

    static char str[STRLEN];

    switch(dwelling){
    
	case DWELLING_MONSTER2:
	
	    switch(race){
		case KNIGHT:
		case BARBARIAN:
		case NECROMANCER:
		case WIZARD:
	        case WARLOCK:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER1));
		break;
	    
		case SORCERESS:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER1),
			GetStringBuilding(race, BUILD_TAVERN));
		break;
	
		default:
		    return NULL;
		break;
	    }
	    break;
	
	case DWELLING_UPGRADE2:

	    switch(race){
		case KNIGHT:
		case BARBARIAN:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringBuilding(race, DWELLING_MONSTER4));
		break;
	    
		case SORCERESS:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, BUILD_WELL));
		break;

		default:
		    return NULL;
		break;
	    }
	    break;
	
	case DWELLING_MONSTER3:

	    switch(race){
		case KNIGHT:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER1),
			GetStringBuilding(race, BUILD_WELL));
		break;
	    
		case BARBARIAN:
		case NECROMANCER:
		case SORCERESS:
		case WIZARD:
		case WARLOCK:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER1));
		break;
	
		default:
		break;
	    }
	    break;

	case DWELLING_UPGRADE3:

	    switch(race){
		case KNIGHT:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, DWELLING_MONSTER4) );
		break;
	    
		case SORCERESS:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER4));
		break;

		case WIZARD:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, BUILD_WELL));
		break;

		default:
		    return NULL;
		break;
	    }
	    break;

	case DWELLING_MONSTER4:

	    switch(race){
		case KNIGHT:
		    snprintf(str, STRLEN, "%s, %s, %s, %s.", 
			GetStringBuilding(race, DWELLING_MONSTER1),
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringBuilding(race, BUILD_TAVERN) );
		break;
	    
		case BARBARIAN:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER1));
		break;
	    
		case NECROMANCER:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringBuilding(race, BUILD_THIEVESGUILD) );
		break;
	    
		case SORCERESS:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, BUILD_MAGEGUILD1) );
		break;

		case WIZARD:
		case WARLOCK:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER2));
		break;
	
		default:
		    return NULL;
		break;
	    }
	    break;

	case DWELLING_UPGRADE4:

	    switch(race){
		case KNIGHT:
		case BARBARIAN:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, DWELLING_MONSTER3) );
		break;

		default:
		    return NULL;
		break;
	    }
	    break;

    
	case DWELLING_MONSTER5:

	    switch(race){
		case KNIGHT:
		case BARBARIAN:
		    snprintf(str, STRLEN, "%s, %s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringBuilding(race, DWELLING_MONSTER4) );
		break;

		case NECROMANCER:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, BUILD_MAGEGUILD1) );
		break;
	    
		case SORCERESS:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER4));
		break;

		case WIZARD:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringLevelMageGuild(MAGIC_LEVEL1) );
		break;
	
		case WARLOCK:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER3));
		break;
	
		default:
		    return NULL;
		break;
	    }
	    break;

	case DWELLING_UPGRADE5:

	    switch(race){
		case NECROMANCER:
		    snprintf(str, STRLEN, "%s.",
			GetStringLevelMageGuild(MAGIC_LEVEL2) );
		break;

		case WIZARD:
		    snprintf(str, STRLEN, "%s.",
			GetStringBuilding(race, BUILD_SPEC) );
		break;

		default:
		    return NULL;
		break;
	    }
	    break;

	case DWELLING_MONSTER6:

	    switch(race){
		case KNIGHT:
		    snprintf(str, STRLEN, "%s, %s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER2),
			GetStringBuilding(race, DWELLING_MONSTER3),
			GetStringBuilding(race, DWELLING_MONSTER4) );
		break;
	    
		case BARBARIAN:
		case NECROMANCER:
		case SORCERESS:
		    snprintf(str, STRLEN, "%s.", GetStringBuilding(race, DWELLING_MONSTER5));
		break;

		case WIZARD:
		case WARLOCK:
		    snprintf(str, STRLEN, "%s, %s.",
			GetStringBuilding(race, DWELLING_MONSTER4),
			GetStringBuilding(race, DWELLING_MONSTER5) );
		break;
	
		default:
		    return NULL;
		break;
	    }
	    break;

	default:
	    return NULL;
	    break;
    }
    
    return str;
}

const char * GetStringMonsterFromDwelling(E_RACE race, E_BUILDINGCASTLE dwelling){

    switch(race){
    
	case KNIGHT:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(PEASANT);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(ARCHER);
	    if(DWELLING_UPGRADE2 == dwelling) return GetStringMonster(RANGER);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(PIKEMAN);
	    if(DWELLING_UPGRADE3 == dwelling) return GetStringMonster(VETERAN_PIKEMAN);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(SWORDSMAN);
	    if(DWELLING_UPGRADE4 == dwelling) return GetStringMonster(MASTER_SWORDSMAN);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(CAVALIRY);
	    if(DWELLING_UPGRADE5 == dwelling) return GetStringMonster(CHAMPION);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(PALADIN);
	    if(DWELLING_UPGRADE6 == dwelling) return GetStringMonster(CRUSADER);
	    break;
	    
	case BARBARIAN:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(GOBLIN);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(ORC);
	    if(DWELLING_UPGRADE2 == dwelling) return GetStringMonster(CHIEF_ORC);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(WOLF);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(OGRE);
	    if(DWELLING_UPGRADE4 == dwelling) return GetStringMonster(LORD_OGRE);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(TROLL);
	    if(DWELLING_UPGRADE5 == dwelling) return GetStringMonster(WAR_TROLL);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(CYCLOPS);
	    break;
	    
	case SORCERESS:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(SPRITE);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(DWARF);
	    if(DWELLING_UPGRADE2 == dwelling) return GetStringMonster(BATTLE_DWARF);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(ELF);
	    if(DWELLING_UPGRADE3 == dwelling) return GetStringMonster(GRAND_ELF);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(DRUID);
	    if(DWELLING_UPGRADE4 == dwelling) return GetStringMonster(GREATER_DRUID);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(UNICORN);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(PHOENIX);
	    break;
	
	case WARLOCK:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(CENTAUR);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(GARGOYLE);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(GRIFFIN);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(MINOTAUR);
	    if(DWELLING_UPGRADE4 == dwelling) return GetStringMonster(KNIGHT_MINOTAUR);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(HIDRA);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(GREEN_DRAGON);
	    if(DWELLING_UPGRADE6 == dwelling) return GetStringMonster(RED_DRAGON);
	    if(DWELLING_UPGRADE7 == dwelling) return GetStringMonster(BLACK_DRAGON);
	    break;
	
	case WIZARD:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(HALFLING);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(BOAR);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(IRON_GOLEM);
	    if(DWELLING_UPGRADE3 == dwelling) return GetStringMonster(STEEL_GOLEM);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(ROC);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(MAGE);
	    if(DWELLING_UPGRADE5 == dwelling) return GetStringMonster(ARCHMAGE);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(GIANT);
	    if(DWELLING_UPGRADE6 == dwelling) return GetStringMonster(TITAN);
	    break;
	
	case NECROMANCER:
	    if(DWELLING_MONSTER1 == dwelling) return GetStringMonster(SKELETON);
	    if(DWELLING_MONSTER2 == dwelling) return GetStringMonster(ZOMBIE);
	    if(DWELLING_UPGRADE2 == dwelling) return GetStringMonster(MUTANT_ZOMBIE);
	    if(DWELLING_MONSTER3 == dwelling) return GetStringMonster(MUMMY);
	    if(DWELLING_UPGRADE3 == dwelling) return GetStringMonster(ROYAL_MUMMY);
	    if(DWELLING_MONSTER4 == dwelling) return GetStringMonster(VAMPIRE);
	    if(DWELLING_UPGRADE4 == dwelling) return GetStringMonster(LORD_VAMPIRE);
	    if(DWELLING_MONSTER5 == dwelling) return GetStringMonster(LICH);
	    if(DWELLING_UPGRADE5 == dwelling) return GetStringMonster(POWER_LICH);
	    if(DWELLING_MONSTER6 == dwelling) return GetStringMonster(BONE_DRAGON);
	    break;
	
	default:
	    break;
    }
    
    return NULL;
}

const char * GetStringLevelSkill(E_LEVELSKILL level){

    const char *string = NULL;

    switch(level){

	case BASIC:
	    string = "Basic";
	    break;
	
	case ADVANCED:
	    string = "Advanced";
	    break;
	
	case EXPERT:
	    string = "Expert";
	    break;
	
	default:
	    break;
    }
    
    return string;
}

const char * GetStringSkill(E_SKILL skill){

    const char *string = NULL;

    switch(skill){

	case ARCHERY:
	    string = "Archery";
	    break;

	case BALLISTICS:
	    string = "Ballistics";
	    break;

	case DIPLOMACY:
	    string = "Diplomacy";
	    break;

	case EAGLEEYE:
	    string = "Eagle Eye";
	    break;

	case ESTATES:
	    string = "Estates";
	    break;

	case LEADERSHIP:
	    string = "Leadership";
	    break;

	case LOGISTICS:
	    string = "Logistics";
	    break;

	case LUCK:
	    string = "Luck";
	    break;

	case MYSTICISM:
	    string = "Mysticism";
	    break;

	case NAVIGATION:
	    string = "Navigation";
	    break;

	case NECROMANCY:
	    string = "Necromancy";
	    break;

	case PATHFINDING:
	    string = "Pathfinding";
	    break;

	case SCOUTING:
	    string = "Scouting";
	    break;

	case WISDOM:
	    string = "Wisdom";
	    break;

	default:
	    break;
    }
    
    return string;
}

const char * GetStringDescriptionsSkill(E_SKILL skill, E_LEVELSKILL level){

    const char *string = NULL;

    switch(skill){

	case ARCHERY:
	    if(BASIC == level)		string = "Basic Archery increases the damage done by range attacking creatures by 10 percent.";
	    else if(ADVANCED == level)	string = "Advanced Archery increases the damage done by range attacking creatures by 25 percent.";
	    else if(EXPERT == level)	string = "Expert Archery increases the damage done by range attacking creatures by 50 percent.";
	    break;

	case BALLISTICS:
	    if(BASIC == level)		string = "Basic Ballistics gives your hero's catapult shots a greater chance to hit and do damage to castle walls.";
	    else if(ADVANCED == level)	string = "Advanced Ballistics gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls.";
	    else if(EXPERT == level)	string = "Expert Ballistics gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town.";
	    break;

	case DIPLOMACY:
	    if(BASIC == level)		string = "Basic Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/4 of the creatures may offer to join you.";
	    else if(ADVANCED == level)	string = "Advanced Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/2 of the creatures may offer to join you.";
	    else if(EXPERT == level)	string = "Expert Diplomacy allows you to negotiate with monsters who are weaker than your group.  All of the creatures may offer to join you.";
	    break;

	case EAGLEEYE:
	    if(BASIC == level)		string = "Basic Eagle Eye gives your hero a 20 percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat.";
	    else if(ADVANCED == level)	string = "Advanced Eagle Eye gives your hero a 30 percent chance to learn any given 3rd level spell (or below) used against him in combat.";
	    else if(EXPERT == level)	string = "Expert Eagle Eye gives your hero a 40 percent chance to learn any given 4th level spell (or below) used against him in combat.";
	    break;

	case ESTATES:
	    if(BASIC == level)		string = "Your hero produces 100 gold pieces per turn as tax revenue from estates.";
	    else if(ADVANCED == level)	string = "Your hero produces 250 gold pieces per turn as tax revenue from estates.";
	    else if(EXPERT == level)	string = "Your hero produces 500 gold pieces per turn as tax revenue from estates.";
	    break;

	case LEADERSHIP:
	    if(BASIC == level)		string = "Basic Leadership increases your hero's troops' morale by 1.";
	    else if(ADVANCED == level)	string = "Advanced Leadership increases your hero's troops' morale by 2.";
	    else if(EXPERT == level)	string = "Expert Leadership increases your hero's troops' morale by 3.";
	    break;

	case LOGISTICS:
	    if(BASIC == level)		string = "Basic Logistics increases your hero's movement points by 10 percent.";
	    else if(ADVANCED == level)	string = "Advanced Logistics increases your hero's movement points by 20 percent.";
	    else if(EXPERT == level)	string = "Expert Logistics increases your hero's movement points by 30 percent.";
	    break;

	case LUCK:
	    if(BASIC == level)		string = "Basic Luck increases your hero's luck by 1.";
	    else if(ADVANCED == level)	string = "Advanced Luck increases your hero's luck by 2.";
	    else if(EXPERT == level)	string = "Expert Luck increases your hero's luck by 3.";
	    break;

	case MYSTICISM:
	    if(BASIC == level)		string = "Basic Mysticism regenerates two of your hero's spell points per day.";
	    else if(ADVANCED == level)	string = "Advanced Mysticism regenerates three of your hero's spell points per day.";
	    else if(EXPERT == level)	string = "Expert Mysticism regenerates four of your hero's spell points per day.";
	    break;

	case NAVIGATION:
	    if(BASIC == level)		string = "Basic Navigation increases your hero's movement points over water by 1/3.";
	    else if(ADVANCED == level)	string = "Advanced Navigation increases your hero's movement points over water by 2/3.";
	    else if(EXPERT == level)	string = "Expert Navigation doubles your hero's movement points over water.";
	    break;

	case NECROMANCY:
	    if(BASIC == level)		string = "Basic Necromancy allows 10 percent of the creatures killed in combat to be brought back from the dead as Skeletons.";
	    else if(ADVANCED == level)	string = "Advanced Necromancy allows 20 percent of the creatures killed in combat to be brought back from the dead as Skeletons.";
	    else if(EXPERT == level)	string = "Expert Necromancy allows 30 percent of the creatures killed in combat to be brought back from the dead as Skeletons.";
	    break;

	case PATHFINDING:
	    if(BASIC == level)		string = "Basic Pathfinding reduces the movement penalty for rough terrain by 25 percent.";
	    else if(ADVANCED == level)	string = "Advanced Pathfinding reduces the movement penalty for rough terrain by 50 percent.";
	    else if(EXPERT == level)	string = "Expert Pathfinding eliminates the movement penalty for rough terrain.";
	    break;

	case SCOUTING:
	    if(BASIC == level)		string = "Basic Scouting increases your hero's viewable area by 1 square.";
	    else if(ADVANCED == level)	string = "Advanced Scouting increases your hero's viewable area by 2 squares.";
	    else if(EXPERT == level)	string = "Expert Scouting increases your hero's viewable area by 3 squares.";
	    break;

	case WISDOM:
	    if(BASIC == level)		string = "Basic Wisdom allows your hero to learn third level spells.";
	    else if(ADVANCED == level)	string = "Advanced Wisdom allows your hero to learn fourth level spells.";
	    else if(EXPERT == level)	string = "Expert Wisdom allows your hero to learn fifth level spells.";
	    break;

	default:
	    break;
    }
    
    return string;
}

const char * GetStringAttackSkill(void){

    const char *string = "Attack Skill";

    return string;
}

const char * GetStringDefenseSkill(void){

    const char *string = "Defense Skill";

    return string;
}

const char * GetStringSpellPower(void){

    const char *string = "Spell Power";

    return string;
}

const char * GetStringKnowledge(void){

    const char *string = "Knowledge";

    return string;
}

const char * GetStringDescriptionsAttackSkill(void){

    const char *string = "Your attack skill is a bonus added to each creature's attack skill.";

    return string;
}

const char * GetStringDescriptionsDefenseSkill(void){

    const char *string = "Your defense skill is a bonus added to each creature's defense skill.";

    return string;
}

const char * GetStringDescriptionsSpellPower(void){

    const char *string = "Your spell power determines the length or power of a spell.";

    return string;
}

const char * GetStringDescriptionsKnowledge(void){

    const char *string = "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge.";

    return string;
}

const char * GetStringFormation(E_ARMYFORMAT af){

    const char *string = NULL;
    
    switch(af){
	case SPREAD:
	    string = "'Spread'";
	    break;
	case GROUPED:
	    string = "'Grouped'";
	    break;
    }
        
    return string;
}

const char * GetStringDescriptionsFormation(E_ARMYFORMAT af){

    const char *string = NULL;
    
    switch(af){
	case SPREAD:
	    string = "'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army.";
	    break;
	case GROUPED:
	    string = "'Grouped' combat formation bunches your army together in the center of your side of the battlefield.";
	    break;
    }

    return string;
}
const char * GetStringSpell(E_SPELL spell){

    const char *string = NULL;

    switch(spell){
    case FIREBALL:string = "Fireball";break;
    case FIREBLAST:string = "Fireblast";break;
    case LIGHTNINGBOLT:string = "Lightning Bolt";break;
    case CHAINLIGHTNING:string = "Chain Lightning";break;
    case TELEPORT:string = "Teleport";break;
    case CURE:string = "Cure";break;
    case MASSCURE:string = "Mass Cure";break;
    case RESURRECT:string = "Resurrect";break;
    case RESURRECTTRUE:string = "Resurrect True";break;
    case HASTE:string = "Haste";break;
    case MASSHASTE:string = "Mass Haste";break;
    case SPELLSLOW:string = "Slow";break;
    case MASSSLOW:string = "Mass Slow";break;
    case BLIND:string = "Blind";break;
    case BLESS:string = "Bless";break;
    case MASSBLESS:string = "Mass Bless";break;
    case STONESKIN:string = "Stoneskin";break;
    case STEELSKIN:string = "Steelskin";break;
    case CURSE:string = "Curse";break;
    case MASSCURSE:string = "Mass Curse";break;
    case HOLYWORD:string = "Holy Word";break;
    case HOLYSHOUT:string = "Holy Shout";break;
    case ANTIMAGIC:string = "Anti-Magic";break;
    case DISPELMAGIC:string = "Dispel Magic";break;
    case MASSDISPEL:string = "Mass Dispel";break;
    case MAGICARROW:string = "Magic Arrow";break;
    case BERZERKER:string = "Berzerker";break;
    case ARMAGEDDON:string = "Armageddon";break;
    case ELEMENTALSTORM:string = "Elemental Storm";break;
    case METEORSHOWER:string = "Meteor Shower";break;
    case PARALYZE:string = "Paralyze";break;
    case HYPNOTIZE:string = "Hypnotize";break;
    case COLDRAY:string = "Cold Ray";break;
    case COLDRING:string = "Cold Ring";break;
    case DISRUPTINGRAY:string = "Disrupting Ray";break;
    case DEATHRIPPLE:string = "Death Ripple";break;
    case DEATHWAVE:string = "Death Wave";break;
    case DRAGONSLAYER:string = "Dragon Slayer";break;
    case BLOODLUST:string = "Blood Lust";break;
    case ANIMATEDEAD:string = "Animate Dead";break;
    case MIRRORIMAGE:string = "Mirror Image";break;
    case SHIELD:string = "Shield";break;
    case MASSSHIELD:string = "Mass Shield";break;
    case EARTHSUMMON:string = "Summon Earth El.";break;
    case AIRSUMMON:string = "Summon Air El.";break;
    case FIRESUMMON:string = "Summon Fire El.";break;
    case WATERSUMMON:string = "Summon Water El.";break;
    case EARTHQUAKE:string = "Earthquake";break;
    case VIEWMINES:string = "View Mines";break;
    case VIEWRESOURCES:string = "View Resources";break;
    case VIEWARTIFACTS:string = "View Artifacts";break;
    case VIEWTOWNS:string = "View Towns";break;
    case VIEWHEROES:string = "View Heroes";break;
    case VIEWALL:string = "View All";break;
    case IDENTIFYHERO:string = "Identify Hero";break;
    case SUMMONBOAT:string = "Summon Boat";break;
    case DIMENSIONDOOR:string = "Dimension Door";break;
    case TOWNGATE:string = "Town Gate";break;
    case TOWNPORTAL:string = "Town Portal";break;
    case VISIONS:string = "Visions";break;
    case HAUNT:string = "Haunt";break;
    case EARTHGUARDIAN:string = "Earth Guardian";break;
    case AIRGUARDIAN:string = "Air Guardian";break;
    case FIREGUARDIAN:string = "Fire Guardian";break;
    case WATERGUARDIAN:string = "Water Guardian";break;
    default:break;
    }

    return string;
}

const char * GetStringDescriptionsSpell(E_SPELL spell){

    const char *string = NULL;

    switch(spell){
    case FIREBALL:string = "Causes a giant fireball to strike the selected area, damaging all nearby creatures.";break;
    case FIREBLAST:string = "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one.";break;
    case LIGHTNINGBOLT:string = "Causes a bolt of electrical energy to strike the selected creature.";break;
    case CHAINLIGHTNING:string = "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!";break;
    case TELEPORT:string = "Teleports the creature you select to any open position on the battlefield.";break;
    case CURE:string = "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power.";break;
    case MASSCURE:string = "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature.";break;
    case RESURRECT:string = "Resurrects creatures from a damaged or dead unit until end of combat.";break;
    case RESURRECTTRUE:string = "Resurrects creatures from a damaged or dead unit permanently.";break;
    case HASTE:string = "Increases the speed of any creature by two.";break;
    case MASSHASTE:string = "Increases the speed of all of your creatures by two.";break;
    case SPELLSLOW:string = "Slows target to half movement rate.";break;
    case MASSSLOW:string = "Slows all enemies to half movement rate.";break;
    case BLIND:string = "Clouds the affected creatures' eyes, preventing them from moving.";break;
    case BLESS:string = "Causes the selected creatures to inflict maximum damage.";break;
    case MASSBLESS:string = "Causes all of your units to inflict maximum damage.";break;
    case STONESKIN:string = "Magically increases the defense skill of the selected creatures.";break;
    case STEELSKIN:string = "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin.";break;
    case CURSE:string = "Causes the selected creatures to inflict minimum damage.";break;
    case MASSCURSE:string = "Causes all enemy troops to inflict minimum damage.";break;
    case HOLYWORD:string = "Damages all undead in the battle.";break;
    case HOLYSHOUT:string = "Damages all undead in the battle.  This is an improved version of Holy Word.";break;
    case ANTIMAGIC:string = "Prevents harmful magic against the selected creatures.";break;
    case DISPELMAGIC:string = "Removes all magic spells from a single target.";break;
    case MASSDISPEL:string = "Removes all magic spells from all creatures.";break;
    case MAGICARROW:string = "Causes a magic arrow to strike the selected target.";break;
    case BERZERKER:string = "Causes a creature to attack its nearest neighbor.";break;
    case ARMAGEDDON:string = "Holy terror strikes the battlefield, causing severe damage to all creatures.";break;
    case ELEMENTALSTORM:string = "Magical elements pour down on the battlefield, damaging all creatures.";break;
    case METEORSHOWER:string = "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures.";break;
    case PARALYZE:string = "The targeted creatures are paralyzed, unable to move or retaliate.";break;
    case HYPNOTIZE:string = "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power.";break;
    case COLDRAY:string = "Drains body heat from a single enemy unit.";break;
    case COLDRING:string = "Drains body heat from all units surrounding the center point, but not including the center point.";break;
    case DISRUPTINGRAY:string = "Reduces the defense rating of an enemy unit by three.";break;
    case DEATHRIPPLE:string = "Damages all living (non-undead) units in the battle.";break;
    case DEATHWAVE:string = "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple.";break;
    case DRAGONSLAYER:string = "Greatly increases a unit's attack skill vs. Dragons.";break;
    case BLOODLUST:string = "Increases a unit's attack skill.";break;
    case ANIMATEDEAD:string = "'Resurrects' creatures from a damaged or dead undead unit permanently.";break;
    case MIRRORIMAGE:string = "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage.";break;
    case SHIELD:string = "Halves damage received from ranged attacks for a single unit.";break;
    case MASSSHIELD:string = "Halves damage received from ranged attacks for all of your units.";break;
    case EARTHSUMMON:string = "Summons Earth Elementals to fight for your army.";break;
    case AIRSUMMON:string = "Summons Air Elementals to fight for your army.";break;
    case FIRESUMMON:string = "Summons Fire Elementals to fight for your army.";break;
    case WATERSUMMON:string = "Summons Water Elementals to fight for your army.";break;
    case EARTHQUAKE:string = "Damages castle walls.";break;
    case VIEWMINES:string = "Causes all mines across the land to become visible.";break;
    case VIEWRESOURCES:string = "Causes all resources across the land to become visible.";break;
    case VIEWARTIFACTS:string = "Causes all artifacts across the land to become visible.";break;
    case VIEWTOWNS:string = "Causes all towns and castles across the land to become visible.";break;
    case VIEWHEROES:string = "Causes all Heroes across the land to become visible.";break;
    case VIEWALL:string = "Causes the entire land to become visible.";break;
    case IDENTIFYHERO:string = "Allows the caster to view detailed information on enemy Heroes.";break;
    case SUMMONBOAT:string = "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy.";break;
    case DIMENSIONDOOR:string = "Allows the caster to magically transport to a nearby location.";break;
    case TOWNGATE:string = "Returns the caster to any town or castle currently owned.";break;
    case TOWNPORTAL:string = "Returns the hero to the town or castle of choice, provided it is controlled by you.";break;
    case VISIONS:string = "Visions predicts the likely outcome of an encounter with a neutral army camp.";break;
    case HAUNT:string = "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)";break;
    case EARTHGUARDIAN:string = "Sets Earth Elementals to guard a mine against enemy armies.";break;
    case AIRGUARDIAN:string = "Sets Air Elementals to guard a mine against enemy armies.";break;
    case FIREGUARDIAN:string = "Sets Fire Elementals to guard a mine against enemy armies.";break;
    case WATERGUARDIAN:string = "Sets Water Elementals to guard a mine against enemy armies.";break;
    default:break;
    }
    
    return string;
}
