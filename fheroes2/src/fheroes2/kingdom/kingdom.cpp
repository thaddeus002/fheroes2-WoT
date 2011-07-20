/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>

#include "settings.h"
#include "castle.h"
#include "heroes.h"
#include "difficulty.h"
#include "gameevent.h"
#include "profit.h"
#include "world.h"
#include "visit.h"
#include "race.h"
#include "battle2.h"
#include "kingdom.h"
#include "game_static.h"
#include "ai.h"

bool HeroesStrongestArmy(const Heroes* h1, const Heroes* h2)
{
    return h1 && h2 && h2->GetArmy().StrongerEnemyArmy(h1->GetArmy());
}

Kingdom::Kingdom() : color(Color::GRAY), control(Game::CONTROL_AI), flags(0), lost_town_days(0), visited_tents_colors(0)
{
}

void Kingdom::Init(Color::color_t cl)
{
    const Settings & conf = Settings::Get();

    clear();

    // set play
    color	= cl;
    if(conf.KingdomColors() & color) SetModes(PLAY);

    heroes.reserve(GetMaxHeroes());
    castles.reserve(15);

    // set control
    if(color & conf.PlayersColors())
    {
#ifdef WITH_NET
	if(Settings::Get().GameType(Game::TYPE_NETWORK))
    	    control = (color == conf.MyColor() ? Game::CONTROL_LOCAL : Game::CONTROL_REMOTE);
        else
#endif
	    control = Game::CONTROL_LOCAL;
    }

    UpdateStartingResource();
}

void Kingdom::clear(void)
{
    color	= Color::GRAY;
    control	= Game::CONTROL_AI;
    flags	= 0;
    visited_tents_colors = 0;
    lost_town_days = Game::GetLostTownDays() + 1;

    heroes.clear();
    castles.clear();
    visit_object.clear();

    recruits.Reset();
}

void Kingdom::UpdateStartingResource(void)
{
    resource = GameStatic::GetKingdomStartingResource(Game::CONTROL_AI == control ? 5 : Settings::Get().GameDifficulty());
}

void Kingdom::SetModes(flags_t f)
{
    flags |= f;
}

void Kingdom::ResetModes(flags_t f)
{
    flags &= ~f;
}

bool Kingdom::Modes(flags_t f) const
{
    return flags & f;
}

void Kingdom::SetControl(u8 ctrl)
{
    control = ctrl > Game::CONTROL_AI ? Game::CONTROL_NONE : ctrl;
}

bool Kingdom::isLoss(void) const
{
    return castles.empty() && heroes.empty();
}

void Kingdom::LossPostActions(void)
{
    if(Modes(PLAY))
    {
	ResetModes(PLAY);
	if(heroes.size())
	{
	    std::for_each(heroes.begin(), heroes.end(),
		std::bind2nd(std::mem_fun(&Heroes::SetFreeman), static_cast<u8>(Battle2::RESULT_LOSS)));
	    heroes.clear();
	}
	if(castles.size())
	{
	    castles.ChangeColors(color, Color::GRAY);
	    castles.clear();
	}
	world.KingdomLoss(color);
    }
}

void Kingdom::ActionBeforeTurn(void)
{
    // rescan heroes path
    std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::RescanPath));
}

