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

#ifndef _CURSOR_H
#define _CURSOR_H

#include "gamedefs.h"

#define MOUSE_NULL	0x0000
#define MOUSE_LCLICK   	0x0001
#define MOUSE_RCLICK   	0x0002
#define MOUSE_UWHEEL  	0x0004
#define MOUSE_DWHEEL	0x0008
#define MOUSE_PRESENT   0x0100

#define CURSOR_WIDTH	32		// максимальная ширина курсора
#define CURSOR_HEIGHT	32		// максимальная высота курсора

#define CURSOR_NULL	0x00000000	// неизменять курсор
#define CURSOR_POINTER	0x015A0000	// стрелка
#define CURSOR_WAIT	0x015A0001	// в процессе
#define CURSOR_HEROES	0x015A0002	// на герое
#define CURSOR_CASTLE	0x015A0003	// в замок
#define CURSOR_MOVE	0x015A0004	// двигать
#define CURSOR_FIGHT	0x015A0005	// атаковать
#define CURSOR_BOAT	0x015A0006	// лодка
#define CURSOR_ANCHOR	0x015A0007	// якорь
#define CURSOR_CHANGE	0x015A0008	// обмен
#define CURSOR_ACTION	0x015A0009	// действие

#define CURSOR_MOVE2	0x015A000A	// двигать 2
#define CURSOR_FIGHT2	0x015A000B	// атаковать 2
#define CURSOR_BOAT2	0x015A000C	// лодка 2
#define CURSOR_ANCHOR2	0x015A000D	// якорь 2
#define CURSOR_CHANGE2	0x015A000E	// обмен 2
#define CURSOR_ACTION2	0x015A000F	// действие 2
#define CURSOR_MOVE3	0x015A0010	// двигать 3
#define CURSOR_FIGHT3	0x015A0011	// атаковать 3
#define CURSOR_BOAT3	0x015A0012	// лодка 3
#define CURSOR_ANCHOR3	0x015A0013	// якорь 3
#define CURSOR_CHANGE3	0x015A0014	// обмен 3
#define CURSOR_ACTION3	0x015A0015	// действие 3
#define CURSOR_MOVE4	0x015A0016	// двигать 4
#define CURSOR_FIGHT4	0x015A0017	// атаковать 4
#define CURSOR_BOAT4	0x015A0018	// лодка 4
#define CURSOR_ANCHOR4	0x015A0019	// якорь 4
#define CURSOR_CHANGE4	0x015A001A	// обмен 4
#define CURSOR_ACTION4	0x015A001B	// действие 4

#define CURSOR_REDBOAT	0x015A001C	// красная лодка
#define CURSOR_REDBOAT2	0x015A001D	// красная лодка 2
#define CURSOR_REDBOAT3	0x015A001E	// красная лодка 3
#define CURSOR_REDBOAT4	0x015A001F	// красная лодка 4

#define CURSOR_SCROLL_TOP		0x015A0020	// скролинг
#define CURSOR_SCROLL_TOPLEFT		0x015A0021	// скролинг
#define CURSOR_SCROLL_RIGHT		0x015A0022	// скролинг
#define CURSOR_SCROLL_BOTTOMLEFT	0x015A0023	// скролинг
#define CURSOR_SCROLL_BOTTOM		0x015A0024	// скролинг
#define CURSOR_SCROLL_BOTTOMRIGHT	0x015A0025	// скролинг
#define CURSOR_SCROLL_LEFT		0x015A0026	// скролинг
#define CURSOR_SCROLL_TOPRIGHT		0x015A0027	// скролинг

#define CURSOR_POINTER2		0x015A0028	// другая стрелка

#define CURSOR_WAR_NONE		0x015B0000	// бой крест
#define CURSOR_WAR_MOVE		0x015B0001	// бой идти
#define CURSOR_WAR_FLIGHT	0x015B0002	// бой лететь
#define CURSOR_WAR_ARROW	0x015B0003	// бой стрела
#define CURSOR_WAR_HELMET	0x015B0004	// бой шлем (герой)
#define CURSOR_WAR_INFO		0x015B0005	// бой инфо
#define CURSOR_WAR_POINTER	0x015B0006	// бой стрелка
#define CURSOR_SWORD_TOPLEFT	0x015B0007	// бой вверх лево
#define CURSOR_SWORD_LEFT	0x015B0008	// бой влево
#define CURSOR_SWORD_BOTTOMLEFT	0x015B0009	// бой вниз лево
#define CURSOR_SWORD_BOTTOMRIGHT 0x015B000A	// бой вниз направо
#define CURSOR_SWORD_RIGHT	0x015B000B	// бой направо
#define CURSOR_SWORD_TOPRIGHT	0x015B000C	// бой вверх направо
#define CURSOR_SWORD_TOP	0x015B000D	// бой вверх
#define CURSOR_SWORD_BOTTOM	0x015B000E	// бой вниз
#define CURSOR_WAR_BROKENARROW	0x015B000F	// бой половинный урон

