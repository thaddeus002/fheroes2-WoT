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
#include "battle_troop.h"
#include "battle_arena.h"
#include "battle_tower.h"
#include "battle_bridge.h"
#include "battle_catapult.h"
#include "battle_interface.h"
#include "server.h"

void Battle::Actions::AddedAutoBattleAction(u8 color)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_AUTO);
    action << color;

    push_back(action);
}

void Battle::Actions::AddedSurrenderAction(void)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_SURRENDER);

    push_back(action);
}

void Battle::Actions::AddedRetreatAction(void)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_RETREAT);

    push_back(action);
}

void Battle::Actions::AddedCastAction(const Spell & spell, s16 dst)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_CAST);
    // id, hero or monster
    action << spell() << static_cast<u16>(0) << dst;

    push_back(action);
}

void Battle::Actions::AddedCastMirrorImageAction(s16 who)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_CAST);
    action << static_cast<u8>(Spell::MIRRORIMAGE) << who;

    push_back(action);
}

void Battle::Actions::AddedCastTeleportAction(s16 src, s16 dst)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_CAST);
    action << static_cast<u8>(Spell::TELEPORT) << src << dst;

    push_back(action);
}

void Battle::Actions::AddedEndAction(const Unit & b)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_END_TURN);
    action << b.GetUID();

    push_back(action);
}

void Battle::Actions::AddedSkipAction(const Unit & b, bool hard)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_SKIP);
    action << b.GetUID() << static_cast<u8>(hard);

    push_back(action);
}

void Battle::Actions::AddedMoveAction(const Unit & b, s16 dst)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_MOVE);
    action << b.GetUID() << dst << static_cast<u16>(0);

    push_back(action);
}

void Battle::Actions::AddedAttackAction(const Unit & a, const Unit & d, s16 dst, u8 dir)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_ATTACK);
    action << a.GetUID() << d.GetUID() <<  dst << dir;

    push_back(action);
}

void Battle::Actions::AddedMoraleAction(const Unit & b, u8 state)
{
    QueueMessage action;
    action.SetID(MSG_BATTLE_MORALE);
    action << b.GetUID() << state;

    push_back(action);
}

void Battle::Arena::BattleProcess(Unit & attacker, Unit & defender, s16 dst, u8 dir)
{
    if(0 > dst) dst = defender.GetHeadIndex();

    if(dir)
    {
	if(attacker.isWide())
	{
	    if(!Board::isNearIndexes(attacker.GetHeadIndex(), dst))
		attacker.UpdateDirection(board[dst].GetPos());
	    if(defender.AllowResponse())
		defender.UpdateDirection(board[attacker.GetHeadIndex()].GetPos());
	}
	else
	{
	    attacker.UpdateDirection(board[dst].GetPos());
	    if(defender.AllowResponse())
		defender.UpdateDirection(board[attacker.GetHeadIndex()].GetPos());
	}
    }
    else
	attacker.UpdateDirection(board[dst].GetPos());

    TargetsInfo targets = GetTargetsForDamage(attacker, defender, dst);

#ifdef WITH_NET
    if(Network::isRemoteClient())
    {
	if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendAttack(army1->GetColor(), attacker, defender, dst, targets);
	if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendAttack(army2->GetColor(), attacker, defender, dst, targets);
    }
#endif

    if(Board::isReflectDirection(dir) != attacker.isReflect())
	attacker.UpdateDirection(board[dst].GetPos());

    if(interface) interface->RedrawActionAttackPart1(attacker, defender, targets);

    TargetsApplyDamage(attacker, defender, targets);
    if(interface) interface->RedrawActionAttackPart2(attacker, targets);

    const Spell spell = attacker.GetSpellMagic();

    // magic attack
    if(defender.isValid() && spell.isValid())
    {
	const std::string name(attacker.GetName());
	targets = GetTargetsForSpells(attacker.GetCommander(), spell, defender.GetHeadIndex());

	if(targets.size())
	{
	    if(interface) interface->RedrawActionSpellCastPart1(spell, defender.GetHeadIndex(), NULL, name, targets);

	    // magic attack not depends from hero
	    TargetsApplySpell(NULL, spell, targets);
	    if(interface) interface->RedrawActionSpellCastPart2(spell, targets);
	    if(interface) interface->RedrawActionMonsterSpellCastStatus(attacker, targets.front());

#ifdef WITH_NET
	    if(Network::isRemoteClient())
    	    {
		if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendSpell(army1->GetColor(), attacker.GetUID(), defender.GetHeadIndex(), spell, targets);
		if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendSpell(army2->GetColor(), attacker.GetUID(), defender.GetHeadIndex(), spell, targets);
	    }
#endif
	}
    }

    attacker.PostAttackAction(defender);
}

