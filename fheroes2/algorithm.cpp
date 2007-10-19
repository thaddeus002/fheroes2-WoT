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
#include "config.h"
#include "tools.h"
#include "error.h"
#include "algorithm.h"

struct cellinfo_t
{
    cellinfo_t() : cost_g(MAXU16),cost_h(MAXU16), parent(MAXU16), open_list(true) {};
    cellinfo_t(const u16 g, const u16 h, const u16 p, const Direction::vector_t & d, bool f) : cost_g(g), cost_h(h), parent(p), direct(d), open_list(f){};

    u16		cost_g;
    u16		cost_h;
    u16		parent;
    Direction::vector_t direct;
    bool	open_list;
};

// find path (A* implementation) - is valid return length path
u16 Algorithm::PathFinding(u16 index1, u16 index2, const Skill::Level::type_t & pathfinding, std::vector<u16> & result)
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
    work_map[index_i] = cellinfo_t(MAXU16, MAXU16, MAXU16, Direction::CENTER, false);

    while(index_i != index2 && !notfound)
    {
	std::vector<Direction::vector_t>::const_iterator it1 = directs.begin();
	std::vector<Direction::vector_t>::const_iterator it2 = directs.end();

	for(; it1 != it2; ++it1)
	{
	    if(Maps::isValidDirection(index_i, *it1))
	    {
		const u16 index_w = Maps::GetDirectionIndex(index_i, *it1);
		cellinfo_t & cell = work_map[index_w];

		if((world.GetTiles(index_w).isPassable() || index_w == index2) && (cell.cost_g == MAXU16))
		{
		    cell.cost_g = Maps::Ground::GetPenalty(index_i, index_w, pathfinding);
		    cell.cost_h = 10 * (std::abs(static_cast<s16>(index2 % width) - static_cast<s16>(index_w % width)) +
			                 std::abs(static_cast<s16>(index2 / width) - static_cast<s16>(index_w / width)));

		    cell.direct = *it1;
		    cell.open_list = true;
		    cell.parent = index_i;
		}

	    }
	}

	if(H2Config::Debug()) Error::Verbose("dump penalty, from index: ", index_i);

	std::map<u16, cellinfo_t>::iterator im1 = work_map.begin();
        std::map<u16, cellinfo_t>::iterator im2 = work_map.end();

	// check min from current
        for(; im1 != im2; ++im1)
    	    if((*im1).second.open_list &&
    		Direction::UNKNOWN != Direction::Get(index_i, (*im1).first) &&
    		(*im1).second.cost_g > work_map[index_i].cost_g + Maps::Ground::GetPenalty(index_i, (*im1).first, pathfinding))
    	    {
    		(*im1).second.cost_g = work_map[index_i].cost_g + Maps::Ground::GetPenalty(index_i, (*im1).first, pathfinding);
    		(*im1).second.parent = index_i;

		if(H2Config::Debug())
		{
		    std::string value = "                to index: ";
		    String::AddInt(value, (*im1).first);
		    value += ", correct minimal cost_g";

		    Error::Verbose(value);
    		}
	    }


	// dump open list
	im1 = work_map.begin();
        for(; im1 != im2; ++im1)
    	    if((*im1).second.open_list)
    	    {
		if(H2Config::Debug() && Direction::UNKNOWN != Direction::Get(index_i, (*im1).first))
		{
		    std::string value = "                to index: ";
		    String::AddInt(value, (*im1).first);
		    value += ", direct: " + Direction::String((*im1).second.direct);
		    value += ", status: " + std::string(((*im1).second.open_list ? "true" : "false"));
		    value += ", cost_g: ";
		    String::AddInt(value, (*im1).second.cost_g);
		    value += ", cost_h: ";
		    String::AddInt(value, (*im1).second.cost_h);

		    Error::Verbose(value);
		}
	    }

	// goto minimal cost
	im1 = work_map.begin();
	u16 cost = MAXU16;
	u16 newindex = index_i;
        for(; im1 != im2; ++im1)
    	    if((*im1).second.open_list &&
    		(*im1).second.cost_g + (*im1).second.cost_h < cost)
    	    {
    		cost = (*im1).second.cost_g + (*im1).second.cost_h;
    		newindex = (*im1).first;
    	    }

	// check not found
	if(newindex != index_i)
	{
	    index_i = newindex;
	    work_map[index_i].open_list = false;

	    if(H2Config::Debug()) Error::Verbose("                  select: ", index_i);
	}
	else
	    notfound = true;

	++count_itr;

	// exception (invalid algorithm)
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
