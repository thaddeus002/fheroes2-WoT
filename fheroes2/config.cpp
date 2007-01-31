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

#include "config.h"

namespace H2Config {
    static u16  boolValue = ANIMATION;
    static std::string pathAGGFile("heroes2.agg");
    static std::string pathMapsDirectory("maps");
    static Display::resolution_t videoMode = Display::SMALL;
    static Maps::difficulty_t gameDifficulty = Maps::NORMAL;
    static Kingdom::color_t humanColor = Kingdom::GRAY;
    static u8 kingdomColors = 0;
    static u8 kingdomCount = 0;
    static u8 allowColors = 0;
    static u8 allowRaces = 0;
    static Kingdom::race_t raceKingdom[KINGDOMMAX];
    static Maps::mapsize_t sizeMaps(Maps::NONE);
    static std::string pathFileMaps;
    static std::string nameMaps;
    static std::string descriptionMaps;
};

/* init Config */
void H2Config::Init(const std::string &filename)
{
	if(filename.empty()) return;

	//
	boolValue = 0;

	Error::Verbose("config file: " + filename);
	
	// read
	std::fstream file(filename.c_str(), std::ios::in);
	if(! file || file.fail()){ Error::Warning("file not found. Load default options."); return; }

	std::string oneString, leftKey, rightValue;

	while(std::getline(file, oneString))
	    // skip comment
	    if(!String::Comment(oneString) && oneString.size())
	    {
		size_t pos = oneString.find('=');
		leftKey = oneString.substr(0, pos - 1);
		rightValue = oneString.substr(pos + 1, oneString.length() - pos - 1);

		String::Trim(leftKey);
		String::Trim(rightValue);

		// convert to lower case string
		String::Lower(leftKey);

 		// bool
		if(leftKey == "debug" && rightValue == "on") H2Config::boolValue |= H2Config::DEBUG;
 		if(leftKey == "sound" && rightValue == "on") H2Config::boolValue |= H2Config::SOUND;
 		if(leftKey == "music" && rightValue == "on") H2Config::boolValue |= H2Config::MUSIC;
 		if(leftKey == "animation" && rightValue == "on") H2Config::boolValue |= H2Config::ANIMATION;
 		if(leftKey == "fullscreen" && rightValue == "on") H2Config::boolValue |= H2Config::FULLSCREEN;
 		if(leftKey == "evilinterface" && rightValue == "on") H2Config::boolValue |= H2Config::EVILINTERFACE;
		if(leftKey == "rledebug" && rightValue == "on") H2Config::boolValue |= H2Config::RLEDEBUG;

		// string
 		if(leftKey == "aggfile") H2Config::pathAGGFile.assign(rightValue);
 		if(leftKey == "directorymaps") H2Config::pathMapsDirectory.assign(rightValue);

		// value
 		if(leftKey == "videomode"){
		    if(rightValue == "640x480") H2Config::videoMode = Display::SMALL;
		    if(rightValue == "800x600") H2Config::videoMode = Display::MEDIUM;
		    if(rightValue == "1024x768") H2Config::videoMode = Display::LARGE;
		    if(rightValue == "1280x1024") H2Config::videoMode = Display::XLARGE;
		}
	    }

	file.close();
}

/* return debug */
bool H2Config::Debug(void){ return H2Config::boolValue & H2Config::DEBUG; };

/* return RLE debug */
bool H2Config::RLEDebug(void){ return H2Config::boolValue & H2Config::RLEDEBUG; };

/* return sound */
bool H2Config::Sound(void){ return H2Config::boolValue & H2Config::SOUND; };

/* return music */
bool H2Config::Music(void){ return H2Config::boolValue & H2Config::MUSIC; };

/* return animation */
bool H2Config::Animation(void){ return H2Config::boolValue & H2Config::ANIMATION; };

/* return full screen */
bool H2Config::FullScreen(void){ return H2Config::boolValue & H2Config::FULLSCREEN; };

/* return evil interface */
bool H2Config::EvilInterface(void){ return H2Config::boolValue & H2Config::EVILINTERFACE; };

