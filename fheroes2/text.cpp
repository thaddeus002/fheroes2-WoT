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
#include "event.h"
#include "cursor.h"
#include "text.h"

#define LETTER_BEGIN	0x20
#define LETTER_END	0x7F

#define WIDTH_BIG	0x0E
#define WIDTH_SMALL	0x07
#define HEIGHT_BIG	0x10
#define HEIGHT_SMALL	0x0B

Text::Text(const Point &pt, const std::string &message, Font::type_t ft) : Rect(), font(ft)
{
    x = pt.x;
    y = pt.y;
    h = (Font::SMALL == ft ? HEIGHT_SMALL : HEIGHT_BIG);
    w = 0;
    const std::string icn((Font::SMALL == ft ? "SMALFONT.ICN" : "FONT.ICN"));
    
    std::string::const_iterator it = message.begin();
    std::string::const_iterator it_end = message.end();

    letter_t chr;

    for(; it != it_end; ++it){

	w += (Font::SMALL == ft ? WIDTH_SMALL : WIDTH_BIG);

	chr.letter = *it;
	chr.sprite = (LETTER_BEGIN >= *it || LETTER_END <= *it ? NULL : &AGG::GetICN(icn, *it - LETTER_BEGIN));

	sprites.push_back(chr);
    }
    
    Redraw();
}

void Text::Redraw(void)
{
    // hide cursor
    LocalEvent & le = LocalEvent::GetLocalEvent();
    bool hide = Cursor::Visible();
    if(hide && *this & le.MouseCursor()) Cursor::Hide();

    std::vector<letter_t>::const_iterator it = sprites.begin();
    std::vector<letter_t>::const_iterator it_end = sprites.end();

    Point pt(x, y);
    u16 y1 = y;

    for(; it != it_end; ++it){

	pt.y = y1;
    
        // valign
	const letter_t &lt = *it;

	switch(lt.letter){

	    case '-':
    		pt.y += (Font::SMALL == font ? HEIGHT_SMALL / 2 : HEIGHT_BIG / 2);
    	    break;

    	    // '
    	    case 0x27:
    	    break;

    	    case 'y':
    	    case 'g':
    	    case 'p':
    	    case 'q':
    	    case 'j':
        	pt.y += (Font::SMALL == font ? HEIGHT_SMALL - lt.sprite->h() + 2 : HEIGHT_BIG - lt.sprite->h() + 2);
    	    break;

    	    default:
        	if(Font::SMALL == font && lt.sprite && lt.sprite->h() < HEIGHT_SMALL) pt.y += HEIGHT_SMALL - lt.sprite->h();
        	else
		if(Font::BIG == font && lt.sprite && lt.sprite->h() < HEIGHT_BIG) pt.y += HEIGHT_BIG - lt.sprite->h();
            break;
	}

	if(lt.sprite) display.Blit(*lt.sprite, pt);

	if(lt.sprite) pt.x += lt.sprite->w(); else pt.x += (Font::SMALL == font ? WIDTH_SMALL : WIDTH_BIG);
	
	if(pt.x > x + w){ pt.x = x; y1 += (Font::SMALL == font ? HEIGHT_SMALL : HEIGHT_BIG); }
    }

    if(hide) Cursor::Show();
}
