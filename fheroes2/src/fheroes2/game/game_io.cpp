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
#include "settings.h"
#include "tools.h"

std::string Game::IO::last_name;

bool Game::Save(const std::string &fn)
{
    DEBUG(DBG_GAME, DBG_INFO, fn);
    const bool autosave = std::string::npos != fn.find("autosave.sav");

    if(Settings::Get().ExtRewriteConfirm() && FilePresent(fn) &&
	(!autosave || Settings::Get().ExtAutosaveConfirm()) &&
	Dialog::NO == Dialog::Message("", _("Are you sure you want to overwrite the save with this name?"), Font::BIG, Dialog::YES|Dialog::NO))
    {
	return false;
    }

    Game::IO msg;

    if(! Game::IO::SaveBIN(msg)) return false;

#ifdef WITH_ZLIB
    std::ofstream fs(fn.c_str(), std::ios::binary);
    if(!fs.is_open()) return false;
    std::vector<char> v;
    if(!ZLib::Compress(v, msg.DtPt(), msg.DtSz())) return false;
    fs.write(&v[0], v.size());
    fs.close();
#else
    msg.Save(fn.c_str());
#endif

    if(! autosave) Game::IO::last_name = fn;

    return true;
}

bool Game::IO::LoadSAV(const std::string & fn)
{
    if(fn.empty()) return false;

    Load(fn.c_str());

#ifdef WITH_ZLIB
    std::vector<char> v;
    if(ZLib::UnCompress(v, data, dtsz))
    {
	dtsz = v.size();
	delete [] data;
        data = new char [dtsz + 1];
	itd1 = data;
	itd2 = data + dtsz;

	std::memcpy(data, &v[0], dtsz);
	v.clear();
    }
#endif

    return true;
}

bool Game::Load(const std::string & fn)
{
    DEBUG(DBG_GAME, DBG_INFO, fn);

    // loading info
    Game::ShowLoadMapsText();

    Game::IO msg;
    if(!msg.LoadSAV(fn) || !Game::IO::LoadBIN(msg)) return false;

    Game::IO::last_name = fn;
    Settings & conf = Settings::Get();

    conf.SetGameType(conf.GameType() | Game::TYPE_LOADFILE);

    return true;
}

