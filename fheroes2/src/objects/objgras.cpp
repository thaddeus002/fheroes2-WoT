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
#include "objgras.h"

bool ObjGrass::isPassable(const u16 icn, const u8 index)
{
    switch(icn)
    {
	case ICN::OBJNGRA2:
	    // hill fort
	    if(1 < index < 5) return false;
	    else
	    // halfling hole
	    if(5 < index < 9) return false;
	    else
	    // tree city
	    if(20 < index < 23) return false;
	    else
	    // wind mill
	    if(55 == index || 59 == index) return false;
	    else
	    // archer house
	    if(84 == index) return false;
	    else
	    // goblin hut
	    if(92 == index) return false;
	    else
	    // dwarf cottadge
	    if(114 == index) return false;
	    else
	    // oracul
	    if(124 < index < 127) return false;
	    else
	    // obelisk
	    if(129 == index) return false;
	    else return true;	    

	case ICN::OBJNGRAS:
	    // abandoned mines
	    if(3 == index || 4 < index < 8) return false;
	    else
	    // faerie ring
	    if(29 < index < 32) return false;
	    else
	    // rock
	    if(32 < index < 35 || 36 < index < 39 || 39 < index < 42 ||
		43 == index || 45 == index || 47 == index || 49 == index) return false;
	    else
	    // lake
	    if(53 < index < 64 || 64 < index < 76) return false;
	    else
	    // mound
	    if(76 < index < 79 || 148 < index < 151) return false;
	    else
	    // trees
	    if(82 < index < 86 || 88 < index < 91 || 93 == index) return false;
	    else
	    // shrub
	    if(94 < index < 98 || 98 < index < 102 || 102 < index < 105 ||
		105 < index < 108 || 108 < index < 111 || 112 == index ||
		113 < index < 117 || 120 < index < 124 || 124 < index < 128 ||
		128 < index < 132 || 134 < index < 137 || 138 < index < 141 ||
		142 == index || 144 == index || 146 == index || 148 == index) return false;
	    else return true;

	default: break;
    }

    return false;
}
