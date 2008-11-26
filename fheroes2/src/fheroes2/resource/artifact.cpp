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

#include "agg.h"
#include "maps_tiles.h"
#include "settings.h"
#include "world.h"
#include "artifact.h"

namespace Artifact {

    static const stats_t all_artifacts[] = {
	{ "Ultimate Book", "The Ultimate Book of Knowledge increases your knowledge by 12." },
	{ "Ultimate Sword", "The Ultimate Sword of Dominion increases your attack skill by 12." },
	{ "Ultimate Cloak", "The Ultimate Cloak of Protection increases your defense skill by 12." },
	{ "Ultimate Wand", "The Ultimate Wand of Magic increases your spell power by 12." },
	{ "Ultimate Shield", "The Ultimate Shield increases your attack and defense skills by 6 each." },
	{ "Ultimate Staff", "The Ultimate Staff increases your spell power and knowledge by 6 each." },
	{ "Ultimate Crown", "The Ultimate Crown increases each of your basic skills by 4 points." },
	{ "Ultimate Goose", "The Golden Goose brings in an income of 10,000 gold per turn." },
	{ "Arcane Necklace", "The Arcane Necklace of Magic increases your spell power by 4." },
	{ "Caster's Bracelet", "The Caster's Bracelet of Magic increases your spell power by 2." },
	{ "Mage's Ring", "The Mage's Ring of Power increases your spell power by 2." },
	{ "Witches Broach", "The Witch's Broach of Magic increases your spell power by 3." },
	{ "Medal of Valor", "The Medal of Valor increases your morale." },
	{ "Medal of Courage", "The Medal of Courage increases your morale." },
	{ "Medal of Honor", "The Medal of Honor increases your morale." },
	{ "Medal of Distinction", "The Medal of Distinction increases your morale." },
	{ "Fizbin of Misfortune", "The Fizbin of Misfortune greatly decreases your morale." },
	{ "Thunder Mace", "The Thunder Mace of Dominion increases your attack skill by 1." },
	{ "Armored Gauntlets", "The Armored Gauntlets of Protection increase your defense skill by 1." },
	{ "Defender Helm", "The Defender Helm of Protection increases your defense skill by 1." },
	{ "Giant Flail", "The Giant Flail of Dominion increases your attack skill by 1." },
	{ "Ballista", "The Ballista of Quickness lets your catapult fire twice per combat round." },
	{ "Stealth Shield", "The Stealth Shield of Protection increases your defense skill by 2." },
	{ "Dragon Sword", "The Dragon Sword of Dominion increases your attack skill by 3." },
	{ "Power Axe", "The Power Axe of Dominion increases your attack skill by 2." },
	{ "Divine Breastplate", "The Divine Breastplate of Protection increases your defense skill by 3." },
	{ "Minor Scroll", "The Minor Scroll of Knowledge increases your knowledge by 2." },
	{ "Major Scroll", "The Major Scroll of Knowledge increases your knowledge by 3." },
	{ "Superior Scroll", "The Superior Scroll of Knowledge increases your knowledge by 4." },
	{ "Foremost Scroll", "The Foremost Scroll of Knowledge increases your knowledge by 5." },
	{ "Endless Sack of Gold", "The Endless Sack of Gold provides you with 1000 gold per day." },
	{ "Endless Bag of Gold", "The Endless Bag of Gold provides you with 750 gold per day." },
	{ "Endless Purse of Gold", "The Endless Purse of Gold provides you with 500 gold per day." },
	{ "Nomad Boots of Mobility", "The Nomad Boots of Mobility increase your movement on land." },
	{ "Traveler's Boots of Mobility", "The Traveler's Boots of Mobility increase your movement on land." },
	{ "Rabbit's Foot", "The Lucky Rabbit's Foot increases your luck in combat." },
	{ "Golden Horseshoe", "The Golden Horseshoe increases your luck in combat." },
	{ "Gambler's Lucky Coin", "The Gambler's Lucky Coin increases your luck in combat." },
	{ "Four-Leaf Clover", "The Four_Leaf Clover increases your luck in combat." },
	{ "True Compass of Mobility", "The True Compass of Mobility increases your movement on land and sea." },
	{ "Sailor's Astrolabe of Mobility", "The Sailors' Astrolabe of Mobility increases your movement on sea." },
	{ "Evil Eye", "The Evil Eye reduces the casting cost of curse spells by half." },
	{ "Enchanted Hourglass", "The Enchanted Hourglass extends the duration of all your spells by 2 turns." },
	{ "Gold Wath", "The Gold Watch doubles the effectiveness of your hypnotize spells." },
	{ "Skullcap", "The Skullcap halves the casting cost of all mind influencing spells." },
	{ "Ice Cloak", "The Ice Cloak halves all damage your troops take from cold spells." },
	{ "Fire Cloak", "The Fire Cloak halves all damage your troops take from fire spells." },
	{ "Lightning Helm", "The Lightning Helm halves all damage your troops take from lightning spells." },
	{ "Evercold Icicle", "The Evercold Icicle causes your cold spells to do 50% more damage to enemy troops." },
	{ "Everhot Lava Rock", "The Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops." },
	{ "Lightning Rod", "The Lightning Rod causes your lightning spells to do 50% more damage to enemy troops." },
	{ "Snake-Ring", "The Snake Ring halves the casting cost of all your bless spells." },
	{ "Ankh", "The Ankh doubles the effectiveness of all your resurrect and animate spells." },
	{ "Book of Elements", "The Book of Elements doubles the effectiveness of all your summoning spells." },
	{ "Elemental Ring", "The Elemental Ring halves the casting cost of all summoning spells." },
	{ "Holy Pendant", "The Holy Pendant makes all your troops immune to curse spells." },
	{ "Pendant of Free Will", "The Pendant of Free Will makes all your troops immune to hypnotize spells." },
	{ "Pendant of Life", "The Pendant of Life makes all your troops immune to death spells." },
	{ "Serenity Pendant", "The Serenity Pendant makes all your troops immune to berserk spells." },
	{ "Seeing-eye Pendant", "The Seeing-eye Pendant makes all your troops immune to blindness spells." },
	{ "Kinetic Pendant" , "The Kinetic Pendant makes all your troops immune to paralyze spells." },
	{ "Pendant of Death", "The Pendant of Death makes all your troops immune to holy spells." },
	{ "Wand of Negation", "The Wand of Negation protects your troops from the Dispel Magic spell." },
	{ "Golden Bow", "The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)" },
	{ "Telescope", "The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square." },
	{ "Statesman's Quill", "The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army." },
	{ "Wizard's Hat", "The Wizard's Hat increases the duration of your spells by 10 turns!" },
	{ "Power Ring", "The Power Ring returns 2 extra power points/turn to your hero." },
	{ "Ammo Cart", "The Ammo Cart provides endless ammunition for all your troops that shoot." },
	{ "Tax Lien", "The Tax Lien costs you 250 gold pieces/turn." },
	{ "Hideous Mask", "The Hideous Mask prevents all 'wandering' armies from joining your hero." },
	{ "Endless Pouch of Sulfur", "The Endless Pouch of Sulfur provides 1 unit of sulfur per day." },
	{ "Endless Vial of Mercury", "The Endless Vial of Mercury provides 1 unit of mercury per day." },
	{ "Endless Pouch of Gems", "The Endless Pouch of Gems provides 1 unit of gems per day." },
	{ "Endless Cord of Wood", "The Endless Cord of Wood provides 1 unit of wood per day." },
	{ "Endless Cart of Ore", "The Endless Cart of Ore provides 1 unit of ore per day." },
	{ "Endless Pouch of Crystal", "The Endless Pouch of Crystal provides 1 unit of crystal/day." },
	{ "Spiked Helm", "The Spiked Helm increases your attack and defense skills by 1 each." },
	{ "Spiked Shield", "The Spiked Shield increases your attack and defense skills by 2 each." },
	{ "White Pearl", "The White Pearl increases your spell power and knowledge by 1 each." },
	{ "Black Pearl", "The Black Pearl increases your spell power and knowledge by 2 each." },

	{ "Magic Book", "The Magic Book enables you to cast spells." },
	
	{ "Dummy 1", "The reserved artifact.", },
	{ "Dummy 2", "The reserved artifact.", },
	{ "Dummy 3", "The reserved artifact.", },
	{ "Dummy 4", "The reserved artifact.", },

	{ "Spell Scroll", "This Spell Scroll gives your hero the ability to cast the '%s' spell. "},
	{ "Arm of the Martyr", "The Arm of the Martyr increases your spell power by 3 but adds the undead morale penalty. "},
	{ "Breastplate of Anduran", "The Breastplate increases your defense by 5. "},
	{ "Broach of Shielding", "The Broach of Shielding provides 50% protection from Armageddon and Elemental Storm, but decreases spell power by 2. "},
	{ "Battle Garb", "The Battle Garb of Anduran combines the powers of the three Anduran artifacts.  It provides maximum luck and morale for your troops and gives you the Town Portal spell. "},
	{ "Crystal Ball", "The Crystal Ball lets you get more specific information about monsters, enemy heroes, and castles nearby the hero who holds it. "},
	{ "Heart of Fire", "The Heart of Fire provides 50% protection from fire, but doubles the damage taken from cold. "},
	{ "Heart of Ice", "The Heart of Ice provides 50% protection from cold, but doubles the damage taken from fire. "},
	{ "Helmet of Anduran", "The Helmet increases your spell power by 5. "},
	{ "Holy Hammer" ,"The Holy Hammer increases your attack skill by 5. "},
	{ "Legendary Scepter" ,"The Legendary Scepter adds 2 points to all attributes. "},
	{ "Masthead" ,"The Masthead boosts your luck and morale by 1 each in sea combat. "},
	{ "Sphere of Negation", "The Sphere of Negation disables all spell casting, for both sides, in combat. "},
	{ "Staff of Wizardry", "The Staff of Wizardry boosts your spell power by 5. "},
	{ "Sword Breaker", "The Sword Breaker increases your defense by 4 and attack by 1. "},
	{ "Sword of Anduran", "The Sword increases your attack skill by 5. "},
	{ "Spade of Necromancy", "The Spade gives you increased necromancy skill. "},

	{ "Unknown", "Unknown" },
    };

}