bool Game::LoadSAV2FileInfo(const std::string & fn,  Maps::FileInfo & maps_file)
{
    Game::IO msg;

    if(!msg.LoadSAV(fn)) return false;

    u8 byte8;
    u16 byte16, version;
    u32 byte32;
    std::string str;

    maps_file.file = fn;

    msg.Pop(byte16);
    if(byte16 != 0xFF01) return false;
    // format version
    msg.Pop(version);
    // major version
    msg.Pop(byte8);
    // minor version
    msg.Pop(byte8);
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
    msg.Pop(maps_file.human_colors);
    msg.Pop(maps_file.computer_colors);
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
    // lang
    msg.Push(conf.force_lang);

    // maps
    msg.Push(static_cast<u16>(0xFF02));
    msg.Push(conf.current_maps_file.size_w);
    msg.Push(conf.current_maps_file.size_h);
    msg.Push(GetBasename(conf.current_maps_file.file));
    msg.Push(conf.current_maps_file.difficulty);
    msg.Push(conf.current_maps_file.kingdom_colors);
    msg.Push(conf.current_maps_file.human_colors);
    msg.Push(conf.current_maps_file.computer_colors);
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
    msg.Push(static_cast<u32>(KINGDOMMAX));
    for(u32 ii = 0; ii < KINGDOMMAX; ++ii)
	msg.Push(conf.current_maps_file.races[ii]);
    // unions
    msg.Push(static_cast<u32>(KINGDOMMAX));
    for(u32 ii = 0; ii < KINGDOMMAX; ++ii)
	msg.Push(conf.current_maps_file.unions[ii]);
    // maps name
    msg.Push(conf.current_maps_file.name);
    // maps description
    msg.Push(conf.current_maps_file.description);
    // game
    msg.Push(static_cast<u16>(0xFF04));
    msg.Push(conf.game_difficulty);
    msg.Push(static_cast<u8>(conf.my_color));
    msg.Push(static_cast<u8>(conf.cur_color));
    msg.Push(conf.game_type);
    msg.Push(conf.players_colors);
    msg.Push(conf.preferably_count_players);
    msg.Push(conf.debug);

    msg.Push(conf.opt_game());
    msg.Push(conf.opt_world());
    msg.Push(conf.opt_battle());

    // world
    msg.Push(static_cast<u16>(0xFF05));
    msg.Push(world.width);
    msg.Push(world.height);
    msg.Push(world.ultimate_index);
    msg.Push(world.uniq0);
    msg.Push(world.week_current.first);
    msg.Push(world.week_current.second);
    msg.Push(world.week_next.first);
    msg.Push(world.week_next.second);
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
	//if(NULL == world.vec_tiles[ii]){ DEBUG(DBG_GAME, DBG_WARN, "tiles: " << "is NULL"); return false; }
	PackTile(msg, world.vec_tiles[ii]);
    }

    // heroes
    msg.Push(static_cast<u16>(0xFF07));
    msg.Push(static_cast<u32>(world.vec_heroes.size()));
    for(u32 ii = 0; ii < world.vec_heroes.size(); ++ii)
    {
	//if(NULL == world.vec_heroes[ii]){ DEBUG(DBG_GAME, DBG_WARN, "heroes: " << "is NULL"); return false; }
	PackHeroes(msg, *world.vec_heroes[ii]);
    }

    // castles
    msg.Push(static_cast<u16>(0xFF08));
    msg.Push(static_cast<u32>(world.vec_castles.size()));
    for(u32 ii = 0; ii < world.vec_castles.size(); ++ii)
    {
	//if(NULL == world.vec_castles[ii]){ DEBUG(DBG_GAME, DBG_WARN, "castles: " << "is NULL"); return false; }
	PackCastle(msg, *world.vec_castles[ii]);
    }

    // kingdoms
    msg.Push(static_cast<u16>(0xFF09));
    msg.Push(static_cast<u32>(world.vec_kingdoms.size()));
    for(u32 ii = 0; ii < world.vec_kingdoms.size(); ++ii)
    {
	//if(NULL == world.vec_kingdoms[ii]){ DEBUG(DBG_GAME, DBG_WARN, "kingdoms: " << "is NULL"); return false; }
	PackKingdom(msg, world.vec_kingdoms.kingdoms[ii]);
    }

    // signs
    msg.Push(static_cast<u16>(0xFF0A));
    msg.Push(static_cast<u32>(world.map_sign.size()));
    {
        for(std::map<s32, std::string>::const_iterator
	    it = world.map_sign.begin(); it != world.map_sign.end(); ++it)
	{
	    msg.Push((*it).first);
	    msg.Push((*it).second);
	}
    }

    // captured object
    msg.Push(static_cast<u16>(0xFF0B));
    msg.Push(static_cast<u32>(world.map_captureobj.size()));
    {
	for(std::map<s32, ObjectColor>::const_iterator
	    it = world.map_captureobj.begin(); it != world.map_captureobj.end(); ++it)
	{
	    msg.Push((*it).first);
	    msg.Push(static_cast<u8>((*it).second.first));
	    msg.Push(static_cast<u8>((*it).second.second));
	}
    }

    // rumors
    msg.Push(static_cast<u16>(0xFF0C));
    msg.Push(static_cast<u32>(world.vec_rumors.size()));
    {
	for(Rumors::const_iterator
	    it = world.vec_rumors.begin(); it != world.vec_rumors.end(); ++it)
	    msg.Push(*it);
    }

    // day events
    msg.Push(static_cast<u16>(0xFF0D));
    msg.Push(static_cast<u32>(world.vec_eventsday.size()));
    {
	for(EventsDate::const_iterator
	    it = world.vec_eventsday.begin(); it != world.vec_eventsday.end(); ++it)
	{
	    msg.Push((*it).resource.wood);
	    msg.Push((*it).resource.mercury);
	    msg.Push((*it).resource.ore);
	    msg.Push((*it).resource.sulfur);
	    msg.Push((*it).resource.crystal);
	    msg.Push((*it).resource.gems);
	    msg.Push((*it).resource.gold);
	    msg.Push((*it).computer);
	    msg.Push((*it).first);
	    msg.Push((*it).subsequent);
	    msg.Push((*it).colors);
	    msg.Push((*it).message);
	}
    }

    // coord events
    msg.Push(static_cast<u16>(0xFF0E));
    msg.Push(static_cast<u32>(world.vec_eventsmap.size()));
    {
	for(EventsMaps::const_iterator
	    it = world.vec_eventsmap.begin(); it != world.vec_eventsmap.end(); ++it)
	{
	    msg.Push((*it).GetIndex());
	    msg.Push((*it).resource.wood);
	    msg.Push((*it).resource.mercury);
	    msg.Push((*it).resource.ore);
	    msg.Push((*it).resource.sulfur);
	    msg.Push((*it).resource.crystal);
	    msg.Push((*it).resource.gems);
	    msg.Push((*it).resource.gold);
	    msg.Push((*it).artifact());
	    msg.Push((*it).computer);
	    msg.Push((*it).cancel);
	    msg.Push((*it).colors);
	    msg.Push((*it).message);
	}
    }

    // sphinx riddles
    msg.Push(static_cast<u16>(0xFF0F));
    msg.Push(static_cast<u32>(world.vec_riddles.size()));
    {
	for(Riddles::const_iterator
	    it = world.vec_riddles.begin(); it != world.vec_riddles.end(); ++it)
	{
	    msg.Push((*it).GetIndex());
	    msg.Push((*it).resource.wood);
	    msg.Push((*it).resource.mercury);
	    msg.Push((*it).resource.ore);
	    msg.Push((*it).resource.sulfur);
	    msg.Push((*it).resource.crystal);
	    msg.Push((*it).resource.gems);
	    msg.Push((*it).resource.gold);
	    msg.Push((*it).artifact());
	    msg.Push((*it).valid);

	    msg.Push(static_cast<u32>((*it).answers.size()));
	    for(RiddleAnswers::const_iterator
		ita = (*it).answers.begin(); ita != (*it).answers.end(); ++ita)
		msg.Push(*ita);

	    msg.Push((*it).message);
	}
    }

    msg.Push(static_cast<u16>(0xFFFF));
    return true;
}

