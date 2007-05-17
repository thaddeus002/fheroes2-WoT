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
#include "castle.h"
#include "heroes.h"
#include "error.h"
#include "difficulty.h"
#include "game_statuswindow.h" 
#include "kingdom.h"

#define INCOME_CASTLE_GOLD	1000 
#define INCOME_TOWN_GOLD	750 
#define INCOME_STATUE_GOLD	250 
#define INCOME_DUNGEON_GOLD	500 


Kingdom::Kingdom(Color::color_t cl) : color(cl), build(false), play(cl & H2Config::GetKingdomColors() ? true : false)
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

void Kingdom::AITurns(const Game::StatusWindow & status)
{
    status.RedrawAITurns(color, 0);
    status.RedrawAITurns(color, 1);
    status.RedrawAITurns(color, 2);
    status.RedrawAITurns(color, 3);
    status.RedrawAITurns(color, 4);
    status.RedrawAITurns(color, 5);
    status.RedrawAITurns(color, 6);
    status.RedrawAITurns(color, 7);
    status.RedrawAITurns(color, 8);
    status.RedrawAITurns(color, 9);

    if(H2Config::Debug()) Error::Verbose("Kingdom::AITurns: " + Color::String(color) + " moved");
}

void Kingdom::ActionNewDay(void)
{
    // castle New Day
    for(u16 ii = 0; ii < castles.size(); ++ii) (*castles[ii]).ActionNewDay();

    // gold
    for(u16 ii = 0; ii < castles.size(); ++ii)
    {
	// castle or town profit
	resource.gold += ((*castles[ii]).isCastle() ? INCOME_CASTLE_GOLD : INCOME_TOWN_GOLD);

	// statue
	resource.gold += ((*castles[ii]).isBuild(Castle::BUILD_STATUE) ? INCOME_STATUE_GOLD : 0);

	// dungeon for warlock
	resource.gold += ((*castles[ii]).isBuild(Castle::BUILD_SPEC) && Race::WRLK == (*castles[ii]).GetRace() ? INCOME_STATUE_GOLD : 0);
    }
}

void Kingdom::ActionNewWeek(void)
{
    // castle New Week
    for(u16 ii = 0; ii < castles.size(); ++ii) (*castles[ii]).ActionNewWeek();
}

void Kingdom::ActionNewMonth(void)
{
    // castle New Month
    for(u16 ii = 0; ii < castles.size(); ++ii) (*castles[ii]).ActionNewMonth();
}

void Kingdom::AddHeroes(const Heroes *hero)
{
    if(hero)
    {
	std::vector<Heroes *>::const_iterator ith = heroes.begin();

	for(; ith != heroes.end(); ++ith) if(*ith == hero) return;

	heroes.push_back(const_cast<Heroes *>(hero));
    }
}

void Kingdom::RemoveHeroes(const Heroes *hero)
{
    if(hero && heroes.size())
    {
	std::vector<Heroes *>::iterator ith = heroes.begin();

	for(; ith != heroes.end(); ++ith) if(*ith == hero)
	{
	    heroes.erase(ith);
	    break;
	}
    }
}

void Kingdom::AddCastle(const Castle *castle)
{
    if(castle)
    {
	std::vector<Castle *>::const_iterator itk = castles.begin();

	for(; itk != castles.end(); ++itk) if(*itk == castle) return;

	castles.push_back(const_cast<Castle *>(castle));
    }
}

void Kingdom::RemoveCastle(const Castle *castle)
{
    if(castle && castles.size())
    {
	std::vector<Castle *>::iterator itk = castles.begin();

	for(; itk != castles.end(); ++itk) if(*itk == castle)
	{
	    castles.erase(itk);
	    break;
	}
    }
}

u8 Kingdom::GetCountCastle(void) const
{
    u8 result = 0;

    if(castles.size())
    {
	std::vector<Castle *>::const_iterator it = castles.begin();

	for(; it != castles.end(); ++it) if((**it).isCastle()) ++result;
    }

    return result;
}

u8 Kingdom::GetCountTown(void) const
{
    if(castles.size()) return castles.size() - GetCountCastle();

    return 0;
}
