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
#include "maps.h"
#include "race.h"
#include "rect.h"
#include "maps_tiles.h"
#include "heroes.h"
#include "week.h"
#include "color.h"

class Heroes;
class Castle;
class Kingdom;

namespace GameEvent
{
    class Day;
    class Coord;
    class Riddle;
};

#define DAYOFWEEK       7 
#define WEEKOFMONTH     4 

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
    Kingdom & GetKingdom(Color::color_t color);
    const Kingdom & GetMyKingdom(void) const;
    const Kingdom & GetKingdom(Color::color_t color) const;

    const Castle * GetCastle(u16 maps_index);
    const Castle * GetCastle(u8 ax, u8 ay);
    const Castle * GetCastle(const Point & pt){ return GetCastle(pt.x, pt.y); };

    const Heroes * GetHeroes(u16 maps_index) const;
    const Heroes * GetHeroes(u8 ax, u8 ay) const;
    const Heroes * GetHeroes(const Point & pt) const{ return GetHeroes(pt.x, pt.y); };
    const Heroes * GetFreemanHeroes(Race::race_t rc = Race::BOMG);

    const Heroes::heroes_t & GetFreeRecruit1(void);
    const Heroes::heroes_t & GetFreeRecruit2(void);

    Heroes & GetHeroes(const Heroes::heroes_t & hero){ return *vec_heroes[hero]; };

    Artifact::artifact_t DiggingForUltimateArtifacts(const Point & center);

    u8 GetDay(void){ return day % DAYOFWEEK + 1; };
    u8 GetWeek(void){ return week % WEEKOFMONTH + 1; };
    u8 GetMonth(void){ return month + 1; };
    u16 CountDay(void){ return day + 1; };
    u16 CountWeek(void){ return week + 1; };
    bool BeginWeek(void){ return begin_week; };
    bool BeginMonth(void){ return begin_month; };
    Week::type_t GetWeekType(void) const{ return week_name; };

    void NextDay(void);
    void NewWeek(void);
    void NewMonth(void);

    const std::string & GetRumors(void);
    
    Spell::spell_t SpellFromShrine(const u16 index);
    u16 NextTeleport(const u16 index) const;
    Skill::Secondary::skill_t SkillFromWitchsHut(const u16 index);
    const std::string & MessageSign(const u16 index);

    void CaptureObject(const u16 index, const Color::color_t col);
    u16 CountCapturedObject(const MP2::object_t obj, const Color::color_t col) const;
    u16 CountCapturedMines(const Resource::resource_t res, const Color::color_t col) const;
    Color::color_t ColorCapturedObject(const u16 index) const;

    void ClearFog(const u8 color);

    static u32 GetUniq(void){ return ++uniq0; };

protected:
    void UpdateDwellingPopulation(void);
    void UpdateMonsterPopulation(void);

private:
    World() : Size(0, 0), width(Size::w), height(Size::h) {};

    void FreeOldMaps(void);

private:
    std::vector<Maps::Tiles *>          vec_tiles;
    std::vector<Kingdom *>              vec_kingdoms;
    std::vector<GameEvent::Day *>       vec_eventsday;
    std::vector<GameEvent::Coord *>     vec_eventsmap;
    std::vector<GameEvent::Riddle *>    vec_riddles;
    std::vector<std::string>     	vec_rumors;
    std::vector<Castle *>               vec_castles;
    std::vector<Heroes *>               vec_heroes;
    std::vector<u16>                    vec_teleports;

    std::map<u16, std::string>		map_sign;
    std::map<u16, Spell::spell_t>	map_shrine;
    std::map<u16, Skill::Secondary::skill_t> map_witchshut;

    // index, object, color
    std::map<u16, std::pair<MP2::object_t, Color::color_t> > map_captureobj;

    u16 ultimate_artifact;

    u16 & width;
    u16 & height;

    u16 day;
    u16 week;
    u8 month;
    bool begin_week;
    bool begin_month;

    Heroes::heroes_t free_recruit_hero1;
    Heroes::heroes_t free_recruit_hero2;

    Week::type_t week_name;
    
    static u32 uniq0;
};

extern World & world;

#endif
