/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <locale>
#include <climits>
#include <cctype>
#include "error.h"
#include "types.h"
#include "tools.h"

/* trim left right space */
std::string String::Trim(std::string str)
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

    return str;
}

/* convert to lower case */
std::string String::Lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

/* convert to upper case */
std::string String::Upper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

/* int to string */
void String::AddInt(std::string &str, int value)
{
    std::ostringstream stream;
    stream << value;

    str += stream.str();
}

int String::ToInt(const std::string & str)
{
    int res = 0;

    // decimal
    if(str.end() == std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isdigit))))
    {
        std::istringstream ss(str);
        ss >> res;
    }
    else
    // hex
    if(str.size() > 3 && str.at(0) == '0' && std::tolower(str.at(1)) == 'x' &&
        str.end() == std::find_if(str.begin() + 2, str.end(), std::not1(std::ptr_fun<int, int>(std::isxdigit))))
    {
        std::istringstream ss(str);
        ss >> std::hex >> res;
    }
    else
    // str
    {
        std::string lower = String::Lower(str);

        if(lower == "on")       return 1;
        else
        if(lower == "one")      return 1;
        else
        if(lower == "two")      return 2;
        else
        if(lower == "three")    return 3;
        else
        if(lower == "four")     return 4;
        else
        if(lower == "five")     return 5;
        else
        if(lower == "six")      return 6;
        else
        if(lower == "seven")    return 7;
        else
        if(lower == "eight")    return 8;
        else
        if(lower == "nine")     return 9;
    }

    return res;
}

void String::Replace(std::string & dst, const char* pred, const char* src)
{
    size_t pos = std::string::npos;

    while(std::string::npos != (pos = dst.find(pred))) dst.replace(pos, std::strlen(pred), src);
}

void String::Replace(std::string & dst, const char* pred, const std::string & src)
{
    size_t pos = std::string::npos;

    while(std::string::npos != (pos = dst.find(pred))) dst.replace(pos, std::strlen(pred), src);
}

void String::Replace(std::string & dst, const char* pred, int value)
{
    if(std::string::npos != dst.find(pred))
    {
	std::ostringstream stream;
	stream << value;
	Replace(dst, pred, stream.str());
    }
}

