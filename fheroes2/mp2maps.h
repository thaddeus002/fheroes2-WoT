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
#include "castle.h"
#include "heroes.h"
#include "animation.h"
#include "actionevent.h"

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
		ICNHEADER	*upgrade;
		S_ANIMATION	*animation;
		S_ANIMATION	*monster;

                } S_CELLMAPS;

void		FreeMaps(void);
ACTION		InitMaps(char *);
S_CELLMAPS     *GetCELLMAPS(Uint16);
Uint8		GetWidthMaps(void);
Uint8		GetHeightMaps(void);
E_GROUND	GetGroundMaps(Uint8, Uint8);
void		MapsUpgradeTown2Castle(Uint8, Uint8);

/* ***************************** START MP2 ********************************** */
/* заголовок карты */
typedef struct {
Uint32  headerMagic;		// заголовок
Uint16  difficultLevel;		// уровень сложности
Uint8   widthMaps;              // размерность карты
Uint8	heightMaps;             // размерность карты
Uint8	kingdomBlue;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	kingdomGreen;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	kingdomRed;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	kingdomYellow;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	kingdomOrange;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	kingdomPurple;		// цвет королевства (00 - нет, 01 - участвует в игре)
Uint8	allowBlue;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	allowGreen;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	allowRed;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	allowYellow;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	allowOrange;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	allowPurple;		// выбор цвета игроком (00 - заблокировано, 01 - разрешено)
Uint8	rndBlue;		// случайная раса королевства (00 - нет, 01 - да)
Uint8	rndGreen;		// случайная раса королевства (00 - нет, 01 - да)
Uint8	rndRed;			// случайная раса королевства (00 - нет, 01 - да)
Uint8	rndYellow;		// случайная раса королевства (00 - нет, 01 - да)
Uint8	rndOrange;		// случайная раса королевства (00 - нет, 01 - да)
Uint8	rndPurple;		// случайная раса королевства (00 - нет, 01 - да)
Uint8	countHeroes;            // общее количество участников
Uint8	requiredHumans;		// из них, количество участников людей
Uint8	maxHumans;		// максимальное количество участников людей
Uint8   conditionsWins;         // 
Uint8	specialWins1;		// 
Uint8	specialWins2;		// 
Uint8	specialWins3;		// 
Uint8	specialWins4;		// 
Uint8   conditionsLoss;         // 
Uint8	specialLoss1;		// 
Uint8	specialLoss2;		// 
Uint8	startWithHeroCastle;	// Начинать игру с героем возле замка (00 - да, 01 - нет)
Uint8	t2[20];			// неопределенный блок 20 байт
Uint8   longname[16];           // полное имя сценария
Uint8	t3[44];			// неопределенный блок 44 байт
Uint8   description[143];       // описание сценария
Uint8	uniqName1[159];		// неопределенный блок 159 байт
Uint32	mapsWidth;		// размерность карты
Uint32	mapsHeight;		// размерность карты
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
// 216 байт
// массив из 3 байтовых элементов в количестве 72, координаты замков и идентификатор
// где идентификатор:
// 0x80 castle kni, 0x00 tower kni
// 0x81 castle bar, 0x01 tower bar
// 0x82 castle sor, 0x02 tower sor
// 0x83 castle war, 0x03 tower war
// 0x84 castle wiz, 0x04 tower wiz
// 0x85 castle nec, 0x05 tower nec
// 0x86 castle rnd, 0x06 tower rnd
//
// далее
// 432 байт
// массив из 3 байтовых элементов в количестве 144, координаты ресурсов и идентификатор
// где идентификатор:
// 0x00 mines woo
// 0x01 mines mer
// 0x02 mines ore
// 0x03 mines sul
// 0x04 mines cry
// 0x05 mines gem
// 0x06 mines gol
// 0x64 маяк
// 0x65 дом драконов
// 0x67 mines ghost
//
// далее
// несколько динамических байт
// порядок:
// Uint8, несколько Uint16 счетчики?, в конце 0x0000
//
// далее по блокам
// порядок определяется Uint16 байтами quantity1 quantity2 объекта, кратным 0x08. (начинается с 0x08, следующий 0x10, 0x18, 0x20 и.т.д):
//  - дополнительная информация о замках по 0x48 байт
//  - дополнительная информация о героях по 0x4E байт
//  - дополнительная информация о табличках OBJ_SIGN
//  - дополнительная информация о событиях  OBJ_EVENT по клетке
//  - дополнительная информация о слухах в таверне
//  - дополнительная информация о событиях по дню
//

typedef struct {

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
    Uint16	count1;
    Uint16	count2;
    Uint16	count3;
    Uint16	count4;
    Uint16	count5;
    BOOL	capitan;
    BOOL	customCastleName;
    char	castleName[13];		// name + '\0' // 39 byte
    Uint8	type;			// 00 knight, 01 barb, 02 sorc, 03 warl, 04 wiz, 05 necr, 06 rnd
    BOOL	castle;
    Uint8	allowCastle;		// 00 TRUE, 01 FALSE
    Uint8	unknown[29];

} MP2CASTLE;


typedef struct {

    Uint8	identify;	// 0x4c
    Uint8	xx1;		// 0
    Uint8	xx2;		// 0
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
    Uint8	xx3;		// 0
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
    Uint8	xx4;		// 0
    Uint8	customName;
    char	name[13];	// name + '\0'
    BOOL	patrol;
    Uint8	countSquare;	// for patrol
    Uint8	xx5[15];	// 0
} MP2HEROES;

// структура sign
// Uint16 количество следующих байт
// 0x01
// 0x08 байт по 0x00
// текст
// завершающий \0

// структура event по клетке
// Uint16 количество следующих байт
// 0x01
// Uint32 WOOD
// Uint32 MERCURY
// Uint32 ORE
// Uint32 SULFUR
// Uint32 CRYSTAL
// Uint32 GEMS
// Uint32 GOLDS
// Uin16  artifact
// Uint8  allow computer
// 10 байт 0x00
// Uint8 blue   ? кто имеет право получать event
// Uint8 green  ?
// Uint8 red    !
// Uint8 yellow ?
// Uint8 orange !
// Uint8 purple ?
// текст
// завершающий \0

// структура rumor
// Uint16 количество следующих байт
// 8 байт по 0x00
// текст
// завершающий \0

// структура event по дню
// Uint16 количество следующих байт
// 0x00
// Uint32 WOOD
// Uint32 MERCURY
// Uint32 ORE
// Uint32 SULFUR
// Uint32 CRYSTAL
// Uint32 GEMS
// Uint32 GOLDS
// Uin16  artifact         0xffff
// Uint8  allow computer
// Uint8  first visit      0x00
// Uint16 day of first
// Uint8 Every day
// 6 байт по 0x00
// 0x01
// Uint8 blue   ? кто имеет право получать event
// Uint8 green  ?
// Uint8 red    !
// Uint8 yellow ?
// Uint8 orange !
// Uint8 purple ?
// текст
// завершающий \0

//
// в самом конце 4 байтовый счетчик, версионность карты?
//Uint32	endCount

/* ****************************** END MP2 ************************************** */

#endif
