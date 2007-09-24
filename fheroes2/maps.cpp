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

#include "world.h"
#include "maps.h"

u16 Maps::GetTopIndex(u16 from)
{
    return from - world.w();
}

u16 Maps::GetTopRightIndex(u16 from)
{
    return from - world.w() + 1;    
}

u16 Maps::GetRightIndex(u16 from)
{
    return from + 1;
}

u16 Maps::GetBottomRightIndex(u16 from)
{
    return from + world.w() + 1;
}

u16 Maps::GetBottomIndex(u16 from)
{
    return from + world.w();
}

u16 Maps::GetBottomLeftIndex(u16 from)
{
    return from + world.w() - 1;
}

u16 Maps::GetLeftIndex(u16 from)
{
    return from - 1;
}

u16 Maps::GetTopLeftIndex(u16 from)
{
    return from - world.w() - 1;
}
