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

#include <vector>

#include "maps.h"
#include "difficulty.h"
#include "color.h"
#include "race.h"
#include "kingdom.h"
#include "gamedefs.h"

namespace Maps
{

class FileInfo
{

public:
    FileInfo();

    bool Read(const std::string &filename);

    const std::string & FileMaps(void) const;
    const std::string & Name(void) const;
    const std::string & Description(void) const;

    u8 KingdomColors(void) const;
    u8 AllowColors(void) const;
    u8 ConditionsWins(void) const;
    u8 ConditionsLoss(void) const;

    Race::race_t KingdomRace(const Color::color_t color) const;

    bool PlayWithHeroes(void) const;

    mapsize_t SizeMaps(void) const;
    Difficulty::difficulty_t Difficulty(void) const;

    void SetKingdomColors(const u8 colors);
    void SetKingdomRace(const Color::color_t color, const Race::race_t race);

protected:
    static Race::race_t ByteToRace(u8 byte);

private:
    std::string file;
    std::string name;
    std::string description;

    mapsize_t size;
    Difficulty::difficulty_t difficulty;

    u8 kingdom_colors;
    u8 allow_colors;
    u8 rnd_colors;
    u8 conditions_wins;
    u8 conditions_loss;

    bool with_heroes;

    std::vector<Race::race_t> races;
};

};

#endif
