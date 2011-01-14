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

#include "heroes_base.h"
#include "artifact.h"
#include "settings.h"
#include "spell.h"
#include "army.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

enum { SP_DISABLE = 0x01 };

struct spellstats_t
{
        const char* name;
        u8 mana;
        u8 sprite;
        u8 extra;
	const u8 lvl;
	u8 bits;
        const char* description;
};

u16 Spell::dd_distance = 0;
u16 Spell::dd_sp = 0;
u16 Spell::dd_hp = 0;

static spellstats_t spells[] = {
	//  name                      mana spr value  lvl bits description
	{ "Unknown",                    0,  0,     0, 0,  0,   "Unknown spell." },
	{ _("Fireball"),                9,  8,    10, 3,  0,   _("Causes a giant fireball to strike the selected area, damaging all nearby creatures.") },
	{ _("Fireblast"),              15,  9,    10, 4,  0,   _("An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one.") }, 
	{ _("Lightning Bolt"),          7,  4,    25, 2,  0,   _("Causes a bolt of electrical energy to strike the selected creature.") }, 
	{ _("Chain Lightning"),        15,  5,    40, 4,  0,   _("Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!") }, 
	{ _("Teleport"),                9, 10,     0, 3,  0,   _("Teleports the creature you select to any open position on the battlefield.") }, 
	{ _("Cure"),                    6,  6,     5, 1,  0,   _("Removes all negative spells cast upon one of your units, and restores up to %{count} HP per level of spell power.") }, 
	{ _("Mass Cure"),              15,  2,     5, 4,  0,   _("Removes all negative spells cast upon your forces, and restores up to %{count} HP per level of spell power, per creature.") }, 
	{ _("Resurrect"),              12, 13,    50, 4,  0,   _("Resurrects creatures from a damaged or dead unit until end of combat.") }, 
	{ _("Resurrect True"),         15, 12,    50, 5,  0,   _("Resurrects creatures from a damaged or dead unit permanently.") }, 
	// extra: 0, because also used Speed:GetOriginal (see Battle2::Stats::GetSpeed)
	{ _("Haste"),                   3, 14,     0, 1,  0,   _("Increases the speed of any creature by %{count}.") }, 
	{ _("Mass Haste"),             10, 14,     0, 3,  0,   _("Increases the speed of all of your creatures by %{count}.") }, 
	{ _("spell|Slow"),              3,  1,     0, 1,  0,   _("Slows target to half movement rate.") }, 
	{ _("Mass Slow"),              15,  1,     0, 4,  0,   _("Slows all enemies to half movement rate.") }, 
	//
	{ _("Blind "),                  6, 21,     0, 2,  0,   _("Clouds the affected creatures' eyes, preventing them from moving.") }, 
	{ _("Bless"),                   3,  7,     0, 1,  0,   _("Causes the selected creatures to inflict maximum damage.") }, 
	{ _("Mass Bless"),             12,  7,     0, 3,  0,   _("Causes all of your units to inflict maximum damage.") },
	{ _("Stoneskin"),               3, 31,     3, 1,  0,   _("Magically increases the defense skill of the selected creatures.") },
	{ _("Steelskin"),               6, 30,     5, 2,  0,   _("Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin.") },
	{ _("Curse"),                   3,  3,     0, 1,  0,   _("Causes the selected creatures to inflict minimum damage.") },
	{ _("Mass Curse"),             12,  3,     0, 3,  0,   _("Causes all enemy troops to inflict minimum damage.") },
	{ _("Holy Word"),               9, 22,    10, 3,  0,   _("Damages all undead in the battle.") },
	{ _("Holy Shout"),             12, 23,    20, 4,  0,   _("Damages all undead in the battle.  This is an improved version of Holy Word.") },
	{ _("Anti-Magic"),              7, 17,     0, 3,  0,   _("Prevents harmful magic against the selected creatures.") },
	{ _("Dispel Magic"),            5, 18,     0, 1,  0,   _("Removes all magic spells from a single target.") },
	{ _("Mass Dispel"),            12, 18,     0, 3,  0,   _("Removes all magic spells from all creatures.") },
	{ _("Magic Arrow"),             3, 38,    10, 1,  0,   _("Causes a magic arrow to strike the selected target.") },
	{ _("Berserker"),              12, 19,     0, 4,  0,   _("Causes a creature to attack its nearest neighbor.") },
	{ _("Armageddon"),             20, 16,    50, 5,  0,   _("Holy terror strikes the battlefield, causing severe damage to all creatures.") },
	{ _("Elemental Storm"),        15, 11,    25, 4,  0,   _("Magical elements pour down on the battlefield, damaging all creatures.") },
	{ _("Meteor Shower"),          15, 24,    25, 4,  0,   _("A rain of rocks strikes an area of the battlefield, damaging all nearby creatures.") },
	{ _("Paralyze"),                9, 20,     0, 3,  0,   _("The targeted creatures are paralyzed, unable to move or retaliate.") },
	{ _("Hypnotize"),              15, 37,    25, 5,  0,   _("Brings a single enemy unit under your control for one combat round if its hits are less than %{count} times the caster's spell power.") },
	{ _("Cold Ray"),                6, 36,    20, 2,  0,   _("Drains body heat from a single enemy unit.") },
	{ _("Cold Ring"),               9, 35,    10, 3,  0,   _("Drains body heat from all units surrounding the center point, but not including the center point.") },
	{ _("Disrupting Ray"),          7, 34,     3, 2,  0,   _("Reduces the defense rating of an enemy unit by three.") },
	{ _("Death Ripple"),            6, 28,     5, 2,  0,   _("Damages all living (non-undead) units in the battle.") },
	{ _("Death Wave"),             10, 29,    10, 3,  0,   _("Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple.") },
	{ _("Dragon Slayer"),           6, 32,     5, 2,  0,   _("Greatly increases a unit's attack skill vs. Dragons.") },
	{ _("Blood Lust"),              3, 27,     3, 1,  0,   _("Increases a unit's attack skill.") },
	{ _("Animate Dead"),           10, 25,    50, 3,  0,   _("Resurrects creatures from a damaged or dead undead unit permanently.") },
	{ _("Mirror Image"),           25, 26,     0, 5,  0,   _("Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage.") },
	{ _("Shield"),                  3, 15,     2, 1,  0,   _("Halves damage received from ranged attacks for a single unit.") },
	{ _("Mass Shield"),             7, 15,     0, 4,  0,   _("Halves damage received from ranged attacks for all of your units.") },
	{ _("Summon Earth Elemental"), 30, 56,     3, 5,  0,   _("Summons Earth Elementals to fight for your army.") },
	{ _("Summon Air Elemental"),   30, 57,     3, 5,  0,   _("Summons Air Elementals to fight for your army.") },
	{ _("Summon Fire Elemental"),  30, 58,     3, 5,  0,   _("Summons Fire Elementals to fight for your army.") },
	{ _("Summon Water Elemental"), 30, 59,     3, 5,  0,   _("Summons Water Elementals to fight for your army.") },
	{ _("Earthquake"),             15, 33,     0, 3,  0,   _("Damages castle walls.") },
	{ _("View Mines"),              1, 39,     0, 1,  0,   _("Causes all mines across the land to become visible.") },
	{ _("View Resources"),          1, 40,     0, 1,  0,   _("Causes all resources across the land to become visible.") },
	{ _("View Artifacts"),          2, 41,     0, 2,  0,   _("Causes all artifacts across the land to become visible.") },
	{ _("View Towns"),              2, 42,     0, 3,  0,   _("Causes all towns and castles across the land to become visible.") },
	{ _("View Heroes"),             2, 43,     0, 3,  0,   _("Causes all Heroes across the land to become visible.") },
	{ _("View All"),                3, 44,     0, 4,  0,   _("Causes the entire land to become visible.") },
	{ _("Identify Hero"),           3, 45,     0, 3,  0,   _("Allows the caster to view detailed information on enemy Heroes.") },
	{ _("Summon Boat"),             5, 46,     0, 2,  0,   _("Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy.") },
	{ _("Dimension Door"),         10, 47,     0, 5,  0,   _("Allows the caster to magically transport to a nearby location.") },
	{ _("Town Gate"),              10, 48,     0, 4,  0,   _("Returns the caster to any town or castle currently owned.") },
	{ _("Town Portal"),            20, 49,     0, 5,  0,   _("Returns the hero to the town or castle of choice, provided it is controlled by you.") },
	{ _("Visions"),                 6, 50,     3, 2,  0,   _("Visions predicts the likely outcome of an encounter with a neutral army camp.") },
	{ _("Haunt"),                   8, 51,     4, 2,  0,   _("Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)") },
	{ _("Set Earth Guardian"),     15, 52,     4, 4,  0,   _("Sets Earth Elementals to guard a mine against enemy armies.") },
	{ _("Set Air Guardian"),       15, 53,     4, 4,  0,   _("Sets Air Elementals to guard a mine against enemy armies.") },
	{ _("Set Fire Guardian"),      15, 54,     4, 4,  0,   _("Sets Fire Elementals to guard a mine against enemy armies.") },
	{ _("Set Water Guardian"),     15, 55,     4, 4,  0,   _("Sets Water Elementals to guard a mine against enemy armies.") },
	{ _("Stone"),  		        0,  0,     0, 0,  0,   _("Stone spell from Medusa.") },
};

