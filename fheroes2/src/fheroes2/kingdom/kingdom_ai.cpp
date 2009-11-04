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
#include <algorithm>
#include <deque>
#include "game.h"
#include "cursor.h"
#include "engine.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "interface_status.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "agg.h"

#define HERO_MAX_SHEDULED_TASK 7

void RedrawAITurns(u8 color, u8 i)
{
    Cursor::Get().Hide();
    Interface::StatusWindow::Get().RedrawAITurns(color, i);
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
    if(isLoss() || color == Color::GRAY)
    {
	LossPostActions();
	return;
    }
    
    if(!Settings::Get().Modes(Settings::MUSIC_MIDI)) AGG::PlayMusic(MUS::COMPUTER);

    Cursor & cursor = Cursor::Get();
    Display & display = Display::Get();

    cursor.Hide();
    cursor.SetThemes(Cursor::WAIT);
    cursor.Show();
    display.Flip();

    Interface::StatusWindow::Get().SetState(STATUS_AITURN);

    // turn indicator
    RedrawAITurns(color, 0);

    // scan map
    ai_objects.clear();
    world.StoreActionObject(GetColor(), ai_objects);
    if(1 < Settings::Get().Debug()) std::cout << "Kingdom::AITurns: " << Color::String(color) << ", size cache objects: " << ai_objects.size() << std::endl;

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
    if(heroes.size()) std::for_each(heroes.begin(), heroes.end(), std::bind2nd(std::mem_fun(&Heroes::SetModes), Heroes::SCOUTER|Heroes::HUNTER));

    // buy hero in capital
    if(heroes.empty() && ai_capital && ai_capital->isCastle())
    {
	Recruits & rec = GetRecruits();

	if(rec.GetHero1()) ai_capital->RecruitHero(rec.GetHero1());
	else
	if(rec.GetHero2()) ai_capital->RecruitHero(rec.GetHero2());

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

	    // get task for heroes
	    AIHeroesGetTask(hero);

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

    if(Settings::Get().Debug()) std::cout << "Kingdom::AITurns: " << Color::String(color) << " moved" << std::endl;
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
	for(s8 y = -range; y <= range && !defence; ++y)
    	    for(s8 x = -range; x <= range && !defence; ++x)
	{
    	    if(!y && !x) continue;

    	    if(Maps::isValidAbsPoint(castle.GetCenter().x + x, castle.GetCenter().y + y))
    	    {
        	const Maps::Tiles & tile = world.GetTiles(Maps::GetIndexFromAbsPoint(castle.GetCenter().x + x, castle.GetCenter().y + y));
        	const Heroes* hero = NULL;

        	if(MP2::OBJ_HEROES == tile.GetObject() && NULL != (hero = world.GetHeroes(tile.GetIndex())) && GetColor() != hero->GetColor())
		    defence = true;
	    }
	}

	defence ? castle.AIDefence() : castle.AIDevelopment();
    }
}

void Kingdom::AIHeroesTurns(Heroes &hero)
{
        if(hero.GetPath().isValid()) hero.SetMove(true);
	else return;

	Display & display = Display::Get();
	Cursor & cursor = Cursor::Get();
	Interface::Basic & I = Interface::Basic::Get();

	cursor.Hide();
	u32 ticket = 0;

	if(!Settings::Get().HideAIMove() && hero.isShow(Settings::Get().MyColor()))
	{
	    cursor.Hide();
	    I.gameArea.Center(hero.GetCenter());
	    I.Redraw(REDRAW_GAMEAREA);
	    cursor.Show();
	    display.Flip();
	}

	while(LocalEvent::Get().HandleEvents())
	{
	    if(hero.isFreeman() || !hero.isEnableMove()) break;

	    if(Settings::Get().HideAIMove() ||
	       (0 == Settings::Get().Debug() && !hero.isShow(Settings::Get().MyColor())))
		hero.Move(true);
	    else
	    if(Game::ShouldAnimateInfrequent(ticket, 1))
	    {
		cursor.Hide();
		hero.Move();

    		if(Game::ShouldAnimateInfrequent(ticket, 12)) Maps::IncreaseAnimationTicket();

		I.gameArea.Center(hero.GetCenter());
		I.Redraw(REDRAW_GAMEAREA);
		cursor.Show();
		display.Flip();
	    }

	    ++ticket;
	}

	// 0.2 sec delay for show enemy hero position
	if(!hero.isFreeman() && !Settings::Get().HideAIMove() && hero.isShow(Settings::Get().MyColor())) DELAY(200);
}

