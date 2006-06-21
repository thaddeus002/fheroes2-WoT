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

#ifndef _OBJECT_H
#define _OBJECT_H

#include "SDL.h"
#include "agg.h"
#include "gamedefs.h"

#define OBJECTMAXCOUNT		139

typedef enum {
		OBJ_ZERO		= 0x00,	// пусто
		OBJN_ALCHEMYTOWER	= 0x01,	// башня алхимика, mercury
		OBJN_DAEMONCAVE		= 0x05,	// пещера демона
		OBJN_FAERIERING		= 0x07,	// холм с мухоморами (+1 удача)
		OBJN_GRAVEYARD		= 0x0C,	// кладбище

		OBJN_DRAGONCITY		= 0x14,	// город драконов
		OBJN_LIGHTHOUSE		= 0x15,	// маяк
		OBJN_WATERMILL		= 0x16,	// водяная мельница
		OBJN_MINES		= 0x17,	// шахта
		OBJN_OBELISK		= 0x19,	// обелиск
		OBJN_OASIS		= 0x1A,	// оазис
		OBJ_COAST		= 0x1C,	// побережье (граница между морем и сушей)
		OBJN_SAWMILL		= 0x1D,	// лесопилка
		OBJN_ORACLE		= 0x1E,	// оракул

		OBJN_DERELICTSHIP	= 0x20,	// разрушенный корабль (правый)
		OBJN_DESERTTENT		= 0x22,	// тент кочевников nomads
		OBJN_CASTLE		= 0x23,	// замок
		OBJN_WAGONCAMP		= 0x25,	// вагоны на поляне (rogues)
		OBJN_WINDMILL		= 0x28,	// мельница

		OBJN_RNDTOWN		= 0x30,	// random город
		OBJN_RNDCASTLE		= 0x31,	// random замок
		OBJ_SMALLCRACK		= 0x38,	// трещина малая
		OBJN_TREECITY		= 0x3C,	// тройной дом спрайтов
		OBJN_RUINS		= 0x3D,	// medusa hunt
		OBJN_FORT		= 0x3E,	// повышение навыка (+1 defence)
		OBJN_TRADINGPOST	= 0x3F,	// рынок

		OBJN_ABANDONEDMINE	= 0x40,	// заброшенная шахта
		OBJN_TREEKNOWLEDGE	= 0x44,	// дерево знаний
		OBJN_DOCTORHUNT		= 0x45,	// хижина повышения знаний (+1 knowledge)
		OBJN_TEMPLE		= 0x46,	// храм
		OBJN_HILLFORT		= 0x47,	// форт для upgrade orc ogre
		OBJN_HALFLINGHOLE	= 0x48,	// нора halfling
		OBJN_MERCENARYCAMP	= 0x49,	// палатка повышения атаки (+1 attack)
		OBJN_PIRAMID		= 0x4C,	// пирамида
		OBJN_CITYDEAD		= 0x4D,	// город мертвых, руины в песке
		OBJN_EXCAVATION		= 0x4E,	// развалины дома, в песке
		OBJN_SPHINX		= 0x4F,	// сфинкс

		OBJ_OILLAKE		= 0x51,	// озеро нефти
		OBJN_TROLLBRIDGE	= 0x53,	// мост троллей
		OBJN_WITCHHUNT		= 0x55,	// хижина приобретения вторичного навыка (skill)
		OBJN_XANADU		= 0x56,	// замок дворецкого
		OBJN_CAVE		= 0x57,	// пещера кентавров
		OBJN_MAGELLANMAPS	= 0x59,	// лодочник
		OBJN_SHIPWRECK		= 0x5B,	// разрушенный корабль (левый)

		OBJN_OBSERVATIONTOWER	= 0x60,	// башня обзора
		OBJN_FREEMANFOUNDRY	= 0x61,	// кузница upgrade
		OBJ_TREES		= 0x63,	// деревья
		OBJ_MOUNTS		= 0x64,	// горы
		OBJ_STONES		= 0x67,	// камни
		OBJ_VEGETATION1		= 0x66,	// цветы, растительность
		OBJ_WATERLAKE		= 0x68,	// озеро воды
		OBJ_VEGETATION2		= 0x69,	// цветы, растительность
		OBJ_BIGCRACK		= 0x6C,	// трещина большая

		OBJ_LAVALAKE		= 0x70,	// озеро лавы
		OBJ_VEGETATION3		= 0x71,	// цветы, растительность

		OBJ_ALCHEMYTOWER	= 0x81,	// башня алхимика mercury, точка входа
		OBJ_BUOY		= 0x83,	// буй на воде
		OBJ_SKELETON		= 0x84,	// скелет странника в пустыне
		OBJ_DAEMONCAVE		= 0x85,	// пещера демона, точка входа
		OBJ_TREASURECHEST	= 0x86,	// сундук с сокровищами
		OBJ_FAERIERING		= 0x87,	// холм с мухоморами (+1 удача), точка входа
		OBJ_CAMPFIRE		= 0x88,	// брошенный костер
		OBJ_FOUNTAIN		= 0x89,	// фонтан +luck
		OBJ_GAZEBO		= 0x8A,	// беседка (+1000 exp)
		OBJ_ANCIENTLAMP		= 0x8B,	// лампа джина
		OBJ_GRAVEYARD		= 0x8C,	// кладбище, точка входа
		OBJ_ARCHERHOUSE		= 0x8D,	// хижина лучников
		OBJ_GOBLINHUNT		= 0x8E,	// хижина гоблинов
		OBJ_DWARFCOTT		= 0x8F,	// хижина dwarf

		OBJ_PEASANTHUNT		= 0x90,	// хижина крестьянина
		OBJ_EVENT		= 0x93,	// event, программированое событие (раздариваие ресурсов и т.п.)
		OBJ_DRAGONCITY		= 0x94,	// город драконов, точка входа
		OBJ_LIGHTHOUSE		= 0x95,	// маяк, точка входа
		OBJ_WATERMILL		= 0x96,	// водяная мельница, точка входа
		OBJ_MINES		= 0x97,	// шахта, точка входа
		OBJ_MONSTER		= 0x98,	// монстры
		OBJ_OBELISK		= 0x99,	// обелиск, точка входа
		OBJ_OASIS		= 0x9A,	// оазис, точка входа
		OBJ_RESOURCE		= 0x9B,	// ресурсы тип определяется по indexName1 (1: wood, 3: mercury, 5: ore, 7: sulfur, 8: crystal, B: gems, D: gold)
		OBJ_SAWMILL		= 0x9D,	// лесопилка, точка входа
		OBJ_ORACLE		= 0x9E,	// оракул
		OBJ_SHRINE1		= 0x9F,	// хижина магии 1 уровня

		OBJ_DERELICTSHIP	= 0xA0,	// разрушенный корабль, точка входа
		OBJ_DESERTTENT		= 0xA2,	// тент кочевников nomads, точка входа
		OBJ_CASTLE		= 0xA3,	// замок, точка входа
		OBJ_STONELITHS		= 0xA4,	// телепорт
		OBJ_WAGONCAMP		= 0xA5,	// вагоны на поляне (rogues), точка входа
		OBJ_WHIRLPOOL		= 0xA7,	// водоворот
		OBJ_WINDMILL		= 0xA8,	// мельница, точка входа
		OBJ_ARTIFACT		= 0xA9,	// артифакт
		OBJ_BOAT		= 0xAB,	// лодка
		OBJ_RNDULTIMATEARTIFACT	= 0xAC,	// random ultimate artifact
		OBJ_RNDARTIFACT		= 0xAD,	// random артифакт
		OBJ_RNDRESOURCE		= 0xAE,	// random ресурс
		OBJ_RNDMONSTER		= 0xAF,	// radom monster

		OBJ_RNDTOWN		= 0xB0,	// random город, точка входа
		OBJ_RNDCASTLE		= 0xB1,	// random замок, точка входа
		OBJ_RNDMONSTER1		= 0xB3,	// radom monster level1
		OBJ_RNDMONSTER2		= 0xB4,	// radom monster level2
		OBJ_RNDMONSTER3		= 0xB5,	// radom monster level3
		OBJ_RNDMONSTER4		= 0xB6,	// radom monster level4
		OBJ_WATCHTOWER		= 0xBA,	// домик орков
		OBJ_TREEHOUSE		= 0xBB,	// домик спрайтов (маленький)
		OBJ_TREECITY		= 0xBC,	// тройной дом спрайтов, точка входа
		OBJ_RUINS		= 0xBD,	// medusa hunt, точка входа
		OBJ_FORT		= 0xBE,	// хижина повышение навыка (+1 defence), точка входа
		OBJ_TRADINGPOST		= 0xBF,	// рынок, точка входа

		OBJ_ABANDONEDMINE	= 0xC0,	// заброшенная шахта, точка входа
		OBJ_STANDINGSTONES	= 0xC2,	// стоунхедж, (+1 power)
		OBJ_IDOL		= 0xC3,	// идол
		OBJ_TREEKNOWLEDGE	= 0xC4,	// дерево знаний, точка входа
		OBJ_DOCTORHUNT		= 0xC5,	// хижина повышения знаний (+1 knowledge), точка входа
		OBJ_TEMPLE		= 0xC6,	// храм, точка входа
		OBJ_HILLFORT		= 0xC7,	// форт для upgrade orc ogre, точка входа
		OBJ_HALFLINGHOLE	= 0xC8,	// нора halfling, точка входа
		OBJ_MERCENARYCAMP	= 0xC9,	// палатка повышения атаки (+1 attack), точка входа
		OBJ_SHRINE2		= 0xCA,	// хижина магии 2 уровня
		OBJ_SHRINE3		= 0xCB,	// хижина магии 3 уровня
		OBJ_PIRAMID		= 0xCC,	// пирамида, точка входа
		OBJ_CITYDEAD		= 0xCD,	// город мертвых, руины в песке, точка входа
		OBJ_EXCAVATION		= 0xCE,	// развалины дома, в песке, точка входа
		OBJ_SPHINX		= 0xCF,	// сфинкс, точка входа

		OBJ_WAGON		= 0xD0,	// брошенная тачка
		OBJ_ARTESIANSPRING	= 0xD2,	// родник
		OBJ_TROLLBRIDGE		= 0xD3,	// мост троллей, точка входа
		OBJ_CRAKEDLAKE		= 0xD4,	// озеро на cracked
		OBJ_WITCHHUNT		= 0xD5,	// хижина приобретения вторичного навыка (skill), точка входа
		OBJ_XANADU		= 0xD6,	// замок дворецкого, точка входа
		OBJ_CAVE		= 0xD7,	// пещера кентавров, точка входа
		OBJ_LEANTO		= 0xD8,	// загон (зимовка)
		OBJ_MAGELLANMAPS	= 0xD9,	// лодочник, точка входа
		OBJ_FLOTSAM		= 0xDA,	// бревна на воде
		OBJ_SHIPWRECK		= 0xDB,	// разрушенный корабль, точка входа
		OBJ_SHIPWRECKSURVIROR	= 0xDC,	// матрос в воде
		OBJ_BOTTLE		= 0xDD,	// бутылка в воде
		OBJ_MAGICWELL		= 0xDE,	// колодец
		OBJ_MAGICGARDEN		= 0xDF,	// гном под грибами (+5 gems)

		OBJ_OBSERVATIONTOWER	= 0xE0,	// башня обзора, точка входа
		OBJ_FREEMANFOUNDRY	= 0xE1,	// кузница upgrade, точка входа

		OBJ_RNDARTIFACT1	= 0xF4,	// random артифакт 1 уровня
		OBJ_RNDARTIFACT2	= 0xF5,	// random артифакт 2 уровня
		OBJ_RNDARTIFACT3	= 0xF6,	// random артифакт 3 уровня
		
		OBJ_END			= 0xFF
	    } E_OBJECT;

E_OBJECT	CheckValidObject(Uint8);
ICNHEADER	*GetICNHEADERCellObject(Uint8, Uint8);
BOOL		StoreAnimationFrame(Uint8, Uint8, void *);

#endif
