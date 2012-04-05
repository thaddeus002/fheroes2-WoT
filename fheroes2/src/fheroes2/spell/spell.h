/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#ifndef H2SPELL_H
#define H2SPELL_H

#include "payment.h"
#include "gamedefs.h"

class HeroBase;
class StreamBase;
class StreamBase;

class Spell
{
public:
    enum type_t
    {
	NONE		= 0,
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
	BERSERKER,
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

	STONE
    };

    Spell(u8 = NONE);

    bool operator< (const Spell &) const;
    bool operator== (const Spell &) const;
    bool operator!= (const Spell &) const;

    u8 operator() (void) const;
    u8 GetID(void) const;

    const char* GetName(void) const;
    const char* GetDescription(void) const;

    u8 SpellPoint(const HeroBase* hero = NULL) const;
    u16 MovePoint(void) const;
    u8 Level(void) const;
    u8 Damage(void) const;
    u8 Restore(void) const;
    u8 Resurrect(void) const;

    u8 ExtraValue(void) const;
    payment_t GetCost(void) const;
    
    bool isValid(void) const;
    bool isLevel(u8) const;
    bool isCombat(void) const;
    bool isAdventure(void) const;
    bool isDamage(void) const;
    bool isRestore(void) const;
    bool isResurrect(void) const;
    bool isMindInfluence(void) const;
    bool isUndeadOnly(void) const;
    bool isALiveOnly(void) const;
    bool isSummon(void) const;
    bool isApplyWithoutFocusObject(void) const;
    bool isApplyToAnyTroops(void) const;
    bool isApplyToFriends(void) const;
    bool isApplyToEnemies(void) const;
    bool isMassActions(void) const;
    bool isRaceCompatible(u8 race) const;

    /* return index sprite spells.icn */
    u8 IndexSprite(void) const;
    /* return index in spellinl.icn */
    u8 InlIndexSprite(void) const;

    static Spell RandCombat(u8);
    static Spell RandAdventure(u8);
    static Spell Rand(u8, bool adv);

    static void UpdateStats(const std::string &);
    static u8 CalculateDimensionDoorDistance(u8 current_sp, u32 total_hp);

private:
    friend StreamBase & operator<< (StreamBase &, const Spell &);
    friend StreamBase & operator>> (StreamBase &, Spell &);

    u8 id;
};

StreamBase & operator<< (StreamBase &, const Spell &);
StreamBase & operator>> (StreamBase &, Spell &);

#endif

