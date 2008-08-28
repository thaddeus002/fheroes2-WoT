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

#include "icn.h"
#include "objcrck.h"

bool ObjWasteLand::isPassable(const u16 icn, const u8 index)
{
    switch(icn)
    {
	case ICN::OBJNCRCK:
	    // artesian spring
	    if(2 < index < 5) return false;
	    else
	    // rock
	    if(9 < index < 12 || 18 == index || 20 < index < 23 ||
		23 < index < 26 || 28 < index < 33 || 33 < index < 36) return false;
	    else
	    // cactus
	    if(14 == index || 16 == index) return false;
	    else
	    // skull
	    if(17 == index) return false;
	    /*
	     *
	     */
	    else return true;

	default: break;
    }

    return false;
}