void Game::IO::PackTile(QueueMessage & msg, const Maps::Tiles & tile)
{
    msg.Push(tile.tile_sprite_index);
    msg.Push(tile.tile_sprite_shape);
    msg.Push(tile.mp2_object);
    msg.Push(tile.quantity1);
    msg.Push(tile.quantity2);
    msg.Push(tile.quantity3);
    msg.Push(tile.quantity4);
    msg.Push(tile.fogs);
    msg.Push(tile.quantity5);
    msg.Push(tile.quantity6);
    msg.Push(tile.quantity7);

    // addons 1
    PackTileAddons(msg, tile.addons_level1);
    // addons 2
    PackTileAddons(msg, tile.addons_level2);
}

void Game::IO::PackTileAddons(QueueMessage & msg, const Maps::Addons & addons)
{
    msg.Push(static_cast<u8>(addons.size()));
    for(Maps::Addons::const_iterator
	it = addons.begin(); it != addons.end(); ++it)
    {
	msg.Push((*it).level);
	msg.Push((*it).uniq);
	msg.Push((*it).object);
	msg.Push((*it).index);
    }
}

void Game::IO::PackKingdom(QueueMessage & msg, const Kingdom & kingdom)
{
    msg.Push(static_cast<u8>(kingdom.color));

    msg.Push(kingdom.control);
    msg.Push(kingdom.flags);
    msg.Push(kingdom.lost_town_days);
    // unused
    msg.Push(static_cast<u16>(0));

    // funds
    msg.Push(kingdom.resource.wood);
    msg.Push(kingdom.resource.mercury);
    msg.Push(kingdom.resource.ore);
    msg.Push(kingdom.resource.sulfur);
    msg.Push(kingdom.resource.crystal);
    msg.Push(kingdom.resource.gems);
    msg.Push(kingdom.resource.gold);

    // visit objects
    msg.Push(static_cast<u32>(kingdom.visit_object.size()));
    for(std::list<IndexObject>::const_iterator
	it = kingdom.visit_object.begin(); it != kingdom.visit_object.end(); ++it)
    {
	msg.Push((*it).first);
	msg.Push(static_cast<u8>((*it).second));
    }

    // recruits
    msg.Push(static_cast<u8>(kingdom.recruits.GetID1()));
    msg.Push(static_cast<u8>(kingdom.recruits.GetID2()));

    const Puzzle & pzl = kingdom.puzzle_maps;

    // puzzle
    msg.Push(pzl.to_string<char,std::char_traits<char>,std::allocator<char> >());

    // puzzle orders
    msg.Push(static_cast<u32>(sizeof(pzl.zone1_order)));
    for(size_t ii = 0; ii < sizeof(pzl.zone1_order); ++ii) msg.Push(pzl.zone1_order[ii]);
    msg.Push(static_cast<u32>(sizeof(pzl.zone2_order)));
    for(size_t ii = 0; ii < sizeof(pzl.zone2_order); ++ii) msg.Push(pzl.zone2_order[ii]);
    msg.Push(static_cast<u32>(sizeof(pzl.zone3_order)));
    for(size_t ii = 0; ii < sizeof(pzl.zone3_order); ++ii) msg.Push(pzl.zone3_order[ii]);
    msg.Push(static_cast<u32>(sizeof(pzl.zone4_order)));
    for(size_t ii = 0; ii < sizeof(pzl.zone4_order); ++ii) msg.Push(pzl.zone4_order[ii]);

    // tents colors
    msg.Push(kingdom.visited_tents_colors);
}

