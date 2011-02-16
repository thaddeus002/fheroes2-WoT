/*************************************************************************** 
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include <utility>
#include <sstream>
#include "artifact.h"
#include "race.h"
#include "army.h"
#include "world.h"
#include "kingdom.h"
#include "castle.h"
#include "heroes_base.h"

typedef std::vector< std::pair<Artifact, s8> > ArtifactsModifiers;

s8 GetResultModifiers(const ArtifactsModifiers & modifiers, const HeroBase & base, std::string* strs)
{
    s8 result = 0;

    for(size_t ii = 0; ii < modifiers.size(); ++ii)
    {
	const std::pair<Artifact, s8> & pair = modifiers[ii];

        if(base.HasArtifact(pair.first))
        {
    	    result += pair.second;

    	    if(strs)
    	    {
        	strs->append(pair.first.GetName());
		StringAppendModifiers(*strs, pair.second);
        	strs->append("\n");
	    }
	}
    }

    return result;
}

HeroBase::HeroBase() : magic_point(0), spell_book()
{
}

u16 HeroBase::GetSpellPoints(void) const
{
    return magic_point;
}

void HeroBase::SetSpellPoints(u16 points)
{
    magic_point = points;
}

bool HeroBase::HaveSpellPoints(u16 points) const
{
    return magic_point >= points;
}

void HeroBase::EditSpellBook(void)
{
    spell_book.Edit(*this);
}

Spell HeroBase::OpenSpellBook(SpellBook::filter_t flt, bool canselect) const
{
    return spell_book.Open(*this, flt, canselect);
}

bool HeroBase::HaveSpellBook(void) const
{
    return bag_artifacts.isPresentArtifact(Artifact::MAGIC_BOOK);
}

bool HeroBase::HaveSpell(const Spell & spell) const
{
    return HaveSpellBook() &&
	(spell_book.isPresentSpell(spell) || bag_artifacts.ContainSpell(spell));
}

void HeroBase::AppendSpellToBook(const Spell & spell, bool without_wisdom)
{
    if(without_wisdom || CanLearnSpell(spell))
	spell_book.Append(spell);
}

void HeroBase::AppendSpellsToBook(const SpellStorage & spells, bool without_wisdom)
{
    for(SpellStorage::const_iterator
	it = spells.begin(); it != spells.end(); ++it)
	AppendSpellToBook(*it, without_wisdom);
}

bool HeroBase::SpellBookActivate(void)
{
    return ! HaveSpellBook() &&	
	    bag_artifacts.PushArtifact(Artifact::MAGIC_BOOK);
}

const BagArtifacts & HeroBase::GetBagArtifacts(void) const
{
    return bag_artifacts;
}

BagArtifacts & HeroBase::GetBagArtifacts(void)
{
    return bag_artifacts;
}

bool HeroBase::HasArtifact(const Artifact & art) const
{
    return bag_artifacts.isPresentArtifact(art);
}

s8 HeroBase::GetAttackModificator(std::string* strs) const
{
    static ArtifactsModifiers modifiers;

    if(modifiers.empty())
    {
	const Artifact arts[] = {
	    Artifact::SPIKED_HELM, Artifact::THUNDER_MACE, Artifact::GIANT_FLAIL,
	    Artifact::SWORD_BREAKER, Artifact::SPIKED_SHIELD, Artifact::POWER_AXE,
	    Artifact::LEGENDARY_SCEPTER, Artifact::DRAGON_SWORD, Artifact::ULTIMATE_CROWN,
	    Artifact::BATTLE_GARB, Artifact::SWORD_ANDURAN, Artifact::HOLY_HAMMER,
	    Artifact::ULTIMATE_SHIELD, Artifact::ULTIMATE_SWORD };

	const u8 size = sizeof(arts) / sizeof(Artifact);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], arts[ii].ExtraValue()));
    }

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetAttackModificator(strs);

    return result;
}

s8 HeroBase::GetDefenseModificator(std::string* strs) const
{
    static ArtifactsModifiers modifiers;

    if(modifiers.empty())
    {
	const Artifact arts[] = {
	    Artifact::SPIKED_HELM, Artifact::ARMORED_GAUNTLETS, Artifact::DEFENDER_HELM,
	    Artifact::SPIKED_SHIELD, Artifact::STEALTH_SHIELD, Artifact::LEGENDARY_SCEPTER,
	    Artifact::DIVINE_BREASTPLATE, Artifact::ULTIMATE_CROWN,
	    Artifact::SWORD_BREAKER, Artifact::BREASTPLATE_ANDURAN, Artifact::BATTLE_GARB,
	    Artifact::ULTIMATE_SHIELD, Artifact::ULTIMATE_CLOAK };

	const u8 size = sizeof(arts) / sizeof(Artifact);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], arts[ii].ExtraValue()));
    }

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetDefenseModificator(strs);

    return result;
}

s8 HeroBase::GetPowerModificator(std::string* strs) const
{
    static ArtifactsModifiers modifiers;

    if(modifiers.empty())
    {
	const Artifact arts[] = {
	    Artifact::WHITE_PEARL, Artifact::BLACK_PEARL, Artifact::CASTER_BRACELET,
	    Artifact::MAGE_RING, Artifact::LEGENDARY_SCEPTER, Artifact::WITCHES_BROACH,
	    Artifact::ARM_MARTYR, Artifact::ULTIMATE_CROWN, Artifact::ARCANE_NECKLACE,
	    Artifact::BATTLE_GARB, Artifact::STAFF_WIZARDRY, Artifact::HELMET_ANDURAN,
	    Artifact::ULTIMATE_STAFF, Artifact::ULTIMATE_WAND };

	const u8 size = sizeof(arts) / sizeof(Artifact);

	modifiers.reserve(size + 1);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], arts[ii].ExtraValue()));

	modifiers.push_back(std::make_pair(Artifact(Artifact::BROACH_SHIELDING), -1));
    }

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetPowerModificator(strs);

    return result;
}

s8 HeroBase::GetKnowledgeModificator(std::string* strs) const
{
    static ArtifactsModifiers modifiers;

    if(modifiers.empty())
    {
	const Artifact arts[] = {
	    Artifact::WHITE_PEARL, Artifact::BLACK_PEARL, Artifact::MINOR_SCROLL,
	    Artifact::MAJOR_SCROLL, Artifact::SUPERIOR_SCROLL, Artifact::FOREMOST_SCROLL,
	    Artifact::LEGENDARY_SCEPTER, Artifact::ULTIMATE_CROWN,
	    Artifact::ULTIMATE_STAFF, Artifact::ULTIMATE_BOOK };

	const u8 size = sizeof(arts) / sizeof(Artifact);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], arts[ii].ExtraValue()));
    }

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetKnowledgeModificator(strs);

    return result;
}

s8 HeroBase::GetMoraleModificator(bool shipmaster, std::string* strs) const
{
    ArtifactsModifiers modifiers;
    Artifact art;

    modifiers.reserve(7);

    art = Artifact::MEDAL_VALOR;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::MEDAL_COURAGE;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::MEDAL_HONOR;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::MEDAL_DISTINCTION;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    modifiers.push_back(std::make_pair(Artifact(Artifact::BATTLE_GARB), 10));

    art = Artifact::MASTHEAD;
    if(shipmaster) modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::FIZBIN_MISFORTUNE;
    modifiers.push_back(std::make_pair(art, -art.ExtraValue()));

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetMoraleModificator(strs);

    // army modificator
    if(GetArmy().AllTroopsIsRace(Race::NECR))
    {
	if(strs) strs->clear();
	result = 0;
    }

    result += GetArmy().GetMoraleModificator(strs);

    return result;
}

s8 HeroBase::GetLuckModificator(bool shipmaster, std::string* strs) const
{
    ArtifactsModifiers modifiers;
    Artifact art;

    modifiers.reserve(6);

    art = Artifact::RABBIT_FOOT;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::GOLDEN_HORSESHOE;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::GAMBLER_LUCKY_COIN;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    art = Artifact::FOUR_LEAF_CLOVER;
    modifiers.push_back(std::make_pair(art, art.ExtraValue()));
    modifiers.push_back(std::make_pair(Artifact(Artifact::BATTLE_GARB), 10));
    art = Artifact::MASTHEAD;
    if(shipmaster) modifiers.push_back(std::make_pair(art, art.ExtraValue()));

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetLuckModificator(strs);

    // army modificator
    result += GetArmy().GetLuckModificator(strs);

    return result;
}

bool HeroBase::CanCastSpell(const Spell & spell, std::string* res) const
{
    const Kingdom & kingdom = world.GetKingdom(GetColor());

    if(res)
    {
	std::ostringstream os;

	if(HaveSpellBook())
	{
	    if(HaveSpell(spell))
	    {
		if(HaveSpellPoints(spell.CostManaPoints(this)))
		{
		    if(kingdom.AllowPayment(spell.GetCost()))
			return true;
		    else
	    		os << "resource" << " " << "failed";
		}
		else
	    	    os << "mana points" << " " << "failed";
	    }
	    else
	    	os << spell.GetName() << " " << "not found";
	}
	else
	    os << "spell book" << " " << "not found";
	*res = os.str();
	return false;
    }
    return HaveSpellBook() && HaveSpell(spell) && HaveSpellPoints(spell.CostManaPoints(this)) && kingdom.AllowPayment(spell.GetCost());
}

void HeroBase::SpellCasted(const Spell & spell)
{
    // resource cost
    Kingdom & kingdom = world.GetKingdom(GetColor());
    const payment_t & cost = spell.GetCost();
    if(cost.GetValidItems()) kingdom.OddFundsResource(cost);

    // mana cost
    u16 mp_cost = spell.CostManaPoints(this);
    magic_point -= (mp_cost < magic_point ? mp_cost : magic_point);
}

bool HeroBase::CanTranscribeScroll(const Artifact & art) const
{
    Spell spell = art.GetSpell();

    if(spell.isValid() && CanCastSpell(spell))
    {
	u8 learning = GetLevelSkill(Skill::Secondary::LEARNING);

	return ((3 <  spell.Level() && Skill::Level::EXPERT == learning) ||
	    (3 == spell.Level() && Skill::Level::ADVANCED <= learning) ||
	    (3 > spell.Level() && Skill::Level::BASIC <= learning));
    }

    return false;
}

bool HeroBase::CanTeachSpell(const Spell & spell) const
{
    u8 learning = GetLevelSkill(Skill::Secondary::LEARNING);

    return ((4 == spell.Level() && Skill::Level::EXPERT == learning) ||
	    (3 == spell.Level() && Skill::Level::ADVANCED <= learning) ||
	    (3 > spell.Level() && Skill::Level::BASIC <= learning));
}

bool HeroBase::CanLearnSpell(const Spell & spell) const
{
    u8 wisdom = GetLevelSkill(Skill::Secondary::WISDOM);

    return ((4 < spell.Level() && Skill::Level::EXPERT > wisdom) ||
            (3 < spell.Level() && Skill::Level::ADVANCED > wisdom) ||
            (2 < spell.Level() && Skill::Level::BASIC > wisdom) ? false : true);
}

void HeroBase::TranscribeScroll(const Artifact & art)
{
    Spell spell = art.GetSpell();

    if(spell.isValid())
    {
	// add spell
	spell_book.Append(spell);

	// remove art
	bag_artifacts.RemoveScroll(art);

	// reduce mp and resource
	SpellCasted(spell);
    }
}