void Battle::Arena::ApplyAction(QueueMessage & action)
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
		if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendAction(army1->GetColor(), action);
    		if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendAction(army2->GetColor(), action);
	    }
	    break;

	default: break;
    }
#endif
}

void Battle::Arena::ApplyActionSpellCast(QueueMessage & action)
{
    u8 byte8;
    action >> byte8;

    const Spell spell(byte8);
    HeroBase* current_commander = GetCurrentForce().GetCommander();

    if(current_commander && current_commander->HaveSpellBook() &&
	! current_commander->Modes(Heroes::SPELLCASTED) &&
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
	    if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendBoard(army1->GetColor(), *this);
	    if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendBoard(army2->GetColor(), *this);
	}
#endif
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_INFO, spell.GetName() << ", " << "incorrect param");
    }
}

void Battle::Arena::ApplyActionAttack(QueueMessage & action)
{
    u32 uid1, uid2;
    s16 dst;
    u8 dir;

    action >> uid1 >> uid2 >> dst >> dir;

    Battle::Unit* b1 = GetTroopUID(uid1);
    Battle::Unit* b2 = GetTroopUID(uid2);

    if(b1 && b1->isValid() &&
	b2 && b2->isValid() &&
	(b1->GetColor() != b2->GetColor() || b2->Modes(SP_HYPNOTIZE)))
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, b1->String() << " to " << b2->String());

	// reset blind
	if(b2->Modes(SP_BLIND)) b2->ResetBlind();

	const bool handfighting = Unit::isHandFighting(*b1, *b2);
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

	    b1->UpdateDirection();
	    b2->UpdateDirection();
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << \
		b1->String(true) << " and " << b2->String(true));
	}

#ifdef WITH_NET
	if(Network::isRemoteClient())
	{
	    if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendBoard(army1->GetColor(), *this);
	    if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendBoard(army2->GetColor(), *this);
	}
#endif
    }
    else
    	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << uid1 << ", " << uid2);
}

void Battle::Arena::ApplyActionMove(QueueMessage & action)
{
    u32 uid = 0;
    s16 dst = -1;
    u16 size = 0;

    action >> uid >> dst >> size;

    Battle::Unit* b = GetTroopUID(uid);
    Cell* cell = Board::GetCell(dst);

    if(b && b->isValid() &&
	cell && cell->isPassable3(*b, false))
    {
	Position pos1, pos2;
	const s16 head = b->GetHeadIndex();
	pos1 = Position::GetCorrect(*b, dst);

	DEBUG(DBG_BATTLE, DBG_TRACE, b->String() << ", dst: " << dst << ", (head: " <<
		    pos1.GetHead()->GetIndex() << ", tail: " << (b->isWide() ? pos1.GetTail()->GetIndex() : -1) << ")");

	// force check fly
	if(static_cast<ArmyTroop*>(b)->isFly())
	{
	    if(b->UpdateDirection(pos1.GetRect()))
		pos1.SetReflection();
	    if(interface) interface->RedrawActionFly(*b, pos1);
	    pos2 = pos1;
	}
	else
	{
	    Indexes path;

	    // check path
	    if(0 == size)
	    {
		path = GetPath(*b, pos1);
		size = path.size();

		action.Reset();
		action.SetID(MSG_BATTLE_MOVE);
		action << b->GetUID() << dst;

		action << size;	// FIXME: serialize
		for(u16 ii = 0; ii < size; ++ii) action << path[ii];
	    }
	    else
	    {
		path.resize(size, 0);
		for(u16 ii = 0; ii < size; ++ii) action >> path[ii];
	    }

	    if(path.empty())
	    {
		DEBUG(DBG_BATTLE, DBG_WARN, "path empty, " << b->String() << " to " << "dst: " << dst);
		return;
	    }

	    if(interface) interface->RedrawActionMove(*b, path);
	    else
    	    if(bridge)
    	    {
		for(Indexes::const_iterator
		    it = path.begin(); it != path.end(); ++it)
		    if(bridge->NeedAction(*b, *it)) bridge->Action(*b, *it);
	    }

	    if(b->isWide())
	    {
        	const s16 & dst1 = path.back();
        	const s16 & dst2 = 1 < path.size() ? path[path.size() - 2] : head;

		pos2.Set(dst1, b->isWide(), RIGHT_SIDE & Board::GetDirection(dst1, dst2));
	    }
	    else
		pos2.Set(path.back(), false, b->isReflect());
	}

	b->SetPosition(pos2);
	b->UpdateDirection();
    }
    else
    {
    	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "uid: " << uid << ", dst: " << dst);
    }
}

