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

#include "ground.h"

const std::string & Maps::Ground::String(Maps::Ground::ground_t ground)
{
    static const std::string str_desert("Desert");
    static const std::string str_snow("Snow");
    static const std::string str_swamp("Swamp");
    static const std::string str_wasteland("Wasteland");
    static const std::string str_beach("Beach");
    static const std::string str_lava("Lava");
    static const std::string str_dirt("Dirt");
    static const std::string str_grass("Grass");
    static const std::string str_water("Water");
    static const std::string str_road("Road");
    static const std::string str_multi("Multi");

    switch(ground){
        case DESERT:	return str_desert;
	case SNOW:	return str_snow;
	case SWAMP:	return str_swamp;
	case WASTELAND:	return str_wasteland;
	case BEACH:	return str_beach;
	case LAVA:	return str_lava;
	case DIRT:	return str_dirt;
	case GRASS:	return str_grass;
	case WATER:	return str_water;
	case ROAD:	return str_road;
	default:	break;
    }

    return str_multi;
}
