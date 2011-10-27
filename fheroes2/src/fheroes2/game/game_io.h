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

#ifndef H2GAMEIO_H
#define H2GAMEIO_H

#include "game.h"
#include "sdlnet.h"
#include "maps_fileinfo.h"

namespace Maps {class Tiles; struct Addons; }
class Kingdom;
class Castle;
class Heroes;
class HeroBase;
class Players;

namespace Game
{
    class IO : public QueueMessage
    {
    public:
	IO(){};

	bool LoadSAV(const std::string &);

	static bool SaveBIN(QueueMessage &);
	static bool LoadBIN(QueueMessage &);

	static void PackTile(QueueMessage &, const Maps::Tiles &);
	static void PackTileAddons(QueueMessage &, const Maps::Addons &);
	static void PackKingdom(QueueMessage &, const Kingdom &);
	static void PackCastle(QueueMessage &, const Castle &);
	static void PackHeroes(QueueMessage &, const Heroes &);
	static void PackHeroBase(QueueMessage &, const HeroBase &);
	static void PackPlayers(QueueMessage &, const Players &);

	static void UnpackTile(QueueMessage &, Maps::Tiles &, u32, u16 = -1);
	static void UnpackTileAddons(QueueMessage &, Maps::Addons &, u16 = -1);
	static void UnpackKingdom(QueueMessage &, Kingdom &, u16 = -1);
	static void UnpackCastle(QueueMessage &, Castle &, u16 = -1);
	static void UnpackHeroes(QueueMessage &, Heroes &, u16 = -1);
	static void UnpackHeroBase(QueueMessage &, HeroBase &, u16 = -1);
	static void UnpackPlayers(QueueMessage &, Players &, u16 = -1);

	static std::string last_name;
    };

    bool Save(const std::string &);
    bool Load(const std::string &);
    bool LoadSAV2FileInfo(const std::string &,  Maps::FileInfo &);
}

#endif
