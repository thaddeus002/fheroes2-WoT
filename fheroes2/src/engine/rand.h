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
#ifndef H2RAND_H
#define H2RAND_H

#include <vector>
#include <list>
#include "types.h"

namespace Rand
{
    void Init(void);
    u32 Get(u32 min, u32 max = 0);

    template< typename T > const T * Get(const std::vector< T > & vec)
    {
        if(vec.empty()) return NULL;

        return & vec[Rand::Get(vec.size() - 1)];
    };
    
    template< typename T > const T * Get(const std::list< T > & list)
    {
        if(list.empty()) return NULL;

	u32 index1  = Rand::Get(list.size() - 1);
	u32 index2 = 0;
        typename std::list<T>::const_iterator it = list.begin();

        for(; it != list.end(); ++it) if(index1 == index2++) break;

        return & (*it);
    };
};
   
#endif
