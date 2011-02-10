/**************************************************************************
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

#include <algorithm>

#include "game.h"
#include "dialog.h"
#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "localclient.h"
#include "world.h"
#include "kingdom.h"
#include "battle2.h"
#include "spell.h"
#include "battle_stats.h"
#include "heroes_recruits.h"
#include "server.h"

#ifdef WITH_NET

u32 RemoteMessage::sid = 1001;

void RemoteMessage::InitMutex(void)
{
    mutexReady.Create();
}

bool RemoteMessage::IsReady(void) const
{
    mutexReady.Lock();
    bool res = ready;
    mutexReady.Unlock();
    return res;
}

void RemoteMessage::SetReady(void)
{
    mutexReady.Lock();
    ready = true;
    mutexReady.Unlock();
}

FH2Server::FH2Server()
{
    AGG::Cache & cache = AGG::Cache::Get();
    if(! cache.ReadDataDir())
    {
	DEBUG(DBG_NETWORK|DBG_GAME, DBG_WARN, "data files not found");
	Error::Except("create server");
    }

    if(!PrepareMapsFileInfoList(finfoList, true) ||
       !Settings::Get().LoadFileMapsMP2(finfoList.front().file))
	    DEBUG(DBG_NETWORK, DBG_WARN, "maps not found");

    mutexConf.Create();
    mutexSpool.Create();
    mutexModes.Create();
}

FH2Server::~FH2Server()
{
}

FH2Server & FH2Server::Get(void)
{
    static FH2Server fh2server;
    return fh2server;
}

bool FH2Server::Bind(u16 port)
{
    IPaddress ip;
    return Network::ResolveHost(ip, NULL, port) && Open(ip);
}

void FH2Server::SetModes(u32 f)
{
    mutexModes.Lock();
    BitModes::SetModes(f);
    mutexModes.Unlock();
}

bool FH2Server::Modes(u32 f) const
{
    mutexModes.Lock();
    bool res = BitModes::Modes(f);
    mutexModes.Unlock();
    return res;
}

RemoteMessage* FH2Server::FindRemoteReady(void)
{
    mutexSpool.Lock();
    // find ready
    std::list<RemoteMessage>::iterator it = std::find_if(spool.begin(), spool.end(),
				std::mem_fun_ref(&RemoteMessage::IsReady));
    mutexSpool.Unlock();
    return it != spool.end() ? &(*it) : NULL;
}

int FH2Server::Main(void* ptr)
{
    Settings & conf = Settings::Get();
    FH2Server & server = Get();
    SDL::Thread threadConnections;

    DEBUG(DBG_NETWORK, DBG_INFO, "count players: " << static_cast<int>(conf.PreferablyCountPlayers()));

    threadConnections.Create(FH2Server::WaitClients, NULL);

    Recruits heroes;

    while(! server.Modes(ST_SHUTDOWN))
    {
	RemoteMessage* rm = server.FindRemoteReady();
	//
	if(rm)
	{
	    FH2RemoteClient* client = rm->own;

	    if(client)
	    {
		QueueMessage & msg = rm->packet;
		bool clientResult = true;

		switch(Network::GetMsg(msg.GetID()))
		{
		    case MSG_SET_GAMETYPE:
			server.MsgSetGameType(msg, *client);
			break;

		    case MSG_MAPS_LOAD:
			server.MsgLoadMaps(msg, *client);
			break;

		    case MSG_ACCESS_DENIED:
			server.SetModes(ST_FULLHOUSE);
			break;

		    case MSG_LOGOUT:
			server.MsgLogout(msg, *client);
			if(IS_DEBUG(DBG_NETWORK, DBG_TRACE)) server.clients.Dump();
			break;

    		    case MSG_SHUTDOWN:
                        if(client->Modes(ST_ADMIN))
			    server.MsgShutdown(msg);
			break;

    		    case MSG_UPDATE_PLAYERS:
			server.SendUpdatePlayers(msg, 0);
			break;

    		    case MSG_GET_CURRENT_MAP:
			clientResult = client->SendCurrentMapInfo(msg);
                	break;

		    case MSG_SET_CURRENT_MAP:
                        if(client->Modes(ST_ADMIN))
			    server.SetCurrentMap(msg);
			break;

		    case MSG_GET_CURRENT_COLOR:
                	clientResult = client->SendCurrentColor(msg);
                	break;

            	    case MSG_GET_MAPS_LIST:
                    	clientResult = client->Modes(ST_ADMIN) ? client->SendMapsInfoList(msg) :
					client->SendAccessDenied(msg);
                	break;

            	    case MSG_CHANGE_COLORS:
                	if(client->Modes(ST_ADMIN))
			    server.MsgChangeColors(msg);
                	break;

            	    case MSG_CHANGE_RACE:
                	if(client->Modes(ST_ADMIN))
			    server.MsgChangeRaces(msg);
                	break;

		    case MSG_UPDATE_BATTLEONLY:
		    {
			server.mutexConf.Lock();
			Network::UnpackBattleOnly(msg, heroes);
			server.clients.Send2All(msg, 0);
			server.mutexConf.Unlock();
			break;
		    }

		    case MSG_START_BATTLEONLY:
                	if(client->Modes(ST_ADMIN))
			{
			    server.clients.Send2All(msg, 0);
			    Heroes* hero1 = heroes.GetHero1();
			    Heroes* hero2 = heroes.GetHero2();
			    if(hero1 && hero2)
			    {
				Color::color_t color1 = Color::BLUE;
				Color::color_t color2 = Color::RED;

				Kingdom* kingdom1 = &world.GetKingdom(color1);
				Kingdom* kingdom2 = &world.GetKingdom(color2);

				kingdom1->SetControl(Game::REMOTE);
				kingdom2->SetControl(Game::REMOTE);

				conf.SetKingdomRace(color1, hero1->GetRace());
				conf.SetKingdomRace(color2, hero2->GetRace());
				conf.SetPlayersColors(color1 | color2);

				hero1->SetSpellPoints(hero1->GetMaxSpellPoints());
				hero1->Recruit(color1, Point(5, 5));
				hero2->SetSpellPoints(hero2->GetMaxSpellPoints());
				hero2->Recruit(color2, Point(5, 6));

				Battle2::Loader(hero1->GetArmy(), hero2->GetArmy(), hero1->GetIndex() + 1);
			    }
			    server.MsgShutdown(msg);
			}
			break;

		    default:
			DEBUG(DBG_NETWORK, DBG_INFO, "unused: " << Network::GetMsgString(msg.GetID()));
			break;
		}

		if(!clientResult)
		{
		    DEBUG(DBG_NETWORK, DBG_WARN, "client shutdown: " << "id: 0x" << std::hex << client->player_id);
		    client->ShutdownThread();
		}

		server.RemoveMessage(*rm);
	    }
	}

	DELAY(10);
    }

    server.clients.Shutdown();

    if(threadConnections.IsRun()) threadConnections.Wait();

    server.Close();

    return 1;
}

bool FH2Server::BattleRecvTurn(u8 color, const Battle2::Stats & b, const Battle2::Arena & arena, Battle2::Actions & a)
{
    DEBUG(DBG_NETWORK, DBG_INFO, "color: " << static_cast<int>(color));

    FH2RemoteClient* client = clients.GetClient(color);
    if(!client)
    {
	clients.Dump();
	Error::Except("FH2Server::BattleRecvTurn");
	return false;
    }

    QueueMessage msg(MSG_BATTLE_TURN);
    msg.Push(b.GetID());

    // send battle turn
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << b.GetID() << ", send turn");
    if(!client->Send(msg)) return false;

    bool exit = false;

    while(! Modes(ST_SHUTDOWN) && ! exit)
    {
	RemoteMessage* rm = FindRemoteReady();
	//
	if(rm)
	{
	    FH2RemoteClient* client = rm->own;

	    if(client)
	    {
		QueueMessage & msg = rm->packet;

		switch(Network::GetMsg(msg.GetID()))
		{
		    case MSG_BATTLE_AUTO:
		    case MSG_BATTLE_CAST:
		    case MSG_BATTLE_SKIP:
		    case MSG_BATTLE_END_TURN:
			exit = true;
			a.push_back(msg);
			break;

		    case MSG_BATTLE_MOVE:
		    case MSG_BATTLE_ATTACK:
		    case MSG_BATTLE_DEFENSE:
		    case MSG_BATTLE_DAMAGE:
		    case MSG_BATTLE_MORALE:
		    case MSG_BATTLE_LUCK:
		    case MSG_BATTLE_CATAPULT:
		    case MSG_BATTLE_TOWER:
		    case MSG_BATTLE_RETREAT:
		    case MSG_BATTLE_SURRENDER:
			a.push_back(msg);
			break;

		    default:
			DEBUG(DBG_NETWORK, DBG_INFO, "unused: " << Network::GetMsgString(msg.GetID()));
			break;
		}

		RemoveMessage(*rm);
	    }
	}
	DELAY(10);
    }

    // send board
    DEBUG(DBG_NETWORK, DBG_INFO, "id: 0x" << b.GetID() << ", send board");
    if(! BattleSendBoard(color, arena)) return false;

    return true;
}

int FH2Server::WaitClients(void* ptr)
{
    FH2Server & server = Get();
    FH2RemoteClients & clients = server.clients;
    size_t preferablyPlayers = Settings::Get().PreferablyCountPlayers();

    DEBUG(DBG_NETWORK, DBG_INFO, "start");

    // wait players
    while(! server.Modes(ST_SHUTDOWN))
    {
	if(TCPsocket csd = server.Accept())
    	{
	    size_t connectedPlayers = clients.GetConnected();

    	    // request admin
	    FH2RemoteClient* client = clients.GetAdmin();

	    // check count players
    	    if(preferablyPlayers <= connectedPlayers ||
		server.Modes(ST_FULLHOUSE))
	    {
		DEBUG(DBG_NETWORK, DBG_INFO, "full house");
		Socket sct(csd);
		sct.Close();
	    }
	    else
	    // connect
    	    {
		client = clients.GetNewClient();

		client->Reset();

		// first player: set admin mode
		if(0 == connectedPlayers)
		    client->SetModes(ST_ADMIN);

        	client->Assign(csd);
    		client->RunThread();

		DELAY(200);
	    }
	}

	DELAY(100);
    }

    DEBUG(DBG_NETWORK, DBG_INFO, "shutdown");

    return 1;
}

bool FH2Server::WaitReadyClients(u32 ms)
{
    const u16 magick = Rand::Get(0xFFFF);
    u8 remains = clients.GetPlayersColors();
    SDL::Time time;

    QueueMessage msg(MSG_PING);
    msg.Push(magick);

    DEBUG(DBG_NETWORK, DBG_INFO, "waiting " << ms);

    time.Start();

    while(remains)
    {
	time.Stop();
	if(ms && time.Get() >= ms) break;

	clients.Send2All(msg, 0);

	DELAY(100);

	while(RemoteMessage* rm = FindRemoteReady())
	{
	    QueueMessage & msg = rm->packet;

	    if(msg.GetID() == MSG_PING)
	    {
		u16 reply;
		msg.Pop(reply);
		if(reply == magick)
		    remains &= (~rm->own->player_color);
	    }

	    RemoveMessage(*rm);
	}
    }

    if(remains)
    {
	DEBUG(DBG_NETWORK, DBG_WARN, "false");
    }
    else
    {
	DEBUG(DBG_NETWORK, DBG_INFO, "reply " << time.Get());
    }

    return remains ? false : true;
}

RemoteMessage & FH2Server::GetNewMessage(FH2RemoteClient & rc)
{
    mutexSpool.Lock();
    spool.push_back(RemoteMessage(&rc));
    spool.back().InitMutex();
    mutexSpool.Unlock();
    return spool.back();
}

void FH2Server::UpdateColors(void)
{
    Settings & conf = Settings::Get();

    const u8 colors = clients.GetPlayersColors();

    mutexConf.Lock();
    conf.SetPlayersColors(colors);
    mutexConf.Unlock();

    DEBUG(DBG_NETWORK, DBG_INFO, static_cast<int>(colors));
}

u8 FH2Server::GetFreeColor(bool admin)
{
    Settings & conf = Settings::Get();
    u8 res = 0;

    if(Game::STANDARD & conf.GameType())
    {
        // check color
        mutexConf.Lock();
        res = Color::GetFirst(conf.CurrentFileInfo().human_colors & (~clients.GetPlayersColors()));
        mutexConf.Unlock();
    }
    else
    if(Game::BATTLEONLY & conf.GameType())
    {
        res = admin ? Color::BLUE : Color::RED;
    }

    DEBUG(DBG_NETWORK, DBG_INFO, Color::String(res));

    return res;
}

void FH2Server::RemoveMessage(const RemoteMessage & rm)
{
    mutexSpool.Lock();
    std::list<RemoteMessage>::iterator it = std::find(spool.begin(), spool.end(), rm);
    if(it != spool.end()) spool.erase(it);
    mutexSpool.Unlock();
}

void FH2Server::PushPlayersInfo(QueueMessage & msg) const
{
    clients.PushPlayersInfo(msg);

    DEBUG(DBG_NETWORK, DBG_INFO, "count: " << clients.GetConnected());
    if(IS_DEBUG(DBG_NETWORK, DBG_TRACE)) clients.Dump();
}

void FH2Server::PushMapsFileInfoList(QueueMessage & msg) const
{
    mutexConf.Lock();
    msg.Push(static_cast<u16>(finfoList.size()));
    for(MapsFileInfoList::const_iterator
	it = finfoList.begin(); it != finfoList.end(); ++it)
	    Network::PacketPushMapsFileInfo(msg, *it);
    DEBUG(DBG_NETWORK, DBG_INFO, std::dec << finfoList.size() << "maps, " << "size: " << msg.DtSz() << " bytes");
    mutexConf.Unlock();
}

void FH2Server::SendUpdatePlayers(QueueMessage & msg, u32 exclude)
{
    msg.Reset();
    msg.SetID(MSG_UPDATE_PLAYERS);
    PushPlayersInfo(msg);
    clients.Send2All(msg, exclude);

    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
}

void FH2Server::ResetPlayers(void)
{
    Settings & conf = Settings::Get();
    clients.ResetPlayersColors();
    const u8 colors = clients.ResetPlayersColors();

    mutexConf.Lock();
    conf.SetPlayersColors(0);
    conf.SetPlayersColors(colors);
    mutexConf.Unlock();

    DEBUG(DBG_NETWORK, DBG_INFO, "colors: " << static_cast<int>(colors));
}

void FH2Server::SetCurrentMap(QueueMessage & msg)
{
    Settings & conf = Settings::Get();
    std::string str;

    msg.Pop(str);

    if(conf.LoadFileMapsMP2(str))
    {
	msg.Reset();
	msg.SetID(MSG_SET_CURRENT_MAP);

	mutexConf.Lock();
	Network::PacketPushMapsFileInfo(msg, conf.CurrentFileInfo());
	DEBUG(DBG_NETWORK, DBG_INFO, "file: " << conf.CurrentFileInfo().file << ", "
					<< "size: " << std::dec << msg.DtSz() << " bytes");
	mutexConf.Unlock();

	clients.Send2All(msg, 0);
	ResetPlayers();
	SendUpdatePlayers(msg, 0);
    }
}

void FH2Server::MsgChangeColors(QueueMessage & msg)
{
    Settings & conf = Settings::Get();
    u8 from, to;

    msg.Pop(from);
    msg.Pop(to);

    if(conf.AllowColors(from) && conf.AllowColors(to))
    {
	if(! clients.ChangeColors(from, to))
	    UpdateColors();
	DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
	SendUpdatePlayers(msg, 0);
    }
}

void FH2Server::MsgChangeRaces(QueueMessage & msg)
{
    Settings & conf = Settings::Get();
    u8 color, race;

    msg.Pop(color);
    msg.Pop(race);

    if(conf.AllowChangeRace(color))
    {
        conf.SetKingdomRace(color, race);
        msg.Reset();
        msg.SetID(MSG_CHANGE_RACE);
        clients.ChangeRace(color, race);
        Network::PackRaceColors(msg);
	DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
        clients.Send2All(msg, 0);
    }
}

void FH2Server::MsgLogout(QueueMessage & msg, FH2RemoteClient & client)
{
    std::string str, err;

    msg.Pop(err);

    // send message
    msg.Reset();
    msg.SetID(MSG_MESSAGE);
    str = "logout player: " + client.player_name;
    if(err.size())
    {
	str.append("\n");
	str.append(err);
    }
    msg.Push(str);

    DEBUG(DBG_NETWORK, DBG_INFO, str);
    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");

    clients.Send2All(msg, client.player_id);

    if(client.Modes(ST_ADMIN)) clients.SetNewAdmin(client.player_id);

    client.SetModes(ST_SHUTDOWN);

    DELAY(100);

    // send players
    UpdateColors();
    SendUpdatePlayers(msg, client.player_id);

    // FIXME: logout
    // if(Modes(ST_INGAME)) world.GetKingdom(player_color).SetControl(Game::AI); // FIXME: MSGLOGOUT: INGAME AND CURRENT TURN?    
}

void FH2Server::MsgShutdown(QueueMessage & msg)
{
    // send message
    msg.Reset();
    msg.SetID(MSG_MESSAGE);
    std::string str = "server shutdown";
    msg.Push(str);

    DEBUG(DBG_NETWORK, DBG_INFO, str);
    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");

    clients.Send2All(msg, 0);

    SetModes(ST_SHUTDOWN);
}

void FH2Server::MsgSetGameType(QueueMessage & msg, FH2RemoteClient & client)
{
    Settings & conf = Settings::Get();
    u8 type;
    msg.Pop(type);

    if(client.Modes(ST_ADMIN))
    {
	if(type & Game::STANDARD)
	    conf.SetGameType(Game::STANDARD | Game::NETWORK);
	else
	if(type & Game::BATTLEONLY)
	{
	    conf.SetGameType(Game::BATTLEONLY | Game::NETWORK);
	    world.NewMaps(10, 10);
	}

	DEBUG(DBG_NETWORK, DBG_INFO, "0x" << std::hex << static_cast<int>(conf.GameType()));
    }
}

void FH2Server::MsgLoadMaps(QueueMessage & msg, FH2RemoteClient & client)
{
    Settings & conf = Settings::Get();
    u8 toAll, forceLoad;
    msg.Pop(forceLoad);
    msg.Pop(toAll);

    // load maps
    if(client.Modes(ST_ADMIN) && forceLoad)
    {
	mutexConf.Lock();

	conf.SetPlayersColors(clients.GetPlayersColors());

	if(Game::STANDARD & conf.GameType())
	{
	    // disable connect
	    // SetModes(ST_FULLHOUSE);
	    //world.LoadMaps(conf.MapsFile());
	}
	else
	if(Game::BATTLEONLY & conf.GameType())
	{
	    // FIXME
	    //world.NewMaps(10, 10);
	}

	// set control
	Color::Colors colors = Color::GetColors(conf.PlayersColors());
	for(Color::Colors::iterator
	    it = colors.begin(); it != colors.end(); ++it)
    	    world.GetKingdom(*it).SetControl(Game::REMOTE);

	mutexConf.Unlock();
    }

    msg.Reset();
    msg.SetID(MSG_MAPS_LOAD);
    Game::IO::SaveBIN(msg);

    DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");

    if(client.Modes(ST_ADMIN) && toAll)
	clients.Send2All(msg, 0);
    else
	client.Send(msg);
}

bool FH2Server::BattleSendAction(u8 color, QueueMessage & msg)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	DEBUG(DBG_NETWORK, DBG_INFO, Network::GetMsgString(msg.GetID()) << ", "
				<< "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendResult(u8 color, const Battle2::Result & res)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_RESULT);
	msg.Push(res.army1);
	msg.Push(res.army2);
	msg.Push(res.exp1);
	msg.Push(res.exp2);

	DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendAttack(u8 color, const Battle2::Stats & attacker, const Battle2::Stats & defender, u16 dst, const Battle2::TargetsInfo & targets)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_ATTACK);
	msg.Push(attacker.GetID());
	attacker.Pack(msg);
	msg.Push(defender.GetID());
	defender.Pack(msg);
	msg.Push(dst);
	targets.Pack(msg);

	DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendBoard(u8 color, const Battle2::Arena & a)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_BOARD);
	a.PackBoard(msg);

	DEBUG(DBG_NETWORK, DBG_INFO, "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendSpell(u8 color, u16 who, u16 dst, const Spell & spell, const Battle2::TargetsInfo & targets)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_CAST);
	msg.Push(spell());
	msg.Push(who);
	msg.Push(dst);
	msg.Push(color);
	targets.Pack(msg);

	DEBUG(DBG_NETWORK, DBG_INFO, spell.GetName() << " " << "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendTeleportSpell(u8 color, u16 src, u16 dst)
{
    Spell spell(Spell::TELEPORT);

    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_CAST);
	msg.Push(spell());
	msg.Push(src);
	msg.Push(dst);

	DEBUG(DBG_NETWORK, DBG_INFO, spell.GetName() << " " << "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendMirrorImageSpell(u8 color, u16 src, u16 dst, const Battle2::Stats & image)
{
    Spell spell(Spell::MIRRORIMAGE);

    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_CAST);
	msg.Push(spell());
	msg.Push(src);
	msg.Push(dst);
	msg.Push(image.GetID());
	image.Pack(msg);

	DEBUG(DBG_NETWORK, DBG_INFO, spell.GetName() << " " << "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendSummonElementalSpell(u8 color, const Spell & spell, const Battle2::Stats & elem)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	QueueMessage msg(MSG_BATTLE_CAST);
	msg.Push(spell());
	msg.Push(elem.GetID());
	elem.Pack(msg);

	DEBUG(DBG_NETWORK, DBG_INFO, spell.GetName() << " " << "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}

bool FH2Server::BattleSendEarthQuakeSpell(u8 color, const std::vector<u8> & targets)
{
    FH2RemoteClient* client = clients.GetClient(color);
    if(client)
    {
	Spell spell(Spell::EARTHQUAKE);
	QueueMessage msg(MSG_BATTLE_CAST);
	msg.Push(spell());
	msg.Push(static_cast<u32>(targets.size()));

	for(std::vector<u8>::const_iterator
	    it = targets.begin(); it != targets.end(); ++it)
	    msg.Push(*it);

	DEBUG(DBG_NETWORK, DBG_INFO, spell.GetName() << " " << "size: " << std::dec << msg.DtSz() << " bytes");
	return client->Send(msg);
    }
    return false;
}





Game::menu_t Game::NetworkHost(void)
{
    Settings & conf = Settings::Get();

    if(conf.GameType() & Game::STANDARD)
    {
	// select count players
	const u8 max_players = Game::SelectCountPlayers();

	if(2 > max_players) return Game::MAINMENU;
	conf.SetPreferablyCountPlayers(max_players);

	Display & display = Display::Get();
	Cursor & cursor = Cursor::Get();

	// clear background
	cursor.Hide();
	display.Blit(AGG::GetICN(ICN::HEROES, 0));
	cursor.Show();
	display.Flip();
    }
    else
    if(conf.GameType() & Game::BATTLEONLY)
	conf.SetPreferablyCountPlayers(2);

    // create local server
    FH2Server & server = FH2Server::Get();

    if(! server.Bind(conf.GetPort()))
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }

    SDL::Thread thread;
    thread.Create(FH2Server::Main, NULL);

    // create local client
    const std::string localhost("127.0.0.1");
    FH2LocalClient & client = FH2LocalClient::Get();

    // connect to server
    if(client.Connect(localhost, conf.GetPort()))
    {
	client.SetModes(ST_LOCALSERVER);
	client.Main();
    }
    else
        Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);

    server.SetModes(ST_SHUTDOWN);

    if(0 > thread.Wait())
    {
	Dialog::Message(_("Error"), Network::GetError(), Font::BIG, Dialog::OK);
	return Game::MAINMENU;
    }
    server.Close();

    return QUITGAME;
}

#else
Game::menu_t Game::NetworkHost(void)
{
    Dialog::Message(_("Error"), _("This release is compiled without network support."), Font::BIG, Dialog::OK);
    return MAINMENU;
}
#endif