void Battle::Arena::ApplyActionSkip(QueueMessage & action)
{
    u32 uid;
    u8 hard;
    action >> uid >> hard;

    Battle::Unit* battle = GetTroopUID(uid);
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

	    DEBUG(DBG_BATTLE, DBG_TRACE, battle->String());
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_WARN, "uid: " << uid << " moved");
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "uid: " << uid);
}

void Battle::Arena::ApplyActionEnd(QueueMessage & action)
{
    u32 uid;
    action >> uid;

    Battle::Unit* battle = GetTroopUID(uid);

    if(battle)
    {
	if(!battle->Modes(TR_MOVED))
	{
	    battle->SetModes(TR_MOVED);

	    if(battle->Modes(TR_SKIPMOVE) && interface) interface->RedrawActionSkipStatus(*battle);

	    DEBUG(DBG_BATTLE, DBG_TRACE, battle->String());
	}
	else
	{
	    DEBUG(DBG_BATTLE, DBG_INFO, "uid: " << uid << " moved");
	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_INFO, "incorrect param: " << "uid: " << uid);
}

void Battle::Arena::ApplyActionMorale(QueueMessage & action)
{
    u32 uid;
    u8  morale;
    action >> uid >> morale;

    Battle::Unit* b = GetTroopUID(uid);

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

	DEBUG(DBG_BATTLE, DBG_TRACE, (morale ? "good" : "bad") << " to " << b->String());
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "uid: " << uid);
}

