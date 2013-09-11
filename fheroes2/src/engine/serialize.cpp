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
#include "rect.h"
#include "zzlib.h"
#include "serialize.h"

#define MINCAPACITY 12

int StreamBase::get(void)
{
    char res = 0;
    get(res);
    return 0x000000FF & res;
}

bool StreamBase::bigendian(void) const
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return true;
#else
    return false;
#endif
}

int StreamBase::getBE16(void)
{
    return (get() << 8) | get();
}

int StreamBase::getLE16(void)
{
    return get() | (get() << 8);
}

int StreamBase::getBE32(void)
{
    return (get() << 24) | (get() << 16) | (get() << 8) | get();
}

int StreamBase::getLE32(void)
{
    return get() | (get() << 8) | (get() << 16) | (get() << 24);
}

void StreamBase::get16(u16 & v)
{
    if(sizeg() > 1)
	v = get16();
}

int StreamBase::get16(void)
{
    return bigendian() ? getBE16() : getLE16();
}

void StreamBase::get32(u32 & v)
{
    if(sizeg() > 3)
	v = get32();
}

int StreamBase::get32(void)
{
    return bigendian() ? getBE32() : getLE32();
}

int StreamBase::getBE32(std::istream & is)
{
    return (is.get() << 24) | (is.get() << 16) | (is.get() << 8) | is.get();
}

int StreamBase::getLE32(std::istream & is)
{
    return is.get() | (is.get() << 8) | (is.get() << 16) | (is.get() << 24);
}

int StreamBase::getBE16(std::istream & is)
{
    return (is.get() << 8) | is.get();
}

int StreamBase::getLE16(std::istream & is)
{
    return is.get() | (is.get() << 8);
}

std::vector<u8> StreamBase::getRaw(size_t sz)
{
    std::vector<u8> v(sz, 0);

    for(std::vector<u8>::iterator
        it = v.begin(); it != v.end(); ++it) *this >> *it;

    return v;
}

void StreamBase::putRaw(const char* ptr, size_t sz)
{
    for(size_t it = 0; it < sz; ++it)
	*this << ptr[it];
}

void StreamBase::skip(size_t sz)
{
    char res = 0;
    for(size_t it = 0; it < sz; ++it) get(res);
}

StreamBase & StreamBase::operator>> (bool & v)
{
    v = get();
    return *this;
}

StreamBase & StreamBase::operator>> (u8 & v)
{
    v = get();
    return *this;
}

StreamBase & StreamBase::operator>> (s8 & v)
{
    v = get();
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
        it = v.begin(); it != v.end(); ++it) get(*it);

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

void StreamBase::put16(const u16 & v)
{
    bigendian() ? putBE16(v) : putLE16(v);
}

void StreamBase::putBE16(const u16 & v)
{
    if(sizep() > 1)
    {
	put((v >> 8) & 0x00FF);
	put(v & 0x00FF);
    }
}

void StreamBase::putLE16(const u16 & v)
{
    if(sizep() > 1)
    {
	put(v & 0x00FF);
	put((v >> 8) & 0x00FF);
    }
}

void StreamBase::put32(const u32 & v)
{
    bigendian() ? putBE32(v) : putLE32(v);
}

void StreamBase::putBE32(const u32 & v)
{
    if(sizep() > 3)
    {
	put((v >> 24) & 0x000000FF);
	put((v >> 16) & 0x000000FF);
        put((v >> 8) & 0x000000FF);
	put(v & 0x000000FF);
    }
}

void StreamBase::putLE32(const u32 & v)
{
    if(sizep() > 3)
    {
	put(v & 0x000000FF);
        put((v >> 8) & 0x000000FF);
	put((v >> 16) & 0x000000FF);
	put((v >> 24) & 0x000000FF);
    }
}

void StreamBase::putBE32(std::ostream & os, const u32 & v)
{
    os.put((v >> 24) & 0x000000FF);
    os.put((v >> 16) & 0x000000FF);
    os.put((v >> 8) & 0x000000FF);
    os.put(v & 0x000000FF);
}

void StreamBase::putLE32(std::ostream & os, const u32 & v)
{
    os.put(v & 0x000000FF);
    os.put((v >> 8) & 0x000000FF);
    os.put((v >> 16) & 0x000000FF);
    os.put((v >> 24) & 0x000000FF);
}

void StreamBase::putBE16(std::ostream & os, const u16 & v)
{
    os.put((v >> 8) & 0x00FF);
    os.put(v & 0x00FF);
}

void StreamBase::putLE16(std::ostream & os, const u16 & v)
{
    os.put(v & 0x00FF);
    os.put((v >> 8) & 0x00FF);
}

