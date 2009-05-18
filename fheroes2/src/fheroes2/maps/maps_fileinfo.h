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
#include <vector>
#include "gamedefs.h"
#include "kingdom_defines.h"
#include "game_over.h"
#include "artifact.h"

namespace Maps
{
    struct FileInfo
    {
    FileInfo();

    bool Read(const std::string &);
    bool ReadBIN(const std::string &);
    bool ReadXML(const std::string &);

    u8 AllowColorsCount(void) const;
    u8 KingdomColorsCount(void) const;

    GameOver::conditions_t ConditionWins(void) const;
    GameOver::conditions_t ConditionLoss(void) const;
    bool WinsCompAlsoWins(void) const;
    bool WinsAllowNormalVictory(void) const;
    Artifact::artifact_t WinsFindArtifact(void) const;
    bool WinsFindUltimateArtifact(void) const;
    u16 WinsSidePart(void) const;
    u32 WinsAccumulateGold(void) const;
    u32 WinsMapsIndexObject(void) const;
    u32 LossMapsIndexObject(void) const;
    u16 LossCountDays(void) const;

    static bool PredicateForSorting(const FileInfo &, const FileInfo &);

    std::string file;
    std::string name;
    std::string description;

    u8 size_w;
    u8 size_h;
    u8 difficulty;
    u8 races[KINGDOMMAX];

    u8 kingdom_colors;
    u8 allow_colors;
    u8 rnd_colors;
    u8 rnd_races;
    u8 conditions_wins;
    u8 wins1;
    u8 wins2;
    u16 wins3;
    u16 wins4;
    u8 conditions_loss;
    u16 loss1;
    u16 loss2;

    time_t localtime;

    bool with_heroes;
    };
};

typedef std::vector<Maps::FileInfo> MapsFileInfoList;

#endif
