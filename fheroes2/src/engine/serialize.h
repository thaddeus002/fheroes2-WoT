/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <iostream>
#include <streambuf>
#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <cstdlib>
#include <vector>

#include "types.h"

#define DEFAULT_PAGE_SIZE 64

struct Point;
struct Rect;
struct Size;

class PageBuffer : public std::streambuf
{
    private:
	size_t pagesz;
	std::list<char*> listbuf;
	std::list<char*>::iterator isp, osp;

	std::list<char*>::iterator AddPage(void)
	{
	    char* ptr = new char [pagesz];
	    listbuf.push_back(ptr);
	    return --listbuf.end();
	}

	static void DelPage(char* ptr)
	{
	    delete [] ptr;
	}

    public:
        PageBuffer(size_t sz) : pagesz(sz)
	{
	    isp = osp = AddPage();
	    setp(*osp, *osp + pagesz);
	    setg(*isp, *isp, *isp);
	}

	virtual ~PageBuffer()
	{
	    std::for_each(listbuf.begin(), listbuf.end(), DelPage);
	}

    private:
	size_t psize(void)
	{
	    return pptr() - pbase();
	}

	size_t gsize(void)
	{
	    return gptr() - eback();
	}

	size_t obufpos(void)
	{
	    return std::distance(listbuf.begin(), osp) * pagesz + psize();
	}

	size_t ibufpos(void)
	{
	    return std::distance(listbuf.begin(), isp) * pagesz + gsize();
	}

    private:
	int overflow(int c)
        {
	    osp = AddPage();
	    if(*osp)
	    {
		setp(*osp, *osp + pagesz);
		sputc(c);
		return traits_type::to_int_type(*pptr());
	    }
	    return traits_type::eof();
        }

	int underflow(void)
        {
	    if(pagesz > gsize())
	    {
		if(eback() != pbase())
		{
		    setg(eback(), gptr(), eback() + pagesz);
	    	    return traits_type::to_int_type(*gptr());
		}
		else
		if(gsize() < psize())
		{
		    setg(eback(), gptr(), eback() + psize());
	    	    return traits_type::to_int_type(*gptr());
		}
	    }
	    else
	    if(++isp != listbuf.end())
	    {
		setg(*isp, *isp, (*isp != pbase() ? *isp + pagesz : *isp + psize()));
		return traits_type::to_int_type(*gptr());
    	    }
	    return traits_type::eof();
        }

	std::streamsize showmanyc(void)
	{
	    return obufpos() - ibufpos();
	}

	std::streampos seekpos(std::streampos sp, std::ios_base::openmode which)
	{
	    if((which & std::ios_base::in) &&
		sp >= 0 &&
		static_cast<size_t>(sp) < obufpos())
	    {
		isp = listbuf.begin();
		std::advance(isp, sp / pagesz);
		setg(*isp, *isp + (sp % pagesz), (*isp != pbase() ? *isp + pagesz : *isp + psize()));
		return sp;
	    }
	    return std::streampos(-1);
	}

	std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
	{
	    if(which & std::ios_base::in)
	    {
		switch(way)
		{
		    case std::ios_base::beg: return seekpos(off, which);
		    case std::ios_base::cur: return seekpos(ibufpos() + off, which);
		    case std::ios_base::end: return seekpos(obufpos() - std::abs(static_cast<int>(off)), which);
		    default: break;
		}
	    }
	    return std::streampos(-1);
	}

    private:
        PageBuffer(const PageBuffer &);
        PageBuffer& operator= (const PageBuffer &);
};

class Serialize : protected std::iostream
{
    private:
	PageBuffer buf;

	Serialize & put16(const u16 &);
	Serialize & put32(const u32 &);

	Serialize & get16(u16 &);
	Serialize & get32(u32 &);

    public:
	Serialize(size_t sizedef = DEFAULT_PAGE_SIZE) : std::iostream(&buf), buf(sizedef ? sizedef : DEFAULT_PAGE_SIZE) {}

	Serialize & operator<< (const bool &);
	Serialize & operator<< (const u8 &);
	Serialize & operator<< (const s8 &);
	Serialize & operator<< (const u16 &);
	Serialize & operator<< (const s16 &);
	Serialize & operator<< (const u32 &);
	Serialize & operator<< (const s32 &);
	Serialize & operator<< (const std::string &);

	Serialize & operator<< (const Rect &);
	Serialize & operator<< (const Point&);
	Serialize & operator<< (const Size &);

	Serialize & operator>> (bool &);
	Serialize & operator>> (u8 &);
	Serialize & operator>> (s8 &);
	Serialize & operator>> (u16 &);
	Serialize & operator>> (s16 &);
	Serialize & operator>> (u32 &);
	Serialize & operator>> (s32 &);
	Serialize & operator>> (std::string &);

	Serialize & operator>> (Rect &);
	Serialize & operator>> (Point&);
	Serialize & operator>> (Size &);

	template<class Type>
	Serialize & operator<< (const std::vector<Type> & v)
	{
	    put32(v.size());
	    for(typename std::vector<Type>::const_iterator
    		it = v.begin(); it != v.end(); ++it) *this << *it;
	    return *this;
	}

	template<class Type>
	Serialize & operator<< (const std::list<Type> & v)
	{
	    put32(v.size());
	    for(typename std::list<Type>::const_iterator
    		it = v.begin(); it != v.end(); ++it) *this << *it;
	    return *this;
	}

	template<class Type>
	Serialize & operator>> (std::vector<Type> & v)
	{
	    u32 size;
	    get32(size);
	    v.resize(size);
	    for(typename std::vector<Type>::iterator
    		it = v.begin(); it != v.end(); ++it) *this >> *it;
	    return *this;
	}

	template<class Type>
	Serialize & operator>> (std::list<Type> & v)
	{
	    u32 size;
	    get32(size);
	    v.resize(size);
	    for(typename std::list<Type>::iterator
    		it = v.begin(); it != v.end(); ++it) *this >> *it;
	    return *this;
	}
};

#endif
