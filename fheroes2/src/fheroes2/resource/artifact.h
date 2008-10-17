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
#ifndef H2ARTIFACT_H
#define H2ARTIFACT_H

#include <string>
#include <vector>
#include "maps_tiles.h"
#include "mp2.h"
#include "gamedefs.h"

namespace Artifact
{
    typedef enum {
	ULTIMATE_BOOK,
	ULTIMATE_SWORD,
	ULTIMATE_CLOAK,
	ULTIMATE_WAND,
	ULTIMATE_SHIELD,
	ULTIMATE_STAFF,
	ULTIMATE_CROWN,
	GOLDEN_GOOSE,
	ARCANE_NECKLACE,
	CASTER_BRACELET,
	MAGE_RING,
	WITCHES_BROACH,
	MEDAL_VALOR,
	MEDAL_COURAGE,
	MEDAL_HONOR,
	MEDAL_DISTINCTION,
	FIZBIN_MISFORTUNE,
	THUNDER_MACE,
	ARMORED_GAUNTLETS,
	DEFENDER_HELM,
	GIANT_FLAIL,
	BALLISTA,
	STEALTH_SHIELD,
	DRAGON_SWORD,
	POWER_AXE,
	DIVINE_BREASTPLATE,
	MINOR_SCROLL,
	MAJOR_SCROLL,
	SUPERIOR_SCROLL,
	FOREMOST_SCROLL,
	ENDLESS_SACK_GOLD,
	ENDLESS_BAG_GOLD,
	ENDLESS_PURSE_GOLD,
	NOMAD_BOOTS_MOBILITY,
	TRAVELER_BOOTS_MOBILITY,
	RABBIT_FOOT,
	GOLDEN_HORSESHOE,
	GAMBLER_LUCKY_COIN,
	FOUR_LEAF_CLOVER,
	TRUE_COMPASS_MOBILITY,
	SAILORS_ASTROLABE_MOBILITY,
	EVIL_EYE,
	ENCHANTED_HOURGLASS,
	GOLD_WATCH,
	SKULLCAP,
	ICE_CLOAK,
	FIRE_CLOAK,
	LIGHTNING_HELM,
	EVERCOLD_ICICLE,
	EVERHOT_LAVA_ROCK,
	LIGHTNING_ROD,
	SNAKE_RING,
	ANKH,
	BOOK_ELEMENTS,
	ELEMENTAL_RING,
	HOLY_PENDANT,
	PENDANT_FREE_WILL,
	PENDANT_LIFE,
	SERENITY_PENDANT,
	SEEING_EYE_PENDANT,
	KINETIC_PENDANT,
	PENDANT_DEATH,
	WAND_NEGATION,
	GOLDEN_BOW,
	TELESCOPE,
	STATESMAN_QUILL,
	WIZARD_HAT,
	POWER_RING,
	AMMO_CART,
	TAX_LIEN,
	HIDEOUS_MASK,
	ENDLESS_POUCH_SULFUR,
	ENDLESS_VIAL_MERCURY,
	ENDLESS_POUCH_GEMS,
	ENDLESS_CORD_WOOD,
	ENDLESS_CART_ORE,
	ENDLESS_POUCH_CRYSTAL,
	SPIKED_HELM,
	SPIKED_SHIELD,
	WHITE_PEARL,
	BLACK_PEARL,

	MAGIC_BOOK,

	DUMMY1,
	DUMMY2,
	DUMMY3,
	DUMMY4,
	SPELL_SCROLL,
	ARM_MARTYR,
	BREASTPLATE_ANDURAN,
	BROACH_SHIELDING,
	BATTLE_GARB,
	CRYSTAL_BALL,
	HEART_FIRE,
	HEART_ICE,
	HELMET_ANDURAN,
	HOLY_HAMMER,
	LEGENDARY_SCEPTER,
	MASTHEAD,
	SPHERE_NEGATION,
	STAFF_WIZARDRY,
	SWORD_BREAKER,
	SWORD_ANDURAN,
	SPADE_NECROMANCY,

	UNKNOWN

    } artifact_t;


    typedef struct {
        const std::string name;
        const std::string description;
    } stats_t;

    artifact_t RandUltimate(void);
    artifact_t Rand(MP2::object_t object);
    artifact_t Rand(void);
    artifact_t Rand1(void);
    artifact_t Rand2(void);
    artifact_t Rand3(void);

    const std::string & String(artifact_t artifact);
    const std::string & Description(artifact_t artifact);
    
    artifact_t Artifact(u8 index);
    artifact_t FromIndexSprite(u8 index);
    
    u8 GetIndexSprite(Artifact::artifact_t artifact);
    
    void ChangeTileWithRNDArtifact(Maps::Tiles & tile);
};

#endif
