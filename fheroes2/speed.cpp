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

#include "speed.h"

const std::string & Speed::String(speed_t speed)
{
    static const std::string str_crawling("Crawling");
    static const std::string str_veryslow("Very Slow");
    static const std::string str_slow("Slow");
    static const std::string str_average("Average");
    static const std::string str_fast("Fast");
    static const std::string str_veryfast("Very Fast");
    static const std::string str_ultrafast("Ultra Fast");
    static const std::string str_blazing("Blazing");
    static const std::string str_instant("Instant");

    switch(speed)
    {
	case CRAWLING:	return str_crawling;
        case VERYSLOW:	return str_veryslow;
        case SLOW:	return str_slow;
        case AVERAGE:	return str_average;
        case FAST:	return str_fast;
        case VERYFAST:	return str_veryfast;
        case ULTRAFAST:	return str_ultrafast;
        case BLAZING:	return str_blazing;
        case INSTANT:	return str_instant;
    }
    
    return str_crawling;
}
