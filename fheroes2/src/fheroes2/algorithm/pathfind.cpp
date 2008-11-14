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
#include "settings.h"
#include "object.h"
#include "heroes.h"
#include "route.h"
#include "algorithm.h"

struct cell_t
{
    cell_t() : cost_g(MAXU16), cost_t(MAXU16), parent(MAXU16), open(true){};

    u16		cost_g;
    u16		cost_t;
    u16		parent;
    bool	open;
};

bool ImpassableCorners(const u16 from, const Direction::vector_t to)
{
    if( to & (Direction::TOP | Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)) return false;

    if(to & (Direction::TOP_LEFT | Direction::BOTTOM_LEFT))
    {
    	if(Maps::isValidDirection(from, Direction::LEFT) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::LEFT)).isPassable()) return true;
    }

    if(to & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::RIGHT) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::RIGHT)).isPassable()) return true;
    }

    if(to & (Direction::TOP_LEFT | Direction::TOP_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::TOP) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::TOP)).isPassable()) return true;
    }

    if(to & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::BOTTOM) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::BOTTOM)).isPassable()) return true;
    }

    return false;
}

bool Algorithm::PathFind(std::list<Route::Step> *result, const u16 from, const u16 to, const u16 limit, const Skill::Level::type_t pathfinding, const u8 under)
{
    const u8 debug = Settings::Get().Debug();

    u16 cur = from;
    u16 alt = 0;
    u16 tmp = 0;
    std::map<u16, cell_t> list;
    std::map<u16, cell_t>::iterator it1 = list.begin();
    std::map<u16, cell_t>::iterator it2 = list.end();
    Direction::vector_t direct = Direction::CENTER;

    list[cur].cost_g = 0;
    list[cur].cost_t = 0;
    list[cur].parent = MAXU16;
    list[cur].open   = false;

    u32 itr = 0;

    while(cur != to)
    {
	for(direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	{
    	    if(Maps::isValidDirection(cur, direct))
	    {
		tmp = Maps::GetDirectionIndex(cur, direct);

		if(list[tmp].open)
		{
		    // new
		    if(MAXU16 == list[tmp].parent)
		    {
			cell_t cell;
	    		cell.cost_g = Maps::Ground::GetPenalty(tmp, direct, pathfinding);
			cell.parent = cur;
			cell.open = true;
	    		cell.cost_t = cell.cost_g + list[cur].cost_t;

			if(MAXU16 == cell.cost_g) continue;

			// check direct from object
			const Maps::Tiles & tile1 = world.GetTiles(cur);
			if(MP2::OBJ_ZERO != under && MP2::OBJ_HEROES == tile1.GetObject() &&  ! Object::AllowDirect(under, direct)) continue;

			// check obstacles as corners
			//if(ImpassableCorners(cur, direct)) continue;  // disable, need fix more objects with passable option

			// check direct to object
			const Maps::Tiles & tile2 = world.GetTiles(tmp);
			if(! Object::AllowDirect(tile2.GetObject(), Direction::Reflect(direct))) continue;

			if(tile2.isPassable() || tmp == to) list[tmp] = cell;
		    }
		    // check alt
		    else
		    {
			alt = Maps::Ground::GetPenalty(cur, Direction::Reflect(direct), pathfinding);
			if(list[cur].cost_t > list[tmp].cost_t + alt)
			{
			    list[cur].parent = tmp;
			    list[cur].cost_g = alt;
			    list[cur].cost_t = list[tmp].cost_t + alt;
			}
		    }
    		}
	    }
	}

	if(3 <= debug)
	{
	    std::cout << "route, from: " << cur << std::endl;
	}

	it1 = list.begin();
	alt = tmp = MAXU16;
	
	// find minimal cost
	for(; it1 != it2; ++it1) if((*it1).second.open)
	{
	    const cell_t & cell = (*it1).second;
	    direct = Direction::Get(cur, (*it1).first);

	    if(Direction::UNKNOWN != direct)
	    {
		if(3 <= debug && cell.cost_g != MAXU16)
		{
		    std::cout << "  direct: " << Direction::String(Direction::Get(cur, (*it1).first));
		    std::cout << ", index: " << (*it1).first;
		    std::cout << ", cost g: " << cell.cost_g;
		    std::cout << ", cost t: " << cell.cost_t << std::endl;
		}
	    }

	    if(cell.cost_t < tmp)
	    {
    		tmp = (*it1).second.cost_t;
    		alt = (*it1).first;
	    }
	}

	// not found, and exception
	if(MAXU16 == tmp || MAXU16 == alt || (limit && itr > limit)) break;
	else
	if(3 <= debug)
	{
		std::cout << "  select: " << alt << std::endl << std::endl;
	}

	cur = alt;
	list[cur].open = false;
	++itr;
    }

    // save path
    if(cur == to)
    {
	while(cur != from)
	{
	    if(MAXU16 == list[cur].parent) break;
	    alt = cur;
    	    cur = list[cur].parent;
	    if(result) result->push_front(Route::Step(Direction::Get(cur, alt), list[alt].cost_g));
	}
        return true;
    }
    if(debug)
	std::cout << "Algorithm::PathFind: not found, from:" << from << ", to: " << to << std::endl;

    return false;
}
