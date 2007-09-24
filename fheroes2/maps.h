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
#ifndef H2MAPS_H
#define H2MAPS_H

#include "gamedefs.h"

#define TILEWIDTH 32

namespace Maps
{

    typedef enum {
	ZERO	= 0,
        SMALL	= 36,
        MEDIUM	= 72,
        LARGE	= 108,
        XLARGE	= 144
    } mapsize_t;

    u16 GetTopIndex(u16 from);
    u16 GetTopRightIndex(u16 from);
    u16 GetRightIndex(u16 from);
    u16 GetBottomRightIndex(u16 from);
    u16 GetBottomIndex(u16 from);
    u16 GetBottomLeftIndex(u16 from);
    u16 GetLeftIndex(u16 from);
    u16 GetTopLeftIndex(u16 from);
};

#endif