void Kingdom::ActionNewDay(void)
{
    Settings::Get().SetCurrentColor(color);

    if(isLoss() || 0 == lost_town_days)
    {
	LossPostActions();
	return;
    }

    // modes
    ResetModes(IDENTIFYHERO);

    // check lost town
    if(castles.empty()) --lost_town_days;

    // skip incomes for first day
    if(1 < world.CountDay())
    {
        // castle New Day
	std::for_each(castles.begin(), castles.end(), std::mem_fun(&Castle::ActionNewDay));

	// heroes New Day
	std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::ActionNewDay));

	// captured object
	resource += ProfitConditions::FromMine(Resource::WOOD) * world.CountCapturedObject(MP2::OBJ_SAWMILL, color);
	resource += ProfitConditions::FromMine(Resource::ORE) * world.CountCapturedMines(Resource::ORE, color);
	resource += ProfitConditions::FromMine(Resource::MERCURY) * world.CountCapturedObject(MP2::OBJ_ALCHEMYLAB, color);
	resource += ProfitConditions::FromMine(Resource::SULFUR) * world.CountCapturedMines(Resource::SULFUR, color);
	resource += ProfitConditions::FromMine(Resource::CRYSTAL) * world.CountCapturedMines(Resource::CRYSTAL, color);
	resource += ProfitConditions::FromMine(Resource::GEMS) * world.CountCapturedMines(Resource::GEMS, color);
	resource += ProfitConditions::FromMine(Resource::GOLD) * world.CountCapturedMines(Resource::GOLD, color);

	// funds
	for(KingdomCastles::const_iterator
	    it = castles.begin(); it != castles.end(); ++it)
	{
	    const Castle & castle = **it;

	    // castle or town profit
	    resource += ProfitConditions::FromBuilding((castle.isCastle() ? BUILD_CASTLE : BUILD_TENT), 0);

	    // statue
	    if(castle.isBuild(BUILD_STATUE))
		resource += ProfitConditions::FromBuilding(BUILD_STATUE, 0);

	    // dungeon for warlock
	    if(castle.isBuild(BUILD_SPEC) && Race::WRLK == castle.GetRace())
		resource += ProfitConditions::FromBuilding(BUILD_SPEC, Race::WRLK);
	}
    }

    // check event day
    EventsDate events = world.GetEventsDate(color);
    for(EventsDate::const_iterator
	it = events.begin(); it != events.end(); ++it)
	AddFundsResource((*it).resource);

    // remove day visit object
    visit_object.remove_if(Visit::isDayLife);
}

void Kingdom::ActionNewWeek(void)
{
    // skip first day
    if(1 < world.CountDay())
    {
	// castle New Week
	std::for_each(castles.begin(), castles.end(), std::mem_fun(&Castle::ActionNewWeek));

	// heroes New Week
	std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::ActionNewWeek));

	// debug an gift
	if(IS_DEVEL() && Game::CONTROL_LOCAL == Control())
	{
	    Funds gift(20, 20, 10, 10, 10, 10, 5000);
	    DEBUG(DBG_GAME, DBG_INFO, "debug gift: " << gift.Dump());
	    resource += gift;
	}
    }

    // remove week visit object
    visit_object.remove_if(Visit::isWeekLife);

    UpdateRecruits();
}

void Kingdom::ActionNewMonth(void)
{
    // skip first day
    if(1 < world.CountDay())
    {
	// castle New Month
	std::for_each(castles.begin(), castles.end(), std::mem_fun(&Castle::ActionNewMonth));

	// heroes New Month
	std::for_each(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::ActionNewMonth));
    }

    // remove week visit object
    visit_object.remove_if(Visit::isMonthLife);
}

void Kingdom::AddHeroes(Heroes *hero)
{
    if(hero)
    {
	if(heroes.end() == std::find(heroes.begin(), heroes.end(), hero))
	    heroes.push_back(hero);

	AI::AddHeroes(*hero);
    }
}

void Kingdom::RemoveHeroes(const Heroes *hero)
{
    if(hero)
    {
	if(heroes.size())
	    heroes.erase(std::find(heroes.begin(), heroes.end(), hero));

	AI::RemoveHeroes(*hero);
    }

    if(isLoss()) LossPostActions();
}

void Kingdom::AddCastle(const Castle *castle)
{
    if(castle)
    {
	if(castles.end() == std::find(castles.begin(), castles.end(), castle))
	    castles.push_back(const_cast<Castle *>(castle));

	AI::AddCastle(*castle);
    }

    lost_town_days = Game::GetLostTownDays() + 1;
}

