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
#include "text.h"

#define WIDTH_BIG	0x0E
#define WIDTH_SMALL	0x07
#define HEIGHT_BIG	0x10
#define HEIGHT_SMALL	0x0B

Text::Text(u16 ax, u16 ay, const std::string &msg, Font::type_t ft, bool draw) : font(ft), message(msg)
{
    pos.x = ax;
    pos.y = ay;
    pos.h = (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG);
    pos.w = 0;

    std::string::const_iterator it = message.begin();
    std::string::const_iterator it_end = message.end();

    for(; it != it_end; ++it)
        if(0x20 == *it) pos.w += (Font::SMALL == ft ? WIDTH_SMALL : WIDTH_BIG);
        else
            pos.w += AGG::GetLetter(*it, ft).w();

    if(draw) Blit();
}

void Text::Blit(void)
{
    std::string::const_iterator it = message.begin();
    std::string::const_iterator it_end = message.end();

    Point pt(pos.x, pos.y);
    u16 y1 = pos.y;

    for(; it != it_end; ++it){

	pt.y = y1;

	// space or unknown letter
	if(*it < 0x21){
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

	display.Blit(sprite, pt);

	pt.x += sprite.w();
    }
}

u8 Text::width(char ch, Font::type_t ft)
{
    if(ch < 0x21)
	return (Font::SMALL == ft ? WIDTH_SMALL / 2 : WIDTH_BIG / 2);

    return AGG::GetLetter(ch, ft).w();
}

u16 Text::width(const std::string &str, u16 start, u16 count, Font::type_t ft)
{
    u16 res = 0;

    for(int ii = start; ii < start + count; ++ii) res += Text::width(str[ii], ft);
    
    return res;
}

u16 Text::height(u16 width, const std::string &str, Font::type_t ft)
{
    if(str.empty()) return 0;
    
    u16 pos_last = str.size() - 1;

    u16 s_start = 0;
    u16 s_end = pos_last;
    u16 res = 0;

    while(s_start < s_end){

	while(s_start < s_end && width < Text::width(str, s_start, s_end - s_start, ft)){
    
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

TextBox::TextBox(const Rect &rt, const std::string &msg, Font::type_t ft, bool draw) : pos(rt)
{

    u16 pos_last = msg.size() - 1;

    u16 s_start = 0;
    u16 s_end = pos_last;
    Point pt(pos.x, pos.y + 2);

    while(s_start < s_end){

	while(s_start < s_end && pos.w < Text::width(msg, s_start, s_end - s_start, ft)){
    
	    while(s_start < s_end && !isspace(msg[s_end])) --s_end;

    	    --s_end;
	}
    	++s_end;

	pt.x = pos.x + (pos.w - Text::width(msg, s_start, s_end - s_start, ft)) / 2;
	
	box.push_back(Text(pt.x, pt.y, msg.substr(s_start, s_end - s_start), ft));
	
	pt.y += (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG) + 1;

	s_start = s_end + 1;
	s_end = pos_last;
    }

    if(draw) Redraw();
}

void TextBox::Redraw(void)
{
    std::vector<Text>::iterator it = box.begin();
    std::vector<Text>::iterator it_end = box.end();
    
    for(; it != it_end; ++it) (*it).Blit();
}
