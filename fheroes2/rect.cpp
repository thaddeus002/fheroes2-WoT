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
    int x1 = 32766;
    int y1 = 32766;
    int x2 = -32766;
    int y2 = -32766;
       
    std::vector<Rect>::const_iterator it = vect.begin();
    
    for(; it != vect.end(); ++it)
    {
	if((*it).x < x1) x1 = (*it).x;
	if((*it).y < y1) y1 = (*it).y;
	if((*it).x + (*it).w > x2) x2 = (*it).x + (*it).w;
	if((*it).y + (*it).h > y2) y2 = (*it).y + (*it).h;
    }
    
    x = x1;
    y = y1;
    w = x2 - x1;
    h = y2 - y1;
}
