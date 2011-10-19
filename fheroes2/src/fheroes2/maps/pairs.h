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

#ifndef H2PAIRS_H
#define H2PAIRS_H

#include <list>
#include <utility>
#include "mp2.h"
#include "color.h"
#include "resource.h"

class IndexDistance : public std::pair<s32, u16>
{
    public:
    IndexDistance() : std::pair<s32, u16>(-1, 0) {};
    IndexDistance(const s32 & i, const u16 & d) : std::pair<s32, u16>(i, d) {};

    static bool Shortest(const IndexDistance & id1, const IndexDistance & id2){ return id1.second < id2.second; };
    static bool Longest(const IndexDistance & id1, const IndexDistance & id2){ return id1.second > id2.second; };
};

class IndexObject : public std::pair<s32, u8>
{
    public:
    IndexObject() : std::pair<s32, u8>(-1, MP2::OBJ_ZERO) {};
    IndexObject(const s32 & index, const u8 & object) : std::pair<s32, u8>(index, object) {};

    bool isIndex(s32 index) const { return index == first; };
    bool isObject(u8 object) const { return object == second; };
};

class ObjectColor : public std::pair<u8, u8>
{
    public:
    ObjectColor() : std::pair<u8, u8>(MP2::OBJ_ZERO, Color::NONE) {};
    ObjectColor(const u8 & object, const u8 & color) : std::pair<u8, u8>(object, color) {};

    bool isObject(u8 object) const { return object == first; };
    bool isColor(u8 colors) const { return colors & second; };
};

class ResourceCount : public std::pair<u8, u16>
{
    public:
    ResourceCount() : std::pair<u8, u16>(Resource::UNKNOWN, 0) {};
    ResourceCount(const u8 & res, const u16 & count) : std::pair<u8, u16>(res, count) {};

    bool isResource(u8 res) const { return res == first; };
    bool isValid(void) const { return (first & Resource::ALL) && second; };
};

/*
template<class T>
struct map_data_compare : public std::binary_function<typename T::value_type,
                                                      typename T::mapped_type, bool>
{
    bool operator() (const typename T::value_type & p, const typename T::mapped_type & i) const
    {
        return p.second == i;
    }
};
*/

#endif