void Kingdom::RemoveCastle(const Castle *castle)
{
    if(castle)
    {
	if(castles.size())
	    castles.erase(std::find(castles.begin(), castles.end(), castle));

	AI::RemoveCastle(*castle);
    }

    if(isLoss()) LossPostActions();
}

u8 Kingdom::GetCountCastle(void) const
{
    return std::count_if(castles.begin(), castles.end(), Castle::PredicateIsCastle);
}

u8 Kingdom::GetCountTown(void) const
{
    return std::count_if(castles.begin(), castles.end(), Castle::PredicateIsTown);
}

u8 Kingdom::GetCountMarketplace(void) const
{
    return std::count_if(castles.begin(), castles.end(), Castle::PredicateIsBuildMarketplace);
}

u8 Kingdom::GetCountNecromancyShrineBuild(void) const
{
    return std::count_if(castles.begin(), castles.end(), std::mem_fun(&Castle::isNecromancyShrineBuild));
}

u8 Kingdom::GetCountBuilding(u32 build) const
{
    return std::count_if(castles.begin(), castles.end(), std::bind2nd(std::mem_fun(&Castle::isBuild), build));
}

u8 Kingdom::GetRace(void) const
{
    return Settings::Get().KingdomRace(color);
}

bool Kingdom::AllowPayment(const Funds & funds) const
{
    return funds > resource ? false : true;
}

/* is visited cell */
bool Kingdom::isVisited(const Maps::Tiles & tile) const
{
    return isVisited(tile.GetIndex(), tile.GetObject());
}

bool Kingdom::isVisited(s32 index, u8 object) const
{
    std::list<IndexObject>::const_iterator it = std::find_if(visit_object.begin(), visit_object.end(), std::bind2nd(std::mem_fun_ref(&IndexObject::isIndex), index));
    return visit_object.end() != it && (*it).isObject(object);
}

/* return true if object visited */
bool Kingdom::isVisited(const u8 object) const
{
    return visit_object.end() != std::find_if(visit_object.begin(), visit_object.end(), std::bind2nd(std::mem_fun_ref(&IndexObject::isObject), object));
}

u16 Kingdom::CountVisitedObjects(const MP2::object_t object) const
{
    return std::count_if(visit_object.begin(), visit_object.end(), std::bind2nd(std::mem_fun_ref(&IndexObject::isObject), object));
}

/* set visited cell */
void Kingdom::SetVisited(const s32 index, const MP2::object_t object)
{
    if(!isVisited(index, object) && object != MP2::OBJ_ZERO) visit_object.push_front(IndexObject(index, object));
}

bool Kingdom::HeroesMayStillMove(void) const
{
    return heroes.end() != std::find_if(heroes.begin(), heroes.end(), std::mem_fun(&Heroes::MayStillMove));
}

void Kingdom::Dump(void) const
{
    world.DateDump();
    DEBUG(DBG_GAME, DBG_INFO, "color: " << Color::String(color) << ", resource: " << resource.Dump());
}

u8 Kingdom::GetCountCapital(void) const
{
    return std::count_if(castles.begin(), castles.end(), Castle::PredicateIsCapital);
}

void Kingdom::AddFundsResource(const Funds & funds)
{
    resource = resource + funds;

    if(0 > resource.wood) resource.wood = 0;
    if(0 > resource.mercury) resource.mercury = 0;
    if(0 > resource.ore) resource.ore = 0;
    if(0 > resource.sulfur) resource.sulfur = 0;
    if(0 > resource.crystal) resource.crystal = 0;
    if(0 > resource.gems) resource.gems = 0;
    if(0 > resource.gold) resource.gold = 0;
}

void Kingdom::OddFundsResource(const Funds & funds)
{
    resource = resource - funds;

    if(0 > resource.wood) resource.wood = 0;
    if(0 > resource.mercury) resource.mercury = 0;
    if(0 > resource.ore) resource.ore = 0;
    if(0 > resource.sulfur) resource.sulfur = 0;
    if(0 > resource.crystal) resource.crystal = 0;
    if(0 > resource.gems) resource.gems = 0;
    if(0 > resource.gold) resource.gold = 0;
}

