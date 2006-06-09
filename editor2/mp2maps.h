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

#ifndef _MP2MAPS_H
#define _MP2MAPS_H

#include "SDL.h"

/* ************** START MP2 **************** 
*/
/* заголовок карты */
typedef struct {
Uint32  headerMagic;		// address: 0x0000
Uint16  difficultLevel;		// address: 0x0004
Uint8   widthMaps;              // address: 0x0006
Uint8	heightMaps;             // address: 0x0007
Uint8	t1[18];			// address: 0x0008 - 0x0019  ??
Uint8	countHeroes;            // address: 0x001A
Uint8	requiredHumans;		// address: 0x001B
Uint8	maxHumans;		// address: 0x001C
Uint8   conditionsWins;         // address: 0x001D
Uint8	specialWins1;		// address: 0x001E
Uint8	specialWins2;		// address: 0x001F
Uint8	specialWins3;		// address: 0x0020
Uint8	specialWins4;		// address: 0x0021
Uint8   conditionsLoss;         // address: 0x0022
Uint8	specialLoss1;		// address: 0x0023
Uint8	specialLoss2;		// address: 0x0024
Uint8	startWithHeroCastle;	// address: 0x0025
Uint8	t2[20];			// address: 0x0026 - 0x0039  ??
Uint8   longname[16];           // address: 0x003A - 0x0049
Uint8	t3[44];			// address: 0x004A - 0x0075  ??
Uint8   description[143];       // address: 0x0076 - 0x0105
Uint8	uniqName1[159];		// address: 0x0106 - 0x01A3  ??
Uint32	mapsWidth;		// address: 0x01A4
Uint32	mapsHeight;		// address: 0x01A8
} MP2HEADER;			// total:   428 байт

// далее
// mapsWidth * mapsHeight основных ячеек карты
//
/* основная информация о клетке карты */
typedef struct {
Uint16	tileIndex;		// индекс tile (ocean, grass, snow, swamp, lava, desert, dirt, wasteland, beach)
Uint8	objectName1;		// объект 1 уровня (список далее)
Uint8	indexName1;		// 0xFF или индекс спрайта
Uint8	quantity1;		// количество
Uint8	quantity2;		// количество
Uint8	objectName2;		// объект 2 уровня
Uint8	indexName2;		// 0xFF или индекс спрайта
Uint8	shape;			// развернуть спрайт tiles, shape % 4, 0 без изменений, 1 по вертикали, 2 по горизонтали, 3 оба варианта
Uint8	generalObject;		// основной тип объекта на данной клетке или 0
Uint16	indexAddon;		// индекс следующей неотрисованной части объекта иначе 0
Uint32	uniqNumber1;		// уникальный номер целого объекта первого уровня
Uint32	uniqNumber2; 	        // уникальный номер целого объекта второго уровня
} MP2TILEINFO;			// total: 20 байт

// далее
// Uint32	countData количество 15 байтных блоков MP2ADDONTAIL
//		(постоянный инкремент после сохранения, начальное состояние 0x96)

/* дополнительная информация о клетке карты */
typedef struct {
Uint16	indexAddon;		// индекс следующей неотрисованной части объекта иначе 0
Uint8	objectNameN1;		// объект следующего 1 уровня
Uint8	indexNameN1;		// 0xFF или индекс спрайта
Uint8	quantityN;		// используется для порядка отрисовки % 4
Uint8	objectNameN2;		// объект следующего 2 уровня
Uint8	indexNameN2;		// 0xFF или индекс спрайта
Uint32	uniqNumberN1;		// уникальный номер целого объекта следующего первого уровня
Uint32	uniqNumberN2;		// уникальный номер целого объекта следующего второго уровня
} MP2ADDONTAIL;

// далее
// дополнительная информация о карте
// тексты сообщений запрограммированных событий
// дополнительная инормация о замках по 48 байт

/* ************** END MP2 **************** */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// список general maps object

#define	OBJ_ZERO			0x00	// пусто
#define	OBJN_ALCHEMYTOWER		0x01	// башня алхимика, mercury
#define	OBJN_DAEMONCAVE			0x05	// пещера демона
#define OBJN_FAERIERING			0x07	// холм с мухоморами (+1 удача)
#define	OBJN_GRAVEYARD			0x0C	// кладбище

