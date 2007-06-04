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

#ifndef H2HEROES_H
#define H2HEROES_H

#include <string>
#include <vector>
#include "race.h"
#include "spell.h"
#include "color.h"
#include "morale.h"
#include "mp2.h"
#include "luck.h"
#include "dialog.h"
#include "army.h"
#include "skill.h"
#include "artifact.h"
#include "gamedefs.h"

#define HEROESMAXSKILL		8
#define HEROESMAXARTIFACT	14
#define HEROESMAXARMY		5

/*
#define HEROESMAXCOUNT		60
#define SCOUTINGBASE		4
*/

#define DEFAULT_KNGT_ATTACK	2
#define DEFAULT_KNGT_DEFENCE	2
#define DEFAULT_KNGT_POWER	1
#define DEFAULT_KNGT_KNOWLEDGE	1
#define DEFAULT_BARB_ATTACK	3
#define DEFAULT_BARB_DEFENCE	1
#define DEFAULT_BARB_POWER	1
#define DEFAULT_BARB_KNOWLEDGE	1
#define DEFAULT_NECR_ATTACK	1
#define DEFAULT_NECR_DEFENCE	0
#define DEFAULT_NECR_POWER	2
#define DEFAULT_NECR_KNOWLEDGE	2
#define DEFAULT_SORC_ATTACK	0
#define DEFAULT_SORC_DEFENCE	0
#define DEFAULT_SORC_POWER	2
#define DEFAULT_SORC_KNOWLEDGE	3
#define DEFAULT_WRLK_ATTACK	0
#define DEFAULT_WRLK_DEFENCE	0
#define DEFAULT_WRLK_POWER	3
#define DEFAULT_WRLK_KNOWLEDGE	2
#define DEFAULT_WZRD_ATTACK	0
#define DEFAULT_WZRD_DEFENCE	1
#define DEFAULT_WZRD_POWER	2
#define DEFAULT_WZRD_KNOWLEDGE	2

class Heroes
{
public:
    typedef enum {
	// knight
	LORDKILBURN, SIRGALLANTH, ECTOR, GVENNETH, TYRO, AMBROSE, RUBY, MAXIMUS, DIMITRY,
	// barbarian
	THUNDAX, FINEOUS, JOJOSH, CRAGHACK, JEZEBEL, JACLYN, ERGON, TSABU, ATLAS,
	// sorceress
	ASTRA, NATASHA, TROYAN, VATAWNA, REBECCA, GEM, ARIEL, CARLAWN, LUNA,
	// warlock
	ARIE, ALAMAR, VESPER, CRODO, BAROK, KASTORE, AGAR, FALAGAR, WRATHMONT,
	// wizard
	MYRA, FLINT, DAWN, HALON, MYRINI, WILFREY, SARAKIN, KALINDRA, MANDIGAL,
	// necromancer
	ZOM, DARLANA, ZAM, RANLOO, CHARITY, RIALDO, ROXANA, SANDRO, CELIA,
	// from campain
	ROLAND, UNKNOWN1, UNKNOWN2, ARCHIBALD, SANDYSANDY, BRAX,
    } heroes_t;

    Heroes(heroes_t ht, Race::race_t rc, const std::string & str);

    bool isFreeman(void) const{ return Color::GRAY == color; };
    void LoadFromMP2(u16 map_index, const void *ptr,  const Color::color_t cl);

    Heroes::heroes_t GetHeroes(void) const{ return heroes; };
    Color::color_t GetColor(void) const{ return color; };
    Race::race_t GetRace(void) const{ return race; };
    const std::string & GetName(void) const{ return name; };
    const Point & GetCenter(void) const{ return mp; };
    const std::vector<Army::Troops> & GetArmy(void) const{ return army; };
    u8 GetCountArmy(void) const;

    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;
    u32 GetExperience(void) const;
    u32 GetNextLevelExperience(u8 level) const;
    u16 GetMaxSpellPoints(void) const;
    u16 GetSpellPoints(void) const;
    Morale::morale_t GetMorale(void) const;
    Luck::luck_t GetLuck(void) const;
    u8 GetLevel(void) const{ return 1; };
    const std::vector<Artifact::artifact_t> GetArtifacts(void) const{ return artifacts; };

    u8 GetMobilityIndexSprite(void) const;
    u8 GetManaIndexSprite(void) const;

    Dialog::answer_t OpenDialog(void);

    void Recrut(const Castle & castle);
    void Move(u16 ax, u16 ay);

private:
    std::string		name;
    Color::color_t	color;
    u8			attack;
    u8			defence;
    u8			power;
    u8			knowledge;
    u32			experience;
    u16			magic_point;
    Morale::morale_t	morale;
    Luck::luck_t	luck;

    std::vector<Skill> skills;
    std::vector<Artifact::artifact_t>	artifacts;
    std::vector<Army::Troops> army;
    //std::vector<Spell::spell_t>	books;

    const heroes_t	heroes;
    const Race::race_t	race;

    bool		army_spread;
    float		move_point;
    
    MP2::object_t	save_maps_general;
    
    Point		mp;
};

#endif
