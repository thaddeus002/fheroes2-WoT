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

#ifndef H2REMOTECLIENT_H
#define H2REMOTECLIENT_H

#ifdef WITH_NET

#include <vector>
#include "gamedefs.h"
#include "thread.h"
#include "network.h"

class FH2Server;
namespace Battle
{
    class Actions;
    class Arena;
    class Stats;
    struct Result;
    struct TargetInfo;
    struct TargetsInfo;
}

struct FH2RemoteClient : public FH2Client
{
    FH2RemoteClient();

    void InitMutex(void);
    int Main(void);
    bool ConnectionChat(void);
    void CloseConnection(void);

    void RunThread(void);
    void ShutdownThread(void);

    void SetModes(u32);
    bool Modes(u32) const;

    bool SendCurrentMapInfo(QueueMessage &);
    bool SendCurrentColor(QueueMessage &);
    bool SendMapsInfoList(QueueMessage &);
    void MsgChangeColors(void);
    void MsgChangeRace(void);
    void SetGameType(u8 type);

    std::string String(void) const;
    void Reset(void);

    static int callbackCreateThread(void*);

    bool UpdateColors(void);

    SDL::Thread thread;
    SDL::Mutex mutexModes;
};

class FH2RemoteClients
{
public:
    FH2RemoteClients();

    void InitMutex(void);
    size_t GetConnected(void) const;
    FH2RemoteClient* GetAdmin(void);
    FH2RemoteClient* GetNewClient(void);
    u8 GetPlayersColors(void) const;
    void Shutdown(void);
    FH2RemoteClient* GetClient(u8);
    void PushPlayersInfo(QueueMessage &) const;
    void Send2All(const QueueMessage &, u32 exclude);
    u8 ResetPlayersColors(void);
    bool ChangeColors(u8 color1, u8 color2);
    void ChangeRace(u8 color, u8 race);
    void SetNewAdmin(u32 old_rid);
    void Dump(void) const;

private:
    std::vector<FH2RemoteClient> clients;
    SDL::Mutex mutexClients;
};

#endif
#endif
