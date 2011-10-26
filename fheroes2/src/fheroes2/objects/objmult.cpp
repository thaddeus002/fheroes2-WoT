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

#include <algorithm>
#include "icn.h"
#include "direction.h"
#include "objmult.h"

u16 ObjMult::GetPassable(const u8 & index)
{
    const u8 restricted[] = { 2, 4, 58, 63, 64, 65, 70, 72, 73, 89, 104 };

    if(isShadow(index))
        return DIRECTION_ALL;
    else
    if(isAction(index))
        return 0;

    return ARRAY_COUNT_END(restricted) != std::find(restricted, ARRAY_COUNT_END(restricted), index) ?
            DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW : DIRECTION_ALL;
}

bool ObjMult::isAction(const u8 & index)
{
/*
    peasant hut: 35
    fort: 59
    gasebo: 62
    witch hut: 69
    mercenary camp: 71
    ruins: 74
    shrine: 76, 78, 80
    idol: 82
    standing stones: 84, 85
    temple: 88
    market: 111
    tree house: 114
    watch tower: 116
    tree knowledge: 123
*/
    const u8 actions[] = { 35, 59, 62, 69, 71, 74, 76, 78, 80, 82, 84, 85, 88, 111, 114, 116, 123 };

    return ARRAY_COUNT_END(actions) != std::find(actions, ARRAY_COUNT_END(actions), index);
}

bool ObjMult::isShadow(const u8 & index)
{
    const u8 shadows2[] = { 1, 3, 15, 25, 45, 54, 57, 61, 67, 68, 75, 77, 79, 81, 83,
		    97, 98, 105, 113, 115, 121, 122, 124 };

    return ARRAY_COUNT_END(shadows2) != std::find(shadows2, ARRAY_COUNT_END(shadows2), index);
}

u16 ObjMul2::GetPassable(const u8 & index)
{
    const u8 disabled[] = { 46, 76, 77, 124, 125, 126, 221, 213 };
    const u8 restricted[] = { 16, 18, 19, 25, 27, 51, 52, 53, 55, 57, 78, 79, 81, 98, 105, 128, 136, 187, 207, 209, 214, 215, 217 };

    if(isShadow(index))
        return DIRECTION_ALL;
    else
    if(isAction(index) ||
        ARRAY_COUNT_END(disabled) != std::find(disabled, ARRAY_COUNT_END(disabled), index))
        return 0;

    return ARRAY_COUNT_END(restricted) != std::find(restricted, ARRAY_COUNT_END(restricted), index) ?
            DIRECTION_CENTER_ROW | DIRECTION_BOTTOM_ROW : DIRECTION_ALL;
}

bool ObjMul2::isAction(const u8 & index)
{
/*
    fountain: 15
    alchemy tower: 26
    dragon city: 54
    grave yard: 58
    light house: 73
    saw mill: 80
    water well: 112
    sign: 114
    teleport: 116, 119, 122
    wagon camp: 129
    well: 162, 165
    freeman foundry: 188
    magick garden: 190
    observation tower: 201
    grave yard: 208
    saw mill: 216
*/
    const u8 actions[] = { 15, 26, 54, 58, 73, 80, 112, 114, 116, 119, 122, 129, 162, 165,
	188, 190, 201, 208, 216 };

    return ARRAY_COUNT_END(actions) != std::find(actions, ARRAY_COUNT_END(actions), index);
}

bool ObjMul2::isShadow(const u8 & index)
{
    const u8 shadows1[] = { 14, 17, 20, 24, 34, 36, 42, 43, 49, 50, 60, 71, 72, 113, 115, 118, 121, 123, 127,
		    161, 164, 180, 181, 189, 199, 200, 202, 206 };

    return ARRAY_COUNT_END(shadows1) != std::find(shadows1, ARRAY_COUNT_END(shadows1), index);
}
