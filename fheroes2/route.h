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

#include <vector>
#include "gamedefs.h"
#include "rect.h"

class Route
{
    public:
	Route() : hero(NULL), dst(MAXU16){};
	
	void	FromHeroes(const Heroes *from);

	u16	Calculate(u16 dst_index);
	u16	Length(void) const{ return path.size(); };
	u16	GetDestinationPoint(void) const{ return dst; };

	void	Show(void);
	void	Hide(void);
	void	Reset(void);

    private:
	const Sprite & GetSprite(const Direction::vector_t & from, const Direction::vector_t & to);
	void	Dump(void);

    private:
	const Heroes * hero;
	u16   dst;
	std::vector<u16> path;
};

#endif
