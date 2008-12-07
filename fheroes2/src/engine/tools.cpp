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

#include <algorithm>
#include <sstream>
#include "error.h"
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

int String::ToInt(const std::string &str)
{
    std::istringstream stream(str);
    int res;

    stream >> res;

    return res;
}

/* string compare */
bool String::Compare(const std::string &str1, const std::string &str2, bool sensitive)
{
    if(str1.size() != str2.size()) return false;
    if(sensitive) return str1 == str2;

    std::string strl1(str1);
    std::string strl2(str2);
    Lower(strl1);
    Lower(strl2);

    return str1 == str2;
}

// from SDL_ttf
void String::UTF8_to_UNICODE(u16 *unicode, const char *utf8, int len)
{
    int i, j;
    u16 ch;

    for ( i=0, j=0; i < len; ++i, ++j )
    {
	ch = ((const unsigned char *)utf8)[i];
        if ( ch >= 0xF0 )
	{
            ch  =  (u16)(utf8[i]&0x07) << 18;
            ch |=  (u16)(utf8[++i]&0x3F) << 12;
            ch |=  (u16)(utf8[++i]&0x3F) << 6;
            ch |=  (u16)(utf8[++i]&0x3F);
        }
	else
        if ( ch >= 0xE0 )
	{
            ch  =  (u16)(utf8[i]&0x0F) << 12;
            ch |=  (u16)(utf8[++i]&0x3F) << 6;
            ch |=  (u16)(utf8[++i]&0x3F);
        }
	else
        if ( ch >= 0xC0 )
	{
            ch  =  (u16)(utf8[i]&0x1F) << 6;
            ch |=  (u16)(utf8[++i]&0x3F);
        }
        unicode[j] = ch;
    }
    unicode[j] = 0;
}
