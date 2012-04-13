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

static u16 SAV2ID = 0xFF02;

namespace Game
{
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

	info << GetString(GetLoadVersion()) << GetLoadVersion() <<
		HeaderSAV(conf.CurrentFileInfo(), conf.PriceLoyaltyVersion());
	gdata << GetLoadVersion() << World::Get() << Settings::Get() <<
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

	    SetLoadVersion(binver);
	    u16 end_check = 0;

	    if(GetLoadVersion() < FORMAT_VERSION_2830)
	    {
		gdata >> Settings::Get() >> World::Get() >> GameOver::Result::Get() >>
		    GameStatic::Data::Get() >> MonsterStaticData::Get() >> end_check;
	    }
	    else
	    {
		gdata >> World::Get() >> Settings::Get() >> GameOver::Result::Get() >>
		    GameStatic::Data::Get() >> MonsterStaticData::Get() >> end_check;
	    }

	    if(end_check == SAV2ID)
	    {
		SetLoadVersion(CURRENT_FORMAT_VERSION);
		result = true;
	    }
	    else
	    {
		DEBUG(DBG_GAME, DBG_WARN, "invalid load file: " << fn);
	    }
	}
	else
	    result = false;
    }

    if(result)
    {
	Settings & conf = Settings::Get();
	Game::SetLastSavename(fn);
	conf.SetGameType(conf.GameType() | Game::TYPE_LOADFILE);
    }

    return result;
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

	return false;
    }

    return false;
}

