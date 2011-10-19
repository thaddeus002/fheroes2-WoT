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
#include "objdsrt.h"

bool ObjDesert::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 ObjDesert::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
	case ICN::OBJNDSRT:
	    // trees
	    if(3 == index || 6 == index || 9 == index || 12 == index ||
		24 == index || 26 == index || 28 == index || 76 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // dune
	    if((13 < index && index < 16) || (16 < index && index < 19) || (19 < index && index < 23))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // cactus
	    if((29 < index && index < 33) || 34 == index || 36 == index ||
		(38 < index && index < 41) || 42 == index || 45 == index ||
		(47 < index && index < 50) || 51 == index || 53 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // camp fire
	    if(61 == index) return 0;
	    else
	    // desert tent
	    if(71 < index && index < 74)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // piramids
	    if(80 < index && index < 83)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // skeleton
	    if(82 < index && index < 85)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // sphinx
	    if(86 < index && index < 89)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // city of dead
	    if((88 < index && index < 94) || index == 96) return 0;
	    else
	    if(93 < index && index < 99)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // excavation
	    if(index == 101) return 0;
	    else
	    if(98 < index && index < 102)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // obelisk
	    if(104 == index) return 0;
	    else
	    // oasis
	    if(104 < index && index < 107) return 0;
	    else
	    if(107 < index && index < 110)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // desert
	    if(109 < index && index < 113)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // daemon cave
	    if(115 < index && index < 118)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // sign
	    if(119 == index) return 0;
	    else
	    // grave yard
	    if(120 < index && index < 123)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // saw mill
	    if(124 < index && index < 127) return 0;
	    else
	    if(126 < index && index < 131)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
		return DIRECTION_ALL;

	default: break;
    }

    return 0;
}
