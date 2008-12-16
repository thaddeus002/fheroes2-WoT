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

#include <functional>
#include "game.h"
#include "cursor.h"
#include "engine.h"
#include "gamearea.h"
#include "game_statuswindow.h"
#include "radar.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "agg.h"

void RedrawAITurns(u8 color, u8 i)
{
    Cursor::Get().Hide();
    Game::StatusWindow::Get().RedrawAITurns(color, i);
    Cursor::Get().Show();
    Display::Get().Flip();
}


void Kingdom::AIDumpCacheObjects(const IndexDistance & id) const
{
    std::map<u16, MP2::object_t>::const_iterator it = ai_objects.find(id.first);
    if(it != ai_objects.end())
    std::cout << "AIDumpCacheObjects: " << MP2::StringObject((*it).second) << ", maps index: " << id.first << ", dist: " << id.second << std::endl;
}

void Kingdom::AITurns(void)
{
    if((castles.empty() && heroes.empty()) || color == Color::GRAY)
    {
	ResetModes(PLAY);
	world.KingdomLoss(color);
	return;
    }
    
    AGG::PlayMusic(MUS::COMPUTER);

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    cursor.Hide();
    cursor.SetThemes(Cursor::WAIT);
    cursor.Show();
    display.Flip();

    // turn indicator
    RedrawAITurns(color, 0);

    // scan map
    ai_objects.clear();
    world.StoreActionObject(GetColor(), ai_objects);
    if(1 < Settings::Get().Debug()) Error::Verbose("Kingdom::AITurns: " + Color::String(color) + ", size cache objects: ", ai_objects.size());

    // turn indicator
    RedrawAITurns(color, 1);

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
	{
	    ai_capital = castles.front();
	    ai_capital->SetModes(Castle::CAPITAL);
	}
    }

    // turn indicator
    RedrawAITurns(color, 2);

    // castle AI turn
    AICastlesTurns();

    // turn indicator
    RedrawAITurns(color, 3);

    // update roles
    if(heroes.size()) std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::AIUpdateRoles));

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

    // turn indicator
    RedrawAITurns(color, 4);
    
    // push stupid heroes
    std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::ResetStupidFlag));

    // turn indicator
    RedrawAITurns(color, 5);


    // heroes turns
    std::vector<Heroes *>::const_iterator ith1 = heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = heroes.end();

    for(; ith1 != ith2; ++ith1) if(*ith1)
    {
	Heroes & hero = **ith1;

	while(hero.MayStillMove())
	{
	RedrawAITurns(color, 6);

	// prepare task for heroes
	AIHeroesTask(hero);

	// turn indicator
	RedrawAITurns(color, 7);

	// heroes AI turn
	AIHeroesTurns(hero);

	// turn indicator
	RedrawAITurns(color, 8);
	}
    }

    // turn indicator
    RedrawAITurns(color, 9);

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
	defence ? castle.AIDefence() : castle.AIDevelopment();
    }
}

void Kingdom::AIHeroesTurns(Heroes &hero)
{
        if(hero.GetPath().isValid()) hero.SetMove(true);
	else return;

	Cursor::Get().Hide();

	u32 ticket = 0;

	if(hero.isShow(Settings::Get().MyColor()))
	{
	    Cursor::Get().Hide();
	    GameArea::Get().Center(hero.GetCenter());
	    GameArea::Get().Redraw();
	    Cursor::Get().Show();
	    Display::Get().Flip();
	}

	while(LocalEvent::GetLocalEvent().HandleEvents())
	{
	    if(hero.isFreeman() || !hero.isEnableMove()) break;

	    if(0 == Settings::Get().Debug() && !hero.isShow(Settings::Get().MyColor()))
		hero.Move(true);
	    else
	    if(Game::ShouldAnimateInfrequent(ticket, 1))
	    {
		Cursor::Get().Hide();
		hero.Move();

    		if(Game::ShouldAnimateInfrequent(ticket, 12)) Maps::IncreaseAnimationTicket();

		GameArea::Get().Center(hero.GetCenter());
		GameArea::Get().Redraw();
		Cursor::Get().Show();
		Display::Get().Flip();
	    }

	    ++ticket;
	}

	// 0.2 sec delay for show enemy hero position
	if(!hero.isFreeman() && hero.isShow(Settings::Get().MyColor())) DELAY(200);
}

