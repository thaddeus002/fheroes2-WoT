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
#include "game_interface.h"
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

    Interface::Basic & I = Interface::Basic::Get();
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();

    castleBar.Unselect();
    heroesBar.Select(hr);

    I.gameArea.Center(heroes->GetCenter());

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(hr->GetCenter()).GetGround()));
    Game::EnvironmentSoundMixer();
    
    I.statusWindow.SetState(STATUS_ARMY);
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

    Interface::Basic & I = Interface::Basic::Get();
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();

    heroesBar.Unselect();
    castleBar.Select(cs);

    I.gameArea.Center(castle->GetCenter());

    AGG::PlayMusic(MUS::FromGround(world.GetTiles(cs->GetCenter()).GetGround()));
    Game::EnvironmentSoundMixer();

    I.statusWindow.SetState(STATUS_FUNDS);
}

void Game::Focus::Reset(const focus_t priority)
{
    Kingdom & myKingdom = world.GetMyKingdom();

    Interface::Basic & I = Interface::Basic::Get();
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();

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
                I.SetRedraw(REDRAW_HEROES);
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
                I.SetRedraw(REDRAW_CASTLES);
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
    if(NULL == castle) DEBUG(DBG_GAME , DBG_WARN, "Game::Focus::GetCastle: is NULL");

    return *castle;
}

Castle & Game::Focus::GetCastle(void)
{
    if(NULL == castle) DEBUG(DBG_GAME , DBG_WARN, "Game::Focus::GetCastle: is NULL");

    return *const_cast<Castle *>(castle);
}

const Heroes & Game::Focus::GetHeroes(void) const
{
    if(NULL == heroes) DEBUG(DBG_GAME , DBG_WARN, "Game::Focus::GetHeroes: is NULL");

    return *heroes;
}

Heroes & Game::Focus::GetHeroes(void)
{
    if(NULL == heroes) DEBUG(DBG_GAME , DBG_WARN, "Game::Focus::GetHeroes: is NULL");

    return *const_cast<Heroes *>(heroes);
}

const Point & Game::Focus::Center(void) const
{
    if(heroes) return heroes->GetCenter();
    else
    if(castle) return castle->GetCenter();

    return center;
}

void Game::Focus::SetRedraw(void)
{
    Interface::Basic & I = Interface::Basic::Get();
    Interface::HeroesIcons & heroesBar = I.iconsPanel.GetHeroesBar();
    Interface::CastleIcons & castleBar = I.iconsPanel.GetCastleBar();

    if(!heroes && heroesBar.isSelected()) heroesBar.Reset();
    if(!castle && castleBar.isSelected()) castleBar.Reset();

    I.SetRedraw(REDRAW_GAMEAREA | REDRAW_RADAR);

    if(heroes) I.SetRedraw(REDRAW_HEROES);
    else
    if(castle) I.SetRedraw(REDRAW_CASTLES);

    I.SetRedraw(REDRAW_STATUS);
}