Artifact::artifact_t Artifact::Artifact(u8 index)
{
    switch(index)
    {
        case 0x00: return ULTIMATE_BOOK;
        case 0x01: return ULTIMATE_SWORD;
        case 0x02: return ULTIMATE_CLOAK;
        case 0x03: return ULTIMATE_WAND;
        case 0x04: return ULTIMATE_SHIELD;
        case 0x05: return ULTIMATE_STAFF;
        case 0x06: return ULTIMATE_CROWN;
        case 0x07: return GOLDEN_GOOSE;
	case 0x08: return ARCANE_NECKLACE;
	case 0x09: return CASTER_BRACELET;
	case 0x0A: return MAGE_RING;
	case 0x0B: return WITCHES_BROACH;
	case 0x0C: return MEDAL_VALOR;
	case 0x0D: return MEDAL_COURAGE;
	case 0x0E: return MEDAL_HONOR;
	case 0x0F: return MEDAL_DISTINCTION;
	case 0x10: return FIZBIN_MISFORTUNE;
	case 0x11: return THUNDER_MACE;
	case 0x12: return ARMORED_GAUNTLETS;
	case 0x13: return DEFENDER_HELM;
	case 0x14: return GIANT_FLAIL;
	case 0x15: return BALLISTA;
	case 0x16: return STEALTH_SHIELD;
	case 0x17: return DRAGON_SWORD;
	case 0x18: return POWER_AXE;
	case 0x19: return DIVINE_BREASTPLATE;
	case 0x1A: return MINOR_SCROLL;
	case 0x1B: return MAJOR_SCROLL;
	case 0x1C: return SUPERIOR_SCROLL;
	case 0x1D: return FOREMOST_SCROLL;
	case 0x1E: return ENDLESS_SACK_GOLD;
	case 0x1F: return ENDLESS_BAG_GOLD;
	case 0x20: return ENDLESS_PURSE_GOLD;
	case 0x21: return NOMAD_BOOTS_MOBILITY;
	case 0x22: return TRAVELER_BOOTS_MOBILITY;
	case 0x23: return RABBIT_FOOT;
	case 0x24: return GOLDEN_HORSESHOE;
	case 0x25: return GAMBLER_LUCKY_COIN;
	case 0x26: return FOUR_LEAF_CLOVER;
	case 0x27: return TRUE_COMPASS_MOBILITY;
	case 0x28: return SAILORS_ASTROLABE_MOBILITY;
	case 0x29: return EVIL_EYE;
	case 0x2A: return ENCHANTED_HOURGLASS;
	case 0x2B: return GOLD_WATCH;
	case 0x2C: return SKULLCAP;
	case 0x2D: return ICE_CLOAK;
	case 0x2E: return FIRE_CLOAK;
	case 0x2F: return LIGHTNING_HELM;
	case 0x30: return EVERCOLD_ICICLE;
	case 0x31: return EVERHOT_LAVA_ROCK;
	case 0x32: return LIGHTNING_ROD;
	case 0x33: return SNAKE_RING;
	case 0x34: return ANKH;
	case 0x35: return BOOK_ELEMENTS;
	case 0x36: return ELEMENTAL_RING;
	case 0x37: return HOLY_PENDANT;
	case 0x38: return PENDANT_FREE_WILL;
	case 0x39: return PENDANT_LIFE;
	case 0x3A: return SERENITY_PENDANT;
	case 0x3B: return SEEING_EYE_PENDANT;
	case 0x3C: return KINETIC_PENDANT;
	case 0x3D: return PENDANT_DEATH;
	case 0x3E: return WAND_NEGATION;
	case 0x3F: return GOLDEN_BOW;
	case 0x40: return TELESCOPE;
	case 0x41: return STATESMAN_QUILL;
	case 0x42: return WIZARD_HAT;
	case 0x43: return POWER_RING;
	case 0x44: return AMMO_CART;
	case 0x45: return TAX_LIEN;
	case 0x46: return HIDEOUS_MASK;
	case 0x47: return ENDLESS_POUCH_SULFUR;
	case 0x48: return ENDLESS_VIAL_MERCURY;
	case 0x49: return ENDLESS_POUCH_GEMS;
	case 0x4A: return ENDLESS_CORD_WOOD;
	case 0x4B: return ENDLESS_CART_ORE;
	case 0x4C: return ENDLESS_POUCH_CRYSTAL;
	case 0x4D: return SPIKED_HELM;
	case 0x4E: return SPIKED_SHIELD;
	case 0x4F: return WHITE_PEARL;
	case 0x50: return BLACK_PEARL;
	case 0x51: return MAGIC_BOOK;
	case 0x52: return DUMMY1;
	case 0x53: return DUMMY2;
	case 0x54: return DUMMY3;
	case 0x55: return DUMMY4;
	case 0x56: return SPELL_SCROLL;
	case 0x57: return ARM_MARTYR;
	case 0x58: return BREASTPLATE_ANDURAN;
	case 0x59: return BROACH_SHIELDING;
	case 0x5A: return BATTLE_GARB;
	case 0x5B: return CRYSTAL_BALL;
	case 0x5C: return HEART_FIRE;
	case 0x5D: return HEART_ICE;
	case 0x5E: return HELMET_ANDURAN;
	case 0x5F: return HOLY_HAMMER;
	case 0x60: return LEGENDARY_SCEPTER;
	case 0x61: return MASTHEAD;
	case 0x62: return SPHERE_NEGATION;
	case 0x63: return STAFF_WIZARDRY;
	case 0x64: return SWORD_BREAKER;
	case 0x65: return SWORD_ANDURAN;
	case 0x66: return SPADE_NECROMANCY;

	case 0xFF: return UNKNOWN;

	// not used
	default:
	    Error::Warning("Artifact::Artifact: unknown: ", index);
    }
    
    return Artifact::UNKNOWN;
}

