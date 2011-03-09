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

#include <algorithm>
#include "settings.h"
#include "world.h"
#include "kingdom.h"
#include "spell.h"
#include "battle_cell.h"
#include "battle_stats.h"
#include "battle_arena.h"
#include "battle_tower.h"
#include "battle_bridge.h"
#include "battle_catapult.h"
#include "battle_interface.h"
#include "server.h"

void Battle2::Actions::AddedAutoBattleAction(u8 color)
{
    Action action;
    action.SetID(MSG_BATTLE_AUTO);
    action.Push(color);

    push_back(action);
}

void Battle2::Actions::AddedSurrenderAction(void)
{
    Action action;
    action.SetID(MSG_BATTLE_SURRENDER);

    push_back(action);
}

void Battle2::Actions::AddedRetreatAction(void)
{
    Action action;
    action.SetID(MSG_BATTLE_RETREAT);

    push_back(action);
}

void Battle2::Actions::AddedCastAction(const Spell & spell, u16 dst)
{
    Action action;
    action.SetID(MSG_BATTLE_CAST);
    action.Push(spell());
    action.Push(static_cast<u16>(0));	// id, hero or monster
    action.Push(dst);

    push_back(action);
}

void Battle2::Actions::AddedCastMirrorImageAction(u16 who)
{
    Action action;
    action.SetID(MSG_BATTLE_CAST);
    action.Push(static_cast<u8>(Spell::MIRRORIMAGE));
    action.Push(who);

    push_back(action);
}

void Battle2::Actions::AddedCastTeleportAction(u16 src, u16 dst)
{
    Action action;
    action.SetID(MSG_BATTLE_CAST);
    action.Push(static_cast<u8>(Spell::TELEPORT));
    action.Push(src);
    action.Push(dst);

    push_back(action);
}

void Battle2::Actions::AddedEndAction(const Stats & b)
{
    Action action;
    action.SetID(MSG_BATTLE_END_TURN);
    action.Push(b.id);

    push_back(action);
}

void Battle2::Actions::AddedSkipAction(const Stats & b, bool hard)
{
    Action action;
    action.SetID(MSG_BATTLE_SKIP);
    action.Push(b.id);
    action.Push(static_cast<u8>(hard));

    push_back(action);
}

void Battle2::Actions::AddedMoveAction(const Stats & b, u16 dst)
{
    Action action;
    action.SetID(MSG_BATTLE_MOVE);
    action.Push(b.id);
    action.Push(dst);
    action.Push(static_cast<u16>(0));

    push_back(action);
}

void Battle2::Actions::AddedAttackAction(const Stats & a, const Stats & d, u16 dst, u8 dir)
{
    Action action;
    action.SetID(MSG_BATTLE_ATTACK);
    action.Push(a.id);
    action.Push(d.id);
    action.Push(dst);
    action.Push(dir);

    push_back(action);
}

void Battle2::Actions::AddedMoraleAction(const Stats & b, u8 state)
{
    Action action;
    action.SetID(MSG_BATTLE_MORALE);
    action.Push(b.id);
    action.Push(state);

    push_back(action);
}

void Battle2::Arena::BattleProcess(Stats & attacker, Stats & defender, s16 dst, u8 dir)
{
    if(0 > dst) dst = defender.GetPosition();

    if(dir)
    {
	if(attacker.isWide())
	{
	    if(!Board::NearCells(attacker.GetPosition(), dst))
		attacker.UpdateDirection(board[dst]);
	    if(defender.AllowResponse())
		defender.UpdateDirection(board[attacker.GetPosition()]);
	}
	else
	{
	    attacker.UpdateDirection(board[dst]);
	    if(defender.AllowResponse())
		defender.UpdateDirection(board[attacker.GetPosition()]);
	}
    }
    else
	attacker.UpdateDirection(board[dst]);

    TargetsInfo targets = GetTargetsForDamage(attacker, defender, dst);

#ifdef WITH_NET
    if(Network::isRemoteClient())
    {
	if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendAttack(army1.GetColor(), attacker, defender, dst, targets);
	if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendAttack(army2.GetColor(), attacker, defender, dst, targets);
    }
#endif

    if(Board::isReflectDirection(dir) != attacker.isReflect())
	attacker.UpdateDirection(board[dst]);

    if(interface) interface->RedrawActionAttackPart1(attacker, defender, targets);

    TargetsApplyDamage(attacker, defender, targets);
    if(interface) interface->RedrawActionAttackPart2(attacker, targets);

    const Spell spell = attacker.GetSpellMagic();

    // magic attack
    if(defender.isValid() && spell.isValid())
    {
	const std::string name(attacker.GetName());

	targets = GetTargetsForSpells(attacker.GetCommander(), spell, defender.GetPosition());
	if(interface) interface->RedrawActionSpellCastPart1(spell, defender.GetPosition(), name, targets);

	// magic attack not depends from hero
	TargetsApplySpell(NULL, spell, targets);
	if(interface) interface->RedrawActionSpellCastPart2(spell, targets);
	if(interface) interface->RedrawActionMonsterSpellCastStatus(attacker, targets.front());

#ifdef WITH_NET
	if(Network::isRemoteClient())
    	{
	    if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendSpell(army1.GetColor(), attacker.GetID(), defender.GetPosition(), spell, targets);
	    if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendSpell(army2.GetColor(), attacker.GetID(), defender.GetPosition(), spell, targets);
	}
#endif
    }

    attacker.PostAttackAction(defender);
}

