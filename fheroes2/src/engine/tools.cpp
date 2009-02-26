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

void String::Replace(std::string & dst, const std::string & pred, const std::string & src)
{
    size_t pos = std::string::npos;

    while(std::string::npos != (pos = dst.find(pred))) dst.replace(pos, pred.size(), src);
}

void String::Replace(std::string & dst, const std::string & pred, int value)
{
    if(std::string::npos != dst.find(pred))
    {
	std::ostringstream stream;
	stream << value;
	Replace(dst, pred, stream.str());
    }
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

void String::UNICODE_to_UTF8(std::string & utf8, const u16 *unicode, size_t len)
{
    utf8.reserve(2 * len);

    for(size_t ii = 0; ii < len; ++ii)
    {
	if(unicode[ii] < 128)
	{
            utf8.append(1, static_cast<char>(unicode[ii]));
	}
	else
	if(unicode[ii] < 2048)
	{
    	    utf8.append(1, static_cast<char>(192 + ((unicode[ii] - (unicode[ii] % 64)) / 64)));
            utf8.append(1, static_cast<char>(128 + (unicode[ii] % 64)));
        }
        else
        {
    	    utf8.append(1, static_cast<char>(224 + ((unicode[ii] - (unicode[ii] % 4096)) / 4096)));
            utf8.append(1, static_cast<char>(128 + (((unicode[ii] % 4096) - (unicode[ii] % 64)) / 64)));
            utf8.append(1, static_cast<char>(128 + (unicode[ii] % 64)));
        }
    }
}
