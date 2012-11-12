/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2BATTLE_TROOP_H
#define H2BATTLE_TROOP_H

#include <vector>
#include <utility>
#include "bitmodes.h"
#include "sprite.h"
#include "battle.h"
#include "battle_army.h"
#include "battle_arena.h"

class Sprite;
class Spell;
class HeroBase;

namespace Battle
{
    struct ModeDuration : public std::pair<u32, u16>
    {
	ModeDuration();
	ModeDuration(u32, u16);

	bool isMode(u32) const;
	bool isZeroDuration(void) const;
	void DecreaseDuration(void);
    };

    struct ModesAffected : public std::vector<ModeDuration>
    {
	ModesAffected();

	u16  GetMode(u32) const;
	void AddMode(u32, u16);
	void RemoveMode(u32);
	void DecreaseDuration(void);

	u32  FindZeroDuration(void) const;
    };

    StreamBase & operator<< (StreamBase &, const ModesAffected &);
    StreamBase & operator>> (StreamBase &, ModesAffected &);

    enum { CONTOUR_MAIN = 0, CONTOUR_BLACK = 0x01, CONTOUR_REFLECT = 0x02 };

    // battle troop stats
    class Unit : public ArmyTroop, public BitModes
    {
    public:
	Unit(const Troop &, u32 _uid, s16 pos, bool reflect);
	~Unit();

	bool		isModes(u32) const;
	bool		isBattle(void) const;
	std::string	GetShotString(void) const;
	std::string	GetSpeedString(void) const;
	u32		GetHitPointsLeft(void) const;
	u16		GetAffectedDuration(u32) const;
	u8		GetSpeed(void) const;
	const Sprite*	GetContour(u8) const;

	void	InitContours(void);
	void	SetMirror(Unit*);
	void	SetRandomMorale(void);
	void	SetRandomLuck(void);
	void	NewTurn(void);

	bool	isValid(void) const;
	bool	isArchers(void) const;
	bool	isFly(void) const;
	bool	isTwiceAttack(void) const;

	bool	AllowResponse(void) const;
	bool	isHandFighting(void) const;
	bool	isReflect(void) const;
	bool	isHaveDamage(void) const;
	bool 	isMagicResist(const Spell &, u8) const;
	bool	isMagicAttack(void) const;
	bool	OutOfWalls(void) const;


	std::string String(bool more = false) const;

	u32	GetUID(void) const;
	bool	isUID(u32) const;

	s16		GetHeadIndex(void) const;
	s16		GetTailIndex(void) const;
	const Position & GetPosition(void) const;
	void		SetPosition(s16);
	void		SetPosition(const Position &);
	void		SetReflection(bool);

	u16	GetAttack(void) const;
	u16	GetDefense(void) const;
	u8	GetArmyColor(void) const;
	u8	GetColor(void) const;
	u8	GetSpeed(bool skip_standing_check) const;
	u8	GetControl(void) const;
	u32	GetDamage(const Unit &) const;
	s32	GetScoreQuality(const Unit &) const;
	u32	GetDead(void) const;
	u32	GetHitPoints(void) const;
	u8	GetShots(void) const;
	u32	ApplyDamage(Unit &, u32);
	u32	ApplyDamage(u32);
	u32	GetDamageMin(const Unit &) const;
	u32	GetDamageMax(const Unit &) const;
	u32     CalculateDamageUnit(const Unit &, float) const;
	bool	ApplySpell(const Spell &, const HeroBase* hero, TargetInfo &);
	bool	AllowApplySpell(const Spell &, const HeroBase* hero, std::string* msg = NULL) const;
	void	PostAttackAction(Unit &);
	void	ResetBlind(void);
	void	SpellModesAction(const Spell &, u8, const HeroBase*);
	void	SpellApplyDamage(const Spell &, u8, const HeroBase*, TargetInfo &);
	void	SpellRestoreAction(const Spell &, u8, const HeroBase*);
	u32	Resurrect(u32, bool, bool);

	const monstersprite_t & GetMonsterSprite(void) const;

	const animframe_t & GetFrameState(void) const;
	const animframe_t & GetFrameState(u8) const;
	void	IncreaseAnimFrame(bool loop = false);
	bool    isStartAnimFrame(void) const;
	bool    isFinishAnimFrame(void) const;
	void	SetFrameStep(s8);
	void	SetFrame(u8);
	u8	GetFrame(void) const;
	u8      GetFrameOffset(void) const;
	u8      GetFrameStart(void) const;
	u8      GetFrameCount(void) const;

	s8 GetStartMissileOffset(u8) const;

	M82::m82_t M82Attk(void) const;
	M82::m82_t M82Kill(void) const;
	M82::m82_t M82Move(void) const;
	M82::m82_t M82Wnce(void) const;
	M82::m82_t M82Expl(void) const;

	ICN::icn_t ICNFile(void) const;
	ICN::icn_t ICNMiss(void) const;

	Point	GetBackPoint(void) const;
	Rect	GetRectPosition(void) const;

	u32	HowManyCanKill(const Unit &) const;
	u32	HowManyWillKilled(u32) const;

	void	SetResponse(void);
	void	ResetAnimFrame(u8);
	void	UpdateDirection(void);
	bool	UpdateDirection(const Rect &);
	void	PostKilledAction(void);

	u8	GetMagicResist(const Spell &, u8) const;
	u8	GetSpellMagic(bool force = false) const;
	u8	GetObstaclesPenalty(const Unit &) const;

	const HeroBase* GetCommander(void) const;

	static bool isHandFighting(const Unit &, const Unit &);

    private:
	friend StreamBase & operator<< (StreamBase &, const Unit &);
	friend StreamBase & operator>> (StreamBase &, Unit &);

	u32		uid;
	u32		hp;
	u32		count0;
	u32		dead;
	u8		shots;
	u8		disruptingray;
	bool		reflect;

	u8		animstate;
	u8		animframe;
	s8		animstep;

	Position	position;
	ModesAffected	affected;
	Unit*		mirror;
	Sprite		contours[4];

	bool		blindanswer;
    };

    StreamBase & operator<< (StreamBase &, const Unit &);
    StreamBase & operator>> (StreamBase &, Unit &);
}

#endif
