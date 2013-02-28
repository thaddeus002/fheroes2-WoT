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

#include <sstream>
#include <iterator>

#include <SDL_net.h>

#include "network_message.h"

void NetworkMessage::add_str_chunk(Uint8 type, const std::string &data)
{
    str_chunks.insert(std::make_pair(type, data));
}

void NetworkMessage::add_bin_chunk(Uint8 type, const std::string &data)
{
    bin_chunks.insert(std::make_pair(type, data));
}

void NetworkMessage::add_bin_chunk(Uint8 Attribute, const char *data, size_t len)
{
    bin_chunks.insert(std::make_pair(Attribute, std::string(data, len)));
}

void NetworkMessage::add_int_chunk(Uint8 type, int data)
{
    int_chunks.insert(std::make_pair(type, data));
}

bool NetworkMessage::HasInt(Uint8 Attribute) const
{
    std::map<Uint8, int>::const_iterator i = int_chunks.find(Attribute);
    return i != int_chunks.end();
}

bool NetworkMessage::HasStr(Uint8 Attribute) const
{
    std::map<Uint8, std::string>::const_iterator i = str_chunks.find(Attribute);
    return i != str_chunks.end();
}

bool NetworkMessage::HasBin(Uint8 Attribute) const
{
    std::map<Uint8, std::string>::const_iterator i = bin_chunks.find(Attribute);
    return i != bin_chunks.end();
}

int NetworkMessage::GetInt(Uint8 Attribute) const
{
    std::map<Uint8, int>::const_iterator i = int_chunks.find(Attribute);
    return i != int_chunks.end() ? i->second : 0;
}

std::string NetworkMessage::GetStr(Uint8 Attribute) const
{
    std::map<Uint8, std::string>::const_iterator i = str_chunks.find(Attribute);
    return i != str_chunks.end() ? i->second : "";
}

std::string NetworkMessage::GetBin(Uint8 Attribute) const
{
    std::map<Uint8, std::string>::const_iterator i = bin_chunks.find(Attribute);
    return i != bin_chunks.end() ? i->second : "";
}

void NetworkMessage::CopyBin(Uint8 Attribute, Uint8 *data, size_t size) const
{
    std::map<Uint8, std::string>::const_iterator i = bin_chunks.find(Attribute);
    if(i != bin_chunks.end() && i->second.size() <= size) {
        std::copy(i->second.begin(), i->second.end(), data);
    }
}

std::ostream& operator<<(std::ostream& o, const NetworkMessage &Message)
{
    std::ostringstream body;

    for(std::map<Uint8, int>::const_iterator i = Message.int_chunks.begin() ;
        i != Message.int_chunks.end() ; i++)
    {
        Uint32 data = i->second;
        body << '\000' << '\004' << '\001' << i->first
            << (Uint8)((data >> 24) & 0xff)
            << (Uint8)((data >> 16) & 0xff)
            << (Uint8)((data >> 8) & 0xff)
            << (Uint8)(data & 0xff);
    }

    for(std::map<Uint8, std::string>::const_iterator i = Message.str_chunks.begin() ;
        i != Message.str_chunks.end() ; i++)
    {
        Uint16 length = i->second.size();
        body << (Uint8)((length >> 8) & 0xff) 
            << (Uint8)(length & 0xff)
            << '\002' << i->first;
        std::ostream_iterator<Uint8> out_it(body);
        std::copy(i->second.begin(), i->second.end(), out_it);
    }

    for(std::map<Uint8, std::string>::const_iterator i = Message.bin_chunks.begin() ;
        i != Message.bin_chunks.end() ; i++)
    {
        Uint16 length = i->second.size();
        body << (Uint8)((length >> 8) & 0xff) 
            << (Uint8)(length & 0xff)
            << '\003' << i->first;
        std::ostream_iterator<Uint8> out_it(body);
        std::copy(i->second.begin(), i->second.end(), out_it);
    }

    std::string body_str(body.str());

    Uint16 length = body_str.size() + sizeof(Uint16) + 2;

    o.put(length >> 8);
    o.put(length & 0xff);

    o.put(Message.type);
    o.put(Message.int_chunks.size() + Message.str_chunks.size() + Message.bin_chunks.size());

    std::ostream_iterator<Uint8> out_it(o);
    std::copy(body_str.begin(), body_str.end(), out_it);

    return o;
}

std::istream& operator>>(std::istream& i, NetworkMessage &Message)
{
    Uint8 lo, hi, NumChunks, ChunkType, Attribute, hi32, lo32;
    size_t len;
    std::string data;
    char c;

    if(!i.get((char&)hi)) {
        return i;
    }

    if(!i.get((char&)lo)) {
        return i;
    }

    if(!i.get((char&)Message.type)) {
        return i;
    }

    if(!i.get((char&)NumChunks)) {
        return i;
    }

    std::cout << "decaps pdu type=" << (int)Message.type << " num_chunks=" << (int)NumChunks << std::endl;

    len = (hi << 8) | lo;

    for(size_t t = 0 ; t != NumChunks ; t++) {
        if(!i.get((char&)lo)) {
            return i;
        }

        if(!i.get((char&)hi)) {
            return i;
        }

        if(!i.get((char&)ChunkType)) {
            return i;
        }

        if(!i.get((char&)Attribute)) {
            return i;
        }

        len = (lo << 8) | hi;

        switch(ChunkType) {
            case 1:
                if(!i.get((char&)hi32)) {
                    return i;
                }

                if(!i.get((char&)lo32)) {
                    return i;
                }

                if(!i.get((char&)hi)) {
                    return i;
                }

                if(!i.get((char&)lo)) {
                    return i;
                }
                std::cout << "decaps attribute=" << (int)Attribute << " value=" << ((hi32 << 24) | (lo32 << 16) | (hi << 8) | lo) << std::endl;
                Message.int_chunks.insert(std::make_pair(Attribute, (hi32 << 24) | (lo32 << 16) | (hi << 8) | lo));
                break;
            case 2:
                for(size_t s = 0 ; s != len ; s++) {
                    if(!i.get(c)) {
                        return i;
                    }
                    data.append(1, c);
                }
                std::cout << "decaps attribute=" << (int)Attribute << " value=\"" << data << "\"" << std::endl;
                Message.str_chunks.insert(std::make_pair(Attribute, data));
                break;
            case 3:
                for(size_t s = 0 ; s != len ; s++) {
                    if(!i.get(c)) {
                        return i;
                    }
                    data.append(1, c);
                }
                std::cout << "decaps attribute=" << (int)Attribute << " <" << len << " bytes of binary data>" << std::endl;
                Message.bin_chunks.insert(std::make_pair(Attribute, data));
                break;
        }

        if(!i.good()) {
            return i;
        }

        data.clear();
    }

    return i;
}