void Kingdom::AIHeroesGetTask(Heroes & hero)
{
    if(hero.GetPath().isValid()) return;

    std::deque<u16> & task = hero.GetSheduledVisit();

    if(task.empty()) AIHeroesPrepareTask(hero);

    // find passable index
    while(task.size() && !hero.GetPath().Calculate(task.front())){ task.pop_front(); }

    // success
    if(task.size())
    {
	const u16 index = task.front();
	if(Settings::Get().Debug()) std::cout << "AI::HeroesTask: " << Color::String(color) << ", Hero " << hero.GetName() << " say: go to object: " << MP2::StringObject(ai_objects[index]) << ", index: " << index << std::endl;
	task.pop_front();
	ai_objects.erase(index);
	if(2 < Settings::Get().Debug()) hero.GetPath().Dump();
    }
    else
    {
	hero.GetPath().Reset();
	if(Settings::Get().Debug()) std::cout << "AI::HeroesTask: " << Color::String(color) << ", Hero " << hero.GetName() << " say: unknown task., help my please.." << std::endl;
	hero.SetModes(Heroes::STUPID);
    }
}

void Kingdom::AIHeroesPrepareTask(Heroes & hero)
{
    if(hero.GetPath().isValid()) return;

    Castle *castle = hero.inCastle();

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

    std::deque<u16> & task = hero.GetSheduledVisit();

    // load minimal distance tasks
    std::vector<IndexDistance> objs;
    objs.reserve(ai_objects.size());
    std::vector<IndexDistance>::const_reverse_iterator ito1 = objs.rbegin();
    std::vector<IndexDistance>::const_reverse_iterator ito2 = objs.rend();

    std::map<u16, MP2::object_t>::const_iterator itm1 = ai_objects.begin();
    std::map<u16, MP2::object_t>::const_iterator itm2 = ai_objects.end();

    for(; itm1 != itm2; ++itm1)
    {
	//std::cout << "AIHeroTask: obj: " << MP2::StringObject((*itm1).second) << ", index: " << (*itm1).first << std::endl;

	if(hero.isShipMaster())
	{
	    if(MP2::isGroundObject((*itm1).second)) continue;

	    if(hero.GetPath().Calculate((*itm1).first) &&
		  hero.AIValidObject((*itm1).first, (*itm1).second))
	    {
		if(hero.AIPriorityObject((*itm1).first, (*itm1).second))
		    task.push_front((*itm1).first);
		else
		    objs.push_back(IndexDistance((*itm1).first, hero.GetPath().size()));

		if(objs.size() > HERO_MAX_SHEDULED_TASK) break;
	    }
	}
	else
	{
	    if(MP2::isWaterObject((*itm1).second)) continue;

	    if(hero.GetPath().Calculate((*itm1).first) && hero.AIValidObject((*itm1).first, (*itm1).second))
	    {
		u16 pos = 0;
		// check monster on path
		if(hero.GetPath().isUnderProtection(pos))
		{
		    const Maps::Tiles & tile = world.GetTiles(pos);
		    Army::army_t enemy;
		    enemy.At(0).Set(Monster(tile), tile.GetCountMonster());

		    // can we will win battle
		    if(enemy.isValid() && ! hero.GetArmy().StrongerEnemyArmy(enemy)) continue;
		}

		if(hero.AIPriorityObject((*itm1).first, (*itm1).second))
		    task.push_front((*itm1).first);
		else
		    objs.push_back(IndexDistance((*itm1).first, hero.GetPath().size()));

		if(objs.size() > HERO_MAX_SHEDULED_TASK) break;
	    }
	}
    }

    if(objs.size()) std::sort(objs.begin(), objs.end(), IndexDistance::Longest);
    if(1 < Settings::Get().Debug()) std::cout << "Kingdom::AIHeroesTask: " << Color::String(color) <<", hero: " << hero.GetName() << ", unconfirmed tasks: " << objs.size() << std::endl;

    // store random task
    if(objs.size())
    {
	ito1 = objs.rbegin();
	ito2 = objs.rend();

	while(ito1 != ito2)
	{
	    const u16 index1 = (*ito1).first;
	    ++ito1;
	    const u16 index2 = (ito1 != ito2 ? (*ito1).first : MAXU16);

	    if(MAXU16 == index2)
		task.push_back(index1);
	    else
		task.push_back(Rand::Get(1) ? index1 : index2);
	}
    }
    else
	hero.AIRescueWhereMove();

    // resize
    if(task.size() > HERO_MAX_SHEDULED_TASK) task.resize(HERO_MAX_SHEDULED_TASK);
}
