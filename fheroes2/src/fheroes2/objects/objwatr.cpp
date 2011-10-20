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
#include "objwatr.h"

bool ObjWater::isPassable(const u16 & icn, const u8 & index, u16 direct)
{
    return direct & GetPassable(icn, index);
}

u16 ObjWater::GetPassable(const u16 & icn, const u8 & index)
{
    switch(icn)
    {
	case ICN::OBJNWAT2:
	    // rock
	    if(2 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // ship
	    if(11 == index || 12 == index || 19 == index || 21 == index || 22 == index)
		return 0;
	    else
	    if(20 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
                return DIRECTION_ALL;

	case ICN::OBJNWATR:
	    // buttle
	    if(0 == index) return 0;
	    else
	    // chest
	    if(19 == index) return 0;
	    else
	    // flotsam
	    if(45 == index) return 0;
	    else
	    // magellan maps
	    if(62 == index) return 0;
	    else
	    if(69 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // surviror
	    if(111 == index) return 0;
	    else
	    // rock
	    if((181 < index && index < 184) || (184 < index && index < 188))
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
	    // buoy
	    if(195 == index) return 0;
	    else
	    // whirlpoll
	    if(202 == index || 206 == index || 210 == index || 214 == index || 218 == index || 222 == index) return 0;
	    else
	    // ship
	    if(241 == index) return 0;
	    else
	    if(248 == index)
		return DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW;
	    else
		return DIRECTION_ALL;

	default: break;
    }

    return 0;
}
