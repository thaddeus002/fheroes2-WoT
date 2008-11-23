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

#include <stdexcept>
#include <iostream>
#include <sstream>
#include "types.h"
#include "error.h"

/* exception */
void Error::Except(const std::string & message)
{
    std::cerr << "error: " << message << std::endl;

    throw Exception();
}

void Error::Warning(const std::string & message)
{
    std::cout << "warning: " << message << std::endl;
}

void Error::Warning(const std::string & message, int value)
{
    std::string str;

    std::ostringstream stream;
    stream << value;
    str += stream.str();

    std::cout << "warning: " << message << str << std::endl;
}

void Error::Verbose(const std::string & message)
{
    std::cout << message << std::endl;
}

void Error::Verbose(const std::string & message, int value)
{
    std::string str;

    std::ostringstream stream;
    stream << value;
    str += stream.str();

    std::cout << message << str << std::endl;
}

const std::string & Error::SDLError(void)
{
    static std::string sdl_error;
    
    sdl_error = std::string(SDL_GetError());
    
    return sdl_error;
}
