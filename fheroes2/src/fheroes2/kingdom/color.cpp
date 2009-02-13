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

#include "color.h"

const std::string & Color::String(u8 color)
{
    static const std::string str_color[] = { _("Blue"), _("Green"), _("Red"), _("Yellow"), _("Orange"), _("Purple"), _("Gray") };

    switch(color)
    {
        case Color::BLUE: 	return str_color[0];
        case Color::GREEN: 	return str_color[1];
        case Color::RED:	return str_color[2];
        case Color::YELLOW:	return str_color[3];
	case Color::ORANGE: 	return str_color[4];
	case Color::PURPLE: 	return str_color[5];
	case Color::GRAY: 	return str_color[6];
    }

    return str_color[6];
}

u8 Color::GetIndex(color_t color)
{
    switch(color)
    {
        case BLUE: 	return 0;
        case GREEN: 	return 1;
        case RED:	return 2;
        case YELLOW:	return 3;
	case ORANGE: 	return 4;
	case PURPLE: 	return 5;
	case GRAY: 	return 6;
    }
    
    return 0;
}
