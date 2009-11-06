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
#include "gamedefs.h"

class Spell
{
public:
    enum spell_t
    {
	NONE,
	FIREBALL,
	FIREBLAST,
	LIGHTNINGBOLT,
	CHAINLIGHTNING,
	TELEPORT,
	CURE,
	MASSCURE,
	RESURRECT,
	RESURRECTTRUE,
	HASTE,
	MASSHASTE,
	SLOW,
	MASSSLOW,
	BLIND,
	BLESS,
	MASSBLESS,
	STONESKIN,
	STEELSKIN,
	CURSE,
	MASSCURSE,
	HOLYWORD,
	HOLYSHOUT,
	ANTIMAGIC,
	DISPEL,
	MASSDISPEL,
	ARROW,
	BERZERKER,
	ARMAGEDDON,
	ELEMENTALSTORM,
	METEORSHOWER,
	PARALYZE,
	HYPNOTIZE,
	COLDRAY,
	COLDRING,
	DISRUPTINGRAY,
	DEATHRIPPLE,
	DEATHWAVE,
	DRAGONSLAYER,
	BLOODLUST,
	ANIMATEDEAD,
	MIRRORIMAGE,
	SHIELD,
	MASSSHIELD,
	SUMMONEELEMENT,
	SUMMONAELEMENT,
	SUMMONFELEMENT,
	SUMMONWELEMENT,
	EARTHQUAKE,
	VIEWMINES,
	VIEWRESOURCES,
	VIEWARTIFACTS,
	VIEWTOWNS,
	VIEWHEROES,
	VIEWALL,
	IDENTIFYHERO,
	SUMMONBOAT,
	DIMENSIONDOOR,
	TOWNGATE,
	TOWNPORTAL,
	VISIONS,
	HAUNT,
	SETEGUARDIAN,
	SETAGUARDIAN,
	SETFGUARDIAN,
	SETWGUARDIAN,

	STONE,
    };

    enum target_t
    {
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

    Spell();
    Spell(spell_t);

    bool operator== (spell_t) const;
    bool operator!= (spell_t) const;
    spell_t operator() (void) const;
    spell_t GetID(void) const;

    void Set(spell_t);

    const char* GetName(void) const;
    const char* GetDescription(void) const;

    u8 GetMana(void) const;
    u8 GetLevel(void) const;
    bool isCombat(void) const;
    bool isBad(void) const;
    target_t GetTarget(void) const;
    u8 GetPower(void) const;
    u16 GetInflictDamage(u8) const;

    /* return index sprite spells.icn */
    u8 GetIndexSprite(void) const;
    /* return index in spellinl.icn */
    u8 GetInlIndexSprite(void) const;


    static spell_t FromInt(u8);
    static u8 Mana(spell_t);
    static u8 Level(spell_t);
    static bool isCombat(spell_t);
    static bool isBad(spell_t);
    static target_t Target(spell_t);
    static u8 Power(spell_t);

    /* return index sprite spells.icn */
    static u8 IndexSprite(spell_t);
    /* return index in spellinl.icn */
    static u8 InlIndexSprite(spell_t);

    static const char* GetName(spell_t);
    static const char* GetDescription(spell_t);

    static spell_t RandCombat(u8);
    static spell_t RandAdventure(u8);
    static spell_t TroopAttack(u8);

    static u16 InflictDamage(spell_t, u8);

private:
    spell_t id;
};

#endif
