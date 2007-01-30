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
#ifndef H2MAPS_H
#define H2MAPS_H

#include "gamedefs.h"

namespace Maps
{
    typedef enum {
	BLUE	= 0x01,
    	GREEN	= 0x02,
        RED	= 0x04,
        YELLOW	= 0x08,
        ORANGE	= 0x10,
        PURPLE	= 0x20,
        GRAY	= 0x40,
    } color_t;

    typedef enum {
	NONE	= 0,
        SMALL	= 36,
        MEDIUM	= 72,
        LARGE	= 108,
        XLARGE	= 144
    } mapsize_t;

    typedef enum {
	EASY,
        NORMAL,
        HARD,
        EXPERT,
        IMPOSSIBLE
    } difficulty_t;

    typedef enum {
	KNGT	= 0x01,
        BARB	= 0x02,
        SORC	= 0x04,
        WRLK	= 0x08,
        WZRD	= 0x10,
        NECR	= 0x20,
	MULT	= 0x40,
	RAND	= 0x80,
	BOMG	= 0x00
    } race_t;

    difficulty_t& operator++ (difficulty_t& difficulty);
    difficulty_t& operator-- (difficulty_t& difficulty);

    color_t& operator++ (color_t& color);
    color_t& operator-- (color_t& color);
};

#endif
