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
#include "gamedefs.h"

void StringAppendModifiers(std::string &, int);

class Spell;
class Heroes;

namespace Skill
{
    int		GetLeadershipModifiers(int level, std::string* strs);
    int		GetLuckModifiers(int level, std::string* strs);
    void	UpdateStats(const std::string &);

    namespace Level
    {
	enum { NONE = 0, BASIC = 1, ADVANCED = 2, EXPERT = 3 };

	const char* String(int level);
    }

    class Secondary : public std::pair<int, int>
    {
	public:
	enum
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
	Secondary(int skill, int level);

	void		Reset(void);
	void		Set(const Secondary &);
	void		SetSkill(int);
	void		SetLevel(int);
	void		NextLevel(void);

	int		Level(void) const;
	int		Skill(void) const;

	bool		isLevel(int) const;
	bool		isSkill(int) const;
	bool		isValid(void) const;

	const char*	GetName(void) const;
	std::string	GetDescription(void) const;
	u32		GetValues(void) const;

	/* index sprite from SECSKILL */
	int		GetIndexSprite1(void) const;
	/* index sprite from MINISS */
	int		GetIndexSprite2(void) const;

	static int	RandForWitchsHut(void);
	static const char*
			String(int);
    };

    StreamBase & operator>> (StreamBase &, Secondary &);

    struct SecSkills : std::vector<Secondary>
    {
	SecSkills();
	SecSkills(int race);

	int		GetLevel(int skill) const;
	u32		GetValues(int skill) const;
	void		AddSkill(const Skill::Secondary &);
	void		FindSkillsForLevelUp(int race, Secondary &, Secondary &) const;
	void		FillMax(const Skill::Secondary &);
	std::string	String(void) const;
    };

    class Primary
    {
	public:
	Primary();
	virtual ~Primary(){};

	enum { UNKNOWN = 0, ATTACK = 1, DEFENSE = 2, POWER = 3, KNOWLEDGE = 4 };

    	virtual int GetAttack(void) const = 0;
	virtual int GetDefense(void) const = 0;
        virtual int GetPower(void) const = 0;
        virtual int GetKnowledge(void) const = 0;
	virtual int GetMorale(void) const = 0;
	virtual int GetLuck(void) const = 0;
	virtual int GetRace(void) const = 0;

	virtual bool	isCaptain(void) const;
	virtual bool	isHeroes(void) const;

	int		LevelUp(int race, int level);

	std::string	StringSkills(const std::string &) const;

        static const char*	String(int);
	static std::string	StringDescription(int, const Heroes*);
	static int		GetInitialSpell(int race);

	protected:
	void LoadDefaults(int type, int race);

	friend StreamBase & operator<< (StreamBase &, const Primary &);
	friend StreamBase & operator>> (StreamBase &, Primary &);

	int			attack;
	int			defense;
	int			power;
	int			knowledge;
    };

    StreamBase & operator<< (StreamBase &, const Primary &);
    StreamBase & operator>> (StreamBase &, Primary &);
}

#include "interface_itemsbar.h"
class PrimarySkillsBar : public Interface::ItemsBar<int>
{
public:
    PrimarySkillsBar(const Heroes*, bool mini);

    void	SetTextOff(s32, s32);
    void	RedrawBackground(const Rect &, Surface &);
    void	RedrawItem(int &, const Rect &, Surface &);

    bool	ActionBarSingleClick(const Point &, int &, const Rect &);
    bool	ActionBarPressRight(const Point &, int &, const Rect &);
    bool        ActionBarCursor(const Point &, int &, const Rect &);

    bool        QueueEventProcessing(std::string* = NULL);

protected:
    const Heroes*	hero;
    Surface		backsf;
    bool		use_mini_sprite;
    std::vector<int>	content;
    Point		toff;
    std::string		msg;
};

class SecondarySkillsBar : public Interface::ItemsBar<Skill::Secondary>
{
public:
    SecondarySkillsBar(bool mini = true, bool change = false);

    void	RedrawBackground(const Rect &, Surface &);
    void	RedrawItem(Skill::Secondary &, const Rect &, Surface &);

    bool	ActionBarSingleClick(const Point &, Skill::Secondary &, const Rect &);
    bool	ActionBarPressRight(const Point &, Skill::Secondary &, const Rect &);
    bool        ActionBarCursor(const Point &, Skill::Secondary &, const Rect &);

    bool        QueueEventProcessing(std::string* = NULL);

protected:
    Surface	backsf;
    bool	use_mini_sprite;
    bool	can_change;
    std::string	msg;
};

#endif
