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
#include "direction.h"
#include "objtown.h"
#include "settings.h"

bool ObjTown::isPassable(u16 icn, u8 index, u16 direct)
{
    switch(icn)
    {
	case ICN::OBJNTWBA:
	    index = index % 10;
	     // 2, 12, 22, 32, 42, 52, 62, 72
	    if(2 == index)
		return Direction::UNKNOWN == direct || Direction::BOTTOM == direct;
            else
	    if(index < 5)
		return false;
	    else
	     // 7, 17, 27, 37, 47, 57, 67, 77
	    if(7 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW | Direction::TOP));
	    else
	    if(4 < index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
		return true;
	    break;

	case ICN::OBJNTOWN:
	    index = index % 32;
	    // 13, 29, 45, 61, 77, 93, 109, 125, 141, 157, 173, 189
	    if(13 == index || 29 == index)
		return Direction::UNKNOWN == direct || Direction::BOTTOM == direct;
	    else
	    // town/castle
	    if((5 < index && index < 13) || (13 < index && index < 16) ||
		(21 < index && index < 29) || (29 < index)) return false;
	    else
		return true;

	default: break;
    }

    return false;
}
