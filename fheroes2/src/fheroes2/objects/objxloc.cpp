/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include "icn.h"
#include "direction.h"
#include "objxloc.h"

u16 ObjXlc1::GetPassable(const u8 & index)
{
    const u8 disabled[] = { 40, 49, 50 };
    const u8 restricted[] = { 69, 71, 75, 76,  85, 103, 117, 119, 126, 128, 134, 136 };

    if(isShadow(index))
        return DIRECTION_ALL;
    else
    if(isAction(index) ||
        ARRAY_COUNT_END(disabled) != std::find(disabled, ARRAY_COUNT_END(disabled), index))
        return 0;

    return ARRAY_COUNT_END(restricted) != std::find(restricted, ARRAY_COUNT_END(restricted), index) ?
            DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW : DIRECTION_ALL;
}

bool ObjXlc1::isAction(const u8 & index)
{
    /*
	alchemist tower: 3
	arena: 70
	barrow mounds: 77
	eath altar: 94
	air altar: 118
	fire altar: 127
	water altar: 135
    */
    const u8 actions[] = { 3, 70, 77, 94, 118, 127, 135 };

    return ARRAY_COUNT_END(actions) != std::find(actions, ARRAY_COUNT_END(actions), index);
}

bool ObjXlc1::isShadow(const u8 & index)
{
    const u8 shadows [] = { 1, 2, 59, 68, 72, 78, 79, 83, 84, 112, 116, 120, 124, 125, 129, 133 };

    return ARRAY_COUNT_END(shadows) != std::find(shadows, ARRAY_COUNT_END(shadows), index);
}

u16 ObjXlc2::GetPassable(const u8 & index)
{
    const u8 restricted[] = { 3, 8, 28, 46, 92, 102 };

    if(isShadow(index))
        return DIRECTION_ALL;
    else
    if(isAction(index) ||
	(110 < index && index < 136))
	return 0;

    return ARRAY_COUNT_END(restricted) != std::find(restricted, ARRAY_COUNT_END(restricted), index) ?
            DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW : DIRECTION_ALL;
}

bool ObjXlc2::isAction(const u8 & index)
{
    /*
	stables: 4
	jail: 9
	mermaid: 37
	sirens: 101
    */
    const u8 actions[] = { 4, 9, 37, 101 };

    return ARRAY_COUNT_END(actions) != std::find(actions, ARRAY_COUNT_END(actions), index);
}

bool ObjXlc2::isShadow(const u8 & index)
{
    const u8 shadows [] = { 2, 10, 47, 83 };

    return ARRAY_COUNT_END(shadows) != std::find(shadows, ARRAY_COUNT_END(shadows), index);
}


u16 ObjXlc3::GetPassable(const u8 & index)
{
    if(isShadow(index))
        return DIRECTION_ALL;
    else
    if(isAction(index))
        return 0;

    return DIRECTION_ALL;
}

bool ObjXlc3::isAction(const u8 & index)
{
    /*
	hut magi: 30
	eyes magi: 50
	barrier: 60, 66, 72, 78, 84, 90, 96, 102
	traveller tent: 110, 114, 118, 122, 126, 130, 134, 138
    */
    const u8 actions[] = { 30, 50, 60, 66, 72, 78, 84, 90, 96, 102, 110, 114, 118, 122, 126, 130, 134, 138 };

    return ARRAY_COUNT_END(actions) != std::find(actions, ARRAY_COUNT_END(actions), index);
}

bool ObjXlc3::isShadow(const u8 & index)
{
    const u8 shadows [] = { 0, 9, 20, 29, 41, 59, 65, 71, 77, 83, 89, 95, 101,
	108, 109, 112, 113, 116, 117, 120, 121, 124, 125, 128, 129, 132, 133, 136, 137 };

    return ARRAY_COUNT_END(shadows) != std::find(shadows, ARRAY_COUNT_END(shadows), index);
}
