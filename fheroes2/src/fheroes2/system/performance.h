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

#ifndef _PERFORMANCE_H
#define _PERFORMANCE_H

#include <sys/time.h>
#include <iostream>
#include "types.h"

struct Performance
{
    Performance(){};

    void Start(void){ gettimeofday(&clock1, NULL); };
    void Stop(void){ gettimeofday(&clock2, NULL); };
    double Get(void) const{ return ((clock2.tv_sec - clock1.tv_sec) * 1000 + (clock2.tv_usec - clock1.tv_usec) / 1000.0) + 0.5; };
    void Print(void) const { std::cout << "Performance: " << Get() << " ms" << std::endl; };

    struct timeval clock1;
    struct timeval clock2;
};

#endif