#define OBJN_DRAGONCITY			0x14	// город драконов
#define	OBJN_LIGHTHOUSE			0x15	// маяк
#define	OBJN_WATERMILL			0x16	// водяная мельница
#define	OBJN_MINES			0x17	// шахта
#define	OBJN_OBELISK			0x19	// обелиск
#define	OBJN_OASIS			0x1A	// оазис
#define	OBJ_COAST			0x1C	// побережье (граница между морем и сушей)
#define	OBJN_SAWMILL			0x1D	// лесопилка
#define OBJN_ORACLE			0x1E	// оракул

#define	OBJN_DERELICTSHIP		0x20	// разрушенный корабль (правый)
#define OBJN_DESERTTENT			0x22	// тент кочевников nomads
#define	OBJN_CASTLE			0x23	// замок
#define OBJN_WAGONCAMP			0x25	// вагоны на поляне (rogues)
#define OBJN_WINDMILL			0x28	// мельница

#define	OBJN_RNDTOWN			0x30	// random город
#define	OBJN_RNDCASTLE			0x31	// random замок
#define OBJ_SMALLCRACK			0x38	// трещина малая
#define OBJN_TREECITY			0x3C	// тройной дом спрайтов
#define OBJN_RUINS			0x3D	// medusa hunt
#define	OBJN_FORT			0x3E	// повышение навыка (+1 defence)
#define	OBJN_TRADINGPOST		0x3F	// рынок

#define OBJN_ABANDONEDMINE		0x40	// заброшенная шахта
#define OBJN_TREEKNOWLEDGE		0x44	// дерево знаний
#define OBJN_DOCTORHUNT			0x45	// хижина повышения знаний (+1 knowledge)
#define	OBJN_TEMPLE			0x46	// храм
#define OBJN_HILLFORT			0x47	// форт для upgrade orc ogre
#define OBJN_HALFLINGHOLE		0x48	// нора halfling
#define OBJN_MERCENARYCAMP		0x49	// палатка повышения атаки (+1 attack)
#define	OBJN_PIRAMID			0x4C	// пирамида
#define OBJN_CITYDEAD			0x4D	// город мертвых, руины в песке
#define OBJN_EXCAVATION			0x4E	// развалины дома, в песке
#define OBJN_SPHINX			0x4F	// сфинкс

#define OBJ_OILLAKE			0x51	// озеро нефти
#define OBJN_TROLLBRIDGE		0x53	// мост троллей
#define	OBJN_WITCHHUNT			0x55	// хижина приобретения вторичного навыка (skill)
#define OBJN_XANADU			0x56	// замок дворецкого
#define OBJN_CAVE			0x57	// пещера кентавров
#define OBJN_MAGELLANMAPS		0x59	// лодочник
#define	OBJN_SHIPWRECK			0x5B	// разрушенный корабль (левый)

#define	OBJN_OBSERVATIONTOWER		0x60	// башня обзора
#define OBJN_FREEMANFOUNDRY		0x61	// кузница upgrade
#define	OBJ_TREES			0x63	// деревья
#define	OBJ_MOUNTS			0x64	// горы
#define OBJ_STONES			0x67	// камни
#define OBJ_VEGETATION1			0x66	// цветы, растительность
#define OBJ_WATERLAKE			0x68	// озеро воды
#define OBJ_VEGETATION2			0x69	// цветы, растительность
#define OBJ_BIGCRACK			0x6C	// трещина большая

#define OBJ_LAVALAKE			0x70	// озеро лавы
#define OBJ_VEGETATION3			0x71	// цветы, растительность

#define	OBJ_ALCHEMYTOWER		0x81	// башня алхимика mercury, точка входа
#define	OBJ_BUOY			0x83	// буй на воде
#define	OBJ_SKELETON			0x84	// скелет странника в пустыне
#define	OBJ_DAEMONCAVE			0x85	// пещера демона, точка входа
#define	OBJ_TREASURECHEST		0x86	// сундук с сокровищами
#define OBJ_FAERIERING			0x87	// холм с мухоморами (+1 удача), точка входа
#define	OBJ_CAMPFIRE			0x88	// брошенный костер
#define	OBJ_FOUNTAIN			0x89	// фонтан +luck
#define	OBJ_GAZEBO			0x8A	// беседка (+1000 exp)
#define	OBJ_ANCIENTLAMP			0x8B	// лампа джина
#define	OBJ_GRAVEYARD			0x8C	// кладбище, точка входа
#define	OBJ_ARCHERHOUSE			0x8D	// хижина лучников
#define	OBJ_GOBLINHUNT			0x8E	// хижина гоблинов
#define	OBJ_DWARFCOTT			0x8F	// хижина dwarf

