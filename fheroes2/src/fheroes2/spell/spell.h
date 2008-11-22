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
#ifndef H2SPELL_H
#define H2SPELL_H

#include <string>
#include <vector>
#include "icn.h"
#include "m82.h"
#include "monster.h"
#include "army_types.h"
#include "spell_types.h"

// FIXME: For definitions of spell_t and magic_t, look in spell_types.h
//        This is hopefully only a temporary hack.

namespace Spell
{
    enum target_t {
	NOTARGET,
	ONEFRIEND,
	ONEENEMY,
	ALLFRIEND,
	ALLENEMY,
	ALLLIVE,
	ALLDEAD,
	ALL,
	//ANYCELL,
	FREECELL
    };

    struct stats_t {
        const std::string name;
	u8 mana;
	u8 level;
	bool combat;
	u8 sprite;
	target_t target;
	u8 power;
	ICN::icn_t icn;
	M82::m82_t m82;
        const std::string description;
    };

    const std::string &String(spell_t spell);
    u8 Mana(spell_t spell);
    u8 Level(spell_t spell);
    bool isCombat(spell_t spell);
    /* return index sprite spells.icn */
    u8 GetIndexSprite(spell_t spell);
    /* return index in spellinl.icn */
    u8 GetInlIndexSprite(spell_t spell);
    target_t Target(spell_t spell);
    u8 Power(spell_t spell);
    ICN::icn_t Icn(spell_t spell);
    M82::m82_t M82(spell_t spell);
    const std::string & Description(spell_t spell);

    spell_t RandCombat(const u8 lvl);
    spell_t RandAdventure(const u8 lvl);
    
    bool AllowSpell(spell_t spell, const Army::BattleTroop &troop);
    void ApplySpell(int spower, spell_t spell, Army::BattleTroop &troop);
    spell_t TroopAttack(Monster::monster_t monster);
};

#endif
