/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include "agg.h"
#include "castle.h"
#include "heroes.h"
#include "race.h"
#include "cursor.h"
#include "world.h"
#include "portrait.h"
#include "settings.h"
#include "game_focus.h"
#include "kingdom.h"
#include "game_interface.h"
#include "interface_icons.h"

namespace Game
{
    // game_startgame.cpp
    void OpenCastle(Castle *);
    void OpenHeroes(Heroes *);
}

#define ICONS_WIDTH             46
#define ICONS_HEIGHT            22
#define ICONS_CURSOR_WIDTH      56
#define ICONS_CURSOR_HEIGHT     32
#define ICONS_CURSOR_COLOR      0x98

/* Interface::IconsBar */
Interface::IconsBar::IconsBar() : selected(false)
{
    area.w = ICONS_WIDTH;
}

void Interface::IconsBar::SetPos(s16 px, s16 py)
{
    area.x = px;
    area.y = py;

    for(u8 ii = 0; ii < coords.size(); ++ii)
	coords[ii] = Rect(area.x, area.y + ii * ICONS_CURSOR_HEIGHT, ICONS_WIDTH, ICONS_HEIGHT);
}

void Interface::IconsBar::SetCount(u8 icons)
{
    coords.resize(icons);

    for(u8 ii = 0; ii < icons; ++ii)
	coords[ii] = Rect(area.x, area.y + ii * ICONS_CURSOR_HEIGHT, ICONS_WIDTH, ICONS_HEIGHT);

    area.h = coords.back().y + ICONS_HEIGHT;
}

void Interface::IconsBar::Redraw(void) const
{
    Display & display = Display::Get();
    const Sprite & back = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, 1);
    for(u8 ii = 0; ii < coords.size(); ++ii) display.Blit(back, coords[ii].x, coords[ii].y);
}

const std::vector<Rect> & Interface::IconsBar::GetCoords(void) const
{
    return coords;
}

const Rect & Interface::IconsBar::GetArea(void) const
{
    return area;
}

Splitter & Interface::IconsBar::GetSplitter(void)
{
    return splitter;
}

SpriteCursor & Interface::IconsBar::GetCursor(void)
{
    return sp_cursor;
}


/*Interface::HeroesIcons */
Interface::HeroesIcons::HeroesIcons()
{
    sf_cursor.Set(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT);
    sf_cursor.SetColorKey();
    Cursor::DrawCursor(sf_cursor, ICONS_CURSOR_COLOR, true);

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetOrientation(Splitter::VERTICAL);
}

void Interface::HeroesIcons::SetPos(s16 px, s16 py)
{
    IconsBar::SetPos(px, py);

    sp_cursor.Save(area.x, area.y);
    sp_cursor.SetSprite(sf_cursor);

    splitter.SetArea(px + 55, py + 13, 10, ICONS_CURSOR_HEIGHT * coords.size() - 35);
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    splitter.SetRange(0, heroes.size() > coords.size() ? heroes.size() - coords.size() : 0);
}

const Heroes * Interface::HeroesIcons::Selected(void) const
{
    return Size() && selected ? *it_cur : NULL;
}

u32 Interface::HeroesIcons::Size(void) const
{
    return world.GetMyKingdom().GetHeroes().size();
}

void Interface::HeroesIcons::RedrawIcon(const Heroes & hero, const Rect & dst)
{
    Display & display = Display::Get();

    const Sprite & mobility = AGG::GetICN(ICN::MOBILITY, hero.GetMobilityIndexSprite());
    const Sprite & mana = AGG::GetICN(ICN::MANA, hero.GetManaIndexSprite());
    const Surface & port = Portrait::Hero(hero, Portrait::SMALL);

    display.FillRect(0, 0, 0, dst);

    Surface sf_blue(7, ICONS_HEIGHT, true);
    sf_blue.Fill(sf_blue.GetColor(0x4C));

    // mobility
    display.Blit(sf_blue, dst.x, dst.y);
    display.Blit(mobility, dst.x, dst.y + mobility.y());

    // portrait
    display.Blit(port, dst.x + sf_blue.w() + 1, dst.y);

    // mana
    display.Blit(sf_blue, dst.x + sf_blue.w() + port.w() + 2, dst.y);
    display.Blit(mana, dst.x + sf_blue.w() + port.w() + 2, dst.y + mana.y());
}

