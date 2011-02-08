/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "world.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"
#include "race.h"
#include "ai_simple.h"

void AICastleDefense(Castle & c)
{
    if(c.isCastle())
    {
        if(!c.isBuild(BUILD_LEFTTURRET)) 
		c.BuyBuilding(BUILD_LEFTTURRET);

        if(!c.isBuild(BUILD_RIGHTTURRET))
		c.BuyBuilding(BUILD_RIGHTTURRET);

        if(!c.isBuild(BUILD_MOAT))
    		c.BuyBuilding(BUILD_MOAT);

        if(!c.isBuild(BUILD_CAPTAIN) && NULL == c.GetHeroes().Guest())
		c.BuyBuilding(BUILD_CAPTAIN);

        if(!c.isBuild(BUILD_TAVERN) && Race::KNGT == c.GetRace())
		c.BuyBuilding(BUILD_TAVERN);

        if(!c.isBuild(BUILD_SPEC))
		c.BuyBuilding(BUILD_SPEC);
    }
    c.RecruitAllMonster();
}

void AICastleDevelopment(Castle & c)
{
    const Kingdom & kingdom = world.GetKingdom(c.GetColor());

    if(c.isCastle())
    {
	// build for capital or large golds
	if(c.isCapital() || kingdom.GetFundsGold() > 8000)
	{
	    if(!c.isBuild(BUILD_STATUE))
		c.BuyBuilding(BUILD_STATUE);

	    if(!c.isBuild(BUILD_SPEC) && Race::WRLK == c.GetRace())
		c.BuyBuilding(BUILD_SPEC);

	    if(!c.isBuild(BUILD_TAVERN) && Race::KNGT == c.GetRace())
		c.BuyBuilding(BUILD_TAVERN);

	    if(!c.isBuild(BUILD_MAGEGUILD1) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & c.GetRace()))
		c.BuyBuilding(BUILD_MAGEGUILD1);

	    if(!c.isBuild(BUILD_WELL))
		c.BuyBuilding(BUILD_WELL);


	    if(!c.isBuild(DWELLING_MONSTER1))
		c.BuyBuilding(DWELLING_MONSTER1);

	    if(!c.isBuild(DWELLING_MONSTER2))
		c.BuyBuilding(DWELLING_MONSTER2);

	    if(!c.isBuild(DWELLING_MONSTER3))
		c.BuyBuilding(DWELLING_MONSTER3);

	    if(!c.isBuild(DWELLING_MONSTER4))
		c.BuyBuilding(DWELLING_MONSTER4);


	    if(!c.isBuild(BUILD_THIEVESGUILD) && ((Race::NECR) & c.GetRace()))
		c.BuyBuilding(BUILD_THIEVESGUILD);

	    if(!c.isBuild(BUILD_MARKETPLACE))
		c.BuyBuilding(BUILD_MARKETPLACE);

	    if(!c.isBuild(BUILD_MAGEGUILD1))
		c.BuyBuilding(BUILD_MAGEGUILD1);

	    if(!c.isBuild(BUILD_MAGEGUILD2) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & c.GetRace()))
		c.BuyBuilding(BUILD_MAGEGUILD2);

	    if(!c.isBuild(DWELLING_UPGRADE2))
		c.BuyBuilding(DWELLING_UPGRADE2);

	    if(!c.isBuild(DWELLING_UPGRADE3))
		c.BuyBuilding(DWELLING_UPGRADE3);

	    if(!c.isBuild(DWELLING_UPGRADE4))
		c.BuyBuilding(DWELLING_UPGRADE4);

	    if(!c.isBuild(BUILD_LEFTTURRET))
		c.BuyBuilding(BUILD_LEFTTURRET);

	    if(!c.isBuild(BUILD_RIGHTTURRET))
		c.BuyBuilding(BUILD_RIGHTTURRET);

	    if(!c.isBuild(BUILD_MOAT))
		c.BuyBuilding(BUILD_MOAT);

	    if(!c.isBuild(BUILD_CAPTAIN))
		c.BuyBuilding(BUILD_CAPTAIN);


	    if(!c.isBuild(BUILD_MAGEGUILD2))
		c.BuyBuilding(BUILD_MAGEGUILD2);

	    if(!c.isBuild(DWELLING_MONSTER5))
		c.BuyBuilding(DWELLING_MONSTER5);

	    if(!c.isBuild(DWELLING_MONSTER6))
		c.BuyBuilding(DWELLING_MONSTER6);

	    if(!c.isBuild(BUILD_MAGEGUILD3))
		c.BuyBuilding(BUILD_MAGEGUILD3);

	    if(!c.isBuild(DWELLING_UPGRADE5))
		c.BuyBuilding(DWELLING_UPGRADE5);

	    if(!c.isBuild(DWELLING_UPGRADE6))
		c.BuyBuilding(DWELLING_UPGRADE6);
	}
    }
    else
    {
	// build castle only monday or tuesday or for capital
	if(c.isCapital() || 3 > world.GetDay()) c.BuyBuilding(BUILD_CASTLE);
    }

    // last day and buy monster
    if(world.LastDay()) c.RecruitAllMonster();
}

