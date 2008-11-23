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
#include <list>
#include <vector>
#include "race.h"
#include "monster.h"
#include "skill.h"
#include "gamedefs.h"

class Surface;
class Heroes;
class Point;
class Rect;

#define	ARMYMAXTROOPS		5

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

    class Troop
    {
        public:
            Troop(Monster::monster_t m = Monster::UNKNOWN, u16 c = 0);
            
            void Set(Monster::monster_t m, u16 c){ monster = m; count = c; };
            void SetMonster(Monster::monster_t m){ monster = m; };
            void SetCount(u16 c){ count = c; };
            
            const Skill::Primary* MasterSkill(void) const{ return master_skill; };
            void SetMasterSkill(const Skill::Primary* p){ master_skill = p; };
            
            Monster::monster_t Monster(void) const{ return monster; };
            u16 Count(void) const{ return count; };
            
            bool isValid(void) const;
            
            static bool PredicateIsValid(const Troop & t);
            
        protected:
            Monster::monster_t monster;
            u16 count;
            const Skill::Primary* master_skill;
    };

    bool isValid(const Troop & troop);

    bool PredicateStrongestTroop(const Troop & t1, const Troop & t2);
    bool PredicateWeakestTroop(const Troop & t1, const Troop & t2);
    bool PredicateSlowestTroop(const Troop & t1, const Troop & t2);
    bool PredicateFastestTroop(const Troop & t1, const Troop & t2);

    typedef std::vector<Troop> army_t;

    s8 GetMoraleWithModificators(const army_t &, std::list<std::string> *list = NULL);
    s8 GetLuckWithModificators(const army_t &, std::list<std::string> *list = NULL);
    Troop & GetSlowestTroop(army_t & army);
    Troop & GetFastestTroop(army_t & army);
    Troop & GetStrongestTroop(army_t & army);
    Troop & GetWeakestTroop(army_t & army);
    const Troop & GetSlowestTroop(const army_t & army);
    const Troop & GetFastestTroop(const army_t & army);
    const Troop & GetStrongestTroop(const army_t & army);
    const Troop & GetWeakestTroop(const army_t & army);
    u8 GetCountTroops(const army_t & army);
    u8 GetCountUniqTroops(const army_t & army);
    Race::race_t GetRace(const army_t & army);
    bool HasMonster(const army_t & army, const Monster::monster_t mon);
    bool JoinTroop(army_t & army, const Troop & troop);
    bool JoinTroop(army_t & army, const Monster::monster_t mon, const u16 count);
    void SetMasterSkill(army_t & army, const Skill::Primary & primary);


    // deprecated
    class SelectBar
    {
    public:
	SelectBar(const Point & pos, const army_t & troops);

	bool isSelected(void) const{ return selected; };

	const std::vector<Rect> & GetCoords(void) const{ return coords; };
	
	u8 GetCursorIndex(void) const{ return cursor_index; };

	void Redraw(const u8 alpha = 0xFF);
	void Reset(void);
	void Select(u8 index);

    private:
	const Point pos_pt;
	const Surface & empty_back;
	const u8 step;
	SpriteCursor cursor;
	const army_t & army;
	bool selected;
	u8 cursor_index;
	
	std::vector<Rect> coords;
    };
};

#endif
