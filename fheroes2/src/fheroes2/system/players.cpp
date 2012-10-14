/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include "agg.h"
#include "game.h"
#include "color.h"
#include "race.h"
#include "world.h"
#include "maps_fileinfo.h"
#include "settings.h"
#include "players.h"

namespace
{
    Player* _players[KINGDOMMAX + 1] = { NULL };
    u8 human_colors = 0;

    enum { ST_INGAME = 0x2000 };
}

void PlayerFocusReset(Player* player)
{
    if(player) player->focus.Reset();
}

void PlayerFixMultiControl(Player* player)
{
    if(player && player->control == (CONTROL_HUMAN|CONTROL_AI)) player->control = CONTROL_AI;
}

void PlayerFixRandomRace(Player* player)
{
    if(player && player->race == Race::RAND) player->race = Race::Rand();
}

Player::Player(u8 col) : control(CONTROL_NONE), color(col), race(Race::NONE), friends(col), id(World::GetUniq())
{
    name  = Color::String(color);
}

bool Player::isRemote(void) const
{
    return control & CONTROL_REMOTE;
}

bool Player::isLocal(void) const
{
    return ! isRemote();
}

bool Player::isHuman(void) const
{
    return control & CONTROL_HUMAN;
}

bool Player::isAI(void) const
{
    return control & CONTROL_AI;
}

bool Player::isID(u32 id2) const
{
    return id2 == id;
}

bool Player::isColor(u8 col) const
{
    return col == color;
}

bool Player::isName(const std::string & str) const
{
    return str == name;
}

bool Player::isPlay(void) const
{
    return Modes(ST_INGAME);
}

void Player::SetControl(u8 ctl)
{
    control = ctl;
}

void Player::SetPlay(bool f)
{
    if(f) SetModes(ST_INGAME); else ResetModes(ST_INGAME);
}

StreamBase & operator<< (StreamBase & msg, const Focus & focus)
{
    msg << focus.first;

    switch(focus.first)
    {
	case FOCUS_HEROES: msg << reinterpret_cast<Heroes*>(focus.second)->GetIndex(); break;
	case FOCUS_CASTLE: msg << reinterpret_cast<Castle*>(focus.second)->GetIndex(); break;
	default: msg << static_cast<s32>(-1); break;
    }

    return msg;
}

StreamBase & operator>> (StreamBase & msg, Focus & focus)
{
    s32 index;
    msg >> focus.first >> index;

    switch(focus.first)
    {
	case FOCUS_HEROES: focus.second = world.GetHeroes(index); break;
	case FOCUS_CASTLE: focus.second = world.GetCastle(index); break;
	default: focus.second = NULL; break;
    }

    return msg;
}

StreamBase & operator<< (StreamBase & msg, const Player & player)
{
    const BitModes & modes = player;

    return msg <<
	modes <<
	player.id <<
	player.control <<
	player.color <<
	player.race <<
	player.friends <<
	player.name <<
	player.focus;
}

StreamBase & operator>> (StreamBase & msg, Player & player)
{
    BitModes & modes = player;

    return msg >>
	modes >>
	player.id >>
	player.control >>
	player.color >>
	player.race >>
	player.friends >>
	player.name >>
	player.focus;
}

Players::Players() : current_color(0)
{
    reserve(KINGDOMMAX);
}

Players::~Players()
{
    clear();
}

void Players::clear(void)
{
    for(iterator it = begin(); it != end(); ++it)
	delete *it;

    std::vector<Player*>::clear();

    for(u8 ii = 0 ;ii < KINGDOMMAX + 1; ++ii)
	_players[ii] = NULL;

    current_color = 0;
    human_colors = 0;
}

void Players::Init(u8 colors)
{
    clear();

    const Colors vcolors(colors);

    for(Colors::const_iterator
	it = vcolors.begin(); it != vcolors.end(); ++it)
    {
	push_back(new Player(*it));
	_players[Color::GetIndex(*it)] = back();
    }

    DEBUG(DBG_GAME, DBG_INFO, "Players: " << String());
}

void Players::Init(const Maps::FileInfo & fi)
{
    if(fi.kingdom_colors)
    {
	clear();
	const Colors vcolors(fi.kingdom_colors);

	Player* first = NULL;

	for(Colors::const_iterator
	    it = vcolors.begin(); it != vcolors.end(); ++it)
	{
	    Player* player = new Player(*it);
	    player->race  = fi.KingdomRace(*it);
	    player->control = CONTROL_AI;
	    player->friends = *it | fi.unions[Color::GetIndex(*it)];

	    if((*it & fi.HumanOnlyColors()) && Settings::Get().GameType(Game::TYPE_MULTI))
		player->control = CONTROL_HUMAN;
	    else
	    if(*it & fi.AllowHumanColors())
		player->control |= CONTROL_HUMAN;

	    if(!first && (player->control & CONTROL_HUMAN))
		first = player;

	    push_back(player);
	    _players[Color::GetIndex(*it)] = back();
	}

	if(first)
	    first->SetControl(CONTROL_HUMAN);

	DEBUG(DBG_GAME, DBG_INFO, "Players: " << String());
    }
    else
    {
	DEBUG(DBG_GAME, DBG_INFO, "Players: " << "unknown colors");
    }
}

