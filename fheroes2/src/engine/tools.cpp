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
#include <cstring>
#include <climits>
#include "error.h"
#include "types.h"
#include "tools.h"

/* trim left right space */
void String::Trim(std::string & str)
{
    std::string::iterator iter;

    // left
    iter = str.begin();
    while(iter != str.end() && std::isspace(*iter)) ++iter;
    if(iter != str.begin()) str.erase(str.begin(), iter);

    // right
    iter = str.end() - 1;
    while(iter != str.begin() && std::isspace(*iter)) --iter;
    if(iter != str.end() - 1) str.erase(iter + 1, str.end());
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

void String::AppendKey(std::string & res, KeySym sym, u16 mod)
{
    switch(sym)
    {
        case KEY_EXCLAIM:       res += '!'; break;
        case KEY_QUOTEDBL:      res += '"'; break;
        case KEY_HASH:          res += '#'; break;
        case KEY_DOLLAR:        res += '$'; break;
        case KEY_AMPERSAND:     res += '&'; break;
        case KEY_QUOTE:         res += '\''; break;
        case KEY_LEFTPAREN:     res += '('; break;
        case KEY_RIGHTPAREN:    res += ')'; break;
        case KEY_ASTERISK:      res += '*'; break;
        case KEY_PLUS:          res += '+'; break;
        case KEY_COMMA:         res += ','; break;
        case KEY_MINUS:         res += '-'; break;
        case KEY_PERIOD:        res += '.'; break;
        case KEY_SLASH:         res += '/'; break;
        case KEY_COLON:         res += ':'; break;
        case KEY_SEMICOLON:     res += ';'; break;
	case KEY_LESS:          res += '<'; break;
        case KEY_EQUALS:        res += '='; break;
	case KEY_GREATER:       res += '>'; break;
	case KEY_QUESTION:      res += '?'; break;
	case KEY_AT:            res += '@'; break;
	case KEY_LEFTBRACKET:   res += '['; break;
	case KEY_BACKSLASH:     res += '\\'; break;
	case KEY_RIGHTBRACKET:  res += ']'; break;
	case KEY_CARET:         res += '^'; break;
	case KEY_UNDERSCORE:    res += '_'; break;
        case KEY_SPACE:		res += ' '; break;

        case KEY_a:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'A' : 'a'); break;
        case KEY_b:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'B' : 'b'); break;
        case KEY_c:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'C' : 'c'); break;
        case KEY_d:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'D' : 'd'); break;
        case KEY_e:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'E' : 'e'); break;
        case KEY_f:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'F' : 'f'); break;
        case KEY_g:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'G' : 'g'); break;
        case KEY_h:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'H' : 'h'); break;
        case KEY_i:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'I' : 'i'); break;
        case KEY_j:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'J' : 'j'); break;
        case KEY_k:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'K' : 'k'); break;
        case KEY_l:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'L' : 'l'); break;
        case KEY_m:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'M' : 'm'); break;
        case KEY_n:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'N' : 'n'); break;
        case KEY_o:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'O' : 'o'); break;
        case KEY_p:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'P' : 'p'); break;
        case KEY_q:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'Q' : 'q'); break;
        case KEY_r:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'R' : 'r'); break;
        case KEY_s:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'S' : 's'); break;
        case KEY_t:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'T' : 't'); break;
        case KEY_u:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'U' : 'u'); break;
        case KEY_v:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'V' : 'v'); break;
        case KEY_w:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'W' : 'w'); break;
        case KEY_x:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'X' : 'x'); break;
        case KEY_y:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'Y' : 'y'); break;
        case KEY_z:     res += ((MOD_SHIFT | MOD_CAPS) & mod ? 'Z' : 'z'); break;

        case KEY_0:     res += '0'; break;
        case KEY_1:     res += '1'; break;
        case KEY_2:     res += '2'; break;
        case KEY_3:     res += '3'; break;
        case KEY_4:     res += '4'; break;
        case KEY_5:     res += '5'; break;
        case KEY_6:     res += '6'; break;
        case KEY_7:     res += '7'; break;
        case KEY_8:     res += '8'; break;
        case KEY_9:     res += '9'; break;

        case KEY_BACKSPACE: if(res.size()) res.resize(res.size() - 1); break;
        default: break;
    }
}

#ifdef __WIN32__ /* SDL_platform.h */
const char *GetDirname(const char *path)
{
    static char buff[PATH_MAX];
    strncpy(buff, path, PATH_MAX);
    char *c = strrchr(buff, SEPARATOR);
    if(!c) strcpy(buff, ".");
    else *c = 0;
    return buff;
}

const char *GetBasename(const char *path)
{
    static char buff[FILENAME_MAX];
    const char *c = strrchr(path, SEPARATOR);
    if(!c) strncpy(buff, path, FILENAME_MAX);
    else strcpy(buff, c + 1);
    return buff;
}
#else
#include <libgen.h>
const char *GetDirname(const char *path)
{
    return dirname(const_cast<char *>(path));
}

const char *GetBasename(const char *path)
{
    return basename(const_cast<char *>(path));
}
#endif
