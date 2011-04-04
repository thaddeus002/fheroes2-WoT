/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <string>
#include "rect.h"
#include "serialize.h"

Serialize & Serialize::put16(const u16 & v)
{
    put((v >> 8) & 0x00FF);
    put(v & 0x00FF);
    return *this;
}

Serialize & Serialize::put32(const u32 & v)
{
    put((v >> 24) & 0x000000FF);
    put((v >> 16) & 0x000000FF);
    put((v >> 8) & 0x000000FF);
    put(v & 0x000000FF);
    return *this;
}

Serialize & Serialize::get16(u16 & v)
{
    char ch;

    get(ch);
    v = ch;
    v <<= 8;

    get(ch);
    v |= 0x00FF & ch;

    return *this;
}

Serialize & Serialize::get32(u32 & v)
{
    char ch;

    get(ch);
    v = ch;
    v <<= 8;

    get(ch);
    v |= 0x000000FF & ch;
    v <<= 8;

    get(ch);
    v |= 0x000000FF & ch;
    v <<= 8;

    get(ch);
    v |= 0x000000FF & ch;

    return *this;
}

Serialize & Serialize::operator<< (const bool & v)
{
    put(v);
    return *this;
}

Serialize & Serialize::operator<< (const u8 & v)
{
    put(v);
    return *this;
}

Serialize & Serialize::operator<< (const s8 & v)
{
    put(v);
    return *this;
}

Serialize & Serialize::operator<< (const u16 & v)
{
    return put16(v);
}
Serialize & Serialize::operator<< (const s16 & v)
{
    return put16(v);
}

Serialize & Serialize::operator<< (const s32 & v)
{
    return put32(v);
}

Serialize & Serialize::operator<< (const u32 & v)
{
    return put32(v);
}

Serialize & Serialize::operator<< (const std::string & v)
{
    put32(v.size());

    for(std::string::const_iterator
        it = v.begin(); it != v.end(); ++it) put(*it);

    return *this;
}

Serialize & Serialize::operator<< (const Point & v)
{
    return *this << v.x << v.y;
}

Serialize & Serialize::operator<< (const Rect & v)
{
    const Point & p = v;
    const Size  & s = v;

    return *this << p << s;
}

Serialize & Serialize::operator<< (const Size & v)
{
    return *this << v.w << v.h;
}

Serialize & Serialize::operator>> (bool & v)
{
    char ch;
    get(ch);
    v = ch;
    return *this;
}

Serialize & Serialize::operator>> (u8 & v)
{
    char ch;
    get(ch);
    v = ch;
    return *this;
}

Serialize & Serialize::operator>> (s8 & v)
{
    char ch;
    get(ch);
    v = ch;
    return *this;
}

Serialize & Serialize::operator>> (u16 & v)
{
    return get16(v);
}

Serialize & Serialize::operator>> (s16 & v)
{
    u16 rs;
    get16(rs);
    v = rs;
    return *this;
}

Serialize & Serialize::operator>> (u32 & v)
{
    return get32(v);
}

Serialize & Serialize::operator>> (s32 & v)
{
    u32 rs;
    get32(rs);
    v = rs;
    return *this;
}

Serialize & Serialize::operator>> (std::string & v)
{
    u32 size;
    get32(size);
    v.resize(size);

    for(std::string::iterator
        it = v.begin(); it != v.end(); ++it) get(*it);

    return *this;
}

Serialize & Serialize::operator>> (Rect & v)
{
    Point & p = v;
    Size  & s = v;

    return *this >> p >> s;
}

Serialize & Serialize::operator>> (Point& v)
{
    return *this >> v.x >> v.y;
}

Serialize & Serialize::operator>> (Size & v)
{
    return *this >> v.w >> v.h;
}
