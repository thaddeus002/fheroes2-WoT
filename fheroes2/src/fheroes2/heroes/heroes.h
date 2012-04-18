/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2HEROES_H
#define H2HEROES_H

#include <string>
#include <vector>
#include <list>
#include "mp2.h"
#include "dialog.h"
#include "route.h"
#include "pairs.h"
#include "visit.h"
#include "direction.h"
#include "heroes_base.h"
#include "army.h"
#include "gamedefs.h"

class Recruits;
class Surface;
class MageGuild;
namespace Battle { class Only; }

class Heroes : public HeroBase, public ColorBase
{
public:
    enum heroes_t
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
    };

    static heroes_t ConvertID(u8);

    enum flags_t
    {
	SHIPMASTER	= 0x00000001,
	//UNUSED	= 0x00000002,
	SPELLCASTED	= 0x00000004,
	ENABLEMOVE	= 0x00000008,
	//UNUSED	= 0x00000010,
	//UNUSED	= 0x00000020,
	//UNUSED	= 0x00000040,
	JAIL		= 0x00000080,
	ACTION		= 0x00000100,
	SAVEPOINTS	= 0x00000200,
	SLEEPER		= 0x00000400,
	GUARDIAN	= 0x00000800,
	NOTDEFAULTS	= 0x00001000,
	NOTDISMISS	= 0x00002000,
	VISIONS		= 0x00004000,
	PATROL		= 0x00008000,
	CUSTOMARMY	= 0x00010000
    };

    Heroes();
    Heroes(heroes_t ht, u8 rc);

    bool isValid(void) const;
    bool isFreeman(void) const;
    void SetFreeman(const u8 reason);

    const Castle* inCastle(void) const;
    Castle* inCastle(void);

    void LoadFromMP2(s32 map_index, const void *ptr,  const Color::color_t cl, const u8 rc);

    u8 GetRace(void) const{ return race; }
    const std::string & GetName(void) const{ return name; }
    Color::color_t GetColor(void) const{ return ColorBase::GetColor(); }
    static const char* GetName(Heroes::heroes_t);
    u8 GetType(void) const { return Skill::Primary::HEROES; }
    u8 GetControl(void) const;

    const Color::color_t & GetKillerColor(void) const;
    void SetKillerColor(u8);

    const Surface & GetPortrait30x22(void) const;
    const Surface & GetPortrait50x46(void) const;
    const Surface & GetPortrait101x93(void) const;

    static const Surface & GetPortrait30x22(heroes_t);
    static const Surface & GetPortrait50x46(heroes_t);
    static const Surface & GetPortrait101x93(heroes_t);

    const Army & GetArmy(void) const{ return army; }
    Army & GetArmy(void) { return army; }

    heroes_t GetID(void) const;

    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;

    u8 GetAttack(std::string*) const;
    u8 GetDefense(std::string*) const;
    u8 GetPower(std::string*) const;
    u8 GetKnowledge(std::string*) const;

    void IncreasePrimarySkill(const Skill::Primary::skill_t skill);

    s8 GetMorale(void) const;
    s8 GetLuck(void) const;
    s8 GetMoraleWithModificators(std::string *str = NULL) const;
    s8 GetLuckWithModificators(std::string *str = NULL) const;
    u8 GetLevel(void) const;

    u8 GetMapsObject(void) const;
    void SetMapsObject(u8);

    const Point & GetCenterPatrol(void) const;
    void SetCenterPatrol(const Point &);
    u8 GetSquarePatrol(void) const;

    u16 GetMaxSpellPoints(void) const;

    u16 GetMaxMovePoints(void) const;
    u16 GetMovePoints(void) const;
    void IncreaseMovePoints(const u16 point);
    bool MayStillMove(void) const;
    void ResetMovePoints(void) { move_point = 0; };
    void MovePointsScaleFixed(void);
    void RecalculateMovePoints(void);

    bool HasSecondarySkill(u8) const;
    bool HasMaxSecondarySkill(void) const;
    u8   GetLevelSkill(u8) const;
    u16  GetSecondaryValues(u8) const;
    void LearnSkill(const Skill::Secondary &);
    Skill::SecSkills & GetSecondarySkills(void);

    bool PickupArtifact(const Artifact &);
    bool HasUltimateArtifact(void) const;
    u8 GetCountArtifacts(void) const;
    bool IsFullBagArtifacts(void) const;

    u8 GetMobilityIndexSprite(void) const;
    u8 GetManaIndexSprite(void) const;

    Dialog::answer_t OpenDialog(bool readonly = false, bool fade = false);
    void MeetingDialog(Heroes &);

    bool Recruit(u8 col, const Point & pt);
    bool Recruit(const Castle & castle);

    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);
    void ActionAfterBattle(void);
    void ActionPreBattle(void);

    bool BuySpellBook(const Castle*, u8 shrine = 0);

    const Route::Path & GetPath(void) const{ return path; }
    Route::Path & GetPath(void) { return path; }
    u8 GetRangeRouteDays(const s32 dst) const;
    void ShowPath(bool f){ f ? path.Show() : path.Hide(); }
    void RescanPath(void);
    void RescanPathPassable(void);

    u16 GetDirection(void) const{ return direction; }

    void SetVisited(const s32 index, const Visit::type_t type = Visit::LOCAL);
    void SetVisitedWideTile(const s32 index, const u8 object, const Visit::type_t type = Visit::LOCAL);
    bool isVisited(const u8 object, const Visit::type_t type = Visit::LOCAL) const;
    bool isVisited(const Maps::Tiles & tile, const Visit::type_t type = Visit::LOCAL) const;

    bool Move(bool fast = false);
    void Move2Dest(const s32 &, bool skip_action = false);
    bool isEnableMove(void) const;
    bool CanMove(void) const;
    void SetMove(bool f);
    bool isAction(void) const { return Modes(ACTION); }
    void ResetAction(void) { ResetModes(ACTION); }
    void Action(const s32 dst_index);
    void ActionNewPosition(void);
    bool ApplyPenaltyMovement(void);
    bool ActionSpellCast(const Spell &);

    void Redraw(Surface &, bool) const;
    void Redraw(Surface &, const s16, const s16, bool) const;
    u8   GetSpriteIndex(void) const{ return sprite_index; }
    void FadeOut(void) const;
    void FadeIn(void) const;
    void Scoute(void) const;
    u8   GetScoute(void) const;
    u8   CanScouteTile(s32) const;
    u16  GetVisionsDistance(void) const;

    bool isShipMaster(void) const;
    void SetShipMaster(bool f);

    u32 GetExperience(void) const;
    void IncreaseExperience(const u32 exp);

    bool AllowBattle(bool attacker) const;

    std::string String(void) const;

    static u8 GetLevelFromExperience(u32 exp);
    static u32 GetExperienceFromLevel(u8 lvl);

    static void ScholarAction(Heroes &, Heroes &);