void Game::IO::PackCastle(QueueMessage & msg, const Castle & castle)
{
    msg.Push(castle.GetCenter().x);
    msg.Push(castle.GetCenter().y);
    msg.Push(castle.race);

    msg.Push(castle.modes);
    msg.Push(static_cast<u8>(castle.color));
    msg.Push(castle.name);
    msg.Push(castle.building);
	
    // mageguild
    {
	msg.Push(static_cast<u32>(castle.mageguild.general.size()));

	for(SpellStorage::const_iterator
	    it = castle.mageguild.general.begin(); it != castle.mageguild.general.end(); ++it)
	    msg.Push((*it)());

	msg.Push(static_cast<u32>(castle.mageguild.library.size()));

	for(SpellStorage::const_iterator
	    it = castle.mageguild.library.begin(); it != castle.mageguild.library.end(); ++it)
	    msg.Push((*it)());
    }

    // armies
    msg.Push(static_cast<u32>(castle.army.Size()));
    for(u32 jj = 0; jj < castle.army.Size(); ++jj)
    {
	const Army::Troop & troop = castle.army.At(jj);
	msg.Push(static_cast<u8>(troop()));
	msg.Push(troop.GetCount());
    }

    // dwelling
    msg.Push(static_cast<u32>(CASTLEMAXMONSTER));
    for(u32 jj = 0; jj < CASTLEMAXMONSTER; ++jj) msg.Push(castle.dwelling[jj]);

    // captain
    PackHeroBase(msg, castle.captain);
}

void Game::IO::PackHeroBase(QueueMessage & msg, const HeroBase & hero)
{
    // primary
    msg.Push(hero.attack);
    msg.Push(hero.defense);
    msg.Push(hero.knowledge);
    msg.Push(hero.power);

    // position
    msg.Push(hero.center.x);
    msg.Push(hero.center.y);

    // modes
    msg.Push(hero.modes);

    // hero base
    msg.Push(hero.magic_point);
    msg.Push(hero.move_point);

    // spell book
    msg.Push(static_cast<u32>(hero.spell_book.size()));

    for(SpellBook::const_iterator
	it = hero.spell_book.begin(); it != hero.spell_book.end(); ++it)
	msg.Push((*it)());

    // artifacts
    msg.Push(static_cast<u32>(hero.bag_artifacts.size()));
    for(u32 jj = 0; jj < hero.bag_artifacts.size(); ++jj)
    {
	msg.Push(hero.bag_artifacts[jj].id);
	msg.Push(hero.bag_artifacts[jj].ext);
    }
}

