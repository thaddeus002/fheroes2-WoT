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

#include "luck.h"

const std::string & Luck::String(Luck::luck_t luck)
{
    static const std::string str_luck[] = { "Cursed", "Awful", "Bad", "Normal", "Good", "Great", "Irish" };

    switch(luck){
        case Luck::CURSED:	return str_luck[0];
        case Luck::AWFUL:	return str_luck[1];
        case Luck::BAD:		return str_luck[2];
        case Luck::NORMAL:	return str_luck[3];
        case Luck::GOOD:	return str_luck[4];
        case Luck::GREAT:	return str_luck[5];
        case Luck::IRISH:	return str_luck[6];
    }

    return str_luck[0];
}