void Battle::Arena::ApplyActionRetreat(QueueMessage & action)
{
    if(CanRetreatOpponent(current_color))
    {
	if(army1->GetColor() == current_color)
    	{
    	    result_game.army1 = RESULT_RETREAT;
    	}
    	else
    	if(army2->GetColor() == current_color)
    	{
    	    result_game.army2 = RESULT_RETREAT;
    	}
	DEBUG(DBG_BATTLE, DBG_TRACE, "color: " << Color::String(current_color));
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle::Arena::ApplyActionSurrender(QueueMessage & action)
{
    if(CanSurrenderOpponent(current_color))
    {
	Funds cost;

    	if(army1->GetColor() == current_color)
		cost.gold = army1->GetSurrenderCost();
    	else
    	if(army2->GetColor() == current_color)
		cost.gold = army2->GetSurrenderCost();

    	if(world.GetKingdom(current_color).AllowPayment(cost))
    	{
	    if(army1->GetColor() == current_color)
    	    {
		result_game.army1 = RESULT_SURRENDER;
		world.GetKingdom(current_color).OddFundsResource(cost);
		world.GetKingdom(army2->GetColor()).AddFundsResource(cost);
	    }
	    else
	    if(army2->GetColor() == current_color)
	    {
		result_game.army2 = RESULT_SURRENDER;
		world.GetKingdom(current_color).OddFundsResource(cost);
		world.GetKingdom(army1->GetColor()).AddFundsResource(cost);
	    }
	    DEBUG(DBG_BATTLE, DBG_TRACE, "color: " << Color::String(current_color));
    	}
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param");
}

void Battle::Arena::TargetsApplyDamage(Unit & attacker, Unit & defender, TargetsInfo & targets)
{
    TargetsInfo::iterator it = targets.begin();

    for(; it != targets.end(); ++it)
    {
	TargetInfo & target = *it;
	if(target.defender) target.killed = target.defender->ApplyDamage(attacker, target.damage);
    }
}

Battle::TargetsInfo Battle::Arena::GetTargetsForDamage(Unit & attacker, Unit & defender, s16 dst)
{
    TargetsInfo targets;
    targets.reserve(8);

    Unit* enemy = NULL;
    Cell* cell = NULL;
    TargetInfo res;

    // first target
    res.defender = &defender;
    res.damage = attacker.GetDamage(defender);
    targets.push_back(res);

    // long distance attack
    if(attacker.isDoubleCellAttack())
    {
        const direction_t dir = Board::GetDirection(attacker.GetHeadIndex(), dst);
        if(!defender.isWide() || 0 == ((RIGHT | LEFT) & dir))
	{
	    if(NULL != (cell = Board::GetCell(dst, dir)) &&
		NULL != (enemy = cell->GetUnit()) && enemy != &defender)
    	    {
		res.defender = enemy;
		res.damage = attacker.GetDamage(*enemy);
		targets.push_back(res);
	    }
        }
    }
    else
    // around hydra
    if(attacker.GetID() == Monster::HYDRA)
    {
	std::vector<Unit*> v;
	v.reserve(8);

	const Indexes around = Board::GetAroundIndexes(attacker);

	for(Indexes::const_iterator it = around.begin(); it != around.end(); ++it)
	{
	    if(NULL != (enemy = Board::GetCell(*it)->GetUnit()) &&
		enemy != &defender && enemy->GetColor() != attacker.GetColor())
    	    {
		res.defender = enemy;
		res.damage = attacker.GetDamage(*enemy);
		targets.push_back(res);
	    }
	}
    }
    else
    // lich cloud damages
    if((attacker.GetID() == Monster::LICH ||
	attacker.GetID() == Monster::POWER_LICH) && !attacker.isHandFighting())
    {
	const Indexes around = Board::GetAroundIndexes(defender.GetHeadIndex());

	for(Indexes::const_iterator it = around.begin(); it != around.end(); ++it)
	{
	    if(NULL != (enemy = Board::GetCell(*it)->GetUnit()) && enemy != &defender)
    	    {
		res.defender = enemy;
		res.damage = attacker.GetDamage(*enemy);
		targets.push_back(res);
	    }
	}
    }

    return targets;
}

void Battle::Arena::TargetsApplySpell(const HeroBase* hero, const Spell & spell, TargetsInfo & targets)
{
    DEBUG(DBG_BATTLE, DBG_TRACE, "targets: " << targets.size());

    TargetsInfo::iterator it = targets.begin();

    for(; it != targets.end(); ++it)
    {
	TargetInfo & target = *it;
	if(target.defender) target.defender->ApplySpell(spell, hero, target);
    }
}

Battle::TargetsInfo Battle::Arena::GetTargetsForSpells(const HeroBase* hero, const Spell & spell, s16 dst)
{
    TargetsInfo targets;
    targets.reserve(8);

    TargetInfo res;
    Unit* target = GetTroopBoard(dst);

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
    if(NULL == target && GraveyardAllowResurrect(dst, spell))
    {
        target = GetTroopUID(graveyard.GetLastTroopUID(dst));

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
	    Indexes trgts;
	    trgts.reserve(12);
	    trgts.push_back(dst);

	    // find targets
	    for(u8 ii = 0; ii < 3; ++ii)
	    {
		const Indexes reslt = board.GetNearestTroopIndexes(dst, &trgts);
		if(reslt.empty()) break;
		trgts.push_back(reslt.size() > 1 ? *Rand::Get(reslt) : reslt.front());
	    }

	    // save targets
	    for(Indexes::iterator
		it = trgts.begin(); it != trgts.end(); ++it)
	    {
		Unit* target = GetTroopBoard(*it);

		if(target)
		{
		    res.defender = target;
		    // store temp priority for calculate damage
		    res.damage = std::distance(trgts.begin(), it);
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
	    const Indexes positions = Board::GetDistanceIndexes(dst, (spell == Spell::FIREBLAST ? 2 : 1));

	    for(Indexes::const_iterator
		it = positions.begin(); it != positions.end(); ++it)
            {
		Unit* target = GetTroopBoard(*it);
		if(target && target->AllowApplySpell(spell, hero))
		{
		    res.defender = target;
		    targets.push_back(res);
		}
	    }

	    // unique
	    targets.resize(std::distance(targets.begin(), std::unique(targets.begin(), targets.end())));
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
	    for(Board::iterator
		it = board.begin();  it != board.end(); ++it)
            {
		target = (*it).GetUnit();
		if(target && target->AllowApplySpell(spell, hero))
		{
		    res.defender = target;
		    targets.push_back(res);
		}
	    }

	    // unique
	    targets.resize(std::distance(targets.begin(), std::unique(targets.begin(), targets.end())));
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

void Battle::Arena::ApplyActionTower(QueueMessage & action)
{
    u8 type;
    u32 uid;

    action >> type >> uid;

    Battle::Unit* b2 = GetTroopUID(uid);
    Tower* tower = GetTower(type);

    if(b2 && b2->isValid() && tower)
    {
	DEBUG(DBG_BATTLE, DBG_TRACE, "tower: " << static_cast<int>(type) << \
		", attack to " << b2->String());

	TargetInfo target;
	target.defender = b2;
	target.damage = tower->GetDamage(*b2);

	if(interface) interface->RedrawActionTowerPart1(*tower, *b2);
	target.killed = b2->ApplyDamage(*tower, target.damage);
	if(interface) interface->RedrawActionTowerPart2(*tower, target);

	if(b2->Modes(SP_BLIND)) b2->ResetBlind();
    }
    else
	DEBUG(DBG_BATTLE, DBG_WARN, "incorrect param: " << "tower: " << static_cast<int>(type) << ", uid: " << uid);
}

void Battle::Arena::ApplyActionCatapult(QueueMessage & action)
{
    if(catapult)
    {
	u8 shots, target, damage;

	action >> shots;

	while(shots--)
	{
	    action >> target >> damage;

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

void Battle::Arena::ApplyActionAutoBattle(QueueMessage & action)
{
    u8 color;
    action >> color;

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

void Battle::Arena::SpellActionSummonElemental(QueueMessage & a, const Spell & spell)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::TYPE_NETWORK)) || Network::isRemoteClient())
    {
#endif
	Unit* elem = CreateElemental(spell);
	if(interface) interface->RedrawActionSummonElementalSpell(*elem);

#ifdef WITH_NET
	if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendSummonElementalSpell(army1->GetColor(), spell, *elem);
	if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendSummonElementalSpell(army2->GetColor(), spell, *elem);
    }
    else
    if(Network::isLocalClient())
    {
	u32 uid = 0;
	a >> uid;

	Unit* elem = CreateElemental(spell);
	if(elem)
	{
	    a >> *elem;

	    if(uid != elem->GetUID())
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

void Battle::Arena::SpellActionDefaults(QueueMessage & a, const Spell & spell)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::TYPE_NETWORK)) || Network::isRemoteClient())
    {
#endif
        const HeroBase* current_commander = GetCurrentCommander();
	if(!current_commander) return;

	s16 nop, dst;
	a >> nop >> dst;

	TargetsInfo targets = GetTargetsForSpells(current_commander, spell, dst);
	if(interface) interface->RedrawActionSpellCastPart1(spell, dst, current_commander, current_commander->GetName(), targets);

    	TargetsApplySpell(current_commander, spell, targets);
	if(interface) interface->RedrawActionSpellCastPart2(spell, targets);

#ifdef WITH_NET
	if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendSpell(army1->GetColor(), 0, dst, spell, targets);
	if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendSpell(army2->GetColor(), 0, dst, spell, targets);
    }
    else
    if(Network::isLocalClient())
    {
        TargetsInfo targets;
	u32 uid = 0;
	s16 dst = -1;
	u8 color;
	std::string name;

	a >> uid >> dst >> color >> targets;

        Unit* monster = uid ? GetTroopUID(uid) : NULL;
        const HeroBase* hero = uid == 0 && color ? GetCommander(color) : NULL;

        if(monster)
            name = monster->GetName();
        else
        if(hero)
            name = hero->GetName();

	if(interface)
	{
	    interface->RedrawActionSpellCastPart1(spell, dst, (monster ? NULL : hero), name, targets);
    	    if(monster && targets.size())
        	interface->RedrawActionMonsterSpellCastStatus(*monster, targets.front());
        }
	TargetsApplySpell(hero, spell, targets);
        if(interface) interface->RedrawActionSpellCastPart2(spell, targets);
    }
#endif
}

void Battle::Arena::SpellActionTeleport(QueueMessage & a)
{
    s16 src, dst;

    a >> src >> dst;

    Unit* b = GetTroopBoard(src);
    Cell* cell = Board::GetCell(dst);
    const Spell spell(Spell::TELEPORT);

    if(b)
    {
        if(b->isWide() && !cell->isPassable3(*b, true))
	    dst = Board::GetIndexDirection(dst, b->isReflect() ? LEFT : RIGHT);

	if(interface) interface->RedrawActionTeleportSpell(*b, dst);
	b->SetPosition(dst);

	DEBUG(DBG_BATTLE, DBG_TRACE, "spell: " << spell.GetName() << ", src: " << src << ", dst: " << dst);
    }
    else
    {
	DEBUG(DBG_BATTLE, DBG_WARN, "spell: " << spell.GetName() << " false");
    }

#ifdef WITH_NET
    if(Network::isRemoteClient())
    {
	if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendTeleportSpell(army1->GetColor(), src, dst);
	if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendTeleportSpell(army2->GetColor(), src, dst);
    }
#endif
}

void Battle::Arena::SpellActionEarthQuake(QueueMessage & a)
{
#ifdef WITH_NET
    if(! (Settings::Get().GameType(Game::TYPE_NETWORK)) || Network::isRemoteClient())
    {
#endif
	std::vector<u8> targets = GetCastleTargets();

	if(interface) interface->RedrawActionEarthQuakeSpell(targets);

	// FIXME: Arena::SpellActionEarthQuake: check hero spell power

	// apply random damage
	if(0 != board[8].GetObject())  board[8].SetObject(Rand::Get(board[8].GetObject()));
	if(0 != board[29].GetObject()) board[29].SetObject(Rand::Get(board[29].GetObject()));
	if(0 != board[73].GetObject()) board[73].SetObject(Rand::Get(board[73].GetObject()));
	if(0 != board[96].GetObject()) board[96].SetObject(Rand::Get(board[96].GetObject()));

	if(towers[0] && towers[0]->isValid() && Rand::Get(1)) towers[0]->SetDestroy();
	if(towers[2] && towers[2]->isValid() && Rand::Get(1)) towers[2]->SetDestroy();

	DEBUG(DBG_BATTLE, DBG_TRACE, "spell: " << Spell(Spell::EARTHQUAKE).GetName() << ", targets: " << targets.size());

#ifdef WITH_NET
	if(CONTROL_REMOTE & army1->GetControl()) FH2Server::Get().BattleSendEarthQuakeSpell(army1->GetColor(), targets);
	if(CONTROL_REMOTE & army2->GetControl()) FH2Server::Get().BattleSendEarthQuakeSpell(army2->GetColor(), targets);
    }
    else
    if(Network::isLocalClient())
    {
        u32 size;
        a >> size;

	std::vector<u8> targets(size);
	for(std::vector<u8>::iterator
	    it = targets.begin(); it != targets.end(); ++it)
            a >> *it;

	if(interface) interface->RedrawActionEarthQuakeSpell(targets);
    }
#endif
}

void Battle::Arena::SpellActionMirrorImage(QueueMessage & a)
{
    s16 who;
    a >> who;
    Unit* b = GetTroopBoard(who);

    if(b)
    {
#ifdef WITH_NET
	if(! (Settings::Get().GameType(Game::TYPE_NETWORK)) || Network::isRemoteClient())
	{
#endif
	    const Indexes distances = Board::GetDistanceIndexes(b->GetHeadIndex(), 4);

	    Indexes::const_iterator it = std::find_if(distances.begin(), distances.end(),
						std::bind2nd(std::ptr_fun(&Board::isValidMirrorImageIndex), b));

	    for(Indexes::const_iterator
		it = distances.begin(); it != distances.end(); ++it)
	    {
    		const Cell* cell = Board::GetCell(*it);
    		if(cell && cell->isPassable3(*b, true)) break;
	    }

	    if(it != distances.end())
	    {
		const Position pos = Position::GetCorrect(*b, *it);
		const s16 & dst = pos.GetHead()->GetIndex();
    		DEBUG(DBG_BATTLE, DBG_TRACE, "set position: " << dst);
		if(interface) interface->RedrawActionMirrorImageSpell(*b, dst);

		Unit* image = CreateMirrorImage(*b, dst);

		if(image)
		{
#ifdef WITH_NET
		    if(CONTROL_REMOTE & army1->GetControl())
			FH2Server::Get().BattleSendMirrorImageSpell(army1->GetColor(), who, dst, *image);
		    if(CONTROL_REMOTE & army2->GetControl())
			FH2Server::Get().BattleSendMirrorImageSpell(army2->GetColor(), who, dst, *image);
#endif
		}
		else
		{
    		    DEBUG(DBG_BATTLE, DBG_WARN, "is NULL");
		}
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
	    u32 uid = 0;
	    s16 dst = -1;

	    a >> dst >> uid;

	    Unit* image = CreateMirrorImage(*b, dst);
	    if(image)
	    {
		a >> *image;

		if(uid != image->GetUID())
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
