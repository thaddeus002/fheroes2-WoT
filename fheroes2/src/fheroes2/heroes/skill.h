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
#include "morale.h"
#include "luck.h"
#include "gamedefs.h"

#define MAXPRIMARYSKILL		6
#define MAXSECONDARYSKILL	14

namespace Skill
{


    namespace Level
    {
	typedef enum { NONE, BASIC, ADVANCED, EXPERT } type_t;
	
	type_t FromMP2(const u8 byte);

	const std::string & String(const type_t level);
    };
    
    class Primary
    {
	public:

	typedef enum
	{
	    UNKNOWN	= 0,
	    ATTACK	= 1,
	    DEFENCE	= 2,
	    POWER	= 3,
	    KNOWLEDGE	= 4,
	} skill_t;

	Primary();
	virtual ~Primary(){};

    	virtual u8 GetAttack(void) const = 0;
	virtual u8 GetDefense(void) const = 0;
        virtual u8 GetPower(void) const = 0;
        virtual u8 GetKnowledge(void) const = 0;
	virtual Morale::morale_t GetMorale(void) const = 0;
	virtual Luck::luck_t GetLuck(void) const = 0;

        static const std::string & String(const skill_t skill);
	static skill_t FromLevelUp(const u8 race, const u8 level);

	protected:
	u8			attack;
	u8			defence;
	u8			power;
	u8			knowledge;
	Morale::morale_t	morale;		// deprecated, will be removed.
        Luck::luck_t		luck;		// deprecated, will be removed.
    };

    class Secondary : private std::pair<u8, u8>
    {
	public:

	typedef enum
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
	} skill_t;

	Secondary();
	Secondary(const skill_t & s, const Level::type_t & t);

	void		SetSkill(const skill_t s);
	void		SetLevel(const Level::type_t level);
	void		NextLevel(void);

	Level::type_t	Level(void) const;
	skill_t		Skill(void) const;

	static skill_t 	FromMP2(const u8 byte);
	static skill_t 	RandForWitchsHut(void);
	static const std::string & String(const skill_t skill);
	static const std::string & Description(const skill_t skill, const Level::type_t level);

	/* index sprite from SECSKILL */
	static u8 GetIndexSprite1(const skill_t skill);

	/* index sprite from MINISS */
	static u8 GetIndexSprite2(const skill_t skill);
    };
};

#endif
