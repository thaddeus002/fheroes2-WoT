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

#ifndef H2MAGEGUILD_H
#define H2MAGEGUILD_H

#include "gamedefs.h"
#include "game_io.h"
#include "spell_storage.h"

class Castle;
class HeroBase;

class MageGuild
{
    public:
	MageGuild() {};

	void Builds(u8 race, bool libraryCap);
	void EducateHero(HeroBase &, u8 lvlmage, bool isLibraryBuild) const;

	SpellStorage GetSpells(u8 lvlmage, bool islibrary, u8) const;

    private:
	friend class Game::IO;

	SpellStorage general;
	SpellStorage library;
};

class RowSpells
{
public:
    RowSpells(const Point &, const Castle &, u8);
    void Redraw(void);
    bool QueueEventProcessing(void);

private:
    std::vector<Rect> coords;
    SpellStorage spells;
};

#endif
