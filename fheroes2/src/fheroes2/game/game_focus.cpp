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
#include "castle.h"
#include "heroes.h"
#include "cursor.h"
#include "settings.h"
#include "game_interface.h"
#include "agg.h"
#include "world.h"
#include "kingdom.h"
#include "game_focus.h"

void GameFocus::Set(Heroes *hero)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	Focus & focus = player->focus;

	if(focus.GetHeroes() && focus.GetHeroes() != hero)
	{
	    focus.GetHeroes()->SetMove(false);
	    focus.GetHeroes()->ShowPath(false);
	}

	hero->RescanPath();
	hero->ShowPath(true);
	focus.Set(hero);

	Interface::Basic & I = Interface::Basic::Get();

	I.iconsPanel.Select(*hero);
	I.gameArea.SetCenter(hero->GetCenter());
	I.statusWindow.SetState(STATUS_ARMY);

	if(! Game::ChangeMusicDisabled())
	{
	    AGG::PlayMusic(MUS::FromGround(world.GetTiles(hero->GetCenter()).GetGround()));
	    Game::EnvironmentSoundMixer();    
	}
    }
}

void GameFocus::Set(Castle *castle)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	Focus & focus = player->focus;

	if(focus.GetHeroes())
	{
	    focus.GetHeroes()->SetMove(false);
	    focus.GetHeroes()->ShowPath(false);
	}

	focus.Set(castle);

	Interface::Basic & I = Interface::Basic::Get();

	I.iconsPanel.Select(*castle);
	I.gameArea.SetCenter(castle->GetCenter());
	I.statusWindow.SetState(STATUS_FUNDS);

	AGG::PlayMusic(MUS::FromGround(world.GetTiles(castle->GetCenter()).GetGround()));
	Game::EnvironmentSoundMixer();
    }
}

void GameFocus::Reset(u8 priority)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	Focus & focus = player->focus;

	Kingdom & myKingdom = world.GetKingdom(player->color);
	Interface::Basic & I = Interface::Basic::Get();

	I.iconsPanel.ResetIcons();

	switch(priority)
	{
	    case FIRSTHERO:
	    {
		const KingdomHeroes & heroes = myKingdom.GetHeroes();
		// skip sleeping
		KingdomHeroes::const_iterator it =
		    std::find_if(heroes.begin(), heroes.end(),
            	    std::not1(std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::SLEEPER)));

		if(it != heroes.end())
    		    Set(*it);
		else
		    Reset(GameFocus::CASTLE);
	    }
	    break;

	    case HEROES:
		if(focus.GetHeroes() && focus.GetHeroes()->GetColor() == player->color)
        	    Set(focus.GetHeroes());
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
		    focus.Reset();
	    break;

	    case CASTLE:
		if(focus.GetCastle() && focus.GetCastle()->GetColor() == player->color)
        	    Set(focus.GetCastle());
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
		    focus.Reset();
	    break;

	    default:
		focus.Reset();
	    break;
	}
    }
}

u8 GameFocus::Type(void)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	Focus & focus = player->focus;

	if(focus.GetHeroes()) return HEROES;
	else
	if(focus.GetCastle()) return CASTLE;
    }

    return UNSEL;
}

Castle* GameFocus::GetCastle(void)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	return player->focus.GetCastle();
    }

    return NULL;
}

Heroes* GameFocus::GetHeroes(void)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	return player->focus.GetHeroes();
    }

    return NULL;
}

Point GameFocus::GetCenter(void)
{
    Player* player = Settings::Get().GetPlayers().GetCurrent();

    if(player)
    {
	Focus & focus = player->focus;

	if(focus.GetHeroes()) return focus.GetHeroes()->GetCenter();
	else
	if(focus.GetCastle()) return focus.GetCastle()->GetCenter();
    }

    return Point(world.w()/2, world.h()/2);
}

void GameFocus::SetRedraw(void)
{
    Interface::Basic & I = Interface::Basic::Get();

    u8 type = Type();

    if(type != FOCUS_HEROES && I.iconsPanel.IsSelected(ICON_HEROES))
    {
	I.iconsPanel.ResetIcons(ICON_HEROES);
	I.SetRedraw(REDRAW_ICONS);
    }
    else
    if(type == FOCUS_HEROES && !I.iconsPanel.IsSelected(ICON_HEROES))
    {
	I.iconsPanel.Select(*GetHeroes());
	I.SetRedraw(REDRAW_ICONS);
    }

    if(type != FOCUS_CASTLE && I.iconsPanel.IsSelected(ICON_CASTLES))
    {
	I.iconsPanel.ResetIcons(ICON_CASTLES);
	I.SetRedraw(REDRAW_ICONS);
    }
    else
    if(type == FOCUS_CASTLE && !I.iconsPanel.IsSelected(ICON_CASTLES))
    {
	I.iconsPanel.Select(*GetCastle());
	I.SetRedraw(REDRAW_ICONS);
    }

    I.SetRedraw(REDRAW_GAMEAREA | REDRAW_RADAR);

    if(type == FOCUS_HEROES) I.SetRedraw(REDRAW_HEROES);
    else
    if(type == FOCUS_CASTLE) I.SetRedraw(REDRAW_CASTLES);

    I.SetRedraw(REDRAW_STATUS);
}
