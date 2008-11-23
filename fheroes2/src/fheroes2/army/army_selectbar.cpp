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
#include "text.h"
#include "army.h"

Army::SelectBar::SelectBar(const Point & pos, const Army::army_t & troops)
    : pos_pt(pos), empty_back(AGG::GetICN(ICN::STRIP, 2)), step(6), cursor(AGG::GetICN(ICN::STRIP, 1), pos),
    army(troops), selected(false), cursor_index(0xFF), coords(army.size())
{
    for(u8 ii = 0; ii < army.size(); ++ii)
        coords[ii] = Rect(pos_pt.x + (empty_back.w() + step) * ii, pos_pt.y, empty_back.w(), empty_back.h());
}

void Army::SelectBar::Reset(void)
{
    cursor_index = 0xFF;

    selected = false;
    
    cursor.Hide();
}

void Army::SelectBar::Redraw(const u8 alpha)
{
    Display & display = Display::Get();
    
    // redraw monster or background
    for(u8 ii = 0; ii < army.size(); ++ii)
	if(Army::isValid(army.at(ii)))
	{
	    Monster::monster_t monster = army.at(ii).Monster();

	    u8 index_sprite = 0xFF;

	    switch(Monster::GetRace(monster))
	    {
		case Race::KNGT: index_sprite = 4;  break;
		case Race::BARB: index_sprite = 5;  break;
		case Race::SORC: index_sprite = 6;  break;
		case Race::WRLK: index_sprite = 7;  break;
		case Race::WZRD: index_sprite = 8;  break;
		case Race::NECR: index_sprite = 9;  break;
		case Race::BOMG: index_sprite = 10; break;
		default: Error::Warning("Army::SelectBar::Redraw: unknown race."); return;
	    }

	    const Surface & monster_back = AGG::GetICN(ICN::STRIP, index_sprite);
	    const Sprite & monster_sprite = AGG::GetICN(Monster::GetStats(monster).monh_icn, 0);

	    Point dst_pt;

	    dst_pt.x = pos_pt.x + (monster_back.w() + step) * ii;
	    dst_pt.y = pos_pt.y;

	    if(alpha != 0xFF)
	    {
		Surface sf(monster_back.w(), monster_back.h());
		sf.SetColorKey();

		// monster background
		sf.Blit(monster_back);
		sf.Blit(monster_sprite, monster_sprite.x(), monster_sprite.y());

		dst_pt.x = pos_pt.x + (monster_back.w() + step) * ii;
		dst_pt.y = pos_pt.y;

		sf.SetAlpha(alpha);
		display.Blit(sf, dst_pt);
	    }
	    else
	    {
		// monster background
		display.Blit(monster_back, dst_pt);

		dst_pt.x = pos_pt.x + (monster_back.w() + step) * ii + monster_sprite.x();
		dst_pt.y = pos_pt.y + monster_sprite.y();

		display.Blit(monster_sprite, dst_pt);

		// draw count
		std::string str;
		String::AddInt(str, army.at(ii).Count());
		dst_pt.x = pos_pt.x + (monster_back.w() + step) * ii + 68  - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = pos_pt.y + 80;
		Text(str, Font::SMALL, dst_pt);
	    }
	}
	else
	    display.Blit(empty_back, pos_pt.x + (empty_back.w() + step) * ii, pos_pt.y);

    // draw cursor
    if(selected && cursor_index < coords.size()) cursor.Show(coords[cursor_index].x, coords[cursor_index].y);
}

void Army::SelectBar::Select(u8 index)
{
    if(index < coords.size())
    {
	selected = true;

	cursor_index = index;
    }
}
