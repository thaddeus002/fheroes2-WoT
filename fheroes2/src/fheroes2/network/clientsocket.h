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

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#ifdef WITH_NET

#include "gamedefs.h"
#include "thread.h"
#include "network.h"
#include "bitmodes.h"

class FH2Server;

enum status_t
{
    ST_CONNECT          = 0x0001,
    ST_ADMIN            = 0x0008,
    ST_SHUTDOWN         = 0x0010,
};

class ClientSocket : public Network::Socket, public BitModes
{
public:
    ClientSocket();

    bool IsConnected(void) const;
    void Join(TCPsocket);
    void RunThread(void);
    void Shutdown(void);

    const std::string & GetName(void) const;
    int  ConnectionChat(void);

    bool IsThreadID(u32) const;
    u32 GetThreadID(void) const;

private:
    static int callbackCreateThread(void *);

    Thread thread;
    Network::Message packet;
    std::string name;
};

#endif
#endif
