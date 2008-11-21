/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>           *
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
#ifndef H2SPELL_TYPES_H
#define H2SPELL_TYPES_H

namespace Spell
{
    typedef enum {
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
	STELLSKIN,
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
    } spell_t;
    
    struct stats_t;
    struct magic_t;
}

#endif
