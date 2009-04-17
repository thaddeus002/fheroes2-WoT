/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
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
#ifndef H2MAPSFILEINFO_H
#define H2MAPSFILEINFO_H

#include <ctime>
#include "maps.h"
#include "difficulty.h"
#include "color.h"
#include "race.h"
#include "gamedefs.h"
#include "kingdom_defines.h"
#include "game_io.h"

namespace Maps
{

class FileInfo
{

public:
    FileInfo();

    bool Read(const std::string &);
    bool ReadBIN(const std::string &);
    bool ReadXML(const std::string &);

    const std::string & FileMaps(void) const;
    const std::string & Name(void) const;
    const std::string & Description(void) const;
    const time_t & Time(void) const;

    u8 KingdomColors(void) const;
    u8 AllowColors(void) const;
    u8 ConditionsWins(void) const;
    u8 ConditionsLoss(void) const;
    u8 Wins1(void) const;
    u8 Wins2(void) const;
    u8 Wins3(void) const;
    u8 Wins4(void) const;
    u8 Loss1(void) const;
    u8 Loss2(void) const;

    bool PlayWithHeroes(void) const;
    const Size & SizeMaps(void) const;
    Difficulty::difficulty_t Difficulty(void) const;
    void SetKingdomColors(const u8 colors);

    Race::race_t KingdomRace(Color::color_t) const;
    void SetKingdomRace(Color::color_t, Race::race_t);

    static bool PredicateForSorting(const FileInfo &, const FileInfo &);

private:
    friend void Game::SaveXML(const std::string &);
    friend void Game::LoadXML(const std::string &);

    std::string file;
    std::string name;
    std::string description;

    Size size;
    Difficulty::difficulty_t difficulty;
    Race::race_t races[KINGDOMMAX];

    u8 kingdom_colors;
    u8 allow_colors;
    u8 rnd_colors;
    u8 conditions_wins;
    u8 wins1;
    u8 wins2;
    u8 wins3;
    u8 wins4;
    u8 conditions_loss;
    u8 loss1;
    u8 loss2;

    time_t localtime;

    bool with_heroes;
};

};

#endif
