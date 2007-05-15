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

Game::SelectFocusObject::SelectFocusObject() : active(false), cursor_index(0),
    first_icons_index(0), spriteCursor(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT, true),
    cursor(spriteCursor, Rect(-1, -1, ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT))
{
    // draw frame cursor
    spriteCursor.SetColorKey();

    u32 color = AGG::GetColor(ICONS_CURSOR_COLOR);
    spriteCursor.Lock();

    for(u8 ii = 0; ii < spriteCursor.w(); ++ii)
    {
        spriteCursor.SetPixel2(ii, 0, color);
        spriteCursor.SetPixel2(ii, spriteCursor.h() - 1, color);
    }

    for(u8 ii = 0; ii < spriteCursor.h(); ++ii)
    {
        spriteCursor.SetPixel2(0, ii, color);
        spriteCursor.SetPixel2(spriteCursor.w() - 1, ii, color);
    }

    spriteCursor.Unlock();
    spriteCursor.SetDisplayFormat();
}

void Game::SelectFocusObject::HideCursor(void)
{
    if(! active) return;
    
    active = false;

    cursor.Hide();
}

void Game::SelectFocusObject::ShowCursor(void)
{

    if(first_icons_index <= cursor_index && first_icons_index + coords.size() > cursor_index)
    {
	active = true;

	Point dst_pt(coords[cursor_index - first_icons_index]);
	dst_pt.x -= 5;
	dst_pt.y -= 5;
	cursor.Show(dst_pt);
    }
}

void Game::SelectFocusObject::MoveCursor(u8 index_icon)
{
    if(index_icon < coords.size())
    {
	Point dst_pt(coords[index_icon]);
	dst_pt.x -= 5;
	dst_pt.y -= 5;
	active ? cursor.Move(dst_pt) : cursor.Show(dst_pt);
	
	cursor_index = first_icons_index + index_icon;

	active = true;
    }
}

Game::SelectFocusCastles::SelectFocusCastles(const std::vector<Castle *> & vec) : castles(vec)
{
    u8 count_icons = 0;

    switch(H2Config::GetVideoMode())
    {
	// 640
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

    const Point top_pt(display.w() - RADARWIDTH - BORDERWIDTH + 77, RADARWIDTH + BORDERWIDTH + 21);

    for(u8 ii = 0; ii < count_icons; ++ii)
	coords.push_back(Rect(top_pt.x, top_pt.y + ii * 32, ICONS_WIDTH, ICONS_HEIGHT));

    max.x = top_pt.x;
    max.y = top_pt.y;
    max.w = ICONS_WIDTH;
    max.h = coords.back().y + ICONS_HEIGHT;
}

bool Game::SelectFocusCastles::Prev(void)
{
    if(first_icons_index)
    {
	--first_icons_index;

	return true;
    }

    return false;
}

bool Game::SelectFocusCastles::Next(void)
{
    if(first_icons_index + coords.size() < castles.size())
    {
	++first_icons_index;

	return true;
    }

    return false;
}

void Game::SelectFocusCastles::Redraw(void)
{
    const std::string & locators = (H2Config::EvilInterface() ? "LOCATORE.ICN" : "LOCATORS.ICN");

    HideCursor();

    for(u8 ii = 0; ii < coords.size(); ++ii)
    {
	u8 index_sprite = 0;

	// select castle icons
	if(first_icons_index + ii < castles.size())
	{
	    const Castle & castle = *castles[first_icons_index + ii];

	    switch(castle.GetRace())
	    {
		case Race::KNGT: index_sprite = castle.isCastle() ?  9 : 15; break;
		case Race::BARB: index_sprite = castle.isCastle() ? 10 : 16; break;
		case Race::SORC: index_sprite = castle.isCastle() ? 11 : 17; break;
		case Race::WRLK: index_sprite = castle.isCastle() ? 12 : 18; break;
		case Race::WZRD: index_sprite = castle.isCastle() ? 13 : 19; break;
		case Race::NECR: index_sprite = castle.isCastle() ? 14 : 20; break;
		default: Error::Warning("Game::SelectFocusCastles::Redraw: unknown race."); return;
	    }

	}
	// select background icons
	else index_sprite = 1;

	display.Blit(AGG::GetICN(locators, index_sprite), coords[ii]);
    }

    ShowCursor();
}

const Point & Game::SelectFocusCastles::GetCenter(u8 index_icon)
{
    if(! Valid(index_icon)) Error::Except("Game::SelectFocusCastles::GetCenter: out of range.");
    
    return (*castles[index_icon + first_icons_index]).GetCenter();
}

/* set focus panel to center object */
void Game::SelectFocusCastles::SetCenter(const Point & pt)
{
    for(u8 ii = 0; ii < castles.size(); ++ii)
	if(pt == (*castles[ii]).GetCenter())
	{
	    cursor_index = ii;
	    first_icons_index = (cursor_index + coords.size() >= castles.size() && castles.size() > coords.size() ? castles.size() - coords.size() : ii);
	    return;
	}
}

Game::SelectFocusHeroes::SelectFocusHeroes(const std::vector<Heroes *> & vec) : heroes(vec)
{
    u8 count_icons = 0;

    switch(H2Config::GetVideoMode())
    {
	// 640
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
    const Point top_pt(display.w() - RADARWIDTH - BORDERWIDTH + 5, RADARWIDTH + BORDERWIDTH + 21);

    for(u8 ii = 0; ii < count_icons; ++ii)
	coords.push_back(Rect(top_pt.x, top_pt.y + ii * 32, ICONS_WIDTH, ICONS_HEIGHT));

    max.x = top_pt.x;
    max.y = top_pt.y;
    max.w = ICONS_WIDTH;
    max.h = coords.back().y + ICONS_HEIGHT;;
}

bool Game::SelectFocusHeroes::Prev(void)
{
    return false;
}

bool Game::SelectFocusHeroes::Next(void)
{
    return false;
}

void Game::SelectFocusHeroes::Redraw(void)
{
}

const Point & Game::SelectFocusHeroes::GetCenter(u8 index_icon)
{
    if(! Valid(index_icon)) Error::Except("Game::SelectFocusHeroes::GetCenter: out of range.");
    
    return (*heroes[index_icon + first_icons_index]).GetCenter();
}

void Game::SelectFocusHeroes::SetCenter(const Point & pt)
{
}