Player* Players::Get(u8 color)
{
    return _players[Color::GetIndex(color)];
}

bool Players::isFriends(u8 player, u8 colors)
{
    const Player* ptr = Get(player);
    return ptr ? ptr->friends & colors : false;
}

void Players::SetPlayerRace(u8 color, u8 race)
{
    Player* player = Get(color);

    if(player)
	player->race = race;
}

void Players::SetPlayerControl(u8 color, u8 ctrl)
{
    Player* player = Get(color);

    if(player)
	player->control = ctrl;
}

u8 Players::GetColors(u8 control, bool strong) const
{
    u8 res = 0;

    for(const_iterator it = begin(); it != end(); ++it)
	if(control == 0xFF ||
    	    (strong && (*it)->control == control) ||
    	    (!strong && ((*it)->control & control))) res |= (*it)->color;

    return res;
}

u8 Players::GetActualColors(void) const
{
    u8 res = 0;

    for(const_iterator it = begin(); it != end(); ++it)
	if((*it)->isPlay()) res |= (*it)->color;

    return res;
}

Player* Players::GetCurrent(void)
{
    return Get(current_color);
}

const Player* Players::GetCurrent(void) const
{
    return Get(current_color);
}

u8 Players::GetPlayerFriends(u8 color)
{
    const Player* player = Get(color);
    return player ? player->friends : 0;
}

u8 Players::GetPlayerControl(u8 color)
{
    const Player* player = Get(color);
    return player ? player->control : CONTROL_NONE;
}

u8 Players::GetPlayerRace(u8 color)
{
    const Player* player = Get(color);
    return player ? player->race : Race::NONE;
}

bool Players::GetPlayerInGame(u8 color)
{
    const Player* player = Get(color);
    return player && player->isPlay();
}

void Players::SetPlayerInGame(u8 color, bool f)
{
    Player* player = Get(color);
    if(player) player->SetPlay(f);
}

void Players::SetHumanColors(u8 cols) /* remove: server.cpp */
{
    for(iterator it = begin(); it != end(); ++it)
	(*it)->control = (*it)->color & cols ? CONTROL_HUMAN : CONTROL_AI;
}

void Players::SetStartGame(void)
{
    for_each(begin(), end(), std::bind2nd(std::mem_fun(&Player::SetPlay), true));
    for_each(begin(), end(), std::ptr_fun(&PlayerFocusReset));
    for_each(begin(), end(), std::ptr_fun(&PlayerFixRandomRace));
    for_each(begin(), end(), std::ptr_fun(&PlayerFixMultiControl));

    current_color = Color::NONE;
    human_colors = Color::NONE;

    DEBUG(DBG_GAME, DBG_INFO, String());
}

u8 Players::HumanColors(void)
{
    if(0 == human_colors)
	human_colors = Settings::Get().GetPlayers().GetColors(CONTROL_HUMAN, true);
    return human_colors;
}

u8 Players::FriendColors(void)
{
    u8 colors = 0;
    const Players & players = Settings::Get().GetPlayers();

    if(players.current_color & Players::HumanColors())
    {
        const Player* player = players.GetCurrent();
        if(player)
            colors = player->friends;
    }
    else
        colors = Players::HumanColors();

    return colors;
}

std::string Players::String(void) const
{
    std::ostringstream os;
    os << "Players: ";

    for(const_iterator
	it = begin(); it != end(); ++it)
    {
	os << Color::String((*it)->color) << "(" << Race::String((*it)->race) << ", ";

	switch((*it)->control)
	{
	    case CONTROL_AI|CONTROL_HUMAN:
	    os << "ai|human";
	    break;

	    case CONTROL_AI:
	    os << "ai";
	    break;

	    case CONTROL_HUMAN:
	    os << "human";
	    break;

	    case CONTROL_REMOTE:
	    os << "remote";
	    break;

	    default:
	    os << "unknown";
	    break;
	}

	os << ")" << ", ";
    }

    return os.str();
}

StreamBase & operator<< (StreamBase & msg, const Players & players)
{
    msg << players.GetColors() << players.current_color;

    for(Players::const_iterator
	it = players.begin(); it != players.end(); ++it)
        msg << (**it);

    return msg;
}

