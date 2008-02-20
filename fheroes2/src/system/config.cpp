/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "config.h"

u8 H2Config::Debug(void) { return Settings::Get().Debug(); }

bool H2Config::Shadow(void) { return Settings::Get().Shadow(); }
bool H2Config::EvilInterface(void) { return Settings::Get().EvilInterface(); }

Display::resolution_t H2Config::VideoMode(void)
{
    const Size & size = Settings::Get().VideoMode();
    
    if(640 == size.w && 480 == size.h)	return Display::SMALL;
    else
    if(800 == size.w && 600 == size.h)	return Display::MEDIUM;
    else
    if(1024 == size.w && 768 == size.h)	return Display::LARGE;
    else
    if(1280 == size.w && 1024 == size.h)return Display::XLARGE;

    return Display::CUSTOM;
}

Color::color_t H2Config::MyColor(void) { return Settings::Get().MyColor(); }