u8 Kingdom::GetLostTownDays(void) const
{
    return lost_town_days;
}

Recruits & Kingdom::GetRecruits(void)
{
    // update hero1
    if(Heroes::UNKNOWN == recruits.GetID1() || (recruits.GetHero1() && !recruits.GetHero1()->isFreeman()))
	recruits.SetHero1(world.GetFreemanHeroes(GetRace()));

    // update hero2
    if(Heroes::UNKNOWN == recruits.GetID2() || (recruits.GetHero2() && !recruits.GetHero2()->isFreeman()))
	recruits.SetHero2(world.GetFreemanHeroes());

    if(recruits.GetID1() == recruits.GetID2()) world.UpdateRecruits(recruits);

    return recruits;
}

void Kingdom::UpdateRecruits(void)
{
    recruits.SetHero1(world.GetFreemanHeroes(GetRace()));
    recruits.SetHero2(world.GetFreemanHeroes());

    if(recruits.GetID1() == recruits.GetID2()) world.UpdateRecruits(recruits);
}

const Puzzle & Kingdom::PuzzleMaps(void) const
{
    return puzzle_maps;
}

Puzzle & Kingdom::PuzzleMaps(void)
{
    return puzzle_maps;
}

void Kingdom::SetVisitTravelersTent(u8 col)
{
    visited_tents_colors |= col;
}

bool Kingdom::IsVisitTravelersTent(u8 col) const
{
    return visited_tents_colors & col;
}

bool Kingdom::AllowRecruitHero(bool check_payment, u8 level) const
{
    return (heroes.size() < GetMaxHeroes()) && (!check_payment || AllowPayment(PaymentConditions::RecruitHero(level)));
}

void Kingdom::ApplyPlayWithStartingHero(void)
{
    if(isPlay() && castles.size())
    {
	// get first castle
	Castle* first = castles.GetFirstCastle();
	if(NULL == first) first = castles.front();

	// check manual set hero (castle position + point(0, 1))?
	const Point & cp = (first)->GetCenter();
	if(world.GetTiles(cp.x, cp.y + 1).GetObject() == MP2::OBJ_HEROES)
	{
    	    Heroes *hero = world.GetHeroes((cp.y + 1) * world.w() + cp.x);
    	    // and move manual set hero to castle
    	    if(hero && hero->GetColor() == GetColor())
    	    {
		bool patrol = hero->Modes(Heroes::PATROL);
    		hero->SetFreeman(0);
    		hero->Recruit(*first);

		if(patrol)
		{
		    hero->SetModes(Heroes::PATROL);
		    hero->SetCenterPatrol(cp);
		}
    	    }
	}
	else
	if(Settings::Get().GameStartWithHeroes())
	{
    	    Heroes *hero = world.GetFreemanHeroes(first->GetRace());
	    if(hero && AllowRecruitHero(false, 0)) hero->Recruit(*first);
	}
    }
}

u8 Kingdom::GetMaxHeroes(void)
{
    return GameStatic::GetKingdomMaxHeroes();
}

void Kingdom::HeroesActionNewPosition(void)
{
    // Heroes::ActionNewPosition: can remove elements from heroes vector.
    KingdomHeroes heroes2(heroes);
    std::for_each(heroes2.begin(), heroes2.end(), std::mem_fun(&Heroes::ActionNewPosition));
}

