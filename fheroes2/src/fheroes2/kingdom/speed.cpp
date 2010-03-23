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

#include "speed.h"

const char* Speed::String(u8 speed)
{
    const char* str_speed[] = { _("speed|Standing"), _("speed|Crawling"), _("speed|Very Slow"), _("speed|Slow"), _("speed|Average"), _("speed|Fast"),
	_("speed|Very Fast"), _("speed|Ultra Fast"), _("speed|Blazing"), _("speed|Instant"), "Unknown" };

    switch(speed)
    {
	case STANDING:	return str_speed[0];
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
    
    return str_speed[10];
}