StreamBase & operator>> (StreamBase & msg, Players & players)
{
    u8 colors, current;

    msg >> colors >> current;

    players.clear();
    players.current_color = current;
    const Colors vcolors(colors);

    for(u8 ii = 0; ii < vcolors.size(); ++ii)
    {
	Player* player = new Player();
	msg >> *player;
	_players[Color::GetIndex(player->color)] = player;
	players.push_back(player);
    }

    return msg;
}

bool Interface::PlayerInfo::operator== (const Player* p) const
{
    return player == p;
}

Interface::PlayersInfo::PlayersInfo(bool name, bool race, bool swap) : show_name(name), show_race(race), show_swap(swap)
{
    reserve(KINGDOMMAX);
}

void Interface::PlayersInfo::UpdateInfo(Players & players, const Point & pt1, const Point & pt2)
{
    const Sprite & sprite = AGG::GetICN(ICN::NGEXTRA, 3);

    clear();

    for(Players::iterator
        it = players.begin(); it != players.end(); ++it)
    {
        const u8 current = std::distance(players.begin(), it);
        PlayerInfo info;

        info.player = *it;
        info.rect1  = Rect(pt1.x + Game::GetStep4Player(current, sprite.w(), players.size()), pt1.y, sprite.w(), sprite.h());
        info.rect2  = Rect(pt2.x + Game::GetStep4Player(current, sprite.w(), players.size()), pt2.y, sprite.w(), sprite.h());

        push_back(info);
    }

    for(iterator
        it = begin(); it != end(); ++it)
    {
	if((it + 1) != end())
	{
	    const Rect & rect1 = (*it).rect2;
	    const Rect & rect2 = (*(it + 1)).rect2;
	    const Sprite & sprite = AGG::GetICN(ICN::ADVMCO, 8);

	    (*it).rect3 = Rect(rect1.x + rect1.w + (rect2.x - (rect1.x + rect1.w)) / 2 - 5, rect1.y + rect1.h + 20, sprite.w(), sprite.h());
	}
    }
}

Player* Interface::PlayersInfo::GetFromOpponentClick(const Point & pt)
{
    for(iterator it = begin(); it != end(); ++it)
        if((*it).rect1 & pt) return (*it).player;

    return NULL;
}

Player* Interface::PlayersInfo::GetFromOpponentNameClick(const Point & pt)
{
    for(iterator it = begin(); it != end(); ++it)
        if(Rect((*it).rect1.x, (*it).rect1.y + (*it).rect1.h, (*it).rect1.w, 10)  & pt) return (*it).player;

    return NULL;
}

Player* Interface::PlayersInfo::GetFromOpponentChangeClick(const Point & pt)
{
    for(iterator it = begin(); it != end(); ++it)
        if((*it).rect3 & pt) return (*it).player;

    return NULL;
}

Player* Interface::PlayersInfo::GetFromClassClick(const Point & pt)
{
    for(iterator it = begin(); it != end(); ++it)
	if((*it).rect2 & pt) return (*it).player;

    return NULL;
}

void Interface::PlayersInfo::RedrawInfo(bool show_play_info) const /* show_play_info: show game info with color status (play/not play) */
{
    const Settings & conf = Settings::Get();
    const Maps::FileInfo & fi = conf.CurrentFileInfo();

    const u8 humans_colors = conf.GetPlayers().GetColors(CONTROL_HUMAN, true);
    u8 index = 0;

    for(const_iterator it = begin(); it != end(); ++it)
    {
	const Player & player = *((*it).player);
	const Rect & rect1 = (*it).rect1;
	const Rect & rect2 = (*it).rect2;
	const Rect & rect3 = (*it).rect3;

	// 1. redraw opponents

        // current human
        if(humans_colors & player.color)
            index = 9 + Color::GetIndex(player.color);
        else
        // comp only
        if(fi.ComputerOnlyColors() & player.color)
	{
	    if(show_play_info)
	    {
		index = (player.isPlay() ? 3 : 15) + Color::GetIndex(player.color);
	    }
            else
        	index = 15 + Color::GetIndex(player.color);
        }
	else
        // comp/human
	{
	    if(show_play_info)
	    {
		index = (player.isPlay() ? 3 : 15) + Color::GetIndex(player.color);
	    }
            else
		index = 3 + Color::GetIndex(player.color);
	}

        // wide sprite offset
        if(show_name) index += 24;

        const Sprite & sprite1 = AGG::GetICN(ICN::NGEXTRA, index);
        sprite1.Blit(rect1.x, rect1.y);

        if(show_name)
        {
            // draw player name
            Text name(player.name, Font::SMALL);
    	    name.Blit(rect1.x + (rect1.w - name.w()) / 2, rect1.y + rect1.h - (show_name ? 1 : 14));
	}

	// 2. redraw class
	bool class_color = conf.AllowChangeRace(player.color);

	if(show_play_info)
	{
	    class_color = player.isPlay();
	}

        switch(player.race)
        {
            case Race::KNGT: index = class_color ? 51 : 70; break;
            case Race::BARB: index = class_color ? 52 : 71; break;
            case Race::SORC: index = class_color ? 53 : 72; break;
            case Race::WRLK: index = class_color ? 54 : 73; break;
            case Race::WZRD: index = class_color ? 55 : 74; break;
            case Race::NECR: index = class_color ? 56 : 75; break;
            case Race::MULT: index = 76; break;
            case Race::RAND: index = 58; break;
            default: continue;
        }

        const Sprite & sprite2 = AGG::GetICN(ICN::NGEXTRA, index);
	sprite2.Blit(rect2.x, rect2.y);

	if(show_race)
        {
            const std::string & name = (Race::NECR == player.race ? _("Necroman") : Race::String(player.race));
            Text text(name, Font::SMALL);
            text.Blit(rect2.x + (rect2.w - text.w()) / 2, rect2.y + rect2.h + 2);
        }

	// "swap" sprite

	if(show_swap &&
	   ! conf.QVGA() && (it + 1) != end())
	{
	    const Sprite & sprite3 = AGG::GetICN(ICN::ADVMCO, 8);
	    sprite3.Blit(rect3.x, rect3.y);
	}
    }
}

