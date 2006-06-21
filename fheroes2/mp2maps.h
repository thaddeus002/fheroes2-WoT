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
#include "agg.h"
#include "object.h"
#include "resource.h"
#include "artifact.h"
#include "heroes.h"
#include "animation.h"
#include "actionevent.h"

typedef enum {
                DESERT,
                SNOW,
                SWAMP,
                WASTELAND,
                BEACH,
                LAVA,
                DIRT,
                GRASS,
                WATER,
                ROAD

            } E_GROUND;

typedef struct {
		Uint8		ax;
		Uint8		ay;
                E_GROUND        ground;
                SDL_Surface     *tile;
                BOOL            move;
                E_OBJECT        type;
		Uint16		count;

		union {
		    E_MONSTER	monster;
		    E_RESOURCE  resource;
		    E_ARTIFACT  artifact;
		} object;

		ICNHEADER	*level1;
		ICNHEADER	*level2;
		S_ANIMATION	*animation;
                S_HEROES        *heroes;

                } S_CELLMAPS;

void		FreeMaps(void);
ACTION		InitMaps(char *);
S_CELLMAPS     *GetCELLMAPS(Uint16);
Uint8		GetWidthMaps(void);
Uint8		GetHeightMaps(void);

/* ***************************** START MP2 ********************************** */
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
// блок FF FF FF нахрен нужен непонятно
// дополнительная информация о замках по 0x48 байт
// дополнительная информация о героях по 0x4E байт

// count;
// count;
// count;

typedef struct {

    Uint8	identify;	// 0x46
    Uint8	zero;		// 0x00
    Uint8	color; 		// 00 blue, 01 green, 02 red, 03 yellow, 04 orange, 05 purpl, ff unknown
    BOOL	customBuilding;
    Uint16	building;
		    /*
			0000 0000 0000 0010	Thieve's Guild
			0000 0000 0000 0100	Tavern
			0000 0000 0000 1000	Shipyard
			0000 0000 0001 0000	Well
			0000 0000 1000 0000	Statue
			0000 0001 0000 0000	Left Turret
			0000 0010 0000 0000	Right Turret
			0000 0100 0000 0000	Marketplace
			0000 1000 0000 0000	Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
			0001 0000 0000 0000	Moat
			0010 0000 0000 0000	Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
		    */
    Uint16	dwelling;
		    /*
			0000 0000 0000 1000	dweling1
			0000 0000 0001 0000	dweling2
			0000 0000 0010 0000	dweling3
			0000 0000 0100 0000	dweling4
			0000 0000 1000 0000	dweling5
			0000 0001 0000 0000	dweling6
			0000 0010 0000 0000	upgDweling2
			0000 0100 0000 0000	upgDweling3
			0000 1000 0000 0000	upgDweling4
			0001 0000 0000 0000	upgDweling5
			0010 0000 0000 0000	upgDweling6
		    */
    Uint8	magicTower;
    BOOL	customTroops;
    Uint8	monster1;
    Uint8	monster2;
    Uint8	monster3;
    Uint8	monster4;
    Uint8	monster5;
    Uint16	countMonter1;
    Uint16	countMonter2;
    Uint16	countMonter3;
    Uint16	countMonter4;
    Uint16	countMonter5;
    BOOL	capitan;
    BOOL	customCastleName;
    char	castleName[13];		// name + '\0' // 40 byte
    Uint8	type;			// 00 knight, 01 barb, 02 sorc, 03 warl, 04 wiz, 05 necr, 06 rnd
    BOOL	castle;
    Uint8	allowCastle;		// 00 TRUE, 01 FALSE
    Uint8	xxc[29];

} MP2CASTLE;

/*
typedef struct {

    Uint8	identify;	// 0x4c
    Uint8	00;
    Uint8	00;
    BOOL	customTroops;
    Uint8	monster1;	// 0xff none
    Uint8	monster2;	// 0xff none
    Uint8	monster3;	// 0xff none
    Uint8	monster4;	// 0xff none
    Uint8	monster5;	// 0xff none
    Uint16	countMonter1;
    Uint16	countMonter2;
    Uint16	countMonter3;
    Uint16	countMonter4;
    Uint16	countMonter5;
    Uint8	customPortrate;
    Uint8	portrate;
    Uint8	artifact1;	// 0xff none
    Uint8	artifact2;	// 0xff none
    Uint8	artifact3;	// 0xff none
    Uint8	00;
    Uint32	exerience;
    BOOL	customSkill;
    Uint8	skill1;		// 0xff none
    Uint8	skill2;		// pathfinding, arcgery, logistic, scouting, 
    Uint8	skill3;		// diplomacy, navigation, leadership, wisdom,
    Uint8	skill4;		// mysticism, luck, ballistics, eagle, necromance, estate
    Uint8	skill5;
    Uint8	skill6;
    Uint8	skill7;
    Uint8	skill8;
    Uint8	skillLevel1;
    Uint8	skillLevel2;
    Uint8	skillLevel3;
    Uint8	skillLevel4;
    Uint8	skillLevel5;
    Uint8	skillLevel6;
    Uint8	skillLevel7;
    Uint8	skillLevel8;
    Uint8	00;
    Uint8	customName;
    char	name[13];	// name + '\0'
    BOOL	patrol;
    Uint8	countSquare;	// for patrol
    000000000000 total size 0x4e
} MPHEROES;
*/

// дополнительная информация о карте
// тексты сообщений запрограммированных событий

//Uint32	endCount;

/* ****************************** END MP2 ************************************** */


#endif
