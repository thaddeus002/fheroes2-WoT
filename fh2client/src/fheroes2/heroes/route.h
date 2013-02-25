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
#ifndef H2HEROPATH_H
#define H2HEROPATH_H

#include <list>
#include "gamedefs.h"
#include "direction.h"

class Heroes;

namespace Route
{
    class Step
    {
	public:
	    Step() : from(-1), direction(Direction::CENTER), penalty(0) {}
	    Step(s32 index, u16 dir, u16 cost) : from(index), direction(dir), penalty(cost) {}

	    s32		GetIndex(void) const;
	    const u16 &	GetPenalty(void) const;
	    const s32 &	GetFrom(void) const;
	    const u16 &	GetDirection(void) const;
	    bool	isBad(void) const;

	protected:
    	    friend StreamBase & operator<< (StreamBase &, const Step &);
    	    friend StreamBase & operator>> (StreamBase &, Step &);

	    s32		from;
	    u16		direction;
	    u16		penalty;
    };

    class Path : public std::list<Step>
    {
	public:
	    Path(const Heroes &);
	    Path(const Path &);

	    Path &	operator= (const Path &);

	    s32		GetDestinationIndex(void) const;
	    s32		GetLastIndex(void) const;
	    s32		GetDestinedIndex(void) const;
	    u16		GetFrontDirection(void) const;
	    u16		GetFrontPenalty(void) const;
	    u32		GetTotalPenalty(void) const;
	    bool	Calculate(const s32 &, u16 limit = MAXU16);

	    void	Show(void){ hide = false; }
	    void	Hide(void){ hide = true; }
	    void	Reset(void);
	    void	PopFront(void);
	    void	PopBack(void);
	    void	RescanObstacle(void);
	    void	RescanPassable(void);

	    bool	isComplete(void) const;
	    bool	isValid(void) const;
	    bool	isShow(void) const { return !hide; }
	    bool	hasObstacle(void) const;

	    std::string	String(void) const;

	    u16		GetAllowStep(void) const;

    	    static u16	GetIndexSprite(u16 from, u16 to, u8 mod);

	private:
	    bool	Find(const s32 &, const u16 limit = MAXU16);

	    friend StreamBase & operator<< (StreamBase &, const Path &);
	    friend StreamBase & operator>> (StreamBase &, Path &);

	    const Heroes* hero;
	    s32		dst;
	    bool	hide;
    };

    StreamBase & operator<< (StreamBase &, const Step &);
    StreamBase & operator<< (StreamBase &, const Path &);
    StreamBase & operator>> (StreamBase &, Step &);
    StreamBase & operator>> (StreamBase &, Path &);
}

#endif