void Battle2::Arena::ApplyAction(Action & action)
{
    switch(action.GetID())
    {
	case MSG_BATTLE_CAST:		ApplyActionSpellCast(action); break;
	case MSG_BATTLE_ATTACK:		ApplyActionAttack(action); break;
	case MSG_BATTLE_MOVE:		ApplyActionMove(action);   break;
	case MSG_BATTLE_SKIP:		ApplyActionSkip(action);   break;
	case MSG_BATTLE_END_TURN:	ApplyActionEnd(action);    break;
	case MSG_BATTLE_MORALE:		ApplyActionMorale(action); break;

	case MSG_BATTLE_TOWER:		ApplyActionTower(action); break;
	case MSG_BATTLE_CATAPULT:	ApplyActionCatapult(action); break;

	case MSG_BATTLE_RETREAT:	ApplyActionRetreat(action); break;
	case MSG_BATTLE_SURRENDER:	ApplyActionSurrender(action); break;

	case MSG_BATTLE_AUTO:		ApplyActionAutoBattle(action); break;

	default: break;
    }

#ifdef WITH_NET
    switch(action.GetID())
    {
	case MSG_BATTLE_MOVE:
	case MSG_BATTLE_SKIP:
	case MSG_BATTLE_END_TURN:
	case MSG_BATTLE_MORALE:
	case MSG_BATTLE_TOWER:
	case MSG_BATTLE_CATAPULT:
	    if(Network::isRemoteClient())
    	    {
		if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendAction(army1.GetColor(), action);
    		if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendAction(army2.GetColor(), action);
	    }
	    break;

	default: break;
    }
#endif
}

void Battle2::Arena::ApplyActionSpellCast(Action & action)
{
    u8 byte8;
    action.Pop(byte8);

    const Spell spell(byte8);
    HeroBase* current_commander = GetCurrentCommander();

    if(current_commander && !current_commander->Modes(Heroes::SPELLCASTED) &&
	current_commander->CanCastSpell(spell) && spell.isCombat())
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, current_commander->GetName() << ", color: " << \
	    Color::String(current_commander->GetColor()) << ", spell: " << spell.GetName());

	// uniq spells action
	switch(spell())
	{
	    case Spell::TELEPORT:
		SpellActionTeleport(action);
		break;

	    case Spell::EARTHQUAKE:
		SpellActionEarthQuake(action);
		break;

	    case Spell::MIRRORIMAGE:
		SpellActionMirrorImage(action);
		break;

	    case Spell::SUMMONEELEMENT:
	    case Spell::SUMMONAELEMENT:
	    case Spell::SUMMONFELEMENT:
	    case Spell::SUMMONWELEMENT:
		SpellActionSummonElemental(action, spell);
		break;

	    default:
		SpellActionDefaults(action, spell);
		break;
	}

	current_commander->SetModes(Heroes::SPELLCASTED);
	current_commander->SpellCasted(spell);

	// save spell for "eagle eye" capability
	usage_spells.Append(spell);

#ifdef WITH_NET
	if(Network::isRemoteClient())
	{
	    if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendBoard(army1.GetColor(), *this);
	    if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendBoard(army2.GetColor(), *this);
	}
#endif
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_WARN, spell.GetName() << ", " << "incorrect param");
    }
}

