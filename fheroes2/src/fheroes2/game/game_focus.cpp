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

#include "castle.h"
#include "heroes.h"
#include "cursor.h"
#include "settings.h"
#include "gamearea.h"
#include "interface_icons.h"
#include "interface_status.h"
#include "interface_radar.h"
#include "agg.h"
#include "world.h"
#include "kingdom.h"
#include "game_focus.h"

Game::Focus::Focus() : castle(NULL), heroes(NULL)
{
}

Game::Focus & Game::Focus::Get(void)
{
    static Game::Focus gfocus;
    
    return gfocus;
}

void Game::Focus::Set(const Heroes *hr)
{
    if(NULL == hr) return;

    if(heroes && hr != heroes)
    {
	const_cast<Heroes *>(heroes)->SetMove(false);
        const_cast<Heroes *>(heroes)->ShowPath(false);
    }

    heroes = hr;
    castle = NULL;

    const_cast<Heroes *>(heroes)->ShowPath(true);

    Interface::HeroesIcons & heroesBar = Interface::IconsPanel::Get().GetHeroesBar();
    Interface::CastleIcons & castleBar = Interface::IconsPanel::Get().GetCastleBar();

    castleBar.Unselect();
    heroesBar.Select(hr);

    GameArea::Get().Center(heroes->GetCenter());

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(hr->GetCenter()).GetGround()));
    Game::EnvironmentSoundMixer();
    
    Interface::StatusWindow::Get().SetState(STATUS_ARMY);
}

void Game::Focus::Set(const Castle *cs)
{
    if(NULL == cs) return;

    if(heroes)
    {
	const_cast<Heroes *>(heroes)->SetMove(false);
        const_cast<Heroes *>(heroes)->ShowPath(false);
    }

    castle = cs;
    heroes = NULL;

    Interface::HeroesIcons & heroesBar = Interface::IconsPanel::Get().GetHeroesBar();
    Interface::CastleIcons & castleBar = Interface::IconsPanel::Get().GetCastleBar();

    heroesBar.Unselect();
    castleBar.Select(cs);

    GameArea::Get().Center(castle->GetCenter());

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(cs->GetCenter()).GetGround()));
    Game::EnvironmentSoundMixer();

    Interface::StatusWindow::Get().SetState(STATUS_FUNDS);
}

void Game::Focus::Reset(const focus_t priority)
{
    Kingdom & myKingdom = world.GetMyKingdom();

    Interface::HeroesIcons & heroesBar = Interface::IconsPanel::Get().GetHeroesBar();
    Interface::CastleIcons & castleBar = Interface::IconsPanel::Get().GetCastleBar();

    heroesBar.Reset();
    castleBar.Reset();

    switch(priority)
    {
	case HEROES:
	    if(heroes && heroes->GetColor() == Settings::Get().MyColor())
        	Set(heroes);
	    else
	    if(myKingdom.GetHeroes().size())
        	Set(myKingdom.GetHeroes().front());
	    else
            if(myKingdom.GetCastles().size())
            {
                heroesBar.Redraw();
                Set(myKingdom.GetCastles().front());
            }
            else
	    {
	    	castle = NULL;
		heroes = NULL;
	    }
	    break;

	case CASTLE:
	    if(castle && castle->GetColor() == Settings::Get().MyColor())
        	Set(castle);
	    else
	    if(myKingdom.GetCastles().size())
        	Set(myKingdom.GetCastles().front());
	    else
            if(myKingdom.GetHeroes().size())
            {
                castleBar.Redraw();
                Set(myKingdom.GetHeroes().front());
            }
            else
	    {
		castle = NULL;
		heroes = NULL;
	    }
	    break;

	default:
	    castle = NULL;
	    heroes = NULL;
	    break;
    }
}

Game::Focus::focus_t Game::Focus::Type(void) const
{
    if(heroes) return HEROES;
    else
    if(castle) return CASTLE;

    return UNSEL;
}

const Castle & Game::Focus::GetCastle(void) const
{
    if(NULL == castle) Error::Warning("Game::Focus::GetCastle: is NULL");

    return *castle;
}

Castle & Game::Focus::GetCastle(void)
{
    if(NULL == castle) Error::Warning("Game::Focus::GetCastle: is NULL");

    return *const_cast<Castle *>(castle);
}

const Heroes & Game::Focus::GetHeroes(void) const
{
    if(NULL == heroes) Error::Warning("Game::Focus::GetHeroes: is NULL");

    return *heroes;
}

Heroes & Game::Focus::GetHeroes(void)
{
    if(NULL == heroes) Error::Warning("Game::Focus::GetHeroes: is NULL");

    return *const_cast<Heroes *>(heroes);
}

const Point & Game::Focus::Center(void) const
{
    if(heroes) return heroes->GetCenter();
    else
    if(castle) return castle->GetCenter();

    return center;
}

void Game::Focus::Redraw(void)
{
    Interface::Radar & radar = Interface::Radar::Get();
    GameArea & areaMaps = GameArea::Get();
    Interface::HeroesIcons & heroesBar = Interface::IconsPanel::Get().GetHeroesBar();
    Interface::CastleIcons & castleBar = Interface::IconsPanel::Get().GetCastleBar();
    Interface::StatusWindow& statusWin = Interface::StatusWindow::Get();

    if(! heroes && !castle)
    {
	castleBar.Reset();
	heroesBar.Reset();
    }

    areaMaps.Redraw();
    radar.Redraw();

    if(heroes) heroesBar.Redraw();
    else
    if(castle) castleBar.Redraw();

    statusWin.Redraw();
}
