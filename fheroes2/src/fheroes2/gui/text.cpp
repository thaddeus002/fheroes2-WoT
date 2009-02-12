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

#include "agg.h"
#include "settings.h"
#include "text.h"

#define WIDTH_BIG	0x0E
#define WIDTH_SMALL	0x07
#define HEIGHT_BIG	0x10
#define HEIGHT_SMALL	0x0B

TextAscii::TextAscii(const std::string & msg, Font::type_t ft) : TextInterface(ft), message(msg)
{
}

void TextAscii::SetText(const std::string & msg)
{
    message = msg;
}

void TextAscii::SetFont(const Font::type_t & ft)
{
    font = ft;
}

void TextAscii::Clear(void)
{
    message.clear();
}

u8 TextAscii::char_w(char c, Font::type_t f)
{
    return (c < 0x21 ? (Font::SMALL == f ? WIDTH_SMALL / 2 : WIDTH_BIG / 2) : AGG::GetLetter(c, f).w());
}

u16 TextAscii::w(u16 s, u16 c) const
{
    if(message.empty()) return 0;

    u16 res = 0;

    if(s > message.size() - 1) s = message.size() - 1;
    if(!c || c > message.size()) c = message.size() - s;

    for(u16 ii = s; ii < s + c; ++ii)
	res += char_w(message[ii], font);

    return res;
}

u16 TextAscii::w(void) const
{
    return w(0, message.size());
}

u16 TextAscii::h(void) const
{
    return h(0);
}

