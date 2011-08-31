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

#include <algorithm>
#include <functional>
#include "settings.h"
#include "heroes.h"
#include "castle.h"
#include "speed.h"
#include "battle_arena.h"
#include "battle_cell.h"
#include "battle_stats.h"
#include "battle_interface.h"
#include "ai_simple.h"

namespace Battle2
{
    bool isApplySpell(const Spell &, const Stats*, const HeroBase &, Actions &);
    u16  AIGetAttackPosition(Arena &, const Stats &, const std::vector<u16> &);
}

using namespace Battle2;

u16 Battle2::AIGetAttackPosition(Arena & arena, const Stats & b, const std::vector<u16> & positions)
{
    u16 res = MAXU16;

    if(b.isMultiCellAttack())
    {
        res = arena.GetMaxQualityPosition(positions);
    }
    else
    if(b.isDoubleCellAttack())
    {
        std::vector<u16> enemies, results;

        enemies.reserve(6);
        results.reserve(12);

        arena.GetArmyPositions(arena.GetOppositeColor(b.GetColor()), enemies);

        // get actual coord if enemy troop near placed
        while(1 < enemies.size())
        {
            const u16 cur = enemies.back();
            enemies.pop_back();

            std::vector<u16>::const_iterator it1 = enemies.begin();
            std::vector<u16>::const_iterator it2 = enemies.end();
            for(; it1 != it2; ++it1)
            {
                // get near placed enemies
                const direction_t dir = Board::GetDirection(cur, *it1);

                if(UNKNOWN != dir)
                {
                    if(Board::isValidDirection(cur, Board::GetReflectDirection(dir)))
                        results.push_back(Board::GetIndexDirection(cur, Board::GetReflectDirection(dir)));
                    if(Board::isValidDirection(*it1, dir))
                        results.push_back(Board::GetIndexDirection(*it1, dir));
                }
            }
        }

        if(results.size())
        {
            // find passable results
            std::vector<u16> passable;
            arena.GetPassableQualityPositions(b, passable);

            std::vector<u16>::iterator it1 = results.begin();
            std::vector<u16>::iterator it3 = it1;

            for (; it1 != results.end(); ++it1)
                if(passable.end() != std::find(passable.begin(), passable.end(), *it1))
                    *it3++ = *it1;

            if(it3 != results.end())
                results.resize(std::distance(results.begin(), it3));

            // get max quality
            if(results.size())
            {
                res = arena.GetMaxQualityPosition(results);
            }
        }
    }

    return res != MAXU16 ? res : Arena::GetShortDistance(b.GetPosition(), positions);
}


void AI::BattleTurn(Arena & arena, const Stats & b, Actions & a)
{
    Board & board = arena.board;

    // reset quality param for board
    arena.ResetBoard();

    // set quality for enemy troop
    board.SetEnemyQuality(b);

    const Stats* enemy = NULL;
    bool attack = false;

    if(b.isArchers() && !b.isHandFighting())
    {
	enemy = arena.GetEnemyMaxQuality(b.GetColor());
	BattleMagicTurn(arena, b, a, enemy);
	attack = true;
    }
    else
    if(b.isHandFighting())
    {
	enemy = arena.GetEnemyAbroadMaxQuality(b);
	BattleMagicTurn(arena, b, a, enemy);
	attack = true;
    }
    else
    {
	std::vector<u16> positions;
	positions.reserve(20);
	u16 move = MAXU16;

	if(b.Modes(SP_BERSERKER))
	{
	    arena.GetNearestTroops(b.GetPosition(), positions, NULL);
	    if(positions.size())
		move = *Rand::Get(positions);
	}
	else
	{
	    // set quality position from enemy
	    board.SetPositionQuality(b);

	    // get passable quality positions
	    arena.GetPassableQualityPositions(b, positions);
	    attack = true;

	    if(positions.size())
		move = AIGetAttackPosition(arena, b, positions);
	}

	if(MAXU16 != move)
	{
	    if(b.isFly())
	    {
		enemy = arena.GetEnemyAbroadMaxQuality(move, b.GetColor());
		BattleMagicTurn(arena, b, a, enemy);
	    	a.AddedMoveAction(b, move);
		attack = true;
	    }
	    else
	    {
		std::vector<u16> path;
		BattleMagicTurn(arena, b, a, NULL);
		arena.GetPath(b, move, path);
		if(path.size())
		{
		    enemy = arena.GetEnemyAbroadMaxQuality(path.back(), b.GetColor());
		    BattleMagicTurn(arena, b, a, enemy);
	    	    a.AddedMoveAction(b, path.back());

		    // archers move and short attack only
		    attack = b.isArchers() ? false : true;
		}
	    }
	}
	else
	    enemy = arena.GetEnemyAbroadMaxQuality(b);
    }

    if(enemy)
    {
	if(attack) a.AddedAttackAction(b, *enemy, enemy->GetPosition(), 0);
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, "enemy: " << "is NULL" << ", board: " << arena.BoardString());
    }

    // end action
    a.AddedEndAction(b);
}


