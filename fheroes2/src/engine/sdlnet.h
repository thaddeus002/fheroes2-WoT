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

#ifndef SDLNET_H
#define SDLNET_H

#include <iostream>
#include <string>
#include "types.h"

class QueueMessage
{
public:
    QueueMessage();
    QueueMessage(u16);
    QueueMessage(const QueueMessage &);
    ~QueueMessage();

    QueueMessage & operator= (const QueueMessage &);

    u16		GetID(void) const;
    void	SetID(u16);

    void	Push(u8);
    void	Push(u16);
    void	Push(u32);
    void	Push(const std::string &);
    void	Push(const char*);

    bool	Pop(u8 &);
    bool	Pop(u16 &);
    bool	Pop(u32 &);
    bool	Pop(std::string &);

    void	Reset(void);
    void	Reserve(size_t);
    void	Dump(std::ostream & = std::cerr) const;

    const char*	DtPt(void) const;
    size_t      DtSz(void) const;

protected:
    void	Resize(size_t);
    size_t	Size(void) const;

    u16		type;
    char*		data;
    char*		itd1;
    char*		itd2;
    size_t		dtsz;
};

#ifdef WITH_NET
#include "SDL_net.h"

namespace Network
{
    bool		Init(void);
    void		Quit(void);
    bool		ResolveHost(IPaddress &, const char*, u16);
    const char*		GetError(void);
    void		SetProtocolVersion(u16);

    class Socket
    {
    public:
	Socket();
	Socket(const TCPsocket);
	~Socket();

	void		Assign(const TCPsocket);

	bool		Ready(void) const;

	bool		Recv(char *, size_t) const;
	bool		Send(const char*, size_t) const;

	u32		Host(void) const;
	u16		Port(void) const;

	bool		Open(IPaddress &);
	bool		IsValid(void) const;
	void		Close(void);

    protected:
	Socket(const Socket &);
	Socket &	operator= (const Socket &);

	TCPsocket	sd;
	SDLNet_SocketSet sdset;
    };

    class Message : public QueueMessage
    {
    public:
	Message();
	Message(u16);

	bool	Recv(const Socket &, bool = false);
	bool	Send(const Socket &) const;
    };

    class Server : public Socket
    {
    public:
	Server();

	TCPsocket	Accept(void);
    };
};
#endif
#endif
