/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2AI_SIMPLE_H
#define H2AI_SIMPLE_H

#include <map>
#include <list>
#include <vector>

#include "pairs.h"
#include "ai.h"

struct IndexObjectMap : public std::map<s32, MP2::object_t>
{
    void DumpObjects(const IndexDistance & id);
};

struct AIKingdom
{
    AIKingdom() : capital(NULL) {};
    void Reset(void);

    Castle*         capital;
    IndexObjectMap  scans;
};

class AIKingdoms : public std::vector<AIKingdom>
{
public:
    static AIKingdom & Get(Color::color_t);
    static void Reset(void);

private:
    static AIKingdoms & Get(void);
    AIKingdoms() : std::vector<AIKingdom>(KINGDOMMAX + 1) {};
};

struct Queue : public std::list<s32>
{
    bool isPresent(s32) const;
};

struct AIHero
{
    AIHero() : primary_target(-1), fix_loop(0) {};

    void ClearTasks(void) { sheduled_visit.clear(); }
    void Reset(void);

    Queue           sheduled_visit;
    s32             primary_target;
    u8              fix_loop;
};

struct AIHeroes : public std::vector<AIHero>
{
public:
    static AIHero & Get(const Heroes &);
    static void Reset(void);

private:
    static AIHeroes & Get(void);
    AIHeroes() : std::vector<AIHero>(HEROESMAXCOUNT + 2) {};
};

#endif
