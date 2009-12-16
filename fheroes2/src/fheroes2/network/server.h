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

#ifndef H2SERVER_H
#define H2SERVER_H

#include "gamedefs.h"

#ifdef WITH_NET

#include <vector>
#include <deque>
#include "network.h"
#include "remoteclient.h"
#include "maps_fileinfo.h"

class FH2Server : public Network::Server
{
public:
    static FH2Server & Get(void);
    static int callbackCreateThread(void *);

    ~FH2Server();

    bool Bind(u16);

    bool IsRun(void) const;
    void Lock(void);
    void Unlock(void);
    void PushMapsFileInfoList(Network::Message &) const;
    void PushPlayersInfo(Network::Message &, u32 exclude = 0) const;
    void PopMapsFileInfoList(Network::Message &);
    u8 GetPlayersColors(void) const;
    void ResetPlayers(u32 first_player);
    void SendToAllClients(const Network::Message &, u32 = 0);

    void SetExit(void);
    void SetStartGame(void);

protected:
    void ScanQueue(void);

    FH2Server();
    int Main(void);
    void WaitClients(void);
    void StartGame(void);

    Mutex mutex;
    Timer timer;
    std::deque<MessageID> queue;
    std::vector<FH2RemoteClient> clients;
    bool exit;
    bool start_game;
    u32 admin_id;
    MapsFileInfoList finfo_list;
};

#endif

#endif
