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

const char* Luck::String(s8 luck)
{
    const char* str_luck[] = { "Unknown", _("Cursed"), _("Awful"), _("Bad"), _("Normal"), _("Good"), _("Great"), _("Irish") };

    switch(luck)
    {
        case Luck::CURSED:	return str_luck[1];
        case Luck::AWFUL:	return str_luck[2];
        case Luck::BAD:		return str_luck[3];
        case Luck::NORMAL:	return str_luck[4];
        case Luck::GOOD:	return str_luck[5];
        case Luck::GREAT:	return str_luck[6];
        case Luck::IRISH:	return str_luck[7];
	default: break;
    }

    return str_luck[0];
}

const char* Luck::Description(s8 luck)
{
    const char* str_desc_luck[] = { "Unknown",
	_("Bad luck sometimes falls on your armies in combat, causing their attacks to only do half damage."),
	_("Neutral luck means your armies will never get lucky or unlucky attacks on the enemy."),
	_("Good luck sometimes lets your armies get lucky attacks (double strength) in combat.") };

    switch(luck)
    {
        case Luck::CURSED:
        case Luck::AWFUL:
        case Luck::BAD:		return str_desc_luck[1];
        case Luck::NORMAL:	return str_desc_luck[2];
        case Luck::GOOD:
        case Luck::GREAT:
        case Luck::IRISH:	return str_desc_luck[3];
	default: break;
    }

    return str_desc_luck[0];
}
