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
#ifndef H2KINGDOM_H
#define H2KINGDOM_H

#include <vector>
#include <map>
#include "color.h"
#include "payment.h"
#include "puzzle.h"
#include "game.h"
#include "mp2.h"
#include "pairs.h"
#include "game_io.h"
#include "heroes.h"
#include "castle.h"
#include "heroes_recruits.h"

class Player;
class Castle;
class Heroes;
struct AllHeroes;
struct VecHeroes;
struct AllCastles;
struct VecCastles;

struct LastLoseHero : std::pair<Heroes*, u16> /* Heroes, date */
{
    LastLoseHero() : std::pair<Heroes*, u16>(NULL, 0) {}
};

struct KingdomCastles : public VecCastles
{
};

struct KingdomHeroes : public VecHeroes
{
};

class Kingdom : public BitModes
{
public:
    enum
    {
	PLAY	     = 0x0001,
	IDENTIFYHERO = 0x0002,
	DISABLEHIRES = 0x0004
    };

    Kingdom();

    void Init(u8 color);
    void clear(void);

    void UpdateStartingResource(void);
    bool isPlay(void) const{ return Modes(PLAY); }
    bool isLoss(void) const;
    bool AllowPayment(const Funds & funds) const;
    bool AllowRecruitHero(bool check_payment, u8 level) const;

    void SetLastLostHero(Heroes &);
    void ResetLastLostHero(void);
    Heroes* GetLastLostHero(void) const;

    void  AddHeroStartCondLoss(Heroes*);
    std::string GetNamesHeroStartCondLoss(void) const;
    const Heroes* GetFirstHeroStartCondLoss(void) const;

    u8 GetControl(void) const;
    Color::color_t GetColor(void) const;
    u8 GetRace(void) const;

    const Funds & GetFunds(void) const{ return resource; }
    u16 GetFundsWood(void) const{ return resource.wood; }
    u16 GetFundsMercury(void) const{ return resource.mercury; }
    u16 GetFundsOre(void) const{ return resource.ore; }
    u16 GetFundsSulfur(void) const{ return resource.sulfur; }
    u16 GetFundsCrystal(void) const{ return resource.crystal; }
    u16 GetFundsGems(void) const{ return resource.gems; }
    u32 GetFundsGold(void) const{ return resource.gold; }

    u32 GetIncome(void);
    const Heroes* GetBestHero(void) const;
    u32 GetArmiesStrength(void) const;

    void AddFundsResource(const Funds & funds);
    void OddFundsResource(const Funds & funds);

    u8 GetCountCastle(void) const;
    u8 GetCountTown(void) const;
    u8 GetCountMarketplace(void) const;
    u8 GetCountCapital(void) const;
    u8 GetLostTownDays(void) const;
    u8 GetCountNecromancyShrineBuild(void) const;
    u8 GetCountBuilding(u32) const;

    Recruits & GetRecruits(void);

    const KingdomHeroes & GetHeroes(void) const{ return heroes; }
    const KingdomCastles & GetCastles(void) const{ return castles; }

    KingdomHeroes & GetHeroes(void) { return heroes; }
    KingdomCastles & GetCastles(void) { return castles; }

    void AddHeroes(Heroes *);
    void RemoveHeroes(const Heroes *hero);
    void ApplyPlayWithStartingHero(void);
    void HeroesActionNewPosition(void);

    void AddCastle(const Castle *castle);
    void RemoveCastle(const Castle *castle);
    
    void ActionBeforeTurn(void);
    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);

    void SetVisited(const s32 index, const MP2::object_t object = MP2::OBJ_ZERO);
    u16  CountVisitedObjects(const MP2::object_t) const;
    bool isVisited(const u8 object) const;
    bool isVisited(const Maps::Tiles &) const;
    bool isVisited(s32, u8) const;

    bool HeroesMayStillMove(void) const;

    const Puzzle & PuzzleMaps(void) const;
    Puzzle & PuzzleMaps(void);

    void SetVisitTravelersTent(u8);
    bool IsVisitTravelersTent(u8) const;

    void UpdateRecruits(void);
    void LossPostActions(void);

    static u8 GetMaxHeroes(void);

private:
    friend class Game::IO;

    u8 color;
    Funds resource;

    u8  lost_town_days;

    KingdomCastles castles;
    KingdomHeroes heroes;

    Recruits recruits;
    LastLoseHero lost_hero;

    std::list<IndexObject> visit_object;

    Puzzle puzzle_maps;
    u8 visited_tents_colors;

    KingdomHeroes heroes_cond_loss;
};

class Kingdoms
{
public:
    Kingdoms();

    void Init(void);
    void clear(void);

    void ApplyPlayWithStartingHero(void);

    void NewDay(void);
    void NewWeek(void);
    void NewMonth(void);

    Kingdom & GetKingdom(u8 color);
    const Kingdom & GetKingdom(u8 color) const;

    u8 GetLossColors(void) const;
    u8 GetNotLossColors(void) const;
    u8 FindWins(u16) const;

    void AddHeroes(const AllHeroes &);
    void AddCastles(const AllCastles &);

    void AddCondLossHeroes(const AllHeroes &);

    u8 size(void) const;

private:
    friend class Game::IO;

    Kingdom kingdoms[KINGDOMMAX + 1];
};

#endif
