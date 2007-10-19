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
#include <vector>
#include <map>
#include <utility>
#include "morale.h"
#include "luck.h"
#include "gamedefs.h"

#define MAXPRIMARYSKILL		6
#define MAXSECONDARYSKILL	14

class Sprite;

namespace Skill
{
    typedef enum
    {
	ATTACK		= 0,
	DEFENCE		= 1,
	POWER		= 2,
	KNOWLEDGE	= 3
    } primary_t;

    typedef enum
    {
	PATHFINDING	= 0x0001,
	ARCHERY		= 0x0002,
	LOGISTICS	= 0x0004,
	SCOUTING	= 0x0008,
	DIPLOMACY	= 0x0010,
	NAVIGATION	= 0x0020,
	LEADERSHIP	= 0x0040,
	WISDOM		= 0x0080,
	MYSTICISM	= 0x0100,
	LUCK		= 0x0200,
	BALLISTICS	= 0x0400,
	EAGLEEYE	= 0x0800,
	NECROMANCY	= 0x1000,
	ESTATES		= 0x2000,
	UNKNOWN		= 0xFFFF,
    } secondary_t;

    inline secondary_t & operator++ (secondary_t & skill){ return skill = ( ESTATES == skill ? UNKNOWN : secondary_t(skill << 1)); };
    inline secondary_t & operator-- (secondary_t & skill){ return skill = ( UNKNOWN == skill ? ESTATES : secondary_t(skill >> 1)); };

    namespace Level
    {
	typedef enum { NONE, BASIC, ADVANCED, EXPERT } type_t;
	
	type_t FromMP2(u8 byte);

	const std::string & String(type_t level);
    };

    const std::string & String(primary_t skill);
    const std::string & String(secondary_t skill);
    const std::string & Description(secondary_t skill, Level::type_t level);

    class Primary
    {
	public:
	Primary();

	u8			attack;
	u8			defence;
	u8			power;
	u8			knowledge;
	Morale::morale_t	morale;
        Luck::luck_t		luck;
    };

    class Secondary
    {
	public:
	Secondary();

	void		Level(secondary_t skill, Level::type_t level);
	Level::type_t	GetLevel(secondary_t skill) const;
	secondary_t	GetSkill(u8 index) const;

	static secondary_t 	FromMP2(u8 byte);
	static const Sprite &	GetSprite(secondary_t skill);

	private:
	std::map<secondary_t, Level::type_t> skills;
	u8 count;
    };
};

#endif
