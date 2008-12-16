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
#include <list>
#include <vector>
#include "race.h"
#include "monster.h"
#include "skill.h"
#include "gamedefs.h"

class Surface;
class Castle;
class Heroes;
class Point;
class Rect;
namespace Maps { class Tiles; };

#define	ARMYMAXTROOPS		5

namespace Army
{
    class army_t;
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

    const std::string & String(armysize_t);
    armysize_t GetSize(u16);

    class Troop
    {
        public:
            Troop(Monster::monster_t m = Monster::UNKNOWN, u16 c = 0);
            Troop(const Troop &);
	    Troop & operator= (const Troop &);

            void	Set(Monster::monster_t, u16);
            void	SetMonster(Monster::monster_t);
            void	UpgradeMonster(void);
            void	SetCount(u16);
            void	Reset(void);
            
            const Skill::Primary* MasterSkill(void) const;
            const army_t* GetArmy(void) const;

            Monster::monster_t Monster(void) const;
            u16 	Count(void) const;

	    u8		Attack(void) const;
	    u8		Defense(void) const;
	    u32		HitPoint(void) const;
	    u16		DamageMin(void) const;
	    u16		DamageMax(void) const;

            bool	isValid(void) const;
	    bool	HasMonster(Monster::monster_t) const;

        protected:
    	    friend class army_t;
            Monster::monster_t	monster;
            u16			count;
	    const army_t*	army;
    };

    bool isValidTroop(const Troop & troop);
    bool StrongestTroop(const Troop & t1, const Troop & t2);
    bool WeakestTroop(const Troop & t1, const Troop & t2);
    bool SlowestTroop(const Troop & t1, const Troop & t2);
    bool FastestTroop(const Troop & t1, const Troop & t2);
    void SwapTroops(Troop & t1, Troop & t2);

    enum flags_t
    {
	FIGHT	= 0x0001,
    };

    class army_t
    {
	public:
	    army_t(const Skill::Primary* s = NULL);

	    army_t & operator= (const army_t &);		// deprecated, will be removed!

	    void 	SetModes(flags_t);
	    void	ResetModes(flags_t);
	    bool	Modes(flags_t) const;

	    void	FromGuardian(const Maps::Tiles &);
	    void	Import(const std::vector<Troop> &);
	    void	Import(const std::vector<BattleTroop> &);
	    void	UpgradeMonsters(const Monster::monster_t);
	    void	Reset(bool = false);	// reset: soft or hard

	    void	DrawMons32Line(s16, s16, u8, u8 = 0, u8 = 0) const;

	    Troop&	FirstValid(void);
	    Troop&	At(u8);
	    Troop &	GetSlowestTroop(void);
	    Troop &	GetFastestTroop(void);
	    Troop &	GetStrongestTroop(void);
	    Troop &	GetWeakestTroop(void);

	    const Troop&At(u8) const;
	    const Troop&GetSlowestTroop(void) const;
	    const Troop&GetFastestTroop(void) const;
	    const Troop&GetStrongestTroop(void) const;
	    const Troop&GetWeakestTroop(void) const;

	    Race::race_t GetRace(void) const;

	    u8		Size(void) const;
	    u8		GetCount(void) const;
	    u8		GetUniqCount(void) const;
	    u16		GetCountMonsters(const Monster::monster_t) const;
	    s8		GetMoraleWithModificators(std::list<std::string> *list = NULL) const;
	    s8		GetLuckWithModificators(std::list<std::string> *list = NULL) const;
	    u32		CalculateExperience(void) const;

	    u16		Attack(void) const;
	    u16		Defense(void) const;
	    u32		HitPoint(void) const;
	    u32		DamageMin(void) const;
	    u32		DamageMax(void) const;

	    bool	isValid(void) const;
	    bool	HasMonster(const Monster::monster_t mon) const;
	    bool	JoinTroop(const Troop & troop);
	    bool	JoinTroop(const Monster::monster_t mon, const u16 count);
	    bool	StrongerEnemyArmy(const army_t &);

	    void	JoinStrongestFromArmy(army_t &);
            void	KeepOnlyWeakestTroops(army_t &);
	    void	UpgradeTroops(const Castle &);
	    void	ArrangeForBattle(void);

	    void	Dump(void) const;

	private:
	    friend class Troop;

	    std::vector<Troop>	army;
	    const Skill::Primary* commander;
	    u16			flags;
    };





};

#endif
