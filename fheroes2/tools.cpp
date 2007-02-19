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

#include "error.h"
#include "tools.h"

/* trim left right space */
void String::Trim(std::string & str)
{
    std::string::iterator iter;

    // left
    iter = str.begin();

    while(iter != str.end() && std::isspace(*iter)) ++iter;
    
    if(iter != str.begin()) str.replace(str.begin(), iter, "");

    // right
    iter = str.end();

    while(iter != str.begin() && std::isspace(*iter)) --iter;

    if(iter != str.end()) str.replace(iter, str.end(), "");
}

/*true is comment */
bool String::Comment(std::string & str)
{
    String::Trim(str);

    switch(str[0]){
    
	case ';':
	case '#':
	    return true;
	
	default:
	    return false;
    }
}

/* convert to lower case */
void String::Lower(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/* convert to upper case */
void String::Upper(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/* int to string */
void String::AddInt(std::string &str, int value)
{
    std::ostringstream stream;
    stream << value;

    str += stream.str();
}

const char * CStr::Difficulty(Maps::difficulty_t difficulty)
{
    switch(difficulty){
        case Maps::EASY:
	    return "Easy";
        case Maps::NORMAL:
	    return "Normal";
        case Maps::HARD:
	    return "Hard";
        case Maps::EXPERT:
	    return "Expert";
        case Maps::IMPOSSIBLE:
	    return "Impossible";
    }
    
    return "";
}

const char * CStr::Color(Kingdom::color_t color)
{
    switch(color){
        case Kingdom::BLUE:
	    return "Blue";
        case Kingdom::GREEN:
	    return "Green";
        case Kingdom::RED:
	    return "Red";
        case Kingdom::YELLOW:
	    return "Yellow";
        case Kingdom::ORANGE:
	    return "Orange";
        case Kingdom::PURPLE:
	    return "Purple";
        default:
	    break;
    }

    return "Gray";
}

const char * CStr::Race(Kingdom::race_t race)
{
    switch(race){
        case Kingdom::KNGT:
	    return "Knight";
        case Kingdom::BARB:
	    return "Barbarian";
        case Kingdom::SORC:
	    return "Sorceress";
        case Kingdom::WRLK:
	    return "Warlock";
        case Kingdom::WZRD:
	    return "Wizard";
        case Kingdom::NECR:
	    return "Necromancer";
        case Kingdom::MULT:
	    return "Multi";
        case Kingdom::RAND:
	    return "Randomize";
	default:
	    break;
    }
    
    return "Bomg";
}

const char * CStr::Object(MP2::object_t object)
{
    switch(object){
        case MP2::OBJ_ZERO:			return "OBJ_ZERO";
        case MP2::OBJN_ALCHEMYTOWER:		return "Alchemy Tower";
        case MP2::OBJ_ALCHEMYTOWER:		return "Alchemy Tower";
        case MP2::OBJN_DAEMONCAVE:		return "Daemon Cave";
        case MP2::OBJ_DAEMONCAVE:		return "Daemon Cave";
        case MP2::OBJN_FAERIERING:		return "Faerie Ring";
        case MP2::OBJ_FAERIERING:		return "Faerie Ring";
        case MP2::OBJN_GRAVEYARD:		return "Graveyard";
        case MP2::OBJ_GRAVEYARD:		return "Graveyard";
        case MP2::OBJN_DRAGONCITY:		return "Dragon City";
        case MP2::OBJ_DRAGONCITY:		return "Dragon City";
        case MP2::OBJN_LIGHTHOUSE:		return "Lighthouse";
        case MP2::OBJ_LIGHTHOUSE:		return "Lighthouse";
        case MP2::OBJN_WATERMILL:		return "Watermill";
        case MP2::OBJ_WATERMILL:		return "Watermill";
        case MP2::OBJN_MINES:			return "Mines";
        case MP2::OBJ_MINES:			return "Mines";
        case MP2::OBJN_OBELISK:			return "Obelisk";
        case MP2::OBJ_OBELISK:			return "Obelisk";
        case MP2::OBJN_OASIS:			return "Oasis";
        case MP2::OBJ_OASIS:			return "Oasis";
        case MP2::OBJ_COAST:			return "Coast";
        case MP2::OBJN_SAWMILL:			return "Sawmill";
        case MP2::OBJ_SAWMILL:			return "Sawmill";
        case MP2::OBJN_ORACLE:			return "Oracle";
        case MP2::OBJ_ORACLE:			return "Oracle";
        case MP2::OBJN_SHIPWRECK:		return "Ship Wreck";
        case MP2::OBJ_SHIPWRECK:		return "Ship Wreck";
        case MP2::OBJN_DESERTTENT:		return "Desert Tent";
        case MP2::OBJ_DESERTTENT:		return "Desert Tent";
        case MP2::OBJN_CASTLE:			return "Castle";
        case MP2::OBJ_CASTLE:			return "Castle";
        case MP2::OBJN_WAGONCAMP:		return "Wagon Camp";
        case MP2::OBJ_WAGONCAMP:		return "Wagon Camp";
        case MP2::OBJN_WINDMILL:		return "Windmill";
        case MP2::OBJ_WINDMILL:			return "Windmill";
        case MP2::OBJN_RNDTOWN:			return "Random Town";
        case MP2::OBJ_RNDTOWN:			return "Random Town";
        case MP2::OBJN_RNDCASTLE:		return "Random Castle";
        case MP2::OBJ_RNDCASTLE:		return "Random Castle";
        case MP2::OBJ_SMALLCRACK:		return "Small Crack";
        case MP2::OBJN_TREECITY:		return "Tree City";
        case MP2::OBJ_TREECITY:			return "Tree City";
        case MP2::OBJN_RUINS:			return "Ruins";
        case MP2::OBJ_RUINS:			return "Ruins";
        case MP2::OBJN_FORT:			return "Fort";
        case MP2::OBJ_FORT:			return "Fort";
        case MP2::OBJN_TRADINGPOST:		return "Trading Post";
        case MP2::OBJ_TRADINGPOST:		return "Trading Post";
        case MP2::OBJN_ABANDONEDMINE:		return "Abandone Mine";
        case MP2::OBJ_ABANDONEDMINE:		return "Abandone Mine";
        case MP2::OBJN_TREEKNOWLEDGE:		return "Tree Knowledge";
        case MP2::OBJ_TREEKNOWLEDGE:		return "Tree Knowledge";
        case MP2::OBJN_DOCTORHUNT:		return "Doctor Hunt";
        case MP2::OBJ_DOCTORHUNT:		return "Doctor Hunt";
        case MP2::OBJN_TEMPLE:			return "Temple";
        case MP2::OBJ_TEMPLE:			return "Temple";
        case MP2::OBJ_HEROES:			return "Heroes";
        case MP2::OBJ_SIGN:			return "Sign";
        case MP2::OBJN_HILLFORT:		return "Hill Fort";
        case MP2::OBJ_HILLFORT:			return "Hill Fort";
        case MP2::OBJN_HALFLINGHOLE:		return "Halfling Hole";
        case MP2::OBJ_HALFLINGHOLE:		return "Halfling Hole";
        case MP2::OBJN_MERCENARYCAMP:		return "Mercenary Camp";
        case MP2::OBJ_MERCENARYCAMP:		return "Mercenary Camp";
        case MP2::OBJN_PIRAMID:			return "Piramid";
        case MP2::OBJ_PIRAMID:			return "Piramid";
        case MP2::OBJN_CITYDEAD:		return "City Dead";
        case MP2::OBJ_CITYDEAD:			return "City Dead";
        case MP2::OBJN_EXCAVATION:		return "Excavation";
        case MP2::OBJ_EXCAVATION:		return "Excavation";
        case MP2::OBJN_SPHINX:			return "Sphinx";
        case MP2::OBJ_SPHINX:			return "Sphinx";
        case MP2::OBJ_OILLAKE:			return "Oil Lake";
        case MP2::OBJN_TROLLBRIDGE:		return "Troll Bridge";
        case MP2::OBJ_TROLLBRIDGE:		return "Troll Bridge";
        case MP2::OBJN_WITCHHUNT:		return "Witch's Hunt";
        case MP2::OBJ_WITCHHUNT:		return "Witch's Hunt";
        case MP2::OBJN_XANADU:			return "Xanadu";
        case MP2::OBJ_XANADU:			return "Xanadu";
        case MP2::OBJN_CAVE:			return "Cave";
        case MP2::OBJ_CAVE:			return "Cave";
        case MP2::OBJN_MAGELLANMAPS:		return "Magellan Maps";
        case MP2::OBJ_MAGELLANMAPS:		return "Magellan Maps";
        case MP2::OBJN_DERELICTSHIP:		return "Derelict Ship";
        case MP2::OBJ_DERELICTSHIP:		return "Derelict Ship";
        case MP2::OBJN_OBSERVATIONTOWER:	return "Observation Tower";
        case MP2::OBJ_OBSERVATIONTOWER:		return "Observation Tower";
        case MP2::OBJN_FREEMANFOUNDRY:		return "Freeman Foundry";
        case MP2::OBJ_FREEMANFOUNDRY:		return "Freeman Foundry";
        case MP2::OBJ_TREES:			return "Trees";
        case MP2::OBJ_FIRTREES:			return "Fir Trees";
        case MP2::OBJ_MOUNTS:			return "Mountains";
        case MP2::OBJ_VOLCANO:			return "Volcano";
        case MP2::OBJ_STONES:			return "Stones";
        case MP2::OBJ_VEGETATION1:		return "Vegetation";
        case MP2::OBJ_WATERLAKE:		return "Water Lake";
        case MP2::OBJ_VEGETATION2:		return "Vegetation";
        case MP2::OBJ_BIGCRACK:			return "Big Crack";
        case MP2::OBJ_LAVALAKE:			return "Lava Lake";
        case MP2::OBJ_VEGETATION3:		return "Vegetation";
        case MP2::OBJ_BUOY:			return "Buoy";
        case MP2::OBJ_SKELETON:			return "Skeleton";
        case MP2::OBJ_TREASURECHEST:		return "Treasure Chest";
        case MP2::OBJ_CAMPFIRE:			return "Camp Fire";
        case MP2::OBJ_FOUNTAIN:			return "Fountain";
        case MP2::OBJ_GAZEBO:			return "Gazebo";
        case MP2::OBJ_ANCIENTLAMP:		return "Ancient Lamp";
        case MP2::OBJ_ARCHERHOUSE:		return "Archer House";
        case MP2::OBJ_GOBLINHUNT:		return "Goblin Hunt";
        case MP2::OBJ_DWARFCOTT:		return "Dwarf Cott";
        case MP2::OBJ_PEASANTHUNT:		return "Peasant Hunt";
        case MP2::OBJ_PEASANTHUNT2:		return "Peasant Hunt";
        case MP2::OBJ_MONSTER:			return "Monster";
        case MP2::OBJ_RESOURCE:			return "Resource";
        case MP2::OBJ_STONELITHS:		return "Stonelights";
        case MP2::OBJ_WHIRLPOOL:		return "Whirlpool";
        case MP2::OBJ_ARTIFACT:			return "Artifact";
        case MP2::OBJ_BOAT:			return "Boat";
        case MP2::OBJ_RNDARTIFACT:		return "Random Artifact";
        case MP2::OBJ_RNDRESOURCE:		return "Random Resource";
        case MP2::OBJ_RNDMONSTER1:           	return "OBJ_RNDMONSTER1";
        case MP2::OBJ_RNDMONSTER2:           	return "OBJ_RNDMONSTER2";
        case MP2::OBJ_RNDMONSTER3:           	return "OBJ_RNDMONSTER3";
        case MP2::OBJ_RNDMONSTER4:           	return "OBJ_RNDMONSTER4";
        case MP2::OBJ_WATCHTOWER:            	return "Watch Tower";
        case MP2::OBJ_TREEHOUSE:             	return "Tree House";
        case MP2::OBJ_STANDINGSTONES:        	return "Standing Stones";
        case MP2::OBJ_EVENT:                 	return "OBJ_EVENT";
        case MP2::OBJ_RNDMONSTER:            	return "OBJ_RNDMONSTER";
        case MP2::OBJ_RNDULTIMATEARTIFACT:   	return "OBJ_RNDULTIMATEARTIFACT";
        case MP2::OBJ_IDOL:                  	return "Idol";
        case MP2::OBJ_SHRINE1:               	return "Shrine of Level 1";
        case MP2::OBJ_SHRINE2:               	return "Shrine of Level 2";
        case MP2::OBJ_SHRINE3:               	return "Shrine of Level 3";
        case MP2::OBJ_WAGON:                 	return "Wagon";
        case MP2::OBJ_ARTESIANSPRING:        	return "Artesian Spring";
        case MP2::OBJ_CRAKEDLAKE:            	return "Cracked Lake";
        case MP2::OBJ_LEANTO:                	return "Leanto";
        case MP2::OBJ_FLOTSAM:               	return "Flotsam";
        case MP2::OBJ_SHIPWRECKSURVIROR:     	return "Shipwreck Surviror";
        case MP2::OBJ_BOTTLE:                	return "Bottle";
        case MP2::OBJ_MAGICWELL:             	return "Magic Well";
        case MP2::OBJ_MAGICGARDEN:           	return "Magic Garden";
        case MP2::OBJ_RNDARTIFACT1:          	return "OBJ_RNDARTIFACT1";
        case MP2::OBJ_RNDARTIFACT2:          	return "OBJ_RNDARTIFACT2";
        case MP2::OBJ_RNDARTIFACT3:          	return "OBJ_RNDARTIFACT3";
	
	default:
	    Error::Warning("MP2::Object: unknown, ", object);
	    break;
    }
    
    return "";
}

/*
    case FIREBALL:string = "Fireball";break;
    case FIREBLAST:string = "Fireblast";break;
    case LIGHTNINGBOLT:string = "Lightning Bolt";break;
    case CHAINLIGHTNING:string = "Chain Lightning";break;
    case TELEPORT:string = "Teleport";break;
    case CURE:string = "Cure";break;
    case MASSCURE:string = "Mass Cure";break;
    case RESURRECT:string = "Resurrect";break;
    case RESURRECTTRUE:string = "Resurrect True";break;
    case HASTE:string = "Haste";break;
    case MASSHASTE:string = "Mass Haste";break;
    case SPELLSLOW:string = "Slow";break;
    case MASSSLOW:string = "Mass Slow";break;
    case BLIND:string = "Blind";break;
    case BLESS:string = "Bless";break;
    case MASSBLESS:string = "Mass Bless";break;
    case STONESKIN:string = "Stoneskin";break;
    case STEELSKIN:string = "Steelskin";break;
    case CURSE:string = "Curse";break;
    case MASSCURSE:string = "Mass Curse";break;
    case HOLYWORD:string = "Holy Word";break;
    case HOLYSHOUT:string = "Holy Shout";break;
    case ANTIMAGIC:string = "Anti-Magic";break;
    case DISPELMAGIC:string = "Dispel Magic";break;
    case MASSDISPEL:string = "Mass Dispel";break;
    case MAGICARROW:string = "Magic Arrow";break;
    case BERZERKER:string = "Berzerker";break;
    case ARMAGEDDON:string = "Armageddon";break;
    case ELEMENTALSTORM:string = "Elemental Storm";break;
    case METEORSHOWER:string = "Meteor Shower";break;
    case PARALYZE:string = "Paralyze";break;
    case HYPNOTIZE:string = "Hypnotize";break;
    case COLDRAY:string = "Cold Ray";break;
    case COLDRING:string = "Cold Ring";break;
    case DISRUPTINGRAY:string = "Disrupting Ray";break;
    case DEATHRIPPLE:string = "Death Ripple";break;
    case DEATHWAVE:string = "Death Wave";break;
    case DRAGONSLAYER:string = "Dragon Slayer";break;
    case BLOODLUST:string = "Blood Lust";break;
    case ANIMATEDEAD:string = "Animate Dead";break;
    case MIRRORIMAGE:string = "Mirror Image";break;
    case SHIELD:string = "Shield";break;
    case MASSSHIELD:string = "Mass Shield";break;
    case EARTHSUMMON:string = "Summon Earth El.";break;
    case AIRSUMMON:string = "Summon Air El.";break;
    case FIRESUMMON:string = "Summon Fire El.";break;
    case WATERSUMMON:string = "Summon Water El.";break;
    case EARTHQUAKE:string = "Earthquake";break;
    case VIEWMINES:string = "View Mines";break;
    case VIEWRESOURCES:string = "View Resources";break;
    case VIEWARTIFACTS:string = "View Artifacts";break;
    case VIEWTOWNS:string = "View Towns";break;
    case VIEWHEROES:string = "View Heroes";break;
    case VIEWALL:string = "View All";break;
    case IDENTIFYHERO:string = "Identify Hero";break;
    case SUMMONBOAT:string = "Summon Boat";break;
    case DIMENSIONDOOR:string = "Dimension Door";break;
    case TOWNGATE:string = "Town Gate";break;
    case TOWNPORTAL:string = "Town Portal";break;
    case VISIONS:string = "Visions";break;
    case HAUNT:string = "Haunt";break;
    case EARTHGUARDIAN:string = "Earth Guardian";break;
    case AIRGUARDIAN:string = "Air Guardian";break;
    case FIREGUARDIAN:string = "Fire Guardian";break;
    case WATERGUARDIAN:string = "Water Guardian";break;
*/

/*
    case FIREBALL:string = "Causes a giant fireball to strike the selected area, damaging all nearby creatures.";break;
    case FIREBLAST:string = "An improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one.";break;
    case LIGHTNINGBOLT:string = "Causes a bolt of electrical energy to strike the selected creature.";break;
    case CHAINLIGHTNING:string = "Causes a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!";break;
    case TELEPORT:string = "Teleports the creature you select to any open position on the battlefield.";break;
    case CURE:string = "Removes all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power.";break;
    case MASSCURE:string = "Removes all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature.";break;
    case RESURRECT:string = "Resurrects creatures from a damaged or dead unit until end of combat.";break;
    case RESURRECTTRUE:string = "Resurrects creatures from a damaged or dead unit permanently.";break;
    case HASTE:string = "Increases the speed of any creature by two.";break;
    case MASSHASTE:string = "Increases the speed of all of your creatures by two.";break;
    case SPELLSLOW:string = "Slows target to half movement rate.";break;
    case MASSSLOW:string = "Slows all enemies to half movement rate.";break;
    case BLIND:string = "Clouds the affected creatures' eyes, preventing them from moving.";break;
    case BLESS:string = "Causes the selected creatures to inflict maximum damage.";break;
    case MASSBLESS:string = "Causes all of your units to inflict maximum damage.";break;
    case STONESKIN:string = "Magically increases the defense skill of the selected creatures.";break;
    case STEELSKIN:string = "Increases the defense skill of the targeted creatures.  This is an improved version of Stoneskin.";break;
    case CURSE:string = "Causes the selected creatures to inflict minimum damage.";break;
    case MASSCURSE:string = "Causes all enemy troops to inflict minimum damage.";break;
    case HOLYWORD:string = "Damages all undead in the battle.";break;
    case HOLYSHOUT:string = "Damages all undead in the battle.  This is an improved version of Holy Word.";break;
    case ANTIMAGIC:string = "Prevents harmful magic against the selected creatures.";break;
    case DISPELMAGIC:string = "Removes all magic spells from a single target.";break;
    case MASSDISPEL:string = "Removes all magic spells from all creatures.";break;
    case MAGICARROW:string = "Causes a magic arrow to strike the selected target.";break;
    case BERZERKER:string = "Causes a creature to attack its nearest neighbor.";break;
    case ARMAGEDDON:string = "Holy terror strikes the battlefield, causing severe damage to all creatures.";break;
    case ELEMENTALSTORM:string = "Magical elements pour down on the battlefield, damaging all creatures.";break;
    case METEORSHOWER:string = "A rain of rocks strikes an area of the battlefield, damaging all nearby creatures.";break;
    case PARALYZE:string = "The targeted creatures are paralyzed, unable to move or retaliate.";break;
    case HYPNOTIZE:string = "Brings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power.";break;
    case COLDRAY:string = "Drains body heat from a single enemy unit.";break;
    case COLDRING:string = "Drains body heat from all units surrounding the center point, but not including the center point.";break;
    case DISRUPTINGRAY:string = "Reduces the defense rating of an enemy unit by three.";break;
    case DEATHRIPPLE:string = "Damages all living (non-undead) units in the battle.";break;
    case DEATHWAVE:string = "Damages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple.";break;
    case DRAGONSLAYER:string = "Greatly increases a unit's attack skill vs. Dragons.";break;
    case BLOODLUST:string = "Increases a unit's attack skill.";break;
    case ANIMATEDEAD:string = "'Resurrects' creatures from a damaged or dead undead unit permanently.";break;
    case MIRRORIMAGE:string = "Creates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage.";break;
    case SHIELD:string = "Halves damage received from ranged attacks for a single unit.";break;
    case MASSSHIELD:string = "Halves damage received from ranged attacks for all of your units.";break;
    case EARTHSUMMON:string = "Summons Earth Elementals to fight for your army.";break;
    case AIRSUMMON:string = "Summons Air Elementals to fight for your army.";break;
    case FIRESUMMON:string = "Summons Fire Elementals to fight for your army.";break;
    case WATERSUMMON:string = "Summons Water Elementals to fight for your army.";break;
    case EARTHQUAKE:string = "Damages castle walls.";break;
    case VIEWMINES:string = "Causes all mines across the land to become visible.";break;
    case VIEWRESOURCES:string = "Causes all resources across the land to become visible.";break;
    case VIEWARTIFACTS:string = "Causes all artifacts across the land to become visible.";break;
    case VIEWTOWNS:string = "Causes all towns and castles across the land to become visible.";break;
    case VIEWHEROES:string = "Causes all Heroes across the land to become visible.";break;
    case VIEWALL:string = "Causes the entire land to become visible.";break;
    case IDENTIFYHERO:string = "Allows the caster to view detailed information on enemy Heroes.";break;
    case SUMMONBOAT:string = "Summons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy.";break;
    case DIMENSIONDOOR:string = "Allows the caster to magically transport to a nearby location.";break;
    case TOWNGATE:string = "Returns the caster to any town or castle currently owned.";break;
    case TOWNPORTAL:string = "Returns the hero to the town or castle of choice, provided it is controlled by you.";break;
    case VISIONS:string = "Visions predicts the likely outcome of an encounter with a neutral army camp.";break;
    case HAUNT:string = "Haunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)";break;
    case EARTHGUARDIAN:string = "Sets Earth Elementals to guard a mine against enemy armies.";break;
    case AIRGUARDIAN:string = "Sets Air Elementals to guard a mine against enemy armies.";break;
    case FIREGUARDIAN:string = "Sets Fire Elementals to guard a mine against enemy armies.";break;
    case WATERGUARDIAN:string = "Sets Water Elementals to guard a mine against enemy armies.";break;
*/
