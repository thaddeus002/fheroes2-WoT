/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
 *   <afletdinov@mail.dc.baikal.ru>                                        *
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

#ifndef H2BATTLEARENA_H
#define H2BATTLEARENA_H

#include <vector>
#include "surface.h"
#include "gamedefs.h"
#include "army.h"

class ArenaTiles;

class BattleArena
{
public:
    enum direction_t
    {
	CENTER,
	TOPLEFT,
	TOPRIGHT,
	LEFT,
	RIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT
    };

    BattleArena(const u16, Army::army_t &, Army::army_t &);
    ~BattleArena();

    bool GameOver(void) const;

    void Turns(const u16);
    void HumanTurn(const u8);
    void AITurn(const u8);

    u8 GetResult(void);


protected:
    Army::army_t & army1;
    Army::army_t & army2;

    const u8 color1;
    const u8 color2;

    const u8 control1;
    const u8 control2;

    const bool graphics;

    std::vector<ArenaTiles *> maps;
    Surface arena;

    u8 result;
};

#endif