Artifact::artifact_t Artifact::FromIndexSprite(u8 index)
{
    if(0x10 < index && 0xA2 > index) return Artifact((index - 1)/2);
    else
    if(Settings::Get().Modes(Settings::PRICELOYALTY) && 0xAB < index && 0xCE > index) return Artifact((index - 1)/2);
    else
    if(0xA3 == index) return Artifact::Rand();
    else
    if(0xA4 == index) return Artifact::RandUltimate();
    else
    if(0xA7 == index) return Artifact::Rand1();
    else
    if(0xA9 == index) return Artifact::Rand2();
    else
    if(0xAB == index) return Artifact::Rand3();
    else
	Error::Warning("Artifact::FromMP2: unknown: ", index);
    
    return Artifact::UNKNOWN;
}

/* artifact name */
const std::string & Artifact::String(artifact_t artifact)
{ return (MAGIC_BOOK < artifact ? all_artifacts[MAGIC_BOOK + 1].name : all_artifacts[artifact].name); }

/* artifact description */
const std::string & Artifact::Description(artifact_t artifact)
{ return (MAGIC_BOOK < artifact ? all_artifacts[MAGIC_BOOK + 1].description : all_artifacts[artifact].description); }


Artifact::artifact_t Artifact::RandUltimate(void)
{
    switch(Rand::Get(1, 8))
    {
	case 1: return Artifact::ULTIMATE_BOOK;
        case 2: return Artifact::ULTIMATE_SWORD;
        case 3: return Artifact::ULTIMATE_CLOAK;
        case 4: return Artifact::ULTIMATE_WAND;
        case 5: return Artifact::ULTIMATE_SHIELD;
        case 6: return Artifact::ULTIMATE_STAFF;
        case 7: return Artifact::ULTIMATE_CROWN;
	default: break;
    }
    return Artifact::GOLDEN_GOOSE;
}

