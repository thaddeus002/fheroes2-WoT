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

#include "network.h"
#include "remoteclient.h"
#include "maps_fileinfo.h"

class Spell;

struct RemoteMessage
{
    RemoteMessage(FH2RemoteClient* ptr) : uid(sid++), own(ptr), ready(false) {};

    void InitMutex(void);
    bool IsReady(void) const;
    void SetReady(void);

    bool operator== (const RemoteMessage & rm) const { return rm.uid == uid; }

    u32 uid;
    FH2RemoteClient* own;
    QueueMessage packet;
    SDL::Mutex mutexReady;
    bool ready;

    static u32 sid;
};

class FH2Server : public Network::Server, public BitModes
{
public:
    static FH2Server & Get(void);

    ~FH2Server();

    bool Bind(u16);
    bool WaitReadyClients(u32 ms);

    void UpdateColors(void);
    u8 GetFreeColor(bool);

    RemoteMessage & GetNewMessage(FH2RemoteClient &);

    void SetModes(u32);
    bool Modes(u32) const;

    void PushPlayersInfo(QueueMessage &) const;
    void PushMapsFileInfoList(QueueMessage &) const;
    void SendUpdatePlayers(QueueMessage &, u32 exclude);
    void ResetPlayers(void);

    static int Main(void*);
    static int WaitClients(void*);

    bool BattleSendAction(u8, QueueMessage &);
    bool BattleSendAttack(u8, const Battle2::Stats &, const Battle2::Stats &, u16, const Battle2::TargetsInfo &);
    bool BattleSendSpell(u8, u16, u16, const Spell &, const Battle2::TargetsInfo &);
    bool BattleSendTeleportSpell(u8, u16, u16);
    bool BattleSendEarthQuakeSpell(u8, const std::vector<u8> &);
    bool BattleSendBoard(u8, const Battle2::Arena &);
    bool BattleSendResult(u8, const Battle2::Result &);
    bool BattleRecvTurn(u8, const Battle2::Stats &, const Battle2::Arena &, Battle2::Actions &);

protected:
    FH2Server();
    RemoteMessage* FindRemoteReady(void);
    void RemoveMessage(const RemoteMessage &);
    void SetCurrentMap(QueueMessage &);
    void MsgChangeColors(QueueMessage &);
    void MsgChangeRaces(QueueMessage &);
    void MsgLogout(QueueMessage &, FH2RemoteClient &);
    void MsgShutdown(QueueMessage &);
    void MsgGetGameType(QueueMessage &, FH2RemoteClient &);
    void MsgLoadMaps(QueueMessage &, FH2RemoteClient &);

    FH2RemoteClients clients;
    MapsFileInfoList finfoList;
    std::list<RemoteMessage> spool;
    SDL::Mutex mutexConf;
    SDL::Mutex mutexSpool;
    SDL::Mutex mutexModes;
};

#endif

#endif
