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
#include "world.h"
#include "direction.h"
#include "algorithm.h"

// find path  - is valid return length path
u16 Algorithm::PathFinding(u16 index0, u16 index9, const Skill::level_t & pathfinding, std::vector<u16> & path0)
{
/*
    std::vector<u16> path_i;
    
    u16 index_i = index0;

    if(!(index_i < world.w()) && (index_i % world.w()))
				path_i.push_back(Maps::GetTopLeftIndex(index_i));

    if(!(index_i < world.w()))
    	    			path_i.push_back(Maps::GetTopIndex(index_i));

    if(!(index_i < world.w()) && (world.w() - 1 > (index_i % world.w())))
				path_i.push_back(Maps::GetTopRightIndex(index_i));

    if(world.w() - 1 > (index_i % world.w()))
    				path_i.push_back(Maps::GetRightIndex(index_i));

    if((index_i < world.w() * (world.h() - 1)) && (world.w() - 1 > (index_i % world.w())))
				path_i.push_back(Maps::GetBottomRightIndex(index_i));

    if(index_i < world.w() * (world.h() - 1))
				path_i.push_back(Maps::GetBottomIndex(index_i));

    if((index_i < world.w() * (world.h() - 1)) && (index_i % world.w()))
				path_i.push_back(Maps::GetBottomLeftIndex(index_i));

    if(index_i % world.w())
				path_i.push_back(Maps::GetLeftIndex(index_i));

    // const Maps::Tiles & tile = world.GetTiles(index);
    // tile.isPassable()
    // tile.GetGround()
    // Ground::GetPenalty() / 100;
*/
    return 0;
}
