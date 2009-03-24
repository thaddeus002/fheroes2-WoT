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

#include <iostream>
#include <algorithm>
#include "sdlnet.h"

#define FHXX	0x46483031	// free heroes2 protocol, ver 01

Network::Message::Message() : type(MSG_UNKNOWN)
{
}

Network::Message::Message(msg_t id) : type(id)
{
}

Uint16 Network::Message::GetID(void) const
{
    return type;
}

void Network::Message::SetID(msg_t id)
{
    type = id;
}

void Network::Message::Reset(void)
{
    type = MSG_UNKNOWN;
    data.clear();
}

bool Network::Message::Recv(const Socket & csd)
{
    Uint16 fhid;
    type = MSG_UNKNOWN;

    if(csd.Recv(reinterpret_cast<char *>(&fhid), sizeof(fhid)))
    {
	fhid = SDL_SwapBE16(fhid);

	// check id
	if((0xFF00 & FHXX) != (0xFF00 & fhid))
	{
	    std::cerr << "Network::Message::Recv: unknown packet id: 0x" << std::hex << fhid << std::endl;
	    return false;
	}

	// check ver
	if((0x00FF & FHXX) > (0x00FF & fhid))
	{
	    std::cerr << "Network::Message::Recv: obsolete protocol version: 0x" << std::hex << (0x00FF & fhid) << std::endl;
	    return false;
	}

	Uint32 size;

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

bool Network::Message::Send(const Socket & csd) const
{
    if(MSG_RAW == type)
	return csd.Send(reinterpret_cast<const char *>(&data[0]), data.size());

    const Uint16 fhid = SDL_SwapBE16(static_cast<Uint16>(FHXX));
    const Uint16 sign = SDL_SwapBE16(type);
    const Uint32 size = SDL_SwapBE32(data.size());

    return csd.Send(reinterpret_cast<const char *>(&fhid), sizeof(fhid)) &&
           csd.Send(reinterpret_cast<const char *>(&sign), sizeof(sign)) &&
           csd.Send(reinterpret_cast<const char *>(&size), sizeof(size)) &&
           (size ? csd.Send(reinterpret_cast<const char *>(&data[0]), data.size()) : true);
}

void Network::Message::Push(Uint8 byte8)
{
    data.push_back(byte8);
}

void Network::Message::Push(Uint16 byte16)
{
    data.push_back(static_cast<Uint8>(0x00FF & (byte16 >> 8)));
    data.push_back(static_cast<Uint8>(0x00FF & byte16));
}

void Network::Message::Push(Uint32 byte32)
{
    data.push_back(static_cast<Uint8>(0x000000FF & (byte32 >> 24)));
    data.push_back(static_cast<Uint8>(0x000000FF & (byte32 >> 16)));
    data.push_back(static_cast<Uint8>(0x000000FF & (byte32 >> 8)));
    data.push_back(static_cast<Uint8>(0x000000FF & byte32));
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

bool Network::Message::Pop(Uint8 & byte8)
{
    if(data.empty()) return false;

    byte8 = data.front();
    data.pop_front();

    return true;
}

bool Network::Message::Pop(Uint16 & byte16)
{
    if(data.size() < sizeof(byte16)) return false;

    byte16 = data.front();
    byte16 <<= 8;
    data.pop_front();
    byte16 |= (0x00FF & data.front());
    data.pop_front();

    return true;
}

bool Network::Message::Pop(Uint32 & byte32)
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
    // find end string
    std::deque<Uint8>::const_iterator it_beg = data.begin();
    std::deque<Uint8>::const_iterator it_end = std::find(data.begin(), data.end(), 0x00);
    if(it_end == data.end()) return false;

    for(; it_beg != it_end; ++it_beg) str += *it_beg;
    return true;
}

void Network::Message::Dump(std::ostream & stream) const
{
    stream << "Network::Message::Dump: type: " << GetMsgString(type) << ", size: " << data.size();

    if(data.size())
    {
	stream << ", data:";
	std::deque<Uint8>::const_iterator it1 = data.begin();
	std::deque<Uint8>::const_iterator it2 = data.end();
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

bool Network::ResolveHost(IPaddress & ip, const char* host, Uint16 port)
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

const char* Network::GetMsgString(Uint16 msg)
{
    switch(msg)
    {
	case MSG_RAW:		return "MSG_RAW";
	case MSG_DATA:		return "MSG_DATA";
        case MSG_PING:		return "MSG_PING";
        case MSG_SHUTDOWN:	return "MSG_SHUTDOWN";
        case MSG_CONNECT:	return "MSG_CONNECT";
        case MSG_LOGOUT:	return "MSG_LOGOUT";
        case MSG_WAIT:		return "MSG_WAIT";
        case MSG_READY:		return "MSG_READY";

        default: break;
    }

    return "MSG_UNKNOWN";
}
