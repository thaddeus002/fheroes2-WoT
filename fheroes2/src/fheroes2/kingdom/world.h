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
#include "week.h"
#include "color.h"
#include "kingdom.h"
#include "sprite.h"
#include "castle_heroes.h"
#include "gameevent.h"
#include "artifact_ultimate.h"

class Heroes;
class Castle;
class Kingdom;
class Recruits;
class Radar;

typedef std::list<std::string>		Rumors;
typedef std::list<EventDate>		EventsDate;
typedef std::list<EventMaps>		EventsMaps;
typedef std::list<Riddle>		Riddles;
typedef std::vector<Maps::Tiles>	MapsTiles;

struct CapturedObject
{
    ObjectColor		objcol;
    Troop		guardians;
    u8			split;

    CapturedObject() : split(1) {}

    const u8 & GetSplit(void) const { return split; }
    const u8 & GetObject(void) const { return objcol.first; }
    const u8 & GetColor(void) const { return objcol.second; }
    Troop & GetTroop(void) { return guardians; }

    void Set(u8 obj, u8 col) { objcol = ObjectColor(obj, col); }
    void SetColor(u8 col) { objcol.second = col; }
    void SetSplit(u8 spl) { split = spl; }

    bool GuardiansProtected(void) const { return guardians.isValid(); }
};

struct CapturedObjects : std::map<s32, CapturedObject>
{
    void Set(const s32 &, u8, u8);
    void SetColor(const s32 &, u8);
    void ClearFog(u8);
    void ResetColor(u8);

    CapturedObject & Get(const s32 &);
    Funds TributeCapturedObject(u8 col, u8 obj);

    u16	 GetCount(u8, u8) const;
    u16	 GetCountMines(u8, u8) const;
    u8   GetColor(const s32 &) const;
};

class World : protected Size
{
public:
    ~World(){ Reset(); }

    void LoadMaps(const std::string &filename);
    void NewMaps(const u16 sw, const u16 sh);

    static World & Get(void);

    const u16 & w(void){ return width; }
    const u16 & h(void){ return height; }

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

    Kingdoms & GetKingdoms(void);
    Kingdom & GetKingdom(u8 color);
    const Kingdom & GetKingdom(u8 color) const;

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

    const UltimateArtifact & GetUltimateArtifact(void) const;
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
    std::string DateString(void) const;

    void NewDay(void);
    void NewWeek(void);
    void NewMonth(void);

    const std::string & GetRumors(void);
    
    s32 NextTeleport(const s32 index, bool onwater) const;
    s32 NextWhirlpool(const s32 index);

    const std::string & MessageSign(const s32 index);

    void CaptureObject(const s32 & index, u8 col);
    u16  CountCapturedObject(u8 obj, u8 col) const;
    u16  CountCapturedMines(u8 type, u8 col) const;
    u8   ColorCapturedObject(const s32 & index) const;
    void ResetCapturedObjects(u8);
    CapturedObject & GetCapturedObject(const s32 &);

    void ActionToEyeMagi(u8 color) const;

    u16 CountObeliskOnMaps(void);

    void ClearFog(u8 color);

    u16  CheckKingdomWins(const Kingdom &) const;
    bool KingdomIsWins(const Kingdom &, u16 wins) const;
    u16  CheckKingdomLoss(const Kingdom &) const;
    bool KingdomIsLoss(const Kingdom &, u16 loss) const;

    void AddEventDate(const EventDate &);
    EventsDate GetEventsDate(u8 color) const;
    EventMaps* GetEventMaps(u8 color, s32 index);
    Riddle* GetSphinxRiddle(s32 index);

    Heroes* GetFreemanHeroes(u8 rc = 0) const;
    void UpdateRecruits(Recruits &) const;

    static u32 GetUniq(void);

private:
    World() : Size(0, 0), width(Size::w), height(Size::h) {};
    void Defaults(void);
    void Reset(void);
    void MonthOfMonstersAction(const Monster &);

private:
    friend class Radar;
    friend StreamBase & operator<< (StreamBase &, const World &);
    friend StreamBase & operator>> (StreamBase &, World &);

    MapsTiles				vec_tiles;
    AllHeroes				vec_heroes;
    AllCastles                          vec_castles;
    Kingdoms				vec_kingdoms;
    Rumors				vec_rumors;
    EventsDate                          vec_eventsday;
    EventsMaps                          vec_eventsmap;
    Riddles				vec_riddles;

    std::map<s32, std::string>		map_sign;

    // index, object, color
    CapturedObjects			map_captureobj;

    UltimateArtifact			ultimate_artifact;

    u16 & width;
    u16 & height;

    u16 day;
    u16 week;
    u8 month;

    Week week_current;
    Week week_next;

    Heroes::heroes_t heroes_cond_wins;
    Heroes::heroes_t heroes_cond_loss;
};

StreamBase & operator<< (StreamBase &, const CapturedObject &);
StreamBase & operator>> (StreamBase &, CapturedObject &);
StreamBase & operator<< (StreamBase &, const World &);
StreamBase & operator>> (StreamBase &, World &);

extern World & world;

#endif