void Battle2::Arena::ApplyActionAttack(Action & action)
{
    u16 id1, id2, dst;
    u8 dir;

    action.Pop(id1);
    action.Pop(id2);
    action.Pop(dst);
    action.Pop(dir);

    Battle2::Stats* b1 = GetTroopID(id1);
    Battle2::Stats* b2 = GetTroopID(id2);

    if(b1 && b1->isValid() &&
	b2 && b2->isValid() &&
	(b1->GetColor() != b2->GetColor() || b2->Modes(SP_HYPNOTIZE)))
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, b1->Info() << " to " << b2->Info());

	// fix dst
	if(dst != b2->GetPosition() && dst != b2->GetTailIndex())
	    dst = b2->GetPosition();

	// reset blind
	if(b2->Modes(SP_BLIND)) b2->ResetBlind();

	const bool handfighting = Stats::isHandFighting(*b1, *b2);
	// check position
	if(b1->isArchers() || handfighting)
	{
	    // attack
	    BattleProcess(*b1, *b2, dst, dir);

	    if(b2->isValid())
	    {
		// defense answer
		if(handfighting && !b1->isHideAttack() && b2->AllowResponse())
		{
		    BattleProcess(*b2, *b1);
		    b2->SetResponse();
		}

		// twice attack
		if(b1->isValid() && b1->isTwiceAttack() && !b1->Modes(IS_PARALYZE_MAGIC))
		{
		    DEBUG(DBG_BATTLE, DBG_TRACE, "twice attack");
		    BattleProcess(*b1, *b2);
		}
	    }
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << \
		b1->Info() << " and " << b2->Info());
	}

	if(!Settings::Get().ExtBattleTroopDirection())
	{
	    b1->UpdateDirection();
	    b2->UpdateDirection();
	}

#ifdef WITH_NET
	if(Network::isRemoteClient())
	{
	    if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendBoard(army1.GetColor(), *this);
	    if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendBoard(army2.GetColor(), *this);
	}
#endif
    }
    else
    	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << id1 << ", " << id2);
}

void Battle2::Arena::ApplyActionMove(Action & action)
{
    u16 id, dst, size;

    action.Pop(id);
    action.Pop(dst);
    action.Pop(size);

    Battle2::Stats* b = GetTroopID(id);
    Cell* cell = GetCell(dst);

    if(b && b->isValid() &&
	cell && cell->isPassable(*b, false))
    {
	b->UpdateDirection(*cell);

	// change pos to tail
	if(b->isWide() && !cell->isPassable(*b, true))
	{
	    dst = Board::GetIndexDirection(dst, b->isReflect() ? LEFT : RIGHT);
	    cell = GetCell(dst);
	}

	DEBUG(DBG_BATTLE, DBG_TRACE, b->Info() << " to " << "dst: " << dst);

	// force check fly
	if(b->troop.isFly())
	{
	    if(interface) interface->RedrawActionFly(*b, dst);
	}
	else
	{
	    std::vector<u16> path;

	    // check path
	    if(0 == size)
	    {
		GetPath(*b, dst, path);
		size = path.size();
		action.Reset();
		action.SetID(MSG_BATTLE_MOVE);
		action.Push(b->GetID());
		action.Push(dst);
		action.Push(size);
		for(u16 ii = 0; ii < size; ++ii)
		    action.Push(path[ii]);
	    }
	    else
	    {
		path.resize(size, 0);
		for(u16 ii = 0; ii < size; ++ii) action.Pop(path[ii]);
	    }

	    if(path.empty())
	    {
		DEBUG(DBG_BATTLE, DBG_WARN, "path empty, " << b->Info() << " to " << "dst: " << dst);
		return;
	    }

	    if(interface) interface->RedrawActionMove(*b, path);
	    else
    	    if(bridge)
    	    {
		for(std::vector<u16>::const_iterator
		    it = path.begin(); it != path.end(); ++it)
		    if(bridge->NeedAction(*b, *it))
			bridge->Action(*b, *it);
	    }
	}

	b->position = dst;
	b->UpdateDirection();
    }
    else
    {
    	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "id: " << id << ", dst: " << dst);
    }
}

