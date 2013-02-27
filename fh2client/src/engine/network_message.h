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

    NetworkMessage(Uint8 _type)
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

    void add_str_chunk(Uint8, const std::string&);
    void add_bin_chunk(Uint8, const std::string&);
    void add_bin_chunk(Uint8, const char*, size_t);
    void add_int_chunk(Uint8, int);

    bool HasInt(Uint8) const;
    bool HasStr(Uint8) const;
    bool HasBin(Uint8) const;

    int GetType() const { return type; }
    int GetInt(Uint8) const;
    std::string GetStr(Uint8) const;
    std::string GetBin(Uint8) const;
    void CopyBin(Uint8, Uint8*, size_t) const;

private:
    friend std::ostream& operator<<(std::ostream&, const NetworkMessage&);
    friend std::istream& operator>>(std::istream&, NetworkMessage&);
    std::map<Uint8, std::string> str_chunks;
    std::map<Uint8, std::string> bin_chunks;
    std::map<Uint8, int> int_chunks;
    Uint8 type;
};

std::ostream& operator<<(std::ostream&, const NetworkMessage&);
std::istream& operator>>(std::istream&, NetworkMessage&);

#endif