#define	OBJ_PEASANTHUNT			0x90	// хижина крестьянина
#define OBJ_DRAGONCITY			0x94	// город драконов, точка входа
#define	OBJ_LIGHTHOUSE			0x95	// маяк, точка входа
#define	OBJ_WATERMILL			0x96	// водяная мельница, точка входа
#define	OBJ_MINES			0x97	// шахта, точка входа
#define	OBJ_MONSTER			0x98	// монстры
#define	OBJ_OBELISK			0x99	// обелиск, точка входа
#define	OBJ_OASIS			0x9A	// оазис, точка входа
#define	OBJ_RESOURCE			0x9B	// ресурсы тип определяется по indexName1 (1: wood, 3: mercury, 5: ore, 7: sulfur, 8: crystal, B: gems, D: gold)
#define	OBJ_SAWMILL			0x9D	// лесопилка, точка входа
#define OBJ_ORACLE			0x9E	// оракул
#define	OBJ_SHRINE1			0x9F	// хижина магии 1 уровня

#define	OBJ_DERELICTSHIP		0xA0	// разрушенный корабль, точка входа
#define OBJ_DESERTTENT			0xA2	// тент кочевников nomads, точка входа
#define	OBJ_CASTLE			0xA3	// замок, точка входа
#define	OBJ_STONELITHS			0xA4	// телепорт
#define OBJ_WAGONCAMP			0xA5	// вагоны на поляне (rogues), точка входа
#define OBJ_WHIRLPOOL			0xA7	// водоворот
#define OBJ_WINDMILL			0xA8	// мельница, точка входа
#define	OBJ_ARTIFACT			0xA9	// артифакт
#define	OBJ_BOAT			0xAB	// лодка
#define	OBJ_RNDARTIFACT			0xAD	// random артифакт
#define	OBJ_RNDRESOURCE			0xAE	// random ресурс

#define	OBJ_RNDTOWN			0xB0	// random город, точка входа
#define	OBJ_RNDCASTLE			0xB1	// random замок, точка входа
#define	OBJ_RNDMONSTER1			0xB3	// radom monster level1
#define	OBJ_RNDMONSTER2			0xB4	// radom monster level2
#define	OBJ_RNDMONSTER3			0xB5	// radom monster level3
#define	OBJ_RNDMONSTER4			0xB6	// radom monster level4
#define	OBJ_WATCHTOWER			0xBA	// домик орков
#define OBJ_TREEHOUSE			0xBB	// домик спрайтов (маленький)
#define OBJ_TREECITY			0xBC	// тройной дом спрайтов, точка входа
#define OBJ_RUINS			0xBD	// medusa hunt, точка входа
#define	OBJ_FORT			0xBE	// хижина повышение навыка (+1 defence), точка входа
#define	OBJ_TRADINGPOST			0xBF	// рынок, точка входа

#define OBJ_ABANDONEDMINE		0xC0	// заброшенная шахта, точка входа
#define OBJ_STANDINGSTONES		0xC2	// стоунхедж, (+1 power)
#define OBJ_IDOL			0xC3	// идол
#define OBJ_TREEKNOWLEDGE		0xC4	// дерево знаний, точка входа
#define OBJ_DOCTORHUNT			0xC5	// хижина повышения знаний (+1 knowledge), точка входа
#define	OBJ_TEMPLE			0xC6	// храм, точка входа
#define OBJ_HILLFORT			0xC7	// форт для upgrade orc ogre, точка входа
#define OBJ_HALFLINGHOLE		0xC8	// нора halfling, точка входа
#define OBJ_MERCENARYCAMP		0xC9	// палатка повышения атаки (+1 attack), точка входа
#define	OBJ_SHRINE2			0xCA	// хижина магии 2 уровня
#define	OBJ_SHRINE3			0xCB	// хижина магии 3 уровня
#define	OBJ_PIRAMID			0xCC	// пирамида, точка входа
#define OBJ_CITYDEAD			0xCD	// город мертвых, руины в песке, точка входа
#define OBJ_EXCAVATION			0xCE	// развалины дома, в песке, точка входа
#define OBJ_SPHINX			0xCF	// сфинкс, точка входа

