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

const std::string & Morale::String(s8 morale)
{
    static const std::string str_morale[] = { "Unknown", "Treason", "Awful", "Poor", "Normal", "Good", "Great", "Blood" };

    switch(morale)
    {
        case Morale::TREASON:	return str_morale[1];
        case Morale::AWFUL:	return str_morale[2];
        case Morale::POOR:	return str_morale[3];
        case Morale::NORMAL:	return str_morale[4];
        case Morale::GOOD:	return str_morale[5];
        case Morale::GREAT:	return str_morale[6];
        case Morale::BLOOD:	return str_morale[7];
	default: break;
    }

    return str_morale[0];
}

const std::string & Morale::Description(s8 morale)
{
    static const std::string str_desc_morale[] = { "Unknown",
	"Bad morale may cause your armies to freeze in combat.",
	"Neutral morale means your armies will never be blessed with extra attacks or freeze in combat.",
	"Good morale may give your armies extra attacks in combat." };

    switch(morale)
    {
        case Morale::TREASON:
        case Morale::AWFUL:
        case Morale::POOR:	return str_desc_morale[1];
        case Morale::NORMAL:	return str_desc_morale[2];
        case Morale::GOOD:
        case Morale::GREAT:
        case Morale::BLOOD:	return str_desc_morale[3];
	default: break;
    }

    return str_desc_morale[0];
}
