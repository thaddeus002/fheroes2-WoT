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
#ifndef H2WORLD_H
#define H2WORLD_H

#include <vector>
#include <map>
#include <string>
#include "gamedefs.h"
#include "maps.h"
#include "maps_tiles.h"
#include "heroes.h"
#include "week.h"
#include "color.h"
#include "kingdom.h"
#include "sprite.h"
#include "castle_heroes.h"
#include "gameevent.h"
#include "game_io.h"

class Heroes;
class Castle;
class Kingdom;
class Recruits;
class Radar;
class Artifact;

typedef std::list<std::string> Rumors;
typedef std::list<EventDate> EventsDate;
typedef std::list<EventMaps> EventsMaps;
typedef std::list<Riddle>    Riddles;
typedef std::vector<Maps::Tiles> MapsTiles;

class World : protected Size
{
public:
    ~World(){ Reset(); }

    void LoadMaps(const std::string &filename);
    void NewMaps(const u16 sw, const u16 sh);

    static World & Get(void);

    u16 w(void){ return width; }
    u16 h(void){ return height; }

    const Maps::Tiles & GetTiles(const Point & pt) const{ return GetTiles(pt.y * width + pt.x); }
    const Maps::Tiles & GetTiles(u16 ax, u16 ay) const{ return GetTiles(ay * width + ax); }
#ifdef WITH_DEBUG
    const Maps::Tiles & GetTiles(s32 index) const{ return vec_tiles.at(index); }
#else
    const Maps::Tiles & GetTiles(s32 index) const{ return vec_tiles[index]; }
#endif
    Maps::Tiles & GetTiles(const Point & pt) { return GetTiles(pt.y * width + pt.x); }
    Maps::Tiles & GetTiles(u16 ax, u16 ay) { return GetTiles(ay * width + ax); }
    Maps::Tiles & GetTiles(s32 index){ return vec_tiles.at(index); }

    Kingdom & GetMyKingdom(void);
    Kingdom & GetKingdom(u8 color);
    const Kingdom & GetMyKingdom(void) const;
    const Kingdom & GetKingdom(u8 color) const;
    void KingdomLoss(const Color::color_t);

    const Castle* GetCastle(s32 maps_index) const;
    Castle* GetCastle(s32 maps_index);

    const Heroes* GetHeroes(Heroes::heroes_t) const;
    const Heroes* GetHeroes(s32 maps_index) const;
    Heroes* GetHeroes(Heroes::heroes_t);
    Heroes* GetHeroes(s32 maps_index);
    CastleHeroes GetHeroes(const Castle &) const;

    Heroes* FromJail(s32);
    const Heroes* GetHeroesCondWins(void) const;
    const Heroes* GetHeroesCondLoss(void) const;

    const Surface & GetPuzzleSurface(void) const;
    Artifact GetUltimateArtifact(void) const;
    bool DiggingForUltimateArtifact(const Point & center);
    void ActionForMagellanMaps(u8 color);

    u8 GetDay(void) const{ return LastDay() ? DAYOFWEEK : day % DAYOFWEEK; }
    u8 GetWeek(void) const{ return LastWeek() ? WEEKOFMONTH : week % WEEKOFMONTH; }
    u8 GetMonth(void) const{ return month; }
    u16 CountDay(void) const{ return day; }
    u16 CountWeek(void) const{ return week; }
    bool BeginWeek(void) const{ return 1 == (day % DAYOFWEEK); }
    bool BeginMonth(void) const{ return 1 == (week % WEEKOFMONTH) && BeginWeek(); }
    bool LastDay(void) const{ return (0 == (day % DAYOFWEEK)); }
    bool LastWeek(void) const{ return (0 == (week % WEEKOFMONTH)); }
    const Week & GetWeekType(void) const{ return week_current; }
    void DateDump(void) const;

    void NewDay(void);
    void NewWeek(void);
    void NewMonth(void);

    const std::string & GetRumors(void);
    
    s32 NextTeleport(const s32 index) const;
    s32 NextWhirlpool(const s32 index);

    const std::string & MessageSign(const s32 index);

    bool GetObjectPositions(MP2::object_t, std::vector<s32> &, bool check_hero = false) const;
    bool GetObjectPositions(s32 center, MP2::object_t, std::vector<IndexDistance> &, bool check_hero = false) const;
    s32 GetNearestObject(s32 center, MP2::object_t, bool check_hero = false) const;

    void CaptureObject(const s32 index, const Color::color_t col);
    u16 CountCapturedObject(const MP2::object_t obj, const Color::color_t col) const;
    u16 CountCapturedMines(const u8 res, const Color::color_t col) const;
    Color::color_t ColorCapturedObject(const s32 index) const;

    void ActionToEyeMagi(const Color::color_t) const;

    u16 CountObeliskOnMaps(void);

    void ClearFog(const u8 color);

    u16  CheckKingdomWins(const Kingdom &) const;
    bool KingdomIsWins(const Kingdom &, u16 wins) const;
    u16  CheckKingdomLoss(const Kingdom &) const;
    bool KingdomIsLoss(const Kingdom &, u16 loss) const;

    EventsDate GetEventsDate(const Color::color_t) const;
    EventMaps* GetEventMaps(const Color::color_t c, const s32 index);
    Riddle* GetSphinx(const s32 index);

    Heroes* GetFreemanHeroes(u8 rc = 0) const;
    void UpdateRecruits(Recruits &) const;

    static u32 GetUniq(void){ return ++uniq0; };

protected:
    void UpdateDwellingPopulation(void);
    void UpdateMonsterPopulation(void);
    void MonthOfMonstersAction(const Monster &);

private:
    World() : Size(0, 0), width(Size::w), height(Size::h) {};
    void Defaults(void);
    void Reset(void);

private:
    friend class Radar;
    friend class Game::IO;

    MapsTiles				vec_tiles;
    Kingdoms				vec_kingdoms;
    Rumors				vec_rumors;
    std::vector<Castle *>               vec_castles;
    std::vector<Heroes *>               vec_heroes;
    EventsDate                          vec_eventsday;
    EventsMaps                          vec_eventsmap;
    Riddles				vec_riddles;

    std::map<s32, std::string>		map_sign;

    // index, object, color
    std::map<s32, ObjectColor> map_captureobj;

    s32 ultimate_index;

    u16 & width;
    u16 & height;

    u16 day;
    u16 week;
    u8 month;

    Week week_current;
    Week week_next;

    Heroes::heroes_t heroes_cond_wins;
    Heroes::heroes_t heroes_cond_loss;

    Surface puzzle_surface;

    static u32 uniq0;
};

extern World & world;

#endif
