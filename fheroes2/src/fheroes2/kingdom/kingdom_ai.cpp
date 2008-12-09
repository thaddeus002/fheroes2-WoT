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

#include "game.h"
#include "cursor.h"
#include "display.h"
#include "gamearea.h"
#include "game_statuswindow.h"
#include "radar.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"

void Kingdom::AITurns(void)
{
    if(castles.empty() && heroes.empty())
    {
	ResetModes(PLAY);
	Error::Verbose("Kingdom::AITurns: " + Color::String(color) + "Loss!");
	return;
    }

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    cursor.Hide();
    cursor.SetThemes(Cursor::WAIT);
    cursor.Show();
    display.Flip();

    Game::StatusWindow & status = Game::StatusWindow::Get();

    status.RedrawAITurns(color, 0);
    status.RedrawAITurns(color, 1);
    status.RedrawAITurns(color, 2);
    status.RedrawAITurns(color, 3);
    status.RedrawAITurns(color, 4);
    status.RedrawAITurns(color, 5);
    status.RedrawAITurns(color, 6);
    status.RedrawAITurns(color, 7);
    status.RedrawAITurns(color, 8);
    status.RedrawAITurns(color, 9);

    // set capital
    if(NULL == ai_capital && castles.size())
    {
	std::vector<Castle *>::iterator it = std::find_if(castles.begin(), castles.end(), Castle::PredicateIsCastle);

	if(castles.end() != it)
	{
	    if(*it)
	    {
		ai_capital = *it;
		ai_capital->SetModes(Castle::CAPITAL);
	    }
	}
	else
	    // first town
	    castles.front()->SetModes(Castle::CAPITAL);
    }

    // castle AI turn
    AICastlesTurns();

    // buy hero in capital
    if(heroes.empty() && ai_capital && ai_capital->isCastle())
    {
	const Recruits & recruits = world.GetRecruits(GetColor());

	if(Heroes::UNKNOWN != recruits.first) ai_capital->RecruitHero(recruits.first);
	else
	if(Heroes::UNKNOWN != recruits.second) ai_capital->RecruitHero(recruits.second);

	Heroes *hero = ai_capital->GetHeroes();

	if(hero)
	{
	    hero->SetModes(Heroes::HUNTER);
	    hero->SetModes(Heroes::SCOUTER);
	}
    }

    // prepare task for heroes
    AIHeroesTask();

    // scan area maps
    // and set task for heroes movement

    // heroes AI turn
    AIHeroesTurns();

    cursor.Hide();

    if(Settings::Get().Debug()) Error::Verbose("Kingdom::AITurns: " + Color::String(color) + " moved");
}

void Kingdom::AICastlesTurns(void)
{
    std::vector<Castle *>::const_iterator itc1 = castles.begin();
    std::vector<Castle *>::const_iterator itc2 = castles.end();

    for(; itc1 != itc2; ++itc1) if(*itc1)
    {
	Castle & castle = **itc1;
	const s8 range = castle.isCastle() ? CASTLE_SCOUTE : TOWN_SCOUTE;
	bool defence = false;

	// find enemy hero
	for(s8 y = -range; y <= range; ++y)
    	    for(s8 x = -range; x <= range; ++x)
	{
    	    if(!y && !x) continue;

    	    if(Maps::isValidAbsPoint(castle.GetCenter().x + x, castle.GetCenter().y + y))
    	    {
        	const Maps::Tiles & tile = world.GetTiles(Maps::GetIndexFromAbsPoint(castle.GetCenter().x + x, castle.GetCenter().y + y));
        	const Heroes* hero = NULL;

        	if(MP2::OBJ_HEROES == tile.GetObject() && NULL != (hero = world.GetHeroes(tile.GetIndex())) && GetColor() != hero->GetColor())
            	{
		    defence = true;
		    goto SKIPLOOP;
    		}
	    }
	}

SKIPLOOP:
	defence ? AICastleDefence(castle) : AICastleDevelopment(castle);
    }
}

void Kingdom::AICastleDefence(Castle & castle)
{
    if(castle.isCastle())
    {
        if(!castle.isBuild(Castle::BUILD_LEFTTURRET))  castle.BuyBuilding(Castle::BUILD_LEFTTURRET);
        if(!castle.isBuild(Castle::BUILD_RIGHTTURRET)) castle.BuyBuilding(Castle::BUILD_RIGHTTURRET);
        if(!castle.isBuild(Castle::BUILD_MOAT))        castle.BuyBuilding(Castle::BUILD_MOAT);
        if(!castle.isBuild(Castle::BUILD_CAPTAIN) && NULL == castle.GetHeroes())      castle.BuyBuilding(Castle::BUILD_CAPTAIN);
        if(!castle.isBuild(Castle::BUILD_TAVERN) && Race::KNGT == castle.GetRace())   castle.BuyBuilding(Castle::BUILD_TAVERN);
        if(!castle.isBuild(Castle::BUILD_SPEC))        castle.BuyBuilding(Castle::BUILD_SPEC);
    }
    castle.RecruitAllMonster();
}

