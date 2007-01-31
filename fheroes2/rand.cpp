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

#include "rand.h"


void Rand::Init(void){ std::srand((u32) std::time(0)); }

u32 Rand::Get(u32 min, u32 max){ return max ? min + (std::rand() % (max - min + 1)) : std::rand() % (min + 1); }

Kingdom::race_t Rand::Race(void)
{
    switch(Rand::Get(1, 6)){
	case 1: return Kingdom::KNGT;
	case 2: return Kingdom::BARB;
	case 3: return Kingdom::SORC;
	case 4: return Kingdom::WRLK;
	case 5: return Kingdom::WZRD;
	default: break;
    }

    return Kingdom::NECR;
}
