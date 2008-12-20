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

#ifndef H2TEXT_H
#define H2TEXT_H

#include <vector>
#include "gamedefs.h"

namespace Font
{
    typedef enum { SMALL = 0x01, BIG = 0x02, YELLOWBIG = 0x04 } type_t;
};

namespace Char
{
    u8 width(char ch, Font::type_t ft);
    u8 height(char ch, Font::type_t ft);
};

class Text
{
public:
    typedef enum { LEFT, CENTER, RIGHT } align_t;

    Text();
    Text(const std::string & msg, Font::type_t ft);

    // with blit
    Text(const std::string & msg, Font::type_t ft, const Point & dst_pt);
    Text(const std::string & msg, Font::type_t ft, const Rect & dst_rt);
    Text(const std::string & msg, Font::type_t ft, u16 ax, u16 ay);

    void Set(const std::string & msg, Font::type_t ft){ message = msg; font = ft; gw = width(); };
    void Set(const std::string & msg){  message = msg; gw = width(); };
    void Set(Font::type_t ft){ font = ft; gw = width(); };

    void Clear(void){ message.clear(); gw = 0; };

    u16 w(void){ return gw; }
    u16 h(void){ return gh; }

    u16 width(u16 start = 0, u16 count = 0xffff){ return Text::width(message, font, start, count); };
    u16 height(u16 width = 0){ return Text::height(message, font, width); };

    static u16 width(const std::string &str, Font::type_t ft, u16 start = 0, u16 count = 0xffff);
    static u16 height(const std::string &str, Font::type_t ft, u16 width = 0);

    void Blit(u16 ax, u16 ay, Surface & sf = Display::Get());
    void Blit(const Point & dst_pt, Surface & sf = Display::Get());

protected:
    Font::type_t font;
    std::string message;
    u16 gw;
    u8  gh;
};

class TextSprite : protected Text
{
public:
    TextSprite();
    TextSprite(const std::string & msg, Font::type_t ft, const Point & pt);
    TextSprite(const std::string & msg, Font::type_t ft, u16 ax, u16 ay);

    void SetPos(const Point & pt){ SetPos(pt.x, pt.y); };
    void SetPos(u16 ax, u16 ay);
    void SetText(const std::string & msg);
    void SetFont(Font::type_t ft);

    void Show(void);
    void Hide(void);

    bool isHide(void) const;
    bool isShow(void) const;

    u16  w(void);
    u16  h(void);
    const Rect & GetRect(void) const;

private:
    Background back;
    bool hide;
};

class TextBox
{
public:
    TextBox(const std::string & msg, Font::type_t ft, const Rect & rt);
    TextBox(const std::list<std::string> & list, Font::type_t ft, const Rect & rt);
    Rect extents;
};

#endif
