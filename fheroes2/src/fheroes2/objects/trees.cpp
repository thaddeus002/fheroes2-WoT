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
#include "trees.h"

Trees::Trees(ICN::icn_t icn) : Object(MP2::OBJ_TREES, icn)
{
    switch(icn)
    {
        case ICN::TREJNGL:
        case ICN::TREEVIL:
        case ICN::TRESNOW:
        case ICN::TREFIR:
        case ICN::TREFALL:
        case ICN::TREDECI: break;

        default: DEBUG(DBG_GAME, DBG_WARN, "unknown type"); break;
    }
}

bool Trees::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 Trees::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
        // 36 sprites
        case ICN::TREDECI:
        case ICN::TREEVIL:
        case ICN::TREFALL:
        case ICN::TREFIR:
        case ICN::TREJNGL:
        case ICN::TRESNOW:
    	    if(5 == index) return 0;	// LARGE LEFT
    	    else
	    if(4 == index || (7 < index && index < 10))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
    	    else
    	    if(15 == index) return 0;	// LARGE RIGHT
	    else
    	    if(16 == index || (17 < index && index < 20))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
    	    else
    	    if(22 == index) return 0;	// MEDIUM LEFT
    	    else
	    if(21 == index || (23 < index && index < 26))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
    	    else
    	    if(27 == index) return 0;	// MEDIUM RIGHT
	    else
    	    if(28 == index || (29 < index && index < 32))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
    	    else
    	    if(33 == index || 35 == index)	// SMALL
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
    	    else
		return DIRECTION_ALL;

        default: break;;
    }

    return 0;
}

bool Trees::isShadow(const u16 & icn, const u8 & index)
{
    const u8 shadows[] = { 0, 3, 7, 10, 13, 17, 20, 23, 26, 29, 32, 34 };
    return ARRAY_COUNT_END(shadows) != std::find(shadows, ARRAY_COUNT_END(shadows), index);
}