StreamBase & StreamBase::operator<< (const bool & v)
{
    put(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const char & v)
{
    put(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const u8 & v)
{
    put(v);
    return *this;
}

StreamBase & StreamBase::operator<< (const s8 & v)
{
    put(v);
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
        it = v.begin(); it != v.end(); ++it) put(*it);

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

StreamBuf::StreamBuf(size_t sz) : itbeg(NULL), itget(NULL), itput(NULL), itend(NULL), flags(0)
{
    realloc(sz);
    setbigendian(StreamBase::bigendian()); /* default: hardware endian */
}

StreamBuf::~StreamBuf()
{
    if(itbeg && ! isconstbuf()) delete [] itbeg;
}

StreamBuf::StreamBuf(const StreamBuf & st) : itbeg(NULL), itget(NULL), itput(NULL), itend(NULL), flags(0)
{
    copy(st);
}

StreamBuf::StreamBuf(const std::vector<u8> & buf) : itbeg(NULL), itget(NULL), itput(NULL), itend(NULL), flags(0)
{
    itbeg = (char*) & buf[0];
    itend = itbeg + buf.size();
    itget = itbeg;
    itput = itend;
    setbigendian(StreamBase::bigendian()); /* default: hardware endian */
    setconstbuf(true);
}

StreamBuf & StreamBuf::operator= (const StreamBuf & st)
{
    if(&st != this) copy(st);
    return *this;
}

size_t StreamBuf::capacity(void) const
{
    return itend - itbeg;
}

char* StreamBuf::data(void)
{
    return itget;
}

size_t StreamBuf::size(void) const
{
    return sizeg();
}

void StreamBuf::reset(void)
{
    itput = itbeg;
    itget = itbeg;
}

std::string StreamBuf::dump(void) const
{
    std::ostringstream os;

    for(const char* it = itget; it != itput; ++it)
	os << " 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*it);

    return os.str();
}

size_t StreamBuf::tellg(void) const
{
    return itget - itbeg;
}

size_t StreamBuf::tellp(void) const
{
    return itput - itbeg;
}

void StreamBuf::realloc(size_t sz)
{
    setconstbuf(false);

    if(! itbeg)
    {
	if(sz < MINCAPACITY) sz = MINCAPACITY;

	itbeg = new char [sz];
	itend = itbeg + sz;
    	std::fill(itbeg, itend, 0);

	reset();
    }
    else
    if(sizep() < sz)
    {
	sz = tellp() + sz;
	if(sz < MINCAPACITY) sz = MINCAPACITY;

	char* ptr = new char [sz];

	std::fill(ptr, ptr + sz, 0);
	std::copy(itbeg, itput, ptr);

	itput = ptr + tellp();
	itget = ptr + tellg();

	delete [] itbeg;

	itbeg = ptr;
	itend = itbeg + sz;
    }
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

bool StreamBuf::bigendian(void) const
{
    return flags & 0x80000000;
}

void StreamBuf::setbigendian(bool f)
{
    if(f)
	flags |= 0x80000000;
    else
	flags &= ~0x80000000;
}

bool StreamBuf::fail(void) const
{
    return flags & 0x00000001;
}

void StreamBuf::setfail(void)
{
    flags |= 0x00000001;
}

void StreamBuf::copy(const StreamBuf & sb)
{
    if(capacity() < sb.size())
	realloc(sb.size());

    std::copy(sb.itget, sb.itput, itbeg);

    itput = itbeg + sb.tellp();
    itget = itbeg + sb.tellg();
    flags = 0;

    setbigendian(sb.bigendian());
}

bool StreamBuf::put(const char & ch)
{
    if(0 == sizep())
    {
	realloc(capacity() + capacity() / 2);
    }

    if(sizep())
    {
        *itput++ = ch;
        return true;
    }
    return false;
}

bool StreamBuf::get(char & ch)
{
    if(sizeg())
    {
	ch = *itget++;
	return true;
    }
    return false;
}

size_t StreamBuf::sizeg(void) const
{
    return itput - itget;
}

size_t StreamBuf::sizep(void) const
{
    return itend - itput;
}

std::ostream & operator<< (std::ostream & os, StreamBuf & sb)
{
    const u32 count = sb.sizeg();

    os.unsetf(std::ios::skipws);
    sb.bigendian() ? StreamBase::putBE32(os, count) : StreamBase::putLE32(os, count);

    if(os.write(sb.itget, count))
	sb.itget += count;

    return os;
}

size_t available_count(std::istream & is)
{
    const size_t curpos = is.tellg();
    is.seekg(0, std::ios_base::end);
    const size_t sizeis = is.tellg();
    is.seekg(curpos, std::ios_base::beg);
    return sizeis  > curpos ? sizeis - curpos : 0;
}

std::istream & operator>> (std::istream & is, StreamBuf & sb)
{
    is.unsetf(std::ios::skipws);
    const u32 count = sb.bigendian() ? StreamBase::getBE32(is) : StreamBase::getLE32(is);

    if(count > available_count(is))
    {
	sb.setfail();
	return is;
    }

    if(sb.sizep() < count)
	sb.realloc(count);

    if(is.read(sb.itput, count))
	sb.itput += count;
    else
	sb.setfail();

    return is;
}

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
