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
#ifndef H2MAPSDATA_H
#define H2MAPSDATA_H

#include <map>
#include <vector>
#include "config.h"
#include "gamedefs.h"
#include "gameevent.h"
#include "surface.h"
#include "kingdom.h"
#include "rect.h"
#include "maps.h"
#include "mp2.h"

#define DAYOFWEEK       7
#define WEEKOFMONTH     4

class World
{
public:
    World(const std::string &filename);
    ~World();

    u16 w(void) const{ return width; };
    u16 h(void) const{ return height; };

    const Surface & GetSpriteMaps(void) const{ return *sprite_maps; };
    u16 GetGround(u16 index) const{ return index < vec_tiles.size() ? vec_tiles[index].ground : 0; };
    u8  GetObject(u16 index) const{ return index < vec_tiles.size() ? vec_tiles[index].object : 0; };

    bool Movement(u16 ax, u16 ay) const{ return Movement(ax * width + ay); };
    bool Movement(u16 index) const;

    void Redraw(const Rect &rt, const Point &pt = Point(0, 0)) const;

    u8 GetDay(void) const{ return day % DAYOFWEEK; };
    u8 GetWeek(void) const{ return week % WEEKOFMONTH; };
    u8 GetMonth(void) const{ return month; };
    u16 CountDay(void) const{ return day; };
    u16 CountWeek(void) const{ return week; };
    bool BeginWeek(void) const{ return begin_week; };
    bool BeginMonth(void) const{ return begin_month; };
    void NextDay(void);

    const Kingdom & GetMyKingdom(void) const{ return GetKingdom(H2Config::GetHumanColor()); };
    const Kingdom & GetKingdom(Color::color_t color) const;

private:
    Surface *sprite_maps;
    std::vector<Maps::tiles_t> vec_tiles;

    std::map<Color::color_t, Kingdom> kingdom;

    std::vector<GameEvent::Sign> sign;
    std::vector<GameEvent::Day> event_day;
    std::vector<GameEvent::Coord> event_coord;
    std::vector<GameEvent::Riddle> riddle;
    std::vector<std::string> rumors;

    //std::vector<castle_t> allCastles;
    //std::vector<heroes_t> allHeroes;


    u16 width;
    u16 height;

    u16 day;
    u16 week;
    u8 month;
    bool begin_week;
    bool begin_month;
};

#endif
