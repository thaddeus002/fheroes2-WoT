/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#ifndef H2MAPS_OBJECTS_H
#define H2MAPS_OBJECTS_H

#include <vector>
#include <string>
#include "resource.h"
#include "artifact.h"
#include "position.h"
#include "gamedefs.h"

struct MapEvent : public ObjectSimple, public MapPosition
{
    MapEvent();
    MapEvent(s32 index, const u8*, size_t);

    bool	isAllow(int color) const;
    void	SetVisited(int color);

    Funds	resource;
    Artifact	artifact;
    bool	computer;
    bool	cancel;
    int		colors;
    std::string message;
};

StreamBase & operator<< (StreamBase &, const MapEvent &);
StreamBase & operator>> (StreamBase &, MapEvent &);

typedef std::list<std::string>    RiddleAnswers;

struct MapSphinx : public ObjectSimple, public MapPosition
{
    MapSphinx();
    MapSphinx(s32 index, const u8*, size_t);

    bool	AnswerCorrect(const std::string & answer);
    void	SetQuiet(void);

    Funds		resource;
    Artifact		artifact;
    RiddleAnswers	answers;
    std::string		message;
    bool		valid;
};

StreamBase & operator<< (StreamBase &, const MapSphinx &);
StreamBase & operator>> (StreamBase &, MapSphinx &);

struct MapSign : public ObjectSimple, public MapPosition
{
    MapSign();
    MapSign(s32 index, const char*);
    MapSign(s32 index, const u8*, size_t);

    std::string		message;
};

StreamBase & operator<< (StreamBase &, const MapSign &);
StreamBase & operator>> (StreamBase &, MapSign &);

/*
struct MapArtifact : public ObjectSimple, public MapPosition
{
    MapArtifact(){}
};
*/

#endif
