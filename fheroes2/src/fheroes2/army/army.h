/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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

#ifndef H2ARMY_H
#define H2ARMY_H

#include <string>
#include <vector>
#include "bitmodes.h"
#include "heroes_base.h"
#include "army_troop.h"

class Castle;
namespace Maps { class Tiles; };

#define	ARMYMAXTROOPS		5

namespace Army
{
    class BattleTroop;

    enum armysize_t
    {
	FEW	= 1,
	SEVERAL	= 5,
	PACK	= 10,
	LOTS	= 20,
	HORDE	= 50,
	THRONG	= 100,
	SWARM	= 250,
	ZOUNDS	= 500,
	LEGION	= 1000
    };

    const char* String(u32);
    armysize_t GetSize(u32);

    class army_t
    {
	public:
	    army_t(const HeroBase* s = NULL);

	    army_t & operator= (const army_t &);		// deprecated, will be removed!

	    void	FromGuardian(const Maps::Tiles &);
	    void	Import(const std::vector<Troop> &);
	    void	Import(const std::vector<BattleTroop> &);
	    void	UpgradeMonsters(const Monster &);
	    void	UpgradeMonsters(const Monster::monster_t);
	    void	Clear(void);
	    void	Reset(bool = false);	// reset: soft or hard

	    void	SetModes(u32);
	    void	ResetModes(u32);

	    void	DrawMons32Line(s16, s16, u8, u8 = 0, u8 = 0, bool = false) const;

	    Troop &	FirstValid(void);
	    Troop &	At(u8);
	    Troop &	GetSlowestTroop(void);
	    Troop &	GetFastestTroop(void);
	    Troop &	GetStrongestTroop(void);
	    Troop &	GetWeakestTroop(void);

	    const Troop&At(u8) const;
	    const Troop&GetSlowestTroop(void) const;
	    const Troop&GetFastestTroop(void) const;
	    const Troop&GetStrongestTroop(void) const;
	    const Troop&GetWeakestTroop(void) const;

	    Race::race_t   GetRace(void) const;
	    Color::color_t GetColor(void) const;

	    u8		Size(void) const;
	    u8		GetCount(void) const;
	    u8		GetUniqCount(void) const;
	    u16		GetCountMonsters(const Monster &) const;
	    u16		GetCountMonsters(const Monster::monster_t) const;
	    s8		GetMorale(void) const;
	    s8		GetLuck(void) const;
	    s8		GetMoraleWithModificators(std::string *strs = NULL) const;
	    s8		GetLuckWithModificators(std::string *strs = NULL) const;
	    u32		CalculateExperience(void) const;
	    u32		ActionToSirens(void);

	    u16		GetAttack(void) const;
	    u16		GetDefense(void) const;
	    u32		GetHitPoints(void) const;
	    u32		GetDamageMin(void) const;
	    u32		GetDamageMax(void) const;

	    bool	isValid(void) const;
	    bool	HasMonster(const Monster &) const;
	    bool	HasMonster(const Monster::monster_t) const;
	    bool	JoinTroop(const Troop & troop);
	    bool	JoinTroop(const Monster::monster_t mon, const u16 count);
	    bool	JoinTroop(const Monster & mon, const u16 count);
	    bool	StrongerEnemyArmy(const army_t &);
        void    CalculateForceRatiosVersus(const army_t &a, u32 &own, u32 &other);

	    void	JoinStrongestFromArmy(army_t &);
            void	KeepOnlyWeakestTroops(army_t &);
	    void	UpgradeTroops(const Castle &);
	    void	ArrangeForBattle(void);

	    void	Dump(void) const;

	    const HeroBase* GetCommander(void) const;

	protected:
	    friend class Troop;

	    std::vector<Troop>	army;
	    const HeroBase* commander;
    };
};

#endif