#define OBJ_WAGON			0xD0	// брошенная тачка
#define	OBJ_ARTESIANSPRING		0xD2	// родник
#define OBJ_TROLLBRIDGE			0xD3	// мост троллей, точка входа
#define OBJ_CRAKEDLAKE			0xD4	// озеро на cracked
#define	OBJ_WITCHHUNT			0xD5	// хижина приобретения вторичного навыка (skill), точка входа
#define OBJ_XANADU			0xD6	// замок дворецкого, точка входа
#define OBJ_CAVE			0xD7	// пещера кентавров, точка входа
#define OBJ_LEANTO			0xD8	// загон (зимовка)
#define OBJ_MAGELLANMAPS		0xD9	// лодочник, точка входа
#define	OBJ_FLOTSAM			0xDA	// бревна на воде
#define	OBJ_SHIPWRECK			0xDB	// разрушенный корабль, точка входа
#define OBJ_SHIPWRECKSURVIROR		0xDC	// матрос в воде
#define	OBJ_BOTTLE			0xDD	// бутылка в воде
#define	OBJ_MAGICWELL			0xDE	// колодец
#define	OBJ_MAGICGARDEN			0xDF	// гном под грибами (+5 gems)

#define	OBJ_OBSERVATIONTOWER		0xE0	// башня обзора, точка входа
#define OBJ_FREEMANFOUNDRY		0xE1	// кузница upgrade, точка входа

#define	OBJ_RNDARTIFACT1		0xF4	// random артифакт 1 уровня
#define	OBJ_RNDARTIFACT2		0xF5	// random артифакт 2 уровня
#define	OBJ_RNDARTIFACT3		0xF6	// random артифакт 3 уровня

// список resource
#define OBJ_RESOURCE_WOOD		0x01
#define OBJ_RESOURCE_MERCURY		0x03
#define OBJ_RESOURCE_ORE		0x05
#define OBJ_RESOURCE_SULFUR		0x07
#define OBJ_RESOURCE_CRYSTAL		0x09
#define OBJ_RESOURCE_GEMS		0x0B
#define OBJ_RESOURCE_GOLD		0x0D

// список mines
#define OBJ_MINE_ORE		0x00
#define OBJ_MINE_SULFUR		0x01
#define OBJ_MINE_CRYSTAL	0x02
#define OBJ_MINE_GEMS		0x03
#define OBJ_MINE_GOLD		0x04

// список monster

