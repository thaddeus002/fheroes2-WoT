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

#ifndef H2SKILL_H
#define H2SKILL_H

#include <string>
#include <utility>

class Skill
{
    public:
	typedef enum { NEVER, BASIC, ADVANCED, EXPERT } level_t;

	typedef enum {
	    PATHFINDING	= 0,
	    ARCHERY	= 1,
	    LOGISTICS	= 2,
	    SCOUTING	= 3,
	    DIPLOMACY	= 4,
	    NAVIGATION	= 5,
	    LEADERSHIP	= 6,
	    WISDOM	= 7,
	    MYSTICISM	= 8,
	    LUCK	= 9,
	    BALLISTICS	= 10,
	    EAGLEEYE	= 11,
	    NECROMANCY	= 12,
	    ESTATES	= 13,
	    NONE = 0xFF
	} skill_t;

	Skill(skill_t type = NONE, level_t level = BASIC) : pair(type, level) {};
	
	skill_t GetSkill(void) const{ return pair.first; };
	level_t GetLevel(void) const{ return pair.second; };

	void SetSkill(skill_t skill){ pair.first = skill; };
	void SetLevel(level_t level){ pair.second = level; };

	void SetSkill(u8 skill);
	void SetLevel(u8 level);

	static const std::string & LevelString(level_t level);
	static const std::string & String(skill_t skill);
	static const std::string & Description(skill_t skill, level_t level);

    private:
	std::pair<skill_t, level_t> pair;
};

#endif
