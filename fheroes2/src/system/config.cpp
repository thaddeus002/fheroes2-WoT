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

#include <iostream>
#include <fstream>
#include <cctype>
#include "error.h"
#include "cmdline.h"
#include "tools.h"
#include "config.h"

namespace H2Config
{
    static const u8 major_version = MAJOR_VERSION;
    static const u8 minor_version = MINOR_VERSION;
    static u16  boolValue = 0;
    static std::string pathAGGFile("heroes2.agg");
    static std::string pathMapsDirectory("maps");
    static Display::resolution_t videoMode = Display::SMALL;
    static Difficulty::difficulty_t gameDifficulty = Difficulty::NORMAL;
    static Color::color_t humanColor = Color::GRAY;
    static u8 kingdomColors = 0;
    static u8 allowColors = 0;
    static u8 allowRaces = 0;
    static Race::race_t raceKingdom[KINGDOMMAX];
    static Maps::mapsize_t sizeMaps(Maps::ZERO);
    static std::string pathFileMaps;
    static std::string nameMaps;
    static std::string descriptionMaps;
};

/* init Config */
void H2Config::Defaults(void)
{
    boolValue = ANIMATION | SHADOW | ORIGINAL | LOGO;
    pathAGGFile = "data/heroes2.agg";
    pathMapsDirectory = "maps";
    videoMode = Display::SMALL;
}

/* load config */
bool H2Config::Load(const std::string & filename)
{
    if(filename.empty()) return false;

    // read
    std::fstream file(filename.c_str(), std::ios::in);

    if(! file || file.fail()) return false;

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

 		// debug
		if(leftKey == "debug")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::DEBUG;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::DEBUG;
		}
		// sound
 		if(leftKey == "sound")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::SOUND;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::SOUND;
		}
		// music
 		if(leftKey == "music")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::MUSIC;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::MUSIC;
 		}
		// animation
		if(leftKey == "animation")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::ANIMATION;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::ANIMATION;
 		}
		// fullscreen
		if(leftKey == "fullscreen")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::FULLSCREEN;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::FULLSCREEN;
 		}
		// interface
		if(leftKey == "evilinterface")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::EVILINTERFACE;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::EVILINTERFACE;
		}
		// shadow
		if(leftKey == "shadow")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::SHADOW;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::SHADOW;
		}
		// origin version
		if(leftKey == "original")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::ORIGINAL;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::ORIGINAL;
		}
		// show logo
		if(leftKey == "logo")
		{
		    if(rightValue == "on") H2Config::boolValue |= H2Config::LOGO;
		    else
		    if(rightValue == "off") H2Config::boolValue &= ~H2Config::LOGO;
		}

		// agg file
 		if(leftKey == "aggfile") H2Config::pathAGGFile.assign(rightValue);
 		// directory maps
		if(leftKey == "directorymaps") H2Config::pathMapsDirectory.assign(rightValue);

		// value
 		if(leftKey == "videomode")
		{
		    if(rightValue == "640x480") H2Config::videoMode = Display::SMALL;
		    if(rightValue == "800x600") H2Config::videoMode = Display::MEDIUM;
		    if(rightValue == "1024x768") H2Config::videoMode = Display::LARGE;
		    if(rightValue == "1280x1024") H2Config::videoMode = Display::XLARGE;
		}
	    }

    file.close();
    
    return true;
}

/* save config */
bool H2Config::Save(const std::string & filename)
{
    if(filename.empty()) return false;

    //std::fstream file(filename.c_str(), std::ios::in);

    //if(! file || file.fail()) return false;
    
    return true;
}

/* return editor */
bool H2Config::Editor(void){ return H2Config::boolValue & H2Config::EDITOR; };

/* return debug */
bool H2Config::Debug(void){ return H2Config::boolValue & H2Config::DEBUG; };

/* return original version */
bool H2Config::Original(void){ return H2Config::boolValue & H2Config::ORIGINAL; };

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

/* return shadow */
bool H2Config::Shadow(void){ return H2Config::boolValue & H2Config::SHADOW; };

/* get play with heroes */
bool H2Config::PlayWithHeroes(void){ return H2Config::boolValue & H2Config::STARTHEROES; }

/* get show logo */
bool H2Config::Logo(void){ return H2Config::boolValue & H2Config::LOGO; }

/* return path agg data */
const std::string & H2Config::GetAGGFile(void){ return H2Config::pathAGGFile; };

/* return path maps directory */
const std::string & H2Config::GetMapsDirectory(void){ return H2Config::pathMapsDirectory; };

/* return video mode */
Display::resolution_t H2Config::GetVideoMode(void){ return H2Config::videoMode; };

