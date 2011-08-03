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

#include <bitset>
#include <sstream>
#include "color.h"

const char* Color::String(u8 color)
{
    const char* str_color[] = { "Unknown", _("Blue"), _("Green"), _("Red"), _("Yellow"), _("Orange"), _("Purple") };

    switch(color)
    {
        case Color::BLUE: 	return str_color[1];
        case Color::GREEN: 	return str_color[2];
        case Color::RED:	return str_color[3];
        case Color::YELLOW:	return str_color[4];
	case Color::ORANGE: 	return str_color[5];
	case Color::PURPLE: 	return str_color[6];
    }

    return str_color[0];
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
	default: break;
    }

    // NONE
    return 6;
}

u8 Color::Count(u8 colors)
{
    const std::bitset<8> bset(colors);
    return bset.count();
}

Color::color_t Color::GetFromIndex(u8 index)
{
    switch(index)
    {
        case 0:	return BLUE;
        case 1:	return GREEN;
        case 2:	return RED;
        case 3:	return YELLOW;
	case 4:	return ORANGE;
	case 5:	return PURPLE;
	default: break;
    }

    return NONE;
}

Color::color_t Color::Get(u8 index)
{
    switch(index)
    {
        case BLUE:	return BLUE;
        case GREEN:	return GREEN;
        case RED:	return RED;
        case YELLOW:	return YELLOW;
	case ORANGE:	return ORANGE;
	case PURPLE:	return PURPLE;
	default: break;
    }

    return NONE;
}

Color::color_t Color::GetFirst(u8 colors)
{
    if(colors & BLUE) return BLUE;
    else
    if(colors & GREEN) return GREEN;
    else
    if(colors & RED) return RED;
    else
    if(colors & YELLOW) return YELLOW;
    else
    if(colors & ORANGE) return ORANGE;
    else
    if(colors & PURPLE) return PURPLE;

    return NONE;
}

const char* BarrierColor::String(u8 val)
{
    switch(val)
    {
        case 0x01:	return _("Aqua");
        case 0x02:	return _("Blue");
        case 0x04:	return _("Brown");
        case 0x08:	return _("Gold");
        case 0x10:	return _("Green");
        case 0x20:	return _("Orange");
        case 0x40:	return _("Purple");
        case 0x80:	return _("Red");
        default: break;
    }

    return "None";
}

u8 BarrierColor::FromMP2(u8 val)
{
    switch(val)
    {
        case 0:	return AQUA;
        case 8:	return BLUE;
        case 16:return BROWN;
        case 24:return GOLD;
        case 32:return GREEN;
        case 40:return ORANGE;
        case 48:return PURPLE;
        case 56:return RED;
        default: break;
    }

    return NONE;
}

Colors::Colors(u8 colors)
{
    reserve(6);

    if(colors & Color::BLUE)	push_back(Color::BLUE);
    if(colors & Color::GREEN)	push_back(Color::GREEN);
    if(colors & Color::RED)	push_back(Color::RED);
    if(colors & Color::YELLOW)	push_back(Color::YELLOW);
    if(colors & Color::ORANGE)	push_back(Color::ORANGE);
    if(colors & Color::PURPLE)	push_back(Color::PURPLE);
}

std::string Colors::String(void) const
{
    std::ostringstream os;

    for(const_iterator
	it = begin(); it != end(); ++it)
	    os << Color::String(*it) << ", ";

    return os.str();
}
