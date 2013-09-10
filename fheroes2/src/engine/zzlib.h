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

#ifndef H2ZLIB_H
#define H2ZLIB_H

#ifdef WITH_ZLIB

#include <vector>
#include <iostream>
#include "types.h"
#include "surface.h"
#include "serialize.h"

class ZSurface : public Surface
{
public:
    ZSurface(){}

    bool Load(int w, int h, int b, int pitch, u32 rmask, u32 gmask, u32 bmask, u32 amask, const u8* p, size_t s);

private:
    std::vector<char> buf;
};

class ZStreamBuf : protected StreamBuf
{
public:
    ZStreamBuf() : StreamBuf(0) {}

    ZStreamBuf & operator<< (StreamBuf &);
    ZStreamBuf & operator>> (StreamBuf &);

    bool        fail(void) const { return StreamBuf::fail(); }
    void        setlimit(size_t v) { return StreamBuf::setlimit(v); }

protected:
    friend std::ostream & operator<< (std::ostream &, ZStreamBuf &);
    friend std::istream & operator>> (std::istream &, ZStreamBuf &);
};

std::ostream & operator<< (std::ostream &, ZStreamBuf &);
std::istream & operator>> (std::istream &, ZStreamBuf &);

std::vector<char> zlibCompress(const char* src, size_t srcsz);
std::vector<char> zlibDecompress(const char* src, size_t srcsz, size_t realsz = 0);

#endif
#endif
