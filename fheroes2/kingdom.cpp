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

#include "config.h"
#include "difficulty.h"
#include "kingdom.h"

Kingdom::Kingdom(bool playing) : build(false), play(playing)
{
    // set starting resource
    switch(H2Config::GetGameDifficulty()){
	case Difficulty::EASY:
	    resource.wood	= 30;
	    resource.mercury	= 10;
	    resource.ore	= 10;
	    resource.sulfur	= 10;
	    resource.crystal	= 10;
	    resource.gems	= 10;
	    resource.gold	= 10000;
	    break;
	case Difficulty::NORMAL:
	    resource.wood	= 20;
	    resource.mercury	= 5;
	    resource.ore	= 5;
	    resource.sulfur	= 5;
	    resource.crystal	= 5;
	    resource.gems	= 5;
	    resource.gold	= 7500;
	    break;
	case Difficulty::HARD:
	    resource.wood	= 10;
	    resource.mercury	= 2;
	    resource.ore	= 2;
	    resource.sulfur	= 2;
	    resource.crystal	= 2;
	    resource.gems	= 2;
	    resource.gold	= 5000;
	    break;
	case Difficulty::EXPERT:
	    resource.wood	= 5;
	    resource.mercury	= 0;
	    resource.ore	= 0;
	    resource.sulfur	= 0;
	    resource.crystal	= 0;
	    resource.gems	= 0;
	    resource.gold	= 2500;
	    break;
	case Difficulty::IMPOSSIBLE:
	    resource.wood	= 0;
	    resource.mercury	= 0;
	    resource.ore	= 0;
	    resource.sulfur	= 0;
	    resource.crystal	= 0;
	    resource.gems	= 0;
	    resource.gold	= 0;
	    break;
    }
}