void AI::BattleMagicTurn(Arena & arena, const Stats & b, Actions & a, const Stats* enemy)
{
    const HeroBase* hero = b.GetCommander();

    if(b.Modes(SP_BERSERKER) || !hero || hero->Modes(Heroes::SPELLCASTED) || !hero->HaveSpellBook() ||
	arena.isDisableCastSpell(Spell(), NULL)) return;

    Army::army_t* my_army = arena.GetArmy(b.GetColor());
    Army::army_t* enemy_army = arena.GetArmy(arena.GetOppositeColor(b.GetColor()));

    Armies friends(*my_army);
    Armies enemies(*enemy_army);

    // sort strongest
    friends.SortStrongest();

    // troop bad spell - clean
    {
	// sort strongest
	Armies::iterator it = std::find_if(friends.begin(), friends.end(),
					    std::bind2nd(std::mem_fun(&Stats::Modes), IS_BAD_MAGIC));

	if(it != friends.end())
	{
	    if(isApplySpell(Spell::DISPEL, *it, *hero, a)) return;
	    if(isApplySpell(Spell::CURE, *it, *hero, a)) return;
	}
    }

    // if handfighting
    if(enemy)
    {
	// kill dragons
	if(enemy->troop.isDragons() &&
	    !b.Modes(SP_DRAGONSLAYER) && isApplySpell(Spell::DRAGONSLAYER, &b, *hero, a)) return;

	// curse
	if(!enemy->Modes(SP_CURSE) && isApplySpell(Spell::CURSE, enemy, *hero, a)) return;
	// enemy good spell - clean
	if(enemy->Modes(IS_GOOD_MAGIC) && isApplySpell(Spell::DISPEL, enemy, *hero, a)) return;

	// up defense
	if(!b.Modes(SP_STEELSKIN) && !b.Modes(SP_STONESKIN) && isApplySpell(Spell::STEELSKIN, &b, *hero, a)) return;
	if(!b.Modes(SP_STONESKIN) && !b.Modes(SP_STEELSKIN) && isApplySpell(Spell::STONESKIN, &b, *hero, a)) return;
    }

    // my army blessing
    if(b.isArchers())
    {
	if(!b.Modes(SP_BLESS) && isApplySpell(Spell::BLESS, &b, *hero, a)) return;
	if(!b.Modes(SP_BLOODLUST) && isApplySpell(Spell::BLOODLUST, &b, *hero, a)) return;	
    }

    // up speed
    if(hero->HaveSpell(Spell::HASTE) && !enemy)
    {
	// sort strongest
	Armies::iterator it = std::find_if(friends.begin(), friends.end(),
					    std::not1(std::bind2nd(std::mem_fun(&Stats::Modes), SP_HASTE)));
	if(it != friends.end() &&
		isApplySpell(Spell::HASTE, *it, *hero, a)) return;
    }

    // shield spell conditions
    {
	Armies::iterator it = std::find_if(enemies.begin(), enemies.end(),
					    std::mem_fun(&Stats::isArchers));

	// find enemy archers
	if(it != enemies.end() ||
	// or archers tower
	    (arena.castle && arena.castle->GetColor() != b.GetColor() && arena.castle->isCastle()))
	{
	    // find strongest archers
	    for(it = friends.begin(); it != friends.end(); ++it)
		if((*it)->isArchers() && ! (*it)->Modes(SP_SHIELD)) break;

	    // or other strongest friends
	    if(it == friends.end())
		it = std::find_if(friends.begin(), friends.end(),
				std::not1(std::bind2nd(std::mem_fun(&Stats::Modes), SP_SHIELD)));

	    if(it != friends.end() &&
		    isApplySpell(Spell::SHIELD, *it, *hero, a)) return;
	}
    }

    // enemy army spell
    {
	// find mirror image or summon elem
	Armies::iterator it = std::find_if(enemies.begin(), enemies.end(),
					    std::bind2nd(std::mem_fun(&Stats::Modes), CAP_MIRRORIMAGE | CAP_SUMMONELEM));

	if(it != enemies.end())
	{
	    if(isApplySpell(Spell::ARROW, *it, *hero, a)) return;
	    if(isApplySpell(Spell::COLDRAY, *it, *hero, a)) return;
	    if(isApplySpell(Spell::FIREBALL, *it, *hero, a)) return;
	    if(isApplySpell(Spell::LIGHTNINGBOLT, *it, *hero, a)) return;
	}

	// find good magic
	it = std::find_if(enemies.begin(), enemies.end(),
					std::bind2nd(std::mem_fun(&Stats::Modes), IS_GOOD_MAGIC));

	if(it != enemies.end())
	{
	    // slow
	    if((*it)->Modes(SP_HASTE) && isApplySpell(Spell::SLOW, *it, *hero, a)) return;
	    // curse
	    if((*it)->Modes(SP_CURSE) && isApplySpell(Spell::CURSE, *it, *hero, a)) return;
	    //
	    if(isApplySpell(Spell::DISPEL, *it, *hero, a)) return;
	}

	// check undead
	if(std::count_if(friends.begin(), friends.end(), std::mem_fun(&Stats::isUndead)) <
		    std::count_if(enemies.begin(), enemies.end(), std::mem_fun(&Stats::isUndead)))
	{
	    if(isApplySpell(Spell::HOLYSHOUT, NULL, *hero, a)) return;
	    if(isApplySpell(Spell::HOLYWORD, NULL, *hero, a)) return;
	}

	// check alife
	if(std::count_if(friends.begin(), friends.end(), std::mem_fun(&Stats::isAlive)) <
		    std::count_if(enemies.begin(), enemies.end(), std::mem_fun(&Stats::isAlive)))
	{
	    if(isApplySpell(Spell::DEATHRIPPLE, NULL, *hero, a)) return;
	    if(isApplySpell(Spell::DEATHWAVE, NULL, *hero, a)) return;
	}

	Stats* stats = enemies.GetRandom();

	if(isApplySpell(Spell::LIGHTNINGBOLT, stats, *hero, a)) return;
	if(isApplySpell(Spell::FIREBALL, stats, *hero, a)) return;
	if(isApplySpell(Spell::COLDRAY, stats, *hero, a)) return;
	if(isApplySpell(Spell::ARROW, stats, *hero, a)) return;
    }

/* FIX: Damage Spell:
    Spell::FIREBLAST
    Spell::COLDRING
    Spell::CHAINLIGHTNING
    Spell::METEORSHOWER
*/

    if(isApplySpell(Spell::ARMAGEDDON, NULL, *hero, a)) return;
    if(isApplySpell(Spell::ELEMENTALSTORM, NULL, *hero, a)) return;
}

bool Battle2::isApplySpell(const Spell & spell, const Stats* b, const HeroBase & hero, Actions & a)
{
    switch(spell())
    {
        case Spell::CURE:	if(isApplySpell(Spell::MASSCURE, b, hero, a)) return true; break;
        case Spell::HASTE:	if(isApplySpell(Spell::MASSHASTE, b, hero, a)) return true; break;
        case Spell::SLOW:	if(isApplySpell(Spell::MASSSLOW, b, hero, a)) return true; break;
        case Spell::BLESS:	if(isApplySpell(Spell::MASSBLESS, b, hero, a)) return true; break;
        case Spell::CURSE:	if(isApplySpell(Spell::MASSCURSE, b, hero, a)) return true; break;
        case Spell::DISPEL:	if(isApplySpell(Spell::MASSDISPEL, b, hero, a)) return true; break;
        case Spell::SHIELD:	if(isApplySpell(Spell::MASSSHIELD, b, hero, a)) return true; break;

	default: break;
    }

    if(hero.CanCastSpell(spell) && (!b || b->AllowApplySpell(spell, &hero)))
    {
	a.AddedCastAction(spell, (b ? b->GetPosition() : MAXU16));
	return true;
    }

    return false;
}
