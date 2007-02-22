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
#ifndef H2COLOR_H
#define H2COLOR_H

#include <string>

namespace Color
{
    typedef enum {
        BLUE    = 0x01,
        GREEN   = 0x02,
        RED     = 0x04,
        YELLOW  = 0x08,
        ORANGE  = 0x10,
        PURPLE  = 0x20,
        GRAY    = 0x40,
    } color_t;

    inline color_t& operator++ (color_t& color){ return color = ( GRAY == color ? BLUE : color_t(color << 1)); };
    inline color_t& operator-- (color_t& color){ return color = ( BLUE == color ? GRAY : color_t(color >> 1)); };

    const std::string & String(Color::color_t color);
};

#endif
