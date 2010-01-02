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

#ifndef H2BATTLE_SPELL_H
#define H2BATTLE_SPELL_H

#include "spell.h"
#include "monster.h"
#include "gamedefs.h"
#include "sprite.h"

namespace Army { class BattleTroop; };

namespace Battle
{
    class Battlefield;
    class GUI;
    
    struct magic_t
    {
        magic_t() : spell(Spell::NONE), duration(0) {};

        Spell::spell_t spell;
        u8 duration;
    };

    bool AllowSpell(Spell::spell_t spell, const Army::BattleTroop &troop);
    void ApplySpell(int spower, Spell::spell_t spell, Army::BattleTroop &troop);
    bool isTroopAffectedBySpell(Spell::spell_t spell, const Army::BattleTroop &troop, bool deteterministic);
    u16 GetInflictDamageVersus(Spell::spell_t spell, u8 sp, const Army::BattleTroop &);

    class BasicSpell;
    BasicSpell *CreateSpell(Spell::spell_t spell, const Battle::Battlefield &battlefield, const Rect &rect, HeroBase *hero, std::vector<Army::BattleTroop*> &affected, const Point &targetPoint);

    class BasicSpell
    {
      public:
        BasicSpell(Spell::spell_t spell);
        virtual void PreHit();
        virtual void OnHit(std::vector<Army::BattleTroop*> &affected, u8 spellPower, bool reflect, Battlefield &battlefield, GUI &gui);

        static Point TargetFromTroop(const Army::BattleTroop &troop);
        
      protected:
        void TriggerSound();
        virtual Point SpriteOffset(const Sprite &troop, const Sprite &effect);
        virtual void DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame);
        Background back;
        
      private:
        M82::m82_t sound;
        Spell::spell_t spell;
    };

    class Projectile : public BasicSpell
    {
      public:
        Projectile(Spell::spell_t spell, bool reflect, const Rect &hrect, const Point &target);
        void PreHit();
        
      protected:
        virtual int FrameModifier(int frame) { return frame; }
        
        Point start, end, delta;
        int maxFrames;
        ICN::icn_t icon;
        bool reflect;
    };

    class SplashDamage
    {
      public:
        SplashDamage(std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield);

      protected:
        const Point &target;
    };

    class Splash : public BasicSpell, public SplashDamage
    {
      public:
        Splash(Spell::spell_t spell, std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield);
      protected:
        void DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame);
    };

    class Hover : public BasicSpell
    {
      public:
        Hover(Spell::spell_t spell);

      protected:
        virtual Point SpriteOffset(const Sprite &troop, const Sprite &effect);
    };

    class Offsetless : public BasicSpell
    {
      public:
        Offsetless(Spell::spell_t spell);

      protected:
        virtual Point SpriteOffset(const Sprite &troop, const Sprite &effect);
    };

    class MagicArrow : public Projectile
    {
      public:
        MagicArrow(bool reflect, const Rect &hrect, const Point &target);
      private:
        int FrameModifier(int frame) { return index; }
        int index;
    };

    class Bloodlust : public BasicSpell
    {
      public:
        Bloodlust(const Army::BattleTroop &troop);
        void OnHit(std::vector<Army::BattleTroop*> &affected, u8 spellPower, bool reflect, Battlefield &battlefield, GUI &gui);
    };

    class ColdRing : public Projectile, public SplashDamage
    {
      public:
        ColdRing(std::vector<Army::BattleTroop*> &affected, const Point &targetPoint, const Battlefield &battlefield, bool reflect, const Rect &hrect);
      protected:
        void DrawSprite(std::vector<Army::BattleTroop*> &affected, ICN::icn_t icn, int frame);
    };
};

#endif