void Game::IO::PackHeroes(QueueMessage & msg, const Heroes & hero)
{
    PackHeroBase(msg, hero);

    msg.Push(static_cast<u8>(hero.hid));
    msg.Push(static_cast<u8>(hero.portrait));
    msg.Push(hero.race);

    msg.Push(static_cast<u8>(hero.color));
    msg.Push(hero.name);
    msg.Push(hero.experience);
    msg.Push(hero.direction);
    msg.Push(hero.sprite_index);
    msg.Push(static_cast<u8>(hero.save_maps_object));
    msg.Push(hero.patrol_center.x);
    msg.Push(hero.patrol_center.y);
    msg.Push(hero.patrol_square);

    // sec skills
    msg.Push(static_cast<u32>(hero.secondary_skills.size()));
    for(u32 jj = 0; jj < hero.secondary_skills.size(); ++jj)
    {
	const Skill::Secondary & sec = hero.secondary_skills[jj];
	msg.Push(static_cast<u8>(sec.Skill()));
	msg.Push(static_cast<u8>(sec.Level()));
    }

    // armies
    msg.Push(static_cast<u32>(hero.army.Size()));
    for(u32 jj = 0; jj < hero.army.Size(); ++jj)
    {
	const Army::Troop & troop = hero.army.At(jj);
	msg.Push(static_cast<u8>(troop()));
	msg.Push(troop.GetCount());
    }
	
    // visit objects
    msg.Push(static_cast<u32>(hero.visit_object.size()));
    for(std::list<IndexObject>::const_iterator
	it = hero.visit_object.begin(); it != hero.visit_object.end(); ++it)
    {
	msg.Push((*it).first);
	msg.Push(static_cast<u8>((*it).second));
    }

    // route path
    msg.Push(hero.path.dst);
    msg.Push(static_cast<u8>(hero.path.hide));
    msg.Push(static_cast<u32>(hero.path.size()));
    for(Route::Path::const_iterator
	ip = hero.path.begin(); ip != hero.path.end(); ++ip)
    {
	msg.Push((*ip).from);
	msg.Push((*ip).direction);
	msg.Push((*ip).penalty);
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
    if(byte16 != 0xFF01){ DEBUG(DBG_GAME, DBG_WARN, "0xFF01"); return false; }

    // format version
    msg.Pop(format);
    if(format > CURRENT_FORMAT_VERSION || format < LAST_FORMAT_VERSION)
    {
	std::ostringstream os;
	os << "usupported save format: " << format << std::endl <<
	    "game version: " << CURRENT_FORMAT_VERSION << std::endl <<
	    "last version: " << LAST_FORMAT_VERSION;
	Dialog::Message("Error", os.str(), Font::BIG, Dialog::OK);
    	return false;
    }
    else
    {
    	DEBUG(DBG_GAME , DBG_INFO, "format: " << format);
    }


    // major version
    msg.Pop(byte8);
    // minor version
    msg.Pop(byte8);

    // svn
    msg.Pop(conf.svn_version);
    // time
    msg.Pop(byte32);
    // lang
    msg.Pop(str);
    if(str != "en" && str != conf.force_lang && !conf.Unicode())
    {
	std::string msg("This is an saved game is localized for lang = ");
	msg.append(str);
	msg.append(", and most of the messages will be displayed incorrectly.\n \n");
	msg.append("(tip: set unicode = on)");
	Dialog::Message("Warning!", msg, Font::BIG, Dialog::OK);
    }

    // maps
    msg.Pop(byte16);
    if(byte16 != 0xFF02) DEBUG(DBG_GAME, DBG_WARN, "0xFF02");

    msg.Pop(conf.current_maps_file.size_w);
    msg.Pop(conf.current_maps_file.size_h);
    msg.Pop(conf.current_maps_file.file);
    msg.Pop(conf.current_maps_file.difficulty);
    msg.Pop(conf.current_maps_file.kingdom_colors);
    msg.Pop(conf.current_maps_file.human_colors);
    msg.Pop(conf.current_maps_file.computer_colors);
    msg.Pop(conf.current_maps_file.rnd_races);
    msg.Pop(conf.current_maps_file.conditions_wins);
    msg.Pop(conf.current_maps_file.wins1);
    msg.Pop(conf.current_maps_file.wins2);
    msg.Pop(conf.current_maps_file.wins3);
    msg.Pop(conf.current_maps_file.wins4);
    msg.Pop(conf.current_maps_file.conditions_loss);
    msg.Pop(conf.current_maps_file.loss1);
    msg.Pop(conf.current_maps_file.loss2);

    msg.Pop(byte16);
    if(byte16 != 0xFF03) DEBUG(DBG_GAME, DBG_WARN, "0xFF03");
    // races
    msg.Pop(byte32);
    for(u8 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(byte8);
	conf.current_maps_file.races[ii] = byte8;
    }
    // unions
    msg.Pop(byte32);
    for(u16 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(byte8);
	conf.current_maps_file.unions[ii] = byte8;
    }
    // maps name
    msg.Pop(conf.current_maps_file.name);
    // maps description
    msg.Pop(conf.current_maps_file.description);
    // game
    msg.Pop(byte16);
    if(byte16 != 0xFF04) DEBUG(DBG_GAME, DBG_WARN, "0xFF04");
    msg.Pop(conf.game_difficulty);
    msg.Pop(byte8); conf.my_color = Color::Get(byte8);
    msg.Pop(byte8); conf.cur_color = Color::NONE;
    msg.Pop(conf.game_type);
    msg.Pop(conf.players_colors);
    msg.Pop(conf.preferably_count_players);
#ifdef WITH_DEBUG
    msg.Pop(byte16);
#else
    msg.Pop(conf.debug);
#endif
    msg.Pop(byte32);
    // skip load interface options
    //conf.opt_game.ResetModes(MODES_ALL);
    //conf.opt_game.SetModes(byte32);
    msg.Pop(byte32);
    conf.opt_world.ResetModes(MODES_ALL);
    conf.opt_world.SetModes(byte32);
    msg.Pop(byte32);
    conf.opt_battle.ResetModes(MODES_ALL);
    conf.opt_battle.SetModes(byte32);

    // world
    msg.Pop(byte16);
    if(byte16 != 0xFF05) DEBUG(DBG_GAME, DBG_WARN, "0xFF05");
    msg.Pop(world.width);
    msg.Pop(world.height);

    msg.Pop(world.ultimate_index);
    msg.Pop(world.uniq0);

    if(format < FORMAT_VERSION_2371)
    {
	msg.Pop(byte8); world.week_current = byte8;
	world.week_next = Week::WeekRand();
    }
    else
    {
	if(format < FORMAT_VERSION_2379)
	{
	    msg.Pop(world.week_current.first);
	    msg.Pop(world.week_current.second);
	    world.week_next = Week::WeekRand();
	}
	else
	{
	    msg.Pop(world.week_current.first);
	    msg.Pop(world.week_current.second);
	    msg.Pop(world.week_next.first);
	    msg.Pop(world.week_next.second);
	}
    }

    msg.Pop(byte8); world.heroes_cond_wins = Heroes::ConvertID(byte8);
    msg.Pop(byte8); world.heroes_cond_loss = Heroes::ConvertID(byte8);
    msg.Pop(world.month);
    msg.Pop(world.week);
    msg.Pop(world.day);

    // tiles
    msg.Pop(byte16);
    if(byte16 != 0xFF06) DEBUG(DBG_GAME, DBG_WARN, "0xFF06");
    msg.Pop(byte32);
    world.vec_tiles.resize(byte32);
    for(u32 maps_index = 0; maps_index < byte32; ++maps_index)
    {
	world.vec_tiles[maps_index].maps_index = maps_index;
	UnpackTile(msg, world.vec_tiles[maps_index], format);
    }

    // heroes
    msg.Pop(byte16);
    if(byte16 != 0xFF07) DEBUG(DBG_GAME, DBG_WARN, "0xFF07");
    msg.Pop(byte32);
    world.vec_heroes.Init();

    if(world.vec_heroes.size() < byte32)
    {
	VERBOSE("heroes: " << "incorrect size: " << byte32);
	return false;
    }
    else
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	UnpackHeroes(msg, *world.vec_heroes[ii], format);
    }

    // castles
    msg.Pop(byte16);
    if(byte16 != 0xFF08) DEBUG(DBG_GAME, DBG_WARN, "0xFF08");
    msg.Pop(byte32);
    world.vec_castles.Init();

    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Castle* castle = new Castle();
	UnpackCastle(msg, *castle, format);
	world.vec_castles.push_back(castle);
    }

    // kingdoms
    msg.Pop(byte16);
    if(byte16 != 0xFF09) DEBUG(DBG_GAME, DBG_WARN, "0xFF09");
    msg.Pop(byte32);
    world.vec_kingdoms.Init();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	//Kingdom* kingdom = new Kingdom();
	UnpackKingdom(msg, world.vec_kingdoms.kingdoms[ii], format);
	//world.vec_kingdoms.push_back(kingdom);
    }

    // signs
    msg.Pop(byte16);
    if(byte16 != 0xFF0A) DEBUG(DBG_GAME, DBG_WARN, "0xFF0A");
    msg.Pop(byte32);
    byte16 = byte32;
    world.map_sign.clear();
    for(u16 ii = 0; ii < byte16; ++ii)
    {
	msg.Pop(byte32);
	msg.Pop(str);
	world.map_sign[byte32] = str;
    }

    // captured object
    msg.Pop(byte16);
    if(byte16 != 0xFF0B) DEBUG(DBG_GAME, DBG_WARN, "0xFF0B");
    msg.Pop(byte32);
    byte16 = byte32;
    world.map_captureobj.clear();
    for(u16 ii = 0; ii < byte16; ++ii)
    {
	msg.Pop(byte32);
	ObjectColor & value = world.map_captureobj[byte32];
	msg.Pop(byte8); value.first = static_cast<MP2::object_t>(byte8);
	msg.Pop(byte8); value.second = Color::Get(byte8);
    }

    // rumors
    msg.Pop(byte16);
    if(byte16 != 0xFF0C) DEBUG(DBG_GAME, DBG_WARN, "0xFF0C");
    msg.Pop(byte32);
    world.vec_rumors.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	msg.Pop(str);
	world.vec_rumors.push_back(str);
    }

    // day events
    msg.Pop(byte16);
    if(byte16 != 0xFF0D) DEBUG(DBG_GAME, DBG_WARN, "0xFF0D");
    msg.Pop(byte32);
    world.vec_eventsday.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	EventDate event;

	msg.Pop(event.resource.wood);
	msg.Pop(event.resource.mercury);
	msg.Pop(event.resource.ore);
	msg.Pop(event.resource.sulfur);
	msg.Pop(event.resource.crystal);
	msg.Pop(event.resource.gems);
	msg.Pop(event.resource.gold);
	msg.Pop(event.computer);
	msg.Pop(event.first);
	msg.Pop(event.subsequent);
	msg.Pop(event.colors);
	msg.Pop(event.message);

	world.vec_eventsday.push_back(event);
    }

    // coord events
    msg.Pop(byte16);
    if(byte16 != 0xFF0E) DEBUG(DBG_GAME, DBG_WARN, "0xFF0E");
    msg.Pop(byte32);
    world.vec_eventsmap.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	EventMaps event;

	s32 index;
	msg.Pop(index); event.SetIndex(index);

	msg.Pop(event.resource.wood);
	msg.Pop(event.resource.mercury);
	msg.Pop(event.resource.ore);
	msg.Pop(event.resource.sulfur);
	msg.Pop(event.resource.crystal);
	msg.Pop(event.resource.gems);
	msg.Pop(event.resource.gold);

	msg.Pop(byte8); event.artifact = byte8;
	msg.Pop(event.computer);
	msg.Pop(event.cancel);
	msg.Pop(event.colors);
	msg.Pop(event.message);

	world.vec_eventsmap.push_back(event);
    }
    
    // sphinx riddles
    msg.Pop(byte16);
    if(byte16 != 0xFF0F) DEBUG(DBG_GAME, DBG_WARN, "0xFF0F");
    msg.Pop(byte32);
    world.vec_riddles.clear();
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Riddle riddle;
	s32 index;
	u32 size;

	msg.Pop(index); riddle.SetIndex(index);
	msg.Pop(riddle.resource.wood);
	msg.Pop(riddle.resource.mercury);
	msg.Pop(riddle.resource.ore);
	msg.Pop(riddle.resource.sulfur);
	msg.Pop(riddle.resource.crystal);
	msg.Pop(riddle.resource.gems);
	msg.Pop(riddle.resource.gold);
	msg.Pop(byte8); riddle.artifact = byte8;
	msg.Pop(riddle.valid);

	msg.Pop(size);
	for(u32 jj = 0; jj < size; ++jj)
	{
	    msg.Pop(str);
	    riddle.answers.push_back(str);
	}
	msg.Pop(riddle.message);

	world.vec_riddles.push_back(riddle);
    }

    msg.Pop(byte16);

    // add castles to kingdoms
    world.vec_kingdoms.AddCastles(world.vec_castles);

    // add heroes to kingdoms
    world.vec_kingdoms.AddHeroes(world.vec_heroes);

    // regenerate puzzle surface
    Interface::GameArea::GenerateUltimateArtifactAreaSurface(world.ultimate_index, world.puzzle_surface);

    return byte16 == 0xFFFF;
}

