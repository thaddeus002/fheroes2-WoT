/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#ifndef H2MONSTER_H
#define H2MONSTER_H

#include <string>
#include "skill.h"
#include "icn.h"
#include "m82.h"
#include "gamedefs.h"

namespace Maps { class Tiles; };

class Monster : public Skill::Primary
{
public:
    enum level_t
    {
	LEVEL0,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4
    };

    enum monster_t
    {
	UNKNOWN,

	PEASANT,
	ARCHER,
	RANGER,
	PIKEMAN,
	VETERAN_PIKEMAN,
	SWORDSMAN,
	MASTER_SWORDSMAN,
	CAVALRY,
	CHAMPION,
	PALADIN,
	CRUSADER,
	GOBLIN,
	ORC,
	CHIEF_ORC,
	WOLF,
	OGRE,
	LORD_OGRE,
	TROLL,
	WAR_TROLL,
	CYCLOPS,
	SPRITE,
	DWARF,
	BATTLE_DWARF,
	ELF,
	GRAND_ELF,
	DRUID,
	GREATER_DRUID,
	UNICORN,
	PHOENIX,
	CENTAUR,
	GARGOYLE,
	GRIFFIN,
	MINOTAUR,
	KNIGHT_MINOTAUR,
	HYDRA,
	GREEN_DRAGON,
	RED_DRAGON,
	BLACK_DRAGON,
	HALFLING,
	BOAR,
	IRON_GOLEM,
	STEEL_GOLEM,
	ROC,
	MAGE,
	ARCHMAGE,
	GIANT,
	TITAN,
	SKELETON,
	ZOMBIE,
	MUTANT_ZOMBIE,
	MUMMY,
	ROYAL_MUMMY,
	VAMPIRE,
	LORD_VAMPIRE,
	LICH,
	POWER_LICH,
	BONE_DRAGON,

	ROGUE,
	NOMAD,
	GHOST,
	GENIE,
	MEDUSA,
	EARTH_ELEMENT,
	AIR_ELEMENT,
	FIRE_ELEMENT,
	WATER_ELEMENT,

	MONSTER_RND1,
	MONSTER_RND2,
	MONSTER_RND3,
	MONSTER_RND4,
	MONSTER_RND,
    };

    struct animattack_t
    {
	u8 attprep_start;
	u8 attprep_count;
	u8 attack1_start;
	u8 attack1_count;
	u8 attack2_start;
	u8 attack2_count;
	u8 attack3_start;
	u8 attack3_count;
    };

    struct anim_t
    {
	u8 walk_start;
	u8 walk_count;
	u8 idle_start;
	u8 idle_count;
	u8 pain_start;
	u8 pain_count;
	u8 die_start;
	u8 die_count;
	animattack_t a;
	animattack_t ar;
    };

    enum animstate_t
    { 
	AS_NONE		= 0,
	AS_IDLE		= 1,
	AS_WALK		= 2,
	AS_ATT1		= 3,
	AS_ATT2		= 4,
	AS_ATT3		= 5,
	AS_PAIN		= 6,
	AS_DIE		= 7,
	AS_ATTPREP	= 16,
	AS_ATT1P	= 16+3,
	AS_ATT2P	= 16+4,
	AS_ATT3P	= 16+5
    };

    Monster();
    Monster(monster_t);
    Monster(const Maps::Tiles &);
    Monster(u8, u32);

    bool operator== (monster_t) const;
    bool operator!= (monster_t) const;
    monster_t operator() (void) const;

    monster_t GetID(void) const;

    void Set(monster_t);
    void Upgrade(void);

    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;
    Morale::morale_t GetMorale(void) const;
    Luck::luck_t GetLuck(void) const;
    Race::race_t GetRace(void) const;
    Color::color_t GetColor(void) const;
    u8 GetType(void) const;
    u16 GetSpellPoints(void) const;
    Spell::Book* GetSpellBook(void);

    u8  GetDamageMin(void) const;
    u8  GetDamageMax(void) const;
    u8  GetShots(void) const;
    u16 GetHitPoints(void) const;
    u8  GetSpeed(void) const;
    u8  GetGrown(void) const;
    u8  GetLevel(void) const;
    u16 GetRNDSize(void) const;
    u8  GetSpriteIndex(void) const;

    const std::string & GetName(void) const;
    const std::string & GetMultiName(void) const;

    bool isFly(void) const;
    bool isWide(void) const;
    bool isArchers(void) const;
    bool isAllowUpgrade(void) const;
    bool isTwiceAttack(void) const;

    ICN::icn_t ICNFile(void) const;
    ICN::icn_t ICNMonh(void) const;
    ICN::icn_t ICNMiss(void) const;

    M82::m82_t M82Attk(void) const;
    M82::m82_t M82Expl(void) const;
    M82::m82_t M82Kill(void) const;
    M82::m82_t M82Move(void) const;
    M82::m82_t M82Shot(void) const;
    M82::m82_t M82Wnce(void) const;

    const anim_t & Animation(void) const;
    void GetAnimFrames(u8 anim, u8 & start, u8 & length, bool attranged = false);

    static const std::string & String(Monster &);
    static monster_t Upgrade(Monster &);
    static u16 GetRNDSize(Monster &);
    static u8  GetLevel(Monster &);
    static u32 GetDwelling(Monster &);

    static const std::string & String(monster_t);
    static monster_t Upgrade(monster_t);
    static u16 GetRNDSize(monster_t);
    static u8  GetLevel(monster_t);
    static u32 GetDwelling(monster_t);

    static monster_t FromInt(u8);
    static monster_t FromDwelling(u8, u32);
    static monster_t FromObject(u8);
    static monster_t FromMaps(const Maps::Tiles &);
    static monster_t Rand(level_t = LEVEL0);

protected:
    monster_t id;
};

#endif