void Interface::HeroesIcons::Redraw(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    if(selected) sp_cursor.Hide();
    splitter.Hide();
    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));

    IconsBar::Redraw();

    if(!hide)
    {
	std::vector<Heroes *>::const_iterator it = it_top;
	u8 ii = 0;

	while(it != heroes.end() && ii < coords.size())
	{
	    if(*it)
	    {
		RedrawIcon(**it, coords[ii]);
		// select current
		if(selected && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);
		++ii;
	    }
	    ++it;
	}
    }
    splitter.Show();
}

void Interface::HeroesIcons::Hide(void)
{
    if(isSelected()) Unselect();
    IconsBar::Redraw();
    hide = true;
}

void Interface::HeroesIcons::Show(void)
{
    hide = false;
}

void Interface::HeroesIcons::Unselect(void)
{
    if(selected)
    {
        sp_cursor.Hide();
	selected = false;
    }
}

void Interface::HeroesIcons::Select(const Heroes *hero)
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

    selected = true;
}

bool Interface::HeroesIcons::isSelected(void) const
{
    return selected;
}

void Interface::HeroesIcons::Reset(void)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    sp_cursor.Hide();

    it_top = heroes.begin();
    it_cur = it_top;
    selected = false;

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetRange(0, heroes.size() > coords.size() ? heroes.size() - coords.size() : 0);
}

bool Interface::HeroesIcons::Prev(void)
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

bool Interface::HeroesIcons::Next(void)
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

void Interface::HeroesIcons::SetTop(const u8 index)
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();

    if(heroes.size() > coords.size() && index > heroes.size() - coords.size())
	it_top = heroes.end() - coords.size();
    else
	it_top = heroes.begin() + index;
}

const Heroes * Interface::HeroesIcons::MouseClickLeft(void) const
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    LocalEvent & le = LocalEvent::Get();

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

const Heroes * Interface::HeroesIcons::MousePressRight(void) const
{
    std::vector<Heroes *> & heroes = world.GetMyKingdom().GetHeroes();
    LocalEvent & le = LocalEvent::Get();

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


/* Interface::CastleIcons */
Interface::CastleIcons::CastleIcons()
{
    sf_cursor.Set(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT);
    sf_cursor.SetColorKey();
    Cursor::DrawCursor(sf_cursor, ICONS_CURSOR_COLOR, true);

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetOrientation(Splitter::VERTICAL);
}

void Interface::CastleIcons::SetPos(s16 px, s16 py)
{
    IconsBar::SetPos(px, py);

    sp_cursor.Save(area.x, area.y);
    sp_cursor.SetSprite(sf_cursor);

    splitter.SetArea(px + 55, py + 13, 10, ICONS_CURSOR_HEIGHT * coords.size() - 35);

    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    splitter.SetRange(0, castles.size() > coords.size() ? castles.size() - coords.size() : 0);
}

const Castle * Interface::CastleIcons::Selected(void) const
{
    return Size() && selected ? *it_cur : NULL;
}

u32 Interface::CastleIcons::Size(void) const
{
    return world.GetMyKingdom().GetCastles().size();
}

void Interface::CastleIcons::RedrawIcon(const Castle & castle, const Rect & dst)
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
        default: DEBUG(DBG_ENGINE , DBG_WARN, "Interface::CastleIcons::Redraw: unknown race.");
    }

    display.Blit(AGG::GetICN(interface ? ICN::LOCATORE : ICN::LOCATORS, index_sprite), dst.x, dst.y);

    // castle build marker
    if(! castle.AllowBuild())
        display.Blit(AGG::GetICN(interface ? ICN::LOCATORE : ICN::LOCATORS, 24), dst.x + 39, dst.y + 1);
}

void Interface::CastleIcons::Redraw(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();

    if(selected) sp_cursor.Hide();
    splitter.Hide();
    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));

    IconsBar::Redraw();

    if(!hide)
    {
	std::vector<Castle *>::const_iterator it = it_top;
	u8 ii = 0;

	while(it != castles.end() && ii < coords.size())
	{
	    if(*it)
	    {
    		RedrawIcon(**it, coords[ii]);
		// select current
		if(selected && it_cur == it) sp_cursor.Show(coords[ii].x - 5, coords[ii].y - 5);
		++ii;
	    }
	    ++it;
	}
    }
    splitter.Show();
}

void Interface::CastleIcons::Hide(void)
{
    if(isSelected()) Unselect();
    IconsBar::Redraw();
    hide = true;
}

void Interface::CastleIcons::Show(void)
{
    hide = false;
}

void Interface::CastleIcons::Unselect(void)
{
    if(selected)
    {
        sp_cursor.Hide();
	selected = false;
    }
}

