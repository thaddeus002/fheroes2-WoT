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

#ifndef H2SERVER_H
#define H2SERVER_H

#include "gamedefs.h"

#ifdef WITH_NET

#include <list>
#include <deque>
#include "network.h"
#include "remoteclient.h"

class FH2Server : public Network::Server
{
public:
    static FH2Server & Get(void);
    static int callbackCreateThread(void *);

    ~FH2Server();

    bool Bind(u16);
    void Exit(void);

    void StorePlayersInfo(std::vector<Player> &);

protected:
    friend class FH2RemoteClient;

    FH2Server();
    int ConnectionChat(void);

    Mutex mutex;
    std::deque<MessageID> queue;
    std::list<FH2RemoteClient> clients;
    bool exit;
    u32 admin_id;
    std::string banner;
};

#endif

#endif
