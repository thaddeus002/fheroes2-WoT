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

#include <fstream>
#include <iterator>
#include <cctype>
#include <iostream>

#include "tinyconfig.h"
#include "tools.h"

Tiny::Value::Value() : ival(0)
{
}

Tiny::Value::Value(int val)
{
    ival = val;
    sval = val;
}

Tiny::Value::Value(const std::string & val)
{
    sval = val;
    ival = String::ToInt(sval);
}

void Tiny::Value::operator= (int val)
{
    ival = val;
    sval.clear();
    String::AddInt(sval, val);
}

void Tiny::Value::operator= (const std::string & val)
{
    sval = val;
    ival = String::ToInt(sval);
}

std::ostream & Tiny::operator<< (std::ostream & os, const Tiny::Entry & en)
{
    os << en.first << " = " << en.second.sval << std::endl;
    return os;
}

Tiny::Entry::Entry()
{
}

Tiny::Entry::Entry(const std::string & key, const std::string & val) :
    std::pair<std::string, Value>(key, val)
{
}

Tiny::Entry::Entry(const std::string & key, int val) :
    std::pair<std::string, Value>(key, val)
{
}

bool Tiny::Entry::IsKey(const char* key) const
{
    return key && key == first;
}

bool Tiny::Entry::IsValue(const char* val) const
{
    return val && val == second.sval;
}

bool Tiny::Entry::IsValue(int val) const
{
    return val == second.ival;
}

Tiny::Config::Config() : separator('='), comment('#')
{
}

Tiny::Config::~Config()
{
}

void Tiny::Config::SetSeparator(char c)
{
    separator = c;
}

bool Tiny::Config::Load(const char* cfile)
{
    if(!cfile) return false;

    std::ifstream fs(cfile);
    if(!fs.is_open()) return false;

    filename = cfile;

    std::string str;
    while(std::getline(fs, str))
    {
	String::Trim(str);
	if(str.empty() || str[0] == comment) continue;

        size_t pos = str.find(separator);
        if(std::string::npos != pos)
        {
            std::string left(str.substr(0, pos));
            std::string right(str.substr(pos + 1, str.length() - pos - 1));

	    String::Trim(left);
    	    String::Trim(right);

	    AddEntry(left, right);
        }
    }
    fs.close();

    return true;
}

bool Tiny::Config::Save(const char* cfile)
{
    std::ofstream fs(cfile ? cfile : filename.c_str());
    if(!fs.is_open()) return false;

    Dump(fs);
    fs.close();

    return true;
}

void Tiny::Config::Dump(std::ostream & os)
{
    std::copy(entries.begin(), entries.end(), std::ostream_iterator<Entry>(os, ""));
}

Tiny::EntryIterator Tiny::Config::FindEntry(std::string key)
{
    String::Lower(key);
    return std::find_if(entries.begin(), entries.end(), std::bind2nd(std::mem_fun_ref(&Entry::IsKey), key.c_str()));
}

Tiny::EntryConstIterator Tiny::Config::FindEntry(std::string key) const
{
    String::Lower(key);
    return std::find_if(entries.begin(), entries.end(), std::bind2nd(std::mem_fun_ref(&Entry::IsKey), key.c_str()));
}

int Tiny::Config::IntParams(const std::string & key) const
{
    EntryConstIterator it = FindEntry(key);
    return it != entries.end() ? (*it).second.ival : 0;
}

const char* Tiny::Config::StrParams(const std::string & key) const
{
    EntryConstIterator it = FindEntry(key);
    return it != entries.end() ? (*it).second.sval.c_str() : NULL;
}

void Tiny::Config::AddEntry(const std::string & left, const std::string & right)
{
    EntryIterator it = FindEntry(left);

    if(it == entries.end())
	entries.push_back(Entry(left, right));
    else
	(*it).second = right;
}

void Tiny::Config::Clear(void)
{
    entries.clear();
}
