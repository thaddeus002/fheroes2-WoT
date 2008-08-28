/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "error.h"
#include "mounts.h"

Mounts::Mounts(const ICN::icn_t icn) : Object(MP2::OBJ_MOUNTS, icn)
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

        default: Error::Warning("Mounts::Mounts: unknown type: ", icn); break;
    }
}

bool Mounts::isPassable(const u16 icn, const u8 index)
{
    switch(icn)
    {
        // 133 sprites
        case ICN::MTNCRCK:
        case ICN::MTNDIRT:
    	    if(5 < index < 10 || 11 < index < 17 || 17 < index < 21) return false;		// LARGE LEFT
    	    else
    	    if(27 < index < 32 || 32 < index < 38 || 38 < index < 42) return false;		// LARGE RIGHT
    	    else
    	    if(47 < index < 51 || 53 < index < 57) return false;				// MEDIUM LEFT
    	    else
    	    if(64 < index < 68 || 68 < index < 72) return false;				// MEDIUM RIGHT
    	    else
    	    if(75 < index < 79 || 79 < index < 82) return false;				// MEDIUM2 RIGHT
    	    else
    	    if(86 < index < 89 || 89 < index < 92) return false;				// MEDIUM2 LEFT
    	    else
    	    if(92 < index < 95 || 95 < index < 98) return false;				// SMALL LEFT
    	    else
    	    if(98 < index < 101 || 101 < index < 104) return false;				// SMALL RIGHT
    	    else
    	    if(110 < index < 114) return false; 						// MINES
    	    else return true;

        // 83 sprites
        case ICN::MTNDSRT:
        case ICN::MTNGRAS:
        case ICN::MTNLAVA:
        case ICN::MTNMULT:
	case ICN::MTNSNOW:
        case ICN::MTNSWMP:
    	    if(5 < index < 10 || 11 < index < 17 || 17 < index < 21) return false;		// LARGE LEFT
    	    else
    	    if(27 < index < 32 || 32 < index < 38 || 38 < index < 42) return false;		// LARGE RIGHT
    	    else
    	    if(45 < index < 49 || 49 < index < 52) return false;				// MEDIUM LEFT
    	    else
    	    if(55 < index < 59 || 59 < index < 62) return false;				// MEDIUM RIGHT
    	    else
    	    if(62 < index < 65 || 65 < index < 68) return false;				// SMALL LEFT
    	    else
    	    if(68 < index < 71 || 71 < index < 74) return false;				// SMALL RIGHT
    	    else
    	    if(80 < index < 84) return false; 							// MINES
    	    else return true;

        default: break;
    }

    return false;
}
