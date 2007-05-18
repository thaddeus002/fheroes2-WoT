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
#include <string>
#include "maps.h"
#include "race.h"
#include "maps_tiles.h"
#include "color.h"

class Heroes;
class Castle;
class Kingdom;

namespace GameEvent
{
    class Sign;
    class Day;
    class Coord;
    class Riddle;
    class Rumor;
};

#define DAYOFWEEK       7 
#define WEEKOFMONTH     4 

class World
{
public:
    ~World(){ FreeOldMaps(); };

    void LoadMaps(const std::string &filename);

    static World & GetWorld(void);

    u16 w(void){ return width; };
    u16 h(void){ return height; };

    const Maps::Tiles & GetTiles(u16 index) const{ return *vec_tiles.at(index); };
    Maps::Tiles & GetTiles(u16 index){ return *vec_tiles.at(index); };
    const Kingdom & GetMyKingdom(void) const;
    const Kingdom & GetKingdom(Color::color_t color) const;

    const Castle * GetCastle(u16 maps_index);
    const Castle * GetCastle(u8 ax, u8 ay);

    const Heroes * GetHeroes(u16 maps_index);
    const Heroes * GetHeroes(u8 ax, u8 ay);
    const Heroes * GetFreemanHeroes(Race::race_t rc);

    u8 GetDay(void){ return day % DAYOFWEEK + 1; };
    u8 GetWeek(void){ return week % WEEKOFMONTH + 1; };
    u8 GetMonth(void){ return month + 1; };
    u16 CountDay(void){ return day + 1; };
    u16 CountWeek(void){ return week + 1; };
    bool BeginWeek(void){ return begin_week; };
    bool BeginMonth(void){ return begin_month; };
    void NextDay(void);

private:
    World(){};
    
    void FreeOldMaps(void);

private:
    std::vector<Maps::Tiles *>          vec_tiles;
    std::vector<Kingdom *>              vec_kingdoms;
    std::vector<GameEvent::Sign *>      vec_signs;
    std::vector<GameEvent::Day *>       vec_eventsday;
    std::vector<GameEvent::Coord *>     vec_eventsmap;
    std::vector<GameEvent::Riddle *>    vec_riddles;
    std::vector<GameEvent::Rumor *>     vec_rumors;
    std::vector<Castle *>               vec_castles;
    std::vector<Heroes *>               vec_heroes;

    u16 ultimate_artifact;

    Maps::mapsize_t width;
    Maps::mapsize_t height;

    u16 day;
    u16 week;
    u8 month;
    bool begin_week;
    bool begin_month;
};

static World & world = World::GetWorld();

#endif
