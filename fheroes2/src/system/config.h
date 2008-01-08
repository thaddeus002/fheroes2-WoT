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

#include <string>
#include "difficulty.h"
#include "maps.h"
#include "display.h"
#include "color.h"
#include "kingdom.h"
#include "race.h"
#include "gamedefs.h"

namespace H2Config {

    typedef enum {
	DEBUG		= 0x0001,
	SOUND		= 0x0002,
	MUSIC		= 0x0004,
	ANIMATION	= 0x0008,
	FULLSCREEN	= 0x0010,
	EVILINTERFACE	= 0x0020,
	SHADOW		= 0x0040,
	/*
	...
	*/
	STARTHEROES	= 0x1000,
	LOGO		= 0x2000,
	ORIGINAL	= 0x4000,
	EDITOR		= 0x8000,
    } bool_t;

    void Defaults(void);
    bool Load(const std::string & filename);
    bool Save(const std::string & filename);

    bool Debug(void);
    bool Editor(void);
    bool Original(void);
    bool Sound(void);
    bool Music(void);
    bool Animation(void);
    bool FullScreen(void);
    bool EvilInterface(void);
    bool Shadow(void);
    bool PlayWithHeroes(void);
    bool Logo(void);
    const std::string & GetAGGFile(void);
    const std::string & GetMapsDirectory(void);
    Display::resolution_t GetVideoMode(void);

    Difficulty::difficulty_t GetGameDifficulty(void);
    Color::color_t GetMyColor(void);
    u8 GetKingdomColors(void);
    u8 GetKingdomCount(void);
    u8 GetAllowChangeColors(void);
    Race::race_t GetKingdomRace(Color::color_t color);
    u8 GetAllowChangeRaces(void);
    const std::string & GetFileMaps(void);
    const std::string & GetNameMaps(void);
    const std::string & GetDescriptionMaps(void);
    Maps::mapsize_t GetSizeMaps(void);

    void SetDebug(void);
    void SetEditor(void);
    void SetOriginal(void);
    void SetAGGFile(const std::string & path);
    void SetGameDifficulty(Difficulty::difficulty_t difficulty);
    void SetInterface(bool interface);
    void SetPlayWithHeroes(bool fl);
    void SetHumanColor(Color::color_t color);
    void SetKingdomColors(u8 colors);
    void SetAllowChangeColors(u8 colors);
    void SetKingdomCount(u8 count);
    void SetKingdomRace(Color::color_t color, Race::race_t race);
    void SetAllowChangeRaces(u8 races);
    void SetFileMaps(const std::string & file);
    void SetNameMaps(const std::string & name);
    void SetDescriptionMaps(const std::string & description);
    void SetSizeMaps(Maps::mapsize_t size);
    
    void PrintCurrentValues(void);
};

#endif
