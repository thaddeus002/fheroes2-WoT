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
#include "payment.h"
#include "kingdom.h"

Kingdom::Kingdom(Color::color_t cl) : color(cl), play(cl & H2Config::GetKingdomColors() ? true : false)
{
    // set starting resource
    switch(H2Config::GetGameDifficulty()){
	case Difficulty::EASY:
	    resource.wood	= START_EASY_WOOD;
	    resource.ore	= START_EASY_ORE;
	    resource.mercury	= START_EASY_MERCURY;
	    resource.sulfur	= START_EASY_SULFUR;
	    resource.crystal	= START_EASY_CRYSTAL;
	    resource.gems	= START_EASY_GEMS;
	    resource.gold	= START_EASY_GOLD;
	    break;
	case Difficulty::NORMAL:
	    resource.wood	= START_NORMAL_WOOD;
	    resource.ore	= START_NORMAL_ORE;
	    resource.mercury	= START_NORMAL_MERCURY;
	    resource.sulfur	= START_NORMAL_SULFUR;
	    resource.crystal	= START_NORMAL_CRYSTAL;
	    resource.gems	= START_NORMAL_GEMS;
	    resource.gold	= START_NORMAL_GOLD;
	    break;
	case Difficulty::HARD:
	    resource.wood	= START_HARD_WOOD;
	    resource.ore	= START_HARD_ORE;
	    resource.mercury	= START_HARD_MERCURY;
	    resource.sulfur	= START_HARD_SULFUR;
	    resource.crystal	= START_HARD_CRYSTAL;
	    resource.gems	= START_HARD_GEMS;
	    resource.gold	= START_HARD_GOLD;
	    break;
	case Difficulty::EXPERT:
	    resource.wood	= START_EXPERT_WOOD;
	    resource.ore	= START_EXPERT_ORE;
	    resource.mercury	= START_EXPERT_MERCURY;
	    resource.sulfur	= START_EXPERT_SULFUR;
	    resource.crystal	= START_EXPERT_CRYSTAL;
	    resource.gems	= START_EXPERT_GEMS;
	    resource.gold	= START_EXPERT_GOLD;
	    break;
	case Difficulty::IMPOSSIBLE:
	    resource.wood	= START_IMPOSSIBLE_WOOD;
	    resource.ore	= START_IMPOSSIBLE_ORE;
	    resource.mercury	= START_IMPOSSIBLE_MERCURY;
	    resource.sulfur	= START_IMPOSSIBLE_SULFUR;
	    resource.crystal	= START_IMPOSSIBLE_CRYSTAL;
	    resource.gems	= START_IMPOSSIBLE_GEMS;
	    resource.gold	= START_IMPOSSIBLE_GOLD;
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

    // heroes New Day
    for(u16 ii = 0; ii < heroes.size(); ++ii) (*heroes[ii]).ActionNewDay();

    // funds
    for(u16 ii = 0; ii < castles.size(); ++ii)
    {
	// castle or town profit
	resource.gold += ((*castles[ii]).isCastle() ? INCOME_CASTLE_GOLD : INCOME_TOWN_GOLD);

	// statue
	resource.gold += ((*castles[ii]).isBuild(Castle::BUILD_STATUE) ? INCOME_STATUE_GOLD : 0);

	// dungeon for warlock
	resource.gold += ((*castles[ii]).isBuild(Castle::BUILD_SPEC) && Race::WRLK == (*castles[ii]).GetRace() ? INCOME_DUNGEON_GOLD : 0);
    }

    for(u8 ii = 0; ii < heroes.size(); ++ii)
    {
	const Heroes * hero = heroes[ii];

	if((*hero).GetArtifacts().size())
	{
	    std::vector<Artifact::artifact_t>::const_iterator it1 = (*hero).GetArtifacts().begin();
	    std::vector<Artifact::artifact_t>::const_iterator it2 = (*hero).GetArtifacts().end();
	    
	    for(; it1 != it2; ++it1) switch(*it1)
	    {
		case Artifact::ENDLESS_SACK_GOLD:	resource.gold += INCOME_ENDLESS_SACK_GOLD; break;
		case Artifact::ENDLESS_BAG_GOLD:	resource.gold += INCOME_ENDLESS_BAG_GOLD; break;
		case Artifact::ENDLESS_PURSE_GOLD:	resource.gold += INCOME_ENDLESS_PURSE_GOLD; break;
		case Artifact::ENDLESS_POUCH_SULFUR:	resource.sulfur += INCOME_ENDLESS_POUCH_SULFUR; break;
		case Artifact::ENDLESS_VIAL_MERCURY:	resource.mercury += INCOME_ENDLESS_VIAL_MERCURY; break;
		case Artifact::ENDLESS_POUCH_GEMS:	resource.gems += INCOME_ENDLESS_POUCH_GEMS; break;
		case Artifact::ENDLESS_CORD_WOOD:	resource.wood += INCOME_ENDLESS_CORD_WOOD; break;
		case Artifact::ENDLESS_CART_ORE:	resource.ore += INCOME_ENDLESS_CART_ORE; break;
		case Artifact::ENDLESS_POUCH_CRYSTAL:	resource.crystal += INCOME_ENDLESS_POUCH_CRYSTAL; break;

		default: break;
	    }
	}
    }
}

void Kingdom::ActionNewWeek(void)
{
    // castle New Week
    for(u16 ii = 0; ii < castles.size(); ++ii) (*castles[ii]).ActionNewWeek();

    // heroes New Week
    for(u16 ii = 0; ii < heroes.size(); ++ii) (*heroes[ii]).ActionNewWeek();

    // debug an gift
    if(H2Config::Debug() && color == H2Config::GetMyColor())
    {
	Error::Verbose("Kingdom::ActionNewWeek: for the best debugging, God has sent you a gift.");

	resource.wood += 20;
	resource.ore += 20;
	resource.sulfur += 10;
	resource.crystal += 10;
	resource.gems += 10;
	resource.mercury += 10;
	resource.gold += 5000;
    }
}

void Kingdom::ActionNewMonth(void)
{
    // castle New Month
    for(u16 ii = 0; ii < castles.size(); ++ii) (*castles[ii]).ActionNewMonth();

    // heroes New Month
    for(u16 ii = 0; ii < heroes.size(); ++ii) (*heroes[ii]).ActionNewMonth();
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

u8 Kingdom::GetCountMarketplace(void) const
{
    u8 result = 0;

    if(castles.size())
    {
	std::vector<Castle *>::const_iterator it = castles.begin();

	for(; it != castles.end(); ++it) if((**it).isBuild(Castle::BUILD_MARKETPLACE)) ++result;
    }

    return result;
}
