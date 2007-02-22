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

class Text
{
public:
    typedef enum { LEFT, CENTER, RIGHT } align_t;

    Text(u16 ax, u16 ay, const std::string &msg, Font::type_t ft = Font::SMALL, bool draw = false);
    Text(u16 ax, u16 ay, u32 textcount, Font::type_t ft = Font::SMALL, bool draw = false);
    
    u16 w(void) const{ return pos.w; }

    static u8  width(char ch, Font::type_t ft);
    static u16 width(const std::string &str, u16 start, u16 count, Font::type_t ft);
    static u16 height(u16 width, const std::string &str, Font::type_t ft);

    void Blit(void);

private:
    Font::type_t font;
    std::string message;
    Rect pos;
};

class TextBox
{
public:
    TextBox(const Rect &rt, const std::string &msg, Font::type_t ft = Font::SMALL, bool draw = false);

    void Redraw(void);

private:
    Rect pos;
    std::vector<Text> box;
};

#endif
