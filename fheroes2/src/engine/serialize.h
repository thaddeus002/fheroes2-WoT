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
protected:
    virtual int		get8(void) = 0;
    virtual void	put8(int) = 0;

public:
    enum { EndianBig = 0x80000000 };

    StreamBase() {}
    virtual ~StreamBase() {}

    virtual size_t	sizeg(void) const = 0;
    virtual size_t	sizep(void) const = 0;

    virtual size_t	tellg(void) const = 0;
    virtual size_t	tellp(void) const = 0;

    virtual bool	bigendian(void) const;

    int			getBE16(void);
    int			getLE16(void);
    int			getBE32(void);
    int			getLE32(void);

    int			get16(void);
    int			get32(void);

    std::vector<u8>	getRaw(size_t = 0 /* all data */);
    void		putRaw(const char*, size_t);

    void		put16(u16);
    void		put32(u32);

    void		putBE32(u32);
    void		putLE32(u32);
    void		putBE16(u16);
    void		putLE16(u16);

    virtual void	skip(size_t);

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

    static void		putBE32(std::ostream &, u32);
    static void		putLE32(std::ostream &, u32);

    static void		putBE16(std::ostream &, u16);
    static void		putLE16(std::ostream &, u16);

    static int		getBE32(std::istream &);
    static int		getLE32(std::istream &);

    static int		getBE16(std::istream &);
    static int		getLE16(std::istream &);

    static int		getBE32(const u8*);
    static int		getLE32(const u8*);

    static int		getBE16(const u8*);
    static int		getLE16(const u8*);
};

#ifdef WITH_ZLIB
class ZStreamBuf;
#endif

class StreamBuf : public StreamBase
{
public:
    StreamBuf(size_t);
    StreamBuf(const StreamBuf &);
    StreamBuf(const std::vector<u8> &);
    StreamBuf(const u8*, size_t);

    ~StreamBuf();

    StreamBuf &	operator= (const StreamBuf &);

    size_t	capacity(void) const;
    void	reset(void);
    std::string	dump(void) const;

    u8*		data(void);
    size_t	size(void) const;
    void	setlimit(size_t);

    bool	fail(void) const;

    void	setconstbuf(bool);
    bool	isconstbuf(void) const;

    void	setbigendian(bool);
    bool	bigendian(void) const;

    int		get(void); /* get char */
    void	put(int);

    void	skip(size_t);
    size_t	tellg(void) const;
    size_t	tellp(void) const;
    size_t	sizeg(void) const;
    size_t	sizep(void) const;

protected:
    void	copy(const StreamBuf &);
    void	realloc(size_t);
    void	setfail(void);

    int		get8(void);
    void	put8(int);


    friend std::ostream & operator<< (std::ostream &, StreamBuf &);
    friend std::istream & operator>> (std::istream &, StreamBuf &);

#ifdef WITH_NET
    friend Network::Socket & operator<< (Network::Socket &, StreamBuf &);
    friend Network::Socket & operator>> (Network::Socket &, StreamBuf &);
#endif

#ifdef WITH_ZLIB
    friend class ZStreamBuf;
#endif

    u8*		itbeg;
    u8*		itget;
    u8*		itput;
    u8*		itend;
    size_t	flags;
};

std::ostream & operator<< (std::ostream &, StreamBuf &);
std::istream & operator>> (std::istream &, StreamBuf &);

#ifdef WITH_NET
Network::Socket & operator<< (Network::Socket &, StreamBuf &);
Network::Socket & operator>> (Network::Socket &, StreamBuf &);
#endif

#endif
