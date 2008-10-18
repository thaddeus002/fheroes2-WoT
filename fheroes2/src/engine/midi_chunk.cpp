/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include <cstring>
#include <iostream>
#include "midi_chunk.h"

using namespace MIDI;

Chunk::Chunk() : size(0), data(NULL)
{
    memset(id, '\0', 4);
}

Chunk::Chunk(const char *i, const u32 s, const char *p) : size(s), data(NULL)
{
    i ? memcpy(id, i, 4) : memset(id, '\0', 4);

    if(size)
    {
	data = new char[size];

	if(p) memcpy(data, p, size); else memset(data, '\0', size);
    }
}

Chunk::Chunk(std::istream & i) : size(0), data(NULL)
{
    memset(id, '\0', 4);
    Read(i);
}

Chunk::Chunk(const char *p) : size(0), data(NULL)
{
    memset(id, '\0', 4);
    Read(p);
}

Chunk::Chunk(const Chunk & c) : size(c.size), data(NULL)
{
    c.id ? memcpy(id, c.id, 4) : memset(id, '\0', 4);

    if(size)
    {
	data = new char [size];
	memcpy(data, c.data, size);
    }
}

Chunk::~Chunk()
{
    if(data) delete [] data;
}

Chunk::Chunk & Chunk::operator= (const Chunk & c)
{
    if(data) delete [] data;
    data = NULL;

    c.id ? memcpy(id, c.id, 4) : memset(id, '\0', 4);
    size = c.size;

    if(size)
    {
	data = new char [size];
	memcpy(data, c.data, size);
    }

    return *this;
}

bool Chunk::Read(std::istream & i)
{
    if(i.fail()) return false;

    i.read(id, 4);

    u32 x;
    i.read(reinterpret_cast<char *>(&x), 4);
#if __BYTE_ORDER == __LITTLE_ENDIAN
    size = MIDI::Swap32(x);
#else
    size = x;
#endif

    if(data) delete [] data;
    data = NULL;

    if(size)
    {
	data = new char [size];
	i.read(data, size);
    }

    return true;
}

bool Chunk::Read(const std::vector<char> & b)
{
    if(8 > b.size()) return false;

    memcpy(id, &b[0], 4);

    size = MIDI::ReadBE32(&b[4]);

    if(data) delete [] data;
    data = NULL;

    if(size + 8 > b.size()) size = b.size() - 8;

    if(size)
    {
	data = new char [size];
	memcpy(data, &b[8], size);
    }

    return true;
}

bool Chunk::Read(const char *p)
{
    if(NULL == p) return false;

    memcpy(id, p, 4);

    size = MIDI::ReadBE32(&p[4]);

    if(data) delete [] data;
    data = NULL;

    if(size)
    {
	data = new char [size];
	memcpy(data, &p[8], size);
    }

    return true;
}

bool Chunk::Write(std::ostream & o) const
{
    if(o.fail()) return false;

    o.write(id, 4);

    u32 x;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    x = MIDI::Swap32(size);
#else
    x = size;
#endif
    o.write(reinterpret_cast<char *>(&x), 4);

    if(size && data) o.write(data, size);

    return true;
}

bool Chunk::Write(char *p) const
{
    if(NULL == p) return false;

    memcpy(p, id, 4);

    MIDI::WriteBE32(&p[4], size);

    if(size && data) memcpy(&p[8], data, size);

    return true;
}

void Chunk::Dump(void) const
{
    std::cout << "id:   ";
    std::cout.write(id, 4);
    std::cout << std::endl << "size: " << size << std::endl << "data: " << (data ? "[binary]" : "NULL" ) << std::endl;
}
