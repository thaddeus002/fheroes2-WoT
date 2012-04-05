/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include "sdlnet.h"

#define BUFSIZE 16

enum { ERROR_SEND = 0x8000, ERROR_RECV = 0x4000 };

QueueMessage::QueueMessage() : type(0), data(NULL), itd1(NULL), itd2(NULL), dtsz(BUFSIZE)
{
    data = new char [dtsz + 1];

    itd1 = data;
    itd2 = itd1;
}

QueueMessage::~QueueMessage()
{
    if(data) delete [] data;
}

size_t QueueMessage::Size(void) const
{
    return itd2 - data;
}

void QueueMessage::Resize(size_t lack)
{
    const size_t newsize = lack > dtsz ? dtsz + lack + 1 : 2 * dtsz + 1;
    char* dat2 = new char [newsize];
    std::memcpy(dat2, data, dtsz);
    itd1 = &dat2[itd1 - data];
    itd2 = &dat2[itd2 - data];
    dtsz = newsize - 1;
    delete [] data;
    data = dat2;
}

void QueueMessage::Push(u8 byte8)
{
    if(data + dtsz < itd2 + 1) Resize(1);

    *itd2 = byte8;
    ++itd2;
}

bool QueueMessage::Pop(s8 & byte8)
{
    u8 tmp;
    if(Pop(tmp))
    {
	byte8 = tmp;
	return true;
    }
    return false;
}

bool QueueMessage::Pop(u8 & byte8)
{
    if(itd1 + 1 > itd2) return false;

    byte8 = *itd1;
    ++itd1;

    return true;
}

bool QueueMessage::Pop(s16 & byte16)
{
    u16 tmp;
    if(Pop(tmp))
    {
	byte16 = tmp;
	return true;
    }
    return false;
}

bool QueueMessage::Pop(u16 & byte16)
{
    if(itd1 + 2 > itd2) return false;

    byte16 = *itd1;
    byte16 <<= 8;
    ++itd1;

    byte16 |= (0x00FF & *itd1);
    ++itd1;

    return true;
}

bool QueueMessage::Pop(s32 & byte32)
{
    u32 tmp;
    if(Pop(tmp))
    {
	byte32 = tmp;
	return true;
    }
    return false;
}

bool QueueMessage::Pop(u32 & byte32)
{
    if(itd1 + 4 > itd2) return false;

    byte32 = *itd1;
    byte32 <<= 8;
    ++itd1;

    byte32 |= (0x000000FF & *itd1);
    byte32 <<= 8;
    ++itd1;

    byte32 |= (0x000000FF & *itd1);
    byte32 <<= 8;
    ++itd1;

    byte32 |= (0x000000FF & *itd1);
    ++itd1;

    return true;
}

bool QueueMessage::Pop(bool & f)
{
    u8 tmp;
    if(Pop(tmp))
    {
	f = tmp;
	return true;
    }
    return false;
}

bool QueueMessage::Pop(std::string & str)
{
    if(itd1 >= itd2) return false;

    // find end string
    char* end = itd1;
    while(*end && end < itd2) ++end;
    if(end == itd2) return false;

    str = itd1;
    itd1 = end + 1;

    return true;
}

void QueueMessage::Load(const char* fn)
{
    std::ifstream fs(fn, std::ios::binary);

    if(fs.is_open())
    {
	fs.seekg(0, std::ios_base::end);
	dtsz = fs.tellg();
	fs.seekg(0, std::ios_base::beg);

	delete [] data;
	data = new char [dtsz + 1];

	fs.read(data, dtsz);
	fs.close();

	itd1 = data;
	itd2 = itd1 + dtsz;
    }
}

#ifdef WITH_NET

