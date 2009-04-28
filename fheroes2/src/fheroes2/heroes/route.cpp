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

#include <iostream>
#include "agg.h"
#include "world.h"
#include "heroes.h"
#include "algorithm.h"
#include "direction.h"
#include "maps.h"
#include "game.h"
#include "settings.h"
#include "route.h"

/* construct */
Route::Path::Path(const Heroes & h)
    : hero(h), dst(Maps::GetIndexFromAbsPoint(h.GetCenter())), hide(true)
{
}

Direction::vector_t Route::Path::GetFrontDirection(void) const
{
    return size() ? front().Direction() : Direction::CENTER;
}

u16 Route::Path::GetFrontPenalty(void) const
{
    return size() ? front().Penalty() : 0;
}

void Route::Path::PopFront(void)
{
    if(size()) pop_front();
}

/* return length path */
u16 Route::Path::Calculate(const u16 dst_index, const u16 limit)
{
    clear();

    Algorithm::PathFind(this, hero.GetIndex(), dst_index, limit, &hero);

    dst = dst_index;

    return size();
}

void Route::Path::Reset(void)
{
    if(empty()) return;

    dst = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    clear();

    hide = true;
}

u16 Route::Path::GetIndexMultiplier(u16 penalty)
{
    return penalty / 25 - 3;
}

u16 Route::Path::GetIndexSprite(const Direction::vector_t & from, const Direction::vector_t & to)
{
    // ICN::ROUTE
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
		case Direction::BOTTOM_LEFT:	index +=  5; break;
		case Direction::BOTTOM_RIGHT:	index += 19; break;
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
		case Direction::BOTTOM:		index += 20; break;
		case Direction::LEFT:		index +=  6; break;
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
		case Direction::TOP_LEFT:	index +=  7; break;
		case Direction::BOTTOM_LEFT:	index += 21; break;
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
		case Direction::TOP:		index +=  0; break;
		case Direction::LEFT:		index +=  6; break;
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
		case Direction::TOP_LEFT:	index += 16; break;
		case Direction::TOP_RIGHT:	index +=  1; break;
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
		case Direction::TOP:		index += 16; break;
		case Direction::RIGHT:		index +=  2; break;
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
		case Direction::TOP_RIGHT:	index += 17; break;
		case Direction::BOTTOM_RIGHT:	index +=  3; break;
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
		case Direction::BOTTOM:		index +=  4; break;
		case Direction::RIGHT:		index += 18; break;
		default: 			index  =  0; break;
	    }
	    break;

	default: 		   	        index  =  0; break;
    }

    return index;
}

/* total penalty cast */
u32 Route::Path::TotalPenalty(void) const
{
    u32 result = 0;

    const_iterator it1 = begin();
    const_iterator it2 = end();

    for(; it1 != it2; ++it1) result += (*it1).Penalty();

    return result;
}

u16 Route::Path::GetAllowStep(void) const
{
    u16 green = 0;

    const_iterator it1 = begin();
    const_iterator it2 = end();

    u16 move_point = hero.GetMovePoints();

    for(; it1 != it2; ++it1)
    {
	if(move_point >= (*it1).Penalty())
	{
	    move_point -= (*it1).Penalty();
	    ++green;
	}
	else break;
    }

    return green;
}

void Route::Path::Dump(void) const
{
    std::cout << "Path::Dump: from: " << hero.GetIndex() << ", to: " << dst << ", obj: " << MP2::StringObject(world.GetTiles(dst).GetObject()) << std::endl;

    const_iterator it1 = begin();
    const_iterator it2 = end();

    for(; it1 != it2; ++it1) std::cout << "Path::Dump: " << Direction::String((*it1).Direction()) << ", " << (*it1).Penalty() << std::endl;
}

bool Route::Path::isUnderProtection(u16 & res) const
{
    const_iterator it1 = begin();
    const_iterator it2 = end();

    u16 next = hero.GetIndex();

    for(; it1 != it2; ++it1)
    {
	if(Maps::isValidDirection(next, (*it1).Direction()))
	    next = Maps::GetDirectionIndex(next, (*it1).Direction());

	if(Maps::TileUnderProtection(next, &res))  return true;
    }

    return false;
}

bool Route::Path::isUnderProtection(void) const
{
    const_iterator it1 = begin();
    const_iterator it2 = end();

    u16 next = dst;

    for(; it1 != it2; ++it1)
    {
	if(Maps::isValidDirection(next, (*it1).Direction()))
	    next = Maps::GetDirectionIndex(next, (*it1).Direction());

	if(Maps::TileUnderProtection(next)) return true;
    }

    return false;
}
