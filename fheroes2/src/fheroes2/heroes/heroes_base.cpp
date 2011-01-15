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
#include "artifact.h"
#include "army.h"
#include "castle.h"
#include "heroes_base.h"

typedef std::vector< std::pair<Artifact::artifact_t, s8> > ArtifactsModifiers;

s8 GetResultModifiers(const ArtifactsModifiers & modifiers, const HeroBase & base, std::string* strs)
{
    s8 result = 0;

    for(size_t ii = 0; ii < modifiers.size(); ++ii)
    {
	const std::pair<Artifact::artifact_t, s8> & pair = modifiers[ii];

        if(base.HasArtifact(pair.first))
        {
    	    result += pair.second;

    	    if(strs)
    	    {
        	strs->append(Artifact::GetName(pair.first));
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

void HeroBase::TakeSpellPoints(u16 points)
{
    magic_point -= (points < magic_point ? points : magic_point);
}

Spell::spell_t HeroBase::OpenSpellBook(SpellBook::filter_t flt, bool canselect) const
{
    return spell_book.Open(*this, flt, canselect);
}

bool HeroBase::HaveSpellBook(void) const
{
    return spell_book.isActive();
}

bool HeroBase::HaveSpell(Spell::spell_t spell) const
{
    BagArtifacts::const_iterator it1 = bag_artifacts.begin();
    BagArtifacts::const_iterator it2 = bag_artifacts.end();
    for(; it1 != it2; ++it1) if(*it1 == Artifact::SPELL_SCROLL)
    {
        if(spell == Spell::FromInt((*it1).GetExt())) return true;
    }

    return spell_book.isActive() && spell_book.isPresentSpell(spell);
}

void HeroBase::AppendSpellToBook(Spell::spell_t spell, bool without_wisdom)
{
    if(without_wisdom)
	spell_book.Append(spell);
    else
	spell_book.Append(spell, GetLevelSkill(Skill::Secondary::WISDOM));
}

void HeroBase::SpellBookActivate(void)
{
    spell_book.Activate();
}

const BagArtifacts & HeroBase::GetBagArtifacts(void) const
{
    return bag_artifacts;
}

BagArtifacts & HeroBase::GetBagArtifacts(void)
{
    return bag_artifacts;
}

bool HeroBase::HasArtifact(Artifact::artifact_t art) const
{
    return bag_artifacts.size() && bag_artifacts.end() != std::find(bag_artifacts.begin(), bag_artifacts.end(), art);
}

s8 HeroBase::GetAttackModificator(std::string* strs) const
{
    static ArtifactsModifiers modifiers;

    if(modifiers.empty())
    {
	const Artifact::artifact_t arts[] = {
	    Artifact::SPIKED_HELM, Artifact::THUNDER_MACE, Artifact::GIANT_FLAIL,
	    Artifact::SWORD_BREAKER, Artifact::SPIKED_SHIELD, Artifact::POWER_AXE,
	    Artifact::LEGENDARY_SCEPTER, Artifact::DRAGON_SWORD, Artifact::ULTIMATE_CROWN,
	    Artifact::BATTLE_GARB, Artifact::SWORD_ANDURAN, Artifact::HOLY_HAMMER,
	    Artifact::ULTIMATE_SHIELD, Artifact::ULTIMATE_SWORD };

	const u8 size = sizeof(arts) / sizeof(Artifact::artifact_t);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], Artifact::GetExtraValue(arts[ii])));
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
	const Artifact::artifact_t arts[] = {
	    Artifact::SPIKED_HELM, Artifact::ARMORED_GAUNTLETS, Artifact::DEFENDER_HELM,
	    Artifact::SPIKED_SHIELD, Artifact::STEALTH_SHIELD, Artifact::LEGENDARY_SCEPTER,
	    Artifact::DIVINE_BREASTPLATE, Artifact::ULTIMATE_CROWN,
	    Artifact::SWORD_BREAKER, Artifact::BREASTPLATE_ANDURAN, Artifact::BATTLE_GARB,
	    Artifact::ULTIMATE_SHIELD, Artifact::ULTIMATE_CLOAK };

	const u8 size = sizeof(arts) / sizeof(Artifact::artifact_t);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], Artifact::GetExtraValue(arts[ii])));
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
	const Artifact::artifact_t arts[] = {
	    Artifact::WHITE_PEARL, Artifact::BLACK_PEARL, Artifact::CASTER_BRACELET,
	    Artifact::MAGE_RING, Artifact::LEGENDARY_SCEPTER, Artifact::WITCHES_BROACH,
	    Artifact::ARM_MARTYR, Artifact::ULTIMATE_CROWN, Artifact::ARCANE_NECKLACE,
	    Artifact::BATTLE_GARB, Artifact::STAFF_WIZARDRY, Artifact::HELMET_ANDURAN,
	    Artifact::ULTIMATE_STAFF, Artifact::ULTIMATE_WAND };

	const u8 size = sizeof(arts) / sizeof(Artifact::artifact_t);

	modifiers.reserve(size + 1);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], Artifact::GetExtraValue(arts[ii])));

	modifiers.push_back(std::make_pair(Artifact::BROACH_SHIELDING, -1));
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
	const Artifact::artifact_t arts[] = {
	    Artifact::WHITE_PEARL, Artifact::BLACK_PEARL, Artifact::MINOR_SCROLL,
	    Artifact::MAJOR_SCROLL, Artifact::SUPERIOR_SCROLL, Artifact::FOREMOST_SCROLL,
	    Artifact::LEGENDARY_SCEPTER, Artifact::ULTIMATE_CROWN,
	    Artifact::ULTIMATE_STAFF, Artifact::ULTIMATE_BOOK };

	const u8 size = sizeof(arts) / sizeof(Artifact::artifact_t);

	modifiers.reserve(size);
	for(u8 ii = 0; ii < size; ++ii) 
	    modifiers.push_back(std::make_pair(arts[ii], Artifact::GetExtraValue(arts[ii])));
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

    modifiers.reserve(7);

    modifiers.push_back(std::make_pair(Artifact::MEDAL_VALOR, Artifact::GetExtraValue(Artifact::MEDAL_VALOR)));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_COURAGE, Artifact::GetExtraValue(Artifact::MEDAL_COURAGE)));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_HONOR, Artifact::GetExtraValue(Artifact::MEDAL_HONOR)));
    modifiers.push_back(std::make_pair(Artifact::MEDAL_DISTINCTION, Artifact::GetExtraValue(Artifact::MEDAL_DISTINCTION)));
    modifiers.push_back(std::make_pair(Artifact::BATTLE_GARB, 10));

    if(shipmaster) modifiers.push_back(std::make_pair(Artifact::MASTHEAD, Artifact::GetExtraValue(Artifact::MASTHEAD)));
    modifiers.push_back(std::make_pair(Artifact::FIZBIN_MISFORTUNE, -Artifact::GetExtraValue(Artifact::FIZBIN_MISFORTUNE)));

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

    modifiers.reserve(6);

    modifiers.push_back(std::make_pair(Artifact::RABBIT_FOOT, Artifact::GetExtraValue(Artifact::RABBIT_FOOT)));
    modifiers.push_back(std::make_pair(Artifact::GOLDEN_HORSESHOE, Artifact::GetExtraValue(Artifact::GOLDEN_HORSESHOE)));
    modifiers.push_back(std::make_pair(Artifact::GAMBLER_LUCKY_COIN, Artifact::GetExtraValue(Artifact::GAMBLER_LUCKY_COIN)));
    modifiers.push_back(std::make_pair(Artifact::FOUR_LEAF_CLOVER, Artifact::GetExtraValue(Artifact::FOUR_LEAF_CLOVER)));
    modifiers.push_back(std::make_pair(Artifact::BATTLE_GARB, 10));
    if(shipmaster) modifiers.push_back(std::make_pair(Artifact::MASTHEAD, Artifact::GetExtraValue(Artifact::MASTHEAD)));

    s8 result = GetResultModifiers(modifiers, *this, strs);

    // check castle modificator
    const Castle* castle = inCastle();

    if(castle)
	result += castle->GetLuckModificator(strs);

    // army modificator
    result += GetArmy().GetLuckModificator(strs);

    return result;
}
