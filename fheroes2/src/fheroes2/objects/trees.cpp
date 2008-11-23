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

#include "trees.h"

Trees::Trees(const ICN::icn_t icn) : Object(MP2::OBJ_TREES, icn)
{
    switch(icn)
    {
        case ICN::TREJNGL:
        case ICN::TREEVIL:
        case ICN::TRESNOW:
        case ICN::TREFIR:
        case ICN::TREFALL:
        case ICN::TREDECI: break;

        default: Error::Warning("Trees::Trees: unknown type: ", icn); break;
    }
}

bool Trees::isPassable(const u16 icn, const u8 index)
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
    	    if((3 < index && index < 6) || (7 < index && index < 10)) return false;	// LARGE LEFT
    	    else
    	    if((14 < index && index < 17) || (17 < index && index < 20)) return false;	// LARGE RIGHT
    	    else
    	    if((20 < index && index < 23) || (23 < index && index < 26)) return false;	// MEDIUM LEFT
    	    else
    	    if((26 < index && index < 29) || (29 < index && index < 32)) return false;	// MEDIUM RIGHT
    	    else
    	    if(33 == index || 35 == index) return false;				// SMALL
    	    else return true;

        default: break;;
    }

    return false;
}
