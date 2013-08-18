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

bool ZLibUnCompress(const char* src, size_t srcsz, std::vector<char> & dst)
{
    int res = Z_BUF_ERROR;

    if(src && srcsz)
    {
	uLong dstsz = dst.size();

	if(dstsz == 0)
	{
	    dstsz = srcsz * 7;
	    dst.resize(dstsz, 0);
	}

	while(Z_BUF_ERROR ==
		(res = uncompress(reinterpret_cast<Bytef*>(&dst[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz)))
	{
	    dstsz = dst.size() * 2;
	    dst.resize(dstsz);
	}

	if(res == Z_OK)
	    dst.resize(dstsz);
	else
	    dst.clear();
    }

    return res == Z_OK;
}

bool ZLibCompress(const char* src, size_t srcsz, std::vector<char> & dst)
{
    int res = Z_BUF_ERROR;

    if(src && srcsz)
    {
	dst.resize(compressBound(srcsz));
        uLong dstsz = dst.size();
        res = compress(reinterpret_cast<Bytef*>(&dst[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz);

	if(res == Z_OK)
	    dst.resize(dstsz);
	else
	    dst.clear();
    }

    return Z_OK == res;
}

bool ZSurface::Load(int w, int h, int bpp, int pitch, u32 rmask, u32 gmask, u32 bmask, u32 amask, const u8* p, size_t s)
{
    if(ZLibUnCompress(reinterpret_cast<const char*>(p), s, buf))
    {
	Set(SDL_CreateRGBSurfaceFrom(&buf[0], w, h, bpp, pitch, rmask, gmask, bmask, amask));
	return true;
    }

    return false;
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
        ZLibCompress(sb.itget, size0, v))
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
        const u32 size0 = get32();
        const u32 size1 = get32();
        std::vector<char> v(size0, 0);

        if(size1 <= sizeg() &&
            ZLibUnCompress(itget, size1, v))
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
