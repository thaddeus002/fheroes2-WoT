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
#include "error.h"
#include "rand.h"
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
	{ "Ice Clock", "The Ice Cloak halves all damage your troops take from cold spells." },
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

	{ "Unknown", "Unknown" },
    };

}

Artifact::artifact_t Artifact::FromMP2(u8 index)
{
    switch(index){
	case 0x11: return Artifact::ARCANE_NECKLACE;
	case 0x13: return Artifact::CASTER_BRACELET;
	case 0x15: return Artifact::MAGE_RING;
	case 0x17: return Artifact::WITCHES_BROACH;
	case 0x19: return Artifact::MEDAL_VALOR;
	case 0x1B: return Artifact::MEDAL_COURAGE;
	case 0x1D: return Artifact::MEDAL_HONOR;
	case 0x1F: return Artifact::MEDAL_DISTINCTION;
	case 0x21: return Artifact::FIZBIN_MISFORTUNE;
	case 0x23: return Artifact::THUNDER_MACE;
	case 0x25: return Artifact::ARMORED_GAUNTLETS;
	case 0x27: return Artifact::DEFENDER_HELM;
	case 0x29: return Artifact::GIANT_FLAIL;
	case 0x2B: return Artifact::BALLISTA;
	case 0x2D: return Artifact::STEALTH_SHIELD;
	case 0x2F: return Artifact::DRAGON_SWORD;
	case 0x31: return Artifact::POWER_AXE;
	case 0x33: return Artifact::DIVINE_BREASTPLATE;
	case 0x35: return Artifact::MINOR_SCROLL;
	case 0x37: return Artifact::MAJOR_SCROLL;
	case 0x39: return Artifact::SUPERIOR_SCROLL;
	case 0x3B: return Artifact::FOREMOST_SCROLL;
	case 0x3D: return Artifact::ENDLESS_SACK_GOLD;
	case 0x3F: return Artifact::ENDLESS_BAG_GOLD;
	case 0x41: return Artifact::ENDLESS_PURSE_GOLD;
	case 0x43: return Artifact::NOMAD_BOOTS_MOBILITY;
	case 0x45: return Artifact::TRAVELER_BOOTS_MOBILITY;
	case 0x47: return Artifact::RABBIT_FOOT;
	case 0x49: return Artifact::GOLDEN_HORSESHOE;
	case 0x4B: return Artifact::GAMBLER_LUCKY_COIN;
	case 0x4D: return Artifact::FOUR_LEAF_CLOVER;
	case 0x4F: return Artifact::TRUE_COMPASS_MOBILITY;
	case 0x51: return Artifact::SAILORS_ASTROLABE_MOBILITY;
	case 0x53: return Artifact::EVIL_EYE;
	case 0x55: return Artifact::ENCHANTED_HOURGLASS;
	case 0x57: return Artifact::GOLD_WATCH;
	case 0x59: return Artifact::SKULLCAP;
	case 0x5B: return Artifact::ICE_CLOAK;
	case 0x5D: return Artifact::FIRE_CLOAK;
	case 0x5F: return Artifact::LIGHTNING_HELM;
	case 0x61: return Artifact::EVERCOLD_ICICLE;
	case 0x63: return Artifact::EVERHOT_LAVA_ROCK;
	case 0x65: return Artifact::LIGHTNING_ROD;
	case 0x67: return Artifact::SNAKE_RING;
	case 0x69: return Artifact::ANKH;
	case 0x6B: return Artifact::BOOK_ELEMENTS;
	case 0x6D: return Artifact::ELEMENTAL_RING;
	case 0x6F: return Artifact::HOLY_PENDANT;
	case 0x71: return Artifact::PENDANT_FREE_WILL;
	case 0x73: return Artifact::PENDANT_LIFE;
	case 0x75: return Artifact::SERENITY_PENDANT;
	case 0x77: return Artifact::SEEING_EYE_PENDANT;
	case 0x79: return Artifact::KINETIC_PENDANT;
	case 0x7B: return Artifact::PENDANT_DEATH;
	case 0x7D: return Artifact::WAND_NEGATION;
	case 0x7F: return Artifact::GOLDEN_BOW;
	case 0x81: return Artifact::TELESCOPE;
	case 0x83: return Artifact::STATESMAN_QUILL;
	case 0x85: return Artifact::WIZARD_HAT;
	case 0x87: return Artifact::POWER_RING;
	case 0x89: return Artifact::AMMO_CART;
	case 0x8B: return Artifact::TAX_LIEN;
	case 0x8D: return Artifact::HIDEOUS_MASK;
	case 0x8F: return Artifact::ENDLESS_POUCH_SULFUR;
	case 0x91: return Artifact::ENDLESS_VIAL_MERCURY;
	case 0x93: return Artifact::ENDLESS_POUCH_GEMS;
	case 0x95: return Artifact::ENDLESS_CORD_WOOD;
	case 0x97: return Artifact::ENDLESS_CART_ORE;
	case 0x99: return Artifact::ENDLESS_POUCH_CRYSTAL;
	case 0x9B: return Artifact::SPIKED_HELM;
	case 0x9D: return Artifact::SPIKED_SHIELD;
	case 0x9F: return Artifact::WHITE_PEARL;
	case 0xA1: return Artifact::BLACK_PEARL;
	
	// special
	case 0xA3: return Artifact::Rand();
	case 0xA4: return Artifact::RandUltimate();
	case 0xA7: return Artifact::Rand1();
	case 0xA9: return Artifact::Rand2();
	case 0xAB: return Artifact::Rand3();

	// not used
	default:
	    Error::Warning("Artifact::FromMP2: unknown: ", index);
    }
    
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
    switch(Rand::Get(1, 8)){
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

/* get rand all artifact */
Artifact::artifact_t Artifact::Rand(void)
{
    switch(Rand::Get(1, 3)){
	case 1: return Artifact::Rand1();
	case 2: return Artifact::Rand2();
	default: break;
    }

    return Artifact::Rand3();
}

/* get rand level 1 artifact */
Artifact::artifact_t Artifact::Rand1(void)
{
    switch(Rand::Get(1, 27)){
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
    switch(Rand::Get(1, 21)){
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
    switch(Rand::Get(1, 22)){
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

/* greturn sprite artifact */
const Sprite & Artifact::GetForMapsSprite(Artifact::artifact_t artifact)
{
    switch(artifact){
	// null sprite
        case Artifact::ULTIMATE_BOOK:			return AGG::GetICN("OBJNARTI.ICN", 0x01);
	// null sprite
        case Artifact::ULTIMATE_SWORD:			return AGG::GetICN("OBJNARTI.ICN", 0x03);
	// null sprite
        case Artifact::ULTIMATE_CLOAK:			return AGG::GetICN("OBJNARTI.ICN", 0x05);
	// null sprite
	case Artifact::ULTIMATE_WAND:			return AGG::GetICN("OBJNARTI.ICN", 0x07);
	// sprite artifact
        case Artifact::ULTIMATE_SHIELD:			return AGG::GetICN("OBJNARTI.ICN", 0x09);
        case Artifact::ULTIMATE_STAFF:			return AGG::GetICN("OBJNARTI.ICN", 0x0B);
        case Artifact::ULTIMATE_CROWN:			return AGG::GetICN("OBJNARTI.ICN", 0x0D);
        case Artifact::GOLDEN_GOOSE:			return AGG::GetICN("OBJNARTI.ICN", 0x0F);
	case Artifact::ARCANE_NECKLACE:			return AGG::GetICN("OBJNARTI.ICN", 0x11);
	case Artifact::CASTER_BRACELET:			return AGG::GetICN("OBJNARTI.ICN", 0x13);
	case Artifact::MAGE_RING:			return AGG::GetICN("OBJNARTI.ICN", 0x15);
	case Artifact::WITCHES_BROACH:			return AGG::GetICN("OBJNARTI.ICN", 0x17);
	case Artifact::MEDAL_VALOR:			return AGG::GetICN("OBJNARTI.ICN", 0x19);
	case Artifact::MEDAL_COURAGE:			return AGG::GetICN("OBJNARTI.ICN", 0x1B);
	case Artifact::MEDAL_HONOR:			return AGG::GetICN("OBJNARTI.ICN", 0x1D);
	case Artifact::MEDAL_DISTINCTION:		return AGG::GetICN("OBJNARTI.ICN", 0x1F);
	case Artifact::FIZBIN_MISFORTUNE:		return AGG::GetICN("OBJNARTI.ICN", 0x21);
	case Artifact::THUNDER_MACE:			return AGG::GetICN("OBJNARTI.ICN", 0x23);
	case Artifact::ARMORED_GAUNTLETS:		return AGG::GetICN("OBJNARTI.ICN", 0x25);
	case Artifact::DEFENDER_HELM:			return AGG::GetICN("OBJNARTI.ICN", 0x27);
	case Artifact::GIANT_FLAIL:			return AGG::GetICN("OBJNARTI.ICN", 0x29);
	case Artifact::BALLISTA:			return AGG::GetICN("OBJNARTI.ICN", 0x2B);
	case Artifact::STEALTH_SHIELD:			return AGG::GetICN("OBJNARTI.ICN", 0x2D);
	case Artifact::DRAGON_SWORD:			return AGG::GetICN("OBJNARTI.ICN", 0x2F);
	case Artifact::POWER_AXE:			return AGG::GetICN("OBJNARTI.ICN", 0x31);
	case Artifact::DIVINE_BREASTPLATE:		return AGG::GetICN("OBJNARTI.ICN", 0x33);
	case Artifact::MINOR_SCROLL:			return AGG::GetICN("OBJNARTI.ICN", 0x35);
	case Artifact::MAJOR_SCROLL:			return AGG::GetICN("OBJNARTI.ICN", 0x37);
	case Artifact::SUPERIOR_SCROLL:			return AGG::GetICN("OBJNARTI.ICN", 0x39);
	case Artifact::FOREMOST_SCROLL:			return AGG::GetICN("OBJNARTI.ICN", 0x3B);
	case Artifact::ENDLESS_SACK_GOLD:		return AGG::GetICN("OBJNARTI.ICN", 0x3D);
	case Artifact::ENDLESS_BAG_GOLD:		return AGG::GetICN("OBJNARTI.ICN", 0x3F);
	case Artifact::ENDLESS_PURSE_GOLD:		return AGG::GetICN("OBJNARTI.ICN", 0x41);
	case Artifact::NOMAD_BOOTS_MOBILITY:		return AGG::GetICN("OBJNARTI.ICN", 0x43);
	case Artifact::TRAVELER_BOOTS_MOBILITY:		return AGG::GetICN("OBJNARTI.ICN", 0x45);
	case Artifact::RABBIT_FOOT:			return AGG::GetICN("OBJNARTI.ICN", 0x47);
	case Artifact::GOLDEN_HORSESHOE:		return AGG::GetICN("OBJNARTI.ICN", 0x49);
	case Artifact::GAMBLER_LUCKY_COIN:		return AGG::GetICN("OBJNARTI.ICN", 0x4B);
	case Artifact::FOUR_LEAF_CLOVER:		return AGG::GetICN("OBJNARTI.ICN", 0x4D);
	case Artifact::TRUE_COMPASS_MOBILITY:		return AGG::GetICN("OBJNARTI.ICN", 0x4F);
	case Artifact::SAILORS_ASTROLABE_MOBILITY:	return AGG::GetICN("OBJNARTI.ICN", 0x51);
	case Artifact::EVIL_EYE:			return AGG::GetICN("OBJNARTI.ICN", 0x53);
	case Artifact::ENCHANTED_HOURGLASS:		return AGG::GetICN("OBJNARTI.ICN", 0x55);
	case Artifact::GOLD_WATCH:			return AGG::GetICN("OBJNARTI.ICN", 0x57);
	case Artifact::SKULLCAP:			return AGG::GetICN("OBJNARTI.ICN", 0x59);
	case Artifact::ICE_CLOAK:			return AGG::GetICN("OBJNARTI.ICN", 0x5B);
	case Artifact::FIRE_CLOAK:			return AGG::GetICN("OBJNARTI.ICN", 0x5D);
	case Artifact::LIGHTNING_HELM:			return AGG::GetICN("OBJNARTI.ICN", 0x5F);
	case Artifact::EVERCOLD_ICICLE:			return AGG::GetICN("OBJNARTI.ICN", 0x61);
	case Artifact::EVERHOT_LAVA_ROCK:		return AGG::GetICN("OBJNARTI.ICN", 0x63);
	case Artifact::LIGHTNING_ROD:			return AGG::GetICN("OBJNARTI.ICN", 0x65);
	case Artifact::SNAKE_RING:			return AGG::GetICN("OBJNARTI.ICN", 0x67);
	case Artifact::ANKH:				return AGG::GetICN("OBJNARTI.ICN", 0x69);
	case Artifact::BOOK_ELEMENTS:			return AGG::GetICN("OBJNARTI.ICN", 0x6B);
	case Artifact::ELEMENTAL_RING:			return AGG::GetICN("OBJNARTI.ICN", 0x6D);
	case Artifact::HOLY_PENDANT:			return AGG::GetICN("OBJNARTI.ICN", 0x6F);
	case Artifact::PENDANT_FREE_WILL:		return AGG::GetICN("OBJNARTI.ICN", 0x71);
	case Artifact::PENDANT_LIFE:			return AGG::GetICN("OBJNARTI.ICN", 0x73);
	case Artifact::SERENITY_PENDANT:		return AGG::GetICN("OBJNARTI.ICN", 0x75);
	case Artifact::SEEING_EYE_PENDANT:		return AGG::GetICN("OBJNARTI.ICN", 0x77);
	case Artifact::KINETIC_PENDANT:			return AGG::GetICN("OBJNARTI.ICN", 0x79);
	case Artifact::PENDANT_DEATH:			return AGG::GetICN("OBJNARTI.ICN", 0x7B);
	case Artifact::WAND_NEGATION:			return AGG::GetICN("OBJNARTI.ICN", 0x7D);
	case Artifact::GOLDEN_BOW:			return AGG::GetICN("OBJNARTI.ICN", 0x7F);
	case Artifact::TELESCOPE:			return AGG::GetICN("OBJNARTI.ICN", 0x81);
	case Artifact::STATESMAN_QUILL:			return AGG::GetICN("OBJNARTI.ICN", 0x83);
	case Artifact::WIZARD_HAT:			return AGG::GetICN("OBJNARTI.ICN", 0x85);
	case Artifact::POWER_RING:			return AGG::GetICN("OBJNARTI.ICN", 0x87);
	case Artifact::AMMO_CART:			return AGG::GetICN("OBJNARTI.ICN", 0x89);
	case Artifact::TAX_LIEN:			return AGG::GetICN("OBJNARTI.ICN", 0x8B);
	case Artifact::HIDEOUS_MASK:			return AGG::GetICN("OBJNARTI.ICN", 0x8D);
	case Artifact::ENDLESS_POUCH_SULFUR:		return AGG::GetICN("OBJNARTI.ICN", 0x8F);
	case Artifact::ENDLESS_VIAL_MERCURY:		return AGG::GetICN("OBJNARTI.ICN", 0x91);
	case Artifact::ENDLESS_POUCH_GEMS:		return AGG::GetICN("OBJNARTI.ICN", 0x93);
	case Artifact::ENDLESS_CORD_WOOD:		return AGG::GetICN("OBJNARTI.ICN", 0x95);
	case Artifact::ENDLESS_CART_ORE:		return AGG::GetICN("OBJNARTI.ICN", 0x97);
	case Artifact::ENDLESS_POUCH_CRYSTAL:		return AGG::GetICN("OBJNARTI.ICN", 0x99);
	case Artifact::SPIKED_HELM:			return AGG::GetICN("OBJNARTI.ICN", 0x9B);
	case Artifact::SPIKED_SHIELD:			return AGG::GetICN("OBJNARTI.ICN", 0x9D);
	case Artifact::WHITE_PEARL:			return AGG::GetICN("OBJNARTI.ICN", 0x9F);
	case Artifact::BLACK_PEARL:			return AGG::GetICN("OBJNARTI.ICN", 0xA1);
	default: Error::Warning("Artifact::GetForMapsSprite: unknown:", artifact); break;
    }

    // null sprite
    return AGG::GetICN("OBJNARTI.ICN", 0x01);
}

