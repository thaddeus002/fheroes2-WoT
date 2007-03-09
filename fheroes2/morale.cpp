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

#include "morale.h"

const std::string & Morale::String(Morale::morale_t morale)
{
    static const std::string treason("Treason");
    static const std::string awful("Awful");
    static const std::string poor("Poor");
    static const std::string normal("Normal");
    static const std::string good("Good");
    static const std::string great("Great");
    static const std::string blood("Blood");

    switch(morale){
        case Morale::TREASON:	return treason;
        case Morale::AWFUL:	return awful;
        case Morale::POOR:	return poor;
        case Morale::NORMAL:	return normal;
        case Morale::GOOD:	return good;
        case Morale::GREAT:	return great;
        default: break;
    }

    return blood;
}
