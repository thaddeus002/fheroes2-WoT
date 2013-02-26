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

struct Point;
struct Rect;
struct Size;

class StreamBase
{
public:
    StreamBase() {}
    virtual ~StreamBase() {}

    virtual bool	get(char &) = 0;
    virtual size_t	sizeg(void) const = 0;

    virtual bool	put(const char &) = 0;
    virtual size_t	sizep(void) const = 0;

    char		get(void);
    void		get16(u16 &);
    u16			get16(void);
    void		get32(u32 &);
    u32			get32(void);

    void		put16(const u16 &);
    void		put32(const u32 &);

    StreamBase &	operator>> (bool &);
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

    static void		put32(std::ostream &, const u32 &);
    static void		put16(std::ostream &, const u16 &);

    static u32		get32(std::istream &);
    static u16		get16(std::istream &);
};

#ifdef WITH_ZLIB
class ZStreamBuf;
#endif

class StreamBuf : public StreamBase
{
public:
    StreamBuf(size_t);
    StreamBuf(const StreamBuf &);

    ~StreamBuf();

    StreamBuf &	operator= (const StreamBuf &);

    size_t	capacity(void) const;
    void	reset(void);
    std::string	dump(void) const;

    char*	data(void);
    size_t	size(void) const;
    void	setlimit(size_t);

    bool	fail(void) const;

protected:
    size_t	tellg(void) const;
    size_t	tellp(void) const;

    void	copy(const StreamBuf &);
    void	realloc(size_t);
    void	setfail(void);

    bool	get(char &);
    bool	put(const char &);

    size_t	sizeg(void) const;
    size_t	sizep(void) const;

    friend std::ostream & operator<< (std::ostream &, StreamBuf &);
    friend std::istream & operator>> (std::istream &, StreamBuf &);

#ifdef WITH_ZLIB
    friend class ZStreamBuf;
#endif

    char*	itbeg;
    char*	itget;
    char*	itput;
    char*	itend;
    size_t	flags;
};

std::ostream & operator<< (std::ostream &, StreamBuf &);
std::istream & operator>> (std::istream &, StreamBuf &);

#endif