void AICastleTurn(Castle* castle)
{
    if(castle)
    {
	s8 range = Game::GetViewDistance(castle->isCastle() ? Game::VIEW_CASTLE : Game::VIEW_TOWN);
	const Heroes* enemy = NULL;

	// find enemy hero
	for(s8 y = -range; y <= range; ++y)
    	    for(s8 x = -range; x <= range; ++x)
	{
    	    if(!y && !x) continue;
	    const Point & center = castle->GetCenter();

    	    if(Maps::isValidAbsPoint(center.x + x, center.y + y))
    	    {
        	const Maps::Tiles & tile = world.GetTiles(Maps::GetIndexFromAbsPoint(center.x + x, center.y + y));

        	if(MP2::OBJ_HEROES == tile.GetObject())
		    enemy = world.GetHeroes(tile.GetIndex());

		if(enemy && castle->GetColor() == enemy->GetColor())
		    enemy = NULL;

		if(enemy) break;
	    }
	}

	enemy ? AICastleDefense(*castle) : AICastleDevelopment(*castle);

	Kingdom & kingdom = world.GetKingdom(castle->GetColor());
	bool can_recruit = kingdom.GetHeroes().size() < Kingdom::GetMaxHeroes();

	// part II
	if(enemy &&
	    castle->GetArmy().isValid() &&
	    castle->GetArmy().StrongerEnemyArmy(enemy->GetArmy()))
	{
    	    if(!castle->GetHeroes().Guest() && can_recruit)
    	    {
        	Recruits & rec = kingdom.GetRecruits();
                                                                                                                              
        	if(rec.GetHero1()) castle->RecruitHero(rec.GetHero1());
        	else
        	if(rec.GetHero2()) castle->RecruitHero(rec.GetHero2());
    	    }

    	    if(castle->GetHeroes().Guest())
		castle->GetHeroes().Guest()->SetModes(Heroes::HUNTER);
	}

	// part III
	AIKingdom & ai = AIKingdoms::Get(castle->GetColor());
	if(ai.capital != castle &&
	    castle->GetArmy().isValid() &&
	    ! castle->GetHeroes().Guest() &&
	    2 < castle->GetArmy().GetCount() &&
	    150 < castle->GetArmy().GetHitPoints() &&
	    can_recruit)
	{
    	    Recruits & rec = kingdom.GetRecruits();
                                                                                                                              
    	    if(rec.GetHero1()) castle->RecruitHero(rec.GetHero1());
    	    else
    	    if(rec.GetHero2()) castle->RecruitHero(rec.GetHero2());

    	    if(castle->GetHeroes().Guest())
		castle->GetHeroes().Guest()->SetModes(Heroes::HUNTER|Heroes::SCOUTER);
	}
    }
}

void AI::CastlePreBattle(Castle & castle)
{
    Heroes *hero = castle.GetHeroes().GuardFirst();
    if(hero && castle.GetArmy().isValid())
	hero->GetArmy().JoinStrongestFromArmy(castle.GetArmy());
}

void AI::CastleAfterBattle(Castle &, bool attacker_wins)
{
}
