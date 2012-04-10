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
#include "game_over.h"
#include "game_io.h"
#include "game_static.h"
#include "game_focus.h"
#include "monster.h"

static u16 SAV1ID = 0xFF01;
static u16 SAV2ID = 0xFF02;

namespace Game
{
    bool LoadSAV2FileInfoOld(const std::string & fn,  Maps::FileInfo & maps_file);
    bool LoadOld(const std::string & fn);

    struct HeaderSAV
    {
	enum { IS_COMPRESS = 0x8000, IS_LOYALTY = 0x4000 };

	HeaderSAV() : status(0)
	{
	}

	HeaderSAV(const Maps::FileInfo & fi, bool loyalty) : status(0), info(fi)
	{
	    time_t rawtime;
	    std::time(&rawtime);
	    info.localtime = rawtime;

	    if(loyalty)
		status |= IS_LOYALTY;

#ifdef WITH_ZLIB
	    status |= IS_COMPRESS;
#endif
	}

	u16		status;
	Maps::FileInfo	info;
    };

    StreamBase & operator<< (StreamBase & msg, const HeaderSAV & hdr)
    {
	return msg << hdr.status << hdr.info;
    }

    StreamBase & operator>> (StreamBase & msg, HeaderSAV & hdr)
    {
	return msg >> hdr.status >> hdr.info;
    }
}

bool Game::Save(const std::string &fn)
{
    DEBUG(DBG_GAME, DBG_INFO, fn);
    const bool autosave = (GetBasename(fn) == "autosave.sav");
    const Settings & conf = Settings::Get();

    // ask overwrite?
    if(IsFile(fn) &&
	((!autosave && conf.ExtGameRewriteConfirm()) || (autosave && Settings::Get().ExtGameAutosaveConfirm())) &&
	Dialog::NO == Dialog::Message("", _("Are you sure you want to overwrite the save with this name?"), Font::BIG, Dialog::YES|Dialog::NO))
    {
	return false;
    }

    std::ofstream fs(fn.c_str(), std::ios::binary);

    if(fs.is_open())
    {
	StreamBuf info(1024);
	StreamBuf gdata((Maps::MEDIUM < conf.MapsWidth() ? 1024 :512) * 1024);
	if(! autosave) Game::SetLastSavename(fn);

	info << GetString(GetSaveVersion()) << GetSaveVersion() <<
		HeaderSAV(conf.CurrentFileInfo(), conf.PriceLoyaltyVersion());
	gdata << GetSaveVersion() << Settings::Get() << World::Get() <<
	    GameOver::Result::Get() << GameStatic::Data::Get() << MonsterStaticData::Get() << SAV2ID; // eof marker

	fs << static_cast<char>(SAV2ID >> 8) << static_cast<char>(SAV2ID) << info;

#ifdef WITH_ZLIB
	ZStreamBuf zdata;
	zdata << gdata;

	if(! zdata.fail())
	    fs << zdata;
	else
	    fs << gdata;
#else
	fs << gdata;
#endif

	return fs.good();
    }

    return false;
}

