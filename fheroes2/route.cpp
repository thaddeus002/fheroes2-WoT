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

void Route::FromHeroes(const Heroes *from)
{
    hero = from;
}

// return length path
u16 Route::Calculate(u16 dst_index)
{
    if(! hero) return 0;

    if(dst != dst_index)
    {
	path.clear();

	Algorithm::PathFinding(world.GetTiles(hero->GetCenter()).GetIndex(), dst_index, hero->GetLevelSkill(Skill::PATHFINDING), path);

	dst = dst_index;
    }

    return path.size();
}

void Route::Show(void)
{
    if(hero)
    {
	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	u16 from = Maps::GetIndexFromAbsPoint(hero->GetCenter());

	for(; it1 != it2; ++it1)
	{
	    const Sprite & sprite = (it1 == it2 ? GetSprite(Direction::CENTER, Direction::CENTER) : GetSprite(Direction::Get(from, *it1), Direction::Get(*it1, *(it1 + 1))));

	    Maps::Tiles & tile = world.GetTiles(*it1);

	    tile.AddExtraSprite(&sprite);
	    tile.Redraw();

	    from = *it1;
	}
    }
}

void Route::Dump(void)
{
    if(hero && H2Config::Debug())
    {
	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	u16 from = Maps::GetIndexFromAbsPoint(hero->GetCenter());

	// dump route
	for(; it1 != it2; ++it1)
	{
	    Error::Verbose("route: " + Direction::String(Direction::Get(from, *it1)));

	    from = *it1;
	}
    }

    Error::Verbose("route: end");
}

const Sprite & Route::GetSprite(const Direction::vector_t & from, const Direction::vector_t & to)
{
    u16 index = 0;

    switch(from)
    {
	case Direction::TOP:
	    switch(to)
	    {
		case Direction::TOP:		index +=  9; break;
		case Direction::TOP_RIGHT:	index += 18; break;
		case Direction::RIGHT:		index += 19; break;
		case Direction::LEFT:		index +=  7; break;
		case Direction::TOP_LEFT:	index +=  8; break;
		default: break;
	    }
	    break;

	case Direction::TOP_RIGHT:
	    switch(to)
	    {
		case Direction::TOP:		index +=  1; break;
		case Direction::TOP_RIGHT:	index += 10; break;
		case Direction::RIGHT:		index += 19; break;
		case Direction::BOTTOM_RIGHT:	index += 20; break;
		case Direction::TOP_LEFT:	index +=  8; break;
		default: break;
	    }
	    break;

	case Direction::RIGHT:
	    switch(to)
	    {
		case Direction::TOP:		index +=  1; break;
		case Direction::BOTTOM:		index += 21; break;
		case Direction::BOTTOM_RIGHT:	index += 20; break;
		case Direction::RIGHT:		index += 11; break;
		case Direction::TOP_RIGHT:	index += 2; break;
		default: break;
	    }
	    break;

	case Direction::BOTTOM_RIGHT:
	    switch(to)
	    {
		case Direction::TOP_RIGHT:	index +=  2; break;
		case Direction::RIGHT:		index +=  3; break;
		case Direction::BOTTOM_RIGHT:	index += 12; break;
		case Direction::BOTTOM:		index += 21; break;
		case Direction::BOTTOM_LEFT:	index += 22; break;
		default: break;
	    }
	    break;

	case Direction::BOTTOM:
	    switch(to)
	    {
		case Direction::RIGHT:		index +=  3; break;
		case Direction::BOTTOM_RIGHT:	index +=  4; break;
		case Direction::BOTTOM:		index += 13; break;
		case Direction::BOTTOM_LEFT:	index += 22; break;
		case Direction::LEFT:		index += 23; break;
		default: break;
	    }
	    break;

	case Direction::BOTTOM_LEFT:
	    switch(to)
	    {
		case Direction::BOTTOM_RIGHT:	index +=  4; break;
		case Direction::BOTTOM:		index +=  5; break;
		case Direction::BOTTOM_LEFT:	index += 14; break;
		case Direction::LEFT:		index += 23; break;
		case Direction::TOP_LEFT:	index += 24; break;
		default: break;
	    }
	    break;

	case Direction::LEFT:
	    switch(to)
	    {
		case Direction::TOP:		index += 17; break;
		case Direction::BOTTOM:		index +=  5; break;
		case Direction::BOTTOM_LEFT:	index +=  6; break;
		case Direction::LEFT:		index += 15; break;
		case Direction::TOP_LEFT:	index += 24; break;
		default: break;
	    }
	    break;

	case Direction::TOP_LEFT:
	    switch(to)
	    {
		case Direction::TOP:		index += 17; break;
		case Direction::TOP_RIGHT:	index += 18; break;
		case Direction::BOTTOM_LEFT:	index +=  6; break;
		case Direction::LEFT:		index +=  7; break;
		case Direction::TOP_LEFT:	index += 16; break;
		default: break;
	    }
	    break;

	default: break;
    }

    return AGG::GetICN("ROUTE.ICN", index);
}

void Route::Hide(void)
{
    if(path.size())
    {
	// redraw tiles
	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	for(; it1 != it2; ++it1)
	{
	    Maps::Tiles & tile = world.GetTiles(*it1);

	    tile.DelExtraSprite();
	    tile.Redraw();
	}
    }
}

void Route::Reset(void)
{
    if(hero) Hide();

    hero = NULL;
    dst = MAXU16;

    path.clear();
}
