/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include "zzlib.h"
#include "settings.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"
#include "army.h"
#include "world.h"
#include "gameevent.h"
#include "interface_gamearea.h"
#include "tools.h"

#define FORMAT_VERSION_1335 0x537
#define FORMAT_VERSION_1293 0x50C

#define CURRENT_FORMAT_VERSION FORMAT_VERSION_1335

bool Game::Save(const std::string &fn)
{
    if(Settings::Get().Debug()) Error::Verbose("Game::Save: " + fn);

    Game::IO msg;

    if(! Game::IO::SaveBIN(msg)) return false;

    std::fstream fs(fn.c_str(), std::ios::out | std::ios::binary);
    if(fs.fail()) return false;

#ifdef WITH_ZLIB
    std::vector<char> v;
    if(!ZLib::Compress(v, msg.DtPt(), msg.DtSz())) return false;
    fs.write(&v[0], v.size());
#else
    fs.write(msg.DtPt(), msg.DtSz());
#endif
    fs.close();

    return true;
}

bool Game::IO::LoadSAV(const std::string & fn)
{
    if(fn.empty()) return false;

    std::fstream fs(fn.c_str(), std::ios::in | std::ios::binary);
    if(fs.fail()) return false;

    fs.seekg(0, std::ios_base::end);
    dtsz = fs.tellg();
    fs.seekg(0, std::ios_base::beg);

    delete [] data;
    data = new char [dtsz];

    fs.read(data, dtsz);
    fs.close();

#ifdef WITH_ZLIB
    std::vector<char> v;
    if(ZLib::UnCompress(v, data, dtsz))
    {
	dtsz = v.size();
	delete [] data;
        data = new char [dtsz];
	std::memcpy(data, &v[0], dtsz);
	v.clear();
    }
#endif

    itd1 = data;
    itd2 = data + dtsz;

    return true;
}