void Kingdom::AIHeroesTask(Heroes & hero)
{
	if(hero.GetPath().isValid()) return;

	Castle *castle = hero.inCastle();
	u16 index = MAXU16;

	// if hero in castle
	if(castle)
	{
	    castle->RecruitAllMonster();
	    hero.GetArmy().UpgradeTroops(*castle);

	    // recruit army
	    if(hero.Modes(Heroes::HUNTER))
		hero.GetArmy().JoinStrongestFromArmy(castle->GetArmy());
	    else
	    if(hero.Modes(Heroes::SCOUTER))
		hero.GetArmy().KeepOnlyWeakestTroops(castle->GetArmy());
	}

	// load minimal distance tasks
	std::vector<IndexDistance>objs;
	objs.reserve(ai_objects.size());
	std::vector<IndexDistance>::const_reverse_iterator ito1 = objs.rbegin();
	std::vector<IndexDistance>::const_reverse_iterator ito2 = objs.rend();

	std::map<u16, MP2::object_t>::const_iterator itm1 = ai_objects.begin();
	std::map<u16, MP2::object_t>::const_iterator itm2 = ai_objects.end();

	for(; itm1 != itm2; ++itm1)
	{
	    if(hero.isShipMaster())
	    {
		if(MP2::isGroundObject((*itm1).second)) continue;

		if(hero.GetPath().Calculate((*itm1).first) &&
		   hero.AIValidObject((*itm1).second, (*itm1).first)) objs.push_back(IndexDistance((*itm1).first, hero.GetPath().size()));
	    }
	    else
	    {
		if(MP2::isWaterObject((*itm1).second)) continue;

		if(hero.GetPath().Calculate((*itm1).first) &&
		   hero.AIValidObject((*itm1).second, (*itm1).first))
		{
		    u16 pos = 0;
		    // check monster on path
		    if(hero.GetPath().isUnderProtection(pos))
		    {
			const Maps::Tiles & tile = world.GetTiles(pos);
			Army::army_t enemy;
			enemy.At(0).Set(Monster::Monster(tile), tile.GetCountMonster());

			// can we will win battle
			if(enemy.isValid() && ! hero.GetArmy().StrongerEnemyArmy(enemy)) continue;
		    }

		    objs.push_back(IndexDistance((*itm1).first, hero.GetPath().size()));
		}
	    }
	}

	if(objs.size()) std::sort(objs.begin(), objs.end(), IndexDistance::Longest);
	if(1 < Settings::Get().Debug()) Error::Verbose("Kingdom::AIHeroesTask: " + Color::String(color) + ", hero: " + hero.GetName() + ", unconfirmed tasks: ", objs.size());

	if(MAXU16 == index && objs.size())
	{
	    ito1 = objs.rbegin();
	    ito2 = objs.rend();
	    while(ito1 != ito2 && MAXU16 == index)
	    {
		const u16 index1 = (*ito1).first;
		++ito1;
		const u16 index2 = (ito1 != ito2 ? (*ito1).first : MAXU16);

		if(MAXU16 == index2)
		{
		    index = index1;
		}
		else
		{
		    index = Rand::Get(1) ? index1 : index2;
		}
	    }
	}

	// success
	if(MAXU16 != index)
	{
	    if(Settings::Get().Debug()) Error::Verbose("AI::HeroesTask: " + Color::String(color) + ", Hero "+ hero.GetName() + " say: go to object: " + MP2::StringObject(ai_objects[index]) + ", index: ", index);
	    ai_objects.erase(index);
	    hero.GetPath().Calculate(index);
	    if(1 < Settings::Get().Debug()) hero.GetPath().Dump();
	}
	else
	{
	    hero.GetPath().Reset();
	    if(Settings::Get().Debug()) Error::Verbose("AI::HeroesTask: " + Color::String(color) + ", Hero "+ hero.GetName() + " say: unknown task...");
	    if(1 < Settings::Get().Debug())
	    {
		ito1 = objs.rbegin();
		ito2 = objs.rend();
		for(; ito1 != ito2; ++ito1) AIDumpCacheObjects(*ito1);
	    }
	    if(Settings::Get().Debug()) Error::Verbose("AI::HeroesTask: " + Color::String(color) + ", Hero " + hero.GetName() + " say: I'm stupid, help my please..");
	    hero.SetModes(Heroes::STUPID);
	}
}
