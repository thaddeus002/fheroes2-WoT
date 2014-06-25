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
#include <sstream>
#include <zlib.h>
#include "error.h"
#include "zzlib.h"

std::vector<u8> zlibDecompress(const u8* src, size_t srcsz, size_t realsz)
{
    std::vector<u8> res;

    if(src && srcsz)
    {
	if(realsz) res.reserve(realsz);
	res.resize((realsz ? realsz : srcsz * 7), 0);
	uLong dstsz = res.size();
	int ret = Z_BUF_ERROR;

	while(Z_BUF_ERROR ==
		(ret = uncompress(reinterpret_cast<Bytef*>(& res[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz)))
	{
	    dstsz = res.size() * 2;
	    res.resize(dstsz);
	}

	if(ret == Z_OK)
	    res.resize(dstsz);
	else
	{
	    res.clear();
	    std::ostringstream os;
	    os << "zlib error:" << ret;
	    Error::Message(__FUNCTION__, os.str().c_str());
	}
    }

    return res;
}

std::vector<u8> zlibCompress(const u8* src, size_t srcsz)
{
    std::vector<u8> res;

    if(src && srcsz)
    {
	res.resize(compressBound(srcsz));
        uLong dstsz = res.size();
        int ret = compress(reinterpret_cast<Bytef*>(& res[0]), &dstsz, reinterpret_cast<const Bytef*>(src), srcsz);

	if(ret == Z_OK)
	    res.resize(dstsz);
	else
	{
	    res.clear();
	    std::ostringstream os;
	    os << "zlib error:" << ret;
	    Error::Message(__FUNCTION__, os.str().c_str());
	}
    }

    return res;
}

bool ZSurface::Load(int w, int h, int bpp, int pitch, u32 rmask, u32 gmask, u32 bmask, u32 amask, const u8* p, size_t s)
{
    buf = zlibDecompress(p, s);

    if(! buf.empty())
    {
        SDL_Surface* sf = SDL_CreateRGBSurfaceFrom(&buf[0], w, h, bpp, pitch, rmask, gmask, bmask, amask);

        if(!sf)
            Error::Except(__FUNCTION__, SDL_GetError());

        Set(sf);
	return true;
    }

    return false;
}

ZStreamBuf::ZStreamBuf() : StreamBuf(0)
{
    setbigendian(true);
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
    const u32 size0 = sb.sizeg();
    std::vector<u8> v = zlibCompress(sb.itget, size0);

    if(! v.empty())
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
        std::vector<u8> v = zlibDecompress(itget, size1, size0);

        if(size1 <= sizeg() &&
            ! v.empty())
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