Artifact::artifact_t Artifact::Rand(MP2::object_t object)
{
    switch(object)
    {
	case MP2::OBJ_RNDARTIFACT1: return Rand1();
	case MP2::OBJ_RNDARTIFACT2: return Rand2();
	case MP2::OBJ_RNDARTIFACT3: return Rand3();
	default: break;
    }
    
    return Rand();
}

/* get rand all artifact */
Artifact::artifact_t Artifact::Rand(void)
{
    switch(Rand::Get(1, 3))
    {
	case 1: return Artifact::Rand1();
	case 2: return Artifact::Rand2();
	default: break;
    }

    return Artifact::Rand3();
}

/* get rand level 1 artifact */
Artifact::artifact_t Artifact::Rand1(void)
{
    switch(Rand::Get(1, 27))
    {
        case 1: return Artifact::MEDAL_VALOR;
        case 2: return Artifact::MEDAL_COURAGE;
        case 3: return Artifact::MEDAL_HONOR;
        case 4: return Artifact::MEDAL_DISTINCTION;
        case 5: return Artifact::THUNDER_MACE;
        case 6: return Artifact::ARMORED_GAUNTLETS;
        case 7: return Artifact::DEFENDER_HELM;
        case 8: return Artifact::GIANT_FLAIL;
        case 9: return Artifact::RABBIT_FOOT;
        case 10: return Artifact::GOLDEN_HORSESHOE;
        case 11: return Artifact::GAMBLER_LUCKY_COIN;
        case 12: return Artifact::FOUR_LEAF_CLOVER;
        case 13: return Artifact::ENCHANTED_HOURGLASS;
        case 14: return Artifact::ICE_CLOAK;
        case 15: return Artifact::FIRE_CLOAK;
        case 16: return Artifact::LIGHTNING_HELM;
        case 17: return Artifact::SNAKE_RING;
        case 18: return Artifact::HOLY_PENDANT;
        case 19: return Artifact::PENDANT_FREE_WILL;
        case 20: return Artifact::PENDANT_LIFE;
        case 21: return Artifact::PENDANT_DEATH;
        case 22: return Artifact::GOLDEN_BOW;
        case 23: return Artifact::TELESCOPE;
        case 24: return Artifact::SERENITY_PENDANT;
        case 25: return Artifact::STATESMAN_QUILL;
        case 26: return Artifact::KINETIC_PENDANT;
	default: break;
    }

    return Artifact::SEEING_EYE_PENDANT;
}

