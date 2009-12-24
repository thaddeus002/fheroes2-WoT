/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov <drmoriarty@rambler.ru>          *
 *   Copyright (C) 2008 by Josh Matthews  <josh@joshmatthews.net>          *
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

#ifndef H2BATTLE_TROOPS_H
#define H2BATTLE_TROOPS_H

#include <string>
#include <vector>
#include "army.h"
#include "battle_spell.h"
#include "gamedefs.h"

typedef std::vector<s8> IndexList;

namespace Army
{
    enum MovementState {
        NOT_MOVING,
        SELECTED,
        IN_MOTION,
        UNDER_ATTACK
    };

    enum flags_t
    {
        ATTACKED        = 0x00000001,
        RETALIATED      = 0x00000002,
        HANDFIGHTING	= 0x00000008,
        
        LUCK_GOOD	= 0x00000010,
        LUCK_BAD    = 0x00000020,
        MORALE_GOOD	= 0x00000040,
        MORALE_BAD	= 0x00000080,
        
        SP_CURE		= 0x00004000,
        SP_DISPEL	= 0x00008000,
        SP_BLESS	= 0x00010000,
        SP_BLODLUST	= 0x00020000,
        SP_CURSE	= 0x00040000,
        SP_HASTE	= 0x00080000,
        SP_SHIELD	= 0x00100000,
        SP_SLOW		= 0x00200000,
        SP_STONESKIN	= 0x00400000,
        SP_BLIND	= 0x00800000,
        SP_DRAGONSLAYER	= 0x01000000,
        SP_STEELSKIN	= 0x02000000,
        SP_ANTIMAGIC	= 0x04000000,
        SP_PARALYZE	= 0x08000000,
        SP_BERZERKER	= 0x10000000,
        SP_HYPNOTIZE	= 0x20000000,
        SP_STONE	= 0x40000000,
        SP_DISRUPTINGRAY= 0x80000000,
        
        IS_MAGIC        = 0x7FFFC000,
    };
    
    class BattleTroop : public Troop
    {
      public:
        BattleTroop(Monster::monster_t m = Monster::UNKNOWN, u16 c = 0);
        BattleTroop(const BattleTroop & troop);
        BattleTroop(const Troop & troop);
        ~BattleTroop();

        void Init();

        BattleTroop & operator= (const BattleTroop & troops);
        BattleTroop & operator= (const Troop & troops);
	
        void SetPosition(const Point & pt) { pos = pt; };
        const Point& Position() const { return pos; };
        Point Front() const;
        void SetScreenPosition(const Point & pt) { screenPos = pt; };
        const Point& ScreenPosition() const { return screenPos; };
        MovementState isMoving() const { return moving; }
        void SetMoving(MovementState m) { moving = m; }

        Point GetBlitOffset(int frame, bool reflect);
        void BlitR(const Point& dst_pt, bool reflect = false, int frame = -1);
        void Blit(const Point& dst_pt, bool reflect = false, int frame = -1);
        void Animate(u8 as = Monster::AS_NONE);
        void SetMagic(Battle::magic_t &magic);
        bool FindMagic(Spell::spell_t spell) const;
        void RemoveMagic(Spell::spell_t spell);
        void ClearMagic();
        void ProceedMagic();
        const std::vector<Battle::magic_t> &Magics() const { return magics; };
        
        bool IsReflected() const { return reflect; }
        void SetReflect(bool r) { lastReflect = reflect; reflect = r; }
        void SetOriginalReflection(bool r) { origReflect = reflect = lastReflect = r; }
        void ResetReflection() { SetReflect(origReflect); }
        bool WasReflected() const { return lastReflect; }
        bool OriginalReflection() const { return origReflect; }
        
        u32 TotalHP() const { return hp + (count - 1) * Monster::GetHitPoints(); }

        void    SetModes(u32);

        void	NewTurn(void);
	    void	UpdateHitPoints(void);
        bool    isAffectedBySpell(u8);
	    bool	ApplySpell(Spell::spell_t, u8);

        bool    IsDamageFatal(u16) const;
        bool    CanRetaliateAgainst(const BattleTroop &) const;
        
        u8		GetAttack(void) const;
	    u8		GetDefense(void) const;
	    u32		GetHitPoints(void) const;
	    u16		GetDamageVersus(const BattleTroop &) const;
        u8		GetSpeed(void) const;
        
        int     ApplyDamage(long);

        void    LoadContours(bool);
        const Surface* GetContour(u8);

        u8                      astate;
        u16                     aframe;
        bool                    attackRanged;
        u8                      shots;
        u16                     hp;
        u16                     oldcount;
        bool                    summoned;
        long                    damageToApply;
        
      private:
        void RemoveSpellEffect(Spell::spell_t);

        Point                   pos;
        Point                   screenPos;
        Background bg;
        bool saved;
        bool reflect, origReflect, lastReflect;
        MovementState moving;
        std::vector<Battle::magic_t> magics;

        u8			disruptingray;
	    std::vector<Surface *> contours;
    };
    
    typedef std::vector<Army::BattleTroop> BattleArmy_t;
    void GetTroopsOfSpeed(const BattleArmy_t &, u8, IndexList &);
    bool isArmyValid(const BattleArmy_t &);
    void NewTurn(BattleArmy_t &);
    void LoadContours(BattleArmy_t &, bool);
};

#endif