u32 Kingdom::GetIncome(void)
{
    Funds resource;

    // captured object
    //resource += ProfitConditions::FromMine(Resource::WOOD) * world.CountCapturedObject(MP2::OBJ_SAWMILL, color);
    //resource += ProfitConditions::FromMine(Resource::ORE) * world.CountCapturedMines(Resource::ORE, color);
    //resource += ProfitConditions::FromMine(Resource::MERCURY) * world.CountCapturedObject(MP2::OBJ_ALCHEMYLAB, color);
    //resource += ProfitConditions::FromMine(Resource::SULFUR) * world.CountCapturedMines(Resource::SULFUR, color);
    //resource += ProfitConditions::FromMine(Resource::CRYSTAL) * world.CountCapturedMines(Resource::CRYSTAL, color);
    //resource += ProfitConditions::FromMine(Resource::GEMS) * world.CountCapturedMines(Resource::GEMS, color);
    resource += ProfitConditions::FromMine(Resource::GOLD) * world.CountCapturedMines(Resource::GOLD, color);

    for(KingdomCastles::const_iterator
	it = castles.begin(); it != castles.end(); ++it)
    {
        const Castle & castle = **it;

        // castle or town profit
        resource += ProfitConditions::FromBuilding((castle.isCastle() ? BUILD_CASTLE : BUILD_TENT), 0);

        // statue
        if(castle.isBuild(BUILD_STATUE))
                resource += ProfitConditions::FromBuilding(BUILD_STATUE, 0);

        // dungeon for warlock
	if(castle.isBuild(BUILD_SPEC) && Race::WRLK == castle.GetRace())
                resource += ProfitConditions::FromBuilding(BUILD_SPEC, Race::WRLK);
    }

    for(KingdomHeroes::const_iterator
	ith = heroes.begin(); ith != heroes.end(); ++ith)
    {
	if((**ith).HasArtifact(Artifact::GOLDEN_GOOSE))           resource += ProfitConditions::FromArtifact(Artifact::GOLDEN_GOOSE);
        if((**ith).HasArtifact(Artifact::ENDLESS_SACK_GOLD))      resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_SACK_GOLD);
	if((**ith).HasArtifact(Artifact::ENDLESS_BAG_GOLD))       resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_BAG_GOLD);
        if((**ith).HasArtifact(Artifact::ENDLESS_PURSE_GOLD))     resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_PURSE_GOLD);
	//if((**ith).HasArtifact(Artifact::ENDLESS_POUCH_SULFUR))   resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_POUCH_SULFUR);
	//if((**ith).HasArtifact(Artifact::ENDLESS_VIAL_MERCURY))   resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_VIAL_MERCURY);
	//if((**ith).HasArtifact(Artifact::ENDLESS_POUCH_GEMS))     resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_POUCH_GEMS);
	//if((**ith).HasArtifact(Artifact::ENDLESS_CORD_WOOD))      resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_CORD_WOOD);
	//if((**ith).HasArtifact(Artifact::ENDLESS_CART_ORE))       resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_CART_ORE);
	//if((**ith).HasArtifact(Artifact::ENDLESS_POUCH_CRYSTAL))  resource += ProfitConditions::FromArtifact(Artifact::ENDLESS_POUCH_CRYSTAL);

	// estates skill bonus
	resource.gold += (**ith).GetSecondaryValues(Skill::Secondary::ESTATES);

	if((**ith).HasArtifact(Artifact::TAX_LIEN)) resource.gold -= 250;
    }

    return resource.gold;
}

const Heroes* Kingdom::GetBestHero(void) const
{
    return heroes.size() ? *std::max_element(heroes.begin(), heroes.end(), HeroesStrongestArmy) : NULL;
}

u32 Kingdom::GetArmiesStrength(void) const
{
    double res = 0;

    for(KingdomHeroes::const_iterator
	ith = heroes.begin(); ith != heroes.end(); ++ith)
	res += (**ith).GetArmy().GetStrength();

    for(KingdomCastles::const_iterator
	itc = castles.begin(); itc != castles.end(); ++itc)
	res += (**itc).GetArmy().GetStrength();

    return static_cast<u32>(res);
}

Kingdoms::Kingdoms()
{
    Init();
}