#define OBJ_MONSTER_PEASANT		0
#define OBJ_MONSTER_ARCHER		1
#define OBJ_MONSTER_RANGER		2
#define OBJ_MONSTER_PIKEMAN		3
#define OBJ_MONSTER_VETERAN_PIKEMAN	4
#define OBJ_MONSTER_SWORDSMAN		5
#define OBJ_MONSTER_MASTER_SWORDSMAN	6
#define OBJ_MONSTER_CAVALIRY		7
#define OBJ_MONSTER_CHAMPION		8
#define OBJ_MONSTER_PALADIN		9
#define OBJ_MONSTER_CRUSADER		10
#define OBJ_MONSTER_GOBLIN		11
#define OBJ_MONSTER_ORC			12
#define OBJ_MONSTER_CHIEF_ORC		13
#define OBJ_MONSTER_WOLF		14
#define OBJ_MONSTER_OGRE		15
#define OBJ_MONSTER_LORD_OGRE		16
#define OBJ_MONSTER_TROLL		17
#define OBJ_MONSTER_WAR_TROLL		18
#define OBJ_MONSTER_CYCLOPS		19
#define OBJ_MONSTER_SPRITE		20
#define OBJ_MONSTER_DWARF		21
#define OBJ_MONSTER_BATTLE_DWARF	22
#define OBJ_MONSTER_ELF			23
#define OBJ_MONSTER_GRAND_ELF		24
#define OBJ_MONSTER_DRUID		25
#define OBJ_MONSTER_GREATER_DRUID	26
#define OBJ_MONSTER_UNICORN		27
#define OBJ_MONSTER_PHOENIX		28
#define OBJ_MONSTER_CENTAUR		29
#define OBJ_MONSTER_GARGOYLE		30
#define OBJ_MONSTER_GRIFFIN		31
#define OBJ_MONSTER_MINOTAUR		32
#define OBJ_MONSTER_KNIGHT_MINOTAUR	33
#define OBJ_MONSTER_HIDRA		34
#define OBJ_MONSTER_GREEN_DRAGON	35
#define OBJ_MONSTER_RED_DRAGON		36
#define OBJ_MONSTER_BLACK_DRAGON	37
#define OBJ_MONSTER_HALFLING		38
#define OBJ_MONSTER_BOAR		39
#define OBJ_MONSTER_IRON_GOLEM		40
#define OBJ_MONSTER_STEEL_GOLEM		41
#define OBJ_MONSTER_ROC			42
#define OBJ_MONSTER_MAGE		43
#define OBJ_MONSTER_ARCHMAGE		44
#define OBJ_MONSTER_GIANT		45
#define OBJ_MONSTER_TITAN		46
#define OBJ_MONSTER_SKELETON		47
#define OBJ_MONSTER_ZOMBIE		48
#define OBJ_MONSTER_MUTANT_ZOMBIE	49
#define OBJ_MONSTER_MUMMY		50
#define OBJ_MONSTER_ROYAL_MUMMY		51
#define OBJ_MONSTER_VAMPIRE		52
#define OBJ_MONSTER_LORD_VAMPIRE	53
#define OBJ_MONSTER_LICH		54
#define OBJ_MONSTER_POWER_LICH		55
#define OBJ_MONSTER_BONE_DRAGON		56
#define OBJ_MONSTER_ROGUE		57
#define OBJ_MONSTER_NOMAD		58
#define OBJ_MONSTER_GHOST		59
#define OBJ_MONSTER_GENIE		60
#define OBJ_MONSTER_MEDUSA		61
#define OBJ_MONSTER_EARTH_ELEMENT	62
#define OBJ_MONSTER_AIR_ELEMENT		63
#define OBJ_MONSTER_FIRE_ELEMENT	64
#define OBJ_MONSTER_WATER_ELEMENT	65
#define OBJ_MONSTER_RANDOM_LEVEL1	66
#define OBJ_MONSTER_RANDOM_LEVEL2	67
#define OBJ_MONSTER_RANDOM_LEVEL3	68
#define OBJ_MONSTER_RANDOM_LEVEL4	69

// список artifact

