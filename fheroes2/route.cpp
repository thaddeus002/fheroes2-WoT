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

#include "agg.h"
#include "world.h"
#include "heroes.h"
#include "algorithm.h"
#include "direction.h"
#include "maps.h"
#include "game.h"
#include "config.h"
#include "error.h"
#include "route.h"

/* construct */
Route::Route(const Heroes & h)
    : hero(h), dst(Maps::GetIndexFromAbsPoint(h.GetCenter()))
{
}

/* return length path */
u16 Route::Calculate(u16 dst_index)
{
    path.clear();

    Algorithm::PathFinding(Maps::GetIndexFromAbsPoint(hero.GetCenter()), dst_index, hero.GetLevelSkill(Skill::PATHFINDING), path);

    dst = dst_index;

    return path.size();
}

void Route::Show(void) const
{
    if(path.size())
    {
	u16 from = Maps::GetIndexFromAbsPoint(hero.GetCenter());

	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	for(; it1 != it2; ++it1)
	{
	    Maps::Tiles & tile = world.GetTiles(*it1);

	    std::vector<u16>::const_iterator it3 = it1 + 1;

	    if(it3 != it2)
		tile.AddPathSprite(& GetSprite(Direction::Get(from, *it1), Direction::Get(*it1, *it3)));
	    else
		tile.AddPathSprite(& AGG::GetICN("ROUTE.ICN", 0));
	    
	    tile.Redraw();

	    from = *it1;
	}

	if(H2Config::Debug()) Dump();
    }
}

void Route::Dump(void) const
{
    Error::Verbose("route start index: ", Maps::GetIndexFromAbsPoint(hero.GetCenter()));

    std::vector<u16>::const_iterator it1 = path.begin();
    std::vector<u16>::const_iterator it2 = path.end();

    u16 from = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    // dump route
    for(; it1 != it2; ++it1)
    {
	Error::Verbose("route: " + Direction::String(Direction::Get(from, *it1)));

	from = *it1;
    }

    Error::Verbose("route end index: ", dst);
    Error::Verbose("route size: ", path.size());
}

void Route::Hide(void) const
{
    if(path.size())
    {
	// redraw tiles
	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	for(; it1 != it2; ++it1)
	{
	    Maps::Tiles & tile = world.GetTiles(*it1);

	    tile.DelPathSprite();
	    tile.Redraw();
	}
    }
}

void Route::Reset(void)
{
    Hide();

    dst = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    path.clear();
}

const Sprite & Route::GetSprite(const Direction::vector_t & from, const Direction::vector_t & to)
{
    // start index 1, 25, 49, 73, 97, 121 (size arrow path)
    u16 index = 1;

    switch(from)
    {
	case Direction::TOP:
	    switch(to)
	    {
		case Direction::TOP:		index +=  8; break;
		case Direction::TOP_RIGHT:	index += 17; break;
		case Direction::RIGHT:		index += 18; break;
		case Direction::LEFT:		index +=  6; break;
		case Direction::TOP_LEFT:	index +=  7; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::TOP_RIGHT:
	    switch(to)
	    {
		case Direction::TOP:		index +=  0; break;
		case Direction::TOP_RIGHT:	index +=  9; break;
		case Direction::RIGHT:		index += 18; break;
		case Direction::BOTTOM_RIGHT:	index += 19; break;
		case Direction::TOP_LEFT:	index +=  7; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::RIGHT:
	    switch(to)
	    {
		case Direction::TOP:		index +=  0; break;
		case Direction::BOTTOM:		index += 20; break;
		case Direction::BOTTOM_RIGHT:	index += 19; break;
		case Direction::RIGHT:		index += 10; break;
		case Direction::TOP_RIGHT:	index +=  1; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::BOTTOM_RIGHT:
	    switch(to)
	    {
		case Direction::TOP_RIGHT:	index +=  1; break;
		case Direction::RIGHT:		index +=  2; break;
		case Direction::BOTTOM_RIGHT:	index += 11; break;
		case Direction::BOTTOM:		index += 20; break;
		case Direction::BOTTOM_LEFT:	index += 21; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::BOTTOM:
	    switch(to)
	    {
		case Direction::RIGHT:		index +=  2; break;
		case Direction::BOTTOM_RIGHT:	index +=  3; break;
		case Direction::BOTTOM:		index += 12; break;
		case Direction::BOTTOM_LEFT:	index += 21; break;
		case Direction::LEFT:		index += 22; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::BOTTOM_LEFT:
	    switch(to)
	    {
		case Direction::BOTTOM_RIGHT:	index +=  3; break;
		case Direction::BOTTOM:		index +=  4; break;
		case Direction::BOTTOM_LEFT:	index += 13; break;
		case Direction::LEFT:		index += 22; break;
		case Direction::TOP_LEFT:	index += 23; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::LEFT:
	    switch(to)
	    {
		case Direction::TOP:		index += 16; break;
		case Direction::BOTTOM:		index +=  4; break;
		case Direction::BOTTOM_LEFT:	index +=  5; break;
		case Direction::LEFT:		index += 14; break;
		case Direction::TOP_LEFT:	index += 23; break;
		default: 			index  =  0; break;
	    }
	    break;

	case Direction::TOP_LEFT:
	    switch(to)
	    {
		case Direction::TOP:		index += 16; break;
		case Direction::TOP_RIGHT:	index += 17; break;
		case Direction::BOTTOM_LEFT:	index +=  5; break;
		case Direction::LEFT:		index +=  6; break;
		case Direction::TOP_LEFT:	index += 15; break;
		default: 			index  =  0; break;
	    }
	    break;

	default: 		   	        index  =  0; break;
    }

    return AGG::GetICN("ROUTE.ICN", index);
}
