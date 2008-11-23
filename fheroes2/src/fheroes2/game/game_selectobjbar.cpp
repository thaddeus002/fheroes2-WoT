/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include <algorithm>

#include "castle.h"
#include "heroes.h"
#include "race.h"
#include "settings.h"
#include "agg.h"
#include "cursor.h"
#include "world.h"
#include "portrait.h"
#include "game_focus.h"
#include "kingdom.h"
#include "game_selectobjbar.h"

#define ICONS_WIDTH		46
#define ICONS_HEIGHT		22
#define ICONS_CURSOR_WIDTH	56
#define ICONS_CURSOR_HEIGHT	32
#define ICONS_CURSOR_COLOR	0x98

void Game::SelectObjectBar::DrawCursor(Surface & sf)
{
    u32 color = AGG::GetColor(ICONS_CURSOR_COLOR);

    sf.Lock();

    for(u8 ii = 0; ii < ICONS_CURSOR_WIDTH; ++ii)
    {
        sf.SetPixel2(ii, 0, color);
        sf.SetPixel2(ii, ICONS_CURSOR_HEIGHT - 1, color);
    }

    for(u8 ii = 0; ii < ICONS_CURSOR_HEIGHT; ++ii)
    {
        sf.SetPixel2(0, ii, color);
        sf.SetPixel2(ICONS_CURSOR_WIDTH - 1, ii, color);
    }

    sf.Unlock();
}

Game::SelectObjectBar::SelectObjectBar(const s16 px, const s16 py) : pos_pt(px, py)
{
    const u8 count_h = (Display::Get().h() - 480) / TILEWIDTH;
    const u8 count_icons = count_h > 3 ? 8 : ( count_h < 3 ? 4 : 7);

    coords.resize(count_icons);

    for(u8 ii = 0; ii < count_icons; ++ii)
        coords[ii] = Rect(pos_pt.x, pos_pt.y + ii * ICONS_CURSOR_HEIGHT, ICONS_WIDTH, ICONS_HEIGHT);

    max_area.x = pos_pt.x;
    max_area.y = pos_pt.y;
    max_area.w = ICONS_WIDTH;
    max_area.h = coords.back().y + ICONS_HEIGHT;
}

void Game::SelectObjectBar::Redraw(void) const
{
    Display & display = Display::Get();
    const Sprite & back = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, 1);

    for(u8 ii = 0; ii < coords.size(); ++ii) display.Blit(back, coords[ii].x, coords[ii].y);
}

const std::vector<Rect> & Game::SelectObjectBar::GetCoords(void) const
{
    return coords;
}

const Rect & Game::SelectObjectBar::GetArea(void) const
{
    return max_area;
}

Game::SelectBarHeroes::SelectBarHeroes() 
    : SelectObjectBar(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 5, RADARWIDTH + BORDERWIDTH + 21), visible(false)
{
    sf_cursor.Set(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT);
    sf_cursor.SetColorKey();
    DrawCursor(sf_cursor);

    sp_cursor.Save(pos_pt);
    sp_cursor.SetSprite(sf_cursor);

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetArea(Rect(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 60, RADARWIDTH + 2 * BORDERWIDTH + 18, 10, 32 * coords.size() - 35));
    splitter.SetOrientation(Splitter::VERTICAL);
    splitter.SetRange(0, 0);
    splitter.Move(0);
}

Game::SelectBarHeroes & Game::SelectBarHeroes::Get(void)
{
    static SelectBarHeroes instance;

    return instance;
}

const Heroes * Game::SelectBarHeroes::Selected(void) const
{
    return Size() && visible ? *it_cur : NULL;
}

u32 Game::SelectBarHeroes::Size(void) const
{
    return world.GetMyKingdom().GetHeroes().size();
}