bool Game::Load(const std::string & fn)
{
    DEBUG(DBG_GAME, DBG_INFO, fn);
    bool result = false;
    const Settings & conf = Settings::Get();
    // loading info
    Game::ShowLoadMapsText();

    std::ifstream fs(fn.c_str(), std::ios::binary);

    if(fs.is_open())
    {
	char major, minor;
	fs >> std::noskipws >> major >> minor;
	const u16 savid = (static_cast<u16>(major) << 8) | static_cast<u16>(minor);

	// check version sav file
	if(savid == SAV2ID)
	{
	    std::string strver;
	    u16 binver = 0;
	    StreamBuf hinfo(1024);
	    StreamBuf gdata((Maps::MEDIUM < conf.MapsWidth() ? 1024 :512) * 1024);
	    HeaderSAV header;

	    fs >> hinfo;

	    if(hinfo.fail())
	    {
		DEBUG(DBG_GAME, DBG_INFO, fn << ", hinfo" << " read: error");
		return false;
	    }

	    hinfo >> strver >> binver >> header;

#ifndef WITH_ZLIB
	    if(header.status & HeaderSAV::IS_COMPRESS)
	    {
		DEBUG(DBG_GAME, DBG_INFO, fn << ", zlib: unsupported");
		return false;
	    }
	    else
#else
	    if(header.status & HeaderSAV::IS_COMPRESS)
	    {
		ZStreamBuf zdata;
		fs >> zdata;

		if(zdata.fail())
		{
		    DEBUG(DBG_GAME, DBG_INFO, fn << ", zdata" << " read: error");
		    return false;
		}

		zdata >> gdata;

		if(gdata.fail())
		{
		    DEBUG(DBG_GAME, DBG_INFO, ", uncompress: error");
		    return false;
		}
	    }
	    else
#endif
	    {
		fs >> gdata;

		if(gdata.fail())
		{
		    DEBUG(DBG_GAME, DBG_INFO, fn << ", gdata" << " read: error");
		    return false;
		}
	    }

	    if((header.status & HeaderSAV::IS_LOYALTY) &&
		!conf.PriceLoyaltyVersion())
	    {
		Dialog::Message("Warning", _("This file is saved in the \"Price Loyalty\" version.\nSome items may be unavailable."), Font::BIG, Dialog::OK);
	    }

	    gdata >> binver;

	    // check version: false
	    if(binver > CURRENT_FORMAT_VERSION || binver < LAST_FORMAT_VERSION)
	    {
		std::ostringstream os;
		os << "usupported save format: " << binver << std::endl <<
     		"game version: " << CURRENT_FORMAT_VERSION << std::endl <<
     		"last version: " << LAST_FORMAT_VERSION;
 		Dialog::Message("Error", os.str(), Font::BIG, Dialog::OK);
 		return false;
	    }

	    u16 oldver = GetSaveVersion();
	    SetSaveVersion(binver);

	    u16 end_check = 0;

	    gdata >> Settings::Get() >> World::Get() >> GameOver::Result::Get() >>
		    GameStatic::Data::Get() >> MonsterStaticData::Get() >> end_check;

	    if(end_check == SAV2ID)
	    {
		SetSaveVersion(binver);
		result = true;
	    }
	    else
	    {
		SetSaveVersion(oldver);
		DEBUG(DBG_GAME, DBG_WARN, "invalid load file: " << fn);
	    }
	}
	else
	    result = LoadOld(fn);
    }

    if(result)
    {
	Settings & conf = Settings::Get();
	Game::SetLastSavename(fn);
	conf.SetGameType(conf.GameType() | Game::TYPE_LOADFILE);
    }

    return result;
}

bool Game::IOld::LoadSAV(const std::string & fn)
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

bool Game::LoadOld(const std::string & fn)
{
    Game::IOld msg;
    return !msg.LoadSAV(fn) || !Game::IOld::LoadBIN(msg) ? false : true;
}

bool Game::LoadSAV2FileInfo(const std::string & fn,  Maps::FileInfo & finfo)
{
    std::ifstream fs(fn.c_str(), std::ios::binary);

    if(fs.is_open())
    {
	char major, minor;
	fs >> std::noskipws >> major >> minor;
	const u16 savid = (static_cast<u16>(major) << 8) | static_cast<u16>(minor);

	// check version sav file
	if(savid == SAV2ID)
	{
	    HeaderSAV header;
	    StreamBuf hinfo(1024);
	    std::string strver;
	    u16 binver;

	    fs >> hinfo;

	    if(hinfo.fail())
	    {
		DEBUG(DBG_GAME, DBG_INFO, fn << ", hinfo" << " read: error");
		return false;
	    }

	    hinfo >> strver >> binver >> header;

	    // hide: unsupported version
	    if(binver > CURRENT_FORMAT_VERSION || binver < LAST_FORMAT_VERSION)
		return false;

#ifndef WITH_ZLIB
	    // check: compress game data
	    if(header.status & IS_COMPRESS)
	    {
		DEBUG(DBG_GAME, DBG_INFO, fn << ", zlib: unsupported");
		return false;
	    }
#endif

	    finfo = header.info;
	    finfo.file = fn;

	    return true;
	}

	return LoadSAV2FileInfoOld(fn, finfo);
    }

    return false;
}

