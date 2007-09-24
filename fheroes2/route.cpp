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
#include "game.h"
#include "gamearea.h"
#include "route.h"

Route::Route(const Heroes & from) : hero(from)
{
}

// return length path
u16 Route::Calculate(u16 dst_index)
{
    if(path.size()) path.clear();

    return Algorithm::PathFinding(world.GetTiles(hero.GetCenter()).GetIndex(), dst_index, hero.GetLevelSkill(Skill::PATHFINDING), path);
}

void Route::Blit(void)
{
    // sprites in ROUTE.ICN
}

void Route::Hide(void)
{
    if(path.size())
    {
	// redraw tiles
	std::vector<u16>::const_iterator it1 = path.begin();
	std::vector<u16>::const_iterator it2 = path.end();

	const Rect & area = GameArea::GetRect();
	const u16 index_lt = GameArea::GetLeftTopIndexMaps();
	
	for(; it1 != it2; ++it1)
	{
	    bool enditer = false;

	    for(u16 iy = 0; iy < area.h; ++iy)
	    {
		if(enditer) break;

		for(u16 ix = 0; iy < area.w; ++iy)
		{
		    if(enditer) break;

		    if(index_lt + (world.w() * iy) + ix == *it1)
		    {
			world.GetTiles(*it1).Blit(BORDERWIDTH + TILEWIDTH * ix, BORDERWIDTH + TILEWIDTH * iy);
			
			enditer = true;
		    }
		}
	    }
	}
    }
}
