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

const std::string & Color::String(Color::color_t color)
{
    static const std::string blue("Blue");
    static const std::string green("Green");
    static const std::string red("Red");
    static const std::string yellow("Yellow");
    static const std::string orange("Orange");
    static const std::string purple("Purple");
    static const std::string gray("Gray");

    switch(color){
        case Color::BLUE: return blue;
        case Color::GREEN: return green;
        case Color::RED: return red;
        case Color::YELLOW: return yellow;
	case Color::ORANGE: return orange;
	case Color::PURPLE: return purple;
	default: break;
    }

    return gray;
}
