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
#include <algorithm>
#include "dir.h"
#include "gamedefs.h"
#include "settings.h"
#include "world.h"
#include "dialog.h"

Game::menu_t Game::Editor::LoadMaps(void)
{
    Settings & conf = Settings::Get();
        
    Dir dir;
    dir.Read(conf.MapsDirectory(), ".mp2", false);
    // loyality version
    if(conf.Modes(Settings::PRICELOYALTY)) dir.Read(conf.MapsDirectory(), ".mx2", false);
                        
    if(dir.empty())
    {
        Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
        return EDITMAINMENU;
    }

    MapsFileInfoList lists(dir.size());
    MapsFileInfoList::const_iterator res;
    int ii = 0;
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd, ++ii) if(!lists[ii].ReadBIN(*itd)) --ii;
    if(static_cast<size_t>(ii) != lists.size()) lists.resize(ii);
    std::sort(lists.begin(), lists.end(), Maps::FileInfo::PredicateForSorting);

    std::string filemaps;
    Dialog::SelectMapsFileList(lists, filemaps);
    conf.LoadFileMaps(filemaps);
    world.LoadMaps(filemaps);

    return EDITSTART;
}
