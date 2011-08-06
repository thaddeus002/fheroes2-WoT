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

#include <functional>
#include <algorithm>
#include "game.h"
#include "cursor.h"
#include "engine.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "agg.h"
#include "ai_simple.h"

void AICastleTurn(Castle*);
void AIHeroesTurn(Heroes*);
void AIHeroesEnd(Heroes*);
void AIHeroesCaptureNearestTown(Heroes*);
void AIHeroesSetHunterWithTarget(Heroes*, s32);

AIKingdoms & AIKingdoms::Get(void)
{
    static AIKingdoms ai_kingdoms;
    return ai_kingdoms;
}

AIKingdom & AIKingdoms::Get(Color::color_t color)
{
    return AIKingdoms::Get().at(Color::GetIndex(color));
}

void AIKingdoms::Reset(void)
{
    AIKingdoms & ai = AIKingdoms::Get();
    std::for_each(ai.begin(), ai.end(), std::mem_fun_ref(&AIKingdom::Reset));
}

void AIKingdom::Reset(void)
{
    capital = NULL;
    scans.clear();
}

void IndexObjectMap::DumpObjects(const IndexDistance & id)
{
    IndexObjectMap::const_iterator it = find(id.first);

    if(it != end())
    DEBUG(DBG_AI, DBG_TRACE, MP2::StringObject((*it).second) \
			<< ", maps index: " << id.first << ", dist: " << id.second);
}

void WorldStoreObjects(u8 color, IndexObjectMap & store)
{
    for(s32 it = 0; it < world.w() * world.h(); ++it)
    {
        const Maps::Tiles & tile = world.GetTiles(it);
        if(tile.isFog(color)) continue;

        if(MP2::isGroundObject(tile.GetObject()) || MP2::isWaterObject(tile.GetObject()) || MP2::OBJ_HEROES == tile.GetObject())
        {
            // if quantity object is empty
            if(MP2::isQuantityObject(tile.GetObject()) && !tile.ValidQuantity()) continue;

	    // skip captured obj
	    if(MP2::isCaptureObject(tile.GetObject()) &&
		Settings::Get().IsUnions(color, world.ColorCapturedObject(tile.GetIndex()))) continue;

            // skip for meeting heroes
            if(MP2::OBJ_HEROES == tile.GetObject())
            {
                const Heroes* hero = world.GetHeroes(it);
                if(hero && color == hero->GetColor()) continue;
            }

            // check: is visited objects
            switch(tile.GetObject())
            {
                case MP2::OBJ_MAGELLANMAPS:
                case MP2::OBJ_OBSERVATIONTOWER:
                    if(world.GetKingdom(color).isVisited(tile)) continue;
                    break;

                default: break;
            }

            store[it] = tile.GetObject();
        }
    }
}

