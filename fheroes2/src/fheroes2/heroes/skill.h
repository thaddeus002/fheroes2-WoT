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

#ifndef H2SKILL_H
#define H2SKILL_H

#include <string>
#include <vector>
#include <utility>
#include "color.h"
#include "interface_itemsbar.h"
#include "gamedefs.h"

void StringAppendModifiers(std::string &, s8);
class Spell;
class Heroes;

namespace Skill
{
    s8 GetLeadershipModifiers(u8 level, std::string* strs);
    s8 GetLuckModifiers(u8 level, std::string* strs);

    void UpdateStats(const std::string &);

    namespace Level
    {
	enum type_t { NONE=0, BASIC=1, ADVANCED=2, EXPERT=3 };

	const char* String(u8 level);
    }

    class Secondary : public std::pair<u8, u8>
    {
	public:
	enum skill_t
	{
	    UNKNOWN	= 0,
	    PATHFINDING	= 1,
	    ARCHERY	= 2,
	    LOGISTICS	= 3,
	    SCOUTING	= 4,
	    DIPLOMACY	= 5,
	    NAVIGATION	= 6,
	    LEADERSHIP	= 7,
	    WISDOM	= 8,
	    MYSTICISM	= 9,
	    LUCK	= 10,
	    BALLISTICS	= 11,
	    EAGLEEYE	= 12,
	    NECROMANCY	= 13,
	    ESTATES	= 14,

	    LEARNING	= EAGLEEYE
	};

	Secondary();
	Secondary(u8 skill, u8 level);

	void		Reset(void);
	void		Set(const Secondary &);
	void		SetSkill(u8);
	void		SetLevel(u8);
	void		NextLevel(void);

	u8		Level(void) const;
	u8		Skill(void) const;

	bool		isLevel(u8) const;
	bool		isSkill(u8) const;
	bool		isValid(void) const;

	const char*	GetName(void) const;
	std::string	GetDescription(void) const;
	u16		GetValues(void) const;

	/* index sprite from SECSKILL */
	u8		GetIndexSprite1(void) const;
	/* index sprite from MINISS */
	u8		GetIndexSprite2(void) const;

	static u8	RandForWitchsHut(void);
	static const char* String(u8);
    };

    struct SecSkills : std::vector<Secondary>
    {
	SecSkills();
	SecSkills(u8 race);

	u8	GetLevel(u8 skill) const;
	u16	GetValues(u8 skill) const;
	void	AddSkill(const Skill::Secondary &);
	void	FindSkillsForLevelUp(u8 race, Secondary &, Secondary &) const;
	void	FillMax(const Skill::Secondary &);
	void	ReadFromMP2(const u8*);

	std::string String(void) const;
    };

    class Primary
    {
	public:
	enum skill_t
	{
	    UNKNOWN	= 0,
	    ATTACK	= 1,
	    DEFENSE	= 2,
	    POWER	= 3,
	    KNOWLEDGE	= 4
	};

	enum type_t
	{
	    UNDEFINED,
	    CAPTAIN,
	    HEROES
	};

	Primary();
	virtual ~Primary(){};

    	virtual u8 GetAttack(void) const = 0;
	virtual u8 GetDefense(void) const = 0;
        virtual u8 GetPower(void) const = 0;
        virtual u8 GetKnowledge(void) const = 0;
	virtual s8 GetMorale(void) const = 0;
	virtual s8 GetLuck(void) const = 0;
	virtual u8 GetRace(void) const = 0;
	virtual u8 GetType(void) const = 0;

	u8 LevelUp(u8 race, u8 level);

	std::string StringSkills(const std::string &) const;

        static const char* String(u8);
	static std::string StringDescription(u8, const Heroes*);
	static u8 GetInitialSpell(u8 race);

	protected:
	void LoadDefaults(u8 type, u8 race);

	friend StreamBase & operator<< (StreamBase &, const Primary &);
	friend StreamBase & operator>> (StreamBase &, Primary &);

	u8			attack;
	u8			defense;
	u8			power;
	u8			knowledge;
    };

    StreamBase & operator<< (StreamBase &, const Primary &);
    StreamBase & operator>> (StreamBase &, Primary &);
}

class PrimarySkillsBar : public Interface::ItemsBar<Skill::Primary::skill_t>
{
public:
    PrimarySkillsBar(const Heroes*, bool mini);

    void	SetTextOff(s16, s16);
    void	RedrawBackground(const Rect &, Skill::Primary::skill_t*, Surface &);
    void	RedrawItem(Skill::Primary::skill_t &, const Rect &, Surface &);

    bool	ActionBarSingleClick(Skill::Primary::skill_t &);
    bool	ActionBarPressRight(Skill::Primary::skill_t &);

protected:
    const Heroes*                        hero;
    Surface				 backsf;
    bool	                         use_mini_sprite;
    std::vector<Skill::Primary::skill_t> content;
    Point				 toff;
};

class SecondarySkillsBar : public Interface::ItemsBar<Skill::Secondary>
{
public:
    SecondarySkillsBar(bool mini = true, bool change = false);

    void	RedrawBackground(const Rect &, Skill::Secondary*, Surface &);
    void	RedrawItem(Skill::Secondary &, const Rect &, Surface &);

    bool	ActionBarSingleClick(Skill::Secondary &);
    bool	ActionBarPressRight(Skill::Secondary &);

protected:
    Surface	backsf;
    bool	use_mini_sprite;
    bool	can_change;
};

#endif
