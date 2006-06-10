/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "artifact.h"

static char *descriptionArtifact[E_ARTIFACT_COUNT];

const char * GetDescriptionArtifact(E_ARTIFACT artifact){

    return descriptionArtifact[artifact];
}

void    InitArtifact(void){

	descriptionArtifact[ULTIMATE_BOOK] = "The Ultimate Book of Knowledge increases your knowledge by 12.";

	descriptionArtifact[ULTIMATE_SWORD] = "The Ultimate Sword of Dominion increases your attack skill by 12.";

	descriptionArtifact[ULTIMATE_CLOAK] = "The Ultimate Cloak of Protection increases your defense skill by 12.";

	descriptionArtifact[ULTIMATE_WAND] = "The Ultimate Wand of Magic increases your spell power by 12.";

	descriptionArtifact[ULTIMATE_SHIELD] = "The Ultimate Shield increases your attack and defense skills by 6 each.";

	descriptionArtifact[ULTIMATE_STAFF] = "The Ultimate Staff increases your spell power and knowledge by 6 each.";

	descriptionArtifact[ULTIMATE_CROWN] = "The Ultimate Crown increases each of your basic skills by 4 points.";

	descriptionArtifact[GOLDEN_GOOSE] = "The Golden Goose brings in an income of 10,000 gold per turn.";

	descriptionArtifact[ARCANE_NECKLACE] = "The Arcane Necklace of Magic increases your spell power by 4.";

	descriptionArtifact[CASTER_BRACELET] = "The Caster's Bracelet of Magic increases your spell power by 2.";

	descriptionArtifact[MAGE_RING] = "The Mage's Ring of Power increases your spell power by 2.";

	descriptionArtifact[WITCHES_BROACH] = "The Witch's Broach of Magic increases your spell power by 3.";

	descriptionArtifact[MEDAL_VALOR] = "The Medal of Valor increases your morale.";

	descriptionArtifact[MEDAL_COURAGE] = "The Medal of Courage increases your morale.";

	descriptionArtifact[MEDAL_HONOR] = "The Medal of Honor increases your morale.";

	descriptionArtifact[MEDAL_DISTINCTION] = "The Medal of Distinction increases your morale.";

	descriptionArtifact[FIZBIN_MISFORTUNE] = "The Fizbin of Misfortune greatly decreases your morale.";

	descriptionArtifact[THUNDER_MACE] = "The Thunder Mace of Dominion increases your attack skill by 1.";

	descriptionArtifact[ARMORED_GAUNTLETS] = "The Armored Gauntlets of Protection increase your defense skill by 1.";

	descriptionArtifact[DEFENDER_HELM] = "The Defender Helm of Protection increases your defense skill by 1.";

	descriptionArtifact[GIANT_FLAIL] = "The Giant Flail of Dominion increases your attack skill by 1.";

	descriptionArtifact[BALLISTA] = "The Ballista of Quickness lets your catapult fire twice per combat round.";

	descriptionArtifact[STEALTH_SHIELD] = "The Stealth Shield of Protection increases your defense skill by 2.";

	descriptionArtifact[DRAGON_SWORD] = "The Dragon Sword of Dominion increases your attack skill by 3.";

	descriptionArtifact[POWER_AXE]	= "The Power Axe of Dominion increases your attack skill by 2.";

	descriptionArtifact[DIVINE_BREASTPLATE] = "The Divine Breastplate of Protection increases your defense skill by 3.";

	descriptionArtifact[MINOR_SCROLL] = "The Minor Scroll of Knowledge increases your knowledge by 2.";

	descriptionArtifact[MAJOR_SCROLL] = "The Major Scroll of Knowledge increases your knowledge by 3.";

	descriptionArtifact[SUPERIOR_SCROLL] = "The Superior Scroll of Knowledge increases your knowledge by 4.";

	descriptionArtifact[FOREMOST_SCROLL] = "The Foremost Scroll of Knowledge increases your knowledge by 5.";

	descriptionArtifact[ENDLESS_SACK_GOLD] = "The Endless Sack of Gold provides you with 1000 gold per day.";

	descriptionArtifact[ENDLESS_BAG_GOLD] = "The Endless Bag of Gold provides you with 750 gold per day.";

	descriptionArtifact[ENDLESS_PURSE_GOLD] = "The Endless Purse of Gold provides you with 500 gold per day.";

	descriptionArtifact[RABBIT_FOOT] = "The Lucky Rabbit's Foot increases your luck in combat.";

	descriptionArtifact[GOLDEN_HORSESHOE] = "The Golden Horseshoe increases your luck in combat.";

	descriptionArtifact[GAMBLER_LUCKY_COIN] = "The Gambler's Lucky Coin increases your luck in combat.";

	descriptionArtifact[FOUR_LEAF_CLOVER] = "The Four_Leaf Clover increases your luck in combat.";

	descriptionArtifact[EVIL_EYE] = "The Evil Eye reduces the casting cost of curse spells by half.";

	descriptionArtifact[ENCHANTED_HOURGLASS] = "The Enchanted Hourglass extends the duration of all your spells by 2 turns.";

	descriptionArtifact[GOLD_WATCH] = "The Gold Watch doubles the effectiveness of your hypnotize spells.";

	descriptionArtifact[SKULLCAP] = "The Skullcap halves the casting cost of all mind influencing spells.";

	descriptionArtifact[ICE_CLOAK] = "The Ice Cloak halves all damage your troops take from cold spells.";

	descriptionArtifact[FIRE_CLOAK] = "The Fire Cloak halves all damage your troops take from fire spells.";

	descriptionArtifact[LIGHTNING_HELM] = "The Lightning Helm halves all damage your troops take from lightning spells.";

	descriptionArtifact[EVERCOLD_ICICLE] = "The Evercold Icicle causes your cold spells to do 50% more damage to enemy troops.";

	descriptionArtifact[EVERHOT_LAVA_ROCK] = "The Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops.";

	descriptionArtifact[LIGHTNING_ROD] = "The Lightning Rod causes your lightning spells to do 50% more damage to enemy troops.";

	descriptionArtifact[SNAKE_RING] = "The Snake Ring halves the casting cost of all your bless spells.";

	descriptionArtifact[ANKH] = "The Ankh doubles the effectiveness of all your resurrect and animate spells.";

	descriptionArtifact[BOOK_ELEMENTS] = "The Book of Elements doubles the effectiveness of all your summoning spells.";

	descriptionArtifact[ELEMENTAL_RING] = "The Elemental Ring halves the casting cost of all summoning spells.";

	descriptionArtifact[HOLY_PENDANT] = "The Holy Pendant makes all your troops immune to curse spells.";

	descriptionArtifact[PENDANT_FREE_WILL] = "The Pendant of Free Will makes all your troops immune to hypnotize spells.";

	descriptionArtifact[PENDANT_LIFE] = "The Pendant of Life makes all your troops immune to death spells.";

	descriptionArtifact[SERENITY_PENDANT] = "The Serenity Pendant makes all your troops immune to berserk spells.";

	descriptionArtifact[SEEING_EYE_PENDANT] = "The Seeing_eye Pendant makes all your troops immune to blindness spells.";

	descriptionArtifact[KINETIC_PENDANT] = "The Kinetic Pendant makes all your troops immune to paralyze spells.";

	descriptionArtifact[PENDANT_DEATH] = "The Pendant of Death makes all your troops immune to holy spells.";

	descriptionArtifact[WAND_NEGATION] = "The Wand of Negation protects your troops from the Dispel Magic spell.";

	descriptionArtifact[GOLDEN_BOW] = "The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)";

	descriptionArtifact[TELESCOPE] = "The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.";

	descriptionArtifact[STATESMAN_QUILL] = "The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.";

	descriptionArtifact[WIZARD_HAT] = "The Wizard's Hat increases the duration of your spells by 10 turns!";

	descriptionArtifact[POWER_RING] = "The Power Ring returns 2 extra power points/turn to your hero.";

	descriptionArtifact[AMMO_CART] = "The Ammo Cart provides endless ammunition for all your troops that shoot.";

	descriptionArtifact[TAX_LIEN] = "The Tax Lien costs you 250 gold pieces/turn.";

	descriptionArtifact[HIDEOUS_MASK] = "The Hideous Mask prevents all 'wandering' armies from joining your hero.";

	descriptionArtifact[ENDLESS_POUCH_SULFUR] = "The Endless Pouch of Sulfur provides 1 unit of sulfur per day.";

	descriptionArtifact[ENDLESS_VIAL_MERCURY] = "The Endless Vial of Mercury provides 1 unit of mercury per day.";

	descriptionArtifact[ENDLESS_POUCH_GEMS] = "The Endless Pouch of Gems provides 1 unit of gems per day.";

	descriptionArtifact[ENDLESS_CORD_WOOD] = "The Endless Cord of Wood provides 1 unit of wood per day.";

	descriptionArtifact[ENDLESS_CART_ORE] = "The Endless Cart of Ore provides 1 unit of ore per day.";

	descriptionArtifact[ENDLESS_POUCH_CRYSTAL] = "The Endless Pouch of Crystal provides 1 unit of crystal/day.";

	descriptionArtifact[SPIKED_HELM] = "The Spiked Helm increases your attack and defense skills by 1 each.";

	descriptionArtifact[SPIKED_SHIELD] = "The Spiked Shield increases your attack and defense skills by 2 each.";

	descriptionArtifact[WHITE_PEARL] = "The White Pearl increases your spell power and knowledge by 1 each.";

	descriptionArtifact[BLACK_PEARL] = "The Black Pearl increases your spell power and knowledge by 2 each.";

	descriptionArtifact[NOMAD_BOOTS_MOBILITY] = "The Nomad Boots of Mobility increase your movement on land.";

	descriptionArtifact[TRAVELER_BOOTS_MOBILITY] = "The Traveler's Boots of Mobility increase your movement on land.";

	descriptionArtifact[TRUE_COMPASS_MOBILITY] = "The True Compass of Mobility increases your movement on land and sea.";

	descriptionArtifact[SAILORS_ASTROLABE_MOBILITY] = "The Sailors' Astrolabe of Mobility increases your movement on sea.";

}
