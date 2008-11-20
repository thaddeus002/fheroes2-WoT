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

#include <cstdlib>
#include <ctime>
#include <iterator>
#include "rand.h"


void Rand::Init(void){ std::srand((u32) std::time(0)); }

u32 Rand::Get(u32 min, u32 max){ return max ? min + (std::rand() % (max - min + 1)) : std::rand() % (min + 1); }

Rand::Queue::Queue(const u8 size) : max(0)
{
    reserve(size);
}

void Rand::Queue::Reset(void)
{
    max = 0;
    clear();
}

void Rand::Queue::Push(const u8 quote)
{
    push_back(quote);
    max += quote;
}

size_t Rand::Queue::Get(void) const
{
    const u8 value = Rand::Get(1, max);
    u8 amount = 0;

    const_iterator it1 = begin();
    const_iterator it2 = end();

    for(; it1 != it2; ++it1)
    {
        amount += *it1;
        if(value <= amount) break;
    }

    return std::distance(begin(), it1);
}