void Interface::CastleIcons::Select(const Castle *castle)
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

    selected = true;
}

bool Interface::CastleIcons::isSelected(void) const
{
    return selected;
}

void Interface::CastleIcons::Reset(void)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    sp_cursor.Hide();

    it_top = castles.begin();
    it_cur = it_top;
    selected = false;

    splitter.SetSprite(AGG::GetICN(Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL, 4));
    splitter.SetRange(0, castles.size() > coords.size() ? castles.size() - coords.size() : 0);
}

bool Interface::CastleIcons::Prev(void)
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

bool Interface::CastleIcons::Next(void)
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

void Interface::CastleIcons::SetTop(const u8 index)
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    
    if(castles.size() > coords.size() && index > castles.size() - coords.size())
	it_top = castles.end() - coords.size();
    else
	it_top = castles.begin() + index;
}

const Castle * Interface::CastleIcons::MouseClickLeft(void) const
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    LocalEvent & le = LocalEvent::Get();

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

const Castle * Interface::CastleIcons::MousePressRight(void) const
{
    std::vector<Castle *> & castles = world.GetMyKingdom().GetCastles();
    LocalEvent & le = LocalEvent::Get();

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


/* Interface::IconsPanel */
Interface::IconsPanel::IconsPanel() : Rect(0, 0, 144, 128), count_icons(4)
{
}

Interface::IconsPanel & Interface::IconsPanel::Get(void)
{
    static IconsPanel iconsPanel;

    return iconsPanel;
}

const Rect & Interface::IconsPanel::GetArea(void) const
{
    return Settings::Get().HideInterface() && border.isValid() ? border.GetRect() : *this;
}

u8 Interface::IconsPanel::CountIcons(void) const
{
    return count_icons;
}

void Interface::IconsPanel::SetPos(s16 ox, s16 oy)
{
    if(Settings::Get().HideInterface())
    {
        // fix out of range
        Display & display = Display::Get();
        if(ox + Rect::w < 0 || ox > display.w()) ox = 0;
        if(oy + Rect::h < 0 || oy > display.h()) oy = 0;

        Rect::x = ox + BORDERWIDTH;
        Rect::y = oy + BORDERWIDTH;
        border.SetPosition(ox, oy, Rect::w, Rect::h);
    }
    else
    {
	Rect::x = ox;
	Rect::y = oy;
    }

    ox = Rect::x;
    oy = Rect::y;

    const ICN::icn_t icnscroll = Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL;

    buttonScrollHeroesUp.SetSprite(icnscroll, 0, 1);
    buttonScrollCastleUp.SetSprite(icnscroll, 0, 1);
    buttonScrollHeroesDown.SetSprite(icnscroll, 2, 3);
    buttonScrollCastleDown.SetSprite(icnscroll, 2, 3);

    buttonScrollHeroesUp.SetPos(ox + ICONS_CURSOR_WIDTH + 1, oy + 1);
    buttonScrollCastleUp.SetPos(ox + 2 * ICONS_CURSOR_WIDTH + buttonScrollHeroesUp.w + 3, oy + 1);

    SetCount(count_icons);

    Settings::Get().SetPosIcons(*this);
}

void Interface::IconsPanel::SetCount(u8 count)
{
    count_icons = count;
    Rect::h = count_icons * ICONS_CURSOR_HEIGHT;

    buttonScrollHeroesDown.SetPos(x + ICONS_CURSOR_WIDTH + 1, y + count_icons * ICONS_CURSOR_HEIGHT - buttonScrollHeroesDown.h - 1);
    buttonScrollCastleDown.SetPos(x + 2 * ICONS_CURSOR_WIDTH + buttonScrollHeroesDown.w + 3, y + count_icons * ICONS_CURSOR_HEIGHT - buttonScrollCastleDown.h - 1);
    
    heroesIcons.SetPos(x + 5, y + 5);
    heroesIcons.SetCount(count_icons);

    castleIcons.SetPos(x + 77, y + 5);
    castleIcons.SetCount(count_icons);

    border.SetPosition(border.GetRect().x, border.GetRect().y, w, h);
}

void Interface::IconsPanel::Redraw(void)
{
    const Settings & conf = Settings::Get();
    if(conf.HideInterface() && !conf.ShowIcons()) return;

    Display & display = Display::Get();
    const ICN::icn_t icnscroll = Settings::Get().EvilInterface() ? ICN::SCROLLE : ICN::SCROLL;

    buttonScrollHeroesUp.SetSprite(icnscroll, 0, 1);
    buttonScrollCastleUp.SetSprite(icnscroll, 0, 1);
    buttonScrollHeroesDown.SetSprite(icnscroll, 2, 3);
    buttonScrollCastleDown.SetSprite(icnscroll, 2, 3);

    Rect srcrt;
    Point dstpt;

    if(heroesIcons.isSelected()) heroesIcons.GetCursor().Hide();
    else
    if(castleIcons.isSelected()) castleIcons.GetCursor().Hide();

    display.FillRect(0, 0, 0, *this);

    // ICON PANEL
    const Sprite & icnadv = AGG::GetICN(Settings::Get().EvilInterface() ? ICN::ADVBORDE : ICN::ADVBORD, 0);
    srcrt.x = icnadv.w() - RADARWIDTH - BORDERWIDTH;
    srcrt.y = RADARWIDTH + 2 * BORDERWIDTH;
    srcrt.w = RADARWIDTH;
    srcrt.h = 32;
    dstpt.x = x;
    dstpt.y = y;
    display.Blit(icnadv, srcrt, dstpt);
    srcrt.y = srcrt.y + srcrt.h;
    dstpt.y = dstpt.y + srcrt.h;
    srcrt.h = 32;
    for(u8 ii = 0; ii < count_icons - 2; ++ii)
    {
	display.Blit(icnadv, srcrt, dstpt);
	dstpt.y += srcrt.h;
    }
    srcrt.y = srcrt.y + 64;
    srcrt.h = 32;
    display.Blit(icnadv, srcrt, dstpt);

    buttonScrollHeroesUp.Draw();
    buttonScrollCastleUp.Draw();
    buttonScrollHeroesDown.Draw();
    buttonScrollCastleDown.Draw();

    if(heroesIcons.isSelected()) heroesIcons.GetCursor().Show();
    else
    if(castleIcons.isSelected()) castleIcons.GetCursor().Show();

    heroesIcons.Redraw();
    castleIcons.Redraw();        

    // redraw border
    if(conf.HideInterface()) border.Redraw();
}

void Interface::IconsPanel::QueueEventProcessing(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    Game::Focus & global_focus = Game::Focus::Get();

    HeroesIcons & selectHeroes = heroesIcons;
    CastleIcons & selectCastle = castleIcons;

    Splitter & splitCastle = selectCastle.GetSplitter();
    Splitter & splitHeroes = selectHeroes.GetSplitter();

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();

    le.MousePressLeft(buttonScrollHeroesUp) ? buttonScrollHeroesUp.PressDraw() : buttonScrollHeroesUp.ReleaseDraw();
    le.MousePressLeft(buttonScrollCastleUp) ? buttonScrollCastleUp.PressDraw() : buttonScrollCastleUp.ReleaseDraw();
    le.MousePressLeft(buttonScrollHeroesDown) ? buttonScrollHeroesDown.PressDraw() : buttonScrollHeroesDown.ReleaseDraw();
    le.MousePressLeft(buttonScrollCastleDown) ? buttonScrollCastleDown.PressDraw() : buttonScrollCastleDown.ReleaseDraw();

    // click Scroll Heroes Up
    if((le.MouseWheelUp(selectHeroes.GetArea()) ||
	le.MouseWheelUp(splitHeroes.GetRect()) ||
	le.MouseClickLeft(buttonScrollHeroesUp)) && selectHeroes.Prev())
    {
        cursor.Hide();
        selectHeroes.Redraw();
        cursor.Show();
        display.Flip();
    }
    else
    // click Scroll Castle Up
    if((le.MouseWheelUp(selectCastle.GetArea()) ||
        le.MouseWheelUp(splitCastle.GetRect()) ||
        le.MouseClickLeft(buttonScrollCastleUp)) && selectCastle.Prev())
    {
        cursor.Hide();
        selectCastle.Redraw();
        cursor.Show();
        display.Flip();
    }
    else
    // click Scroll Heroes Down
    if((le.MouseWheelDn(selectHeroes.GetArea()) ||
        le.MouseWheelDn(splitHeroes.GetRect()) ||
        le.MouseClickLeft(buttonScrollHeroesDown)) &&
        selectHeroes.Next())
    {
        cursor.Hide();
        selectHeroes.Redraw();
        cursor.Show();
        display.Flip();
    }
    else
    // click Scroll Castle Down
    if((le.MouseWheelDn(selectCastle.GetArea()) ||
        le.MouseWheelDn(splitCastle.GetRect())  ||
        le.MouseClickLeft(buttonScrollCastleDown)) &&
        selectCastle.Next())
    {
        cursor.Hide();
        selectCastle.Redraw();
        cursor.Show();
        display.Flip();
    }

    // move border
    if(conf.HideInterface() && conf.ShowIcons() && le.MousePressLeft(border.GetTop()))
    {
        Surface sf(border.GetRect().w, border.GetRect().h);
        Cursor::DrawCursor(sf, 0x70);
        const Point & mp = le.GetMouseCursor();
        const s16 ox = mp.x - border.GetRect().x;
        const s16 oy = mp.y - border.GetRect().y;
        SpriteCursor sp(sf, border.GetRect().x, border.GetRect().y);
        cursor.Hide();
	if(heroesIcons.isSelected()) heroesIcons.GetCursor().Hide();
	else
	if(castleIcons.isSelected()) castleIcons.GetCursor().Hide();
        sp.Redraw();
        cursor.Show();
        display.Flip();
        while(le.HandleEvents() && le.MousePressLeft())
        {
            if(le.MouseMotion())
            {
                cursor.Hide();
                sp.Move(mp.x - ox, mp.y - oy);
                cursor.Show();
                display.Flip();
            }
        }
        cursor.Hide();
        SetPos(mp.x - ox, mp.y - oy);
	if(heroesIcons.isSelected()) heroesIcons.GetCursor().Show();
	else
	if(castleIcons.isSelected()) castleIcons.GetCursor().Show();
	Interface::Basic::Get().SetRedraw(REDRAW_GAMEAREA);
    }
    else
    // move splitter cursor castle
    if(le.MousePressLeft(selectCastle.GetSplitter().GetRect()) &&
        myCastles.size() > count_icons)
    {
        if(0 != splitCastle.GetStep())
        {
            u32 seek = (le.GetMouseCursor().y - splitCastle.GetRect().y) * 100 / splitCastle.GetStep();
            if(seek > myCastles.size() - count_icons) seek = myCastles.size() - count_icons;

            cursor.Hide();
            splitCastle.Move(seek);
            selectCastle.SetTop(seek);
    	    selectCastle.Redraw();
            cursor.Show();
            display.Flip();
        }
    }
    else
    // move splitter cursor heroes
    if(le.MousePressLeft(selectHeroes.GetSplitter().GetRect()) &&
        myHeroes.size() > count_icons)
    {
        if(0 != splitHeroes.GetStep())
	{
            u32 seek = (le.GetMouseCursor().y - splitHeroes.GetRect().y) * 100 / splitHeroes.GetStep();
            if(seek > myHeroes.size() - count_icons) seek = myHeroes.size() - count_icons;

	    cursor.Hide();
	    splitHeroes.Move(seek);
	    selectHeroes.SetTop(seek);
	    selectHeroes.Redraw();
            cursor.Show();
            display.Flip();
	}
    }

    // click Heroes Icons
    if(const Heroes * hero = selectHeroes.MouseClickLeft())
    {
	if(hero == selectHeroes.Selected())
    	    Game::OpenHeroes(const_cast<Heroes *>(hero));
        else
        {
            global_focus.Set(const_cast<Heroes *>(hero));
            global_focus.SetRedraw();
        }

        // for QVGA: auto hide icons after click
        if(conf.QVGA()) conf.SetShowIcons(false);
    }
    else
    if(const Heroes * hero = selectHeroes.MousePressRight())
    {
        cursor.Hide();
        Dialog::QuickInfo(*hero);
        cursor.Show();
        display.Flip();
    }
    else
    // click Castle Icons
    if(const Castle * cstl = selectCastle.MouseClickLeft())
    {
        if(cstl == selectCastle.Selected())
    	    Game::OpenCastle(const_cast<Castle *>(cstl));
        else
        {
            global_focus.Set(const_cast<Castle *>(cstl));
            global_focus.SetRedraw();
        }

        // for QVGA: auto hide icons after click
        if(conf.QVGA()) conf.SetShowIcons(false);
    }
    else
    if(const Castle * cstl = selectCastle.MousePressRight())
    {
        cursor.Hide();
        Dialog::QuickInfo(*cstl);
        cursor.Show();
        display.Flip();
    }
}

Interface::HeroesIcons & Interface::IconsPanel::GetHeroesBar(void)
{
    return heroesIcons;
}

Interface::CastleIcons & Interface::IconsPanel::GetCastleBar(void)
{
    return castleIcons;
}