std::string String::Double(double value, u8 prec)
{
    std::ostringstream stream;
    stream << std::setprecision(prec) << value;
    return stream.str();
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
        case KEY_1:     res += (MOD_SHIFT & mod ? '!' : '1'); break;
        case KEY_2:     res += (MOD_SHIFT & mod ? '@' : '2'); break;
        case KEY_3:     res += (MOD_SHIFT & mod ? '#' : '3'); break;
        case KEY_4:     res += (MOD_SHIFT & mod ? '$' : '4'); break;
        case KEY_5:     res += (MOD_SHIFT & mod ? '%' : '5'); break;
        case KEY_6:     res += (MOD_SHIFT & mod ? '^' : '6'); break;
        case KEY_7:     res += (MOD_SHIFT & mod ? '&' : '7'); break;
        case KEY_8:     res += (MOD_SHIFT & mod ? '*' : '8'); break;
        case KEY_9:     res += (MOD_SHIFT & mod ? '(' : '9'); break;
        case KEY_0:     res += (MOD_SHIFT & mod ? ')' : '0'); break;

	case KEY_KP0:	if(MOD_NUM & mod) res += '0'; break;
	case KEY_KP1:	if(MOD_NUM & mod) res += '1'; break;
	case KEY_KP2:	if(MOD_NUM & mod) res += '2'; break;
	case KEY_KP3:	if(MOD_NUM & mod) res += '3'; break;
	case KEY_KP4:	if(MOD_NUM & mod) res += '4'; break;
	case KEY_KP5:	if(MOD_NUM & mod) res += '5'; break;
	case KEY_KP6:	if(MOD_NUM & mod) res += '6'; break;
	case KEY_KP7:	if(MOD_NUM & mod) res += '7'; break;
	case KEY_KP8:	if(MOD_NUM & mod) res += '8'; break;
	case KEY_KP9:	if(MOD_NUM & mod) res += '9'; break;

        case KEY_MINUS:         res += (MOD_SHIFT & mod ? '_' : '-'); break;
        case KEY_EQUALS:        res += (MOD_SHIFT & mod ? '+' : '='); break;
	case KEY_BACKSLASH:     res += (MOD_SHIFT & mod ? '|' : '\\'); break;
	case KEY_LEFTBRACKET:   res += (MOD_SHIFT & mod ? '{' : '['); break;
	case KEY_RIGHTBRACKET:  res += (MOD_SHIFT & mod ? '}' : ']'); break;
        case KEY_SEMICOLON:     res += (MOD_SHIFT & mod ? ':' : ';'); break;
        case KEY_QUOTE:         res += (MOD_SHIFT & mod ? '"' : '\''); break;
        case KEY_COMMA:         res += (MOD_SHIFT & mod ? '<' : ','); break;
        case KEY_PERIOD:        res += (MOD_SHIFT & mod ? '>' : '.'); break;
        case KEY_SLASH:         res += (MOD_SHIFT & mod ? '?' : '/'); break;

        case KEY_EXCLAIM:       res += '!'; break;
	case KEY_AT:            res += '@'; break;
        case KEY_HASH:          res += '#'; break;
        case KEY_DOLLAR:        res += '$'; break;
        case KEY_AMPERSAND:     res += '&'; break;
        case KEY_ASTERISK:      res += '*'; break;
        case KEY_LEFTPAREN:     res += '('; break;
        case KEY_RIGHTPAREN:    res += ')'; break;
        case KEY_QUOTEDBL:      res += '"'; break;
        case KEY_PLUS:          res += '+'; break;
        case KEY_COLON:         res += ':'; break;
	case KEY_LESS:          res += '<'; break;
	case KEY_GREATER:       res += '>'; break;
	case KEY_QUESTION:      res += '?'; break;
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


        case KEY_BACKSPACE: if(res.size()) res.resize(res.size() - 1); break;
        default: break;
    }
}

std::string String::GetTime(void)
{
    time_t raw;
    struct tm* tmi;
    char buf [10] = { 0 };

    std::time(&raw);
    tmi = std::localtime(&raw);

    std::strftime(buf, sizeof(buf) - 1, "%X", tmi);

    return std::string(buf);
}

int Sign(int s)
{
    return (s < 0 ? -1 : (s > 0 ? 1 : 0));
}

std::string GetDirname(const std::string & str)
{
    if(str.size())
    {
	size_t pos = str.rfind(SEPARATOR);

	if(std::string::npos == pos)
	    return std::string(".");
        else
	if(pos == 0)
	    return str;
	else
	if(pos == str.size() - 1)
	    return GetDirname(str.substr(0, str.size() - 1));
        else
	    return str.substr(0, pos);
    }

    return str;
}

std::string GetBasename(const std::string & str)
{
    if(str.size())
    {
	size_t pos = str.rfind(SEPARATOR);

	if(std::string::npos == pos ||
	    pos == 0) return str;
	else
	if(pos == str.size() - 1)
	    return GetBasename(str.substr(0, str.size() - 1));
	else
	    return str.substr(pos + 1);
    }

    return str;
}

#if defined __SYMBIAN32__
u32 GetMemoryUsage(void)
{
    return 0;
}
#elif defined __WIN32__
#include "windows.h"
u32 GetMemoryUsage(void)
{
    static MEMORYSTATUS ms;
    ZeroMemory(&ms, sizeof(ms));
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return (ms.dwTotalVirtual - ms.dwAvailVirtual);
}
#elif defined __LINUX__
#include "unistd.h"
u32 GetMemoryUsage(void)
{
    unsigned int size = 0;
    std::ostringstream os;
    os << "/proc/" << getpid() << "/statm";

    std::ifstream fs(os.str().c_str());
    if(fs.is_open())
    {
	fs >> size;
        fs.close();
    }

    return size * getpagesize();
}
#else
u32 GetMemoryUsage(void)
{
    return 0;
}
#endif

