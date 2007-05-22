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
    static const std::string str_morale[] = { "Treason", "Awful", "Poor", "Normal", "Good", "Great", "Blood" };

    switch(morale){
        case Morale::TREASON:	return str_morale[0];
        case Morale::AWFUL:	return str_morale[1];
        case Morale::POOR:	return str_morale[2];
        case Morale::NORMAL:	return str_morale[3];
        case Morale::GOOD:	return str_morale[4];
        case Morale::GREAT:	return str_morale[5];
        case Morale::BLOOD:	return str_morale[6];
    }

    return str_morale[0];
}