/* return game difficulty */
Difficulty::difficulty_t H2Config::GetGameDifficulty(void){ return H2Config::gameDifficulty; };

/* return human selected colors */
Color::color_t H2Config::GetMyColor(void){ return H2Config::humanColor; };

/* return game kingdom colors */
u8 H2Config::GetKingdomColors(void){ return H2Config::kingdomColors; };

/* return game count kingdom */
u8 H2Config::GetKingdomCount(void){ return 0; };

/* get allow change color */
u8 H2Config::GetAllowChangeColors(void){ return H2Config::allowColors; };

/* return race kingdom */
Race::race_t H2Config::GetKingdomRace(Color::color_t color)
{
    switch(color){
	case Color::BLUE:
	    return raceKingdom[0];
	case Color::GREEN:
	    return raceKingdom[1];
	case Color::RED:
	    return raceKingdom[2];
	case Color::YELLOW:
	    return raceKingdom[3];
	case Color::ORANGE:
	    return raceKingdom[4];
	case Color::PURPLE:
	    return raceKingdom[5];
	default:
	    return Race::BOMG;
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

/* set editor */
void H2Config::SetEditor(void){ H2Config::boolValue |= H2Config::EDITOR; };

/* set debug */
void H2Config::SetDebug(void){ H2Config::boolValue |= H2Config::DEBUG; };

/* set original version */
void H2Config::SetOriginal(void){ H2Config::boolValue |= H2Config::ORIGINAL; };

/* set path agg data */
void H2Config::SetAGGFile(const std::string & path){ H2Config::pathAGGFile = path; };

/* set game difficulty */
void H2Config::SetGameDifficulty(Difficulty::difficulty_t difficulty){ H2Config::gameDifficulty = difficulty; };

/* set human selected colors */
void H2Config::SetHumanColor(Color::color_t color){ H2Config::humanColor = color; };

/* set game kingdom colors */
void H2Config::SetKingdomColors(u8 colors){ H2Config::kingdomColors = colors; };

/* set allow change color */
void H2Config::SetAllowChangeColors(u8 colors){ H2Config::allowColors = colors; };

/* set race kingdom */
void H2Config::SetKingdomRace(Color::color_t color, Race::race_t race)
{
    switch(color){
	case Color::BLUE:
	    raceKingdom[0] = race;
	    break;
	case Color::GREEN:
	    raceKingdom[1] = race;
	    break;
	case Color::RED:
	    raceKingdom[2] = race;
	    break;
	case Color::YELLOW:
	    raceKingdom[3] = race;
	    break;
	case Color::ORANGE:
	    raceKingdom[4] = race;
	    break;
	case Color::PURPLE:
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

/* set interface */
void H2Config::SetInterface(bool interface){ interface ? H2Config::boolValue &= ~H2Config::EVILINTERFACE : H2Config::boolValue |= H2Config::EVILINTERFACE; }

/* set play with heroes */
void H2Config::SetPlayWithHeroes(bool fl){ fl ? H2Config::boolValue &= ~H2Config::STARTHEROES : H2Config::boolValue |= H2Config::STARTHEROES; }

/* print default values */
void H2Config::PrintCurrentValues(void)
{
    std::cout << std::endl;
    std::cout << "# dump config, current values:" << std::endl;
    std::cout << "aggfile = " << pathAGGFile << std::endl;
    std::cout << "directorymaps = " << pathMapsDirectory << std::endl;
    std::cout << "videomode = ";
    
    switch(videoMode)
    {
	case Display::SMALL:	std::cout  << "640x480" << std::endl; break;
	case Display::MEDIUM:	std::cout  << "800x600" << std::endl; break;
	case Display::LARGE:	std::cout  << "1024x768" << std::endl; break;
	case Display::XLARGE:	std::cout  << "1280x1024" << std::endl; break;
    }

    std::cout << "sound = " << (boolValue & H2Config::SOUND ? "on"  : "off") << std::endl;
    std::cout << "music = " << (boolValue & H2Config::MUSIC ? "on"  : "off") << std::endl;
    std::cout << "fullscreen = " << (boolValue & H2Config::FULLSCREEN ? "on"  : "off") << std::endl;
    std::cout << "evilinterface = " << (boolValue & H2Config::EVILINTERFACE ? "on"  : "off") << std::endl;
    std::cout << "shadow = " << (boolValue & H2Config::SHADOW ? "on"  : "off") << std::endl;
    std::cout << "original = " << (boolValue & H2Config::ORIGINAL ? "on"  : "off") << std::endl;
    std::cout << "debug = " << (boolValue & H2Config::DEBUG ? "on"  : "off") << std::endl;
    std::cout << std::endl;
}
