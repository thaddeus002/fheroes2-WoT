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

#include "maps.h"
#include "kingdom.h"
#include "gamedefs.h"

namespace Maps
{

class FileInfo
{

public:
    FileInfo(const std::string &filemaps);

    const std::string & GetFileMaps(void) const{ return fileMaps; };
    const std::string & GetName(void) const{ return name; };
    const std::string & GetDescription(void) const{ return description; };
    u8 GetKingdomColors(void) const{ return kingdomColors; };
    u8 GetKingdomCount(void) const{ return kingdomCount; };
    u8 GetAllowColors(void) const{ return allowColors; };
    u8 GetKingdomRace(Kingdom::color_t color) const;
    mapsize_t GetSizeMaps(void) const{ return sizeMaps; };
    u8 GetConditionsWins(void) const{ return conditionsWins; };
    u8 GetConditionsLoss(void) const{ return conditionsLoss; };
    difficulty_t GetDifficulty(void) const{ return mapsDifficulty; };

private:
    std::string fileMaps;
    mapsize_t sizeMaps;
    difficulty_t mapsDifficulty;
    u8 kingdomColors;
    u8 allowColors;
    u8 rndColors;
    u8 kingdomCount;
    u8 conditionsWins;
    u8 conditionsLoss;
    bool withHeroes;
    std::string name;
    std::string description;

    u8 raceKingdom[KINGDOMMAX];
};

};

#endif