bool Game::Load(const std::string & fn)
{
    if(Settings::Get().Debug()) Error::Verbose("Game::Load: " + fn);

    // loading info
    Display & display = Display::Get();
    display.Fill(0, 0, 0);
    TextBox(_("Maps Loading..."), Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    Game::IO msg;
    if(!msg.LoadSAV(fn) || !Game::IO::LoadBIN(msg)) return false;

    Settings::Get().SetModes(Settings::LOADGAME);
    return true;
}

bool Game::LoadSAV2FileInfo(const std::string & fn,  Maps::FileInfo & maps_file)
{
    Game::IO msg;

    if(!msg.LoadSAV(fn)) return false;

    u8 byte8;
    u16 byte16;
    u32 byte32;
    std::string str;

    maps_file.file = fn;

    msg.Pop(byte16);
    if(byte16 != 0xFF01) return false;
    // format version
    msg.Pop(byte16);
    if(byte16 >= FORMAT_VERSION_1293)
    {
	// major version
	msg.Pop(byte8);
	// minor version
	msg.Pop(byte8);
    }
    // svn
    msg.Pop(str);
    // time
    msg.Pop(byte32);
    maps_file.localtime = byte32;

    // maps
    msg.Pop(byte16);
    msg.Pop(maps_file.size_w);
    msg.Pop(maps_file.size_h);
    msg.Pop(str);
    msg.Pop(maps_file.difficulty);
    msg.Pop(maps_file.kingdom_colors);
    msg.Pop(maps_file.allow_colors);
    msg.Pop(maps_file.rnd_colors);
    msg.Pop(maps_file.rnd_races);
    msg.Pop(maps_file.conditions_wins);
    msg.Pop(maps_file.wins1);
    msg.Pop(maps_file.wins2);
    msg.Pop(maps_file.wins3);
    msg.Pop(maps_file.wins4);
    msg.Pop(maps_file.conditions_loss);
    msg.Pop(maps_file.loss1);
    msg.Pop(maps_file.loss2);
    // races
    msg.Pop(byte16);
    msg.Pop(byte8); maps_file.races[0] = byte8;
    msg.Pop(byte8); maps_file.races[1] = byte8;
    msg.Pop(byte8); maps_file.races[2] = byte8;
    msg.Pop(byte8); maps_file.races[3] = byte8;
    msg.Pop(byte8); maps_file.races[4] = byte8;
    msg.Pop(byte8); maps_file.races[5] = byte8;
    // maps name
    msg.Pop(maps_file.name);
    // maps description
    msg.Pop(maps_file.description);

    return true;
}

bool Game::IO::SaveBIN(QueueMessage & msg)
{
    const Settings & conf = Settings::Get();

    msg.Reserve(world.w() > Maps::MEDIUM ? 600 * 1024 : 200 * 1024);

    msg.Push(static_cast<u16>(0xFF01));
    // format version
    msg.Push(static_cast<u16>(CURRENT_FORMAT_VERSION));
    // version
    msg.Push(conf.major_version);
    msg.Push(conf.minor_version);
    // svn
    msg.Push(conf.svn_version);
    // time
    msg.Push(static_cast<u32>(std::time(NULL)));

    // maps
    msg.Push(static_cast<u16>(0xFF02));
    msg.Push(conf.current_maps_file.size_w);
    msg.Push(conf.current_maps_file.size_h);
    msg.Push(GetBasename(conf.current_maps_file.file.c_str()));
    msg.Push(conf.current_maps_file.difficulty);
    msg.Push(conf.current_maps_file.kingdom_colors);
    msg.Push(conf.current_maps_file.allow_colors);
    msg.Push(conf.current_maps_file.rnd_colors);
    msg.Push(conf.current_maps_file.rnd_races);
    msg.Push(conf.current_maps_file.conditions_wins);
    msg.Push(conf.current_maps_file.wins1);
    msg.Push(conf.current_maps_file.wins2);
    msg.Push(conf.current_maps_file.wins3);
    msg.Push(conf.current_maps_file.wins4);
    msg.Push(conf.current_maps_file.conditions_loss);
    msg.Push(conf.current_maps_file.loss1);
    msg.Push(conf.current_maps_file.loss2);
    // races
    msg.Push(static_cast<u16>(0xFF03));
    msg.Push(conf.current_maps_file.races[0]);
    msg.Push(conf.current_maps_file.races[1]);
    msg.Push(conf.current_maps_file.races[2]);
    msg.Push(conf.current_maps_file.races[3]);
    msg.Push(conf.current_maps_file.races[4]);
    msg.Push(conf.current_maps_file.races[5]);
    // maps name
    msg.Push(conf.current_maps_file.name);
    // maps description
    msg.Push(conf.current_maps_file.description);
    // game
    msg.Push(static_cast<u16>(0xFF04));
    msg.Push(static_cast<u8>(conf.game_difficulty));
    msg.Push(static_cast<u8>(conf.my_color));
    msg.Push(static_cast<u8>(conf.cur_color));
    msg.Push(conf.game_type);
    msg.Push(conf.players_colors);
    msg.Push(conf.preferably_count_players);
    msg.Push(conf.debug);
    msg.Push(static_cast<u8>(conf.Original()));

    // world
    msg.Push(static_cast<u16>(0xFF05));
    msg.Push(world.width);
    msg.Push(world.height);
    msg.Push(world.ultimate_artifact);
    msg.Push(world.uniq0);
    msg.Push(static_cast<u8>(world.week_name));
    msg.Push(static_cast<u8>(world.heroes_cond_wins));
    msg.Push(static_cast<u8>(world.heroes_cond_loss));
    msg.Push(world.month);
    msg.Push(world.week);
    msg.Push(world.day);

    // tiles
    msg.Push(static_cast<u16>(0xFF06));
    msg.Push(static_cast<u32>(world.vec_tiles.size()));
    for(u32 ii = 0; ii < world.vec_tiles.size(); ++ii)
    {
	if(NULL == world.vec_tiles[ii]){ Error::Warning("Game::IO::SaveBIN: tiles is NULL"); return false; }
	const Maps::Tiles & tile = *world.vec_tiles[ii];

	msg.Push(tile.tile_index);
	msg.Push(tile.shape);
	msg.Push(tile.general);
	msg.Push(tile.quantity1);
	msg.Push(tile.quantity2);
	msg.Push(tile.fogs);
	msg.Push(tile.flags);

	// addons 1
	msg.Push(static_cast<u32>(tile.addons_level1.size()));
	std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level1.begin();
	std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level1.end();
	for(; it1 != it2; ++it1)
	{
	    const Maps::TilesAddon & addon = *it1;
	    msg.Push(addon.level);
	    msg.Push(addon.uniq);
	    msg.Push(addon.object);
	    msg.Push(addon.index);
	}

	// addons 2
	msg.Push(static_cast<u32>(tile.addons_level2.size()));
	it1 = tile.addons_level2.begin();
	it2 = tile.addons_level2.end();
	for(; it1 != it2; ++it1)
	{
	    const Maps::TilesAddon & addon = *it1;
	    msg.Push(addon.level);
	    msg.Push(addon.uniq);
	    msg.Push(addon.object);
	    msg.Push(addon.index);
	}
    }

    // heroes
    msg.Push(static_cast<u16>(0xFF07));
    msg.Push(static_cast<u32>(world.vec_heroes.size()));
    for(u32 ii = 0; ii < world.vec_heroes.size(); ++ii)
    {
	if(NULL == world.vec_heroes[ii]){ Error::Warning("Game::IO::SaveBIN: heroes is NULL"); return false; }
	PackHeroes(msg, *world.vec_heroes[ii]);
    }

    // castles
    msg.Push(static_cast<u16>(0xFF08));
    msg.Push(static_cast<u32>(world.vec_castles.size()));
    for(u32 ii = 0; ii < world.vec_castles.size(); ++ii)
    {
	if(NULL == world.vec_castles[ii]){ Error::Warning("Game::IO::SaveBIN: castles is NULL"); return false; }
	PackCastle(msg, *world.vec_castles[ii]);
    }

    // kingdoms
    msg.Push(static_cast<u16>(0xFF09));
    msg.Push(static_cast<u32>(world.vec_kingdoms.size()));
    for(u32 ii = 0; ii < world.vec_kingdoms.size(); ++ii)
    {
	if(NULL == world.vec_kingdoms[ii]){ Error::Warning("Game::IO::SaveBIN: kingdoms is NULL"); return false; }
	PackKingdom(msg, *world.vec_kingdoms[ii]);
    }

    // signs
    msg.Push(static_cast<u16>(0xFF0A));
    msg.Push(static_cast<u32>(world.map_sign.size()));
    {
        std::map<u16, std::string>::const_iterator it1 = world.map_sign.begin();
	std::map<u16, std::string>::const_iterator it2 = world.map_sign.end();
	for(; it1 != it2; ++it1)
	{
	    msg.Push((*it1).first);
	    msg.Push((*it1).second);
	}
    }

    // captured object
    msg.Push(static_cast<u16>(0xFF0B));
    msg.Push(static_cast<u32>(world.map_captureobj.size()));
    {
	std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = world.map_captureobj.begin();
	std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = world.map_captureobj.end();
	for(; it1 != it2; ++it1)
	{
	    msg.Push((*it1).first);
	    msg.Push(static_cast<u8>((*it1).second.first));
	    msg.Push(static_cast<u8>((*it1).second.second));
	}
    }

    // rumors
    msg.Push(static_cast<u16>(0xFF0C));
    msg.Push(static_cast<u32>(world.vec_rumors.size()));
    {
	std::vector<std::string>::const_iterator it1 = world.vec_rumors.begin();
	std::vector<std::string>::const_iterator it2 = world.vec_rumors.end();
	for(; it1 != it2; ++it1) msg.Push(*it1);
    }

    // day events
    msg.Push(static_cast<u16>(0xFF0D));
    msg.Push(static_cast<u32>(world.vec_eventsday.size()));
    {
	std::vector<GameEvent::Day *>::const_iterator it1 = world.vec_eventsday.begin();
	std::vector<GameEvent::Day *>::const_iterator it2 = world.vec_eventsday.end();
	for(; it1 != it2; ++it1)
	{
	    if(NULL == *it1){ Error::Warning("Game::IO::SaveBIN: eventday is NULL"); return false; }

	    msg.Push(static_cast<u32>((*it1)->resource.wood));
	    msg.Push(static_cast<u32>((*it1)->resource.mercury));
	    msg.Push(static_cast<u32>((*it1)->resource.ore));
	    msg.Push(static_cast<u32>((*it1)->resource.sulfur));
	    msg.Push(static_cast<u32>((*it1)->resource.crystal));
	    msg.Push(static_cast<u32>((*it1)->resource.gems));
	    msg.Push(static_cast<u32>((*it1)->resource.gold));
	    msg.Push(static_cast<u8>((*it1)->computer));
	    msg.Push((*it1)->first);
	    msg.Push((*it1)->subsequent);
	    msg.Push((*it1)->colors);
	    msg.Push((*it1)->message);
	}
    }

    // coord events
    msg.Push(static_cast<u16>(0xFF0E));
    msg.Push(static_cast<u32>(world.vec_eventsmap.size()));
    {
	std::vector<GameEvent::Coord *>::const_iterator it1 = world.vec_eventsmap.begin();
	std::vector<GameEvent::Coord *>::const_iterator it2 = world.vec_eventsmap.end();
	for(; it1 != it2; ++it1)
	{
	    if(NULL == *it1){ Error::Warning("Game::IO::SaveBIN: eventmaps is NULL"); return false; }

	    msg.Push((*it1)->index_map);
	    msg.Push(static_cast<u32>((*it1)->resource.wood));
	    msg.Push(static_cast<u32>((*it1)->resource.mercury));
	    msg.Push(static_cast<u32>((*it1)->resource.ore));
	    msg.Push(static_cast<u32>((*it1)->resource.sulfur));
	    msg.Push(static_cast<u32>((*it1)->resource.crystal));
	    msg.Push(static_cast<u32>((*it1)->resource.gems));
	    msg.Push(static_cast<u32>((*it1)->resource.gold));
	    msg.Push(static_cast<u8>((*it1)->artifact));
	    msg.Push(static_cast<u8>((*it1)->computer));
	    msg.Push(static_cast<u8>((*it1)->cancel));
	    msg.Push((*it1)->colors);
	    msg.Push((*it1)->message);
	}
    }

    // sphinx riddles
    msg.Push(static_cast<u16>(0xFF0F));
    msg.Push(static_cast<u32>(world.vec_riddles.size()));
    {
	std::vector<GameEvent::Riddle *>::const_iterator it1 = world.vec_riddles.begin();
	std::vector<GameEvent::Riddle *>::const_iterator it2 = world.vec_riddles.end();
	for(; it1 != it2; ++it1)
	{
	    if(NULL == *it1){ Error::Warning("Game::IO::SaveBIN: riddles is NULL"); return false; }

	    msg.Push((*it1)->index_map);
	    msg.Push(static_cast<u32>((*it1)->resource.wood));
	    msg.Push(static_cast<u32>((*it1)->resource.mercury));
	    msg.Push(static_cast<u32>((*it1)->resource.ore));
	    msg.Push(static_cast<u32>((*it1)->resource.sulfur));
	    msg.Push(static_cast<u32>((*it1)->resource.crystal));
	    msg.Push(static_cast<u32>((*it1)->resource.gems));
	    msg.Push(static_cast<u32>((*it1)->resource.gold));
	    msg.Push(static_cast<u8>((*it1)->artifact));
	    msg.Push(static_cast<u8>((*it1)->quiet));

	    msg.Push(static_cast<u32>((*it1)->answers.size()));
	    std::vector<std::string>::const_iterator ita1 = (*it1)->answers.begin();
	    std::vector<std::string>::const_iterator ita2 = (*it1)->answers.end();
	    for(; ita1 != ita2; ++ita1) msg.Push(*ita1);

	    msg.Push((*it1)->message);
	}
    }

    msg.Push(static_cast<u16>(0xFFFF));
    return true;
}

void Game::IO::PackKingdom(QueueMessage & msg, const Kingdom & kingdom)
{
    msg.Push(static_cast<u8>(kingdom.color));
    msg.Push(static_cast<u8>(kingdom.control));

    msg.Push(kingdom.flags);
    msg.Push(kingdom.lost_town_days);
    msg.Push(static_cast<u16>(kingdom.ai_capital ? kingdom.ai_capital->GetIndex() : 0));

    // funds
    msg.Push(static_cast<u32>(kingdom.resource.wood));
    msg.Push(static_cast<u32>(kingdom.resource.mercury));
    msg.Push(static_cast<u32>(kingdom.resource.ore));
    msg.Push(static_cast<u32>(kingdom.resource.sulfur));
    msg.Push(static_cast<u32>(kingdom.resource.crystal));
    msg.Push(static_cast<u32>(kingdom.resource.gems));
    msg.Push(static_cast<u32>(kingdom.resource.gold));

    // visit objects
    msg.Push(static_cast<u32>(kingdom.visit_object.size()));
    std::list<IndexObject>::const_iterator it1 = kingdom.visit_object.begin();
    std::list<IndexObject>::const_iterator it2 = kingdom.visit_object.end();
    for(; it1 != it2; ++it1)
    {
	msg.Push((*it1).first);
	msg.Push(static_cast<u8>((*it1).second));
    }

    // recruits
    msg.Push(static_cast<u8>(kingdom.recruits.GetID1()));
    msg.Push(static_cast<u8>(kingdom.recruits.GetID2()));

    // puzzle
    msg.Push(kingdom.puzzle_maps.to_string<char,std::char_traits<char>,std::allocator<char> >());

    // tents colors
    msg.Push(kingdom.visited_tents_colors);
}

void Game::IO::PackCastle(QueueMessage & msg, const Castle & castle)
{
    msg.Push(static_cast<u16>(castle.mp.x));
    msg.Push(static_cast<u16>(castle.mp.y));
    msg.Push(static_cast<u8>(castle.race));

    msg.Push(castle.modes);
    msg.Push(static_cast<u8>(castle.color));
    msg.Push(castle.name);
    msg.Push(castle.building);
	
    // mageguild
    msg.Push(castle.mageguild.level);
    msg.Push(static_cast<u8>(castle.mageguild.upgrade));
    {
	std::vector<Spell::spell_t>::const_iterator it1, it2;

	msg.Push(static_cast<u32>(castle.mageguild.spells.size()));

	it1 = castle.mageguild.spells.begin();
	it2 = castle.mageguild.spells.end();
	for(; it1 != it2; ++it1) msg.Push(static_cast<u8>(*it1));
    }

    // armies
    msg.Push(static_cast<u32>(ARMYMAXTROOPS));
    for(u32 jj = 0; jj < ARMYMAXTROOPS; ++jj)
    {
	const Army::Troop & troop = castle.army.At(jj);
	msg.Push(static_cast<u8>(troop()));
	msg.Push(troop.Count());
    }
	
    // dwelling
    msg.Push(static_cast<u32>(CASTLEMAXMONSTER));
    for(u32 jj = 0; jj < CASTLEMAXMONSTER; ++jj) msg.Push(castle.dwelling[jj]);
}

void Game::IO::PackHeroes(QueueMessage & msg, const Heroes & hero)
{
    msg.Push(static_cast<u8>(hero.portrait));
    msg.Push(static_cast<u8>(hero.race));

    msg.Push(hero.modes);
    msg.Push(static_cast<u8>(hero.color));
    msg.Push(hero.name);
    msg.Push(hero.attack);
    msg.Push(hero.defence);
    msg.Push(hero.knowledge);
    msg.Push(hero.power);
    msg.Push(hero.experience);
    msg.Push(hero.magic_point);
    msg.Push(hero.move_point);
    msg.Push(static_cast<u16>(hero.direction));
    msg.Push(hero.sprite_index);
    msg.Push(static_cast<u8>(hero.save_maps_general));
    msg.Push(static_cast<u16>(hero.mp.x));
    msg.Push(static_cast<u16>(hero.mp.y));

    // sec skills
    msg.Push(static_cast<u32>(hero.secondary_skills.size()));
    for(u32 jj = 0; jj < hero.secondary_skills.size(); ++jj)
    {
	const Skill::Secondary & sec = hero.secondary_skills[jj];
	msg.Push(static_cast<u8>(sec.Skill()));
	msg.Push(static_cast<u8>(sec.Level()));
    }

    // artifacts
    msg.Push(static_cast<u32>(hero.artifacts.size()));
    for(u32 jj = 0; jj < hero.artifacts.size(); ++jj)
    msg.Push(static_cast<u8>(hero.artifacts[jj].GetID()));

    // armies
    msg.Push(static_cast<u32>(ARMYMAXTROOPS));
    for(u32 jj = 0; jj < ARMYMAXTROOPS; ++jj)
    {
	const Army::Troop & troop = hero.army.At(jj);
	msg.Push(static_cast<u8>(troop()));
	msg.Push(troop.Count());
    }
	
    // spell book
    msg.Push(static_cast<u8>(hero.spell_book.active));
    {
	std::vector<Spell::spell_t>::const_iterator it1, it2;

	msg.Push(static_cast<u32>(hero.spell_book.spells.size()));

	it1 = hero.spell_book.spells.begin();
	it2 = hero.spell_book.spells.end();
	for(; it1 != it2; ++it1) msg.Push(static_cast<u8>(*it1));
    }
	
    // visit objects
    msg.Push(static_cast<u32>(hero.visit_object.size()));
    std::list<IndexObject>::const_iterator it1 = hero.visit_object.begin();
    std::list<IndexObject>::const_iterator it2 = hero.visit_object.end();
    for(; it1 != it2; ++it1)
    {
	msg.Push((*it1).first);
	msg.Push(static_cast<u8>((*it1).second));
    }
}

bool Game::IO::LoadBIN(QueueMessage & msg)
{
    Settings & conf = Settings::Get();

    u8 byte8;
    u16 byte16;
    u32 byte32;
    u16 format;
    std::string str;

    // prepare World
    world.Reset();

    msg.Pop(byte16);
    if(byte16 != 0xFF01){ Error::Warning("0xFF01"); return false; }

    // format version
    msg.Pop(format);
    if(format >= FORMAT_VERSION_1293)
    {
	// major version
	msg.Pop(byte8);
	// minor version
	msg.Pop(byte8);
    }

    // svn
    msg.Pop(str);
    // time
    msg.Pop(byte32);

    // maps
    msg.Pop(byte16);
    if(byte16 != 0xFF02) Error::Warning("0xFF02");
    msg.Pop(conf.current_maps_file.size_w);
    msg.Pop(conf.current_maps_file.size_h);
    msg.Pop(conf.current_maps_file.file);
    msg.Pop(conf.current_maps_file.difficulty);
    msg.Pop(conf.current_maps_file.kingdom_colors);
    msg.Pop(conf.current_maps_file.allow_colors);
    msg.Pop(conf.current_maps_file.rnd_colors);
    msg.Pop(conf.current_maps_file.rnd_races);
    msg.Pop(conf.current_maps_file.conditions_wins);
    msg.Pop(conf.current_maps_file.wins1);
    msg.Pop(conf.current_maps_file.wins2);
    msg.Pop(conf.current_maps_file.wins3);
    msg.Pop(conf.current_maps_file.wins4);
    msg.Pop(conf.current_maps_file.conditions_loss);
    msg.Pop(conf.current_maps_file.loss1);
    msg.Pop(conf.current_maps_file.loss2);
    // races
    msg.Pop(byte16);
    if(byte16 != 0xFF03) Error::Warning("0xFF03");
    msg.Pop(byte8); conf.current_maps_file.races[0] = byte8;
    msg.Pop(byte8); conf.current_maps_file.races[1] = byte8;
    msg.Pop(byte8); conf.current_maps_file.races[2] = byte8;
    msg.Pop(byte8); conf.current_maps_file.races[3] = byte8;
    msg.Pop(byte8); conf.current_maps_file.races[4] = byte8;
    msg.Pop(byte8); conf.current_maps_file.races[5] = byte8;
    // maps name
    msg.Pop(conf.current_maps_file.name);
    // maps description
    msg.Pop(conf.current_maps_file.description);
    // game
    msg.Pop(byte16);
    if(byte16 != 0xFF04) Error::Warning("0xFF04");
    msg.Pop(byte8); conf.game_difficulty = Difficulty::Get(byte8);
    msg.Pop(byte8); conf.my_color = Color::Get(byte8);
    msg.Pop(byte8); conf.cur_color = Color::Get(byte8);
    msg.Pop(conf.game_type);
    msg.Pop(conf.players_colors);
    msg.Pop(conf.preferably_count_players);
#ifdef WITH_DEBUG
    msg.Pop(byte8);
#else
    msg.Pop(conf.debug);
#endif
    // settings: original
    if(format >= FORMAT_VERSION_1293)
    {
	msg.Pop(byte8);
	if(byte8) conf.SetModes(Settings::ORIGINAL);
    }
    // world
    msg.Pop(byte16);
    if(byte16 != 0xFF05) Error::Warning("0xFF05");
    msg.Pop(world.width);
    msg.Pop(world.height);
    msg.Pop(world.ultimate_artifact);
    msg.Pop(world.uniq0);
    msg.Pop(byte8); world.week_name = Week::Get(byte8);
    msg.Pop(byte8); world.heroes_cond_wins = Heroes::ConvertID(byte8);
    msg.Pop(byte8); world.heroes_cond_loss = Heroes::ConvertID(byte8);
    msg.Pop(world.month);
    msg.Pop(world.week);
    msg.Pop(world.day);

    // tiles
    msg.Pop(byte16);
    if(byte16 != 0xFF06) Error::Warning("0xFF06");
    msg.Pop(byte32);
    world.vec_tiles.reserve(byte32);
    for(u32 maps_index = 0; maps_index < byte32; ++maps_index)
    {
	Maps::Tiles *tile = new Maps::Tiles(maps_index);

	msg.Pop(tile->tile_index);
	msg.Pop(tile->shape);
	msg.Pop(tile->general);
	msg.Pop(tile->quantity1);
	msg.Pop(tile->quantity2);
	msg.Pop(tile->fogs);
	msg.Pop(tile->flags);

#ifdef WITH_DEBUG
	tile->fogs &= ~conf.my_color;
#endif

	// addons 1
	u32 size;
	msg.Pop(size);
	for(u32 ii = 0; ii < size; ++ii)
	{
	    Maps::TilesAddon addon;
	    msg.Pop(addon.level);
	    msg.Pop(addon.uniq);
	    msg.Pop(addon.object);
	    msg.Pop(addon.index);
	    tile->addons_level1.push_back(addon);
	}

	// addons 2
	msg.Pop(size);
	for(u32 ii = 0; ii < size; ++ii)
	{
	    Maps::TilesAddon addon;
	    msg.Pop(addon.level);
	    msg.Pop(addon.uniq);
	    msg.Pop(addon.object);
	    msg.Pop(addon.index);
	    tile->addons_level2.push_back(addon);
	}

	world.vec_tiles.push_back(tile);
    }

    // heroes
    msg.Pop(byte16);
    if(byte16 != 0xFF07) Error::Warning("0xFF07");
    msg.Pop(byte32);
    world.vec_heroes.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Heroes* hero = new Heroes();
	UnpackHeroes(msg, *hero);
	world.vec_heroes.push_back(hero);
    }

    // castles
    msg.Pop(byte16);
    if(byte16 != 0xFF08) Error::Warning("0xFF08");
    msg.Pop(byte32);
    world.vec_castles.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Castle* castle = new Castle();
	UnpackCastle(msg, *castle);
	world.vec_castles.push_back(castle);
    }

    // kingdoms
    msg.Pop(byte16);
    if(byte16 != 0xFF09) Error::Warning("0xFF09");
    msg.Pop(byte32);
    world.vec_kingdoms.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Kingdom* kingdom = new Kingdom();
	UnpackKingdom(msg, *kingdom);
	world.vec_kingdoms.push_back(kingdom);
    }

    // signs
    msg.Pop(byte16);
    if(byte16 != 0xFF0A) Error::Warning("0xFF0A");
    msg.Pop(byte32);
    world.map_sign.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(byte16);
	msg.Pop(str);
	world.map_sign[byte16] = str;
    }

    // captured object
    msg.Pop(byte16);
    if(byte16 != 0xFF0B) Error::Warning("0xFF0B");
    msg.Pop(byte32);
    world.map_captureobj.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(byte16);
	std::pair<MP2::object_t, Color::color_t> & value = world.map_captureobj[byte16];
	msg.Pop(byte8); value.first = static_cast<MP2::object_t>(byte8);
	msg.Pop(byte8); value.second = Color::Get(byte8);
    }

    // rumors
    msg.Pop(byte16);
    if(byte16 != 0xFF0C) Error::Warning("0xFF0C");
    msg.Pop(byte32);
    world.vec_rumors.clear();
    world.vec_rumors.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(str);
	world.vec_rumors.push_back(str);
    }

    // day events
    msg.Pop(byte16);
    if(byte16 != 0xFF0D) Error::Warning("0xFF0D");
    msg.Pop(byte32);
    world.vec_eventsday.clear();
    world.vec_eventsday.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	GameEvent::Day *event = new GameEvent::Day();
	u32 size;

	msg.Pop(size); event->resource.wood = size;
	msg.Pop(size); event->resource.mercury = size;
	msg.Pop(size); event->resource.ore = size;
	msg.Pop(size); event->resource.sulfur = size;
	msg.Pop(size); event->resource.crystal = size;
	msg.Pop(size); event->resource.gems = size;
	msg.Pop(size); event->resource.gold = size;
	msg.Pop(byte8);event->computer = byte8;
	msg.Pop(event->first);
	msg.Pop(event->subsequent);
	msg.Pop(event->colors);
	msg.Pop(event->message);

	world.vec_eventsday.push_back(event);
    }

    // coord events
    msg.Pop(byte16);
    if(byte16 != 0xFF0E) Error::Warning("0xFF0E");
    msg.Pop(byte32);
    world.vec_eventsmap.clear();
    world.vec_eventsmap.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	GameEvent::Coord *event = new GameEvent::Coord();
	u32 size;

	msg.Pop(event->index_map);
	msg.Pop(size); event->resource.wood = size;
	msg.Pop(size); event->resource.mercury = size;
	msg.Pop(size); event->resource.ore = size;
	msg.Pop(size); event->resource.sulfur = size;
	msg.Pop(size); event->resource.crystal = size;
	msg.Pop(size); event->resource.gems = size;
	msg.Pop(size); event->resource.gold = size;

	msg.Pop(byte8);event->artifact = Artifact::FromInt(byte8);
	msg.Pop(byte8);event->computer = byte8;
	msg.Pop(byte8);event->cancel = byte8;
	msg.Pop(event->colors);
	msg.Pop(event->message);

	world.vec_eventsmap.push_back(event);
    }
    
    // sphinx riddles
    msg.Pop(byte16);
    if(byte16 != 0xFF0F) Error::Warning("0xFF0F");
    msg.Pop(byte32);
    world.vec_riddles.clear();
    world.vec_riddles.reserve(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	GameEvent::Riddle *riddle = new GameEvent::Riddle();
	u32 size;
	
	msg.Pop(riddle->index_map);
	msg.Pop(size); riddle->resource.wood = size;
	msg.Pop(size); riddle->resource.mercury = size;
	msg.Pop(size); riddle->resource.ore = size;
	msg.Pop(size); riddle->resource.sulfur = size;
	msg.Pop(size); riddle->resource.crystal = size;
	msg.Pop(size); riddle->resource.gems = size;
	msg.Pop(size); riddle->resource.gold = size;
	msg.Pop(byte8);riddle->artifact = Artifact::FromInt(byte8);
	msg.Pop(byte8);riddle->quiet = byte8;

	msg.Pop(size);
	riddle->answers.reserve(size);
	for(u32 jj = 0; jj < size; ++jj)
	{
	    msg.Pop(str);
	    riddle->answers.push_back(str);
	}
	msg.Pop(riddle->message);

	world.vec_riddles.push_back(riddle);
    }

    msg.Pop(byte16);

    // sort castles to kingdoms
    std::vector<Castle *>::const_iterator itc1 = world.vec_castles.begin();
    std::vector<Castle *>::const_iterator itc2 = world.vec_castles.end();
    for(; itc1 != itc2; ++itc1)
        if(*itc1) world.GetKingdom((*itc1)->GetColor()).AddCastle(*itc1);

    // sort heroes to kingdoms
    std::vector<Heroes *>::const_iterator ith1 = world.vec_heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = world.vec_heroes.end();
    for(; ith1 != ith2; ++ith1)
        if(*ith1) world.GetKingdom((*ith1)->GetColor()).AddHeroes(*ith1);

    // regenerate puzzle surface
    Interface::GameArea::GenerateUltimateArtifactAreaSurface(world.ultimate_artifact, world.puzzle_surface);

    return byte16 == 0xFFFF;
}

