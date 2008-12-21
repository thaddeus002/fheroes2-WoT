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

#ifndef H2ARMYTROOP_H
#define H2ARMYTROOP_H

#include <string>
#include "bitmodes.h"
#include "monster.h"

namespace Army
{
    class army_t;
    class Troop;

    bool isValidTroop(const Troop & troop);
    bool StrongestTroop(const Troop & t1, const Troop & t2);
    bool WeakestTroop(const Troop & t1, const Troop & t2);
    bool SlowestTroop(const Troop & t1, const Troop & t2);
    bool FastestTroop(const Troop & t1, const Troop & t2);
    void SwapTroops(Troop & t1, Troop & t2);

    class Troop : public Monster, public BitModes
    {
        public:
            Troop(monster_t m = Monster::UNKNOWN, u16 c = 0);

            void	Set(const Monster &, u16);
            void	Set(monster_t, u16);
            void	SetMonster(const Monster &);
            void	SetMonster(monster_t);
            void	SetCount(u16);
            void	Reset(void);

            void	BattleNewTurn(void);
	    void	BattleUpdateHitPoints(void);
	    bool	BattleApplySpell(u8, u8);

	    void        SetModes(u32);

            const Skill::Primary* MasterSkill(void) const;
            const army_t* GetArmy(void) const;
	    const std::string & GetName(void) const;

            u16 	Count(void) const;

	    u8		GetAttack(void) const;
	    u8		GetDefense(void) const;
	    u32		GetHitPoints(void) const;
	    u16		GetDamage(void) const;
	    u16		GetDamageMin(void) const;
	    u16		GetDamageMax(void) const;
	    Color::color_t GetColor(void) const;
	    u8		GetSpeed(void) const;
	    s8		GetMorale(void) const;
	    s8		GetLuck(void) const;

            bool	isValid(void) const;
	    bool	HasMonster(monster_t) const;

        protected:
    	    friend class army_t;
            u16			count;
	    const army_t*	army;

	    u32			hp;
    };
};

#endif