void Game::IO::UnpackTile(QueueMessage & msg, Maps::Tiles & tile, u16 check_version)
{
    msg.Pop(tile.tile_sprite_index);
    msg.Pop(tile.tile_sprite_shape);
    msg.Pop(tile.mp2_object);
    msg.Pop(tile.quantity1);
    msg.Pop(tile.quantity2);
    msg.Pop(tile.quantity3);
    msg.Pop(tile.quantity4);
    msg.Pop(tile.fogs);
    msg.Pop(tile.quantity5);
    msg.Pop(tile.quantity6);
    msg.Pop(tile.quantity7);

#ifdef WITH_DEBUG
    if(IS_DEVEL()) tile.fogs &= ~Settings::Get().MyColor();
#endif

    // addons 1
    UnpackTileAddons(msg, tile.addons_level1, check_version);
    // addons 2
    UnpackTileAddons(msg, tile.addons_level2, check_version);
}

void Game::IO::UnpackTileAddons(QueueMessage & msg, Maps::Addons & addons, u16 check_version)
{
    addons.clear();
    u8 size;
    msg.Pop(size);
    for(u8 ii = 0; ii < size; ++ii)
    {
	Maps::TilesAddon addon;
	msg.Pop(addon.level);
	msg.Pop(addon.uniq);
	msg.Pop(addon.object);
	msg.Pop(addon.index);
	addons.push_back(addon);
    }
}

