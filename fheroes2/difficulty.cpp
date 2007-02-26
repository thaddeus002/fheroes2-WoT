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

#include "difficulty.h"

// operator ++difficulty_t
Difficulty::difficulty_t& Difficulty::operator++ (Difficulty::difficulty_t& difficulty)
{ return difficulty = ( Difficulty::IMPOSSIBLE == difficulty ? Difficulty::EASY : Difficulty::difficulty_t(difficulty + 1)); };

// operator --difficulty_t
Difficulty::difficulty_t& Difficulty::operator-- (Difficulty::difficulty_t& difficulty)
{ return difficulty = ( Difficulty::EASY == difficulty ? Difficulty::IMPOSSIBLE : Difficulty::difficulty_t(difficulty - 1)); };


const std::string & Difficulty::String(Difficulty::difficulty_t difficulty)
{
    static const std::string easy("Easy");
    static const std::string nrml("Normal");
    static const std::string hard("Hard");
    static const std::string expt("Expert");
    static const std::string imps("Impossible");

    switch(difficulty){
        case Difficulty::EASY: return easy;
        case Difficulty::NORMAL: return nrml;
	case Difficulty::HARD: return hard;
	case Difficulty::EXPERT: return expt;
	default: break;
    }

    return imps;
}