/*
bool Network::RecvMessage(const Network::Socket & csd, QueueMessage & msg, bool debug)
{
    u16 head;
    msg.type = 0;

    if(csd.Recv(reinterpret_cast<char *>(&head), sizeof(head)))
    {
	SwapBE16(head);

	// check id
	if((0xFF00 & Network::proto) != (0xFF00 & head))
	{
	    if(debug) std::cerr << "Network::QueueMessage::Recv: " << "unknown packet id: 0x" << std::hex << head << std::endl;
	    return false;
	}

	// check ver
	if((0x00FF & Network::proto) > (0x00FF & head))
	{
	    if(debug) std::cerr << "Network::QueueMessage::Recv: " << "obsolete protocol ver: 0x" << std::hex << (0x00FF & head) << std::endl;
	    return false;
	}

	u32 size;

	if(csd.Recv(reinterpret_cast<char *>(&msg.type), sizeof(msg.type)) &&
	   csd.Recv(reinterpret_cast<char *>(&size), sizeof(size)))
	{
	    msg.type = SDL_SwapBE16(msg.type);
	    size = SDL_SwapBE32(size);

	    if(size > msg.dtsz)
	    {
		delete [] msg.data;
		msg.data = new char [size + 1];
        	msg.dtsz = size;
	    }

	    msg.itd1 = msg.data;
	    msg.itd2 = msg.itd1 + size;

	    return size ? csd.Recv(msg.data, size) : true;
	}
    }
    return false;
}

bool Network::SendMessage(const Network::Socket & csd, const QueueMessage & msg)
{
    // raw data
    if(0 == msg.type)
	return msg.Size() ? csd.Send(reinterpret_cast<const char *>(msg.data), msg.Size()) : false;

    u16 head = Network::proto;
    u16 sign = msg.type;
    u32 size = msg.Size();

    SwapBE16(head);
    SwapBE16(sign);
    SwapBE32(size);

    return csd.Send(reinterpret_cast<const char *>(&head), sizeof(head)) &&
           csd.Send(reinterpret_cast<const char *>(&sign), sizeof(sign)) &&
           csd.Send(reinterpret_cast<const char *>(&size), sizeof(size)) &&
           (size ? csd.Send(msg.data, msg.Size()) : true);
}
*/

Network::Socket::Socket() : sd(NULL), sdset(NULL), status(0)
{
}

Network::Socket::Socket(const TCPsocket csd) : sd(NULL), sdset(NULL), status(0)
{
    Assign(csd);
}

Network::Socket::Socket(const Socket &) : sd(NULL), sdset(NULL), status(0)
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

void Network::Socket::Assign(const TCPsocket csd)
{
    if(sd) Close();

    if(csd)
    {
	sd = csd;
	sdset = SDLNet_AllocSocketSet(1);
	if(sdset) SDLNet_TCP_AddSocket(sdset, sd);
    }
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

bool Network::Socket::Ready(void) const
{
    return 0 < SDLNet_CheckSockets(sdset, 1) && 0 < SDLNet_SocketReady(sd);
}

bool Network::Socket::Recv(char *buf, int len)
{
    if(sd && buf && len)
    {
	int rcv = 0;

	while((rcv = SDLNet_TCP_Recv(sd, buf, len)) > 0 && rcv < len)
	{
	    buf += rcv;
	    len -= rcv;
	}

	if(rcv != len)
	    status |= ERROR_RECV;
    }

    return ! (status & ERROR_RECV);
}

bool Network::Socket::Send(const char* buf, int len)
{
    if(sd && len != SDLNet_TCP_Send(sd, (void*) buf, len))
	status |= ERROR_SEND;

    return ! (status & ERROR_SEND);
}

bool Network::Socket::Recv(u32 & v)
{
    if(Recv(reinterpret_cast<char*>(&v), sizeof(v)))
    {
        SwapBE32(v);
        return true;
    }
    return false;
}

bool Network::Socket::Send(const u32 & v0)
{
    u32 v = v0;
    SwapBE32(v);

    return Send(reinterpret_cast<char*>(&v), sizeof(v));
}

bool Network::Socket::Open(IPaddress & ip)
{
    Assign(SDLNet_TCP_Open(&ip));

    if(! sd)
	std::cerr << "Network::Socket::Open: " << Network::GetError() << std::endl;

    return sd;
}

bool Network::Socket::isValid(void) const
{
    return sd && 0 == status;
}

void Network::Socket::Close(void)
{
    if(sd)
    {
	if(sdset)
	{
	    SDLNet_TCP_DelSocket(sdset, sd);
	    SDLNet_FreeSocketSet(sdset);
	    sdset = NULL;
	}
	SDLNet_TCP_Close(sd);
	sd = NULL;
    }
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
