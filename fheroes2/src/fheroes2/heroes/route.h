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
#ifndef H2HEROPATH_H
#define H2HEROPATH_H

#include <list>
#include "gamedefs.h"
#include "rect.h"

class Heroes;

namespace Route
{
    struct Step
    {
	Step() : to_index(MAXU16), penalty(MAXU16), green_color(false) {};
	Step(const u16 t, const u16 p) : to_index(t), penalty(p), green_color(false) {};

	const u16 to_index;
	const u16 penalty;
	bool green_color;
    };

    class Path : public std::list<Step>
    {
	public:
	    Path(const Heroes & h);

	    void	DestinationIndex(u16 index){ dst = index; };
	    u16		GetDestinationIndex(void) const{ return dst; };
	    u16		GetFrontIndex(void) const{ return size() ? front().to_index : 0; };
	    u16		GetFrontPenalty(void) const{ return size() ? front().penalty : 0; };

	    u16		Calculate(const u16 dst_index, const u16 limit = MAXU16);

	    void	Show(void);
	    void	Hide(void);
	    void	Reset(void);
	    void	Rescan(void);
	    void	PopFront(void){ if(size()) pop_front(); };

	    u16		NextToLast(void) const;
	    bool	isValid(void) const;
	    bool	isShow(void) const;
	    bool	EnableMove(void) const;
	    u32		TotalPenalty(void) const;

	    static u16	GetIndexSprite(const Direction::vector_t & from, const Direction::vector_t & to);

	private:
	    void	Dump(void) const;
	    void	Find_v1(const u16 index1, const u16 index2, const u16 limit = MAXU16);

	private:
	    const Heroes & hero;
	    u16		dst;
	    bool	hide;
    };
};

#endif
