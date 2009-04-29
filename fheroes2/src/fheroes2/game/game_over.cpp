/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#include "gamedefs.h"
#include "game_over.h"

const std::string & GameOverConditions::Wins(wins_t wins)
{
    static const std::string wins_str[] = { _("Defeat all enemy heroes and towns."), _("Capture a specific town."), _("Defeat a specific hero."), _("Find a specific artifact."), _("Your side defeats the opposing side."), _("Accumulate a large amount of gold.") };

    switch(wins)
    {
	case WINS_TOWN:		return wins_str[1];
	case WINS_HERO:		return wins_str[2];
	case WINS_ARTIFACT:	return wins_str[3];
        case WINS_SIDE:		return wins_str[4];
        case WINS_GOLD:		return wins_str[5];
        default: break;
    }

    return wins_str[0];
}

const std::string & GameOverConditions::Loss(loss_t loss)
{
    static const std::string loss_str[] = { _("Lose all your heroes and towns."), _("Lose a specific town."), _("Lose a specific hero."), _("Run out of time. (Fail to win by a certain point.)") };

    switch(loss)
    {
	case LOSS_TOWN:	return loss_str[1];
	case LOSS_HERO:	return loss_str[2];
	case LOSS_TIME:	return loss_str[3];
        default: break;
    }

    return loss_str[0];
}
