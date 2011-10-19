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
#include "objlava.h"

bool ObjLava::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 ObjLava::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
	case ICN::OBJNLAV2:
	    // volcano
	    if((129 < index && index < 133) || (78 < index && index < 82))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
            else
                return DIRECTION_ALL;

	case ICN::OBJNLAV3:
	    // volcano
	    if(243 < index && index < 247)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
            else
                return DIRECTION_ALL;

	case ICN::OBJNLAVA:
	    // crater
	    if((1 < index && index < 6) || (11 < index && index < 16)) return 0;
	    else
	    if((5 < index && index < 10) || (15 < index && index < 18))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // lava pool
	    if(18 == index || (26 < index && index < 33) || (38 < index && index < 42) ||
		(45 < index && index < 49) || ( 52 < index && index < 55) || (57 == index) || (59 < index && index < 62) ||
		(63 < index && index < 66) || (68 < index && index < 71)) return 0;
	    else
	    if(20 < index && index < 26)
                return DIRECTION_ALL;
	    else
	    if((18 < index && index < 21) || (32 < index && index < 39) || (41 < index && index < 45) ||
		(49 < index && index < 53) || (54 < index && index < 57) || (57 < index && index < 60) || (61 < index || index < 63) ||
		(65 < index && index < 69) || (71 < index && index < 74))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // volcano
	    if((75 < index && index < 78) || 88 == index || 98 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // obelisk
	    if(110 == index) return 0;
	    else
	    // daemon cave
	    if(113 < index && index < 116)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // sign
	    if(117 == index) return 0;
	    else
	    // saw mill
	    if(119 < index && index < 122) return 0;
	    else
	    if(121 < index && index < 127)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
		return DIRECTION_ALL;

	default: break;
    }

    return 0;
}
