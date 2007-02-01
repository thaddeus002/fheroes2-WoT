/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef H2MP2_H
#define H2MP2_H

#include "gamedefs.h"

#define MP2OFFSETDATA	428

namespace MP2
{
    // origin mp2
    typedef struct {
        u16  tileIndex;		// tile (ocean, grass, snow, swamp, lava, desert, dirt, wasteland, beach)
        u8   objectName1;	// level 1.0
        u8   indexName1;	// index level 1.0 or 0xFF
        u8   quantity1;		// count
        u8   quantity2;		// count
        u8   objectName2;	// level 2.0
        u8   indexName2;	// index level 2.0 or 0xFF
        u8   shape;		// shape reflect % 4, 0 none, 1 vertical, 2 horizontal, 3 any
        u8   generalObject;	// zero or object
	u16  indexAddon;	// zero or index addons_t
	u32  uniqNumber1;	// level 1.0
	u32  uniqNumber2;	// level 2.0
    } tile_t;

    typedef struct {
	u16      indexAddon;	// zero or next addons_t
	u8       objectNameN1;	// level 1.N
	u8       indexNameN1;	// level 1.N or 0xFF
	u8       quantityN;	//
	u8       objectNameN2;	// level 2.N
	u8       indexNameN2;	// level 1.N or 0xFF
	u32      uniqNumberN1;	// level 1.N
	u32      uniqNumberN2;	// level 2.N
    } addons_t;
};

#endif
