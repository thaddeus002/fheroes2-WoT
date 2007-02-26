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
#include "monster.h"

const std::string & Monster::String(monster_t monster)
{
    static const std::string strPeasant("Peasant");
    static const std::string strArcher("Archer");
    static const std::string strRanger("Ranger");
    static const std::string strPikeman("Pikeman");
    static const std::string strVeteranPikeman("Veteran Pikeman");
    static const std::string strSwordsman("Swordsman");
    static const std::string strMasterSwordsman("Master Swordman");
    static const std::string strCavalry("Cavalry");
    static const std::string strChampion("Champion");
    static const std::string strPaladin("Paladin");
    static const std::string strCrusader("Crusader");
    static const std::string strGoblin("Goblin");
    static const std::string strOrc("Orc");
    static const std::string strChiefOrc("Chief Orc");
    static const std::string strWolf("Wolf");
    static const std::string strOgre("Ogre");
    static const std::string strLordOgre("Lord Ogre");
    static const std::string strTroll("Troll");
    static const std::string strWarTroll("War Troll");
    static const std::string strCyclops("Cyclops");
    static const std::string strSprite("Sprite");
    static const std::string strDwarf("Dwarf");
    static const std::string strBattleDwarf("Battle Dwarf");
    static const std::string strElf("Elf");
    static const std::string strGrandElf("GrandElf");
    static const std::string strDruid("Druid");
    static const std::string strGreaterDruid("Greater Druid");
    static const std::string strUnicorn("Unicorn");
    static const std::string strPhoenix("Phoenix");
    static const std::string strCentaur("Centaur");
    static const std::string strGargoyle("Gargoyle");
    static const std::string strGriffin("Griffin");
    static const std::string strMinotaur("Minotaur");
    static const std::string strKnightMinotaur("Knight Minotaur");
    static const std::string strHidra("Hidra");
    static const std::string strGreenDragon("Green Dragon");
    static const std::string strRedDragon("Red Dragon");
    static const std::string strBlackDragon("Black Dragon");
    static const std::string strHalfling("Halfling");
    static const std::string strBoar("Boar");
    static const std::string strIronGolem("Iron Golem");
    static const std::string strSteelGolem("Steel Golem");
    static const std::string strRoc("Roc");
    static const std::string strMage("Mage");
    static const std::string strArchmage("Archmage");
    static const std::string strGiant("Giant");
    static const std::string strTitan("Titan");
    static const std::string strSkeleton("Skeleton");
    static const std::string strZombie("Zombie");
    static const std::string strMutantZombie("Mutant Zombie");
    static const std::string strMummy("Mummy");
    static const std::string strRoyalMummy("Royal Mummy");
    static const std::string strVampire("Vampire");
    static const std::string strLordVampire("Lord Vampire");
    static const std::string strLich("Lich");
    static const std::string strPowerLich("Power Lich");
    static const std::string strBoneDragon("Bone Dragon");
    static const std::string strRogue("Rogue");
    static const std::string strNomad("Nomad");
    static const std::string strGhost("Ghost");
    static const std::string strGenie("Genie");
    static const std::string strMedusa("Medusa");
    static const std::string strEarthElement("Earth Element");
    static const std::string strAirElement("Air Element");
    static const std::string strFireElement("Fire Element");
    static const std::string strWaterElement("Water Element");
    static const std::string unknown("unknown");

    switch(monster){

	// knight
        case Monster::PEASANT:			return strPeasant;
        case Monster::ARCHER:			return strArcher;
        case Monster::RANGER:			return strRanger;
        case Monster::PIKEMAN:			return strPikeman;
        case Monster::VETERAN_PIKEMAN:		return strVeteranPikeman;
        case Monster::SWORDSMAN:		return strSwordsman;
	case Monster::MASTER_SWORDSMAN:		return strMasterSwordsman;
	case Monster::CAVALRY:			return strCavalry;
	case Monster::CHAMPION:			return strChampion;
	case Monster::PALADIN:			return strPaladin;
	case Monster::CRUSADER:			return strCrusader;
	
	// barbarian
	case Monster::GOBLIN:			return strGoblin;
	case Monster::ORC:			return strOrc;
	case Monster::CHIEF_ORC:		return strChiefOrc;
	case Monster::WOLF:			return strWolf;
	case Monster::OGRE:			return strOgre;
	case Monster::LORD_OGRE:		return strLordOgre;
	case Monster::TROLL:			return strTroll;
	case Monster::WAR_TROLL:		return strWarTroll;
	case Monster::CYCLOPS:			return strCyclops;
	
	// sorceress
	case Monster::SPRITE:			return strSprite;
	case Monster::DWARF:			return strDwarf;
	case Monster::BATTLE_DWARF:		return strBattleDwarf;
	case Monster::ELF:			return strElf;
	case Monster::GRAND_ELF:		return strGrandElf;
	case Monster::DRUID:			return strDruid;
	case Monster::GREATER_DRUID:		return strGreaterDruid;
	case Monster::UNICORN:			return strUnicorn;
	case Monster::PHOENIX:			return strPhoenix;
	
	// warlock
	case Monster::CENTAUR:			return strCentaur;
	case Monster::GARGOYLE:			return strGargoyle;
	case Monster::GRIFFIN:			return strGriffin;
	case Monster::MINOTAUR:			return strMinotaur;
	case Monster::KNIGHT_MINOTAUR:		return strKnightMinotaur;
	case Monster::HIDRA:			return strHidra;
	case Monster::GREEN_DRAGON:		return strGreenDragon;
	case Monster::RED_DRAGON:		return strRedDragon;
	case Monster::BLACK_DRAGON:		return strBlackDragon;
	
	// wizard
	case Monster::HALFLING:			return strHalfling;
	case Monster::BOAR:			return strBoar;
	case Monster::IRON_GOLEM:		return strIronGolem;
	case Monster::STEEL_GOLEM:		return strSteelGolem;
	case Monster::ROC:			return strRoc;
	case Monster::MAGE:			return strMage;
	case Monster::ARCHMAGE:			return strArchmage;
	case Monster::GIANT:			return strGiant;
	case Monster::TITAN:			return strTitan;
	
	// necromancer
	case Monster::SKELETON:			return strSkeleton;
	case Monster::ZOMBIE:			return strZombie;
	case Monster::MUTANT_ZOMBIE:		return strMutantZombie;
	case Monster::MUMMY:			return strMummy;
	case Monster::ROYAL_MUMMY:		return strRoyalMummy;
	case Monster::VAMPIRE:			return strVampire;
	case Monster::LORD_VAMPIRE:		return strLordVampire;
	case Monster::LICH:			return strLich;
	case Monster::POWER_LICH:		return strPowerLich;
	case Monster::BONE_DRAGON:		return strBoneDragon;

	// bomg
	case Monster::ROGUE:			return strRogue;
	case Monster::NOMAD:			return strNomad;
	case Monster::GHOST:			return strGhost;
	case Monster::GENIE:			return strGenie;
	case Monster::MEDUSA:			return strMedusa;
	case Monster::EARTH_ELEMENT:		return strEarthElement;
	case Monster::AIR_ELEMENT:		return strAirElement;
	case Monster::FIRE_ELEMENT:		return strFireElement;
	case Monster::WATER_ELEMENT:		return strWaterElement;
	
	default: Error::Warning("Monster::String: unknown: ", monster); break;
    }

    return unknown;
}
