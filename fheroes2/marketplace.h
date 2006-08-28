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

#ifndef _MARKETPLACE_H
#define _MARKETPLACE_H

// обмен дешевый на дорогой
#define COSTLY_UNCOSTLY1	1/5
#define COSTLY_UNCOSTLY2	1/4
#define COSTLY_UNCOSTLY3	1/3
#define COSTLY_UNCOSTLY4	1/2
#define COSTLY_UNCOSTLY5	1/2
#define COSTLY_UNCOSTLY6	1/2
#define COSTLY_UNCOSTLY7	1/2
#define COSTLY_UNCOSTLY8	1/2
#define COSTLY_UNCOSTLY9	1/1

// обмен дорогой на дешевый
#define UNCOSTLY_COSTLY1	1/20
#define UNCOSTLY_COSTLY2	1/14
#define UNCOSTLY_COSTLY3	1/10
#define UNCOSTLY_COSTLY4	1/8
#define UNCOSTLY_COSTLY5	1/7
#define UNCOSTLY_COSTLY6	1/6
#define UNCOSTLY_COSTLY7	1/5
#define UNCOSTLY_COSTLY8	1/5
#define UNCOSTLY_COSTLY9	1/4

// обмен равнозначный
#define COSTLY_COSTLY1		10
#define COSTLY_COSTLY2		7
#define COSTLY_COSTLY3		5
#define COSTLY_COSTLY4		4
#define COSTLY_COSTLY5		4
#define COSTLY_COSTLY6		3
#define COSTLY_COSTLY7		3
#define COSTLY_COSTLY8		3
#define COSTLY_COSTLY9		2

// продажа дешевого
#define SALE_UNCOSTLY1		25
#define SALE_UNCOSTLY2		37
#define SALE_UNCOSTLY3		50
#define SALE_UNCOSTLY4		62
#define SALE_UNCOSTLY5		74
#define SALE_UNCOSTLY6		87
#define SALE_UNCOSTLY7		100
#define SALE_UNCOSTLY8		112
#define SALE_UNCOSTLY9		124

// продажа дорогого
#define SALE_COSTLY1		50
#define SALE_COSTLY2		74
#define SALE_COSTLY3		100
#define SALE_COSTLY4		124
#define SALE_COSTLY5		149
#define SALE_COSTLY6		175
#define SALE_COSTLY7		200
#define SALE_COSTLY8		224
#define SALE_COSTLY9		249

// покупка дешевого
#define BUY_UNCOSTLY1		2500
#define BUY_UNCOSTLY2		1667
#define BUY_UNCOSTLY3		1250
#define BUY_UNCOSTLY4		1000
#define BUY_UNCOSTLY5		834
#define BUY_UNCOSTLY6		715
#define BUY_UNCOSTLY7		625
#define BUY_UNCOSTLY8		556
#define BUY_UNCOSTLY9		500

// покупка дорогого
#define BUY_COSTLY1		5000
#define BUY_COSTLY2		3334
#define BUY_COSTLY3		2500
#define BUY_COSTLY4		2000
#define BUY_COSTLY5		1667
#define BUY_COSTLY6		1429
#define BUY_COSTLY7		1250
#define BUY_COSTLY8		1112
#define BUY_COSTLY9		1000

ACTION ShowMarketplace(E_COLORS);

#endif