void Kingdoms::Init(void)
{
    kingdoms[0].Init(Color::BLUE);
    kingdoms[1].Init(Color::GREEN);
    kingdoms[2].Init(Color::RED);
    kingdoms[3].Init(Color::YELLOW);
    kingdoms[4].Init(Color::ORANGE);
    kingdoms[5].Init(Color::PURPLE);
    kingdoms[6].Init(Color::GRAY);
}

u8 Kingdoms::size(void) const
{
    return KINGDOMMAX + 1;
}

void Kingdoms::clear(void)
{
    for(u8 ii = 0; ii < size(); ++ii)
	kingdoms[ii].clear();
}

void Kingdoms::ApplyPlayWithStartingHero(void)
{
    for(u8 ii = 0; ii < size(); ++ii)
	kingdoms[ii].ApplyPlayWithStartingHero();
}

const Kingdom & Kingdoms::Get(u8 color) const
{
    switch(color)
    {
        case Color::BLUE:       return kingdoms[0];
        case Color::GREEN:      return kingdoms[1];
        case Color::RED:        return kingdoms[2];
        case Color::YELLOW:     return kingdoms[3];
        case Color::ORANGE:     return kingdoms[4];
        case Color::PURPLE:     return kingdoms[5];
	default: break;
    }

    return kingdoms[6];
}

Kingdom & Kingdoms::Get(u8 color)
{
    switch(color)
    {
        case Color::BLUE:       return kingdoms[0];
        case Color::GREEN:      return kingdoms[1];
        case Color::RED:        return kingdoms[2];
        case Color::YELLOW:     return kingdoms[3];
        case Color::ORANGE:     return kingdoms[4];
        case Color::PURPLE:     return kingdoms[5];
	default: break;
    }

    return kingdoms[6];
}

void Kingdom::SetLastLostHero(Heroes & hero)
{
    lost_hero.first = &hero;
    lost_hero.second = world.CountDay();
}

Heroes* Kingdom::GetLastLostHero(void) const
{
    return lost_hero.first && world.CountDay() - lost_hero.second < DAYOFWEEK ? lost_hero.first : NULL;
}

void Kingdoms::NewDay(void)
{
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].isPlay()) kingdoms[ii].ActionNewDay();
}

void Kingdoms::NewWeek(void)
{
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].isPlay()) kingdoms[ii].ActionNewWeek();
}

void Kingdoms::NewMonth(void)
{
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].isPlay()) kingdoms[ii].ActionNewMonth();
}

u8 Kingdoms::GetNotLossColors(void) const
{
    u8 result = 0;
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].GetColor() != Color::GRAY &&
	    ! kingdoms[ii].isLoss()) result |= kingdoms[ii].GetColor();
    return result;
}

u8 Kingdoms::GetLossColors(void) const
{
    u8 result = 0;
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].GetColor() != Color::GRAY &&
	    kingdoms[ii].isLoss()) result |= kingdoms[ii].GetColor();
    return result;
}

u8 Kingdoms::FindArtOrGoldWins(void) const
{
    for(u8 ii = 0; ii < size(); ++ii)
	if(kingdoms[ii].GetColor() != Color::GRAY &&
	    (world.KingdomIsWins(kingdoms[ii], GameOver::WINS_GOLD) || world.KingdomIsWins(kingdoms[ii], GameOver::WINS_ARTIFACT)))
		return kingdoms[ii].GetColor();
    return 0;
}

void Kingdoms::AddHeroes(const AllHeroes & heroes)
{
    for(AllHeroes::const_iterator                                                                    
        it = heroes.begin(); it != heroes.end(); ++it)
        Get((*it)->GetColor()).AddHeroes(*it);
}

void Kingdoms::AddCastles(const AllCastles & castles)
{
    for(AllCastles::const_iterator
	it = castles.begin(); it != castles.end(); ++it)
        Get((*it)->GetColor()).AddCastle(*it);
}
