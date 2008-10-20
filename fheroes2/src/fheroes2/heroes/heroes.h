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
#include <list>
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
#include "route.h"
#include "visit.h"
#include "direction.h"
#include "spell_book.h"
#include "gamedefs.h"

#define HEROESMAXARTIFACT	14
#define HEROESMAXSKILL		8
#define HEROESMAXARMY		5
#define HEROESMAXCOUNT		71

#define DEFAULT_MOVE_POINTS	11


#define SCOUTINGBASE		4


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

class Heroes : public Skill::Primary
{
public:
    typedef enum
    {
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
	ROLAND, CORLAGON, ELIZA, ARCHIBALD, HALTON, BAX,
	// from extended
	SOLMYR, DAINWIN, MOG, UNCLEIVAN, JOSEPH, GALLAVANT, ELDERIAN, CEALLACH, DRAKONIA, MARTINE, JARKONAS,
	// debugger
	SANDYSANDY, UNKNOWN
    } heroes_t;

    Heroes(heroes_t ht, Race::race_t rc, const std::string & str);

    bool isFreeman(void) const{ return Color::GRAY == color; };
    void SetFreeman(void) { color = Color::GRAY; mp.x = -1; mp.y = -1; };

    bool operator== (const Heroes & h) const;

    const Castle* inCastle(void) const;

    void LoadFromMP2(u16 map_index, const void *ptr,  const Color::color_t cl);

    Heroes::heroes_t GetHeroes(void) const{ return heroes; };
    Color::color_t GetColor(void) const{ return color; };
    Race::race_t GetRace(void) const{ return race; };
    const std::string & GetName(void) const{ return name; };
    const Point & GetCenter(void) const{ return mp; };
    const std::vector<Army::Troops> & GetArmy(void) const{ return army; };
    bool HasMonster(const Monster::monster_t mon) const;

    u8 GetCountArmy(void) const;
    const Spell::Book & SpellBook(void) const { return spell_book; };

    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;
    u16 GetMaxSpellPoints(void) const;
    u16 GetMaxMovePoints(void) const;
    u16 GetSpellPoints(void) const;
    u16 GetMovePoints(void) const;
    Morale::morale_t GetMorale(void) const;
    Luck::luck_t GetLuck(void) const;
    u8 GetLevel(void) const;

    bool HasSecondarySkill(const Skill::Secondary::skill_t skill) const;
    Skill::Level::type_t GetLevelSkill(const Skill::Secondary::skill_t skill) const;
    void LearnBasicSkill(const Skill::Secondary::skill_t skill);
    void LevelUpSkill(const Skill::Secondary::skill_t skill);

    const std::vector<Artifact::artifact_t> & GetArtifacts(void) const{ return artifacts; };
    bool PickupArtifact(const Artifact::artifact_t & art);
    bool HasArtifact(const Artifact::artifact_t & art) const;

    u8 GetMobilityIndexSprite(void) const;
    u8 GetManaIndexSprite(void) const;

    Dialog::answer_t OpenDialog(bool readonly = false);
    void MeetingDialog(Heroes & heroes2);

    void Recruit(const Color::color_t & cl, const Point & pt);
    void Recruit(const Castle & castle);

    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);
    void ActionAfterBattle(void);

    bool BuySpellBook(void);
    void AppendSpellsToBook(const Spell::Storage & spells);
    void AppendSpellToBook(const Spell::spell_t spell);

    const Route::Path & GetPath(void) const{ return path; };
    Route::Path & GetPath(void) { return path; };
    u16 FindPath(u16 dst_index);
    u8 GetRangeRouteDays(void) const;
    void ShowPath(bool f){ f ? path.Show() : path.Hide(); };

    Direction::vector_t GetDirection(void) const{ return direction; };

    void SetVisited(const u16 index, const Visit::type_t type = Visit::LOCAL);
    bool isVisited(const MP2::object_t & object, const Visit::type_t type = Visit::LOCAL) const;
    bool isVisited(const Maps::Tiles & tile, const Visit::type_t type = Visit::LOCAL) const;

    void SetCenter(const Point& pt){ mp = pt; };
    void SetCenter(const u16 index);

    bool isEnableMove(void) const;
    void SetMove(bool f);
    bool isAllowMove(const u16 dst_index);
    void Action(const Maps::Tiles & dst);
    void ApplyPenaltyMovement(void);

    void AngleStep(const Direction::vector_t to_direct);
    bool MoveStep(void);
    void Redraw(void) const;
    u8   GetSpriteIndex(void) const{ return sprite_index; };

    void PlayWalkSound(void) const;
    void PlayPickupSound(void) const;

    bool isShipMaster(void) const;
    void SetShipMaster(bool f);

    MP2::object_t GetUnderObject(void) const;

    u32 GetExperience(void) const;
    void IncreaseExperience(const u16 exp);
    void LevelUp(void);

    void Scoute(void);

    static u8 GetLevelFromExperience(u32 exp);
    static u32 GetExperienceFromLevel(u8 lvl);

    bool spellCasted;

protected:
    bool JoinTroops(const Monster::monster_t, const u16 count);

    void ActionToCastle(const u16 dst_index);
    void ActionToHeroes(const u16 dst_index);
    void ActionToMonster(const u16 dst_index);
    void ActionToBoat(const u16 dst_index);
    void ActionToCoast(const u16 dst_index);
    void ActionToResource(const u16 dst_index);
    void ActionToPickupResource(const u16 dst_index);
    void ActionToFlotSam(const u16 dst_index);
    void ActionToArtifact(const u16 dst_index);
    void ActionToShrine(const u16 dst_index);
    void ActionToWitchsHut(const u16 dst_index);
    void ActionToLuckObject(const u16 dst_index);
    void ActionToSign(const u16 dst_index);
    void ActionToMagicWell(const u16 dst_index);
    void ActionToTradingPost(const u16 dst_index);
    void ActionToPrimarySkillObject(const u16 dst_index);
    void ActionToMoraleObject(const u16 dst_index);
    void ActionToExperienceObject(const u16 dst_index);
    void ActionToTreasureChest(const u16 dst_index);
    void ActionToAncientLamp(const u16 dst_index);
    void ActionToTeleports(const u16 dst_index);
    void ActionToCaptureObject(const u16 dst_index);
    void ActionToJoinArmy(const u16 dst_index);
    void ActionToRecruitArmy(const u16 dst_index);

    void FindSkillsForLevelUp(Skill::Secondary & sec1, Skill::Secondary & sec2) const;

private:
    std::string		name;
    Color::color_t	color;
    u32			experience;
    u16			magic_point;
    u16			move_point;

    std::vector<Skill::Secondary>	secondary_skills;

    std::vector<Artifact::artifact_t>	artifacts;
    std::vector<Army::Troops> army;
    Spell::Book		spell_book;

    const heroes_t	heroes;
    const Race::race_t	race;

    bool		army_spread;
    bool		enable_move;
    bool 		shipmaster;

    MP2::object_t	save_maps_general;

    Point		mp;
    Route::Path		path;

    Direction::vector_t direction;
    u8			sprite_index;

    std::list<Visit::IndexObject> visit_object;
};

#endif
