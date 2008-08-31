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
#include "objdirt.h"

bool ObjDirt::isPassable(const u16 icn, const u8 index)
{
    switch(icn)
    {
	case ICN::OBJNDIRT:
	    // abandone mines
	    if(6 < index < 10) return false;
	    else
	    // mound
	    if(11 < index < 14 || 14 < index < 17) return false;
	    else
	    // crater
	    if(16 < index < 23) return false;
	    // lake
	    else
	    if(22 < index < 39 || 39 < index < 46 ||
		47 < index < 52 || 52 < index < 59) return false;
	    else
	    // shrub
	    if(59 < index < 62 || 62 < index < 65 || 65 < index < 68 ||
		69 == index || 71 == index || 72 < index < 75 || 75 < index < 78 ||
		78 < index < 81 || 81 < index < 84 || 84 < index < 87 || 87 < index < 91) return false;
	    else
	    // rock
	    if(91 < index < 94 || 97 < index < 100 || 100 < index < 103 || 103 < index < 106) return false;
	    else
	    // trees
	    if(118 == index || 123  == index || 127 == index)  return false;
	    else
	    // faerie ring
	    if(128 < index < 131)  return false;
	    else
	    // hill fort
	    if(132 < index < 136)  return false;
	    else
	    // halfling hole
	    if(136 < index < 140)  return false;
	    else
	    // tree city
	    if(150 < index < 153)  return false;
	    else
	    // wind mill
	    if(185 == index || 189 == index) return false;
	    else
	    // oracul
	    if(196 < index < 199)  return false;
	    else
	    // obelisk
	    if(201 == index)  return false;
	    else return true;

	default: break;
    }

    return false;
}
