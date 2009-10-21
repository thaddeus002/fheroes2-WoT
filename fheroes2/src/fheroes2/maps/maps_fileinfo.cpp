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

#include <bitset>
#include <cstring>
#include <algorithm>
#include <fstream>
#include "difficulty.h"
#include "color.h"
#include "race.h"
#include "world.h"
#include "settings.h"
#include "dir.h"
#include "maps_fileinfo.h"

#define LENGTHNAME		16
#define LENGTHDESCRIPTION	143

Race::race_t ByteToRace(u8 byte)
{
    switch(byte)
    {
	case 0x00:	return Race::KNGT;
	case 0x01:	return Race::BARB;
	case 0x02:	return Race::SORC;
	case 0x03:	return Race::WRLK;
	case 0x04:	return Race::WZRD;
	case 0x05:	return Race::NECR;
	case 0x06:	return Race::MULT;
	case 0x07:	return Race::RAND;

	default: 	break;
    }

    return Race::BOMG;
}

Maps::FileInfo::FileInfo() : difficulty(Difficulty::EASY),
    kingdom_colors(0), allow_colors(0), rnd_colors(0), rnd_races(0), localtime(0), with_heroes(false)
{
    for(u8 ii = 0; ii < KINGDOMMAX; ++ii) races[ii] = Race::BOMG;
}

bool Maps::FileInfo::ReadSAV(const std::string & filename)
{
    return Game::LoadSAV2FileInfo(filename,  *this);
}

bool Maps::FileInfo::ReadMP2(const std::string & filename)
{
    if(filename.empty()) return false;

    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail())
    {
	Error::Warning("Maps::FileInfo: " + filename +", file not found.");
	return false;
    }

    file = filename;
    kingdom_colors = 0;
    allow_colors = 0;
    rnd_colors = 0;
    rnd_races = 0;
    localtime = 0;

    u8  byte8;
    u16 byte16;
    u32 byte32;

    // magic byte
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(byte32));
    SwapLE32(byte32);

    if(byte32 != 0x0000005C)
    {
	Error::Warning("Maps::FileInfo: " + filename +", incorrect maps file.");
	fd.close();
	return false;
    }

    // level
    fd.read(reinterpret_cast<char *>(&byte16), sizeof(byte16));
    SwapLE16(byte16);

    switch(byte16)
    {
	case 0x00:
	    difficulty = Difficulty::EASY;
	    break;

	case 0x01:
	    difficulty = Difficulty::NORMAL;
	    break;

	case 0x02:
	    difficulty = Difficulty::HARD;
	    break;

	case 0x03:
	    difficulty = Difficulty::EXPERT;
	    break;

	default:
	    Error::Warning("Maps::FileInfo: incorrect difficulty maps: " + filename + ". Load EASY default.");
	break;
    }

    // width
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    size_w = byte8;

    // height
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    size_h = byte8;

    // kingdom color blue
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::BLUE;

    // kingdom color green
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::GREEN;

    // kingdom color red
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::RED;

    // kingdom color yellow
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::YELLOW;

    // kingdom color orange
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::ORANGE;

    // kingdom color purple
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) kingdom_colors |= Color::PURPLE;

    // allow color blue
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::BLUE;

    // allow color green
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::GREEN;

    // allow color red
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::RED;

    // allow color yellow
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::YELLOW;

    // allow color orange
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::ORANGE;

    // allow color purple
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) allow_colors |= Color::PURPLE;

    // rnd color blue
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::BLUE;

    // rnd color green
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::GREEN;

    // rnd color red
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::RED;

    // rnd color yellow
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::YELLOW;

    // rnd color orange
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::ORANGE;

    // rnd color purple
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    if(byte8) rnd_colors |= Color::PURPLE;

    // kingdom count
    // fd.seekg(0x1A, std::ios_base::beg);
    // fd.read(&byte8, 1);

    // wins
    fd.seekg(0x1D, std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    conditions_wins = byte8;

    // data wins
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    wins1 = byte8;
    // data wins
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    wins2 = byte8;
    // data wins
    fd.read(reinterpret_cast<char *>(&byte16), 2);
    SwapLE16(byte16);
    wins3 = byte16;
    // data wins
    fd.seekg(0x2c, std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&byte16), 2);
    SwapLE16(byte16);
    wins4 = byte16;

    // loss
    fd.seekg(0x22, std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    conditions_loss = byte8;
    // data loss
    fd.read(reinterpret_cast<char *>(&byte16), 2);
    SwapLE16(byte16);
    loss1 = byte16;
    // data loss
    fd.read(reinterpret_cast<char *>(&byte16), 2);
    SwapLE16(byte16);
    loss2 = byte16;

    // start with hero
    fd.seekg(0x25, std::ios_base::beg);
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    with_heroes = 0 == byte8;

    // race color
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[0] = ByteToRace(byte8);
    if(Race::RAND == races[0]) rnd_races |= Color::BLUE;
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[1] = ByteToRace(byte8);
    if(Race::RAND == races[1]) rnd_races |= Color::GREEN;
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[2] = ByteToRace(byte8);
    if(Race::RAND == races[2]) rnd_races |= Color::RED;
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[3] = ByteToRace(byte8);
    if(Race::RAND == races[3]) rnd_races |= Color::YELLOW;
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[4] = ByteToRace(byte8);
    if(Race::RAND == races[4]) rnd_races |= Color::ORANGE;
    fd.read(reinterpret_cast<char *>(&byte8), 1);
    races[5] = ByteToRace(byte8);
    if(Race::RAND == races[5]) rnd_races |= Color::PURPLE;

    // name
    char bufname[LENGTHNAME];
    fd.seekg(0x3A, std::ios_base::beg);
    fd.read(bufname, LENGTHNAME);
    bufname[LENGTHNAME - 1] = 0;
    name = _(bufname);

    // description
    char bufdescription[LENGTHDESCRIPTION];
    fd.seekg(0x76, std::ios_base::beg);
    fd.read(bufdescription, LENGTHDESCRIPTION);
    bufdescription[LENGTHDESCRIPTION - 1] = 0;
    description = _(bufdescription);

    fd.close();
    
    return true;
}

