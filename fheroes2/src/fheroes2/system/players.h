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

#ifndef H2PLAYERS_H
#define H2PLAYERS_H

#include <string>
#include <vector>
#include "color.h"
#include "bitmodes.h"
#include "gamedefs.h"

namespace Maps { class FileInfo; }

class Castle;
class Heroes;

// control_t
enum { CONTROL_NONE = 0, CONTROL_HUMAN = 1, CONTROL_AI = 4, CONTROL_REMOTE = 2 /*, CONTROL_LOCAL = CONTROL_AI | CONTROL_HUMAN */ };
enum { FOCUS_UNSEL = 0, FOCUS_HEROES = 1, FOCUS_CASTLE = 2 };

struct Focus : std::pair<u8, void*>
{
    Focus() : std::pair<u8, void*>(FOCUS_UNSEL, NULL) {}

    bool	isValid(void) const { return first != FOCUS_UNSEL && second; }

    void	Reset(void) { first = FOCUS_UNSEL; second = NULL; }
    void	Set(Castle* ptr) { first = FOCUS_CASTLE; second = ptr; }
    void	Set(Heroes* ptr) { first = FOCUS_HEROES; second = ptr; }

    Castle*	GetCastle(void) { return first == FOCUS_CASTLE && second ? reinterpret_cast<Castle*>(second) : NULL; }
    Heroes*	GetHeroes(void) { return first == FOCUS_HEROES && second ? reinterpret_cast<Heroes*>(second) : NULL; }
};

class Player : public BitModes
{
public:
    Player(u8 col = Color::NONE);

    bool isID(u32) const;
    bool isColor(u8) const;
    bool isName(const std::string &) const;
    bool isPlay(void) const;

    void SetControl(u8);
    void SetPlay(bool);

    bool isRemote(void) const;
    bool isLocal(void) const;
    bool isHuman(void) const;
    bool isAI(void) const;

    u8		control;
    u8		color;
    u8		race;
    u8		friends;
    std::string	name;
    u32		id;
    Focus	focus;
};

StreamBase & operator<< (StreamBase &, const Player &);
StreamBase & operator>> (StreamBase &, Player &);

class Players : public std::vector<Player*>
{
public:
    Players();
    ~Players();

    void Init(u8 colors);
    void Init(const Maps::FileInfo &);
    void clear(void);

    void	SetHumanColors(u8 colors);
    void	SetStartGame(void);
    u8	 	GetColors(u8 control = 0xFF, bool strong = false) const;
    u8		GetActualColors(void) const;
    std::string	String(void) const;

    Player*		GetCurrent(void);
    const Player*	GetCurrent(void) const;

    static Player*	Get(u8 color);
    static u8		GetPlayerControl(u8 color);
    static u8		GetPlayerRace(u8 color);
    static u8		GetPlayerFriends(u8 color);
    static bool		GetPlayerInGame(u8 color);
    static bool		isFriends(u8 player, u8 colors);
    static void		SetPlayerRace(u8 color, u8 race);
    static void		SetPlayerControl(u8 color, u8 ctrl);
    static void		SetPlayerInGame(u8 color, bool);
    static u8		HumanColors(void);
    static u8 		FriendColors(void);

    u8		current_color;
};

StreamBase & operator<< (StreamBase &, const Players &);
StreamBase & operator>> (StreamBase &, Players &);

namespace Interface
{
    struct PlayerInfo
    {
	PlayerInfo() : player(NULL) {}

	bool operator== (const Player*) const;

	Player*     player;
	Rect        rect1; // opponent
	Rect        rect2; // class
	Rect        rect3; // change
    };

    struct PlayersInfo : std::vector<PlayerInfo>
    {
	PlayersInfo(bool, bool, bool);

	void UpdateInfo(Players &, const Point & opponents, const Point & classes);

	Player* GetFromOpponentClick(const Point & pt);
	Player* GetFromOpponentNameClick(const Point & pt);
	Player* GetFromOpponentChangeClick(const Point & pt);
	Player* GetFromClassClick(const Point & pt);

	void RedrawInfo(bool show_play_info = false) const;
	bool QueueEventProcessing(void);

	bool show_name;
	bool show_race;
	bool show_swap;
    };
}

#endif