void Game::IO::UnpackKingdom(QueueMessage & msg, Kingdom & kingdom, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    msg.Pop(byte8);
    kingdom.color = Color::Get(byte8);

    msg.Pop(kingdom.control);
    msg.Pop(kingdom.flags);
    msg.Pop(kingdom.lost_town_days);
    // unused
    msg.Pop(byte16);

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
    Puzzle & pzl = kingdom.puzzle_maps;
    msg.Pop(str);
    pzl = str.c_str();

    // puzzle orders
    u32 size;
    msg.Pop(size);
    for(u32 ii = 0; ii < size; ++ii) msg.Pop(pzl.zone1_order[ii]);
    msg.Pop(size);
    for(u32 ii = 0; ii < size; ++ii) msg.Pop(pzl.zone2_order[ii]);
    msg.Pop(size);
    for(u32 ii = 0; ii < size; ++ii) msg.Pop(pzl.zone3_order[ii]);
    msg.Pop(size);
    for(u32 ii = 0; ii < size; ++ii) msg.Pop(pzl.zone4_order[ii]);

    // visited tents
    msg.Pop(kingdom.visited_tents_colors);
}

void Game::IO::UnpackCastle(QueueMessage & msg, Castle & castle, u16 check_version)
{
    u8 byte8;
    //u16 byte16;
    u32 byte32;

    msg.Pop(castle.center.x);
    msg.Pop(castle.center.y);

    msg.Pop(castle.race);

    msg.Pop(castle.modes);
    msg.Pop(byte8); castle.color = Color::Get(byte8);
    msg.Pop(castle.name);
    msg.Pop(castle.building);

    // general
    msg.Pop(byte32);
    castle.mageguild.general.clear();
    castle.mageguild.general.reserve(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	castle.mageguild.general.push_back(Spell(byte8));
    }
    // library
    msg.Pop(byte32);
    castle.mageguild.library.clear();
    castle.mageguild.library.reserve(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	castle.mageguild.library.push_back(Spell(byte8));
    }

    // armies
    msg.Pop(byte32);
    for(u32 jj = 0; jj < castle.army.Size(); ++jj)
    {
	msg.Pop(byte8);
	castle.army.At(jj).SetMonster(Monster(byte8));
        msg.Pop(byte32);
        castle.army.At(jj).SetCount(byte32);
    }
    // dwelling
    msg.Pop(byte32);
    for(u32 jj = 0; jj < CASTLEMAXMONSTER; ++jj) msg.Pop(castle.dwelling[jj]);

    // captain
    UnpackHeroBase(msg, castle.captain, check_version);
}

