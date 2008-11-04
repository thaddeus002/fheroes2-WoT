/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#ifndef H2GAMEFOCUS_H
#define H2GAMEFOCUS_H

#include "rect.h"
#include "game.h"

class Castle;
class Heroes;

namespace Game
{
    class Focus
    {
    public:
	typedef enum { UNSEL, HEROES, CASTLE } focus_t;

   	static Focus &	Get(void);

	void		Set(const Heroes *hr);
	void		Set(const Castle *cs);
	void		Redraw(void);
	void		Reset(const focus_t priority = UNSEL);

	const Point &	Center(void) const;

	focus_t		Type(void) const;

	const Castle &	GetCastle(void) const;
	const Heroes &	GetHeroes(void) const;
	
	Heroes & GetHeroes(void);
	Castle & GetCastle(void);

    private:
	Focus();

	const Castle *	castle;
	const Heroes *	heroes;
	Point		center;
    };
};

#endif