private:
    friend StreamBase & operator<< (StreamBase &, const Heroes &);
    friend StreamBase & operator>> (StreamBase &, Heroes &);
    friend class Recruits;
    friend class Battle::Only;

    void LevelUp(bool skipsecondary, bool autoselect = false);
    u8   LevelUpPrimarySkill(void);
    void LevelUpSecondarySkill(u8, bool autoselect = false);
    void AngleStep(u16 to_direct);
    bool MoveStep(bool fast = false);
    static void MoveStep(Heroes &, s32 from, s32 to, bool newpos);

    std::string		name;
    ColorBase		killer_color;
    u32			experience;
    s32			move_point_scale;

    Skill::SecSkills	secondary_skills;

    Army        	army;

    heroes_t		hid;
    heroes_t		portrait;
    u8			race;

    u8			save_maps_object;

    Route::Path		path;

    u16			direction;
    u8			sprite_index;

    Point		patrol_center;
    u8			patrol_square;

    std::list<IndexObject> visit_object;
};

struct VecHeroes : public std::vector<Heroes*>
{
    Heroes* Get(Heroes::heroes_t) const;
    Heroes* Get(s32) const;
};

struct AllHeroes : public VecHeroes
{
    AllHeroes();
    ~AllHeroes();

    void Init(void);
    void clear(void);

    void Scoute(u8) const;

    Heroes* GetGuest(const Castle &) const;
    Heroes* GetGuard(const Castle &) const;
    Heroes* GetFreeman(u8) const;
    Heroes* FromJail(s32) const;

    bool HaveTwoFreemans(void) const;
};

StreamBase & operator<< (StreamBase &, const VecHeroes &);
StreamBase & operator>> (StreamBase &, VecHeroes &);

StreamBase & operator<< (StreamBase &, const Heroes::heroes_t &);
StreamBase & operator>> (StreamBase &, Heroes::heroes_t &);

StreamBase & operator<< (StreamBase &, const Heroes &);
StreamBase & operator>> (StreamBase &, Heroes &);

StreamBase & operator<< (StreamBase &, const AllHeroes &);
StreamBase & operator>> (StreamBase &, AllHeroes &);

#endif
