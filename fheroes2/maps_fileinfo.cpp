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
#include "error.h"
#include "maps_fileinfo.h"

#define LENGTHNAME		16
#define LENGTHDESCRIPTION	143

Maps::FileInfo::FileInfo(const std::string &filemaps) : fileMaps(filemaps), sizeMaps(Maps::NONE), mapsDifficulty(Maps::EASY),
    kingdomColors(0), allowColors(0), rndColors(0), withHeroes(false)
{

    std::fstream fd(filemaps.c_str(), std::ios::in | std::ios::binary);

    if(! fd || fd.fail()){
	Error::Warning("MapsHeaderInfo: " + filemaps +", file not found.");
	return;
    }
    
    char byte8;
    u16 byte16;
    u32 byte32;

    // magic byte
    fd.read(reinterpret_cast<char *>(&byte32), sizeof(byte32));

    if(byte32 != 0x0000005C){
	Error::Warning("MapsHeaderInfo: " + filemaps +", incorrect maps file.");
	fd.close();
	return;

    }

    // level
    fd.read(reinterpret_cast<char *>(&byte16), sizeof(byte16));

    switch(byte16){
    
	case 0x00:
	    mapsDifficulty = Maps::EASY;
	    break;

	case 0x01:
	    mapsDifficulty = Maps::NORMAL;
	    break;

	case 0x02:
	    mapsDifficulty = Maps::HARD;
	    break;
	
	case 0x03:
	    mapsDifficulty = Maps::EXPERT;
	    break;
	
	default:
	    Error::Warning("MapsHeaderInfo: incorrect Difficulty maps: " + filemaps + ". Load EASY default.");
	break;
    }

    // width
    fd.read(&byte8, 1);

    switch(byte8){

	case Maps::SMALL:
	    sizeMaps = Maps::SMALL;
	    break;

	case Maps::MEDIUM:
	    sizeMaps = Maps::MEDIUM;
	    break;

	case Maps::LARGE:
	    sizeMaps = Maps::LARGE;
	    break;

	case Maps::XLARGE:
	    sizeMaps = Maps::XLARGE;
	    break;
	
	default:
	    Error::Warning("MapsHeaderInfo: width unknown");
	    break;
    }

    // height
    fd.read(&byte8, 1);

    switch(byte8){

	case Maps::SMALL:
	    if(sizeMaps != Maps::SMALL) Error::Warning("MapsHeaderInfo: incorrect height");
	    break;

	case Maps::MEDIUM:
	    if(sizeMaps != Maps::MEDIUM) Error::Warning("MapsHeaderInfo: incorrect height");
	    break;

	case Maps::LARGE:
	    if(sizeMaps != Maps::LARGE) Error::Warning("MapsHeaderInfo: incorrect height");
	    break;

	case Maps::XLARGE:
	    if(sizeMaps != Maps::XLARGE) Error::Warning("MapsHeaderInfo: incorrect height");
	    break;
	
	default:
	    Error::Warning("MapsHeaderInfo: height unknown");
	    break;
    }

    // kingdom color blue
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::BLUE;

    // kingdom color green
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::GREEN;

    // kingdom color red
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::RED;

    // kingdom color yellow
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::YELLOW;

    // kingdom color orange
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::ORANGE;

    // kingdom color purple
    fd.read(&byte8, 1);
    if(byte8) kingdomColors |= Kingdom::PURPLE;

    // allow color blue
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::BLUE;

    // allow color green
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::GREEN;

    // allow color red
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::RED;

    // allow color yellow
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::YELLOW;

    // allow color orange
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::ORANGE;

    // allow color purple
    fd.read(&byte8, 1);
    if(byte8) allowColors |= Kingdom::PURPLE;

    // rnd color blue
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::BLUE;

    // rnd color green
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::GREEN;

    // rnd color red
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::RED;

    // rnd color yellow
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::YELLOW;

    // rnd color orange
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::ORANGE;

    // rnd color purple
    fd.read(&byte8, 1);
    if(byte8) rndColors |= Kingdom::PURPLE;

    // count heroes
    fd.seekg(0x1A, std::ios_base::beg);
    fd.read(&byte8, 1);
    kingdomCount = byte8;

    // wins
    fd.seekg(0x1D, std::ios_base::beg);
    fd.read(&byte8, 1);
    conditionsWins = byte8;

    // loss
    fd.seekg(0x22, std::ios_base::beg);
    fd.read(&byte8, 1);
    conditionsLoss = byte8;

    // start with hero
    fd.seekg(0x25, std::ios_base::beg);
    fd.read(&byte8, 1);
    if(byte8) withHeroes = true;

    // race color
    // 00 - kngt, 01 - barb, 02 - sorc, 03 - wrlk, 04 - wzrd, 05 - necr, 06 - mult, 07 - rnd, ff - none
    fd.read(&byte8, 1);
    raceKingdom[0] = byte8;
    fd.read(&byte8, 1);
    raceKingdom[1] = byte8;
    fd.read(&byte8, 1);
    raceKingdom[2] = byte8;
    fd.read(&byte8, 1);
    raceKingdom[3] = byte8;
    fd.read(&byte8, 1);
    raceKingdom[4] = byte8;
    fd.read(&byte8, 1);
    raceKingdom[5] = byte8;

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
}

u8 Maps::FileInfo::GetKingdomRace(Kingdom::color_t color) const
{
    switch(color){
        case Kingdom::BLUE:
            return raceKingdom[0];
        case Kingdom::GREEN:
            return raceKingdom[1];
        case Kingdom::RED:
            return raceKingdom[2];
        case Kingdom::YELLOW:
            return raceKingdom[3];
        case Kingdom::ORANGE:
            return raceKingdom[4];
        case Kingdom::PURPLE:
            return raceKingdom[5];
        default:
            return 0xFF;
        break;
    }
}
