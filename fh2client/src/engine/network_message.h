/***************************************************************************
 *   Copyright (C) 2013 by Star Marine <starmarine10@yahoo.com>            *
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

#ifndef _NETWORK_MESSAGE_H_
#define _NETWORK_MESSAGE_H_

#include <sys/types.h>

#include <string>
#include <iostream>
#include <map>

class NetworkMessage
{
public:
    NetworkMessage()
        : str_chunks()
        , bin_chunks()
        , int_chunks()
        , type(0)
    {
    }

    NetworkMessage(u_char _type)
        : str_chunks()
        , bin_chunks()
        , int_chunks()
        , type(_type)
    {
    }

    NetworkMessage(const NetworkMessage &that)
        : str_chunks(that.str_chunks)
        , bin_chunks(that.bin_chunks)
        , int_chunks(that.int_chunks)
        , type(that.type)
    {
    }

    void add_str_chunk(u_char, const std::string&);
    void add_bin_chunk(u_char, const std::string&);
    void add_bin_chunk(u_char, const char*, size_t);
    void add_int_chunk(u_char, int);

    bool HasInt(u_char) const;
    bool HasStr(u_char) const;
    bool HasBin(u_char) const;

    int GetType() const { return type; }
    int GetInt(u_char) const;
    std::string GetStr(u_char) const;
    std::string GetBin(u_char) const;
    void CopyBin(u_char, u_char*, size_t) const;

private:
    static void write_word(std::ostream &o, u_int16_t _value) {
        o.put(_value >> 8);
        o.put(_value & 0xff);
    }

private:
    friend std::ostream& operator<<(std::ostream&, const NetworkMessage&);
    friend std::istream& operator>>(std::istream&, NetworkMessage&);
    std::map<u_char, std::string> str_chunks;
    std::map<u_char, std::string> bin_chunks;
    std::map<u_char, int> int_chunks;
    u_char type;
};

std::ostream& operator<<(std::ostream&, const NetworkMessage&);
std::istream& operator>>(std::istream&, NetworkMessage&);

#endif
