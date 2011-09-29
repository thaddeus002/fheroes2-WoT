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

#include "settings.h"
#include "mounts.h"

Mounts::Mounts(ICN::icn_t icn) : Object(MP2::OBJ_MOUNTS, icn)
{
    switch(icn)
    {
        case ICN::MTNCRCK:
	case ICN::MTNSNOW:
        case ICN::MTNSWMP:
        case ICN::MTNLAVA:
        case ICN::MTNDSRT:
        case ICN::MTNDIRT:
        case ICN::MTNMULT:
        case ICN::MTNGRAS: break;

        default: DEBUG(DBG_GAME, DBG_WARN, "unknown type"); break;
    }
}

bool Mounts::isPassable(u16 icn, u8 index, u16 direct)
{
    // bug fix
    switch(icn)
    {
        case ICN::MTNGRAS:
	    if(43 == index || 44 == index || 53 == index || 54 == index || 78 == index) return false;
	    break;
        case ICN::MTNDIRT:
	    if(73 == index || 84 == index || 86 == index) return false;
	    break;

	default: break;
    }

    // all
    switch(icn)
    {
        // 133 sprites
        case ICN::MTNCRCK:
        case ICN::MTNDIRT:
    	    if((5 < index && index < 10) || (13 < index && index < 17)) return false;		// LARGE LEFT
    	    else
	    if((11 < index && index < 14) || (17 < index && index < 21))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if((27 < index && index < 32) || (32 < index && index < 36)) return false;		// LARGE RIGHT
    	    else
	    if((35 < index && index < 38) || (38 < index && index < 42))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(49 < index && index < 53) return false;						// MEDIUM LEFT (51 and 52 for dirt need close)
    	    else
	    if((47 < index && index < 50) || (53 < index && index < 57))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(65 == index) return false;							// MEDIUM RIGHT
    	    else
	    if((65 < index && index < 68) || (68 < index && index < 72))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(76 < index && index < 79) return false;						// MEDIUM2 RIGHT
    	    else
	    if(76 == index || (79 < index && index < 82))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(87 == index) return false;							// MEDIUM2 LEFT
    	    else
	    if(88 == index || (89 < index && index < 92))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(94 == index) return false;							// SMALL LEFT
    	    else
	    if(93 == index || (95 < index && index < 98))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(99 == index) return false;							// SMALL RIGHT
    	    else
	    if(100 == index || (101 < index && index < 104))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(110 < index && index < 114) return Mines::isPassable(index, 112, direct);	// MINES
    	    else return true;

        // 83 sprites
        case ICN::MTNDSRT:
        case ICN::MTNGRAS:
        case ICN::MTNLAVA:
        case ICN::MTNMULT:
	case ICN::MTNSNOW:
        case ICN::MTNSWMP:
    	    if((5 < index && index < 10) || (13 < index && index < 17)) return false;		// LARGE LEFT
	    else
	    if((11 < index && index < 14) || (17 < index && index < 21))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
	    if(25 == index) return false;	// bug: level 1 sprite for mtngrass
    	    else
    	    if((27 < index && index < 32) || (32 < index && index < 36)) return false;		// LARGE RIGHT
	    else
	    if((35 < index && index < 38) || (38 < index && index < 42))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(46 < index && index < 49) return false;						// MEDIUM LEFT
	    else
	    if(46 == index || (49 < index && index < 52))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if((55 < index && index < 58)) return false;					// MEDIUM RIGHT
	    else
	    if(58 == index || (59 < index && index < 62))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(63 < index && index < 65) return false;						// SMALL LEFT
	    else
	    if(63 == index || (65 < index && index < 68))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if((68 < index && index < 70)) return false;					// SMALL RIGHT
	    else
	    if(70 == index || (71 < index && index < 74))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
    	    if(80 < index && index < 84)							// MINES
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
    	    else
		return true;

        default: break;
    }

    return false;
}
