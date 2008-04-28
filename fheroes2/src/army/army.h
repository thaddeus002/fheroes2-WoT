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
#ifndef H2ARMY_H
#define H2ARMY_H

#include <string>
#include <vector>
#include <utility>
#include "race.h"
#include "dialog.h"
#include "monster.h"
#include "rect.h"
#include "skill.h"
#include "spritecursor.h"
#include "gamedefs.h"

class Surface;
class Heroes;

namespace Army
{
    typedef enum
    {
	FEW	= 1,
	SEVERAL	= 5,
	PACK	= 10,
	LOTS	= 20,
	HORDE	= 50,
	THRONG	= 100,
	SWARM	= 250,
	ZOUNDS	= 500,
	LEGION	= 1000
    } size_t;

    const std::string & String(size_t size);
    size_t GetSize(u16 count);

    class Troops
    {
    public:
	Troops(Monster::monster_t m = Monster::UNKNOWN, u16 c = 0) : monster(m), count(c), master_skill(NULL){};
	Troops(const Troops & troops);

        void Set(Monster::monster_t m, u16 c){ monster = m; count = c; };
        void SetMonster(Monster::monster_t m){ monster = m; };
        void SetCount(u16 c){ count = c; };
        void SetMasterSkill(const Skill::Primary* p){ master_skill = p; };
	Troops & operator= (const Troops & troops);

        static bool PredicateIsValid(const Troops & t);

        bool isValid(void) const;

        Monster::monster_t Monster(void) const{ return monster; };
        u16 Count(void) const{ return count; };
        const Skill::Primary* MasterSkill(void) const{ return master_skill; };
	
	void SetPosition(const Point & pt) { pos = pt; };
	const Point& Position() const { return pos; };

    private:
        Monster::monster_t	monster;
        u16			count;
        const Skill::Primary*	master_skill;
	Point                   pos;
    };

    bool isValid(const Troops & army);

    class SelectBar
    {
    public:
	SelectBar(const Point & pos, const std::vector<Troops> & troops);

	bool isSelected(void) const{ return selected; };

	const std::vector<Rect> & GetCoords(void) const{ return coords; };
	
	u8 GetCursorIndex(void) const{ return cursor_index; };

	void Redraw(void);
	void Reset(void);
	void Select(u8 index);

    private:
	const Point pos_pt;
	const Surface & empty_back;
	const u8 step;
	SpriteCursor cursor;
	const std::vector<Troops> & army;
	bool selected;
	u8 cursor_index;
	
	std::vector<Rect> coords;
    };
};

#endif
