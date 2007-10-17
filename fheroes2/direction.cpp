/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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

#include "maps.h"
#include "error.h"
#include "direction.h"

const std::string & Direction::String(vector_t direct)
{
   static const std::string str_direct[] = { "center", "top", "top right", "right", "bottom right", "bottom", "bottom left", "left", "top left", "unknown" };
   
   switch(direct)
   {
	case CENTER:		return str_direct[0];
	case TOP:		return str_direct[1];
	case TOP_RIGHT:		return str_direct[2];
	case RIGHT:		return str_direct[3];
	case BOTTOM_RIGHT:	return str_direct[4];
	case BOTTOM:		return str_direct[5];
	case BOTTOM_LEFT:	return str_direct[6];
	case LEFT:		return str_direct[7];
	case TOP_LEFT:		return str_direct[8];
	default:	break;
   }
 
    return str_direct[9];
}


Direction::vector_t Direction::Get(u16 from, u16 to)
{
    if(to == from)
	return CENTER;
    else
    if(to == Maps::GetTopIndex(from))
	return TOP;
    else
    if(to == Maps::GetTopRightIndex(from))
    	return TOP_RIGHT;
    else
    if(to == Maps::GetRightIndex(from))
    	return RIGHT;
    else
    if(to == Maps::GetBottomRightIndex(from))
    	return BOTTOM_RIGHT;
    else
    if(to == Maps::GetBottomIndex(from))
    	return BOTTOM;
    else
    if(to == Maps::GetBottomLeftIndex(from))
    	return BOTTOM_LEFT;
    else
    if(to == Maps::GetLeftIndex(from))
    	return LEFT;
    else
    if(to == Maps::GetTopLeftIndex(from))
    	return TOP_LEFT;

    //Error::Warning("Direction::Get: from: ", from);
    //Error::Warning("Direction::Get:   to: ", to);
    //Error::Warning("Direction::Get: return UNKNOWN");

    return UNKNOWN;
}