void Battle2::Arena::ApplyActionSkip(Action & action)
{
    u16 id;
    u8 hard;
    action.Pop(id);
    action.Pop(hard);

    Battle2::Stats* battle = GetTroopID(id);
    if(battle && battle->isValid())
    {
	if(!battle->Modes(TR_MOVED))
	{
	    if(hard)
	    {
		battle->SetModes(TR_HARDSKIP);
		battle->SetModes(TR_SKIPMOVE);
		battle->SetModes(TR_MOVED);
	    }
	    else
		battle->SetModes(battle->Modes(TR_SKIPMOVE) ? TR_MOVED : TR_SKIPMOVE);

	    if(interface) interface->RedrawActionSkipStatus(*battle);

	    DEBUG(DBG_BATTLE, DBG_TRACE, battle->Info());
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_WARN, "id: " << id << " moved");
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "id: " << id);
}

void Battle2::Arena::ApplyActionEnd(Action & action)
{
    u16 id;

    action.Pop(id);

    Battle2::Stats* battle = GetTroopID(id);

    if(battle)
    {
	if(!battle->Modes(TR_MOVED))
	{
	    battle->SetModes(TR_MOVED);

	    if(battle->Modes(TR_SKIPMOVE) && interface) interface->RedrawActionSkipStatus(*battle);

	    DEBUG(DBG_BATTLE, DBG_TRACE, battle->Info());
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_WARN, "id: " << id << " moved");
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "id: " << id);
}

void Battle2::Arena::ApplyActionMorale(Action & action)
{
    u16 id;
    u8  morale;

    action.Pop(id);
    action.Pop(morale);

    Battle2::Stats* b = GetTroopID(id);

    if(b && b->isValid())
    {
	// good morale
	if(morale && b->Modes(TR_MOVED) && b->Modes(MORALE_GOOD))
	{
	    b->ResetModes(TR_MOVED);
    	    b->ResetModes(MORALE_GOOD);
        }
	// bad morale
        else
	if(!morale && !b->Modes(TR_MOVED) && b->Modes(MORALE_BAD))
        {
	    b->SetModes(TR_MOVED);
	    b->ResetModes(MORALE_BAD);
	}

	if(interface) interface->RedrawActionMorale(*b, morale);

	DEBUG(DBG_BATTLE, DBG_TRACE, (morale ? "good" : "bad") << " to " << b->Info());
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "id: " << id);
}