bool Game::LoadSAV2FileInfoOld(const std::string & fn,  Maps::FileInfo & maps_file)
{
    Game::IOld msg;

    if(!msg.LoadSAV(fn)) return false;

    u8 byte8;
    u16 byte16, version;
    //u32 byte32;
    std::string str;

    maps_file.file = fn;

    msg.Pop(byte16);
    if(byte16 != SAV1ID) return false;
    // format version
    msg.Pop(version);

    // major version
    msg.Pop(byte8);
    // minor version
    msg.Pop(byte8);
    // svn
    msg.Pop(str);

    // time
    msg.Pop(maps_file.localtime);

    // maps
    msg.Pop(byte16);
    msg.Pop(maps_file.size_w);
    msg.Pop(maps_file.size_h);
    msg.Pop(str);
    msg.Pop(maps_file.difficulty);
    msg.Pop(maps_file.kingdom_colors);
    msg.Pop(maps_file.allow_human_colors);
    msg.Pop(maps_file.allow_comp_colors);
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

void Game::IOld::UnpackPlayers(QueueMessage & msg, Players & players, u16 version)
{
    // players
    u8 byte8;
    u32 byte32;
    std::vector<Player*> vec1;
    std::vector<Player*> & vec2 = players;

    msg.Pop(byte8);
    players.Init(byte8);
    msg.Pop(byte32);
    for(u32 ii = 0; ii < byte32; ++ii)
    {
	Player player; u8 mode;

	msg.Pop(player.color);
	msg.Pop(player.id);
	msg.Pop(player.control);
	msg.Pop(player.race);
	msg.Pop(player.friends);
	msg.Pop(mode);
	msg.Pop(player.name);

	if(mode) player.SetPlay(true);

	Player* ptr = players.Get(player.color);
	if(ptr) *ptr = player;

	vec1.push_back(ptr);
    }

    msg.Pop(players.current_color);

    std::swap(vec1, vec2);
}

bool Game::IOld::LoadBIN(QueueMessage & msg)
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
    if(byte16 != SAV1ID){ DEBUG(DBG_GAME, DBG_WARN, "0xFF01"); return false; }

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

    // version
    msg.Pop(str);

    DEBUG(DBG_GAME, DBG_INFO, "version: " << str);

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
    msg.Pop(conf.current_maps_file.allow_human_colors);
    msg.Pop(conf.current_maps_file.allow_comp_colors);
    msg.Pop(conf.current_maps_file.rnd_races);
    msg.Pop(conf.current_maps_file.conditions_wins);
    msg.Pop(conf.current_maps_file.wins1);
    msg.Pop(conf.current_maps_file.wins2);
    msg.Pop(conf.current_maps_file.wins3);
    msg.Pop(conf.current_maps_file.wins4);
    msg.Pop(conf.current_maps_file.conditions_loss);
    msg.Pop(conf.current_maps_file.loss1);
    msg.Pop(conf.current_maps_file.loss2);

    if(! conf.PriceLoyaltyVersion() && 4 < conf.current_maps_file.file.size())
    {
	std::string lower = conf.current_maps_file.file;
	String::Lower(lower);

	if(".mx2" == lower.substr(lower.size() - 4))
	{
	    Dialog::Message("Warning!", "This is an saved file used the \"Price of Loyalty\" extension.\nAnd it will work for you with errors!", Font::BIG, Dialog::OK);
	}
    }

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
    msg.Pop(conf.game_type);
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

    UnpackPlayers(msg, conf.GetPlayers(), format);

    // world
    msg.Pop(byte16);
    if(byte16 != 0xFF05) DEBUG(DBG_GAME, DBG_WARN, "0xFF05");
    msg.Pop(world.width);
    msg.Pop(world.height);

    msg.Pop(byte32); // world.uniq0

    msg.Pop(world.week_current.first);
    msg.Pop(world.week_current.second);
    msg.Pop(world.week_next.first);
    msg.Pop(world.week_next.second);

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
	UnpackTile(msg, world.vec_tiles[maps_index], maps_index, format);

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
	CapturedObject & co = world.map_captureobj[byte32];
	msg.Pop(co.objcol.first);
	msg.Pop(co.objcol.second);

	msg.Pop(byte8);
	msg.Pop(byte32);
	co.guardians.Set(byte8, byte32);
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

    // ultimate
    s32 ultimate_index = -1;
    u8  ultimate_art = Artifact::UNKNOWN;

    msg.Pop(byte16);
    if(byte16 != 0xFF10) DEBUG(DBG_GAME, DBG_WARN, "0xFF10");
    msg.Pop(ultimate_art);
    msg.Pop(world.ultimate_artifact.isfound);
    msg.Pop(ultimate_index);

    // game over
    GameOver::Result & gameover = GameOver::Result::Get();

    if(format >= FORMAT_VERSION_2777)
    {
	msg.Pop(gameover.colors);
	msg.Pop(gameover.result);
	msg.Pop(gameover.continue_game);
    }
    else
	gameover.Reset();

    // end 0xFFFF
    msg.Pop(byte16);

    // add castles to kingdoms
    world.vec_kingdoms.AddCastles(world.vec_castles);

    // add heroes to kingdoms
    world.vec_kingdoms.AddHeroes(world.vec_heroes);

    // update tile passable
    std::for_each(world.vec_tiles.begin(), world.vec_tiles.end(),
        std::mem_fun_ref(&Maps::Tiles::UpdatePassable));

    // init ultimate art
    if(0 <= ultimate_index)
	world.ultimate_artifact.Set(ultimate_index, Artifact(ultimate_art));
    else
	world.ultimate_artifact.Reset();

    // heroes postfix
    for(u32 ii = 0; ii < world.vec_heroes.size(); ++ii)
    {
	Heroes* hero = world.vec_heroes[ii];

	if(hero)
	{
	    if(format < FORMAT_VERSION_2707)
		hero->path.Reset();

	    hero->path.RescanPassable();
	}
    }

    return byte16 == 0xFFFF;
}