/* get rand level 2 artifact */
Artifact::artifact_t Artifact::Rand2(void)
{
    switch(Rand::Get(1, 21))
    {
        case 1: return Artifact::CASTER_BRACELET;
        case 2: return Artifact::MAGE_RING;
        case 3: return Artifact::STEALTH_SHIELD;
        case 4: return Artifact::POWER_AXE;
        case 5: return Artifact::MINOR_SCROLL;
        case 6: return Artifact::ENDLESS_PURSE_GOLD;
        case 7: return Artifact::SAILORS_ASTROLABE_MOBILITY;
        case 8: return Artifact::ENDLESS_CORD_WOOD;
        case 9: return Artifact::ENDLESS_CART_ORE;
        case 10: return Artifact::SPIKED_HELM;
        case 11: return Artifact::WHITE_PEARL;
        case 12: return Artifact::EVIL_EYE;
        case 13: return Artifact::GOLD_WATCH;
        case 14: return Artifact::ANKH;
        case 15: return Artifact::BOOK_ELEMENTS;
        case 16: return Artifact::ELEMENTAL_RING;
        case 17: return Artifact::SKULLCAP;
        case 18: return Artifact::EVERCOLD_ICICLE;
        case 19: return Artifact::POWER_RING;
        case 20: return Artifact::AMMO_CART;
	default: break;
    }
    
    return Artifact::EVERHOT_LAVA_ROCK;
}

