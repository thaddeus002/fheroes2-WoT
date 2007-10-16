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

#include <map>
#include "maps.h"
#include "world.h"
#include "direction.h"
#include "error.h"
#include "algorithm.h"

struct cellinfo_t
{
    cellinfo_t() : cost(MAXU16), parent(MAXU16), open_list(true) {};
    cellinfo_t(const u16 c, const u16 p, const Direction::vector_t & d, bool f) : cost(c), parent(p), direct(d), open_list(f){};

    u16		cost;
    u16		parent;
    Direction::vector_t direct;
    bool	open_list;
};

// find path (A* implementation) - is valid return length path
u16 Algorithm::PathFinding(u16 index1, u16 index2, const Skill::level_t & pathfinding, std::vector<u16> & result)
{
    const u16 width = world.w();
    
    std::map<u16, cellinfo_t>	work_map;
    
    std::vector<Direction::vector_t> directs;

    directs.push_back(Direction::TOP);
    directs.push_back(Direction::TOP_LEFT);
    directs.push_back(Direction::TOP_RIGHT);
    directs.push_back(Direction::RIGHT);
    directs.push_back(Direction::BOTTOM_RIGHT);
    directs.push_back(Direction::BOTTOM);
    directs.push_back(Direction::BOTTOM_LEFT);
    directs.push_back(Direction::LEFT);

    bool notfound = false;
    u32 count_itr = 0;

    u16 index_i = index1;
    work_map[index_i] = cellinfo_t(MAXU16, MAXU16, Direction::CENTER, false);

    while(index_i != index2 && !notfound)
    {
	std::vector<Direction::vector_t>::const_iterator it1 = directs.begin();
	std::vector<Direction::vector_t>::const_iterator it2 = directs.end();

	for(; it1 != it2; ++it1)
	{
	    const Direction::vector_t & direct = *it1;
	    if(Maps::isValidDirection(index_i, direct))
	    {
		const u16 index_w = Maps::GetDirectionIndex(index_i, direct);
		const Maps::Tiles & tile = world.GetTiles(index_w);
		cellinfo_t & cell = work_map[index_w];

		if(tile.isPassable() && (cell.cost == MAXU16))
		{
		    cell.cost = GetPenalty(tile.GetGround(), pathfinding, direct) +
			    100 * (std::abs((index2 % width) - (index_i % width)) + std::abs(static_cast<u16>(index2 / width) - static_cast<u16>(index_i / width)));

		    cell.parent = index_i;
		    cell.direct = direct;
		    cell.open_list = true;
		}
	    }
	}

	// goto minimal cost
	std::map<u16, cellinfo_t>::iterator im1 = work_map.begin();
        std::map<u16, cellinfo_t>::iterator im2 = work_map.end();
	u16 cost = MAXU16;
	u16 newindex = index_i;
        for(; im1 != im2; ++im1) if((*im1).second.open_list && (*im1).second.cost < cost){ cost = (*im1).second.cost; newindex = (*im1).first; }

	if(newindex != index_i)
	{
	    index_i = newindex;
	    work_map[index_i].open_list = false;
	}
	else
	    notfound = true;

	++count_itr;

	if(count_itr > 65000)
	{
	    Error::Warning("Algorithm::PathFinding: unknown path finding, check algorithm.");

	    notfound = true;
	}
    }

    if(!notfound)
    {
	while(index_i != index1)
	{
	    // push_front
	    result.insert(result.begin(), index_i);

	    index_i = work_map[index_i].parent;
	}
    }

    return result.size();
}
