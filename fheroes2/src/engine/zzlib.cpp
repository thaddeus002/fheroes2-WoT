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


#ifdef WITH_ZLIB
#include <zlib.h>
#include "zzlib.h"

bool ZLib::UnCompress(std::vector<char> & dst, const char* src, size_t srcsz, bool debug)
{
    if(src && srcsz)
    {
	uLong dstsz = srcsz * 20;
	dst.resize(dstsz);
	int res = 0;
	while(Z_BUF_ERROR == (res = uncompress(reinterpret_cast<Bytef*>(&dst[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz)))
	{ dstsz = dst.size() * 2; dst.resize(dstsz); }
	dst.resize(dstsz);
	
	switch(res)
	{
	    case Z_OK:  return true;
	    case Z_MEM_ERROR: if(debug) std::cerr << "ZLib::UnCompress: " << "Z_MEM_ERROR" << std::endl; return false;
	    case Z_BUF_ERROR: if(debug) std::cerr << "ZLib::UnCompress: " << "Z_BUF_ERROR" << std::endl; return false;
	    case Z_DATA_ERROR:if(debug) std::cerr << "ZLib::UnCompress: " << "Z_DATA_ERROR"<< std::endl; return false;
	    default: break;
	}

        return Z_OK == res;
    }
    return false;
}

bool ZLib::Compress(std::vector<char> & dst, const char* src, size_t srcsz)
{
    if(src && srcsz)
    {
	dst.resize(compressBound(srcsz));
        uLong dstsz = dst.size();
        int res = compress(reinterpret_cast<Bytef*>(&dst[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz);
        dst.resize(dstsz);
        return Z_OK == res;
    }
    return false;
}

bool ZSurface::Load(u16 w, u16 h, u8 bpp, u16 pitch, u32 rmask, u32 gmask, u32 bmask, u32 amask, const u8* p, size_t s)
{
    if(!ZLib::UnCompress(buf, reinterpret_cast<const char*>(p), s)) return false;
    Set(SDL_CreateRGBSurfaceFrom(&buf[0], w, h, bpp, pitch, rmask, gmask, bmask, amask));
    return true;
}

std::ostream & operator<< (std::ostream & os, ZStreamBuf & zb)
{
    return os << static_cast<StreamBuf &>(zb);
}

std::istream & operator>> (std::istream & is, ZStreamBuf & zb)
{
    return is >> static_cast<StreamBuf &>(zb);
}

ZStreamBuf & ZStreamBuf::operator<< (StreamBuf & sb)
{
    std::vector<char> v;
    const u32 size0 = sb.sizeg();

    if(size0 &&
        ZLib::Compress(v, sb.itget, size0))
    {
        const u32 size1 = v.size();
        sb.itget += size0;

        if(sizep() < size1 + 8)
            realloc(size1 + 8);

        put32(size0);
        put32(size1);
        std::copy(v.begin(), v.end(), itput);
        itput += size1;
    }
    else
        setfail();

    return *this;
}

ZStreamBuf & ZStreamBuf::operator>> (StreamBuf & sb)
{
    if(sizeg() > 8)
    {
        std::vector<char> v;
        const u32 size0 = get32();
        const u32 size1 = get32();
        v.reserve(size0);

        if(size1 <= sizeg() &&
            ZLib::UnCompress(v, itget, size1))
        {
            itget += size1;

            if(sb.sizep() < v.size())
                sb.realloc(v.size());

            std::copy(v.begin(), v.end(), sb.itput);
            sb.itput += v.size();
        }
        else
            sb.setfail();
    }
    else
        sb.setfail();

    return *this;
}

#endif
