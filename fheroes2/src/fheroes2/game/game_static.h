/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2GAMESTATIC_H
#define H2GAMESTATIC_H

#include "gamedefs.h"

struct cost_t;

namespace GameStatic
{
    u8		GetLostOnWhirlpoolPercent(void);
    u8		GetGameOverLostDays(void);
    u8		GetOverViewDistance(u8);

    cost_t &	GetKingdomStartingResource(u8);
    u8		GetKingdomMaxHeroes(void);

    u8		GetCastleGrownWell(void);
    u8		GetCastleGrownWel2(void);
    u8		GetCastleGrownWeekOf(void);
    u8		GetCastleGrownMonthOf(void);

    u8		GetHeroesSpellPointsPerDay(void);

    double	GetMonsterUpgradeRatio(void);
}

#ifdef WITH_XML
#include "xmlccwrap.h"
namespace Game
{
    void	CastleUpdateGrowth(const TiXmlElement*);
    void	KingdomUpdateStartingResource(const TiXmlElement*);
    void	HeroesUpdateStatic(const TiXmlElement*);
    void	KingdomUpdateStatic(const TiXmlElement*);
    void	GameOverUpdateStatic(const TiXmlElement*);
    void	OverViewUpdateStatic(const TiXmlElement*);
    void	WhirlpoolUpdateStatic(const TiXmlElement*);
    void	MonsterUpdateStatic(const TiXmlElement*);
}
#endif

#endif
