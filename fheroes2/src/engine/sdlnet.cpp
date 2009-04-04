/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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

#ifdef WITH_NET

#include <iostream>
#include <algorithm>
#include "sdlnet.h"

namespace Network
{
    static u16 proto = 0xFF01;
};

void Network::SetProtocolVersion(u16 v)
{
    proto = v;
}

Network::Message::Message() : type(0)
{
}

Network::Message::Message(u16 id) : type(id)
{
}

u16 Network::Message::GetID(void) const
{
    return type;
}

void Network::Message::SetID(u16 id)
{
    type = id;
}

void Network::Message::Reset(void)
{
    type = 0;
    data.clear();
}

bool Network::Message::Recv(const Socket & csd, bool debug)
{
    u16 head;
    type = 0;

    if(csd.Recv(reinterpret_cast<char *>(&head), sizeof(head)))
    {
	SwapBE16(head);

	// check id
	if((0xFF00 & proto) != (0xFF00 & head))
	{
	    if(debug) std::cerr << "Network::Message::Recv: unknown packet id: 0x" << std::hex << head << std::endl;
	    return true;
	}

	// check ver
	if((0x00FF & proto) > (0x00FF & head))
	{
	    if(debug) std::cerr << "Network::Message::Recv: obsolete protocol proto: 0x" << std::hex << (0x00FF & head) << std::endl;
	    return true;
	}

	u32 size;

	if(csd.Recv(reinterpret_cast<char *>(&type), sizeof(type)) &&
	   csd.Recv(reinterpret_cast<char *>(&size), sizeof(size)))
	{
	    type = SDL_SwapBE16(type);
	    size = SDL_SwapBE32(size);
	    data.resize(size);

	    return size ? csd.Recv(reinterpret_cast<char *>(&data[0]), size) : true;
	}
    }
    return false;
}

bool Network::Message::Send(const Socket & csd, bool raw) const
{
    if(raw)
	return csd.Send(reinterpret_cast<const char *>(&data[0]), data.size());

    u16 head = proto;
    u16 sign = type;
    u32 size = data.size();

    SwapBE16(head);
    SwapBE16(sign);
    SwapBE32(size);

    return csd.Send(reinterpret_cast<const char *>(&head), sizeof(head)) &&
           csd.Send(reinterpret_cast<const char *>(&sign), sizeof(sign)) &&
           csd.Send(reinterpret_cast<const char *>(&size), sizeof(size)) &&
           (size ? csd.Send(reinterpret_cast<const char *>(&data[0]), data.size()) : true);
}

void Network::Message::Push(u8 byte8)
{
    data.push_back(byte8);
}

void Network::Message::Push(u16 byte16)
{
    data.push_back(static_cast<u8>(0x00FF & (byte16 >> 8)));
    data.push_back(static_cast<u8>(0x00FF & byte16));
}

void Network::Message::Push(u32 byte32)
{
    data.push_back(static_cast<u8>(0x000000FF & (byte32 >> 24)));
    data.push_back(static_cast<u8>(0x000000FF & (byte32 >> 16)));
    data.push_back(static_cast<u8>(0x000000FF & (byte32 >> 8)));
    data.push_back(static_cast<u8>(0x000000FF & byte32));
}

void Network::Message::Push(const std::string & str)
{
    if(str.size())
    {
	std::string::const_iterator it1 = str.begin();
	std::string::const_iterator it2 = str.end();
	for(; it1 != it2; ++it1) data.push_back(*it1);
    }
    // end string
    data.push_back(0x00);
}

bool Network::Message::Pop(u8 & byte8)
{
    if(data.empty()) return false;

    byte8 = data.front();
    data.pop_front();

    return true;
}

bool Network::Message::Pop(u16 & byte16)
{
    if(data.size() < sizeof(byte16)) return false;

    byte16 = data.front();
    byte16 <<= 8;
    data.pop_front();
    byte16 |= (0x00FF & data.front());
    data.pop_front();

    return true;
}