void Spell::UpdateStats(const std::string & spec)
{
#ifdef WITH_XML
    // parse spells.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_spells = NULL;
    spellstats_t* ptr = &spells[0];

    if(doc.LoadFile(spec.c_str()) &&
        NULL != (xml_spells = doc.FirstChildElement("spells")))
    {
        const TiXmlElement* xml_spell = xml_spells->FirstChildElement("spell");
        for(; xml_spell; xml_spell = xml_spell->NextSiblingElement("spell"))
        {
            int value;

            xml_spell->Attribute("skip", &value);
            if(0 == value)
            {
		xml_spell->Attribute("cost", &value); if(value) ptr->mana = value;
		xml_spell->Attribute("extra", &value); if(value) ptr->extra = value;
	    }

            xml_spell->Attribute("disable", &value);
            if(value)
	    {
		ptr->bits |= SP_DISABLE;
	    }

            // load dimension door params
            if((ptr - &spells[0]) == DIMENSIONDOOR)
            {
		xml_spell->Attribute("distance", &value); dd_distance = value;
		xml_spell->Attribute("sp", &value); dd_sp = value;
		xml_spell->Attribute("hp", &value); dd_hp = value;
            }

	    ++ptr;

            // out of range
            if((ptr - &spells[0]) >= STONE) break;
	}
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

Spell::Spell() : id(NONE)
{
}

Spell::Spell(spell_t s) : id(s)
{
}

Spell::Spell(u8 s) : id(FromInt(s))
{
}

bool Spell::operator== (u8 s) const
{
    return s == id;
}

bool Spell::operator!= (u8 s) const
{
    return id != s;
}

Spell::spell_t Spell::operator() (void) const
{
    return id;
}

Spell::spell_t Spell::GetID(void) const
{
    return id;
}

void Spell::Set(spell_t s)
{
    id = s;
}

const char* Spell::GetName(void) const
{
    return GetName(id);
}

const char* Spell::GetDescription(void) const
{
    return GetDescription(id);
}

u8 Spell::GetCostManaPoints(void) const
{
    return CostManaPoints(id);
}

u8 Spell::GetLevel(void) const
{
    return Level(id);
}

bool Spell::isCombat(void) const
{
    return isCombat(id);
}

bool Spell::isAdventure(void) const
{
    return !isCombat(id);
}

bool Spell::isDamage(void) const
{
    return isDamage(id);
}

u8 Spell::GetDamage(void) const
{
    return Damage(id);
}

bool Spell::isMindInfluence(void) const
{
    return isMindInfluence(id);
}


/* ---------- */


u8 Spell::GetIndexSprite(void) const
{
    return IndexSprite(id);
}

u8 Spell::GetInlIndexSprite(void) const
{
    return InlIndexSprite(id);
}

Spell::spell_t Spell::FromInt(u8 index)
{
    return index > STONE ? NONE : static_cast<spell_t>(index);
}

u8 Spell::CostManaPoints(spell_t spell, const HeroBase* hero)
{
    if(hero)
    {
	switch(spell)
	{
	    case BLESS:
	    case MASSBLESS:
	    if(hero->HasArtifact(Artifact::SNAKE_RING)) return spells[spell].mana / 2;
	    break;

	    case SUMMONEELEMENT:
	    case SUMMONAELEMENT:
	    case SUMMONFELEMENT:
	    case SUMMONWELEMENT:
	    if(hero->HasArtifact(Artifact::ELEMENTAL_RING)) return spells[spell].mana / 2;
	    break;

	    case CURSE:
	    case MASSCURSE:
	    if(hero->HasArtifact(Artifact::EVIL_EYE)) return spells[spell].mana / 2;
	    break;

	    default: break;
	}

	if(isMindInfluence(spell) && hero->HasArtifact(Artifact::SKULLCAP)) return spells[spell].mana / 2;
    }

    return spells[spell].mana;
}

u8 Spell::Level(u8 spell)
{
    return spell < STONE ? spells[spell].lvl : 0;
}

bool Spell::isCombat(u8 spell)
{
    switch(spell)
    {
	case NONE:
	case VIEWMINES:
	case VIEWRESOURCES:
	case VIEWARTIFACTS:
	case VIEWTOWNS:
	case VIEWHEROES:
	case VIEWALL:
	case IDENTIFYHERO:
	case SUMMONBOAT:
	case DIMENSIONDOOR:
	case TOWNGATE:
	case TOWNPORTAL:
	case VISIONS:
	case HAUNT:
	case SETEGUARDIAN:
	case SETAGUARDIAN:
	case SETFGUARDIAN:
	case SETWGUARDIAN:	return false;
	default: break;
    }
    return true;
}

u8 Spell::Damage(u8 spell)
{
    switch(spell)
    {
	case ARROW:
	case FIREBALL:
	case FIREBLAST:
	case LIGHTNINGBOLT:
	case COLDRING:
	case DEATHWAVE:
	case HOLYWORD:
	case CHAINLIGHTNING:
	case ARMAGEDDON:
	case ELEMENTALSTORM:
	case METEORSHOWER:
	case COLDRAY:
	case HOLYSHOUT:
	case DEATHRIPPLE:
	    return spells[spell].extra;

	default: break;
    }

    return 0;
}

bool Spell::isDamage(u8 spell)
{
    return Damage(spell);
}

u8 Spell::Restore(u8 spell)
{
    switch(spell)
    {
        case Spell::CURE:
        case Spell::MASSCURE:
	    return spells[spell].extra;

	default: break;
    }

    return Resurrect(spell);
}

u8 Spell::Resurrect(u8 spell)
{
    switch(spell)
    {
	case Spell::ANIMATEDEAD:
        case Spell::RESURRECT:
        case Spell::RESURRECTTRUE:
	    return spells[spell].extra;

	default: break;
    }

    return 0;
}

bool Spell::isRestore(u8 spell)
{
    return Restore(spell);
}

bool Spell::isResurrect(u8 spell)
{
    return Resurrect(spell);
}

u8 Spell::IndexSprite(spell_t spell)
{
    return spells[spell].sprite;
}

u8 Spell::InlIndexSprite(u8 spell)
{
    switch(spell)
    {
	case HASTE:
	case MASSHASTE:		return 0;
	case SLOW:
	case MASSSLOW:		return 1;
	case BLIND:		return 2;
	case BLESS:
	case MASSBLESS:		return 3;
	case CURSE:
	case MASSCURSE:		return 4;
	case BERSERKER:		return 5;
	case PARALYZE:		return 6;
	case HYPNOTIZE:		return 7;
	case DRAGONSLAYER:	return 8;
	case BLOODLUST:		return 9;
	case SHIELD:
	case MASSSHIELD:	return 10;
	case STONE:		return 11;
	case ANTIMAGIC:		return 12;
	case STONESKIN:		return 13;
	case STEELSKIN:		return 14;
	default: break;
    }

    return 0;
}

u8 Spell::GetExtraValue(spell_t spell)
{ 
    return spells[spell].extra;
}

const char* Spell::GetName(spell_t spell)
{ 
    return _(spells[spell].name);
}

const char* Spell::GetDescription(spell_t spell)
{
    return _(spells[spell].description);
}

Spell::spell_t Spell::Rand(u8 lvl, bool adv)
{
    std::vector<spell_t> v;
    v.reserve(15);

    for(u8 sp = NONE; sp < STONE; ++sp)
	if((adv && !isCombat(sp) || (!adv && isCombat(sp))) &&
	    lvl == spells[sp].lvl &&
	    !(spells[sp].bits & SP_DISABLE)) v.push_back(static_cast<spell_t>(sp));

    return v.size() ? *Rand::Get(v) : Spell::NONE;
}

Spell::spell_t Spell::RandCombat(u8 lvl)
{
    return Rand(lvl, false);
}

Spell::spell_t Spell::RandAdventure(u8 lvl)
{
    return Rand(lvl, true);
}

bool Spell::isMindInfluence(u8 spell)
{
    switch(spell)
    {
	case BLIND:
	case PARALYZE:
	case BERSERKER:
	case HYPNOTIZE: return true;

	default: break;
    }

    return false;
}

bool Spell::isUndeadOnly(u8 spell)
{
    switch(spell)
    {
	case ANIMATEDEAD:
	case HOLYWORD:
	case HOLYSHOUT:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isALiveOnly(u8 spell)
{
    switch(spell)
    {
	case BLESS:
	case MASSBLESS:
	case CURSE:
	case MASSCURSE:
	case DEATHRIPPLE:
	case DEATHWAVE:
	case RESURRECT:
	case RESURRECTTRUE:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isApplyWithoutFocusObject(u8 spell)
{
    if(isMassActions(spell) || isSummon(spell))
	return true;
    else
    switch(spell)
    {
	case DEATHRIPPLE:
	case DEATHWAVE:
	case EARTHQUAKE:
	case HOLYWORD:
	case HOLYSHOUT:
        case ARMAGEDDON:
        case ELEMENTALSTORM:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isSummon(u8 spell)
{
    switch(spell)
    {
	case SUMMONEELEMENT:
        case SUMMONAELEMENT:
        case SUMMONFELEMENT:
        case SUMMONWELEMENT:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isApplyToAnyTroops(u8 spell)
{
    switch(spell)
    {
	case DISPEL:
	case MASSDISPEL:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isApplyToFriends(u8 spell)
{
    switch(spell)
    {
	case BLESS:
	case BLOODLUST:
	case CURE:
	case HASTE:
	case SHIELD:
	case STONESKIN:
	case DRAGONSLAYER:
	case STEELSKIN:
	case ANIMATEDEAD:
	case ANTIMAGIC:
	case TELEPORT:
	case RESURRECT:
	case MIRRORIMAGE:
	case RESURRECTTRUE:

	case MASSBLESS:
	case MASSCURE:
	case MASSHASTE:
	case MASSSHIELD:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isMassActions(u8 spell)
{
    switch(spell)
    {
	case MASSCURE:
	case MASSHASTE:
	case MASSSLOW:
	case MASSBLESS:
	case MASSCURSE:
	case MASSDISPEL:
	case MASSSHIELD:
		return true;

	default: break;
    }

    return false;
}

bool Spell::isApplyToEnemies(u8 spell)
{
    switch(spell)
    {
	case MASSSLOW:
	case MASSCURSE:

	case CURSE:
	case ARROW:
	case SLOW:
	case BLIND:
	case COLDRAY:
	case DISRUPTINGRAY:
	case LIGHTNINGBOLT:
	case CHAINLIGHTNING:
	case PARALYZE:
	case BERSERKER:
	case HYPNOTIZE:
	    return true;

	default: break;
    }

    return false;
}

bool Spell::isRaceCompatible(u8 spell, u8 race)
{
    switch(spell)
    {
	case MASSCURE:
	case MASSBLESS:
	case HOLYSHOUT:
	case HOLYWORD:
	case BLESS:
	case CURE:
	    if(Race::NECR == race) return false;
	    break;

	case DEATHWAVE:
	case DEATHRIPPLE:
	case ANIMATEDEAD:
	    if(Race::NECR != race) return false;
	    break;

	default:
	    break;
    }

    return true;
}

u8 Spell::CalculateDimensionDoorDistance(u8 current_sp, u32 total_hp)
{
    if(dd_distance && dd_hp && dd_sp && total_hp)
    {
	const u16 res = (dd_distance * current_sp * dd_hp) / (dd_sp * total_hp);
	return res ? (res < 255 ? res : 255) : 1;
    }
    // original h2 variant
    return 14;
}

bool Spell::AllowWithWisdom(u8 sp, u8 wisdom)
{
    return ((4 < Spell::Level(sp) && Skill::Level::EXPERT > wisdom) ||
	    (3 < Spell::Level(sp) && Skill::Level::ADVANCED > wisdom) ||
	    (2 < Spell::Level(sp) && Skill::Level::BASIC > wisdom) ? false : true);
}
