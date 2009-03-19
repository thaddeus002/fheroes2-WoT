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
#ifndef H2WORLD_H
#define H2WORLD_H

#include <vector>
#include <map>
#include <string>
#include "gamedefs.h"
#include "maps.h"
#include "race.h"
#include "maps_tiles.h"
#include "heroes.h"
#include "week.h"
#include "color.h"
#include "game_io.h"

class Heroes;
class Castle;
class Kingdom;
class Radar;

namespace GameEvent
{
    class Day;
    class Coord;
    class Riddle;
};

struct Recruits : public std::pair<Heroes*, Heroes*>
{
    Recruits() : std::pair<Heroes*, Heroes*>(NULL, NULL){};
};

#define DAYOFWEEK       7 
#define WEEKOFMONTH     4 
#define MAXCASTLES	72

#define OBSERVATIONTOWERSCOUTE 10
#define MAGIEYESCOUTE 9

class World : protected Size
{
public:
    ~World(){ FreeOldMaps(); };

    void LoadMaps(const std::string &filename);
    void NewMaps(const u16 sw, const u16 sh);

    static World & GetWorld(void);

    u16 w(void){ return width; };
    u16 h(void){ return height; };

    const Maps::Tiles & GetTiles(const Point & pt) const{ return GetTiles(pt.y * width + pt.x); };
    const Maps::Tiles & GetTiles(u16 ax, u16 ay) const{ return GetTiles(ay * width + ax); };
    const Maps::Tiles & GetTiles(u16 index) const{ return *vec_tiles.at(index); };
    Maps::Tiles & GetTiles(const Point & pt) { return GetTiles(pt.y * width + pt.x); };
    Maps::Tiles & GetTiles(u16 ax, u16 ay) { return GetTiles(ay * width + ax); };
    Maps::Tiles & GetTiles(u16 index){ return *vec_tiles.at(index); };

    Kingdom & GetMyKingdom(void);
    Kingdom & GetKingdom(u8 color);
    const Kingdom & GetMyKingdom(void) const;
    const Kingdom & GetKingdom(u8 color) const;
    void KingdomLoss(const Color::color_t);

    const Castle * GetCastle(u16 maps_index) const;
    const Castle * GetCastle(const Point & pt) const{ return GetCastle(pt.x, pt.y); };
    Castle * GetCastle(u16 maps_index);
    Castle * GetCastle(u8 ax, u8 ay) const;

    const Heroes * GetHeroes(u16 maps_index) const;
    const Heroes * GetHeroes(const Point & pt) const{ return GetHeroes(pt.x, pt.y); };
    Heroes * GetHeroes(u16 maps_index);
    Heroes * GetHeroes(u8 ax, u8 ay) const;
    Heroes * FromJail(u16);

    Recruits & GetRecruits(Color::color_t);

    Surface & GetUltimateArtifactArea(void);
    u16 GetUltimateArtifactIndex(void);
    Artifact::artifact_t DiggingForUltimateArtifacts(const Point & center);
    void ActionForMagellanMaps(u8 color);

    u8 GetDay(void) const{ return day % DAYOFWEEK + 1; };
    u8 GetWeek(void) const{ return week % WEEKOFMONTH + 1; };
    u8 GetMonth(void) const{ return month + 1; };
    u16 CountDay(void) const{ return day + 1; };
    u16 CountWeek(void) const{ return week + 1; };
    bool BeginWeek(void) const{ return begin_week; };
    bool BeginMonth(void) const{ return begin_month; };
    bool LastDay(void) const{ return ((day % DAYOFWEEK) == (DAYOFWEEK - 1)); };
    bool LastWeek(void) const{ return ((week % WEEKOFMONTH) == (WEEKOFMONTH - 1)); };
    Week::type_t GetWeekType(void) const{ return week_name; };
    void DateDump(void) const;

    void NewDay(void);
    void NewWeek(void);
    void NewMonth(void);

    const std::string & GetRumors(void);
    
    u16 NextTeleport(const u16 index) const;
    u16 NextWhirlpool(const u16 index);

    const std::string & MessageSign(const u16 index);

    u16 GetNearestObject(const u16 center, const MP2::object_t obj);
    void CaptureObject(const u16 index, const Color::color_t col);
    u16 CountCapturedObject(const MP2::object_t obj, const Color::color_t col) const;
    u16 CountCapturedMines(const Resource::resource_t res, const Color::color_t col) const;
    Color::color_t ColorCapturedObject(const u16 index) const;
    void StoreActionObject(const u8 color, std::map<u16, MP2::object_t> & store);

    void ActionToEyeMagi(const Color::color_t) const;

    u16 CountObeliskOnMaps(void);

    bool CreateBoat(const u16 center, bool build);
    void ClearFog(const u8 color);

    const GameEvent::Day*   GetEventDay(const Color::color_t c) const;
    const GameEvent::Coord* GetEventMaps(const Color::color_t c, const u16 index) const;
    GameEvent::Riddle* GetSphinx(const u16 index) const;

    static u32 GetUniq(void){ return ++uniq0; };

protected:
    void UpdateDwellingPopulation(void);
    void UpdateMonsterPopulation(void);
    void UpdateRecruits(void);
    void GetObjectIndexes(std::vector<u16> &, MP2::object_t, bool) const;
    Heroes* GetFreemanHeroes(Race::race_t rc = Race::BOMG);

private:
    World() : Size(0, 0), ultimate_artifact_area(448, 448), width(Size::w), height(Size::h) {};
    void Defaults(void);
    void FreeOldMaps(void);

private:
    friend class Radar;
    friend void Game::SaveXML(const std::string &);
    friend void Game::LoadXML(const std::string &);
    
    std::vector<Maps::Tiles *>          vec_tiles;
    std::vector<Kingdom *>              vec_kingdoms;
    std::vector<GameEvent::Day *>       vec_eventsday;
    std::vector<GameEvent::Coord *>     vec_eventsmap;
    std::vector<GameEvent::Riddle *>    vec_riddles;
    std::vector<std::string>     	vec_rumors;
    std::vector<Castle *>               vec_castles;
    std::vector<Heroes *>               vec_heroes;

    std::map<Color::color_t, Recruits>	map_recruits;
    std::map<u32, std::vector<u16> >	map_whirlpools;
    std::map<u16, std::string>		map_sign;

    // index, object, color
    std::map<u16, std::pair<MP2::object_t, Color::color_t> > map_captureobj;

    u16 ultimate_artifact;
    Surface ultimate_artifact_area;

    u16 & width;
    u16 & height;

    u16 day;
    u16 week;
    u8 month;
    bool begin_week;
    bool begin_month;

    Week::type_t week_name;    
    u16 count_obelisk;

    static u32 uniq0;
};

extern World & world;

#endif
