/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include <sstream>
#include <iomanip>
#include <iostream>

#include "engine.h"
#include "serialize.h"

#define MINCAPACITY 1024
/*
#ifdef WITH_NET
#include "sdlnet.h"

Network::Socket & operator<< (Network::Socket & sc, StreamBuf & sb)
{
    const u32 count = sb.sizeg();

    if(sc.Send(count) && sc.Send(sb.itget, count))
	sb.itget += count;

    return sc;
}

Network::Socket & operator>> (Network::Socket & sc, StreamBuf & sb)
{
    u32 count = 0;
    const u32 limit = 5 * 1024 * 1024;

    if(sc.Recv(count))
    {
	// limit network: 5Mb
	if(count > limit)
	{
	    sb.setfail();
	    return sc;
	}

	if(sb.sizep() < count)
	    sb.realloc(count);

	if(sc.Recv(sb.itput, count))
	    sb.itput += count;
	else
	    sb.setfail();
    }
    else
	sb.setfail();

    return sc;
}
#endif                                                                                                           
*/

bool StreamBase::bigendian(void) const
{
    return flags & 0x80000000;
}

void StreamBase::setbigendian(bool f)
{
    if(f)                                
        flags |= 0x80000000;             
    else
        flags &= ~0x80000000;
}

bool StreamBase::fail(void) const
{
    return flags & 0x00000001;
}   

void StreamBase::setfail(void)
{
    flags |= 0x00000001;
}

long int StreamBase::tell(void) const
{
    return SDL_RWtell(rw);
}

bool StreamBase::seek(long int pos)
{
    if(0 > SDL_RWseek(rw, pos, RW_SEEK_SET)) setfail();
    return ! fail();
}

int StreamBase::get8(void)
{
    u8 ch;
    if(0 == SDL_RWread(rw, & ch, 1, 1)) setfail();
    return ch;
}

void StreamBase::put8(u8 ch)
{
    resize(1);
    if(1 != SDL_RWwrite(rw, & ch, 1, 1)) setfail();
}

int StreamBase::getBE16(void)
{
    return SDL_ReadBE16(rw);
}

int StreamBase::getLE16(void)
{
    return SDL_ReadLE16(rw);
}

int StreamBase::getBE32(void)
{
    return SDL_ReadBE32(rw);
}

int StreamBase::getLE32(void)
{
    return SDL_ReadLE32(rw);
}

int StreamBase::get16(void)
{
    return bigendian() ? getBE16() : getLE16();
}

int StreamBase::get32(void)
{
    return bigendian() ? getBE32() : getLE32();
}

void StreamBase::putBE32(u32 val)
{
    resize(4);
    if(0 == SDL_WriteBE32(rw, val)) setfail();
}

void StreamBase::putLE32(u32 val)
{
    resize(4);
    if(0 == SDL_WriteLE32(rw, val)) setfail();
}

void StreamBase::putBE16(u16 val)
{
    resize(2);
    if(0 == SDL_WriteBE16(rw, val)) setfail();
}

void StreamBase::putLE16(u16 val)
{
    resize(2);
    if(0 == SDL_WriteLE16(rw, val)) setfail();
}

void StreamBase::put16(u16 v)
{
    bigendian() ? putBE16(v) : putLE16(v);
}
 
void StreamBase::put32(u32 v)
{
    bigendian() ? putBE32(v) : putLE32(v);
}

std::vector<u8> StreamBase::getRaw(size_t sz)
{
    std::vector<u8> v(sz, 0);
    if(0 == SDL_RWread(rw, & v[0], v.size(), 1)) setfail();
 
    return v;
}
 
void StreamBase::putRaw(const void* ptr, size_t sz)
{
    resize(sz);
    if(1 != SDL_RWwrite(rw, ptr, sz, 1)) setfail();
}

void StreamBase::skip(size_t sz)
{
    if( 0 > SDL_RWseek(rw, sz, RW_SEEK_CUR)) setfail();
}

StreamBase & StreamBase::operator>> (bool & v)
{
    v = get8();
    return *this;
}

StreamBase & StreamBase::operator>> (char & v)
{
    v = get8();
    return *this;
}

StreamBase & StreamBase::operator>> (u8 & v)
{
    v = get8();
    return *this;
}

StreamBase & StreamBase::operator>> (s8 & v)
{
    v = get8();
    return *this;
}

StreamBase & StreamBase::operator>> (u16 & v)
{
    v = get16();
    return *this;
}

StreamBase & StreamBase::operator>> (s16 & v)
{
    v = get16();
    return *this;
}

StreamBase & StreamBase::operator>> (u32 & v)
{
    v = get32();
    return *this;
}

StreamBase & StreamBase::operator>> (s32 & v)
{
    v = get32();
    return *this;
}

StreamBase & StreamBase::operator>> (float & v)
{
    s32 intpart;
    s32 decpart;
    *this >> intpart >> decpart;
    v = intpart + decpart / 100000000;
    return *this;
}

StreamBase & StreamBase::operator>> (std::string & v)
{
    u32 size = get32();
    v.resize(size);

    for(std::string::iterator
        it = v.begin(); it != v.end(); ++it) *it = get8();

    return *this;
}

StreamBase & StreamBase::operator>> (Rect & v)
{
    Point & p = v;
    Size  & s = v;

    return *this >> p >> s;
}

StreamBase & StreamBase::operator>> (Point& v)
{
    return *this >> v.x >> v.y;
}

