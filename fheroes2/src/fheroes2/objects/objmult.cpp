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
#include "objmult.h"

bool ObjMulti::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 ObjMulti::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
	case ICN::OBJNMUL2:
	    // fountain
	    if(15 == index) return 0;
	    else
	    // stub
	    if(16 == index || (17 < index && index < 20))
                return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // alchemy tower
	    if(24 < index && index < 28)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // dragon city
	    if(46 == index) return 0;
	    else
	    if(50 < index && index < 56)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // grave yard
	    if(56 < index && index < 59)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // light house
	    if(73 == index) return 0;
	    else
	    // saw mill
	    if(75 < index && index < 78) return 0;
	    else
	    if(77 < index && index < 82)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // water well
	    if(98 == index || 105 == index || 112 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // sign
	    if(114 == index) return 0;
	    else
	    // teleport
	    if(116 == index || 119 == index || 122 == index) return 0;
	    else
	    // wagon camp
	    if(123 < index && index < 127) return 0;
	    else
	    if((127 < index && index < 130) || 136 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // well
	    if(162 == index || 165 == index) return 0;
	    else
	    // freeman foundry
	    if(186 < index && index < 189)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // magick garden
	    if(190 == index) return 0;
	    else
	    // observation tower
	    if(201 == index) return 0;
	    else
	    // grave yard
	    if(206 < index && index < 210)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // saw mill
	    if(211 < index && index < 214) return 0;
	    else
	    if(213 < index && index < 217)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
            else
                return DIRECTION_ALL;

        case ICN::OBJNMULT:
    	    // dead tree
            if(2 == index || 4 == index)
                return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // peasant hut
	    if(35 == index) return 0;
	    else
	    // fort
	    if(57 < index && index < 60)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // gasebo
	    if(62 == index) return 0;
	    else
	    // shrub
	    if(62 < index && index < 66)
                return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // witch hut
	    if(69 == index) return 0;
	    else
	    // mercenary camp
	    if(69 < index && index < 73)
                return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // ruins
	    if(72 < index && index < 75)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // shrine
	    if(76 == index || 78 == index || 80 == index) return 0;
	    else
	    // idol
	    if(82 == index) return 0;
	    else
	    // standing stones
	    if(83 < index && index < 86)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // temple
	    if(87 < index && index < 90)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // market
	    if(104 == index || 111 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // tree house
	    if(114 == index) return 0;
	    else
	    // watch tower
	    if(116 == index) return 0;
	    else
	    // tree knowledge
	    if(123 == index) return 0;
	    else
	    // camp fire
	    if(131 == index) return 0;
	    else
		return DIRECTION_ALL;

	default: break;
    }

    return 0;
}
