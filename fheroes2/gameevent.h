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
#ifndef H2GAMEEVENT_H
#define H2GAMEEVENT_H

#include <vector>
#include <string>
#include "dialog.h"
#include "config.h"
#include "resource.h"
#include "artifact.h"
#include "error.h"
#include "gamedefs.h"

namespace GameEvent
{

class Sign
{
public:
    Sign(u16 index, const char *str) : index_map(index), message(str){ if(H2Config::Debug()) Error::Verbose("add Sign: " + message); };

    u16 GetIndex(void) const{ return index_map; };
    const std::string & GetText(void) const{ return message; };
    
    void Show(void){ Dialog::Message("", message, Font::BIG, Dialog::OK); };

private:
    u16 index_map;
    std::string message;
};

class Day
{
public:
    Day(const void *ptr){ if(H2Config::Debug()) Error::Verbose("add EventDay"); };

private:
    Resource::funds_t resource;
    Artifact::artifact_t artifact;
    bool computer;
    u16 first;
    u8 subsequent;
    u8 colors;
    std::string message;
};

class Coord
{
public:
    Coord(u16 index, const void *ptr) : index_map(index){ if(H2Config::Debug()) Error::Verbose("add EventMaps"); };

private:
    u16 index_map;
    Resource::funds_t resource;
    Artifact::artifact_t artifact;
    bool computer;
    bool cancel;
    u8 colors;
    std::string message;
};

class Riddle
{
public:
    Riddle(u16 index, const void *ptr) : index_map(index){ if(H2Config::Debug()) Error::Verbose("add Sphinx riddle"); };

private:
    u16 index_map;
    Resource::funds_t resource;
    Artifact::artifact_t artifact;
    std::vector<std::string> answers;
    std::string message;
};

};
#endif