u16 TextAscii::h(const u16 width) const
{
    if(message.empty()) return 0;
    else
    if(0 == width || w() <= width) return (Font::SMALL == font ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

    u16 res = 0;
    u16 www = 0;
    
    std::string::const_iterator pos1 = message.begin();
    std::string::const_iterator pos2 = message.end();
    std::string::const_iterator space = pos2;

    while(pos1 < pos2)
    {
        if(std::isspace(*pos1)) space = pos1;

	if(www + char_w(*pos1, font) >= width)
	{
	    www = 0;
	    res += (Font::SMALL == font ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	    if(pos2 != space) pos1 = space + 1;
	    space = pos2;
	    continue;
	}

        www += char_w(*pos1, font);
        ++pos1;
    }

    return res;
}

void TextAscii::Blit(u16 ax, u16 ay, Surface & dst)
{
    if(message.empty()) return;

    std::string::const_iterator it = message.begin();
    std::string::const_iterator it_end = message.end();
    u8 oy = 0;

    for(; it != it_end; ++it)
    {
	// space or unknown letter
	if(*it < 0x21)
	{
	    ax += (Font::SMALL == font ? WIDTH_SMALL / 2 : WIDTH_BIG / 2);
	    continue;
	}

	const Surface & sprite = AGG::GetLetter(*it, font);
	if(!sprite.valid()) return;

        // valign
	switch(*it)
	{
	    case '-':
    		oy = (Font::SMALL == font ? HEIGHT_SMALL / 2 : HEIGHT_BIG / 2);
    	    break;

    	    // "
    	    case 0x22:
	    // '
    	    case 0x27:
        	oy = (Font::SMALL == font ? 2 : 3);
    	    break;

    	    case 'y':
    	    case 'g':
    	    case 'p':
    	    case 'q':
    	    case 'j':
        	oy = (Font::SMALL == font ? HEIGHT_SMALL - sprite.h() + 2 : HEIGHT_BIG - sprite.h() + 2);
    	    break;

    	    default:
        	if(Font::SMALL == font && sprite.h() < HEIGHT_SMALL) oy = HEIGHT_SMALL - sprite.h();
        	else
		if(Font::SMALL != font && sprite.h() < HEIGHT_BIG) oy = HEIGHT_BIG - sprite.h();
		else oy = 0;
            break;
	}

	dst.Blit(sprite, ax, ay + oy);
	ax += sprite.w();
    }
}

TextUnicode::TextUnicode(const std::string & msg, Font::type_t ft) : TextInterface(ft), message(2 * msg.size(), 0)
{
    if(msg.size())
    {
	String::UTF8_to_UNICODE(&message[0], msg.c_str(), msg.size());
	while(0 == message.back()) message.pop_back();
    }
}

TextUnicode::TextUnicode(const u16 *pt, size_t sz, Font::type_t ft) : TextInterface(ft), message(pt, pt+sz)
{
}

bool TextUnicode::isspace(u16 c)
{
    switch(c)
    {
	case 0x0009:
	case 0x000a:
	case 0x000b:
	case 0x000c:
	case 0x000d:
	case 0x0020: return true;

	default: break;
    }

    return false;
}

void TextUnicode::SetText(const std::string & msg)
{
    if(msg.size())
    {
	message.resize(2 * msg.size(), 0);
	std::fill(message.begin(), message.end(), 0);
	String::UTF8_to_UNICODE(&message[0], msg.c_str(), msg.size());
	while(0 == message.back()) message.pop_back();
    }
}

void TextUnicode::SetFont(const Font::type_t & ft)
{
    font = ft;
}

void TextUnicode::Clear(void)
{
    message.clear();
}

u8 TextUnicode::char_w(u16 c, Font::type_t f)
{
    return (c < 0x0021 ? (Font::SMALL == f ? WIDTH_SMALL / 2 : WIDTH_BIG / 2) : AGG::GetUnicodeLetter(c, f).w());
}

u16 TextUnicode::w(u16 s, u16 c) const
{
    if(message.empty()) return 0;

    u16 res = 0;

    if(s > message.size() - 1) s = message.size() - 1;
    if(!c || c > message.size()) c = message.size() - s;

    for(u16 ii = s; ii < s + c; ++ii)
	res += char_w(message[ii], font);

    return res;
}

u16 TextUnicode::w(void) const
{
    return w(0, message.size());
}

u16 TextUnicode::h(void) const
{
    return h(0);
}

u16 TextUnicode::h(const u16 width) const
{
    if(message.empty()) return 0;
    else
    if(0 == width || w() <= width) return (Font::SMALL == font ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

    u16 res = 0;
    u16 www = 0;
    
    std::vector<u16>::const_iterator pos1 = message.begin();
    std::vector<u16>::const_iterator pos2 = message.end();
    std::vector<u16>::const_iterator space = pos2;

    while(pos1 < pos2)
    {
        if(isspace(*pos1)) space = pos1;

	if(www + char_w(*pos1, font) >= width)
	{
	    www = 0;
	    res += (Font::SMALL == font ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	    if(pos2 != space) pos1 = space + 1;
	    space = pos2;
	    continue;
	}

        www += char_w(*pos1, font);
        ++pos1;
    }

    return res;
}

void TextUnicode::Blit(u16 ax, u16 ay, Surface & dst)
{
    if(message.empty()) return;

    std::vector<u16>::const_iterator it = message.begin();
    std::vector<u16>::const_iterator it_end = message.end();
    u8 oy = 0;

    for(; it != it_end; ++it)
    {
	// end string
	if(0 == *it) continue;

	// TODO: size font should depend from ttf

	// space or unknown letter
	if(*it < 0x0021)
	{
	    ax += (Font::SMALL == font ? WIDTH_SMALL / 2 : WIDTH_BIG / 2);
	    continue;
	}

	const Surface & sprite = AGG::GetUnicodeLetter(*it, font);
	if(!sprite.valid()) return;

        // valign
	switch(*it)
	{
	    case L'-':
    		oy = (Font::SMALL == font ? HEIGHT_SMALL / 2 : HEIGHT_BIG / 2);
    	    break;

    	    // "
    	    case L'"':
	    // '
    	    case L'\'':
        	oy = (Font::SMALL == font ? 2 : 3);
    	    break;

    	    case L'y':
    	    case L'g':
    	    case L'p':
    	    case L'q':
    	    case L'j':
        	oy = (Font::SMALL == font ? HEIGHT_SMALL - sprite.h() + 2 : HEIGHT_BIG - sprite.h() + 2);
    	    break;

    	    default:
        	if(Font::SMALL == font && sprite.h() < HEIGHT_SMALL) oy = HEIGHT_SMALL - sprite.h();
        	else
		if(Font::SMALL != font && sprite.h() < HEIGHT_BIG) oy = HEIGHT_BIG - sprite.h();
		else oy = 0;
            break;
	}

	dst.Blit(sprite, ax, ay + oy);
	ax += sprite.w();
    }
}




Text::Text() : message(NULL), gw(0), gh(0)
{
    if(Settings::Get().Unicode())
	message = static_cast<TextInterface *>(new TextUnicode());
    else
	message = static_cast<TextInterface *>(new TextAscii());
}

Text::Text(const std::string & msg, Font::type_t ft) : message(NULL), gw(0), gh(0)
{
    if(Settings::Get().Unicode())
	message = static_cast<TextInterface *>(new TextUnicode(msg, ft));
    else
	message = static_cast<TextInterface *>(new TextAscii(msg, ft));

    gw = message->w();
    gh = (ft == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL);
}

Text::Text(const u16 *pt, size_t sz, Font::type_t ft) : message(NULL), gw(0), gh(0)
{
    if(Settings::Get().Unicode() && pt)
    {
    	message = static_cast<TextInterface *>(new TextUnicode(pt, sz, ft));

	gw = message->w();
	gh = (ft == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL);
    }
}

Text::~Text()
{
    delete message;
}

Text::Text(const Text & t)
{
    if(Settings::Get().Unicode())
	message = static_cast<TextInterface *>(new TextUnicode(static_cast<TextUnicode &>(*t.message)));
    else
	message = static_cast<TextInterface *>(new TextAscii(static_cast<TextAscii &>(*t.message)));
    gw = t.gw;
    gh = t.gh;
}

Text & Text::operator= (const Text & t)
{
    delete message;
    if(Settings::Get().Unicode())
	message = static_cast<TextInterface *>(new TextUnicode(static_cast<TextUnicode &>(*t.message)));
    else
	message = static_cast<TextInterface *>(new TextAscii(static_cast<TextAscii &>(*t.message)));
    gw = t.gw;
    gh = t.gh;

    return *this;
}

void Text::Set(const std::string & msg, Font::type_t ft)
{
    message->SetText(msg);
    message->SetFont(ft);
    gw = message->w();
    gh = (ft == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL);
}

void Text::Set(const std::string & msg)
{
    message->SetText(msg);
    gw = message->w();
    gh = (message->font == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL);
}

void Text::Set(Font::type_t ft)
{
    message->SetFont(ft);
    gw = message->w();
    gh = (ft == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL);
}

void Text::Clear(void)
{
    message->Clear();
    gw = 0;
    gh = 0;
}

void Text::Blit(const Point & dst_pt, Surface & dst) const
{
    return message->Blit(dst_pt.x, dst_pt.y, dst);
}

void Text::Blit(u16 ax, u16 ay, Surface & dst) const
{
    return message->Blit(ax, ay, dst);
}

u16 Text::width(const std::string &str, Font::type_t ft, u16 start, u16 count)
{
    if(Settings::Get().Unicode())
    {
	TextUnicode text(str, ft);
	return text.w(start, count);
    }
    else
    {
	TextAscii text(str, ft);
	return text.w(start, count);
    }

    return 0;
}

u16 Text::height(const std::string &str, Font::type_t ft, u16 width)
{
    if(str.empty()) return 0;
    else
    if(! width) return ft == Font::BIG ? HEIGHT_BIG : HEIGHT_SMALL;

    if(Settings::Get().Unicode())
    {
	TextUnicode text(str, ft);
	return text.h(width);
    }
    else
    {
	TextAscii text(str, ft);
	return text.h(width);
    }

    return 0;
}

TextBox::TextBox(const std::string & msg, Font::type_t ft, u16 width)
{
    Set(msg, ft, width);
}

TextBox::TextBox(const std::list<std::string> & list, Font::type_t ft, u16 width)
{
    Set(list, ft, width);
}

TextBox::TextBox(const std::string & msg, Font::type_t ft, const Rect & rt)
{
    Set(msg, ft, rt.w);
    Blit(rt.x, rt.y);
}

TextBox::TextBox(const std::list<std::string> & list, Font::type_t ft, const Rect & rt)
{
    Set(list, ft, rt.w);
    Blit(rt.x, rt.y);
}

void TextBox::Set(const std::list<std::string> & list, Font::type_t ft, u16 width)
{
    if(messages.size()) messages.clear();
    if(list.empty()) return;

    std::list<std::string>::const_iterator it1 = list.begin();
    std::list<std::string>::const_iterator it2 = list.end();

    for(; it1 != it2; ++it1)
    if(Settings::Get().Unicode())
    {
	const std::string & msg = *it1;
	std::vector<u16> unicode(2 * msg.size(), 0);
	String::UTF8_to_UNICODE(&unicode[0], msg.c_str(), msg.size());
	while(0 == unicode.back()) unicode.pop_back();
	Append(unicode, ft, width);
    }
    else
	Append(*it1, ft, width);
}

void TextBox::Set(const std::string & msg, Font::type_t ft, u16 width)
{
    if(messages.size()) messages.clear();
    if(msg.empty()) return;

    if(Settings::Get().Unicode())
    {
	std::vector<u16> unicode(2 * msg.size(), 0);
	String::UTF8_to_UNICODE(&unicode[0], msg.c_str(), msg.size());
	while(0 == unicode.back()) unicode.pop_back();
	Append(unicode, ft, width);
    }
    else
	Append(msg, ft, width);
}

void TextBox::Append(const std::string & msg, Font::type_t ft, u16 width)
{
    if(msg.empty()) return;

    u16 www = 0;
    Rect::w = width;
    
    std::string::const_iterator pos1 = msg.begin();
    std::string::const_iterator pos2 = pos1;
    std::string::const_iterator pos3 = msg.end();
    std::string::const_iterator space = pos2;

    while(pos2 < pos3)
    {
        if(std::isspace(*pos2)) space = pos2;
	u8 char_w = TextAscii::char_w(*pos2, ft);

	if(www + char_w >= width)
	{
	    www = 0;
	    Rect::h += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	    if(pos3 != space) pos2 = space + 1;

	    if(pos3 != space)
	        messages.push_back(Text(msg.substr(pos1 - msg.begin(), pos2 - pos1 - 1), ft));
	    else
	        messages.push_back(Text(msg.substr(pos1 - msg.begin(), pos2 - pos1), ft));

	    pos1 = pos2;
	    space = pos3;
	    continue;
	}

        www += char_w;
        ++pos2;
    }

    if(pos1 != pos2)
    {
        Rect::h += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	messages.push_back(Text(msg.substr(pos1 - msg.begin(), pos2 - pos1), ft));
    }
}

void TextBox::Append(const std::vector<u16> & msg, Font::type_t ft, u16 width)
{
    if(msg.empty()) return;

    u16 www = 0;
    Rect::w = width;
    
    std::vector<u16>::const_iterator pos1 = msg.begin();
    std::vector<u16>::const_iterator pos2 = pos1;
    std::vector<u16>::const_iterator pos3 = msg.end();
    std::vector<u16>::const_iterator space = pos2;

    while(pos2 < pos3)
    {
        if(TextUnicode::isspace(*pos2)) space = pos2;
	u8 char_w = TextUnicode::char_w(*pos2, ft);

	if(www + char_w >= width)
	{
	    www = 0;
	    Rect::h += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	    if(pos3 != space) pos2 = space + 1;

	    if(pos3 != space)
		messages.push_back(Text(&msg.at(pos1 - msg.begin()), pos2 - pos1 - 1, ft));
	    else
		messages.push_back(Text(&msg.at(pos1 - msg.begin()), pos2 - pos1, ft));

	    pos1 = pos2;
	    space = pos3;
	    continue;
	}

        www += char_w;
        ++pos2;
    }

    if(pos1 != pos2)
    {
        Rect::h += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;
	messages.push_back(Text(&msg.at(pos1 - msg.begin()), pos2 - pos1, ft));
    }
}

void TextBox::Blit(u16 ax, u16 ay, Surface & sf)
{
    Rect::x = ax;
    Rect::y = ay;
    
    std::list<Text>::const_iterator it1 = messages.begin();
    std::list<Text>::const_iterator it2 = messages.end();
    
    for(; it1 != it2; ++it1)
    {
    	(*it1).Blit(ax + (Rect::w - (*it1).w()) / 2, ay);
    	ay += (*it1).h();
    }
}

void TextBox::Blit(const Point & pt, Surface & sf)
{
    Blit(pt.x, pt.y, sf);
}

TextSprite::TextSprite() : hide(true)
{
}

TextSprite::TextSprite(const std::string & msg, Font::type_t ft, const Point & pt) : Text(msg, ft), hide(true)
{
    back.Save(Rect(pt, gw, gh + 5));
}

TextSprite::TextSprite(const std::string & msg, Font::type_t ft, u16 ax, u16 ay) : Text(msg, ft), hide(true)
{
    back.Save(Rect(ax, ay, gw, gh + 5));
}

void TextSprite::Show(void)
{
    Blit(back.GetRect().x, back.GetRect().y);
    hide = false;
}

void TextSprite::Hide(void)
{
    if(!hide) back.Restore();
    hide = true;
}

void TextSprite::SetText(const std::string & msg)
{
    Hide();
    Set(msg);
    back.Save(back.GetPos().x, back.GetPos().y, gw, gh + 5);
}

void TextSprite::SetFont(Font::type_t ft)
{
    Hide();
    Set(ft);
    back.Save(back.GetPos().x, back.GetPos().y, gw, gh + 5);
}

void TextSprite::SetPos(u16 ax, u16 ay)
{
    back.Save(ax, ay, gw, gh + 5);
}

u16 TextSprite::w(void)
{
    return back.GetSize().w;
}

u16 TextSprite::h(void)
{
    return back.GetSize().h;
}

bool TextSprite::isHide(void) const
{
    return hide;
}

bool TextSprite::isShow(void) const
{
    return !hide;
}

const Rect & TextSprite::GetRect(void) const
{
    return back.GetRect();
}