void Kingdom::AICastleDevelopment(Castle & castle)
{
    const Race::race_t race = castle.GetRace();

    if(heroes.size() && castle.isCastle() && castle.isCapital())
    {
	if(!castle.isBuild(Castle::BUILD_STATUE))	castle.BuyBuilding(Castle::BUILD_STATUE);
	if(!castle.isBuild(Castle::BUILD_SPEC) && Race::WRLK == race)	castle.BuyBuilding(Castle::BUILD_SPEC);
	if(!castle.isBuild(Castle::BUILD_TAVERN) && Race::KNGT == race)	castle.BuyBuilding(Castle::BUILD_TAVERN);
	if(!castle.isBuild(Castle::BUILD_MAGEGUILD1) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & race)) castle.BuyBuilding(Castle::BUILD_MAGEGUILD1);
	if(!castle.isBuild(Castle::BUILD_WELL))		castle.BuyBuilding(Castle::BUILD_WELL);

	if(!castle.isBuild(Castle::DWELLING_MONSTER1))	castle.BuyBuilding(Castle::DWELLING_MONSTER1);
	if(!castle.isBuild(Castle::DWELLING_MONSTER2))	castle.BuyBuilding(Castle::DWELLING_MONSTER2);
	if(!castle.isBuild(Castle::DWELLING_MONSTER3))	castle.BuyBuilding(Castle::DWELLING_MONSTER3);
	if(!castle.isBuild(Castle::DWELLING_MONSTER4))	castle.BuyBuilding(Castle::DWELLING_MONSTER4);

	if(!castle.isBuild(Castle::BUILD_THIEVESGUILD) && ((Race::NECR) & race)) castle.BuyBuilding(Castle::BUILD_THIEVESGUILD);
	if(!castle.isBuild(Castle::BUILD_MARKETPLACE))	castle.BuyBuilding(Castle::BUILD_MARKETPLACE);

	if(!castle.isBuild(Castle::BUILD_MAGEGUILD1))	castle.BuyBuilding(Castle::BUILD_MAGEGUILD1);
	if(!castle.isBuild(Castle::BUILD_MAGEGUILD2) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & race)) castle.BuyBuilding(Castle::BUILD_MAGEGUILD2);

	if(!castle.isBuild(Castle::DWELLING_UPGRADE2))	castle.BuyBuilding(Castle::DWELLING_UPGRADE2);
	if(!castle.isBuild(Castle::DWELLING_UPGRADE3))	castle.BuyBuilding(Castle::DWELLING_UPGRADE3);
	if(!castle.isBuild(Castle::DWELLING_UPGRADE4))	castle.BuyBuilding(Castle::DWELLING_UPGRADE4);

	if(!castle.isBuild(Castle::BUILD_LEFTTURRET))	castle.BuyBuilding(Castle::BUILD_LEFTTURRET);
	if(!castle.isBuild(Castle::BUILD_RIGHTTURRET))	castle.BuyBuilding(Castle::BUILD_RIGHTTURRET);
	if(!castle.isBuild(Castle::BUILD_MOAT))		castle.BuyBuilding(Castle::BUILD_MOAT);
	if(!castle.isBuild(Castle::BUILD_CAPTAIN))	castle.BuyBuilding(Castle::BUILD_CAPTAIN);

	if(!castle.isBuild(Castle::BUILD_MAGEGUILD2))	castle.BuyBuilding(Castle::BUILD_MAGEGUILD2);

	if(!castle.isBuild(Castle::DWELLING_MONSTER5))	castle.BuyBuilding(Castle::DWELLING_MONSTER5);
	if(!castle.isBuild(Castle::DWELLING_MONSTER6))	castle.BuyBuilding(Castle::DWELLING_MONSTER6);

	if(!castle.isBuild(Castle::BUILD_MAGEGUILD3))	castle.BuyBuilding(Castle::BUILD_MAGEGUILD3);

	if(!castle.isBuild(Castle::DWELLING_UPGRADE5))	castle.BuyBuilding(Castle::DWELLING_UPGRADE5);
	if(!castle.isBuild(Castle::DWELLING_UPGRADE6))	castle.BuyBuilding(Castle::DWELLING_UPGRADE6);
    }
    else
    // build castle only monday or tuesday or for capital
    if(3 > world.GetDay() || castle.isCapital()) castle.BuyBuilding(Castle::BUILD_CASTLE);

    // last day and buy monster
    if(world.LastDay()) castle.RecruitAllMonster();
}

void Kingdom::AIHeroesTurns(void)
{
    std::vector<Heroes *>::const_iterator ith1 = heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = heroes.end();
    Cursor::Get().Hide();

    for(; ith1 != ith2; ++ith1) if(*ith1)
    {
	Heroes & hero = **ith1;

	if(0 == Settings::Get().Debug() && !hero.isShow(Settings::Get().MyColor()))
	    hero.Move(true);
	else
	{
	    u32 ticket = 0;
	    Cursor::Get().Hide();
	    GameArea::Get().Center(hero.GetCenter());
	    GameArea::Get().Redraw();
	    Cursor::Get().Show();
	    Display::Get().Flip();

	    while(!hero.isFreeman() && hero.isEnableMove())
	    {
		if(Game::ShouldAnimateInfrequent(ticket, 1))
		{
		    Cursor::Get().Hide();
		    hero.Move();
		    GameArea::Get().Center(hero.GetCenter());
		    GameArea::Get().Redraw();
		    Cursor::Get().Show();
		    Display::Get().Flip();
		}

		++ticket;
	    }

	    // 0.2 sec delay for show enemy hero position
	    DELAY(200);
	}
    }
}

void Kingdom::AIHeroesTask(void)
{
    std::vector<Heroes *>::const_iterator ith1 = heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = heroes.end();

    for(; ith1 != ith2; ++ith1) if(*ith1)
    {
	Heroes & hero = **ith1;
	Castle *castle= hero.inCastle();

	// if hero in castle
	if(castle)
	{
	    castle->RecruitAllMonster();

	    // recruit army
	    if(hero.Modes(Heroes::HUNTER))
		hero.GetArmy().JoinStrongestFromArmy(castle->GetArmy());
	    else
	    if(hero.Modes(Heroes::SCOUTER))
		hero.GetArmy().KeepOnlyWeakestTroops(castle->GetArmy());
	}

	// more other
    }
}