/* get rand level 3 artifact */
Artifact::artifact_t Artifact::Rand3(void)
{
    switch(Rand::Get(1, 22))
    {
        case 1: return Artifact::ARCANE_NECKLACE;
        case 2: return Artifact::WITCHES_BROACH;
        case 3: return Artifact::BALLISTA;
        case 4: return Artifact::DRAGON_SWORD;
        case 5: return Artifact::DIVINE_BREASTPLATE;
        case 6: return Artifact::MAJOR_SCROLL;
        case 7: return Artifact::SUPERIOR_SCROLL;
        case 8: return Artifact::FOREMOST_SCROLL;
        case 9: return Artifact::ENDLESS_SACK_GOLD;
        case 10: return Artifact::ENDLESS_BAG_GOLD;
        case 11: return Artifact::NOMAD_BOOTS_MOBILITY;
        case 12: return Artifact::TRAVELER_BOOTS_MOBILITY;
        case 13: return Artifact::TRUE_COMPASS_MOBILITY;
        case 14: return Artifact::ENDLESS_POUCH_SULFUR;
        case 15: return Artifact::ENDLESS_POUCH_GEMS;
        case 16: return Artifact::ENDLESS_POUCH_CRYSTAL;
        case 17: return Artifact::ENDLESS_VIAL_MERCURY;
        case 18: return Artifact::SPIKED_SHIELD;
        case 19: return Artifact::BLACK_PEARL;
        case 20: return Artifact::LIGHTNING_ROD;
        case 21: return Artifact::WAND_NEGATION;
	default: break;
    }
    
    return Artifact::WIZARD_HAT;
}

