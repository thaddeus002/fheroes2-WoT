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
#ifndef H2DIRECTION_H
#define H2DIRECTION_H

#include <string>
#include "gamedefs.h"

namespace Direction
{
    typedef enum { CENTER, TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, BOTTOM, BOTTOM_LEFT, LEFT, TOP_LEFT, UNKNOWN } vector_t;

    inline vector_t& operator++ (vector_t& direct){ return direct = ( TOP_LEFT == direct ? CENTER : vector_t(direct + 1)); };
    inline vector_t& operator-- (vector_t& direct){ return direct = ( CENTER == direct ? TOP_LEFT : vector_t(direct - 1)); };

    const std::string & String(vector_t direct);

    vector_t Get(u16 from, u16 to);
};

#endif
