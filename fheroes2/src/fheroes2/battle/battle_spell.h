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

#ifndef H2BATTLE_SPELL_H
#define H2BATTLE_SPELL_H

#include "spell.h"
#include "monster.h"
#include "gamedefs.h"

namespace Army { class BattleTroop; };

namespace Spell	// or Battle 
{
    typedef struct
    {
        spell_t spell;
        u8 duration;
    } magic_t;

    bool AllowSpell(spell_t spell, const Army::BattleTroop &troop);
    void ApplySpell(int spower, spell_t spell, Army::BattleTroop &troop);
    bool isTroopAffectedBySpell(u8 spell, const Army::BattleTroop &troop);
    u16 GetInflictDamageVersus(u8, u8, const Army::BattleTroop &);
    spell_t TroopAttack(const Monster &);
};

#endif