void Game::IO::UnpackKingdom(QueueMessage & msg, Kingdom & kingdom, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    msg.Pop(byte8);
    kingdom.color =Color::Get(byte8);

    msg.Pop(byte8);
    kingdom.control = Game::GetControl(byte8);

    msg.Pop(kingdom.flags);
    msg.Pop(kingdom.lost_town_days);
    msg.Pop(byte16);
    kingdom.ai_capital = byte16 ? world.GetCastle(byte16) : NULL;

    // funds
    msg.Pop(byte32); kingdom.resource.wood = byte32;
    msg.Pop(byte32); kingdom.resource.mercury = byte32;
    msg.Pop(byte32); kingdom.resource.ore = byte32;
    msg.Pop(byte32); kingdom.resource.sulfur = byte32;
    msg.Pop(byte32); kingdom.resource.crystal = byte32;
    msg.Pop(byte32); kingdom.resource.gems = byte32;
    msg.Pop(byte32); kingdom.resource.gold = byte32;

    // visit objects
    msg.Pop(byte32);
    kingdom.visit_object.clear();
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	IndexObject io;
	msg.Pop(io.first);
	msg.Pop(byte8);
	io.second = static_cast<MP2::object_t>(byte8);
	kingdom.visit_object.push_back(io);
    }

    // recruits
    msg.Pop(byte8); kingdom.recruits.SetID1(byte8);
    msg.Pop(byte8); kingdom.recruits.SetID2(byte8);

    std::string str;

    // puzzle
    msg.Pop(str);
    kingdom.puzzle_maps = str.c_str();

    // visited tents
    if(check_version && check_version >= FORMAT_VERSION_1335)
    {
	msg.Pop(kingdom.visited_tents_colors);
    }
}

