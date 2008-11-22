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
#include "rect.h"
#include "display.h"
#include "background.h"

namespace Font
{
    typedef enum { SMALL, BIG } type_t;
};

namespace Char
{
    u8 width(char ch, Font::type_t ft);
};

class Text
{
public:
    typedef enum { LEFT, CENTER, RIGHT } align_t;

    Text(){};
    Text(const std::string & msg, Font::type_t ft);

    // with blit
    Text(const std::string & msg, Font::type_t ft, const Point & dst_pt);
    Text(const std::string & msg, Font::type_t ft, const Rect & dst_rt);
    Text(const std::string & msg, Font::type_t ft, u16 ax, u16 ay);

    void SetText(const std::string & msg){ message = msg; };
    void SetFont(const Font::type_t & ft){ font = ft; };

    u16 width(u16 start = 0, u16 count = 0xffff){ return Text::width(message, font, start, count); };
    u16 height(u16 width = 0){ return Text::height(message, font, width); };

    static u16 width(const std::string &str, Font::type_t ft, u16 start = 0, u16 count = 0xffff);
    static u16 height(const std::string &str, Font::type_t ft, u16 width = 0);

    void Blit(u16 ax, u16 ay, Surface & sf = Display::Get());
    void Blit(const Point & dst_pt, Surface & sf = Display::Get());

private:
    Font::type_t font;
    std::string message;
};

class TextSprite : public Text
{
public:
    TextSprite();
    TextSprite(const std::string & msg, Font::type_t ft, const Point & pt);
    TextSprite(const std::string & msg, Font::type_t ft, u16 ax, u16 ay);

    void SetPos(const Point & pt){ SetPos(pt.x, pt.y); };
    void SetPos(u16 ax, u16 ay);

    void Show(void);
    void Hide(void);

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