#define CURSOR_SPELL_NONE	0x015C0000	// магия красный крест
#define CURSOR_SPELL_SLOW	0x015C0001	// магия замедление
#define CURSOR_SPELL_UNK1	0x015C0002	// магия ?? синий пузырек
#define CURSOR_SPELL_CURSE	0x015C0003	// магия проклятье
#define CURSOR_SPELL_LIGHT	0x015C0004	// магия молния
#define CURSOR_SPELL_MORELIGHT	0x015C0005	// магия цепь молний
#define CURSOR_SPELL_CURE	0x015C0006	// магия лечение
#define CURSOR_SPELL_BLESS	0x015C0007	// магия благословление
#define CURSOR_SPELL_FIRE	0x015C0008	// магия шар огня
#define CURSOR_SPELL_MOREFIRE	0x015C0009	// магия большой шар огня
#define CURSOR_SPELL_TELEPORT	0x015C000A	// магия телепорт
#define CURSOR_SPELL_STORM	0x015C000B	// магия шторм
#define CURSOR_SPELL_ANIMATED	0x015C000C	// магия ?? оживить нечисть
#define CURSOR_SPELL_ANIMATED2	0x015C000D	// магия ?? оживить нечисть
#define CURSOR_SPELL_HASTLE	0x015C000E	// магия ускорение
#define CURSOR_SPELL_SHIELD	0x015C000F	// магия щит
#define CURSOR_SPELL_ARMAGEDON	0x015C0010	// магия армагеддон
#define CURSOR_SPELL_ANTIMAGIC	0x015C0011	// магия антимагия
#define CURSOR_SPELL_DISPEL	0x015C0012	// магия снять заклятие
#define CURSOR_SPELL_BERSERK	0x015C0013	// магия берсерк
#define CURSOR_SPELL_PARALIZE	0x015C0014	// магия парализовать
#define CURSOR_SPELL_BLIND	0x015C0015	// магия ослепить
#define CURSOR_SPELL_HOLYWORLD	0x015C0016	// магия святое слово
#define CURSOR_SPELL_HOLYSHOUT	0x015C0017	// магия святой крик
#define CURSOR_SPELL_METEOR	0x015C0018	// магия метеорит
#define CURSOR_SPELL_UNK7	0x015C0019	// магия ?? череп
#define CURSOR_SPELL_CLONE	0x015C001A	// магия клон
#define CURSOR_SPELL_BLOODLUST	0x015C001B	// магия свиное рыло
#define CURSOR_SPELL_DEATHRIPLE	0x015C001C	// магия череп и кости светлый
#define CURSOR_SPELL_DEATHWAVE	0x015C001D	// магия череп и кости темный
#define CURSOR_SPELL_STELSKIN	0x015C001E	// магия полный шлем
#define CURSOR_SPELL_STONSKIN	0x015C001F	// магия шлем
#define CURSOR_SPELL_DRAGON	0x015C0020	// магия убить дракона
#define CURSOR_SPELL_SEISM	0x015C0021	// магия землетрясение
#define CURSOR_SPELL_DESTRUCTION 0x015C0022	// магия ослабить
#define CURSOR_SPELL_COLDRING	0x015C0023	// магия холод
#define CURSOR_SPELL_COLDRAY	0x015C0024	// магия много холода
#define CURSOR_SPELL_HIPNOTIZE	0x015C0025	// магия гипноз
#define CURSOR_SPELL_ARROW	0x015C0026	// магия стрелка


void	SetCursor(Uint32);
Uint32	GetCursor(void);
BOOL	InitCursor(void);
void	FreeCursor(void);
void	CursorOff(void);
void	CursorOn(void);

#endif