void Game::SelectBarHeroes::RedrawIcon(const Heroes & hero, const Rect & dst)
{
    Display & display = Display::Get();

    const Sprite & mobility = AGG::GetICN(ICN::MOBILITY, hero.GetMobilityIndexSprite());
    const Sprite & mana = AGG::GetICN(ICN::MANA, hero.GetManaIndexSprite());
    const Surface & port = Portrait::Hero(hero, Portrait::SMALL);

    display.FillRect(0, 0, 0, dst);

    Surface sf_blue(7, ICONS_HEIGHT, true);
    sf_blue.Fill(AGG::GetColor(0x4C));

    // mobility
    display.Blit(sf_blue, dst.x, dst.y);
    display.Blit(mobility, dst.x, dst.y + mobility.y());

    // portrait
    display.Blit(port, dst.x + sf_blue.w() + 1, dst.y);

    // mana
    display.Blit(sf_blue, dst.x + sf_blue.w() + port.w() + 2, dst.y);
    display.Blit(mana, dst.x + sf_blue.w() + port.w() + 2, dst.y + mana.y());
}

void Game::SelectBarHeroes::Redraw(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    if(visible) sp_cursor.Hide();
    splitter.Hide();
    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));

    SelectObjectBar::Redraw();

    std::vector<Heroes *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != heroes.end() && ii < coords.size())
    {
	if(*it)
	{
	    RedrawIcon(**it, coords[ii]);

	    // select current
	    if(visible && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);

	    ++ii;
	}
	++it;
    }
    splitter.Show();
}

void Game::SelectBarHeroes::Redraw(const Heroes *hero)
{
    if(NULL == hero) return;

    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    std::vector<Heroes *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != heroes.end() && ii < coords.size())
    {
	if(*it && *it == hero)
	{
	    if(visible && it_cur == it) sp_cursor.Hide();

	    RedrawIcon(**it, coords[ii]);

	    // select current
	    if(visible && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);

	    break;
	}

	if(*it) ++ii;
	++it;
    }
}

void Game::SelectBarHeroes::Hide(void)
{
    if(isSelected()) Unselect();
    SelectObjectBar::Redraw();
}

void Game::SelectBarHeroes::Unselect(void)
{
    if(visible)
    {
        sp_cursor.Hide();
	visible = false;
    }
}

void Game::SelectBarHeroes::Select(const Heroes *hero)
{
    if(NULL == hero) return;

    sp_cursor.Hide();

    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    std::vector<Heroes *>::iterator it = std::find(heroes.begin(), heroes.end(), hero);

    if(it == heroes.end()) return;

    const u8 i1 = it - it_top;
    const u8 i2 = it - heroes.begin();

    if(i1 < coords.size())
    {
	it_cur = it;
	sp_cursor.Show(coords[i1].x - 5, coords[i1].y - 5);
    }
    else
    if(i2 < coords.size())
    {
	it_top = heroes.begin();
	it_cur = it;
	sp_cursor.Show(coords[i2].x - 5, coords[i2].y - 5);
	splitter.Move(0);
    }
    else
    if(it >= heroes.end() - coords.size())
    {
	it_top = heroes.end() - coords.size();
	it_cur = it;
	sp_cursor.Show(coords[it_cur - it_top].x - 5, coords[it_cur - it_top].y - 5);
	splitter.Move(splitter.Max());
    }
    else
    {
	it_top = it;
	it_cur = it;
	sp_cursor.Show(coords[0].x - 5, coords[0].y - 5);
	splitter.Move(i2);
    }

    visible = true;
}

bool Game::SelectBarHeroes::isSelected(void) const
{
    return visible;
}

void Game::SelectBarHeroes::Reset(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    sp_cursor.Hide();

    it_top = heroes.begin();
    it_cur = it_top;
    visible = false;

    splitter.SetRange(0, heroes.size() > coords.size() ? heroes.size() - coords.size() : 0);
    splitter.Move(0);
}

bool Game::SelectBarHeroes::Prev(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    if(it_top != heroes.begin())
    {
	--it_top;
	Cursor::Get().Hide();
	splitter.Backward();
	Cursor::Get().Show();
	return true;
    }

    return false;
}