/* return path agg data */
const std::string & H2Config::GetAGGFile(void){ return H2Config::pathAGGFile; };

/* return path maps directory */
const std::string & H2Config::GetMapsDirectory(void){ return H2Config::pathMapsDirectory; };

/* return video mode */
Display::resolution_t H2Config::GetVideoMode(void){ return H2Config::videoMode; };

/* return game difficulty */
Maps::difficulty_t H2Config::GetGameDifficulty(void){ return H2Config::gameDifficulty; };

/* return human selected colors */
Kingdom::color_t H2Config::GetHumanColor(void){ return H2Config::humanColor; };

/* return game kingdom colors */
u8 H2Config::GetKingdomColors(void){ return H2Config::kingdomColors; };

/* return game count kingdom */
u8 H2Config::GetKingdomCount(void){ return H2Config::kingdomCount; };

/* get allow change color */
u8 H2Config::GetAllowChangeColors(void){ return H2Config::allowColors; };

/* return race kingdom */
Kingdom::race_t H2Config::GetKingdomRace(Kingdom::color_t color)
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
	    return Kingdom::BOMG;
	break;
    }
}

/* get allow change races */
u8 H2Config::GetAllowChangeRaces(void){ return H2Config::allowRaces; };

/* get play file maps */
const std::string & H2Config::GetFileMaps(void){ return H2Config::pathFileMaps; }

/* get play name maps */
const std::string & H2Config::GetNameMaps(void){ return H2Config::nameMaps; }

/* get play description maps */
const std::string & H2Config::GetDescriptionMaps(void){ return H2Config::descriptionMaps; }

/* get size play maps */
Maps::mapsize_t H2Config::GetSizeMaps(void){ return H2Config::sizeMaps; }

/* set debug */
void H2Config::SetDebug(void){ H2Config::boolValue |= H2Config::DEBUG; };

/* set path agg data */
void H2Config::SetAGGFile(const std::string & path){ H2Config::pathAGGFile = path; };

/* set game difficulty */
void H2Config::SetGameDifficulty(Maps::difficulty_t difficulty){ H2Config::gameDifficulty = difficulty; };

/* set human selected colors */
void H2Config::SetHumanColor(Kingdom::color_t color){ H2Config::humanColor = color; };

/* set game kingdom colors */
void H2Config::SetKingdomColors(u8 colors){ H2Config::kingdomColors = colors; };

/* set game count kingdom */
void H2Config::SetKingdomCount(u8 count){ H2Config::kingdomCount = count; };

/* set allow change color */
void H2Config::SetAllowChangeColors(u8 colors){ H2Config::allowColors = colors; };

/* set race kingdom */
void H2Config::SetKingdomRace(Kingdom::color_t color, Kingdom::race_t race)
{
    switch(color){
	case Kingdom::BLUE:
	    raceKingdom[0] = race;
	    break;
	case Kingdom::GREEN:
	    raceKingdom[1] = race;
	    break;
	case Kingdom::RED:
	    raceKingdom[2] = race;
	    break;
	case Kingdom::YELLOW:
	    raceKingdom[3] = race;
	    break;
	case Kingdom::ORANGE:
	    raceKingdom[4] = race;
	    break;
	case Kingdom::PURPLE:
	    raceKingdom[5] = race;
	    break;
	default:
	    return;
	break;
    }
}

/* set allow change race */
void H2Config::SetAllowChangeRaces(u8 races){ H2Config::allowRaces = races; };

/* set play file maps */
void H2Config::SetFileMaps(const std::string & file){ H2Config::pathFileMaps = file; }

/* set play name maps */
void H2Config::SetNameMaps(const std::string & name){ H2Config::nameMaps = name; }

/* set play description maps */
void H2Config::SetDescriptionMaps(const std::string & description){ H2Config::descriptionMaps = description; }

/* set size play maps */
void H2Config::SetSizeMaps(Maps::mapsize_t size){ H2Config::sizeMaps = size; }
