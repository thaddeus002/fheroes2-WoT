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

#include <string>
#include <iostream>
#include "types.h"

#ifdef WITH_NET
#include "SDL_net.h"

class QueueMessage;

namespace Network
{
    bool		Init(void);
    void		Quit(void);
    bool		ResolveHost(IPaddress &, const char*, u16);
    const char*		GetError(void);

    class Socket
    {
    public:
	Socket();
	Socket(const TCPsocket);
	~Socket();

	void		Assign(const TCPsocket);

	bool		Ready(void) const;

        bool            Recv(char*, int);
        bool            Send(const char*, int);

        bool            Recv(u32 &);
        bool            Send(const u32 &);

	u32		Host(void) const;
	u16		Port(void) const;

	bool		Open(IPaddress &);
	bool		isValid(void) const;
	void		Close(void);

    protected:
	Socket(const Socket &);
	Socket &	operator= (const Socket &);

	TCPsocket	 sd;
	SDLNet_SocketSet sdset;
	size_t		 status;
    };

    class Server : public Socket
    {
    public:
	Server();

	TCPsocket	Accept(void);
    };
}
#endif

class QueueMessage	// DEPRECATED
{
public:
    QueueMessage();
    ~QueueMessage();

    void	Push(u8);

    bool	Pop(u8 &);
    bool	Pop(s8 &);
    bool	Pop(u16 &);
    bool	Pop(s16 &);
    bool	Pop(u32 &);
    bool	Pop(s32 &);
    bool	Pop(bool &);
    bool	Pop(std::string &);

    void	Load(const char*);

protected:
    void	Resize(size_t);
    size_t	Size(void) const;

    u16		type;
    char*	data;
    char*	itd1;
    char*	itd2;
    size_t	dtsz;
};

#endif