bool Game::SelectBarHeroes::Next(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    std::vector<Heroes *>::const_iterator it = heroes.end();

    if(heroes.size() > coords.size() && it_top != it - coords.size())
    {
	++it_top;
	Cursor::Get().Hide();
	splitter.Forward();
	Cursor::Get().Show();
	return true;
    }

    return false;
}

void Game::SelectBarHeroes::SetTop(const u8 index)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    if(heroes.size() > coords.size() && index > heroes.size() - coords.size())
	it_top = heroes.end() - coords.size();
    else
	it_top = heroes.begin() + index;
}

const Heroes * Game::SelectBarHeroes::MouseClickLeft(void) const
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Heroes *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != heroes.end() && ii < coords.size())
    {
	if(le.MouseClickLeft(coords[ii])) return *it;

	if(*it) ++ii;
	++it;
    }

    return NULL;
}

const Heroes * Game::SelectBarHeroes::MousePressRight(void) const
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Heroes *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != heroes.end() && ii < coords.size())
    {
	if(le.MousePressRight(coords[ii])) return *it;

	if(*it) ++ii;
	++it;
    }

    return NULL;
}

Splitter & Game::SelectBarHeroes::GetSplitter(void)
{
    return splitter;
}

Game::SelectBarCastle::SelectBarCastle()
    : SelectObjectBar(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 77, RADARWIDTH + BORDERWIDTH + 21), visible(false)
{
    sf_cursor.Set(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT);
    sf_cursor.SetColorKey();
    DrawCursor(sf_cursor);

    sp_cursor.Save(pos_pt);
    sp_cursor.SetSprite(sf_cursor);

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetArea(Rect(Display::Get().w() - RADARWIDTH - BORDERWIDTH + 132, RADARWIDTH + 2 * BORDERWIDTH + 18, 10, 32 * coords.size() - 35));
    splitter.SetOrientation(Splitter::VERTICAL);
    splitter.SetRange(0, 0);
}

Game::SelectBarCastle & Game::SelectBarCastle::Get(void)
{
    static SelectBarCastle instance;

    return instance;
}

const Castle * Game::SelectBarCastle::Selected(void) const
{
    return Size() && visible ? *it_cur : NULL;
}

u32 Game::SelectBarCastle::Size(void) const
{
    return world.GetMyKingdom().GetCastles().size();
}

void Game::SelectBarCastle::RedrawIcon(const Castle & castle, const Rect & dst)
{
    Display & display = Display::Get();

    const bool interface = Settings::Get().EvilInterface();
    u8 index_sprite = 1;

    switch(castle.GetRace())
    {
        case Race::KNGT: index_sprite = castle.isCastle() ?  9 : 15; break;
        case Race::BARB: index_sprite = castle.isCastle() ? 10 : 16; break;
        case Race::SORC: index_sprite = castle.isCastle() ? 11 : 17; break;
        case Race::WRLK: index_sprite = castle.isCastle() ? 12 : 18; break;
        case Race::WZRD: index_sprite = castle.isCastle() ? 13 : 19; break;
        case Race::NECR: index_sprite = castle.isCastle() ? 14 : 20; break;
        default: Error::Warning("Game::SelectBarCastle::Redraw: unknown race.");
    }

    display.Blit(AGG::GetICN(interface ? ICN::LOCATORE : ICN::LOCATORS, index_sprite), dst.x, dst.y);

    // castle build marker
    if(! castle.AllowBuild())
        display.Blit(AGG::GetICN(interface ? ICN::LOCATORE : ICN::LOCATORS, 24), dst.x + 39, dst.y + 1);
}

void Game::SelectBarCastle::Redraw(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();

    if(visible) sp_cursor.Hide();
    splitter.Hide();
    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));

    SelectObjectBar::Redraw();

    std::vector<Castle *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != castles.end() && ii < coords.size())
    {
	if(*it)
	{
    	    RedrawIcon(**it, coords[ii]);

	    // select current
	    if(visible && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);

	    ++ii;
	}
	++it;
    }
    splitter.Show();
}