KeySym KeySymFromChar(char c)
{
    switch(c)
    {
        case '!': return KEY_EXCLAIM;
        case '"': return KEY_QUOTEDBL;
        case '#': return KEY_HASH;
        case '$': return KEY_DOLLAR;
        case '&': return KEY_AMPERSAND;
        case '\'': return KEY_QUOTE;
        case '(': return KEY_LEFTPAREN;
        case ')': return KEY_RIGHTPAREN;
        case '*': return KEY_ASTERISK;
        case '+': return KEY_PLUS;
        case ',': return KEY_COMMA;
        case '-': return KEY_MINUS;
        case '.': return KEY_PERIOD;
        case '/': return KEY_SLASH;
        case ':': return KEY_COLON;
        case ';': return KEY_SEMICOLON;
	case '<': return KEY_LESS;
        case '=': return KEY_EQUALS;
	case '>': return KEY_GREATER;
	case '?': return KEY_QUESTION;
	case '@': return KEY_AT;
	case '[': return KEY_LEFTBRACKET;
	case '\\': return KEY_BACKSLASH;
	case ']': return KEY_RIGHTBRACKET;
	case '^': return KEY_CARET;
	case '_': return KEY_UNDERSCORE;
        case ' ': return KEY_SPACE;

        case 'a': return KEY_a;
        case 'b': return KEY_b;
        case 'c': return KEY_c;
        case 'd': return KEY_d;
        case 'e': return KEY_e;
        case 'f': return KEY_f;
        case 'g': return KEY_g;
        case 'h': return KEY_h;
        case 'i': return KEY_i;
        case 'j': return KEY_j;
        case 'k': return KEY_k;
        case 'l': return KEY_l;
        case 'm': return KEY_m;
        case 'n': return KEY_n;
        case 'o': return KEY_o;
        case 'p': return KEY_p;
        case 'q': return KEY_q;
        case 'r': return KEY_r;
        case 's': return KEY_s;
        case 't': return KEY_t;
        case 'u': return KEY_u;
        case 'v': return KEY_v;
        case 'w': return KEY_w;
        case 'x': return KEY_x;
        case 'y': return KEY_y;
        case 'z': return KEY_z;

        case '0': return KEY_0;
        case '1': return KEY_1;
        case '2': return KEY_2;
        case '3': return KEY_3;
        case '4': return KEY_4;
        case '5': return KEY_5;
        case '6': return KEY_6;
        case '7': return KEY_7;
        case '8': return KEY_8;
        case '9': return KEY_9;

        default: break;
    }
    return KEY_NONE;
}

bool FilePresent(const std::string & file)
{
    std::ifstream fs;
    // check file
    fs.open(file.c_str(), std::ios::binary);
    if(fs.is_open())
    {
        fs.close();
        return true;
    }
    return false;
}

bool SaveMemToFile(const std::vector<u8> & data, const std::string & file)
{
    std::ofstream fs;
    fs.open(file.c_str(), std::ios::binary);
    if(fs.is_open() && data.size())
    {
        fs.write(reinterpret_cast<const char*>(&data[0]), data.size());
        fs.close();
        return true;
    }
    return false;
}

bool LoadFileToMem(std::vector<u8> & data, const std::string & file)
{
    std::ifstream fs;
    fs.open(file.c_str(), std::ios::binary);
    if(fs.is_open())
    {
        fs.seekg(0, std::ios_base::end);
        data.resize(fs.tellg());
        fs.seekg(0, std::ios_base::beg);
        fs.read(reinterpret_cast<char*>(&data[0]), data.size());
        fs.close();
        return true;
    }
    return false;
}

