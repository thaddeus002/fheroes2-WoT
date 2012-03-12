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

#define ICONS_WIDTH             46
#define ICONS_HEIGHT            22
#define ICONS_CURSOR_WIDTH      56
#define ICONS_CURSOR_HEIGHT     32
#define ICONS_CURSOR_COLOR      0x98

void RedrawCastleIcon(const Castle & castle, s16 sx, s16 sy)
{
    const bool evil = Settings::Get().ExtGameEvilInterface();
    u8 index_sprite = 1;

    switch(castle.GetRace())
    {
        case Race::KNGT: index_sprite = castle.isCastle() ?  9 : 15; break;
        case Race::BARB: index_sprite = castle.isCastle() ? 10 : 16; break;
        case Race::SORC: index_sprite = castle.isCastle() ? 11 : 17; break;
        case Race::WRLK: index_sprite = castle.isCastle() ? 12 : 18; break;
        case Race::WZRD: index_sprite = castle.isCastle() ? 13 : 19; break;
        case Race::NECR: index_sprite = castle.isCastle() ? 14 : 20; break;
        default: DEBUG(DBG_ENGINE, DBG_WARN, "unknown race");
    }

    AGG::GetICN(evil ? ICN::LOCATORE : ICN::LOCATORS, index_sprite).Blit(sx, sy);

    // castle build marker
    if(! castle.AllowBuild())
        AGG::GetICN(evil ? ICN::LOCATORE : ICN::LOCATORS, 24).Blit(sx + 39, sy + 1);
}

void RedrawHeroesIcon(const Heroes & hero, s16 sx, s16 sy)
{
    Display & display = Display::Get();
    const Sprite & mobility = AGG::GetICN(ICN::MOBILITY, hero.GetMobilityIndexSprite());
    const Sprite & mana = AGG::GetICN(ICN::MANA, hero.GetManaIndexSprite());
    const Surface & port = Portrait::Hero(hero, Portrait::SMALL);

    display.FillRect(0, 0, 0, Rect(sx, sy, ICONS_WIDTH, ICONS_HEIGHT));

    const u8  barw = 7;
    const u32 blue = display.MapRGB(15, 30, 120);

    // mobility
    display.FillRect(blue, Rect(sx, sy, barw, ICONS_HEIGHT));
    mobility.Blit(sx, sy + mobility.y());

    // portrait
    port.Blit(sx + barw + 1, sy, display);

    // mana
    display.FillRect(blue, Rect(sx + barw + port.w() + 2, sy, barw, ICONS_HEIGHT));
    mana.Blit(sx + barw + port.w() + 2, sy + mana.y());

    u8 oy = 0;

    // heroes marker
    if(hero.Modes(Heroes::SHIPMASTER))
    {
        AGG::GetICN(ICN::BOAT12, 0).Blit(sx + 35, sy - 1);
	oy = AGG::GetICN(ICN::BOAT12, 0).h();
    }
    else
    if(hero.Modes(Heroes::GUARDIAN))
    {
        AGG::GetICN(ICN::MISC6, 11).Blit(sx + 38, sy);
	oy = AGG::GetICN(ICN::MISC6, 11).h();
    }

    if(hero.Modes(Heroes::SLEEPER))
        AGG::GetICN(ICN::MISC4, 14).Blit(sx + 36, sy - 2 + oy);
}

void Interface::IconsBar::RedrawBackground(const Point & pos)
{
    const Sprite & icnadv = AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::ADVBORDE : ICN::ADVBORD, 0);
    const Sprite & back = AGG::GetICN(Settings::Get().ExtGameEvilInterface() ? ICN::LOCATORE : ICN::LOCATORS, 1);
    Rect srcrt;
    Point dstpt;

    srcrt.x = icnadv.w() - RADARWIDTH - BORDERWIDTH;
    srcrt.y = RADARWIDTH + 2 * BORDERWIDTH;
    srcrt.w = RADARWIDTH / 2;
    srcrt.h = 32;

    dstpt.x = pos.x;
    dstpt.y = pos.y;
    icnadv.Blit(srcrt, dstpt);

    srcrt.y = srcrt.y + srcrt.h;
    dstpt.y = dstpt.y + srcrt.h;
    srcrt.h = 32;

    if(2 < icons)
	for(u8 ii = 0; ii < icons - 2; ++ii)
	{
	    icnadv.Blit(srcrt, dstpt);
	    dstpt.y += srcrt.h;
	}

    srcrt.y = srcrt.y + 64;
    srcrt.h = 32;
    icnadv.Blit(srcrt, dstpt);

    for(u8 ii = 0; ii < icons; ++ii)
	back.Blit(pos.x + 5, pos.y + 5 + ii * (ICONS_HEIGHT + 10));
}