void Battle2::Arena::ApplyActionRetreat(Action & action)
{
    if(!result_game || !GetCurrentCommander()) return;

    if(CanRetreatOpponent(current_color))
    {
	if(army1.GetColor() == current_color && result_game)
    	{
    	    result_game->army1 = RESULT_RETREAT;
    	}
    	else
    	if(army2.GetColor() == current_color && result_game)
    	{
    	    result_game->army2 = RESULT_RETREAT;
    	}
	DEBUG(DBG_BATTLE, DBG_TRACE, "color: " << Color::String(current_color));
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle2::Arena::ApplyActionSurrender(Action & action)
{
    if(!result_game || !GetCurrentCommander()) return;

    if(CanSurrenderOpponent(current_color))
    {
	Funds cost;

    	if(army1.GetColor() == current_color)
		cost.gold = army1.GetSurrenderCost();
    	else
    	if(army2.GetColor() == current_color)
		cost.gold = army2.GetSurrenderCost();
        
    	if(world.GetKingdom(current_color).AllowPayment(cost))
    	{
	    if(army1.GetColor() == current_color)
    	    {
		    result_game->army1 = RESULT_SURRENDER;
		    world.GetKingdom(current_color).OddFundsResource(cost);
		    world.GetKingdom(army2.GetColor()).AddFundsResource(cost);
	    }
	    else
	    if(army2.GetColor() == current_color)
	    {
		    result_game->army2 = RESULT_SURRENDER;
		    world.GetKingdom(current_color).OddFundsResource(cost);
		    world.GetKingdom(army1.GetColor()).AddFundsResource(cost);
	    }
	    DEBUG(DBG_BATTLE, DBG_TRACE, "color: " << Color::String(current_color));
    	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle2::Arena::TargetsApplyDamage(Stats & attacker, Stats & defender, TargetsInfo & targets)
{
    TargetsInfo::iterator it = targets.begin();

    for(; it != targets.end(); ++it)
    {
	TargetInfo & target = *it;
	if(target.defender) target.killed = target.defender->ApplyDamage(attacker, target.damage);
    }
}

Battle2::TargetsInfo Battle2::Arena::GetTargetsForDamage(Stats & attacker, Stats & defender, u16 dst)
{
    TargetsInfo targets;
    targets.reserve(8);

    Stats* enemy = NULL;
    Cell* cell = NULL;
    TargetInfo res;

    // first target
    res.defender = &defender;
    res.damage = attacker.GetDamage(defender);
    targets.push_back(res);

    // long distance attack
    if(attacker.isDoubleCellAttack())
    {
        const direction_t dir = Board::GetDirection(attacker.position, dst);
        if(!defender.isWide() || 0 == ((RIGHT | LEFT) & dir))
	{
	    if(NULL != (cell = GetCell(dst, dir)) &&
		NULL != (enemy = GetTroopBoard(cell->index)) && enemy != &defender)
    	    {
		res.defender = enemy;
		res.damage = attacker.GetDamage(*enemy);
		targets.push_back(res);
	    }
        }
    }
    else
    // around hydra
    if(attacker.troop() == Monster::HYDRA)
    {
	std::vector<Stats*> v;
	v.reserve(8);
	for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
	{
	    if(NULL != (cell = GetCell(attacker.position, dir)) &&
		NULL != (enemy = GetTroopBoard(cell->index)) && enemy != &defender && enemy->GetColor() != attacker.GetColor())
        	    v.push_back(enemy);

	    if(NULL != (cell = GetCell(attacker.GetTailIndex(), dir)) &&
		NULL != (enemy = GetTroopBoard(cell->index)) && enemy != &defender && enemy->GetColor() != attacker.GetColor())
        	    v.push_back(enemy);
	}
	std::unique(v.begin(), v.end());
	for(u8 ii = 0; ii < v.size(); ++ii)
    	{
	    enemy = v[ii];
	    res.defender = enemy;
	    res.damage = attacker.GetDamage(*enemy);
	    targets.push_back(res);
	}
    }
    else
    // lich cloud damages
    if((attacker.troop() == Monster::LICH ||
	attacker.troop() == Monster::POWER_LICH) && !attacker.isHandFighting())
    {
	for(direction_t dir = TOP_LEFT; dir < CENTER; ++dir)
	{
	    if(NULL != (cell = GetCell(defender.position, dir)) &&
		NULL != (enemy = GetTroopBoard(cell->index)) && enemy != &defender)
    	    {
		res.defender = enemy;
		res.damage = attacker.GetDamage(*enemy);
		targets.push_back(res);
	    }
	}
    }

    return targets;
}

void Battle2::Arena::TargetsApplySpell(const HeroBase* hero, const Spell & spell, TargetsInfo & targets)
{
    DEBUG(DBG_BATTLE, DBG_TRACE, "targets: " << targets.size());

    TargetsInfo::iterator it = targets.begin();

    for(; it != targets.end(); ++it)
    {
	TargetInfo & target = *it;
	if(target.defender) target.defender->ApplySpell(spell, hero, target);
    }
}

Battle2::TargetsInfo Battle2::Arena::GetTargetsForSpells(const HeroBase* hero, const Spell & spell, const u16 dst)
{
    TargetsInfo targets;
    targets.reserve(8);

    TargetInfo res;
    Stats* target = GetTroopBoard(dst);

    // from spells
    switch(spell())
    {
	case Spell::CHAINLIGHTNING:
	case Spell::COLDRING:
	    // skip center
	    target = NULL;
	    break;

	default: break;
    }

    // first target
    if(target && target->AllowApplySpell(spell, hero))
    {
	res.defender = target;
	targets.push_back(res);
    }

    // resurrect spell? get target from graveyard
    if(NULL == target && isAllowResurrectFromGraveyard(spell, dst))
    {
        target = GetTroopID(graveyard.GetLastTroopIDFromCell(dst));

	if(target && target->AllowApplySpell(spell, hero))
	{
	    res.defender = target;
	    targets.push_back(res);
	}
    }
    else
    // check other spells
    switch(spell())
    {
	case Spell::CHAINLIGHTNING:
        {
	    std::vector<u16> trgts;
	    std::vector<u16> reslt;
	    std::vector<u16>::iterator it1, it2;
	    u16 index = dst;
	    trgts.push_back(index);

	    // find targets
	    for(u8 ii = 0; ii < 3; ++ii)
	    {
		GetNearestTroops(index, reslt, &trgts);
		if(reslt.empty()) break;
		index = reslt.size() > 1 ? *Rand::Get(reslt) : reslt.front();
		trgts.push_back(index);
		reslt.clear();
	    }

	    // save targets
	    it1 = trgts.begin();
	    it2 = trgts.end();
	    for(; it1 != it2; ++it1)
	    {
		Stats* target = GetTroopBoard(*it1);
		if(target)
		{
		    res.defender = target;
		    // store temp priority for calculate damage
		    res.damage = std::distance(trgts.begin(), it1);
		    targets.push_back(res);
		}
	    }
	}
	break;

	// check abroads
	case Spell::FIREBALL:
	case Spell::METEORSHOWER:
	case Spell::COLDRING:
	case Spell::FIREBLAST:
	{
	    std::vector<u16> positions;
	    u8 radius = (spell == Spell::FIREBLAST ? 2 : 1);
	    board.GetAbroadPositions(dst, radius, false, positions);
	    std::vector<u16>::const_iterator it1 = positions.begin();
	    std::vector<u16>::const_iterator it2 = positions.end();
            for(; it1 != it2; ++it1)
            {
		Stats* target = GetTroopBoard(*it1);
		if(target && target->AllowApplySpell(spell, hero))
		{
		    res.defender = target;
		    targets.push_back(res);
		}
	    }
	}
	break;

	// check all troops
	case Spell::DEATHRIPPLE:
	case Spell::DEATHWAVE:
	case Spell::ELEMENTALSTORM:
	case Spell::HOLYWORD:
	case Spell::HOLYSHOUT:
	case Spell::ARMAGEDDON:
	case Spell::MASSBLESS:
	case Spell::MASSCURE:
	case Spell::MASSCURSE:
	case Spell::MASSDISPEL:
	case Spell::MASSHASTE:
	case Spell::MASSSHIELD:
	case Spell::MASSSLOW:
	{
	    Board::const_iterator it1 = board.begin();
	    Board::const_iterator it2 = board.end();
            for(; it1 != it2; ++it1)
            {
		Stats* target = GetTroopBoard((*it1).index);
		if(target && target->GetPosition() != dst && target->AllowApplySpell(spell, hero))
		{
		    res.defender = target;
		    targets.push_back(res);
		}
	    }
	}
        break;

	default: break;
    }

    // remove resistent magic troop
    TargetsInfo::iterator it = targets.begin();
    while(it != targets.end())
    {
	const u8 resist = (*it).defender->GetMagicResist(spell, hero ? hero->GetPower() : 0);

	if(0 < resist && 100 > resist && resist >= Rand::Get(1, 100))
	{
	    if(interface) interface->RedrawActionResistSpell(*(*it).defender);
	
	    // erase(it)
	    if(it + 1 != targets.end()) std::swap(*it, targets.back());
	    targets.pop_back();
	}
	else ++it;
    }

    return targets;
}

void Battle2::Arena::ApplyActionTower(Action & action)
{
    u8 type;
    u16 id;

    action.Pop(type);
    action.Pop(id);

    Battle2::Stats* b2 = GetTroopID(id);
    Tower* tower = GetTower(type);

    if(b2 && b2->isValid() && tower)
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, "tower: " << static_cast<int>(type) << \
		", attack to " << b2->Info());

	Stats* b1 = tower->GetBattleStats();
	TargetInfo target;
	target.defender = b2;
	target.damage = b1->GetDamage(*b2);

	if(interface) interface->RedrawActionTowerPart1(*tower, *b2);
	target.killed = b2->ApplyDamage(*b1, target.damage);
	if(interface) interface->RedrawActionTowerPart2(*tower, target);

	if(b2->Modes(SP_BLIND)) b2->ResetBlind();
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "tower: " << static_cast<int>(type) << ", id: " << id);
}

void Battle2::Arena::ApplyActionCatapult(Action & action)
{
    if(catapult)
    {
	u8 shots, target, damage;

	action.Pop(shots);

	while(shots--)
	{
	    action.Pop(target);
	    action.Pop(damage);

	    if(target)
	    {
		if(interface) interface->RedrawActionCatapult(target);
		SetCastleTargetValue(target, GetCastleTargetValue(target) - damage);
		DEBUG(DBG_BATTLE, DBG_TRACE, "target: " << static_cast<int>(target));
	    }
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle2::Arena::ApplyActionAutoBattle(Action & action)
{
    u8 color;
    action.Pop(color);

    if(current_color == color)
    {
	if(auto_battle & color)
	{
	    if(interface) interface->SetStatus(_("Set auto battle off"), true);
	    auto_battle &= ~color;
	}
	else
	{
	    if(interface) interface->SetStatus(_("Set auto battle on"), true);
	    auto_battle |= color;
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle2::Arena::SpellActionSummonElemental(Action & a, const Spell & spell)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::NETWORK)) || Network::isRemoteClient())
    {
#endif
	Stats* elem = CreateElemental(spell);
	if(interface) interface->RedrawActionSummonElementalSpell(*elem);

#ifdef WITH_NET
	if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendSummonElementalSpell(army1.GetColor(), spell, *elem);
	if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendSummonElementalSpell(army2.GetColor(), spell, *elem);
    }
    else
    if(Network::isLocalClient())
    {
	u16 id;
	a.Pop(id);

	Stats* elem = CreateElemental(spell);
	if(elem)
	{
	    elem->Unpack(a);

	    if(id != elem->GetID())
    		    DEBUG(DBG_BATTLE, DBG_WARN, "internal error");

	    if(interface) interface->RedrawActionSummonElementalSpell(*elem);
	}
	else
	{
    	    DEBUG(DBG_BATTLE, DBG_WARN, "is NULL");
	}
    }
#endif
}

void Battle2::Arena::SpellActionDefaults(Action & a, const Spell & spell)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::NETWORK)) || Network::isRemoteClient())
    {
#endif
        const HeroBase* current_commander = GetCurrentCommander();
	if(!current_commander) return;

	u16 id, dst;
	a.Pop(id);
	a.Pop(dst);

	TargetsInfo targets = GetTargetsForSpells(current_commander, spell, dst);
	if(interface) interface->RedrawActionSpellCastPart1(spell, dst, current_commander->GetName(), targets);

    	TargetsApplySpell(current_commander, spell, targets);
	if(interface) interface->RedrawActionSpellCastPart2(spell, targets);

#ifdef WITH_NET
	if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendSpell(army1.GetColor(), 0, dst, spell, targets);
	if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendSpell(army2.GetColor(), 0, dst, spell, targets);
    }
    else
    if(Network::isLocalClient())
    {
	u16 id, dst;
	u8 color;
	std::string name;

	a.Pop(id);
	a.Pop(dst);
	a.Pop(color);
        TargetsInfo targets(a, *this);

        Stats* monster = id ? GetTroopID(id) : NULL;
        const HeroBase* hero = id == 0 && color ? GetCommander(color) : NULL;

        if(monster)
            name = monster->GetName();
        else
        if(hero)
            name = hero->GetName();

	if(interface)
	{
	    interface->RedrawActionSpellCastPart1(spell, dst, name, targets);
    	    if(monster && targets.size())
        	interface->RedrawActionMonsterSpellCastStatus(*monster, targets.front());
        }
	TargetsApplySpell(hero, spell, targets);
        if(interface) interface->RedrawActionSpellCastPart2(spell, targets);
    }
#endif
}

void Battle2::Arena::SpellActionTeleport(Action & a)
{
    u16 src, dst;

    a.Pop(src);
    a.Pop(dst);

    Stats* b = GetTroopBoard(src);
    Cell* cell = GetCell(dst);
    const Spell spell(Spell::TELEPORT);

    if(b)
    {
        if(b->isWide() && !cell->isPassable(*b, true))
	    dst = Board::GetIndexDirection(dst, b->isReflect() ? LEFT : RIGHT);

	if(interface) interface->RedrawActionTeleportSpell(*b, dst);
	b->position = dst;
	b->UpdateDirection();

	DEBUG(DBG_BATTLE, DBG_TRACE, "spell: " << spell.GetName() << ", src: " << src << ", dst: " << dst);
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_WARN, "spell: " << spell.GetName() << " false");
    }

#ifdef WITH_NET
    if(Network::isRemoteClient())
    {
	if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendTeleportSpell(army1.GetColor(), src, dst);
	if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendTeleportSpell(army2.GetColor(), src, dst);
    }
#endif
}

