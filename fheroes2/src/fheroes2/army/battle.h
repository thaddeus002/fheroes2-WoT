/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov                                  *
 *   drmoriarty@rambler.ru                                                 *
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

#include <vector>
#include "heroes.h"
#include "castle.h"
#include "maps_tiles.h"

#ifndef BATTLE_H
#define BATTLE_H

namespace Army {
    
    typedef enum { WIN, LOSE, RETREAT, SURRENDER, AUTO } battle_t;

    battle_t Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile);
    battle_t Battle(Heroes& hero, Army::army_t & army, const Maps::Tiles & tile);
    battle_t Battle(Heroes& hero, Castle& castle, const Maps::Tiles &tile);
	
};

#endif