/* Interface::CastleIcons */
void Interface::CastleIcons::RedrawItem(const CASTLE & item, s16 ox, s16 oy, bool current)
{
    if(item && show)
    {
	RedrawCastleIcon(*item, ox, oy);

	if(current)
	    marker.Blit(ox - 5, oy - 5, Display::Get());
    }
}

void Interface::CastleIcons::RedrawBackground(const Point & pos)
{
    IconsBar::RedrawBackground(pos);
}

void Interface::CastleIcons::ActionCurrentUp(void)
{
    GameFocus::Set(GetCurrent());
}

void Interface::CastleIcons::ActionCurrentDn(void)
{
    GameFocus::Set(GetCurrent());
}

void Interface::CastleIcons::ActionListDoubleClick(CASTLE & item)
{
    if(item)
    {
        Game::OpenCastleDialog(item);

	// for QVGA: auto hide icons after click
	if(Settings::Get().QVGA()) Settings::Get().SetShowIcons(false);
    }
}

void Interface::CastleIcons::ActionListSingleClick(CASTLE & item)
{
    if(item)
    {
	GameFocus::Set(item);
	GameFocus::SetRedraw();

	// for QVGA: auto hide icons after click
	if(Settings::Get().QVGA()) Settings::Get().SetShowIcons(false);
    }
}

void Interface::CastleIcons::ActionListPressRight(CASTLE & item)
{
    if(item)
    {
	Cursor::Get().Hide();
	Dialog::QuickInfo(*item);
    }
}

void Interface::CastleIcons::SetShow(bool f)
{
    IconsBar::SetShow(f);

    if(f)
	GetSplitter().Show();
    else
	GetSplitter().Hide();
}

void Interface::CastleIcons::SetPos(s16 px, s16 py)
{
    const ICN::icn_t icnscroll = Settings::Get().ExtGameEvilInterface() ? ICN::SCROLLE : ICN::SCROLL;

    SetTopLeft(Point(px, py));
    SetScrollSplitter(AGG::GetICN(icnscroll, 4),
			Rect(px + ICONS_CURSOR_WIDTH + 4, py + 18, 10, ICONS_CURSOR_HEIGHT * icons - 35));
    SetScrollButtonUp(icnscroll, 0, 1,
			Point(px + ICONS_CURSOR_WIDTH + 1, py + 1));
    SetScrollButtonDn(icnscroll, 2, 3,
			Point(px + ICONS_CURSOR_WIDTH + 1, py + icons * ICONS_CURSOR_HEIGHT - 15));
    SetAreaMaxItems(icons);
    SetAreaItems(Rect(px + 5, py + 5, ICONS_CURSOR_WIDTH, icons * ICONS_CURSOR_HEIGHT));
    DisableHotkeys(true);

    SetListContent(world.GetKingdom(Settings::Get().CurrentColor()).GetCastles());
    Reset();
}

/* Interface::HeroesIcons */
void Interface::HeroesIcons::RedrawItem(const HEROES & item, s16 ox, s16 oy, bool current)
{
    if(item && show)
    {
	RedrawHeroesIcon(*item, ox, oy);

	if(current)
	    marker.Blit(ox - 5, oy - 5, Display::Get());
    }
}

void Interface::HeroesIcons::RedrawBackground(const Point & pos)
{
    IconsBar::RedrawBackground(pos);
}