void Battle2::Arena::SpellActionEarthQuake(Action & a)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::NETWORK)) || Network::isRemoteClient())
    {
#endif
	std::vector<u8> targets;
	targets.reserve(8);

	// check walls
	if(0 != board[8].object)  targets.push_back(CAT_WALL1);
	if(0 != board[29].object) targets.push_back(CAT_WALL2);
	if(0 != board[73].object) targets.push_back(CAT_WALL3);
	if(0 != board[96].object) targets.push_back(CAT_WALL4);

	// check right/left towers
	if(towers[0] && towers[0]->isValid()) targets.push_back(CAT_TOWER1);
	if(towers[2] && towers[2]->isValid()) targets.push_back(CAT_TOWER2);

	if(interface) interface->RedrawActionEarthQuakeSpell(targets);

	// FIXME: Arena::SpellActionEarthQuake: check hero spell power

	// apply random damage
	if(0 != board[8].object)  board[8].object = Rand::Get(board[8].object);
	if(0 != board[29].object) board[29].object = Rand::Get(board[29].object);
	if(0 != board[73].object) board[73].object = Rand::Get(board[73].object);
	if(0 != board[96].object) board[96].object = Rand::Get(board[96].object);

	if(towers[0] && towers[0]->isValid() && Rand::Get(1)) towers[0]->SetDestroy();
	if(towers[2] && towers[2]->isValid() && Rand::Get(1)) towers[2]->SetDestroy();

	DEBUG(DBG_BATTLE, DBG_TRACE, "spell: " << Spell(Spell::EARTHQUAKE).GetName() << ", targets: " << targets.size());

#ifdef WITH_NET
	if(Game::REMOTE == army1.GetControl()) FH2Server::Get().BattleSendEarthQuakeSpell(army1.GetColor(), targets);
	if(Game::REMOTE == army2.GetControl()) FH2Server::Get().BattleSendEarthQuakeSpell(army2.GetColor(), targets);
    }
    else
    if(Network::isLocalClient())
    {
        u32 size;
        a.Pop(size);

	std::vector<u8> targets(size);
	for(std::vector<u8>::iterator
	    it = targets.begin(); it != targets.end(); ++it)
            a.Pop(*it);

	if(interface) interface->RedrawActionEarthQuakeSpell(targets);
    }
