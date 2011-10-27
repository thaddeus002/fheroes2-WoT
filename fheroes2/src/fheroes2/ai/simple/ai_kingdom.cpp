/********************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>               *
 *   All rights reserved.                                                       *
 *                                                                              *
 *   Part of the Free Heroes2 Engine:                                           *
 *   http://sourceforge.net/projects/fheroes2                                   *
 *                                                                              *
 *   Redistribution and use in source and binary forms, with or without         *
 *   modification, are permitted provided that the following conditions         *
 *   are met:                                                                   *
 *   - Redistributions may not be sold, nor may they be used in a               *
 *     commercial product or activity.                                          *
 *   - Redistributions of source code and/or in binary form must reproduce      *
 *     the above copyright notice, this list of conditions and the              *
 *     following disclaimer in the documentation and/or other materials         *
 *     provided with the distribution.                                          *
 *                                                                              *
 * THIS SOFTWARE IS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,   *
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,        *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,     *
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE         *
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,            *
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           *
 *******************************************************************************/

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

        if(MP2::isGroundObject(tile.GetObject()) ||
	    MP2::isWaterObject(tile.GetObject()) || MP2::OBJ_HEROES == tile.GetObject())
        {
            // if quantity object is empty
            if(MP2::isQuantityObject(tile.GetObject()) &&
		! MP2::isPickupObject(tile.GetObject()) && ! tile.QuantityIsValid()) continue;

	    // skip captured obj
	    if(MP2::isCaptureObject(tile.GetObject()) &&
		Players::isFriends(color, tile.QuantityColor())) continue;

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
