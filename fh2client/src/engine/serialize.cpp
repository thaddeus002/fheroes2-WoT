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

char StreamBase::get(void)
{
    char res = 0;
    get(res);
    return res;
}

void StreamBase::get16(u16 & v)
{
    if(sizeg() > 1)
    {
	char ch;

	get(ch);
	v = ch;
	v <<= 8;

	get(ch);
	v |= 0x00FF & ch;
    }
}

u16 StreamBase::get16(void)
{
    u16 res = 0;
    get16(res);
    return res;
}

void StreamBase::get32(u32 & v)
{
    if(sizeg() > 3)
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
    }
}

u32 StreamBase::get32(void)
{
    u32 res = 0;
    get32(res);
    return res;
}

u32 StreamBase::get32(std::istream & is)
{
    u32 res;
    char ch;

    is.get(ch);
    res = ch;
    res <<= 8;

    is.get(ch);
    res |= 0x000000FF & ch;
    res <<= 8;

    is.get(ch);
    res |= 0x000000FF & ch;
    res <<= 8;

    is.get(ch);
    res |= 0x000000FF & ch;

    return res;
}

u16 StreamBase::get16(std::istream & is)
{
    u16 res;
    char ch;

    is.get(ch);
    res = ch;
    res <<= 8;

    is.get(ch);
    res |= 0x00FF & ch;

    return res;
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
    if(sizep() > 1)
    {
	put((v >> 8) & 0x00FF);
	put(v & 0x00FF);
    }
}

void StreamBase::put32(const u32 & v)
{
    if(sizep() > 3)
    {
	put((v >> 24) & 0x000000FF);
	put((v >> 16) & 0x000000FF);
        put((v >> 8) & 0x000000FF);
	put(v & 0x000000FF);
    }
}

void StreamBase::put32(std::ostream & os, const u32 & v)
{
    os.put((v >> 24) & 0x000000FF);
    os.put((v >> 16) & 0x000000FF);
    os.put((v >> 8) & 0x000000FF);
    os.put(v & 0x000000FF);
}

void StreamBase::put16(std::ostream & os, const u16 & v)
{
    os.put((v >> 8) & 0x00FF);
    os.put(v & 0x00FF);
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
}

StreamBuf::~StreamBuf()
{
    if(itbeg) delete [] itbeg;
}

StreamBuf::StreamBuf(const StreamBuf & st) : itbeg(NULL), itget(NULL), itput(NULL), itend(NULL), flags(0)
{
    copy(st);
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

bool StreamBuf::fail(void) const
{
    return flags;
}

void StreamBuf::setfail(void)
{
    flags |= 0x01;
}

void StreamBuf::copy(const StreamBuf & sb)
{
    if(capacity() < sb.size())
	realloc(sb.size());

    std::copy(sb.itget, sb.itput, itbeg);

    itput = itbeg + sb.tellp();
    itget = itbeg + sb.tellg();
    flags = 0;
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
    StreamBase::put32(os, count);

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
    const u32 count = StreamBase::get32(is);

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
