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

const std::string & String::Difficulty(Maps::difficulty_t difficulty)
{
    static std::string easy("Easy");
    static std::string normal("Normal");
    static std::string hard("Hard");
    static std::string expert("Expert");
    static std::string impossible("Impossible");

    switch(difficulty){
        default:
	    break;
        case Maps::NORMAL:
	    return normal;
        case Maps::HARD:
	    return hard;
        case Maps::EXPERT:
	    return expert;
        case Maps::IMPOSSIBLE:
	    return impossible;
    }
    
    return easy;
}

const std::string & String::Color(Kingdom::color_t color)
{
    static std::string blue("Blue");
    static std::string green("Green");
    static std::string red("Red");
    static std::string yellow("Yellow");
    static std::string orange("Orange");
    static std::string purple("Purple");
    static std::string gray("Gray");

    switch(color){
        case Kingdom::BLUE:
	    return blue;
        case Kingdom::GREEN:
	    return green;
        case Kingdom::RED:
	    return red;
        case Kingdom::YELLOW:
	    return yellow;
        case Kingdom::ORANGE:
	    return orange;
        case Kingdom::PURPLE:
	    return purple;
        default:
	    break;
    }

    return gray;
}

const std::string & String::Race(Kingdom::race_t race)
{
    static std::string kngt("Knight");
    static std::string barb("Barbarian");
    static std::string sorc("Sorceress");
    static std::string wrlk("Warlock");
    static std::string wzrd("Wizard");
    static std::string necr("Necromancer");
    static std::string mult("Multi");
    static std::string rand("Randomize");
    static std::string bomg("Bomg");
    
    switch(race){
        case Kingdom::KNGT:
	    return kngt;
        case Kingdom::BARB:
	    return barb;
        case Kingdom::SORC:
	    return sorc;
        case Kingdom::WRLK:
	    return wrlk;
        case Kingdom::WZRD:
	    return wzrd;
        case Kingdom::NECR:
	    return necr;
        case Kingdom::MULT:
	    return mult;
        case Kingdom::RAND:
	    return rand;
	default:
	    break;
    }
    
    return bomg;
}