bool Maps::FileInfo::PredicateForSorting(const FileInfo & fi1, const FileInfo & fi2)
{
    if(fi1.name.empty() || fi2.name.empty()) return false;

    return std::tolower(fi1.name[0]) < std::tolower(fi2.name[0]);
}

u8 Maps::FileInfo::AllowColorsCount(void) const
{
    const std::bitset<8> colors(allow_colors);
    return colors.count();
}

u8 Maps::FileInfo::KingdomColorsCount(void) const
{
    const std::bitset<8> colors(kingdom_colors);
    return colors.count();
}

u8 Maps::FileInfo::KingdomRace(u8 color) const
{
    switch(color)
    {
        case Color::BLUE:       return races[0];
        case Color::GREEN:      return races[1];
	case Color::RED:        return races[2];
        case Color::YELLOW:     return races[3];
        case Color::ORANGE:     return races[4];
        case Color::PURPLE:     return races[5];
        default: break;
    }
    return 0;
}

GameOver::conditions_t Maps::FileInfo::ConditionWins(void) const
{
    switch(conditions_wins)
    {
	case 0: return GameOver::WINS_ALL;
	case 1:	return GameOver::WINS_TOWN;
	case 2:	return GameOver::WINS_HERO;
	case 3:	return GameOver::WINS_ARTIFACT;
	case 4:	return GameOver::WINS_SIDE;
	case 5:	return GameOver::WINS_GOLD;
	default: break;
    }

    return GameOver::COND_NONE;
}

GameOver::conditions_t Maps::FileInfo::ConditionLoss(void) const
{
    switch(conditions_loss)
    {
	case 0: return GameOver::LOSS_ALL;
        case 1:	return GameOver::LOSS_TOWN;
        case 2: return GameOver::LOSS_HERO;
        case 3: return GameOver::LOSS_TIME;
        default: break;
    }

    return GameOver::COND_NONE;
}

bool Maps::FileInfo::WinsCompAlsoWins(void) const
{
    return ((GameOver::WINS_TOWN | GameOver::WINS_GOLD) & ConditionWins()) && wins1;
}

bool Maps::FileInfo::WinsAllowNormalVictory(void) const
{
    return ((GameOver::WINS_TOWN | GameOver::WINS_ARTIFACT | GameOver::WINS_GOLD) & ConditionWins()) && wins2;
}

Artifact::artifact_t Maps::FileInfo::WinsFindArtifact(void) const
{
    return wins3 ? Artifact::FromInt(wins3 - 1) : Artifact::UNKNOWN;
}

bool Maps::FileInfo::WinsFindUltimateArtifact(void) const
{
    return 0 == wins3;
}

u32 Maps::FileInfo::WinsAccumulateGold(void) const
{
    return wins3 * 1000;
}

u16 Maps::FileInfo::WinsSidePart(void) const
{
    // FIX:: Maps::FileInfo::ConditionSidePart
    return wins3;
}

u32 Maps::FileInfo::WinsMapsIndexObject(void) const
{
    return wins4 * world.w() + wins3;
}

u32 Maps::FileInfo::LossMapsIndexObject(void) const
{
    return loss2 * world.w() + loss1;
}

u16 Maps::FileInfo::LossCountDays(void) const
{
    return loss1;
}

bool PrepareMapsFileInfoList(MapsFileInfoList & lists)
{
    Settings & conf = Settings::Get();
    Dir dir;

    ListMapsDirectory::const_iterator it1 = conf.GetListMapsDirectory().begin();
    ListMapsDirectory::const_iterator it2 = conf.GetListMapsDirectory().end();

    for(; it1 != it2; ++it1)
    {
        dir.Read(*it1, ".mp2", false);
        dir.Read(conf.LocalPrefix() + SEPARATOR + *it1, ".mp2", false);
        // loyality version
        if(conf.Modes(Settings::PRICELOYALTY))
        {
    	    dir.Read(*it1, ".mx2", false);
	    dir.Read(conf.LocalPrefix() + SEPARATOR + *it1, ".mx2", false);
	}
    }

    if(dir.empty()) return false;

    lists.resize(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;

    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii)
    if(lists[ii].ReadMP2(*itd))
    {
        if(conf.PreferablyCountPlayers() > lists[ii].AllowColorsCount()) --ii;
    }
    else --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);

    std::sort(lists.begin(), lists.end(), Maps::FileInfo::PredicateForSorting);

    return true;
}