#endif
}

void Battle2::Arena::SpellActionMirrorImage(Action & a)
{
    u16 who;
    a.Pop(who);
    Stats* b = GetTroopBoard(who);
    
    if(b)
    {
#ifdef WITH_NET
	if(! (Settings::Get().GameType(Game::NETWORK)) || Network::isRemoteClient())
	{
#endif
	    std::vector<u16> v;
	    board.GetAbroadPositions(b->position, 4, true, v);

	    std::vector<u16>::const_iterator it;
	    for(it = v.begin(); it != v.end(); ++it)
	    {
    		const Cell* cell = GetCell(*it);
    		if(cell && cell->isPassable(*b, true))
		{
		    if(b->isWide() && (b->position + 1 == *it))
			continue; else break;
		}
	    }

	    if(it != v.end())
	    {
    		DEBUG(DBG_BATTLE, DBG_TRACE, "set position: " << *it);
		if(interface) interface->RedrawActionMirrorImageSpell(*b, *it);

		Stats* image = CreateMirrorImage(*b, *it);
#ifdef WITH_NET
		if(image)
		{
		    if(Game::REMOTE == army1.GetControl())
			FH2Server::Get().BattleSendMirrorImageSpell(army1.GetColor(), who, *it, *image);
		    if(Game::REMOTE == army2.GetControl())
			FH2Server::Get().BattleSendMirrorImageSpell(army2.GetColor(), who, *it, *image);
		}
		else
		{
    		    DEBUG(DBG_BATTLE, DBG_WARN, "is NULL");
		}
#endif
	    }
	    else
	    {
    		if(interface) interface->SetStatus(_("spell failed!"), true);
    		DEBUG(DBG_BATTLE, DBG_WARN, "new position not found!");
	    }
#ifdef WITH_NET
	}
	else
	if(Network::isLocalClient())
	{
	    u16 id, dst;

	    a.Pop(dst);
	    a.Pop(id);

	    Stats* image = CreateMirrorImage(*b, dst);
	    if(image)
	    {
		image->Unpack(a);

		if(id != image->GetID())
    		    DEBUG(DBG_BATTLE, DBG_WARN, "internal error");

		if(interface) interface->RedrawActionMirrorImageSpell(*b, dst);
	    }
	    else
	    {
    		DEBUG(DBG_BATTLE, DBG_WARN, "is NULL");
	    }
	}
#endif
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_WARN, "false");
    }
}
