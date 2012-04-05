/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include <cstring>

#include "resource.h"
#include "mp2.h"
#include "difficulty.h"
#include "game_static.h"

namespace GameStatic
{
    u8		whirlpool_lost_percent	= 50;

    /* town, castle, heroes, artifact_telescope, object_observation_tower, object_magi_eyes */
    u8		overview_distance[]	= { 4, 5, 4, 1, 10, 9, 8 };

    u8		gameover_lost_days	= 7;

    // kingdom
    u8          kingdom_max_heroes	= 8;
    cost_t	kingdom_starting_resource[] = {
						{ 10000, 30, 10, 30, 10, 10, 10 },
						{ 7500, 20, 5, 20, 5, 5, 5 },
						{ 5000, 10, 2, 10, 2, 2, 2 },
						{ 2500, 5, 0, 5, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0 },
						// ai resource
						{ 10000, 30, 10, 30, 10, 10, 10 } };

    // castle
    u8          castle_grown_well	= 2;
    u8          castle_grown_wel2	= 8;
    u8          castle_grown_week_of	= 5;
    u8          castle_grown_month_of	= 100;

    u8          mageguild_restore_spell_points_day[] = {  20, 40, 60, 80, 100 };

    // heroes
    u8		heroes_spell_points_day	= 1;

    // spells
    u16		spell_dd_distance	= 0;
    u16		spell_dd_sp		= 0;
    u16		spell_dd_hp		= 0;

    // monsters
    float	monster_upgrade_ratio	= 1.0;

    // visit objects mod:	OBJ_BUOY, OBJ_OASIS, OBJ_WATERINGHOLE, OBJ_TEMPLE, OBJ_GRAVEYARD, OBJ_DERELICTSHIP,
    //			        OBJ_SHIPWRECK, OBJ_MERMAID, OBJ_FAERIERING, OBJ_FOUNTAIN, OBJ_IDOL, OBJ_PYRAMID
    s8		objects_mod[] = { 1, 1, 1, 2, -1, -1, -1, 1, 1, 1, 1, -2 };

    // world
    u32		uniq			= 0;
}

StreamBase & GameStatic::operator<< (StreamBase & msg, const Data & obj)
{
    msg <<
	whirlpool_lost_percent <<
	kingdom_max_heroes <<
	castle_grown_well <<
	castle_grown_wel2 <<
	castle_grown_week_of <<
	castle_grown_month_of <<
	heroes_spell_points_day <<
	gameover_lost_days <<
	spell_dd_distance <<
	spell_dd_sp <<
	spell_dd_hp;

    u8 array_size = ARRAY_COUNT(overview_distance);
    msg << array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg << overview_distance[ii];

    array_size = ARRAY_COUNT(kingdom_starting_resource);
    msg << array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg << kingdom_starting_resource[ii];

    array_size = ARRAY_COUNT(mageguild_restore_spell_points_day);
    msg << array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg << mageguild_restore_spell_points_day[ii];

    array_size = ARRAY_COUNT(objects_mod);
    msg << array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg << objects_mod[ii];

    msg << monster_upgrade_ratio << uniq;

    return msg;
}

StreamBase & GameStatic::operator>> (StreamBase & msg, Data & obj)
{
    msg >>
	whirlpool_lost_percent >>
	kingdom_max_heroes >>
	castle_grown_well >>
	castle_grown_wel2 >>
	castle_grown_week_of >>
	castle_grown_month_of >>
	heroes_spell_points_day >>
	gameover_lost_days >>
	spell_dd_distance >>
	spell_dd_sp >>
	spell_dd_hp;

    u8 array_size = 0;

    msg >> array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg >> overview_distance[ii];

    msg >> array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg >> kingdom_starting_resource[ii];

    msg >> array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg >> mageguild_restore_spell_points_day[ii];

    msg >> array_size;
    for(u8 ii = 0; ii < array_size; ++ii)
	msg >> objects_mod[ii];

    msg >> monster_upgrade_ratio >> uniq;

    return msg;
}

float GameStatic::GetMonsterUpgradeRatio(void)
{
    return monster_upgrade_ratio;
}

u8 GameStatic::GetLostOnWhirlpoolPercent(void)
{
    return whirlpool_lost_percent;
}

u8 GameStatic::GetOverViewDistance(u8 d)
{
    return d >= ARRAY_COUNT(overview_distance) ? 0 : overview_distance[d];
}

u8 GameStatic::GetGameOverLostDays(void)
{
    return gameover_lost_days;
}

cost_t & GameStatic::GetKingdomStartingResource(u8 df)
{
    switch(df)
    {
        case Difficulty::EASY:       return kingdom_starting_resource[0];
        case Difficulty::NORMAL:     return kingdom_starting_resource[1];
        case Difficulty::HARD:       return kingdom_starting_resource[2];
        case Difficulty::EXPERT:     return kingdom_starting_resource[3];
        case Difficulty::IMPOSSIBLE: return kingdom_starting_resource[4];
        default: break;
    }

    return kingdom_starting_resource[5];
}