extern u16 PackTileSpriteIndex(u16, u16);

void Game::IOld::UnpackTile(QueueMessage & msg, Maps::Tiles & tile, u32 maps_index, u16 check_version)
{
    msg.Pop(tile.pack_maps_index);
    msg.Pop(tile.pack_sprite_index);
    msg.Pop(tile.mp2_object);
    msg.Pop(tile.quantity1);
    msg.Pop(tile.quantity2);

    msg.Pop(tile.fog_colors);
    msg.Pop(tile.tile_passable);

#ifdef WITH_DEBUG
    if(IS_DEVEL()) tile.fog_colors &= ~Players::HumanColors();
#endif

    // addons 1
    UnpackTileAddons(msg, tile.addons_level1, check_version);
    // addons 2
    UnpackTileAddons(msg, tile.addons_level2, check_version);

    tile.FixObject();
}

void Game::IOld::UnpackTileAddons(QueueMessage & msg, Maps::Addons & addons, u16 check_version)
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
	msg.Pop(addon.tmp);
	addons.push_back(addon);
    }
}

void Game::IOld::UnpackKingdom(QueueMessage & msg, Kingdom & kingdom, u16 check_version)
{
    u8 byte8;
    u16 byte16;
    u32 byte32;

    // kingdom: color, modes
    msg.Pop(kingdom.color);
    msg.Pop(kingdom.modes);
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

    // lost_hero
    msg.Pop(byte8); kingdom.lost_hero.first = static_cast<Heroes::heroes_t>(byte8);
    msg.Pop(kingdom.lost_hero.second);

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

    // conditions loss
    msg.Pop(byte32);
    kingdom.heroes_cond_loss.clear();
    for(u32 jj = 0; jj < byte32; ++jj)
    {
	msg.Pop(byte8);
	kingdom.heroes_cond_loss.push_back(world.GetHeroes(static_cast<Heroes::heroes_t>(byte8)));
    }
}

void Game::IOld::UnpackCastle(QueueMessage & msg, Castle & castle, u16 check_version)
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
        msg.Pop(byte32);
	Troop* troop = castle.army.GetTroop(jj);
	if(troop) troop->Set(Monster(byte8), byte32);
    }
    castle.army.SetColor(castle.color);

    // dwelling
    msg.Pop(byte32);
    for(u32 jj = 0; jj < CASTLEMAXMONSTER; ++jj) msg.Pop(castle.dwelling[jj]);

    // captain
    UnpackHeroBase(msg, castle.captain, check_version);

    if(castle.isBuild(BUILD_CAPTAIN))
	castle.army.SetCommander(&castle.captain);
}

void Game::IOld::UnpackHeroBase(QueueMessage & msg, HeroBase & hero, u16 check_version)
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

void Game::IOld::UnpackHeroes(QueueMessage & msg, Heroes & hero, u16 check_version)
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
    msg.Pop(hero.save_maps_object);

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
    	msg.Pop(byte32);
	Troop* troop = hero.army.GetTroop(jj);
	if(troop) troop->Set(Monster(byte8), byte32);
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

	msg.Pop(step.from);
	msg.Pop(step.direction);
	msg.Pop(step.penalty);
	hero.path.push_back(step);
    }
}
