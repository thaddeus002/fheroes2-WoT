/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "gamedefs.h"
#include "settings.h"
#include "world.h"
#include "dialog.h"

Game::menu_t Game::Editor::LoadMaps(void)
{
    Settings & conf = Settings::Get();
    std::string filemaps;

    if(Dialog::SelectMapsFile(filemaps) && conf.LoadFileMapsMP2(filemaps))
    {
	Game::ShowLoadMapsText();
    	//
    	world.LoadMaps(filemaps);

	return EDITSTART;
    }

    return MAINMENU;
}