void Game::SelectBarCastle::Redraw(const Castle *castle)
{
    if(NULL == castle) return;

    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    std::vector<Castle *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != castles.end() && ii < coords.size())
    {
	if(*it && *it == castle)
	{
	    if(visible && it_cur == it) sp_cursor.Hide();

    	    RedrawIcon(**it, coords[ii]);

	    // select current
	    if(visible && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);

	    break;
	}

	if(*it) ++ii;
	++it;
    }
}

void Game::SelectBarCastle::Hide(void)
{
    if(isSelected()) Unselect();
    SelectObjectBar::Redraw();
}

void Game::SelectBarCastle::Unselect(void)
{
    if(visible)
    {
        sp_cursor.Hide();
	visible = false;
    }
}

void Game::SelectBarCastle::Select(const Castle *castle)
{
    if(NULL == castle) return;

    sp_cursor.Hide();

    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    std::vector<Castle *>::iterator it = std::find(castles.begin(), castles.end(), castle);

    const u8 i1 = it - it_top;
    const u8 i2 = it - castles.begin();

    if(i1 < coords.size())
    {
	it_cur = it;
	sp_cursor.Show(coords[i1].x - 5, coords[i1].y - 5);
    }
    else
    if(i2 < coords.size())
    {
	it_top = castles.begin();
	it_cur = it;
	sp_cursor.Show(coords[i2].x - 5, coords[i2].y - 5);
	splitter.Move(0);
    }
    else
    if(it >= castles.end() - coords.size())
    {
	it_top = castles.end() - coords.size();
	it_cur = it;
	sp_cursor.Show(coords[it_cur - it_top].x - 5, coords[it_cur - it_top].y - 5);
	splitter.Move(splitter.Max());
    }
    else
    {
	it_top = it;
	it_cur = it;
	sp_cursor.Show(coords[0].x - 5, coords[0].y - 5);
	splitter.Move(i2);
    }

    visible = true;
}

bool Game::SelectBarCastle::isSelected(void) const
{
    return visible;
}

void Game::SelectBarCastle::Reset(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    sp_cursor.Hide();

    it_top = castles.begin();
    it_cur = it_top;
    visible = false;

    splitter.SetRange(0, castles.size() > coords.size() ? castles.size() - coords.size() : 0);
    splitter.Move(0);
}

bool Game::SelectBarCastle::Prev(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();

    if(it_top != castles.begin())
    {
	--it_top;
	Cursor::Get().Hide();
	splitter.Backward();
	Cursor::Get().Show();
	return true;
    }
    
    return false;
}

bool Game::SelectBarCastle::Next(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    std::vector<Castle *>::const_iterator it = castles.end();

    if(castles.size() > coords.size() && it_top != it - coords.size())
    {
	++it_top;
	Cursor::Get().Hide();
	splitter.Forward();
	Cursor::Get().Show();
	return true;
    }

    return false;
}

void Game::SelectBarCastle::SetTop(const u8 index)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    
    if(castles.size() > coords.size() && index > castles.size() - coords.size())
	it_top = castles.end() - coords.size();
    else
	it_top = castles.begin() + index;
}

const Castle * Game::SelectBarCastle::MouseClickLeft(void) const
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Castle *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != castles.end() && ii < coords.size())
    {
	if(le.MouseClickLeft(coords[ii])) return *it;

	if(*it) ++ii;
	++it;
    }

    return NULL;
}

const Castle * Game::SelectBarCastle::MousePressRight(void) const
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Castle *>::const_iterator it = it_top;
    u8 ii = 0;

    while(it != castles.end() && ii < coords.size())
    {
	if(le.MousePressRight(coords[ii])) return *it;

	if(*it) ++ii;
	++it;
    }

    return NULL;
}

Splitter & Game::SelectBarCastle::GetSplitter(void)
{
    return splitter;
}
