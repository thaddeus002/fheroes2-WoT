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

#ifndef H2IO_H
#define H2IO_H

#include <list>
#include <vector>
#include <map>
#include <string>

#include "types.h"

#ifdef WITH_NET
namespace Network { class Socket; }
#endif

struct Point;
struct Rect;
struct Size;

class StreamBase
{
private:
    StreamBase(const StreamBase &) {}
    StreamBase &	operator= (const StreamBase &) { return *this; }

protected:
    int			get8(void);
    void		put8(u8);
    void		setfail(void);

    SDL_RWops*		rw;
    size_t		flags;

    virtual void	resize(size_t) {}

public:
    StreamBase() : rw(NULL), flags(0) {}
    ~StreamBase();

    int			getBE16(void);
    int			getLE16(void);
    int			getBE32(void);
    int			getLE32(void);

    int			get16(void);
    int			get32(void);

    void                putBE32(u32);
    void                putLE32(u32);
    void                putBE16(u16);
    void                putLE16(u16);

    void		put16(u16);
    void		put32(u32);

    int			get(void) { return get8(); } // get char
    void		put(int ch) { put8(ch); }

    std::vector<u8>	getRaw(size_t = 0 /* all data */);
    void		putRaw(const void*, size_t);

    void		skip(size_t);

    void		setbigendian(bool);
    bool		bigendian(void) const;

    bool		fail(void) const;

    long int		tell(void) const;
    bool		seek(long int);

    StreamBase &	operator>> (bool &);
    StreamBase &	operator>> (char &);
    StreamBase &	operator>> (u8 &);
    StreamBase &	operator>> (s8 &);
    StreamBase &	operator>> (u16 &);
    StreamBase &	operator>> (s16 &);
    StreamBase &	operator>> (u32 &);
    StreamBase &	operator>> (s32 &);
    StreamBase &	operator>> (float &);
    StreamBase &	operator>> (std::string &);

    StreamBase &	operator>> (Rect &);
    StreamBase &	operator>> (Point &);
    StreamBase &	operator>> (Size &);

    StreamBase &	operator<< (const bool &);
    StreamBase &	operator<< (const char &);
    StreamBase &	operator<< (const u8 &);
    StreamBase &	operator<< (const s8 &);
    StreamBase &	operator<< (const u16 &);
    StreamBase &	operator<< (const s16 &);
    StreamBase &	operator<< (const u32 &);
    StreamBase &	operator<< (const s32 &);
    StreamBase &	operator<< (const float &);
    StreamBase &	operator<< (const std::string &);

    StreamBase &	operator<< (const Rect &);
    StreamBase &	operator<< (const Point &);
    StreamBase &	operator<< (const Size &);

    template<class Type1, class Type2>
    StreamBase & operator>> (std::pair<Type1, Type2> & p)
    {
	return *this >> p.first >> p.second;
    }

    template<class Type>
    StreamBase & operator>> (std::vector<Type> & v)
    {
	const u32 size = get32();
	v.resize(size);
	for(typename std::vector<Type>::iterator
    	    it = v.begin(); it != v.end(); ++it) *this >> *it;
	return *this;
    }

    template<class Type>
    StreamBase & operator>> (std::list<Type> & v)
    {
	const u32 size = get32();
	v.resize(size);
	for(typename std::list<Type>::iterator
    	    it = v.begin(); it != v.end(); ++it) *this >> *it;
	return *this;
    }

    template<class Type1, class Type2>
    StreamBase & operator>> (std::map<Type1, Type2> & v)
    {
	const u32 size = get32();
	v.clear();
	for(u32 ii = 0; ii < size; ++ii)
	{
	    std::pair<Type1, Type2> pr;
	    *this >> pr;
	    v.insert(pr);
	}
	return *this;
    }

    template<class Type1, class Type2>
    StreamBase & operator<< (const std::pair<Type1, Type2> & p)
    {
	return *this << p.first << p.second;
    }

    template<class Type>
    StreamBase & operator<< (const std::vector<Type> & v)
    {
	put32(static_cast<u32>(v.size()));
	for(typename std::vector<Type>::const_iterator
	    it = v.begin(); it != v.end(); ++it) *this << *it;
	return *this;
    }

    template<class Type>
    StreamBase & operator<< (const std::list<Type> & v)
    {
	put32(static_cast<u32>(v.size()));
	for(typename std::list<Type>::const_iterator
	    it = v.begin(); it != v.end(); ++it) *this << *it;
	return *this;
    }

    template<class Type1, class Type2>
    StreamBase & operator<< (const std::map<Type1, Type2> & v)
    {
	put32(static_cast<u32>(v.size()));
	for(typename std::map<Type1, Type2>::const_iterator
	    it = v.begin(); it != v.end(); ++it) *this << *it;
	return *this;
    }
};

class StreamBuf : public StreamBase
{
    u8*			buf;
    size_t		len;

protected:
    void		setconstbuf(bool);
    bool		isconstbuf(void) const;
    void		resize(size_t);

public:
    StreamBuf(size_t = 0);
    StreamBuf(const u8*, size_t);
    StreamBuf(const std::vector<u8> &);
    StreamBuf(const StreamBuf &);
    ~StreamBuf();

    StreamBuf &		operator= (const StreamBuf &);

    const u8*		data(void) const { return buf; }
    u8*			data(void) { return buf; }
    size_t		size(void) const { return len; }
};

class StreamFile : public StreamBase
{
public:
    StreamFile() {}
    StreamFile(const std::string &, const char* mode);

    long int		size(void) const;
    bool		read(void*, size_t);
    bool		open(const std::string &, const char* mode);
    void		close(void);

    StreamBuf		toStreamBuf(void);
};

#endif
