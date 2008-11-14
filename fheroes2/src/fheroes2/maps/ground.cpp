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

#include "maps_tiles.h"
#include "direction.h"
#include "world.h"
#include "ground.h"

const std::string & Maps::Ground::String(Maps::Ground::ground_t ground)
{
    static const std::string str_ground[] = { "Desert", "Snow", "Swamp", "Wasteland", "Beach", 
	"Lava", "Dirt", "Grass", "Water", "Unknown" };

    switch(ground)
    {
        case DESERT:	return str_ground[0];
	case SNOW:	return str_ground[1];
	case SWAMP:	return str_ground[2];
	case WASTELAND:	return str_ground[3];
	case BEACH:	return str_ground[4];
	case LAVA:	return str_ground[5];
	case DIRT:	return str_ground[6];
	case GRASS:	return str_ground[7];
	case WATER:	return str_ground[8];
	default: break;
    }

    return str_ground[9];
}

u16 Maps::Ground::GetPenalty(const u16 index, const Direction::vector_t direct, const Skill::Level::type_t pathfinding)
{
    if(Direction::UNKNOWN == direct) return MAXU16;

    const Maps::Tiles & tile = world.GetTiles(index);
    u16 result = 0;

//            none   basc   advd   expr
//    Desert  2.00   1.75   1.50   1.00
//    Snow    1.75   1.50   1.25   1.00
//    Swamp   1.75   1.50   1.25   1.00
//    Cracked 1.25   1.00   1.00   1.00
//    Beach   1.25   1.00   1.00   1.00
//    Lava    1.00   1.00   1.00   1.00
//    Dirt    1.00   1.00   1.00   1.00
//    Grass   1.00   1.00   1.00   1.00
//    Water   1.00   1.00   1.00   1.00
//    Road    0.75   0.75   0.75   0.75

    // if(tile.isRoad(direct)) need fix isRoad(Direction::vector_t)
    if(tile.isRoad())
    {
	return 75;
    }
    else
    switch(tile.GetGround())
    {
	case DESERT:
    	    if(Skill::Level::EXPERT == pathfinding)	result = 100;
    	    else
    	    if(Skill::Level::ADVANCED == pathfinding)	result = 150;
    	    else
    	    if(Skill::Level::BASIC == pathfinding)	result = 175;
    	    else					result = 200;
        break;

	case SNOW:
	case SWAMP:
    	    if(Skill::Level::EXPERT == pathfinding)	result = 100;
    	    else
    	    if(Skill::Level::ADVANCED == pathfinding)	result = 125;
    	    else
    	    if(Skill::Level::BASIC == pathfinding)	result = 150;
    	    else					result = 175;
    	break;

	case WASTELAND:
	case BEACH:
					result = (Skill::Level::NONE == pathfinding ? 125 : 100); break;

	case LAVA:
	case DIRT:
	case GRASS:
	case WATER:			result = 100; break;

	default:			result = 100; break;
    }

    switch(direct)
    {
        case Direction::TOP_RIGHT:
        case Direction::BOTTOM_RIGHT:
        case Direction::BOTTOM_LEFT:
        case Direction::TOP_LEFT:	result = static_cast<u16>(1.44 * result); break;
	
	default: break;
     }

    return result;
}
