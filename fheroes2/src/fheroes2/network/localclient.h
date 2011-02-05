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

#ifndef H2LOCALCLIENT_H
#define H2LOCALCLIENT_H

#include "gamedefs.h"

#ifdef WITH_NET

#include "network.h"

class heroes;
class Castle;
class Kingdom;
namespace Army { class army_t; }
namespace Battle2 { class Arena; class Result; }

class FH2LocalClient : public FH2Client
{
public:
    static FH2LocalClient & Get(void);

    ~FH2LocalClient(){};

    int Main(void);
    bool Connect(const std::string &, u16);
    void Logout(const std::string &);

    const Player* FindPlayer(u8 color) const;

    static void SendCastleBuyBuilding(const Castle &, u32);
    static void SendCastleRecruitHero(const Castle &, const Heroes &);
    static void SendCastleBuyBoat(const Castle &, u16);
    static void SendCastleRecruitMonster(const Castle &, u32, u16);

    static void SendMarketSellResource(const Kingdom &, u8, u32, bool);
    static void SendMarketBuyResource(const Kingdom &, u8, u32, bool);

    static void SendHeroesBuyMagicBook(const Heroes &);
    static void SendHeroesSwapArtifacts(const Heroes &, u8, const Heroes &, u8);

    static void SendArmyUpgradeTroop(const Army::army_t &, u8);
    static void SendArmyDismissTroop(const Army::army_t &, u8);
    static void SendArmySwapTroops(const Army::army_t &, u8, const Army::army_t &, u8);
    static void SendArmySplitTroop(const Army::army_t &, u8, const Army::army_t &, u8, u16);
    static void SendArmyJoinTroops(const Army::army_t &, u8, const Army::army_t &, u8);
    static void SendArmyCombatFormation(const Army::army_t &);

    static void SendUpdateBattleOnlySettings(const BattleOnly &);


    bool ConnectionChat(void);
    bool GetCurrentMapInfo(void);
    bool GetPlayersInfo(void);
    bool GetGameType(void);
    bool GetLoadMaps(bool, bool);
    bool ScenarioInfoDialog(void);
    bool Prepare4BattleOnly(void);
    bool SendWait(QueueMessage &, u16, const char*);
    bool WaitSend(QueueMessage &, u16, const char*);
    bool SendStartBattleOnly(void);
    bool StartGame(void);

    void MsgUpdatePlayers(QueueMessage &);
    void PopPlayersInfo(QueueMessage &);

    std::string server;
    std::vector<Player> players;
    u32 admin_id;

private:
    FH2LocalClient();
};

#endif
#endif
