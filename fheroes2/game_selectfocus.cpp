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

#include "castle.h"
#include "heroes.h"
#include "error.h"
#include "race.h"
#include "config.h"
#include "agg.h"
#include "game_selectfocus.h"

#define ICONS_WIDTH		46
#define ICONS_HEIGHT		22
#define ICONS_CURSOR_WIDTH	56
#define ICONS_CURSOR_HEIGHT	32
#define ICONS_CURSOR_COLOR	0x98

Game::SelectFocusObject::SelectFocusObject(const Point & pt) :pos_pt(pt),
    empty_back(AGG::GetICN(H2Config::EvilInterface() ? "LOCATORE.ICN" : "LOCATORS.ICN", 1)),
    step(32), sprite_cursor(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT, true), cursor(sprite_cursor, pt),
    selected(false), top_index(0), cursor_index(0xFF) 
{
    // draw frame cursor
    sprite_cursor.SetColorKey();

    u32 color = AGG::GetColor(ICONS_CURSOR_COLOR);
    sprite_cursor.Lock();

    for(u8 ii = 0; ii < sprite_cursor.w(); ++ii)
    {
        sprite_cursor.SetPixel2(ii, 0, color);
        sprite_cursor.SetPixel2(ii, sprite_cursor.h() - 1, color);
    }

    for(u8 ii = 0; ii < sprite_cursor.h(); ++ii)
    {
        sprite_cursor.SetPixel2(0, ii, color);
        sprite_cursor.SetPixel2(sprite_cursor.w() - 1, ii, color);
    }

    sprite_cursor.Unlock();
    sprite_cursor.SetDisplayFormat();

    u8 count_icons = 0;

    switch(H2Config::GetVideoMode())
    {
        default:
            count_icons = 4;
            break;

        case Display::MEDIUM:
            count_icons = 7;
            break;

        case Display::LARGE:
        case Display::XLARGE:
            count_icons = 8;
            break;
    }

    for(u8 ii = 0; ii < count_icons; ++ii)
        coords.push_back(Rect(pos_pt.x, pos_pt.y + ii * step, ICONS_WIDTH, ICONS_HEIGHT));

    max_area.x = pos_pt.x;
    max_area.y = pos_pt.y;
    max_area.w = ICONS_WIDTH;
    max_area.h = coords.back().y + ICONS_HEIGHT;
}

void Game::SelectFocusObject::Reset(void)
{
    selected = false;

    top_index = 0;

    cursor_index = 0xFF;

    cursor.Hide();
}

void Game::SelectFocusObject::Select(u8 index)
{
    if(top_index + index < GetSizeObject())
    {
        selected = true;

        cursor_index = top_index + index;
    }
}


bool Game::SelectFocusObject::Prev(void)
{
    if(top_index)
    {
	--top_index;

	return true;
    }

    return false;
}

bool Game::SelectFocusObject::Next(void)
{
    if(top_index < GetSizeObject() - coords.size())
    {
	++top_index;

	return true;
    }

    return false;
}
	
Game::SelectFocusCastles::SelectFocusCastles(const std::vector<Castle *> & vec)
    : SelectFocusObject(Point(display.w() - RADARWIDTH - BORDERWIDTH + 77, RADARWIDTH + BORDERWIDTH + 21)), castles(vec)
{}

void Game::SelectFocusCastles::Redraw(void)
{
    cursor.Hide();

    // redraw back
    for(u8 ii = 0; ii < coords.size(); ++ii)
	// draw icons object
	if(top_index + ii < castles.size())
	{
	    const Castle & castle = *castles[top_index + ii];
	    u8 index_sprite = 1;

	    switch(castle.GetRace())
	    {
		case Race::KNGT: index_sprite = castle.isCastle() ?  9 : 15; break;
		case Race::BARB: index_sprite = castle.isCastle() ? 10 : 16; break;
		case Race::SORC: index_sprite = castle.isCastle() ? 11 : 17; break;
		case Race::WRLK: index_sprite = castle.isCastle() ? 12 : 18; break;
		case Race::WZRD: index_sprite = castle.isCastle() ? 13 : 19; break;
		case Race::NECR: index_sprite = castle.isCastle() ? 14 : 20; break;
		default: Error::Warning("Game::SelectFocusCastles::Redraw: unknown race.");
	    }

	    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? "LOCATORE.ICN" : "LOCATORS.ICN", index_sprite), coords[ii]);
	}
	// redraw background
	else
	    display.Blit(empty_back, coords[ii]);

    // redraw cursor
    if(selected && cursor_index >= top_index && cursor_index < top_index + coords.size())
	cursor.Show(coords[cursor_index - top_index].x - 5, coords[cursor_index - top_index].y - 5);
}

void Game::SelectFocusCastles::SetCenter(const Point & pt)
{
    for(u8 ii = 0; ii < castles.size(); ++ii)
	if(pt == (*castles[ii]).GetCenter())
	{
	    top_index = (ii + coords.size() >= castles.size() && castles.size() > coords.size() ?
	                                                         castles.size() - coords.size() : ii);
	    cursor_index = ii;
	    
	    return;
	}
}

const Point & Game::SelectFocusCastles::GetCenter(const Point & pt) const
{
    return (*castles[cursor_index]).GetCenter();
}

Game::SelectFocusHeroes::SelectFocusHeroes(const std::vector<Heroes *> & vec)
    : SelectFocusObject(Point(display.w() - RADARWIDTH - BORDERWIDTH + 5, RADARWIDTH + BORDERWIDTH + 21)), heroes(vec)
{}

void Game::SelectFocusHeroes::Redraw(void)
{
    cursor.Hide();

    // redraw back
    for(u8 ii = 0; ii < coords.size(); ++ii)
	// draw icons object
	if(top_index + ii < heroes.size())
	{
	    const Heroes & hero = *heroes[top_index + ii];
	    
	    const Sprite & mobility = AGG::GetICN("MOBILITY.ICN", hero.GetMobilityIndexSprite());
	    const Sprite & mana = AGG::GetICN("MANA.ICN", hero.GetManaIndexSprite());
	    const Sprite & port = AGG::GetICN("MINIPORT.ICN", hero.GetHeroes());
	    // mobility
	    display.Blit(mobility, coords[ii]);
	    // portrait
	    display.Blit(port, coords[ii].x + mobility.w() + 1, coords[ii].y);
	    // mana
	    display.Blit(mana, coords[ii].x + mobility.w() + port.w() + 2, coords[ii].y);
	}
	// redraw background
	else
	    display.Blit(empty_back, coords[ii]);

    // redraw cursor
    if(selected && cursor_index >= top_index && cursor_index < top_index + coords.size())
	cursor.Show(coords[cursor_index - top_index].x - 5, coords[cursor_index - top_index].y - 5);
}

void Game::SelectFocusHeroes::SetCenter(const Point & pt)
{
    for(u8 ii = 0; ii < heroes.size(); ++ii)
	if(pt == (*heroes[ii]).GetCenter())
	{
	    top_index = (ii + coords.size() >= heroes.size() && heroes.size() > coords.size() ?
	                                                        heroes.size() - coords.size() : ii);
	    cursor_index = ii;

	    return;
	}
}

const Point & Game::SelectFocusHeroes::GetCenter(const Point & pt) const
{
    return (*heroes[cursor_index]).GetCenter();
}
