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
#include "sdlnet.h"
#include "color.h"
#include "gamedefs.h"
namespace Maps { struct FileInfo; }

// control_t
enum { CONTROL_NONE = 0, CONTROL_HUMAN = 1, CONTROL_AI = 4, CONTROL_REMOTE = 2 /*, CONTROL_LOCAL = CONTROL_AI | CONTROL_HUMAN */ };

struct Player
{
    Player(u8 col = Color::NONE);

    bool isID(u32) const;
    bool isColor(u8) const;
    bool isName(const std::string &) const;
    void SetControl(u8);

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
};

class Players : public std::vector<Player*>
{
public:
    Players();
    ~Players();

    void Init(u8 colors);
    void Init(const Maps::FileInfo &);
    void clear(void);

    bool isFriends(u8 player, u8 colors) const;

    void SetPlayerRace(u8 color, u8 race);
    void SetPlayerControl(u8 color, u8 ctrl);

    void SetHumanColors(u8 colors);

    u8   GetPlayerControl(u8 color) const;
    u8   GetPlayerRace(u8 color) const;

    static Player* Get(u8 color);

    u8 GetColors(u8 control = 0xFF, bool strong = false) const;

    void SetStartGame(void);

    std::string String(void) const;
};

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

	void RedrawInfo(void) const;
	bool QueueEventProcessing(void);

	bool show_name;
	bool show_race;
	bool show_swap;
    };
}

#endif
