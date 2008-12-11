/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

#ifndef H2BATTLE_TROOPS_H
#define H2BATTLE_TROOPS_H

#include <string>
#include <vector>
#include "army.h"
#include "battle_spell.h"
#include "gamedefs.h"

namespace Army
{
    class BattleTroop : public Troop
    {
    public:
	BattleTroop(Monster::monster_t m = Monster::UNKNOWN, u16 c = 0);
	BattleTroop(const BattleTroop & troop);
        BattleTroop(const Troop & troop);

	BattleTroop & operator= (const BattleTroop & troops);
        BattleTroop & operator= (const Troop & troops);
	
	void SetPosition(const Point & pt) { pos = pt; };
	const Point& Position() const { return pos; };
        void SetScreenPosition(const Point & pt) { screenPos = pt; };
	const Point& ScreenPosition() const { return screenPos; };
        
        bool IsMoving() const { return moving; }
        void SetMoving(bool m) { moving = m; }

	void BlitR(const Point& dst_pt, bool reflect = false, int frame = -1);
	void Blit(const Point& dst_pt, bool reflect = false, int frame = -1);
	void Animate(Monster::animstate_t as = Monster::AS_NONE);
	void SetMagic(Spell::magic_t &magic);
	bool FindMagic(Spell::spell_t spell) const;
	void RemoveMagic(Spell::spell_t spell);
	void ClearMagic();
	void ProceedMagic();
	const std::vector<Spell::magic_t> &Magics() const { return magics; };
        
        bool IsReflected() const { return reflect; }
        void SetReflect(bool r) { lastReflect = reflect; reflect = r; }
        void SetOriginalReflection(bool r) { origReflect = reflect = lastReflect = r; }
        void ResetReflection() { SetReflect(origReflect); }
        bool WasReflected() const { return lastReflect; }
        bool OriginalReflection() const { return origReflect; }
        
        bool IsWide() const { return Monster::GetStats(Monster()).wide; }
        
        bool HasRetaliated() const { return retaliated; }
        void SetRetaliated(bool r) { retaliated = r; }
        
        int TotalHP() const { return hp + (count - 1) * Monster::GetStats(monster).hp; }
        
        int ApplyDamage(int damage);

	Monster::animstate_t    astate;
	u16                     aframe;
	bool                    attackRanged;
	u8                      shots;
	u16                     hp;
	u16                     oldcount;
	bool                    summoned;

    private:
	Point                   pos;
        Point                   screenPos;
	Background bg;
	bool saved;
        bool reflect, origReflect, lastReflect;
        bool retaliated;
        bool moving;
	std::vector<Spell::magic_t> magics;
    };
    
    typedef std::vector<BattleTroop> BattleArmy_t;
};

#endif
