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

#include "error.h"

/* exception */
void Error::Except(const std::string & message)
{
    std::cerr << "error: " << message << std::endl;

    throw Exception();
}

void Error::Warning(const std::string & message)
{
    if(H2Config::Debug()) std::cout << "warning: " << message << std::endl;
}

void Error::Verbose(const std::string & message)
{
    if(H2Config::Debug()) std::cout << message << std::endl;
}

void Error::Verbose(const std::string & message, int value)
{
    std::string str;
    String::AddInt(str, value);

    if(H2Config::Debug()) std::cout << message << str << std::endl;
}

void Error::Verbose(const Rect &rt)
{
    Verbose("x: ", rt.x);
    Verbose("y: ", rt.y);
    Verbose("w: ", rt.w);
    Verbose("h: ", rt.h);
}

void Error::Verbose(const Point &pt)
{
    Verbose("x: ", pt.x);
    Verbose("y: ", pt.y);
}
