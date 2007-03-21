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
#ifndef H2CASTLE_H
#define H2CASTLE_H

#include <string>
#include "gamedefs.h"
#include "color.h"
#include "race.h"
#include "army.h"

#define CASTLEMAXARMY           5 
#define CASTLEMAXMONSTER        6 

class Castle
{
public:
    Castle(u16 index, const void *ptr, bool rnd = false);

private:
    Color::color_t	color;
    Race::race_t	race;
    std::string		name;
    u16			index_map;
    u32			building;
    bool		allow_castle;
    bool		army_spread;
    bool		boat;
    //MageGuild		guild;
    u16			dwelling[CASTLEMAXMONSTER];
    Army::army_t	army[CASTLEMAXARMY];
};

#endif