bool Interface::PlayersInfo::QueueEventProcessing(void)
{
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();
    Player* player = NULL;

    if(le.MousePressRight())
    {
	// opponent
        if(NULL != (player = GetFromOpponentClick(le.GetMouseCursor())))
            Dialog::Message(_("Opponents"), _("This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player."), Font::BIG);
        else
        // class
        if(NULL != (player = GetFromClassClick(le.GetMouseCursor())))
            Dialog::Message(_("Class"), _("This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alignment."), Font::BIG);
    }
    else
    //if(le.MouseClickLeft())
    {
	// select opponent
        if(NULL != (player = GetFromOpponentClick(le.GetMouseCursor())))
        {
    	    const Maps::FileInfo & fi = conf.CurrentFileInfo();
	    Players & players = conf.GetPlayers();

    	    if((player->color & fi.AllowHumanColors()) &&
                (! Settings::Get().GameType(Game::TYPE_MULTI) || ! (player->color & fi.HumanOnlyColors())))
            {
                u8 humans = players.GetColors(CONTROL_HUMAN, true);

                if(conf.GameType(Game::TYPE_MULTI))
                {
                    /* set color */
                    if(!(humans & player->color))
                        player->SetControl(CONTROL_HUMAN);
                    /* reset color */
                    else
                    if(1 < Color::Count(humans))
                        players.SetPlayerControl(player->color, CONTROL_AI|CONTROL_HUMAN);
                }
                else
                // single play
                {
                    players.SetPlayerControl(humans, CONTROL_AI|CONTROL_HUMAN);
                    player->SetControl(CONTROL_HUMAN);
                }
    	    }
	}
	else
	// modify name
        if(show_name && NULL != (player = GetFromOpponentNameClick(le.GetMouseCursor())))
        {
	    std::string res;
	    std::string str = _("%{color} player");
	    String::Replace(str, "%{color}", Color::String(player->color));

	    if(Dialog::InputString(str, res) && ! res.empty())
		player->name = res;
	}
	else
	// select class
	if(NULL != (player = GetFromClassClick(le.GetMouseCursor())))
        {
            if(conf.AllowChangeRace(player->color))
            {
        	switch(player->race)
                {
                    case Race::KNGT: player->race = Race::BARB; break;
                    case Race::BARB: player->race = Race::SORC; break;
                    case Race::SORC: player->race = Race::WRLK; break;
                    case Race::WRLK: player->race = Race::WZRD; break;
                    case Race::WZRD: player->race = Race::NECR; break;
                    case Race::NECR: player->race = Race::RAND; break;
                    case Race::RAND: player->race = Race::KNGT; break;
                    default: break;
                }
	    }
	}
	else
	// change players
	if(show_swap &&
	    !conf.QVGA() && NULL != (player = GetFromOpponentChangeClick(le.GetMouseCursor())))
	{
	    iterator it = std::find(begin(), end(), player);
	    if(it != end() && (it + 1) != end())
	    {
		Players & players = conf.GetPlayers();
		Players::iterator it1 = std::find(players.begin(), players.end(), (*it).player);
		Players::iterator it2 = std::find(players.begin(), players.end(), (*(it + 1)).player);

		if(it1 != players.end() && it2 != players.end())
		{
		    std::swap((*it).player, (*(it + 1)).player);
		    std::swap(*it1, *it2);
		}
	    }
	    else
		player = NULL;
	}
    }

    return player;
}