bool PressIntKey(u32 min, u32 max, u32 & result)
{
    LocalEvent & le = LocalEvent::Get();

    if(le.KeyPress(KEY_BACKSPACE))
    {
            if(min < result)
            {
                result /= 10;
                if(result < min) result = min;
            }
            return true;
    }
    else
    if(le.KeyPress() && KEY_0 <= le.KeyValue() && KEY_9 >= le.KeyValue())
    {
	if(max > result)
        {
            result *= 10;
            switch(le.KeyValue())
            {
                    case KEY_1: result += 1; break;
                    case KEY_2: result += 2; break;
                    case KEY_3: result += 3; break;
                    case KEY_4: result += 4; break;
                    case KEY_5: result += 5; break;
                    case KEY_6: result += 6; break;
                    case KEY_7: result += 7; break;
                    case KEY_8: result += 8; break;
                    case KEY_9: result += 9; break;

		    case KEY_KP1: result += 1; break;
		    case KEY_KP2: result += 2; break;
		    case KEY_KP3: result += 3; break;
		    case KEY_KP4: result += 4; break;
		    case KEY_KP5: result += 5; break;
		    case KEY_KP6: result += 6; break;
		    case KEY_KP7: result += 7; break;
		    case KEY_KP8: result += 8; break;
		    case KEY_KP9: result += 9; break;

                    default: break;
            }
            if(result > max) result = max;
	}
        return true;
    }
    return false;
}

void ToolsSrcRectFixed(Rect & src, s16 & rx, s16 & ry, const u16 rw, const u16 rh, const Rect & max)
{
    src = Rect(0, 0, 0, 0);

    if(0 != rw && 0 != rh &&
        rx + rw > max.x && ry + rh > max.y &&
        rx < max.x + max.w && ry < max.y + max.h)
    {
        src.w = rw;
        src.h = rh;

        if(rx < max.x)
        {
            src.x = max.x - rx;
            rx = max.x;
        }

        if(ry < max.y)
        {
            src.y = max.y - ry;
            ry = max.y;
        }

        if(rx + rw > max.x + max.w)
            src.w = max.x + max.w - rx;

        if(ry + rh > max.y + max.h)
            src.h = max.y + max.h - ry;
    }
}

#ifdef WITH_ICONV
#include <iconv.h>
std::string EncodeString(const std::string & str, const char* charset)
{
    iconv_t cd;

    if(! charset || (iconv_t)(-1) == (cd = iconv_open("utf-8", charset)))
	return str;

    std::string res(str);
    size_t inbytesleft = str.size();
    size_t outbytesleft = inbytesleft * 2 + 1;
    const char* inbuf = str.c_str();
    char* outbuf1 = new char [outbytesleft];
    char* outbuf2 = outbuf1;

    size_t reslen = iconv(cd, const_cast<char**>(&inbuf), &inbytesleft, &outbuf1, &outbytesleft);
    iconv_close(cd);

    if(reslen != (size_t)(-1))
	res = std::string(outbuf2, outbuf1 - outbuf2);

    delete [] outbuf2;

    return res;
}
#else
void cp1251_to_utf8(char *out, const char *in)
{
    static const int table[128] = {
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };

    while(*in)
    if(*in & 0x80)
    {
	int v = table[(int) (0x7f & *in++)];
        if(!v) continue;
	*out++ = (char) v;
        *out++ = (char) (v >> 8);
	if(v >>= 16) *out++ = (char) v;
    }
    else
	*out++ = *in++;
    *out = 0;
}

std::string EncodeString(const std::string & str, const char* charset)
{
    std::string res(str);

    if(charset)
    {
	if(0 == std::strcmp(charset, "cp1251"))
	{
	    size_t inbytes = str.size();
	    size_t outbytes = inbytes * 2 + 1;
	    char* outbuf= new char [outbytes];

	    cp1251_to_utf8(outbuf, str.c_str());
	    res = std::string(outbuf);

	    delete [] outbuf;
	}
    }

    return res;
}
#endif