void Game::IO::UnpackCastle(QueueMessage & msg, Castle & castle, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    msg.Pop(byte16); castle.mp.x = byte16;
    msg.Pop(byte16); castle.mp.y = byte16;

    msg.Pop(byte8); castle.race = Race::Get(byte8);

    msg.Pop(castle.modes);
    msg.Pop(byte8); castle.color = Color::Get(byte8);
    msg.Pop(castle.name);
    msg.Pop(castle.building);

    // mageguild
    castle.mageguild.spells.clear();
    msg.Pop(castle.mageguild.level);
    msg.Pop(byte8); castle.mageguild.upgrade = byte8;
    msg.Pop(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	castle.mageguild.spells.push_back(Spell::FromInt(byte8));
    }

    // armies
    msg.Pop(byte32);
    for(u32 jj = 0; jj < ARMYMAXTROOPS; ++jj)
    {
	msg.Pop(byte8);
	castle.army.At(jj).SetMonster(Monster::FromInt(byte8));
	msg.Pop(byte16);
	castle.army.At(jj).SetCount(byte16);
    }

    // dwelling
    msg.Pop(byte32);
    for(u32 jj = 0; jj < CASTLEMAXMONSTER; ++jj) msg.Pop(castle.dwelling[jj]);
}

void Game::IO::UnpackHeroes(QueueMessage & msg, Heroes & hero, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    msg.Pop(byte8); hero.portrait = Heroes::ConvertID(byte8);
    msg.Pop(byte8); hero.race = Race::Get(byte8);

    msg.Pop(hero.modes);
    msg.Pop(byte8); hero.color = Color::Get(byte8);
    msg.Pop(hero.name);
    msg.Pop(hero.attack);
    msg.Pop(hero.defence);
    msg.Pop(hero.knowledge);
    msg.Pop(hero.power);
    msg.Pop(hero.experience);
    msg.Pop(hero.magic_point);
    msg.Pop(hero.move_point);
    msg.Pop(byte16); hero.direction = Direction::FromInt(byte16);
    msg.Pop(hero.sprite_index);
    msg.Pop(byte8); hero.save_maps_general = static_cast<MP2::object_t>(byte8);
    msg.Pop(byte16); hero.mp.x = static_cast<s16>(byte16);
    msg.Pop(byte16); hero.mp.y = static_cast<s16>(byte16);

    // sec skills
    hero.secondary_skills.clear();
    hero.secondary_skills.reserve(HEROESMAXSKILL);
    msg.Pop(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	Skill::Secondary skill;
	msg.Pop(byte8); skill.SetSkill(Skill::Secondary::Skill(byte8));
	msg.Pop(byte8); skill.SetLevel(byte8);
	hero.secondary_skills.push_back(skill);
    }

    // artifacts
    std::fill(hero.artifacts.begin(), hero.artifacts.end(), Artifact::UNKNOWN);
    msg.Pop(byte32);
    for(u32 jj = 0; jj < hero.artifacts.size(); ++jj)
    {
	msg.Pop(byte8);
	hero.artifacts[jj].Set(Artifact::FromInt(byte8));
    }

    // armies
    msg.Pop(byte32);
    for(u32 jj = 0; jj < ARMYMAXTROOPS; ++jj)
    {
	msg.Pop(byte8);
	hero.army.At(jj).SetMonster(Monster::FromInt(byte8));
	msg.Pop(byte16);
	hero.army.At(jj).SetCount(byte16);
    }

    // spell book
    hero.spell_book.spells.clear();
    msg.Pop(byte8);
    hero.spell_book.active = byte8;

    msg.Pop(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	hero.spell_book.spells.push_back(Spell::FromInt(byte8));
    }

    // visit objects
    msg.Pop(byte32);
    hero.visit_object.clear();
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	IndexObject io;
	msg.Pop(io.first);
	msg.Pop(byte8);
	io.second = static_cast<MP2::object_t>(byte8);
	hero.visit_object.push_back(io);
    }
}