void Game::IO::UnpackHeroBase(QueueMessage & msg, HeroBase & hero, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    // primary
    msg.Pop(hero.attack);
    msg.Pop(hero.defense);
    msg.Pop(hero.knowledge);
    msg.Pop(hero.power);

    // position
    msg.Pop(byte16); hero.center.x = byte16;
    msg.Pop(byte16); hero.center.y = byte16;

    // modes
    msg.Pop(hero.modes);

    // hero base
    msg.Pop(hero.magic_point);
    msg.Pop(hero.move_point);

    // spell book
    hero.spell_book.clear();

    msg.Pop(byte32);
    hero.spell_book.reserve(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	hero.spell_book.push_back(Spell(byte8));
    }

    // artifacts
    msg.Pop(byte32);
    hero.bag_artifacts.clear();
    hero.bag_artifacts.reserve(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	Artifact art;
	msg.Pop(art.id);
	msg.Pop(art.ext);
	hero.bag_artifacts.push_back(art);
    }
}

void Game::IO::UnpackHeroes(QueueMessage & msg, Heroes & hero, u16 check_version)
{
    u8 byte8;
    //u16 byte16;
    u32 byte32;

    UnpackHeroBase(msg, hero, check_version);

    // hid
    msg.Pop(byte8); hero.hid = Heroes::ConvertID(byte8);
    msg.Pop(byte8); hero.portrait = Heroes::ConvertID(byte8);
    // race
    msg.Pop(hero.race);

    msg.Pop(byte8); hero.color = Color::Get(byte8);
    msg.Pop(hero.name);

    msg.Pop(hero.experience);
    msg.Pop(hero.direction);
    msg.Pop(hero.sprite_index);
    msg.Pop(byte8); hero.save_maps_object = static_cast<MP2::object_t>(byte8);

    msg.Pop(hero.patrol_center.x);
    msg.Pop(hero.patrol_center.y);
    msg.Pop(hero.patrol_square);

    // sec skills
    hero.secondary_skills.clear();
    hero.secondary_skills.reserve(HEROESMAXSKILL);
    msg.Pop(byte32);
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	u8 skill, level;
	msg.Pop(skill);
	msg.Pop(level);
	hero.secondary_skills.push_back(Skill::Secondary(skill, level));
    }

    // armies
    msg.Pop(byte32);
    for(u32 jj = 0; jj < hero.army.Size(); ++jj)
    {
	    msg.Pop(byte8);
	    hero.army.At(jj).SetMonster(Monster(byte8));
    	    msg.Pop(byte32);
	    hero.army.At(jj).SetCount(byte32);
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

    // route path
    msg.Pop(hero.path.dst);
    msg.Pop(byte8);
    hero.path.hide = byte8;
    msg.Pop(byte32);
    hero.path.clear();

    for(u32 jj = 0; jj < byte32; ++jj)
    {
	Route::Step step;
	if(FORMAT_VERSION_2293 <= check_version)
	    msg.Pop(step.from);

	msg.Pop(step.direction);
	msg.Pop(step.penalty);
	hero.path.push_back(step);
    }

    // check path
    if(hero.path.isBroken()) hero.path.Reset();
}
