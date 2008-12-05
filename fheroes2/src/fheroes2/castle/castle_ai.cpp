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

#include "settings.h"
#include "world.h"
#include "maps.h"
#include "kingdom.h"
#include "castle.h"

// task. role. 
bool Castle::AITurns(void)
{
    const s8 range = isCastle() ? CASTLE_SCOUTE : TOWN_SCOUTE;

    // find enemy hero
    for(s8 y = -range; y <= range; ++y)
        for(s8 x = -range; x <= range; ++x)
    {
        if(!y && !x) continue;

        if(Maps::isValidAbsPoint(mp.x + x, mp.y + y))
	{
            const Maps::Tiles & tile = world.GetTiles(Maps::GetIndexFromAbsPoint(mp.x + x, mp.y + y));
	    const Heroes* hero = NULL;

	    if(MP2::OBJ_HEROES == tile.GetObject() && NULL != (hero = world.GetHeroes(tile.GetIndex())) && color != hero->GetColor())
		return AITaskDefence();
	}
    }

    return AITaskDevelopment();
}

bool Castle::AITaskDefence(void)
{
    if(isCastle())
    {
	if(!isBuild(BUILD_LEFTTURRET))				BuyBuilding(BUILD_LEFTTURRET);
	if(!isBuild(BUILD_RIGHTTURRET))				BuyBuilding(BUILD_RIGHTTURRET);
	if(!isBuild(BUILD_MOAT))				BuyBuilding(BUILD_MOAT);
	if(!isBuild(BUILD_CAPTAIN) && NULL == GetHeroes())	BuyBuilding(BUILD_CAPTAIN);
	if(!isBuild(BUILD_TAVERN) && Race::KNGT == race)	BuyBuilding(BUILD_TAVERN);
	if(!isBuild(BUILD_SPEC))				BuyBuilding(BUILD_SPEC);
    }

    if(isBuild(DWELLING_MONSTER6)) RecruitMonster(DWELLING_MONSTER6, MAXU16);
    if(isBuild(DWELLING_MONSTER5)) RecruitMonster(DWELLING_MONSTER5, MAXU16);
    if(isBuild(DWELLING_MONSTER4)) RecruitMonster(DWELLING_MONSTER4, MAXU16);
    if(isBuild(DWELLING_MONSTER3)) RecruitMonster(DWELLING_MONSTER3, MAXU16);
    if(isBuild(DWELLING_MONSTER2)) RecruitMonster(DWELLING_MONSTER2, MAXU16);
    if(isBuild(DWELLING_MONSTER1)) RecruitMonster(DWELLING_MONSTER1, MAXU16);

    return true;
}

bool Castle::AITaskDevelopment(void)
{
    if(isCastle() && Modes(CAPITAL))
    {
	if(!isBuild(BUILD_STATUE))				BuyBuilding(BUILD_STATUE);
	if(!isBuild(BUILD_SPEC) && Race::WRLK == race)		BuyBuilding(BUILD_SPEC);
	if(!isBuild(BUILD_TAVERN) && Race::KNGT == race)	BuyBuilding(BUILD_TAVERN);
	if(!isBuild(BUILD_MAGEGUILD1) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & race)) BuyBuilding(BUILD_MAGEGUILD1);
	if(!isBuild(BUILD_WELL))				BuyBuilding(BUILD_WELL);

	if(!isBuild(DWELLING_MONSTER1))				BuyBuilding(DWELLING_MONSTER1);
	if(!isBuild(DWELLING_MONSTER2))				BuyBuilding(DWELLING_MONSTER2);
	if(!isBuild(DWELLING_MONSTER3))				BuyBuilding(DWELLING_MONSTER3);
	if(!isBuild(DWELLING_MONSTER4))				BuyBuilding(DWELLING_MONSTER4);

	if(!isBuild(BUILD_MARKETPLACE))				BuyBuilding(BUILD_MARKETPLACE);

	if(!isBuild(BUILD_MAGEGUILD1))				BuyBuilding(BUILD_MAGEGUILD1);
	if(!isBuild(BUILD_MAGEGUILD2) && ((Race::SORC | Race::WZRD | Race::WRLK | Race::NECR) & race)) BuyBuilding(BUILD_MAGEGUILD2);

	if(!isBuild(DWELLING_UPGRADE2))				BuyBuilding(DWELLING_UPGRADE2);
	if(!isBuild(DWELLING_UPGRADE3))				BuyBuilding(DWELLING_UPGRADE3);
	if(!isBuild(DWELLING_UPGRADE4))				BuyBuilding(DWELLING_UPGRADE4);

	if(!isBuild(BUILD_LEFTTURRET))				BuyBuilding(BUILD_LEFTTURRET);
	if(!isBuild(BUILD_RIGHTTURRET))				BuyBuilding(BUILD_RIGHTTURRET);
	if(!isBuild(BUILD_MOAT))				BuyBuilding(BUILD_MOAT);
	if(!isBuild(BUILD_CAPTAIN))				BuyBuilding(BUILD_CAPTAIN);

	if(!isBuild(BUILD_MAGEGUILD2))				BuyBuilding(BUILD_MAGEGUILD2);

	if(!isBuild(DWELLING_MONSTER5))				BuyBuilding(DWELLING_MONSTER5);
	if(!isBuild(DWELLING_MONSTER6))				BuyBuilding(DWELLING_MONSTER6);

	if(!isBuild(BUILD_MAGEGUILD3))				BuyBuilding(BUILD_MAGEGUILD3);

	if(!isBuild(DWELLING_UPGRADE5))				BuyBuilding(DWELLING_UPGRADE5);
	if(!isBuild(DWELLING_UPGRADE6))				BuyBuilding(DWELLING_UPGRADE6);
    }
    else
    // build castle only monday or tuesday or for capital
    if(3 > world.GetDay() || Modes(CAPITAL)) BuyBuilding(BUILD_CASTLE);

    // last day and buy monster
    if(world.LastDay())
    {
	if(isBuild(DWELLING_MONSTER6)) RecruitMonster(DWELLING_MONSTER6, MAXU16);
	if(isBuild(DWELLING_MONSTER5)) RecruitMonster(DWELLING_MONSTER5, MAXU16);
	if(isBuild(DWELLING_MONSTER4)) RecruitMonster(DWELLING_MONSTER4, MAXU16);
	if(isBuild(DWELLING_MONSTER3)) RecruitMonster(DWELLING_MONSTER3, MAXU16);
	if(isBuild(DWELLING_MONSTER2)) RecruitMonster(DWELLING_MONSTER2, MAXU16);
	if(isBuild(DWELLING_MONSTER1)) RecruitMonster(DWELLING_MONSTER1, MAXU16);
    }

    return true;
}