#define OBJ_ARTIFACT_ARCANE_NECKLACE			0x11		// The Arcane Necklace of Magic increases your spell power by 4.
#define OBJ_ARTIFACT_CASTER_BRACELET			0x13		// The Caster's Bracelet of Magic increases your spell power by 2.
#define OBJ_ARTIFACT_MAGE_RING				0x15		// The Mage's Ring of Power increases your spell power by 2.
#define OBJ_ARTIFACT_WITCHES_BROACH			0x17		// The Witch's Broach of Magic increases your spell power by 3.
#define OBJ_ARTIFACT_MEDAL_VALOR			0x19		// The Medal of Valor increases your morale.
#define OBJ_ARTIFACT_MEDAL_COURAGE			0x1B		// The Medal of Courage increases your morale.
#define OBJ_ARTIFACT_MEDAL_HONOR			0x1D		// The Medal of Honor increases your morale.
#define OBJ_ARTIFACT_MEDAL_DISTINCTION			0x1F		// The Medal of Distinction increases your morale.
#define OBJ_ARTIFACT_FIZBIN_MISFORTUNE			0x21		// The Fizbin of Misfortune greatly decreases your morale.
#define OBJ_ARTIFACT_THUNDER_MACE			0x23		// The Thunder Mace of Dominion increases your attack skill by 1.
#define OBJ_ARTIFACT_ARMORED_GAUNTLETS			0x25		// The Armored Gauntlets of Protection increase your defense skill by 1.
#define OBJ_ARTIFACT_DEFENDER_HELM			0x27		// The Defender Helm of Protection increases your defense skill by 1.
#define OBJ_ARTIFACT_GIANT_FLAIL			0x29		// The Giant Flail of Dominion increases your attack skill by 1.
#define OBJ_ARTIFACT_BALLISTA				0x2B		// The Ballista of Quickness lets your catapult fire twice per combat round.
#define OBJ_ARTIFACT_STEALTH_SHIELD			0x2D		// The Stealth Shield of Protection increases your defense skill by 2.
#define OBJ_ARTIFACT_DRAGON_SWORD			0x2F		// The Dragon Sword of Dominion increases your attack skill by 3.
#define OBJ_ARTIFACT_POWER_AXE				0x31		// The Power Axe of Dominion increases your attack skill by 2.
#define OBJ_ARTIFACT_DIVINE_BREASTPLATE			0x33		// The Divine Breastplate of Protection increases your defense skill by 3.
#define OBJ_ARTIFACT_MINOR_SCROLL			0x35		// The Minor Scroll of Knowledge increases your knowledge by 2.
#define OBJ_ARTIFACT_MAJOR_SCROLL			0x37		// The Major Scroll of Knowledge increases your knowledge by 3.
#define OBJ_ARTIFACT_SUPERIOR_SCROLL			0x39		// The Superior Scroll of Knowledge increases your knowledge by 4.
#define OBJ_ARTIFACT_FOREMOST_SCROLL			0x3B		// The Foremost Scroll of Knowledge increases your knowledge by 5.
#define OBJ_ARTIFACT_ENDLESS_SACK_GOLD			0x3D		// The Endless Sack of Gold provides you with 1000 gold per day.
#define OBJ_ARTIFACT_ENDLESS_BAG_GOLD			0x3F		// The Endless Bag of Gold provides you with 750 gold per day.
#define OBJ_ARTIFACT_ENDLESS_PURSE_GOLD			0x41		// The Endless Purse of Gold provides you with 500 gold per day.
#define OBJ_ARTIFACT_NOMAD_BOOTS_MOBILITY		0x43		// The Nomad Boots of Mobility increase your movement on land.
#define OBJ_ARTIFACT_TRAVELER_BOOTS_MOBILITY		0x45		// The Traveler's Boots of Mobility increase your movement on land.
#define OBJ_ARTIFACT_RABBIT_FOOT			0x47		// The Lucky Rabbit's Foot increases your luck in combat.
#define OBJ_ARTIFACT_GOLDEN_HORSESHOE			0x49		// The Golden Horseshoe increases your luck in combat.
#define OBJ_ARTIFACT_GAMBLER_LUCKY_COIN			0x4B		// The Gambler's Lucky Coin increases your luck in combat.
#define OBJ_ARTIFACT_FOUR_LEAF_CLOVER			0x4D		// The Four_Leaf Clover increases your luck in combat.
#define OBJ_ARTIFACT_TRUE_COMPASS_MOBILITY		0x4F		// The True Compass of Mobility increases your movement on land and sea.
#define OBJ_ARTIFACT_SAILORS_ASTROLABE_MOBILITY		0x51		// The Sailors' Astrolabe of Mobility increases your movement on sea.
#define OBJ_ARTIFACT_EVIL_EYE				0x53		// The Evil Eye reduces the casting cost of curse spells by half.
#define OBJ_ARTIFACT_ENCHANTED_HOURGLASS		0x55		// The Enchanted Hourglass extends the duration of all your spells by 2 turns.
#define OBJ_ARTIFACT_GOLD_WATCH				0x57		// The Gold Watch doubles the effectiveness of your hypnotize spells.
#define OBJ_ARTIFACT_SKULLCAP				0x59		// The Skullcap halves the casting cost of all mind influencing spells.
#define OBJ_ARTIFACT_ICE_CLOAK				0x5B		// The Ice Cloak halves all damage your troops take from cold spells.
#define OBJ_ARTIFACT_FIRE_CLOAK				0x5D		// The Fire Cloak halves all damage your troops take from fire spells.
#define OBJ_ARTIFACT_LIGHTNING_HELM			0x5F		// The Lightning Helm halves all damage your troops take from lightning spells.
#define OBJ_ARTIFACT_EVERCOLD_ICICLE			0x61		// The Evercold Icicle causes your cold spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_EVERHOT_LAVA_ROCK			0x63		// The Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_LIGHTNING_ROD			0x65		// The Lightning Rod causes your lightning spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_SNAKE_RING				0x67		// The Snake Ring halves the casting cost of all your bless spells.
#define OBJ_ARTIFACT_ANKH				0x69		// The Ankh doubles the effectiveness of all your resurrect and animate spells.
#define OBJ_ARTIFACT_BOOK_ELEMENTS			0x6B		// The Book of Elements doubles the effectiveness of all your summoning spells.
#define OBJ_ARTIFACT_ELEMENTAL_RING			0x6D		// The Elemental Ring halves the casting cost of all summoning spells.
#define OBJ_ARTIFACT_HOLY_PENDANT			0x6F		// The Holy Pendant makes all your troops immune to curse spells.
#define OBJ_ARTIFACT_PENDANT_FREE_WILL			0x71		// The Pendant of Free Will makes all your troops immune to hypnotize spells.
#define OBJ_ARTIFACT_PENDANT_LIFE			0x73		// The Pendant of Life makes all your troops immune to death spells.
#define OBJ_ARTIFACT_SERENITY_PENDANT			0x75		// The Serenity Pendant makes all your troops immune to berserk spells.
#define OBJ_ARTIFACT_SEEING_EYE_PENDANT			0x77		// The Seeing_eye Pendant makes all your troops immune to blindness spells.
#define OBJ_ARTIFACT_KINETIC_PENDANT			0x79		// The Kinetic Pendant makes all your troops immune to paralyze spells.
#define OBJ_ARTIFACT_PENDANT_DEATH			0x7B		// The Pendant of Death makes all your troops immune to holy spells.
#define OBJ_ARTIFACT_WAND_NEGATION			0x7D		// The Wand of Negation protects your troops from the Dispel Magic spell.
#define OBJ_ARTIFACT_GOLDEN_BOW				0x7F		// The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)
#define OBJ_ARTIFACT_TELESCOPE				0x81		// The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.
#define OBJ_ARTIFACT_STATESMAN_QUILL			0x83		// The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.
#define OBJ_ARTIFACT_WIZARD_HAT				0x85		// The Wizard's Hat increases the duration of your spells by 10 turns!
#define OBJ_ARTIFACT_POWER_RING				0x87		// The Power Ring returns 2 extra power points/turn to your hero.
#define OBJ_ARTIFACT_AMMO_CART				0x89		// The Ammo Cart provides endless ammunition for all your troops that shoot.
#define OBJ_ARTIFACT_TAX_LIEN				0x8B		// The Tax Lien costs you 250 gold pieces/turn.
#define OBJ_ARTIFACT_HIDEOUS_MASK			0x8D		// The Hideous Mask prevents all 'wandering' armies from joining your hero.
#define OBJ_ARTIFACT_ENDLESS_POUCH_SULFUR		0x8F		// The Endless Pouch of Sulfur provides 1 unit of sulfur per day.
#define OBJ_ARTIFACT_ENDLESS_VIAL_MERCURY		0x91		// The Endless Vial of Mercury provides 1 unit of mercury per day.
#define OBJ_ARTIFACT_ENDLESS_POUCH_GEMS			0x93		// The Endless Pouch of Gems provides 1 unit of gems per day.
#define OBJ_ARTIFACT_ENDLESS_CORD_WOOD			0x95		// The Endless Cord of Wood provides 1 unit of wood per day.
#define OBJ_ARTIFACT_ENDLESS_CART_ORE			0x97		// The Endless Cart of Ore provides 1 unit of ore per day.
#define OBJ_ARTIFACT_ENDLESS_POUCH_CRYSTAL		0x99		// The Endless Pouch of Crystal provides 1 unit of crystal/day.
#define OBJ_ARTIFACT_SPIKED_HELM			0x9B		// The Spiked Helm increases your attack and defense skills by 1 each.
#define OBJ_ARTIFACT_SPIKED_SHIELD			0x9D		// The Spiked Shield increases your attack and defense skills by 2 each.
#define OBJ_ARTIFACT_WHITE_PEARL			0x9F		// The White Pearl increases your spell power and knowledge by 1 each.
#define OBJ_ARTIFACT_BLACK_PEARL			0xA1		// The Black Pearl increases your spell power and knowledge by 2 each.

