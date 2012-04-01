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

#ifndef H2BATTLE_ARENA_H
#define H2BATTLE_ARENA_H

#include <list>

#include "gamedefs.h"
#include "ai.h"
#include "spell_storage.h"
#include "battle_board.h"
#include "battle_grave.h"

#define ARENAW 11
#define ARENAH 9
#define ARENASIZE ARENAW * ARENAH

class Castle;
class HeroBase;

namespace Battle
{
    class Force;
    class Units;
    class Unit;

    struct Actions : public std::list<QueueMessage>
    {
        void AddedAutoBattleAction(u8);
        void AddedRetreatAction(void);
        void AddedSurrenderAction(void);
        void AddedCastAction(const Spell &, s16);
        void AddedCastTeleportAction(s16, s16);
	void AddedCastMirrorImageAction(s16);
        void AddedEndAction(const Unit &);
        void AddedSkipAction(const Unit &, bool);
        void AddedMoveAction(const Unit &, s16);
        void AddedAttackAction(const Unit &, const Unit &, s16, u8);
        void AddedMoraleAction(const Unit &, u8);
    };

    class Arena
    {
    public:
	Arena(Army &, Army &, s32, bool);
	~Arena();

	void		Turns(void);
	bool		NetworkTurn(void);
	bool		BattleValid(void) const;

	bool		CanBreakAutoBattle(void) const;
	void		BreakAutoBattle(void);

	u16		GetCurrentTurn(void) const;
	Result &	GetResult(void);

	const HeroBase*	GetCommander(u8 color, bool invert = false) const;
	const HeroBase*	GetCommander1(void) const;
	const HeroBase*	GetCommander2(void) const;
	const HeroBase* GetCurrentCommander(void) const;

	Force &		GetForce1(void);
	Force &		GetForce2(void);
	Force &		GetCurrentForce(bool = true);
	const Units*	GetOrderArmies(void) const;

	u8		GetArmyColor1(void) const;
	u8		GetArmyColor2(void) const;
	u8		GetCurrentColor(void) const;
	u8		GetOppositeColor(u8) const;

	Unit*		GetTroopBoard(s16);
	const Unit*	GetTroopBoard(s16) const;

	Unit*		GetTroopUID(u32);
	const Unit*	GetTroopUID(u32) const;

	const Unit*	GetEnemyMaxQuality(u8) const;

	const SpellStorage &
			GetUsageSpells(void) const;

	void		DialogBattleSummary(const Result &) const;
	u8		DialogBattleHero(const HeroBase &) const;

	void		FadeArena(void) const;

	Indexes		GetPath(const Unit &, const Position &);
	void		ApplyAction(QueueMessage &);

	TargetsInfo	GetTargetsForDamage(Unit &, Unit &, s16);
	void		TargetsApplyDamage(Unit &, Unit &, TargetsInfo &);
	TargetsInfo	GetTargetsForSpells(const HeroBase*, const Spell &, s16);
	void		TargetsApplySpell(const HeroBase*, const Spell &, TargetsInfo &);

	bool		isDisableCastSpell(const Spell &, std::string *msg);

	bool		GraveyardAllowResurrect(s16, const Spell &) const;
	const Unit*	GraveyardLastTroop(s16) const;
	Indexes		GraveyardClosedCells(void) const;

	bool		CanSurrenderOpponent(u8 color) const;
	bool		CanRetreatOpponent(u8 color) const;

	// uniq spells
	void		SpellActionSummonElemental(QueueMessage &, const Spell &);
	void		SpellActionMirrorImage(QueueMessage &);
	void		SpellActionTeleport(QueueMessage &);
	void		SpellActionEarthQuake(QueueMessage &);
	void		SpellActionDefaults(QueueMessage &, const Spell &);


	u8		GetObstaclesPenalty(const Unit &, const Unit &) const;
	ICN::icn_t	GetICNCovr(void) const;

	static Board*		GetBoard(void);
	static Tower*		GetTower(u8);
	static Bridge*		GetBridge(void);
	static const Castle*	GetCastle(void);
	static Interface*	GetInterface(void);
	static Graveyard*	GetGraveyard(void);

    private:
	friend QueueMessage & operator<< (QueueMessage &, const Arena &);
	friend QueueMessage & operator>> (QueueMessage &, Arena &);

    	void		RemoteTurn(const Unit &, Actions &);
	void		HumanTurn(const Unit &, Actions &);

	void		TurnTroop(Unit*);
	void		TowerAction(const Tower &);

        u8		GetCastleTargetValue(u8) const;
        void		SetCastleTargetValue(u8, u8);
	void		CatapultAction(void);

	s16		GetFreePositionNearHero(u8) const;
	std::vector<u8>	GetCastleTargets(void) const;

	void		ApplyActionRetreat(QueueMessage &);
	void		ApplyActionSurrender(QueueMessage &);
	void		ApplyActionAttack(QueueMessage &);
	void		ApplyActionMove(QueueMessage &);
	void		ApplyActionEnd(QueueMessage &);
	void		ApplyActionSkip(QueueMessage &);
	void		ApplyActionMorale(QueueMessage &);
	void		ApplyActionLuck(QueueMessage &);
	void		ApplyActionSpellCast(QueueMessage &);
	void		ApplyActionTower(QueueMessage &);
	void		ApplyActionCatapult(QueueMessage &);
	void		ApplyActionAutoBattle(QueueMessage &);

	void		BattleProcess(Unit &, Unit & b2, s16 = -1, u8 = 0);

	Unit*		CreateElemental(const Spell &);
	Unit*		CreateMirrorImage(Unit &, s16);

	Force*		army1;
        Force*		army2;
	Units*		armies;

	const Castle*	castle;
	u8		current_color;

	Tower*		towers[3];
	Catapult*	catapult;
	Bridge*		bridge;

	Interface*	interface;
	Result		result_game;

	Graveyard	graveyard;
	SpellStorage	usage_spells;

	Board		board;
	ICN::icn_t	icn_covr;

	u16		current_turn;
	u8		auto_battle;
    };

    Arena*	GetArena(void);
    QueueMessage &	operator<< (QueueMessage &, const Arena &);
    QueueMessage &	operator>> (QueueMessage &, Arena &);
}

#endif