u8 GameStatic::GetHeroesRestoreSpellPointsPerDay(void)
{
    return heroes_spell_points_day;
}

u8 GameStatic::GetMageGuildRestoreSpellPointsPercentDay(u8 level)
{
    return level && level < 6 ? mageguild_restore_spell_points_day[level - 1] : 0;
}

u8 GameStatic::GetKingdomMaxHeroes(void)
{
    return kingdom_max_heroes;
}

u8 GameStatic::GetCastleGrownWell(void)
{
    return castle_grown_well;
}

u8 GameStatic::GetCastleGrownWel2(void)
{
    return castle_grown_wel2;
}

u8 GameStatic::GetCastleGrownWeekOf(void)
{
    return castle_grown_week_of;
}

u8 GameStatic::GetCastleGrownMonthOf(void)
{
    return castle_grown_month_of;
}

s8 GameStatic::ObjectVisitedModifiers(u8 obj)
{
    switch(obj)
    {
	case MP2::OBJ_BUOY:		return objects_mod[0];
	case MP2::OBJ_OASIS:		return objects_mod[1];
	case MP2::OBJ_WATERINGHOLE:	return objects_mod[2];
	case MP2::OBJ_TEMPLE:		return objects_mod[3];
	case MP2::OBJ_GRAVEYARD:	return objects_mod[4];
	case MP2::OBJ_DERELICTSHIP:	return objects_mod[5];
	case MP2::OBJ_SHIPWRECK:	return objects_mod[6];
	case MP2::OBJ_MERMAID:		return objects_mod[7];
	case MP2::OBJ_FAERIERING:	return objects_mod[8];
	case MP2::OBJ_FOUNTAIN:		return objects_mod[9];
	case MP2::OBJ_IDOL:		return objects_mod[10];
	case MP2::OBJ_PYRAMID:		return objects_mod[11];
	default: break;
    }

    return 0;
}

/*
*/

#ifdef WITH_XML
#include "xmlccwrap.h"

void Game::CastleUpdateGrowth(const TiXmlElement* xml)
{
    if(xml)
    {
	int value;
	xml->Attribute("well", &value);
        GameStatic::castle_grown_well =  value > 255 ? 255 : value;

	xml->Attribute("wel2", &value);
	GameStatic::castle_grown_wel2 =  value > 255 ? 255 : value;

	xml->Attribute("week_of", &value);
	GameStatic::castle_grown_week_of =  value > 255 ? 255 : value;

	xml->Attribute("month_of", &value);
	GameStatic::castle_grown_month_of = value > 255 ? 255 : value;
    }
}

void Game::KingdomUpdateStartingResource(const TiXmlElement* xml)
{
    if(xml)
    {
	const TiXmlElement* xml_difficult;
	const char* ai_always = xml->Attribute("ai_always");
	const char* level[] = { "easy", "normal", "hard", "expert", "impossible", NULL };

	for(u8 ii = 0; ii < 5; ++ii)
	{
	    if(NULL != (xml_difficult = xml->FirstChildElement(level[ii])))
	    {
    		LoadCostFromXMLElement(GameStatic::kingdom_starting_resource[ii], *xml_difficult);
    		if(ai_always && 0 == std::strcmp(ai_always, level[ii]))
		    LoadCostFromXMLElement(GameStatic::kingdom_starting_resource[5], *xml_difficult);
	    }
	}
    }
}

void Game::KingdomUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
        int value;
        xml->Attribute("max_heroes", &value);
	GameStatic::kingdom_max_heroes = value;
    }
}

void Game::HeroesUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
        int value;
        xml->Attribute("spell_points_per_day", &value);
        if(value < 11) GameStatic::heroes_spell_points_day = value;
    }
}

void Game::GameOverUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
	int value;
        xml->Attribute("lost_towns_days", &value);
        GameStatic::gameover_lost_days = value;
    }
}

void Game::OverViewUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
        int value;
        xml->Attribute("town", &value);
        if(value) GameStatic::overview_distance[0] = value;

        xml->Attribute("castle", &value);
        if(value) GameStatic::overview_distance[1] = value;

        xml->Attribute("heroes", &value);
        if(value) GameStatic::overview_distance[2] = value;

        xml->Attribute("artifact_telescope", &value);
        if(value) GameStatic::overview_distance[3] = value;

        xml->Attribute("object_observation_tower", &value);
        if(value) GameStatic::overview_distance[4] = value;

        xml->Attribute("object_magi_eyes", &value);
        if(value) GameStatic::overview_distance[5] = value;
    }
}

void Game::WhirlpoolUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
        int value;
        xml->Attribute("percent", &value);
	GameStatic::whirlpool_lost_percent = 0 < value && value < 90 ? value : 50;
    }
}

void Game::MonsterUpdateStatic(const TiXmlElement* xml)
{
    if(xml)
    {
	double res;
	xml->Attribute("rate", &res);
	GameStatic::monster_upgrade_ratio = static_cast<float>(res);
    }
}

#endif

GameStatic::Data & GameStatic::Data::Get(void)
{
    static Data gds;
    return gds;
}
