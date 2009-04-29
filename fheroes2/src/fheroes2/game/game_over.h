/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifndef H2GAMEOVER_H
#define H2GAMEOVER_H

#include <string>

namespace GameOverConditions
{
    enum wins_t
    {
	WINS_ALL	= 0x01,
	WINS_TOWN	= 0x02,
	WINS_HERO	= 0x04,
	WINS_ARTIFACT	= 0x08,
	WINS_SIDE	= 0x10,
	WINS_GOLD	= 0x20,
    };

    enum loss_t
    {
	LOSS_ALL	= 0x01,
	LOSS_TOWN	= 0x02,
	LOSS_HERO	= 0x04,
	LOSS_TIME	= 0x08,
    };

    const std::string & Wins(wins_t);
    const std::string & Loss(loss_t);
};

#endif