void Interface::HeroesIcons::ActionCurrentUp(void)
{
    GameFocus::Set(GetCurrent());
}

void Interface::HeroesIcons::ActionCurrentDn(void)
{
    GameFocus::Set(GetCurrent());
}

void Interface::HeroesIcons::ActionListDoubleClick(HEROES & item)
{
    if(item)
    {
	if(item->Modes(Heroes::GUARDIAN))
	    Game::OpenCastleDialog(world.GetCastle(item->GetIndex()));
	else
	    Game::OpenHeroesDialog(item);

	// for QVGA: auto hide icons after click
	if(Settings::Get().QVGA()) Settings::Get().SetShowIcons(false);
    }
}

void Interface::HeroesIcons::ActionListSingleClick(HEROES & item)
{
    if(item)
    {
	GameFocus::Set(item);
	GameFocus::SetRedraw();

	// for QVGA: auto hide icons after click
	if(Settings::Get().QVGA()) Settings::Get().SetShowIcons(false);
    }
}

void Interface::HeroesIcons::ActionListPressRight(HEROES & item)
{
    if(item)
    {
	Cursor::Get().Hide();
	Dialog::QuickInfo(*item);
    }
}

void Interface::HeroesIcons::SetShow(bool f)
{
    IconsBar::SetShow(f);

    if(f)
	GetSplitter().Show();
    else
	GetSplitter().Hide();
}

void Interface::HeroesIcons::SetPos(s16 px, s16 py)
{
    const ICN::icn_t icnscroll = Settings::Get().ExtGameEvilInterface() ? ICN::SCROLLE : ICN::SCROLL;

    SetTopLeft(Point(px, py));
    SetScrollSplitter(AGG::GetICN(icnscroll, 4),
			Rect(px + ICONS_CURSOR_WIDTH + 4, py + 18, 10, ICONS_CURSOR_HEIGHT * icons - 35));
    SetScrollButtonUp(icnscroll, 0, 1,
			Point(px + ICONS_CURSOR_WIDTH + 1, py + 1));
    SetScrollButtonDn(icnscroll, 2, 3,
			Point(px + ICONS_CURSOR_WIDTH + 1, py + icons * ICONS_CURSOR_HEIGHT - 15));
    SetAreaMaxItems(icons);
    SetAreaItems(Rect(px + 5, py + 5, ICONS_CURSOR_WIDTH, icons * ICONS_CURSOR_HEIGHT));
    DisableHotkeys(true);

    SetListContent(world.GetKingdom(Settings::Get().CurrentColor()).GetHeroes());
    Reset();
}

/* Interface::IconsPanel */
Interface::IconsPanel::IconsPanel() : Rect(0, 0, 144, 128), icons(4),
    castleIcons(icons, sfMarker), heroesIcons(icons, sfMarker)
{
    sfMarker.Set(ICONS_CURSOR_WIDTH, ICONS_CURSOR_HEIGHT, false);
    Cursor::DrawCursor(sfMarker, ICONS_CURSOR_COLOR, true);
}

Interface::IconsPanel & Interface::IconsPanel::Get(void)
{
    static IconsPanel iconsPanel;
    return iconsPanel;
}

const Rect & Interface::IconsPanel::GetArea(void) const
{
    return Settings::Get().ExtGameHideInterface() && border.isValid() ? border.GetRect() : *this;
}

u8 Interface::IconsPanel::CountIcons(void) const
{
    return icons;
}

void Interface::IconsPanel::SetPos(s16 ox, s16 oy)
{
    if(Settings::Get().ExtGameHideInterface())
    {
	FixOutOfDisplay(*this, ox, oy); 

        Rect::x = ox + BORDERWIDTH;
        Rect::y = oy + BORDERWIDTH;

        border.SetPosition(ox, oy, Rect::w, Rect::h);
	Settings::Get().SetPosIcons(Point(ox, oy));
    }
    else
    {
	Rect::x = ox;
	Rect::y = oy;
    }

    SetCount(icons);
}

