/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of SDL++ Engine:                                                 *
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

#ifndef TINYCONFIG_H
#define TINYCONFIG_H

#include <utility>
#include <ostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <list>

namespace Tiny
{
    struct Value
    {
	Value();
	Value(int val);
	Value(const std::string & val);

	void operator= (int val);
	void operator= (const std::string & val);

	int ival;
	std::string sval;
    };

    struct Entry : public std::pair<std::string, Value>
    {
	Entry();
	Entry(const std::string & key, const std::string & val);
	Entry(const std::string & key, int val);

	bool IsKey(const char* key) const;
	bool IsValue(const char* val) const;
	bool IsValue(int val) const;
    };

    std::ostream & operator<< (std::ostream & os, const Entry & en);

    typedef std::list<Entry>::iterator EntryIterator;
    typedef std::list<Entry>::const_iterator EntryConstIterator;

    class Config
    {
    public:
	Config();
	~Config();

	bool Load(const char*);
	bool Save(const char* = NULL);
	void Dump(std::ostream &);
	void Clear(void);

	void SetSeparator(char);
	void SetComment(char);

	int IntParams(const std::string &) const;
	const char* StrParams(const std::string &) const;

    protected:
	EntryIterator FindEntry(std::string);
	EntryConstIterator FindEntry(std::string) const;
    	void AddEntry(const std::string &, const std::string &);

	std::string filename;
	char separator;
	char comment;
	std::list<Entry> entries;
    };
}

#endif