/* return index sprite objnarti.icn */
u8 Artifact::GetIndexSprite(Artifact::artifact_t artifact)
{
    switch(artifact)
    {
	// null sprite
        case ULTIMATE_BOOK:			return 0x01;
	// null sprite
        case ULTIMATE_SWORD:			return 0x03;
	// null sprite
        case ULTIMATE_CLOAK:			return 0x05;
	// null sprite
	case ULTIMATE_WAND:			return 0x07;
	// sprite artifact
        case ULTIMATE_SHIELD:			return 0x09;
        case ULTIMATE_STAFF:			return 0x0B;
        case ULTIMATE_CROWN:			return 0x0D;
        case GOLDEN_GOOSE:			return 0x0F;
	case ARCANE_NECKLACE:			return 0x11;
	case CASTER_BRACELET:			return 0x13;
	case MAGE_RING:				return 0x15;
	case WITCHES_BROACH:			return 0x17;
	case MEDAL_VALOR:			return 0x19;
	case MEDAL_COURAGE:			return 0x1B;
	case MEDAL_HONOR:			return 0x1D;
	case MEDAL_DISTINCTION:			return 0x1F;
	case FIZBIN_MISFORTUNE:			return 0x21;
	case THUNDER_MACE:			return 0x23;
	case ARMORED_GAUNTLETS:			return 0x25;
	case DEFENDER_HELM:			return 0x27;
	case GIANT_FLAIL:			return 0x29;
	case BALLISTA:				return 0x2B;
	case STEALTH_SHIELD:			return 0x2D;
	case DRAGON_SWORD:			return 0x2F;
	case POWER_AXE:				return 0x31;
	case DIVINE_BREASTPLATE:		return 0x33;
	case MINOR_SCROLL:			return 0x35;
	case MAJOR_SCROLL:			return 0x37;
	case SUPERIOR_SCROLL:			return 0x39;
	case FOREMOST_SCROLL:			return 0x3B;
	case ENDLESS_SACK_GOLD:			return 0x3D;
	case ENDLESS_BAG_GOLD:			return 0x3F;
	case ENDLESS_PURSE_GOLD:		return 0x41;
	case NOMAD_BOOTS_MOBILITY:		return 0x43;
	case TRAVELER_BOOTS_MOBILITY:		return 0x45;
	case RABBIT_FOOT:			return 0x47;
	case GOLDEN_HORSESHOE:			return 0x49;
	case GAMBLER_LUCKY_COIN:		return 0x4B;
	case FOUR_LEAF_CLOVER:			return 0x4D;
	case TRUE_COMPASS_MOBILITY:		return 0x4F;
	case SAILORS_ASTROLABE_MOBILITY:	return 0x51;
	case EVIL_EYE:				return 0x53;
	case ENCHANTED_HOURGLASS:		return 0x55;
	case GOLD_WATCH:			return 0x57;
	case SKULLCAP:				return 0x59;
	case ICE_CLOAK:				return 0x5B;
	case FIRE_CLOAK:			return 0x5D;
	case LIGHTNING_HELM:			return 0x5F;
	case EVERCOLD_ICICLE:			return 0x61;
	case EVERHOT_LAVA_ROCK:			return 0x63;
	case LIGHTNING_ROD:			return 0x65;
	case SNAKE_RING:			return 0x67;
	case ANKH:				return 0x69;
	case BOOK_ELEMENTS:			return 0x6B;
	case ELEMENTAL_RING:			return 0x6D;
	case HOLY_PENDANT:			return 0x6F;
	case PENDANT_FREE_WILL:			return 0x71;
	case PENDANT_LIFE:			return 0x73;
	case SERENITY_PENDANT:			return 0x75;
	case SEEING_EYE_PENDANT:		return 0x77;
	case KINETIC_PENDANT:			return 0x79;
	case PENDANT_DEATH:			return 0x7B;
	case WAND_NEGATION:			return 0x7D;
	case GOLDEN_BOW:			return 0x7F;
	case TELESCOPE:				return 0x81;
	case STATESMAN_QUILL:			return 0x83;
	case WIZARD_HAT:			return 0x85;
	case POWER_RING:			return 0x87;
	case AMMO_CART:				return 0x89;
	case TAX_LIEN:				return 0x8B;
	case HIDEOUS_MASK:			return 0x8D;
	case ENDLESS_POUCH_SULFUR:		return 0x8F;
	case ENDLESS_VIAL_MERCURY:		return 0x91;
	case ENDLESS_POUCH_GEMS:		return 0x93;
	case ENDLESS_CORD_WOOD:			return 0x95;
	case ENDLESS_CART_ORE:			return 0x97;
	case ENDLESS_POUCH_CRYSTAL:		return 0x99;
	case SPIKED_HELM:			return 0x9B;
	case SPIKED_SHIELD:			return 0x9D;
	case WHITE_PEARL:			return 0x9F;
	case BLACK_PEARL:			return 0xA1;

	case MAGIC_BOOK:			return 0xA3;	// sprite RND_ARTIFACT
	case DUMMY1:				return 0xA4;	// sprite ULTIMA_ARTIFACT
	case DUMMY2:				return 0xA7;	// sprite RND1_ARTIFACT
	case DUMMY3:				return 0xA9;	// sprite RND2_ARTIFACT
	case DUMMY4:				return 0xAB;	// sprite RND3_ARTIFACT

	default: break;
    }

    if(Settings::Get().Modes(Settings::PRICELOYALTY))
    switch(artifact)
    {
	case SPELL_SCROLL:			return 0xAD;
	case ARM_MARTYR:			return 0xAF;
	case BREASTPLATE_ANDURAN:		return 0xB1;
	case BROACH_SHIELDING:			return 0xB3;
	case BATTLE_GARB:			return 0xB5;
	case CRYSTAL_BALL:			return 0xB7;
	case HEART_FIRE:			return 0xB9;
	case HEART_ICE:				return 0xBB;
	case HELMET_ANDURAN:			return 0xBD;
	case HOLY_HAMMER:			return 0xBF;
	case LEGENDARY_SCEPTER:			return 0xC1;
	case MASTHEAD:				return 0xC3;
	case SPHERE_NEGATION:			return 0xC5;
	case STAFF_WIZARDRY:			return 0xC7;
	case SWORD_BREAKER:			return 0xC9;
	case SWORD_ANDURAN:			return 0xCB;
	case SPADE_NECROMANCY:			return 0xCD;

	default: break;
    }

    Error::Warning("Artifact::GetIndexSprite: unknown:", artifact);

    // null sprite
    return 0;
}

