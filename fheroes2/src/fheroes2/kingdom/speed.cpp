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

#include "speed.h"

const std::string & Speed::String(u8 speed)
{
    static const std::string str_speed[] = { "Standing", "Crawling", "Very Slow", "Slow", "Average", "Fast",
	"Very Fast", "Ultra Fast", "Blazing", "Instant" };

    switch(speed)
    {
	case CRAWLING:	return str_speed[1];
        case VERYSLOW:	return str_speed[2];
        case SLOW:	return str_speed[3];
        case AVERAGE:	return str_speed[4];
        case FAST:	return str_speed[5];
        case VERYFAST:	return str_speed[6];
        case ULTRAFAST:	return str_speed[7];
        case BLAZING:	return str_speed[8];
        case INSTANT:	return str_speed[9];
	default: break;
    }
    
    return str_speed[0];
}
