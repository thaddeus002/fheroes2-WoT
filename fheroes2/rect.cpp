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

#include "rect.h"

Rect::Rect(const std::vector<Rect> & vect)
{
    x = 32767;
    y = 32767;
    w = 0;
    h = 0;
       
    std::vector<Rect>::const_iterator it = vect.begin();
    
    for(; it != vect.end(); ++it)
    {
	if((*it).x < x) x = (*it).x;
	if((*it).y < y) y = (*it).y;
	if((*it).x + (*it).w > x + w) w = (*it).x + (*it).w - x;
	if((*it).y + (*it).h > y + h) h = (*it).y + (*it).h - y;
    }
}
