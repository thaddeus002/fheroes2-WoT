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

#include "tools.h"

/* trim left right space */
void String::Trim(std::string & str)
{
    std::string::iterator iter;

    // left
    iter = str.begin();

    while(iter != str.end() && std::isspace(*iter)) ++iter;
    
    if(iter != str.begin()) str.replace(str.begin(), iter, "");

    // right
    iter = str.end();

    while(iter != str.begin() && std::isspace(*iter)) --iter;

    if(iter != str.end()) str.replace(iter, str.end(), "");
}

/*true is comment */
bool String::Comment(std::string & str)
{
    String::Trim(str);

    switch(str[0]){
    
	case ';':
	case '#':
	    return true;
	
	default:
	    return false;
    }
}

/* convert to lower case */
void String::Lower(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/* convert to upper case */
void String::Upper(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/* int to string */
void String::AddInt(std::string &str, int value)
{
    std::ostringstream stream;
    stream << value;

    str += stream.str();
}

const char * CStr::Difficulty(Maps::difficulty_t difficulty)
{
    switch(difficulty){
        case Maps::EASY:
	    return "Easy";
        case Maps::NORMAL:
	    return "Normal";
        case Maps::HARD:
	    return "Hard";
        case Maps::EXPERT:
	    return "Expert";
        case Maps::IMPOSSIBLE:
	    return "Impossible";
    }
    
    return "";
}

const char * CStr::Color(Kingdom::color_t color)
{
    switch(color){
        case Kingdom::BLUE:
	    return "Blue";
        case Kingdom::GREEN:
	    return "Green";
        case Kingdom::RED:
	    return "Red";
        case Kingdom::YELLOW:
	    return "Yellow";
        case Kingdom::ORANGE:
	    return "Orange";
        case Kingdom::PURPLE:
	    return "Purple";
        default:
	    break;
    }

    return "Gray";
}

const char * CStr::Race(Kingdom::race_t race)
{
    switch(race){
        case Kingdom::KNGT:
	    return "Knight";
        case Kingdom::BARB:
	    return "Barbarian";
        case Kingdom::SORC:
	    return "Sorceress";
        case Kingdom::WRLK:
	    return "Warlock";
        case Kingdom::WZRD:
	    return "Wizard";
        case Kingdom::NECR:
	    return "Necromancer";
        case Kingdom::MULT:
	    return "Multi";
        case Kingdom::RAND:
	    return "Randomize";
	default:
	    break;
    }
    
    return "Bomg";
}

const char * CStr::Object(MP2::object_t object)
{
    return "ASDASDAS";
}
