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
#ifndef H2CONFIG_H
#define H2CONFIG_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>

#include "error.h"
#include "cmdline.h"
#include "tools.h"
#include "maps.h"
#include "gamedefs.h"

namespace H2Config {

    typedef enum {
	DEBUG		= 0x0001,
	SOUND		= 0x0002,
	MUSIC		= 0x0004,
	ANIMATION	= 0x0008,
	FULLSCREEN	= 0x0010,
	EVILINTERFACE	= 0x0020,
	RLEDEBUG	= 0x8000,
    } bool_t;

    void Init(const std::string & filename = "");

    bool Debug(void);
    bool RLEDebug(void);
    bool Sound(void);
    bool Music(void);
    bool Animation(void);
    bool FullScreen(void);
    bool EvilInterface(void);
    const std::string & GetAGGFile(void);
    const std::string & GetMapsDirectory(void);
    Display::resolution_t VideoMode(void);

    Maps::difficulty_t GetGameDifficulty(void);
    Maps::color_t GetHumanColor(void);
    u8 GetKingdomColors(void);
    u8 GetKingdomCount(void);
    u8 GetAllowChangeColors(void);
    Maps::race_t GetKingdomRace(Maps::color_t color);
    u8 GetAllowChangeRaces(void);
    const std::string & GetFileMaps(void);
    const std::string & GetNameMaps(void);
    const std::string & GetDescriptionMaps(void);

    void SetDebug(void);
    void SetAGGFile(const std::string & path);
    void SetGameDifficulty(Maps::difficulty_t difficulty);
    void SetHumanColor(Maps::color_t color);
    void SetKingdomColors(u8 colors);
    void SetAllowChangeColors(u8 colors);
    void SetKingdomCount(u8 count);
    void SetKingdomRace(Maps::color_t color, Maps::race_t race);
    void SetAllowChangeRaces(u8 races);
    void SetFileMaps(const std::string & file);
    void SetNameMaps(const std::string & name);
    void SetDescriptionMaps(const std::string & description);
};

#endif
