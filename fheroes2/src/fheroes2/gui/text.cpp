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

#include <string>
#include "agg.h"
#include "sprite.h"
#include "display.h"
#include "tools.h"
#include "text.h"

#define WIDTH_BIG	0x0E
#define WIDTH_SMALL	0x07
#define HEIGHT_BIG	0x10
#define HEIGHT_SMALL	0x0B

u8 Char::width(char ch, Font::type_t ft)
{
    if(ch < 0x21)
	return (Font::SMALL == ft ? WIDTH_SMALL / 2 : WIDTH_BIG / 2);

    return AGG::GetLetter(ch, ft).w();
}

/* text string */
Text::Text(const std::string & msg, Font::type_t ft) : font(ft), message(msg)
{
}

Text::Text(const std::string &msg, Font::type_t ft, u16 ax, u16 ay) : font(ft), message(msg)
{
    Blit(ax, ay);
}

Text::Text(const std::string &msg, Font::type_t ft, const Point & pt) : font(ft), message(msg)
{
    Blit(pt.x, pt.y);
}

Text::Text(const std::string &msg, Font::type_t ft, const Rect & rt) : font(ft), message(msg)
{
    Blit(rt.x, rt.y);
}

void Text::Blit(const Point & dst_pt, Surface & dst)
{
    Blit(dst_pt.x, dst_pt.y, dst);
}

void Text::Blit(u16 ax, u16 ay, Surface & dst)
{
    std::string::const_iterator it = message.begin();
    std::string::const_iterator it_end = message.end();

    Point pt(ax, ay);
    u16 y1 = ay;

    for(; it != it_end; ++it){

	pt.y = y1;

	// space or unknown letter
	if(*it < 0x21)
	{
	    pt.x += (Font::SMALL == font ? WIDTH_SMALL / 2 : WIDTH_BIG / 2);
	    continue;
	}

	const Sprite &sprite = AGG::GetLetter(*it, font);
        // valign
	switch(*it){

	    case '-':
    		pt.y += (Font::SMALL == font ? HEIGHT_SMALL / 2 : HEIGHT_BIG / 2);
    	    break;

    	    // "
    	    case 0x22:
	    // '
    	    case 0x27:
        	pt.y += (Font::SMALL == font ? 2 : 3);
    	    break;

    	    case 'y':
    	    case 'g':
    	    case 'p':
    	    case 'q':
    	    case 'j':
        	pt.y += (Font::SMALL == font ? HEIGHT_SMALL - sprite.h() + 2 : HEIGHT_BIG - sprite.h() + 2);
    	    break;

    	    default:
        	if(Font::SMALL == font && sprite.h() < HEIGHT_SMALL) pt.y += HEIGHT_SMALL - sprite.h();
        	else
		if(Font::BIG == font && sprite.h() < HEIGHT_BIG) pt.y += HEIGHT_BIG - sprite.h();
            break;
	}

	dst.Blit(sprite, pt);

	pt.x += sprite.w();
    }
}

u16 Text::width(const std::string &str,  Font::type_t ft, u16 start, u16 count)
{
    u16 res = 0;

    if(0xffff == count) count = str.size();

    for(int ii = start; ii < start + count; ++ii) res += Char::width(str[ii], ft);
    
    return res;
}

u16 Text::height(const std::string &str, Font::type_t ft, u16 width)
{
    if(str.empty()) return 0;

    if(! width) return (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

    u16 pos_last = str.size() - 1;

    u16 s_start = 0;
    u16 s_end = pos_last;
    u16 res = 0;

    while(s_start < s_end)
    {
	while(s_start < s_end && width < Text::width(str, ft, s_start, s_end - s_start))
	{
	    while(s_start < s_end && !isspace(str[s_end])) --s_end;

    	    --s_end;
	}
    	++s_end;

	res += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

	s_start = s_end + 1;
	s_end = pos_last;
    }

    return res;
}

TextBox::TextBox(const std::string & msg, Font::type_t ft, const Rect & rt)
{
    if(msg.empty()) return;

    u16 pos_last = msg.size() - 1;

    u16 s_start = 0;
    u16 s_end = pos_last;
    Point pt(rt.x, rt.y + 2);
    extents.x = rt.x;
    extents.y = rt.y;
    extents.w = 0;

    while(s_start <= s_end)
    {
	while(s_start < s_end && rt.w < Text::width(msg, ft, s_start, s_end - s_start))
	{
	    while(s_start < s_end && !isspace(msg[s_end])) --s_end;

    	    --s_end;
	}
    	++s_end;

	int width = Text::width(msg, ft, s_start, s_end - s_start);
        pt.x = rt.x + (rt.w - width) / 2;
	
	Text(msg.substr(s_start, s_end - s_start), ft, pt);
        
        if(width > extents.w)
             extents.w = width;
	
	pt.y += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

	s_start = s_end + 1;
	s_end = pos_last;
    }
    
    extents.h = pt.y - extents.y;
}

TextBox::TextBox(const std::list<std::string> & list, Font::type_t ft, const Rect & rt)
{
    extents.x = rt.x;
    extents.y = rt.y;
    extents.w = 0;
    Point pt(rt.x, rt.y + 2);

    std::list<std::string>::const_iterator it1 = list.begin();
    std::list<std::string>::const_iterator it2 = list.end();
    for(; it1 != it2; ++it1) if((*it1).size())
    {
	const std::string & msg = *it1;

	u16 pos_last = msg.size() - 1;
	u16 s_start = 0;
	u16 s_end = pos_last;

	while(s_start <= s_end)
	{
	    while(s_start < s_end && rt.w < Text::width(msg, ft, s_start, s_end - s_start))
	    {
		while(s_start < s_end && !isspace(msg[s_end])) --s_end;

    		--s_end;
	    }
    	    ++s_end;

	    int width = Text::width(msg, ft, s_start, s_end - s_start);
    	    pt.x = rt.x + (rt.w - width) / 2;
	
	    Text(msg.substr(s_start, s_end - s_start), ft, pt);
        
    	    if(width > extents.w)
        	extents.w = width;
	
	    pt.y += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

	    s_start = s_end + 1;
	    s_end = pos_last;
	}
    }

    extents.h = pt.y - extents.y;
}

Text2::Text2(const std::string & msg, Font::type_t ft, const Point & pt) : Text(msg, ft)
{
    back.Save(Rect(pt, Text::width(msg, ft), Text::height(msg, ft, Text::width(msg, ft))));
}

Text2::Text2(const std::string & msg, Font::type_t ft, u16 ax, u16 ay) : Text(msg, ft)
{
    back.Save(Rect(ax, ay, Text::width(msg, ft), Text::height(msg, ft, Text::width(msg, ft))));
}

void Text2::Show(void)
{
    Blit(back.GetRect().x, back.GetRect().y);
}

void Text2::Hide(void)
{
    back.Restore();
}

void Text2::SetPos(u16 ax, u16 ay)
{
    back.Restore();
    
    back.Save(Rect(ax, ay, width(), height()));
}
