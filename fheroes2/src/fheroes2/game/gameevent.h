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
#ifndef H2GAMEEVENT_H
#define H2GAMEEVENT_H

#include <vector>
#include <string>
#include "resource.h"
#include "artifact.h"
#include "position.h"
#include "gamedefs.h"

struct EventDate
{
    EventDate() : computer(false), first(0), subsequent(0), colors(0) {}
    EventDate(const void *ptr);

    bool isAllow(u8 color, u16 date) const;
    bool isDeprecated(u16 date) const;

    Funds resource;
    bool computer;
    u16 first;
    u16 subsequent;
    u8 colors;
    std::string message;
};

struct EventMaps : public Maps::Position
{
    EventMaps() : computer(false), cancel(false), colors(0) {}
    EventMaps(s32 index, const void *ptr);
    
    bool isAllow(u8 color, s32 index) const;
    void SetVisited(u8 color);

    Funds resource;
    Artifact artifact;
    bool computer;
    bool cancel;
    u8 colors;
    std::string message;
};

typedef std::list<std::string>    RiddleAnswers;

struct Riddle : public Maps::Position
{
    Riddle() : valid(false) {}
    Riddle(s32 index, const void *ptr);

    bool AnswerCorrect(const std::string & answer);
    void SetQuiet(void);

    Funds resource;
    Artifact artifact;
    RiddleAnswers answers;
    std::string message;
    bool valid;
};

#endif