StreamBase & StreamBase::operator>> (Size & v)
{
    return *this >> v.w >> v.h;
}

StreamBase & StreamBase::operator<< (const bool & v)
{
    put8(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const char & v)
{
    put8(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const u8 & v)
{
    put8(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const s8 & v)
{
    put8(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const u16 & v)
{
    put16(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const s16 & v)
{
    put16(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const s32 & v)
{
    put32(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const u32 & v)
{
    put32(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const float & v)
{
    s32 intpart = static_cast<s32>(v);
    float decpart = (v - intpart) * 100000000;
    return *this << intpart << static_cast<s32>(decpart);
}

StreamBase & StreamBase::operator<< (const std::string & v)
{
    put32(v.size());

    for(std::string::const_iterator
        it = v.begin(); it != v.end(); ++it) put8(*it);

    return *this;
}

StreamBase & StreamBase::operator<< (const Point & v)
{
    return *this << v.x << v.y;
}

StreamBase & StreamBase::operator<< (const Rect & v)
{
    const Point & p = v;
    const Size  & s = v;

    return *this << p << s;
}

StreamBase & StreamBase::operator<< (const Size & v)
{
    return *this << v.w << v.h;
}

StreamFile::StreamFile(const std::string & fn, const char* mode)
{
    open(fn, mode);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    setbigendian(true);
#else
    setbigendian(false);
#endif
}

StreamFile::~StreamFile()
{
    close();
}

bool StreamFile::open(const std::string & fn, const char* mode)
{
    rw = SDL_RWFromFile(fn.c_str(), mode);
    if(! rw) ERROR(SDL_GetError());
    return rw;
}

void StreamFile::close(void)
{
    if(rw) SDL_RWclose(rw);
    rw = NULL;
}

long int StreamFile::size(void) const
{
    long int pos = SDL_RWtell(rw);
    long int res = SDL_RWseek(rw, 0, SEEK_END);
    SDL_RWseek(rw, pos, RW_SEEK_SET);
    return res;
}

bool StreamFile::read(void* buf, size_t sz)
{
    if(0 == SDL_RWread(rw, buf, sz, 1)) setfail();
    return ! fail();
}

StreamBuf StreamFile::toStreamBuf(void)
{
    StreamBuf buf(size() - tell());
    read(buf.data(), buf.size());
    return buf;
}

StreamBuf::StreamBuf(const u8* ptr, size_t sz) : buf(NULL), len(0)
{
    buf = const_cast<u8*>(ptr);
    len = sz;

    rw = SDL_RWFromConstMem(buf, len);
    if(! rw) ERROR(SDL_GetError());

    setconstbuf(true);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    setbigendian(true);
#else
    setbigendian(false);
#endif
}

StreamBuf::StreamBuf(const std::vector<u8> & v) : buf(NULL), len(0)
{
    buf = const_cast<u8*>(& v[0]);
    len = v.size();

    rw = SDL_RWFromConstMem(buf, len);
    if(! rw) ERROR(SDL_GetError());

    setconstbuf(true);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    setbigendian(true);
#else
    setbigendian(false);
#endif
}

StreamBuf::StreamBuf(size_t sz) : buf(NULL), len(sz)
{
    if(len)
    {
	if(len < MINCAPACITY) len = MINCAPACITY;
	buf = new u8 [len];
	rw = SDL_RWFromMem(buf, len);
	if(! rw) ERROR(SDL_GetError());
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    setbigendian(true);
#else
    setbigendian(false);
#endif
}

StreamBuf::StreamBuf(const StreamBuf & sb) : buf(NULL), len(0)
{
    if(sb.len)
    {
	len = sb.len;
	buf = new u8 [len];
	flags = sb.flags;

	std::copy(sb.buf, sb.buf + len, buf);

        rw = SDL_RWFromMem(buf, len);
	SDL_RWseek(rw, SDL_RWtell(sb.rw), RW_SEEK_SET);
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    setbigendian(true);
#else
    setbigendian(false);
#endif
}

StreamBuf & StreamBuf::operator= (const StreamBuf & sb)
{
    clear();

    if(sb.len)
    {
        len = sb.len;
	buf = new u8 [len];
	flags = sb.flags;

        std::copy(sb.buf, sb.buf + len, buf);

	rw = SDL_RWFromMem(buf, len);
        SDL_RWseek(rw, SDL_RWtell(sb.rw), RW_SEEK_SET);
    }

    return *this;
}

StreamBuf::~StreamBuf()
{
    clear();
}

void StreamBuf::clear(void)
{
    if(buf && !isconstbuf())
	delete [] buf;
    if(rw) SDL_RWclose(rw);
    len = 0;
}

void StreamBuf::setconstbuf(bool f)
{
    if(f)
        flags |= 0x00001000;
    else
        flags &= ~0x00001000;
}

bool StreamBuf::isconstbuf(void) const
{
    return flags & 0x00001000;
}

void StreamBuf::resize(size_t sz)
{
    if(SDL_RWtell(rw) + sz > len)
    {
	if(sz < MINCAPACITY) sz = MINCAPACITY;
	u8* ptr = new u8 [len + sz];

	std::fill(ptr, ptr + len + sz, 0);
	std::copy(buf, buf + len, ptr);

	long int pos = SDL_RWtell(rw);
	SDL_RWclose(rw);
	delete [] buf;

	buf = ptr;
	len += sz;

	rw = SDL_RWFromMem(buf, len);
	SDL_RWseek(rw, pos, RW_SEEK_SET);
    }
}