void Interface::IconsPanel::SetCount(u8 count)
{
    icons = count;
    Rect::h = icons * ICONS_CURSOR_HEIGHT;

    heroesIcons.SetPos(x, y);
    castleIcons.SetPos(x + 72, y);

    border.SetPosition(border.GetRect().x, border.GetRect().y, w, h);
}

void Interface::IconsPanel::Redraw(void)
{
    const Settings & conf = Settings::Get();

    // is visible
    if(!conf.ExtGameHideInterface() || conf.ShowIcons())
    {
	//Display::Get().FillRect(0, 0, 0, *this);

	heroesIcons.Redraw();
	castleIcons.Redraw();

	// redraw border
	if(conf.ExtGameHideInterface()) border.Redraw();
    }
}

void Interface::IconsPanel::QueueEventProcessing(void)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    if(heroesIcons.QueueEventProcessing())
    {
	if(heroesIcons.isSelected())
	    castleIcons.Unselect();

	Interface::Basic::Get().SetRedraw(REDRAW_ICONS);
    }
    else
    if(castleIcons.QueueEventProcessing())
    {
	if(castleIcons.isSelected())
	    heroesIcons.Unselect();

	Interface::Basic::Get().SetRedraw(REDRAW_ICONS);
    }
    else
    // move border
    if(conf.ExtGameHideInterface() && conf.ShowIcons() && le.MousePressLeft(border.GetTop()))
    {
        Surface sf(border.GetRect().w, border.GetRect().h);
        Cursor::DrawCursor(sf, 0x70);
        const Point & mp = le.GetMouseCursor();
        const s16 ox = mp.x - border.GetRect().x;
        const s16 oy = mp.y - border.GetRect().y;
        SpriteCursor sp(sf, border.GetRect().x, border.GetRect().y);
        cursor.Hide();
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

	GameFocus::SetRedraw();
	Interface::Basic::Get().SetRedraw(REDRAW_ICONS|REDRAW_GAMEAREA);
    }
}

void Interface::IconsPanel::Select(const Heroes & hr)
{
    castleIcons.Unselect();
    heroesIcons.SetCurrent((const HEROES) &hr);
}

void Interface::IconsPanel::Select(const Castle & cs)
{
    heroesIcons.Unselect();
    castleIcons.SetCurrent((const CASTLE) &cs);
}

void Interface::IconsPanel::ResetIcons(icons_t icons)
{
    if(icons & ICON_HEROES)
    {
	heroesIcons.SetListContent(world.GetKingdom(Settings::Get().CurrentColor()).GetHeroes());
	heroesIcons.Reset();
    }

    if(icons & ICON_CASTLES)
    {
	castleIcons.SetListContent(world.GetKingdom(Settings::Get().CurrentColor()).GetCastles());
	castleIcons.Reset();
    }
}

void Interface::IconsPanel::HideIcons(icons_t icons)
{
    if(icons & ICON_HEROES) heroesIcons.SetShow(false);
    if(icons & ICON_CASTLES) castleIcons.SetShow(false);
}

void Interface::IconsPanel::ShowIcons(icons_t icons)
{
    if(icons & ICON_HEROES) heroesIcons.SetShow(true);
    if(icons & ICON_CASTLES) castleIcons.SetShow(true);
}

void Interface::IconsPanel::SetCurrentVisible(void)
{
    if(heroesIcons.isSelected())
    {
	heroesIcons.SetCurrentVisible();
	heroesIcons.Redraw();
    }
    else
    if(castleIcons.isSelected())
    {
	castleIcons.SetCurrentVisible();
	castleIcons.Redraw();
    }
}

void Interface::IconsPanel::RedrawIcons(icons_t icons)
{
    if(icons & ICON_HEROES) heroesIcons.Redraw();
    if(icons & ICON_CASTLES) castleIcons.Redraw();
}

bool Interface::IconsPanel::IsSelected(icons_t icons) const
{
    if(icons & ICON_HEROES) return heroesIcons.isSelected();
    else
    if(icons & ICON_CASTLES) return castleIcons.isSelected();

    return false;
}
