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


#include <fstream>
#include "kingdom.h"
#include "maps_fileinfo.h"

#define LENGTHNAME		16
#define LENGTHDESCRIPTION	143

Maps::FileInfo::FileInfo() : size(Maps::ZERO), difficulty(Difficulty::EASY),
    kingdom_colors(0), allow_colors(0), rnd_colors(0), with_heroes(false), races(KINGDOMMAX)
{
}

Maps::FileInfo::FileInfo(const std::string &filename) : size(Maps::ZERO), difficulty(Difficulty::EASY),
    kingdom_colors(0), allow_colors(0), rnd_colors(0), with_heroes(false), races(KINGDOMMAX)
{
    Read(filename);
}

const std::string & Maps::FileInfo::FileMaps(void) const
{
    return file;
}

const std::string & Maps::FileInfo::Name(void) const
{
    return name;
}

const std::string & Maps::FileInfo::Description(void) const
{
    return description;
}

Maps::mapsize_t Maps::FileInfo::SizeMaps(void) const
{
    return size;
}

Difficulty::difficulty_t Maps::FileInfo::Difficulty(void) const
{
    return difficulty;
}

u8 Maps::FileInfo::KingdomColors(void) const
{
    return kingdom_colors;
}

u8 Maps::FileInfo::AllowColors(void) const
{
    return allow_colors;
}

u8 Maps::FileInfo::ConditionsWins(void) const
{
    return conditions_wins;
}

u8 Maps::FileInfo::ConditionsLoss(void) const
{
    return conditions_loss;
}

Race::race_t Maps::FileInfo::KingdomRace(const Color::color_t color) const
{
    switch(color)
    {
        case Color::BLUE:	return races[0];
        case Color::GREEN:	return races[1];
        case Color::RED:	return races[2];
        case Color::YELLOW:	return races[3];
        case Color::ORANGE:	return races[4];
        case Color::PURPLE:	return races[5];

        default: break;
    }

    return Race::BOMG;
}

void Maps::FileInfo::SetKingdomRace(const Color::color_t color, const Race::race_t race)
{
    switch(color)
    {
        case Color::BLUE:	races[0] = race; break;
        case Color::GREEN:	races[1] = race; break;
        case Color::RED:	races[2] = race; break;
        case Color::YELLOW:	races[3] = race; break;
        case Color::ORANGE:	races[4] = race; break;
        case Color::PURPLE:	races[5] = race; break;

        default: break;
    }
}

void Maps::FileInfo::SetKingdomColors(const u8 colors)
{
    kingdom_colors = colors;
}

bool Maps::FileInfo::PlayWithHeroes(void) const
{
    return with_heroes;
}

bool Maps::FileInfo::Read(const std::string &filename)
{
    size = Maps::ZERO;
    difficulty = Difficulty::EASY;
    kingdom_colors = 0;
    allow_colors = 0;
    rnd_colors = 0;
    with_heroes = false;
    races[0] = Race::BOMG;
    races[1] = Race::BOMG;
    races[2] = Race::BOMG;
    races[3] = Race::BOMG;
    races[4] = Race::BOMG;
    races[5] = Race::BOMG;

    file = filename;

    std::fstream fd(filename.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail())
    {
	Error::Warning("Maps::FileInfo: " + filename +", file not found.");

	return false;
    }

    char byte8;
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
    fd.read(&byte8, 1);

    switch(byte8)
    {
	case Maps::SMALL:
	    size = Maps::SMALL;
	    break;

	case Maps::MEDIUM:
	    size = Maps::MEDIUM;
	    break;

	case Maps::LARGE:
	    size = Maps::LARGE;
	    break;

	case Maps::XLARGE:
	    size = Maps::XLARGE;
	    break;
	
	default:
	    Error::Warning("Maps::FileInfo: width unknown");
	    break;
    }

    // height
    fd.read(&byte8, 1);

    switch(byte8)
    {
	case Maps::SMALL:
	    if(size != Maps::SMALL) Error::Warning("Maps::FileInfo: incorrect height");
	    break;

	case Maps::MEDIUM:
	    if(size != Maps::MEDIUM) Error::Warning("Maps::FileInfo: incorrect height");
	    break;

	case Maps::LARGE:
	    if(size != Maps::LARGE) Error::Warning("Maps::FileInfo: incorrect height");
	    break;

	case Maps::XLARGE:
	    if(size != Maps::XLARGE) Error::Warning("Maps::FileInfo: incorrect height");
	    break;
	
	default:
	    Error::Warning("Maps::FileInfo: height unknown");
	    break;
    }

    // kingdom color blue
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::BLUE;

    // kingdom color green
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::GREEN;

    // kingdom color red
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::RED;

    // kingdom color yellow
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::YELLOW;

    // kingdom color orange
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::ORANGE;

    // kingdom color purple
    fd.read(&byte8, 1);
    if(byte8) kingdom_colors |= Color::PURPLE;

    // allow color blue
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::BLUE;

    // allow color green
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::GREEN;

    // allow color red
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::RED;

    // allow color yellow
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::YELLOW;

    // allow color orange
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::ORANGE;

    // allow color purple
    fd.read(&byte8, 1);
    if(byte8) allow_colors |= Color::PURPLE;

    // rnd color blue
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::BLUE;

    // rnd color green
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::GREEN;

    // rnd color red
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::RED;

    // rnd color yellow
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::YELLOW;

    // rnd color orange
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::ORANGE;

    // rnd color purple
    fd.read(&byte8, 1);
    if(byte8) rnd_colors |= Color::PURPLE;

    // kingdom count
    // fd.seekg(0x1A, std::ios_base::beg);
    // fd.read(&byte8, 1);

    // wins
    fd.seekg(0x1D, std::ios_base::beg);
    fd.read(&byte8, 1);
    conditions_wins = byte8;

    // loss
    fd.seekg(0x22, std::ios_base::beg);
    fd.read(&byte8, 1);
    conditions_loss = byte8;

    // start with hero
    fd.seekg(0x25, std::ios_base::beg);
    fd.read(&byte8, 1);
    if(0 == byte8) with_heroes = true;

    // race color
    fd.read(&byte8, 1);
    races[0] = ByteToRace(byte8);
    fd.read(&byte8, 1);
    races[1] = ByteToRace(byte8);
    fd.read(&byte8, 1);
    races[2] = ByteToRace(byte8);
    fd.read(&byte8, 1);
    races[3] = ByteToRace(byte8);
    fd.read(&byte8, 1);
    races[4] = ByteToRace(byte8);
    fd.read(&byte8, 1);
    races[5] = ByteToRace(byte8);

    // name
    char bufname[LENGTHNAME];
    fd.seekg(0x3A, std::ios_base::beg);
    fd.read(bufname, LENGTHNAME);
    bufname[LENGTHNAME - 1] = 0;
    name = bufname;

    // description
    char bufdescription[LENGTHDESCRIPTION];
    fd.seekg(0x76, std::ios_base::beg);
    fd.read(bufdescription, LENGTHDESCRIPTION);
    bufdescription[LENGTHDESCRIPTION - 1] = 0;
    description = bufdescription;

    fd.close();
    
    return true;
}

Race::race_t Maps::FileInfo::ByteToRace(u8 byte)
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

bool Maps::FileInfo::PredicateForSorting(const FileInfo *fi1, const FileInfo *fi2)
{
    if(!fi1 || !fi2 || fi1->name.empty() || fi2->name.empty()) return false;

    return std::tolower(fi1->name[0]) < std::tolower(fi2->name[0]);
}
