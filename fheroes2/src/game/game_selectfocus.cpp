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
#include "sprite.h"
#include "portrait.h"
#include "game_focus.h"
#include "game_selectfocus.h"

#define ICONS_WIDTH		46
#define ICONS_HEIGHT		22
#define ICONS_CURSOR_WIDTH	56
#define ICONS_CURSOR_HEIGHT	32
#define ICONS_CURSOR_COLOR	0x98

Game::SelectFocusObject::SelectFocusObject(s16 px, s16 py) : pos_pt(px, py),
    empty_back(AGG::GetICN(H2Config::EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, 1)),
    step(32), sprite_cursor(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT), cursor(sprite_cursor, px, py),
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

    u8 count_icons = 0;

    switch(H2Config::VideoMode())
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

    coords.resize(count_icons);

    for(u8 ii = 0; ii < count_icons; ++ii)
        coords[ii] = Rect(pos_pt.x, pos_pt.y + ii * step, ICONS_WIDTH, ICONS_HEIGHT);

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

bool Game::SelectFocusObject::SetTop(u8 index)
{
    if(index <= GetSizeObject() - coords.size())
    {
	top_index = index;

	return true;
    }

    return false;
}

Game::SelectFocusCastles::SelectFocusCastles()
    : SelectFocusObject(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 77, RADARWIDTH + BORDERWIDTH + 21), castles(NULL)
{}

Game::SelectFocusCastles & Game::SelectFocusCastles::Get(void)
{
    static SelectFocusCastles instance;

    return instance;
}

void Game::SelectFocusCastles::SetCastles(const std::vector<Castle *> & vec)
{
    castles = & vec;
}

void Game::SelectFocusCastles::Redraw(void)
{
    if(! castles) return;

    Display & display = Display::Get();
    cursor.Hide();

    // redraw back
    for(u8 ii = 0; ii < coords.size(); ++ii)
	// draw icons object
	if(top_index + ii < castles->size())
	{
	    const Castle & castle = *castles->at(top_index + ii);
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

	    display.Blit(AGG::GetICN(H2Config::EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, index_sprite), coords[ii].x, coords[ii].y);
	    
	    // castle build marker
	    if(! castle.AllowBuild())
		display.Blit(AGG::GetICN(H2Config::EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, 24), coords[ii].x + 39, coords[ii].y + 1);
	}
	// redraw background
	else
	    display.Blit(empty_back, coords[ii].x, coords[ii].y);

    // redraw cursor
    if(selected && cursor_index >= top_index && cursor_index < top_index + coords.size())
	cursor.Show(coords[cursor_index - top_index].x - 5, coords[cursor_index - top_index].y - 5);
}

/* select from focus */
void Game::SelectFocusCastles::Select(void)
{
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::CASTLE != focus.Type() || NULL == castles) return;

    for(u8 ii = 0; ii < castles->size(); ++ii) if(&focus.GetCastle() == castles->at(ii))
    {
	cursor_index = ii;

	selected = true;

	break;
    }
}

void Game::SelectFocusCastles::Select(u8 index)
{
    SelectFocusObject::Select(index);
}

const Point & Game::SelectFocusCastles::GetCenter(u8 index) const
{
    if(! castles) Error::Except("Game::SelectFocusCastles::GetCenter: castles is NULL.");

    return (*castles->at(top_index + index)).GetCenter();
}

Game::SelectFocusHeroes::SelectFocusHeroes()
    : SelectFocusObject(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 5, RADARWIDTH + BORDERWIDTH + 21), heroes(NULL),
    sprite_blue(7, ICONS_HEIGHT, true)
{
    // fill backgroung to blue
    sprite_blue.Fill(AGG::GetColor(0x4C));
}

Game::SelectFocusHeroes & Game::SelectFocusHeroes::Get(void)
{
    static SelectFocusHeroes instance;

    return instance;
}

void Game::SelectFocusHeroes::SetHeroes(const std::vector<Heroes *> & vec)
{
    heroes = & vec;
}

void Game::SelectFocusHeroes::Redraw(void)
{
    if(! heroes) return;

    Display & display = Display::Get();
    cursor.Hide();

    // redraw back
    for(u8 ii = 0; ii < coords.size(); ++ii)
	// draw icons object
	if(top_index + ii < heroes->size())
	{
	    const Heroes & hero = *heroes->at(top_index + ii);
	    
	    const Sprite & mobility = AGG::GetICN(ICN::MOBILITY, hero.GetMobilityIndexSprite());
	    const Sprite & mana = AGG::GetICN(ICN::MANA, hero.GetManaIndexSprite());
	    const Surface & port = Portrait::Hero(hero.GetHeroes(), Portrait::SMALL);

	    display.FillRect(0, 0, 0, coords[ii]);

	    // mobility
	    display.Blit(sprite_blue, coords[ii].x, coords[ii].y);
	    display.Blit(mobility, coords[ii].x, coords[ii].y + mobility.y());

	    // portrait
	    display.Blit(port, coords[ii].x + sprite_blue.w() + 1, coords[ii].y);

	    // mana
	    display.Blit(sprite_blue, coords[ii].x + sprite_blue.w() + port.w() + 2, coords[ii].y);
	    display.Blit(mana, coords[ii].x + sprite_blue.w() + port.w() + 2, coords[ii].y + mana.y());
	}
	// redraw background
	else
	    display.Blit(empty_back, coords[ii].x, coords[ii].y);

    // redraw cursor
    if(selected && cursor_index >= top_index && cursor_index < top_index + coords.size())
	cursor.Show(coords[cursor_index - top_index].x - 5, coords[cursor_index - top_index].y - 5);
}

/* select from focus */
void Game::SelectFocusHeroes::Select(void)
{
    const Game::Focus & focus = Game::Focus::Get();

    if(Game::Focus::HEROES != focus.Type() || NULL == heroes) return;

    for(u8 ii = 0; ii < heroes->size(); ++ii) if(&focus.GetHeroes() == heroes->at(ii))
    {
	cursor_index = ii;

	selected = true;

	break;
    }
}

void Game::SelectFocusHeroes::Select(u8 index)
{
    SelectFocusObject::Select(index);
}

const Point & Game::SelectFocusHeroes::GetCenter(u8 index) const
{
    if(! heroes) Error::Except("Game::SelectFocusHeroes::GetCenter: heroes is NULL.");

    return (*heroes->at(top_index + index)).GetCenter();
}