void AI::KingdomTurn(Kingdom & kingdom)
{
    KingdomHeroes & heroes = kingdom.GetHeroes();
    KingdomCastles & castles = kingdom.GetCastles();

    const Color::color_t & color = kingdom.GetColor();

    if(kingdom.isLoss() || color == Color::NONE)
    {
	kingdom.LossPostActions();
	return;
    }
    
    if(! Settings::Get().MusicMIDI()) AGG::PlayMusic(MUS::COMPUTER);

    Interface::StatusWindow *status = Interface::NoGUI() ? NULL : &Interface::StatusWindow::Get();
    AIKingdom & ai = AIKingdoms::Get(color);

    // turn indicator
    if(status) status->RedrawTurnProgress(0);

    // scan map
    ai.scans.clear();
    WorldStoreObjects(color, ai.scans);
    DEBUG(DBG_AI, DBG_INFO, Color::String(color) << ", size cache objects: " << ai.scans.size());

    // set capital
    if(NULL == ai.capital && castles.size())
    {
	KingdomCastles::iterator it = std::find_if(castles.begin(), castles.end(), Castle::PredicateIsCastle);

	if(castles.end() != it)
	{
	    if(*it)
	    {
		ai.capital = *it;
		ai.capital->SetModes(Castle::CAPITAL);
	    }
	}
	else
	// first town
	{
	    ai.capital = castles.front();
	    ai.capital->SetModes(Castle::CAPITAL);
	}
    }

    // turn indicator
    if(status) status->RedrawTurnProgress(1);

    // castles AI turn
    std::for_each(castles.begin(), castles.end(), AICastleTurn);

    // need capture town?
    if(castles.empty())
	std::for_each(heroes.begin(), heroes.end(), AIHeroesCaptureNearestTown);

    // buy hero in capital
    if(ai.capital && ai.capital->isCastle())
    {
	u32 modes = 0;
	const u8 maxhero = Maps::XLARGE > world.w() ? (Maps::LARGE > world.w() ? 3 : 2) : 4;

	if(heroes.empty())
	    modes = Heroes::HUNTER|Heroes::SCOUTER;
	else
	if((!ai.capital->GetHeroes().Guest() && heroes.size() < maxhero) ||
	    0 == std::count_if(heroes.begin(), heroes.end(), std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::SCOUTER)))
	    modes = Heroes::SCOUTER;

	if(modes &&
	    heroes.size() < Kingdom::GetMaxHeroes())
	{
	    Recruits & rec = kingdom.GetRecruits();
	    Heroes* hero = NULL;

	    if(rec.GetHero1() && rec.GetHero2())
		hero = ai.capital->RecruitHero(rec.GetHero1()->GetLevel() >= rec.GetHero2()->GetLevel() ? rec.GetHero1() : rec.GetHero2());
	    else
	    if(rec.GetHero1())
		hero = ai.capital->RecruitHero(rec.GetHero1());
	    else
	    if(rec.GetHero2())
		hero = ai.capital->RecruitHero(rec.GetHero2());

	    if(hero)
		hero->SetModes(modes);
	}
    }

    // set hunters
    if(ai.capital)
    {
	const size_t hunters = 
		std::count_if(heroes.begin(), heroes.end(),
			std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::HUNTER));

	// every time
	if(0 == hunters &&
	    heroes.size())
	{
	    KingdomHeroes::iterator it = std::find_if(heroes.begin(), heroes.end(),
				std::not1(std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::PATROL)));

	    if(it != heroes.end() &&
		! ai.capital->GetHeroes().Guest())
		AIHeroesSetHunterWithTarget((*it), ai.capital->GetIndex());
	}
	else
	// each month
	if(world.BeginMonth() && 1 < world.CountDay())
	{
	    KingdomHeroes::iterator it = 
		    std::find_if(heroes.begin(), heroes.end(),
			std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::HUNTER));

	    if(it != heroes.end() &&
		! ai.capital->GetHeroes().Guest())
		AIHeroesSetHunterWithTarget(*it, ai.capital->GetIndex());
	}
    }

    // update roles
    {
	std::for_each(heroes.begin(), heroes.end(), std::bind2nd(std::mem_fun(&Heroes::ResetModes), Heroes::STUPID|Heroes::AIWAITING));

	// init roles
	if(heroes.end() != std::find_if(heroes.begin(), heroes.end(),
		std::not1(std::bind2nd(std::mem_fun(&Heroes::Modes), Heroes::SCOUTER|Heroes::HUNTER))))
	{
	    KingdomHeroes::iterator ith, first = heroes.end();

	    while(heroes.end() != (ith = std::find_if(heroes.begin(), heroes.end(),
				std::not1(std::bind2nd(std::mem_fun(&Heroes::Modes),
						    // also skip patrol
						    Heroes::HUNTER|Heroes::SCOUTER|Heroes::PATROL)))))
	    {
		if(first == heroes.end())
		{
		    first = ith;
		    if(*ith) (*ith)->SetModes(Heroes::HUNTER|Heroes::SCOUTER);
		}
		else
		if(*ith) (*ith)->SetModes(Heroes::SCOUTER);
	    }
	}
    }

    // turn indicator
    if(status) status->RedrawTurnProgress(2);

    // heroes turns
    std::for_each(heroes.begin(), heroes.end(), std::ptr_fun(&AIHeroesTurn));
    std::for_each(heroes.begin(), heroes.end(), std::bind2nd(std::mem_fun(&Heroes::ResetModes), Heroes::STUPID|Heroes::AIWAITING));
    std::for_each(heroes.begin(), heroes.end(), std::ptr_fun(&AIHeroesTurn));
    std::for_each(heroes.begin(), heroes.end(), std::ptr_fun(&AIHeroesEnd));

    // turn indicator
    if(status) status->RedrawTurnProgress(9);

    DEBUG(DBG_AI, DBG_INFO, Color::String(color) << " moved");
}
