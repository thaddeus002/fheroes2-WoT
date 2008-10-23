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

#include <list>
#include "dir.h"
#include "error.h"
#include "config.h"
#include "maps_fileinfo.h"
#include "dialog.h"

Game::menu_t Game::Editor::LoadMaps(void)
{
    std::list<Maps::FileInfo *> info_maps;

    // read maps directory
    Dir dir;
    Settings & conf = Settings::Get();

    dir.Read(conf.MapsDirectory(), ".mp2", false);

    // loyality version
    if(conf.Modes(Settings::PRICELOYALTY)) dir.Read(conf.MapsDirectory(), ".mx2", false);

    // empty maps dir
    if(dir.empty())
    {
        Dialog::Message("Warning", "No maps available!", Font::BIG, Dialog::OK);
        return EDITMAINMENU;
    }

    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd)
	info_maps.push_back(new Maps::FileInfo(*itd));

    // show dialog
    const Maps::FileInfo * finfo = Dialog::SelectFileInfo(info_maps);
    if(NULL != finfo) conf.LoadFileMaps(finfo->FileMaps());

    std::list<Maps::FileInfo *>::const_iterator it1 = info_maps.begin();
    std::list<Maps::FileInfo *>::const_iterator it2 = info_maps.end();
    for(; it1 != it2; ++it1) if(*it1) delete *it1;

    return EDITSTART;
}
