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
#include "dialog.h"
#include "world.h"
#include "castle.h"
#include "settings.h"
#include "game_over.h"

const std::string & GameOver::GetString(conditions_t cond)
{
    static const std::string cond_str[] = { "None", 
	_("Defeat all enemy heroes and towns."), _("Capture a specific town."), _("Defeat a specific hero."), _("Find a specific artifact."), _("Your side defeats the opposing side."), _("Accumulate a large amount of gold."),
	_("Lose all your heroes and towns."), _("Lose a specific town."), _("Lose a specific hero."), _("Run out of time. (Fail to win by a certain point.)") };

    switch(cond)
    {
	case WINS_ALL:		return cond_str[1];
	case WINS_TOWN:		return cond_str[2];
	case WINS_HERO:		return cond_str[3];
	case WINS_ARTIFACT:	return cond_str[4];
        case WINS_SIDE:		return cond_str[5];
        case WINS_GOLD:		return cond_str[6];

	case LOSS_ALL:		return cond_str[7];
	case LOSS_TOWN:		return cond_str[8];
	case LOSS_HERO:		return cond_str[9];
	case LOSS_TIME:		return cond_str[10];

        default: break;
    }

    return cond_str[0];
}

void GameOver::DialogWins(u16 cond)
{
    const Settings & conf = Settings::Get();
    std::string body;

    switch(cond)
    {
	case WINS_ALL:
	    body = _("You win!");
	    break;

	case WINS_TOWN:
	{
	    body = _("You captured %{name}!\nYou are victorious.");
	    const Castle *town = world.GetCastle(conf.WinsMapsIndexObject());
	    if(town) String::Replace(body, "%{name}", town->GetName());
	}
	break;

	case WINS_HERO:
	{
	    body = _("You have captured the enemy hero %{name}!\nYour quest is complete.");
	    const Heroes *hero = world.GetHeroesCondWins();
	    if(hero) String::Replace(body, "%{name}", hero->GetName());
	    break;
	}

	case WINS_ARTIFACT:
	{
	    body = _("You have found the %{name}.\nYour quest is complete.");
	    const Artifact::artifact_t art = conf.WinsFindArtifact();
	    String::Replace(body, "%{name}", Artifact::String(art));
	    break;
        }

        case WINS_SIDE:
    	    body = _("The enemy is beaten.\nYour side has triumphed!");
    	    break;

        case WINS_GOLD:
        {
    	    body = _("You have built up over %{count} gold in your treasury.\nAll enemies bow before your wealth and power.");
	    String::Replace(body, "%{count}", conf.WinsAccumulateGold());
    	    break;
	}

    	default: break;
    }

    Dialog::Message("", body, Font::BIG, Dialog::OK);
}

void GameOver::DialogLoss(u16 cond)
{
    const Settings & conf = Settings::Get();
    std::string body;

    switch(cond)
    {
	case WINS_ARTIFACT:
	{
	    body = _("The enemy has found the %{name}.\nYour quest is a failure.");
	    const Artifact::artifact_t art = conf.WinsFindArtifact();
	    String::Replace(body, "%{name}", Artifact::String(art));
	    break;
        }

        case WINS_SIDE:
        {
    	    body = _("%{color} has fallen!\nAll is lost.");
    	    break;
    	}

        case WINS_GOLD:
        {
    	    body = _("The enemy has built up over %{count} gold in his treasury.\nYou must bow done in defeat before his wealth and power.");
	    String::Replace(body, "%{count}", conf.WinsAccumulateGold());
    	    break;
	}

	case LOSS_ALL:
	    body = _("You have been eliminated from the game!!!");
	    break;

	case LOSS_TOWN:
	{
	    body = _("The enemy has captured %{name}!\nThey are triumphant.");
	    const Castle *town = world.GetCastle(conf.WinsMapsIndexObject());
	    if(town) String::Replace(body, "%{name}", town->GetName());
	}

	case LOSS_HERO:
	{
	    body = _("You have lost the hero %{name}.\nYour quest is over.");
	    const Heroes *hero = world.GetHeroesCondLoss();
	    if(hero) String::Replace(body, "%{name}", hero->GetName());
	    break;
	}

	case LOSS_TIME:
	    body = _("You have failed to complete your quest in time.\nAll is lost.");
	    break;

    	default: break;
    }

    Dialog::Message("", body, Font::BIG, Dialog::OK);
}