bool Artifact::Ultimate(artifact_t art)
{
    switch(art)
    {
	case ULTIMATE_BOOK:
	case ULTIMATE_SWORD:
	case ULTIMATE_CLOAK:
	case ULTIMATE_WAND:
	case ULTIMATE_SHIELD:
	case ULTIMATE_STAFF:
	case ULTIMATE_CROWN:
	case GOLDEN_GOOSE:
	    return true;

	default: break;
    }

    return false;
}

void Artifact::ChangeTileWithRNDArtifact(Maps::Tiles & tile)
{
    Maps::TilesAddon *addon = NULL;

    u8 index = 0;

    switch(tile.GetObject())
    {
	case MP2::OBJ_RNDARTIFACT:
	    addon = tile.FindRNDArtifact(MP2::OBJ_RNDARTIFACT);
    	    index = Artifact::GetIndexSprite(Artifact::Rand(MP2::OBJ_RNDARTIFACT));
	    break;
	case MP2::OBJ_RNDARTIFACT1:
	    addon = tile.FindRNDArtifact(MP2::OBJ_RNDARTIFACT1);
    	    index = Artifact::GetIndexSprite(Artifact::Rand(MP2::OBJ_RNDARTIFACT1));
	    break;
	case MP2::OBJ_RNDARTIFACT2:
	    addon = tile.FindRNDArtifact(MP2::OBJ_RNDARTIFACT2);
    	    index = Artifact::GetIndexSprite(Artifact::Rand(MP2::OBJ_RNDARTIFACT2));
	    break;
	case MP2::OBJ_RNDARTIFACT3:
	    addon = tile.FindRNDArtifact(MP2::OBJ_RNDARTIFACT3);
    	    index = Artifact::GetIndexSprite(Artifact::Rand(MP2::OBJ_RNDARTIFACT3));
	    break;
	default:
	    return;
    }
    
    if(addon)
    {
	const u16 center = tile.GetIndex();
        const u32 uniq = addon->uniq;
        addon->index = index;
        tile.SetObject(MP2::OBJ_ARTIFACT);

        // replace shadow artifact
        if(Maps::isValidDirection(center, Direction::LEFT))
        {
            Maps::Tiles & left_tile = world.GetTiles(Maps::GetDirectionIndex(center, Direction::LEFT));
            Maps::TilesAddon *shadow = left_tile.FindAddonLevel1(uniq);

	    if(shadow) shadow->index = index - 1;
        }
    }
}
