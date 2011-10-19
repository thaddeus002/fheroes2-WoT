/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include "icn.h"
#include "objdirt.h"

bool ObjDirt::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 ObjDirt::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
	case ICN::OBJNDIRT:
	    // abandone mines
	    if(6 < index && index < 10)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // mound
	    if((11 < index && index < 14) || (14 < index && index < 17))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // crater
	    if(16 < index && index < 23)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    // lake
	    else
	    if((22 < index && index < 39) || (47 < index && index < 52)) return 0;
	    else
	    if((39 < index && index < 46) || (52 < index && index < 57) || (56 < index && index < 59))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // shrub
	    if((59 < index && index < 62) || (62 < index && index < 65) || (65 < index && index < 68) ||
		69 == index || 71 == index || (72 < index && index < 75) || (75 < index && index < 78) ||
		(78 < index && index < 81) || (81 < index && index < 84) || (84 < index && index < 87) || (87 < index && index < 91))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // rock
	    if((91 < index && index < 94) || (97 < index && index < 100) || (100 < index && index < 103) || (103 < index && index < 106))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // trees
	    if(118 == index || 123  == index || 127 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // faerie ring
	    if(128 < index && index < 131)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // hill fort
	    if(132 < index && index < 134)  return 0;
	    else
	    if(134 == index || index == 135)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // halfling hole
	    if(136 < index && index < 140)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // tree city
	    if(150 < index && index < 153)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // wind mill
	    if(185 == index || 189 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // oracul
	    if(196 < index && index < 199)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // obelisk
	    if(201 == index)  return 0;
	    else
		return DIRECTION_ALL;

	default: break;
    }

    return 0;
}
