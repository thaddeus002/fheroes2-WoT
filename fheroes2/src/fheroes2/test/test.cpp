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

#include "agg.h"
#include "settings.h"
#include "gamedefs.h"
#include "test.h"

#ifndef BUILD_RELEASE

void RunTest1(void);
void RunTest2(void);
void RunTest3(void);

void TestMonsterSprite(void);

void Test::Run(int num)
{
    switch(num)
    {
	case 1: RunTest1(); break;
	case 2: RunTest2(); break;
	case 3: RunTest3(); break;

	case 9: TestMonsterSprite(); break;

	default: Error::Warning("Test::Run: unknown test."); break;
    }
}

void RunTest1(void)
{
    Error::Verbose("Run Test1");
}

void RunTest2(void)
{
    Error::Verbose("Run Test3");
}

void RunTest3(void)
{
    Error::Verbose("Run Test3");
}

#endif