bool Network::Message::Pop(u32 & byte32)
{
    if(data.size() < sizeof(byte32)) return false;

    byte32 = data.front();
    byte32 <<= 8;
    data.pop_front();
    byte32 |= (0x000000FF & data.front());
    byte32 <<= 8;
    data.pop_front();
    byte32 |= (0x000000FF & data.front());
    byte32 <<= 8;
    data.pop_front();
    byte32 |= (0x000000FF & data.front());
    data.pop_front();

    return true;
}

bool Network::Message::Pop(std::string & str)
{
    str.clear();

    // find end string
    std::deque<u8>::const_iterator it_beg = data.begin();
    std::deque<u8>::const_iterator it_end = std::find(data.begin(), data.end(), 0x00);
    if(it_end == data.end()) return false;

    for(; it_beg != it_end; ++it_beg) str += *it_beg;
    return true;
}

void Network::Message::Dump(std::ostream & stream) const
{
    stream << "Network::Message::Dump: type: 0x" << std::hex << type << ", size: " << data.size();

    if(data.size())
    {
	stream << ", data:";
	std::deque<u8>::const_iterator it1 = data.begin();
	std::deque<u8>::const_iterator it2 = data.end();
	for(; it1 != it2; ++it1) stream << " 0x" << std::hex << static_cast<int>(*it1);
    }

    stream  << std::endl;
}

Network::Socket::Socket() : sd(NULL)
{
}

Network::Socket::Socket(const Socket &) : sd(NULL)
{
}

Network::Socket::Socket(const TCPsocket csd) : sd(csd)
{
}

Network::Socket & Network::Socket::operator= (const Socket &)
{
    return *this;
}

Network::Socket::~Socket()
{
    if(sd) Close();
}

u32 Network::Socket::Host(void) const
{
    IPaddress* remoteIP = sd ? SDLNet_TCP_GetPeerAddress(sd) : NULL;
    if(remoteIP) return SDLNet_Read32(&remoteIP->host);

    std::cerr << "Network::Socket::Host: " << GetError() << std::endl;
    return 0;
}

u16 Network::Socket::Port(void) const
{
    IPaddress* remoteIP = sd ? SDLNet_TCP_GetPeerAddress(sd) : NULL;
    if(remoteIP) return SDLNet_Read16(&remoteIP->port);

    std::cerr << "Network::Socket::Host: " << GetError() << std::endl;
    return 0;
}

bool Network::Socket::Recv(char *buf, size_t len) const
{
    if(sd && buf)
    {
	const int rcv = SDLNet_TCP_Recv(sd, buf, len);
	if(rcv > 0) return true;
	std::cerr << "Network::Socket::Recv: size: " << len << ", receive: " << rcv << ", error: " << GetError() << std::endl;
    }
    return false;
}

bool Network::Socket::Send(const char* buf, size_t len) const
{
    return sd && static_cast<int>(len) == SDLNet_TCP_Send(sd, const_cast<void*>(reinterpret_cast<const void*>(buf)), len);
}

bool Network::Socket::Open(IPaddress & ip)
{
    if(sd) Close();
    sd = SDLNet_TCP_Open(&ip);
    if(! sd)
	std::cerr << "Network::Socket::Open: " << Network::GetError() << std::endl;

    return sd;
}

bool Network::Socket::IsValid(void) const
{
    return sd;
}

void Network::Socket::Close(void)
{
    if(sd) SDLNet_TCP_Close(sd);
    sd = NULL;
}

Network::Server::Server()
{
}

TCPsocket Network::Server::Accept(void)
{
    return SDLNet_TCP_Accept(sd);
}

bool Network::Init(void)
{
    if(SDLNet_Init() < 0)
    {
        std::cerr << "Network::Init: " << GetError() << std::endl;
        return false;
    }
    return true;
}

void Network::Quit(void)
{
    SDLNet_Quit();
}

bool Network::ResolveHost(IPaddress & ip, const char* host, u16 port)
{
    if(SDLNet_ResolveHost(&ip, host, port) < 0)
    {
	std::cerr << "Network::ResolveHost: " << GetError() << std::endl;
	return false;
    }
    return true;
}

const char* Network::GetError(void)
{
    return SDLNet_GetError();
}

#endif