/*
#define OBJ_ARTIFACT_ULTIMATE_BOOK			0		// The Ultimate Book of Knowledge increases your knowledge by 12.
#define OBJ_ARTIFACT_ULTIMATE_SWORD			1		// The Ultimate Sword of Dominion increases your attack skill by 12.
#define OBJ_ARTIFACT_ULTIMATE_CLOAK			2		// The Ultimate Cloak of Protection increases your defense skill by 12.
#define OBJ_ARTIFACT_ULTIMATE_WAND			3		// The Ultimate Wand of Magic increases your spell power by 12.
#define OBJ_ARTIFACT_ULTIMATE_SHIELD			4		// The Ultimate Shield increases your attack and defense skills by 6 each.
#define OBJ_ARTIFACT_ULTIMATE_STAFF			5		// The Ultimate Staff increases your spell power and knowledge by 6 each.
#define OBJ_ARTIFACT_ULTIMATE_CROWN			6		// The Ultimate Crown increases each of your basic skills by 4 points.
#define OBJ_ARTIFACT_GOLDEN_GOOSE			7		// The Golden Goose brings in an income of 10,000 gold per turn.
*/

/*
#define OBJ_ARTIFACT_MAGIC_BOOK				81		// The Magic Book enables you to cast spells.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_82			82		// Artifact 82.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_83			83		// Artifact 83.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_84			84		// Artifact 84.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_85			85		// Artifact 85.
#define OBJ_ARTIFACT_SPELL_SCROLL			86		// This Spell Scroll gives your hero the ability to cast the '%s' spell.
#define OBJ_ARTIFACT_ARM_MARTYR				87		// The Arm of the Martyr increases your spell power by 3 but adds the undead morale penalty.
#define OBJ_ARTIFACT_BREASTPLATE_ANDURAN		88		// The Breastplate increases your defense by 5.
#define OBJ_ARTIFACT_BROACH_SHIELDING			89		// The Broach of Shielding provides 50% protection from Armageddon and Elemental Storm, but decreases spell power by 2.
#define OBJ_ARTIFACT_BATTLE_GARB			90		// The Battle Garb of Anduran combines the powers of the three Anduran artifacts.
#define OBJ_ARTIFACT_CRYSTAL_BALL			91		// The Crystal Ball lets you get more specific information about monsters,
#define OBJ_ARTIFACT_HEART_FIRE				92		// The Heart of Fire provides 50% protection from fire, but doubles the damage taken from cold.
#define OBJ_ARTIFACT_HEART_ICE				93		// The Heart of Ice provides 50% protection from cold, but doubles the damage taken from fire.
#define OBJ_ARTIFACT_HELMET_ANDURAN			94		// The Helmet increases your spell power by 5.
#define OBJ_ARTIFACT_HOLY_HAMMER			95		// The Holy Hammer increases your attack skill by 5.
#define OBJ_ARTIFACT_LEGENDARY_SCEPTER			96		// The Legendary Scepter adds 2 points to all attributes.
#define OBJ_ARTIFACT_MASTHEAD				97		// The Masthead boosts your luck and morale by 1 each in sea combat.
#define OBJ_ARTIFACT_SPHERE_NEGATION			98		// The Sphere of Negation disables all spell casting, for both sides, in combat.
#define OBJ_ARTIFACT_STAFF_WIZARDRY			99		// The Staff of Wizardry boosts your spell power by 5.
#define OBJ_ARTIFACT_SWORD_BREAKER			100		// The Sword Breaker increases your defense by 4 and attack by 1.
#define OBJ_ARTIFACT_SWORD_ANDURAN			101		// The Sword increases your attack skill by 5.
#define OBJ_ARTIFACT_SPADE_NECROMANCY			102		// The Spade gives you increased necromancy skill.
#define OBJ_ARTIFACT_NONE				255		// 
*/

const char * PrintObjectType(Uint8, Uint8);

#endif
