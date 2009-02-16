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

#include "settings.h"
#include "world.h"

#ifdef WITH_XML
#include "xmlccwrap.h"

/* save world */
void World::Save(const std::string &fn)
{
    Error::Verbose("World::Save: to " + fn);

    xmlcc::XMLTree tree;
    std::string str;
    const Settings & conf = Settings::Get();

    xmlcc::Element *root =  new xmlcc::Element("fheroes2");

    // world
    str.clear();
    String::AddInt(str, conf.MajorVersion());
    str += ".";
    String::AddInt(str, conf.MinorVersion());
    root->addAttribute("version", str);

    str.clear();
    String::AddInt(str, conf.DateBuild());
    root->addAttribute("build", str);
    xmlcc::Element *world = root->addElement("world");

    str.clear();
    String::AddInt(str, width);
    world->addAttribute("width", str);

    str.clear();
    String::AddInt(str, height);
    world->addAttribute("height", str);

    str.clear();
    String::AddInt(str, ultimate_artifact);
    world->addAttribute("ultimate", str);

    str.clear();
    String::AddInt(str, uniq0);
    world->addAttribute("uniq", str);

    // world->date
    xmlcc::Element *date = world->addElement("date");

    str.clear();
    String::AddInt(str, month);
    date->addAttribute("month", str);

    str.clear();
    String::AddInt(str, week);
    date->addAttribute("week", str);

    str.clear();
    String::AddInt(str, day);
    date->addAttribute("day", str);

    // world->tiles
    xmlcc::Element *tiles = world->addElement("tiles");
    for(u16 ii = 0; ii < vec_tiles.size(); ++ii)
    {
	xmlcc::Element *tile = tiles->addElement("tile");
	//if(vec_tiles[ii]) vec_tiles[ii]->Save(tile);
    }

    // world->kingdom
    xmlcc::Element *kingdoms = world->addElement("kingdoms");
    for(u16 ii = 0; ii < vec_kingdoms.size(); ++ii)
    {
	xmlcc::Element *kingdom = kingdoms->addElement("kingdom");
	//if(vec_kingdoms[ii]) vec_kingdoms[ii]->Save(kingdom);
    }

    // world->heroes
    xmlcc::Element *heroes = world->addElement("heroes");
    for(u16 ii = 0; ii < vec_heroes.size(); ++ii)
    {
	xmlcc::Element *hero = heroes->addElement("hero");
	//if(vec_heroes[ii]) vec_heroes[ii]->Save(hero);
    }

    // world->castles
    xmlcc::Element *castles = world->addElement("castles");
    for(u16 ii = 0; ii < vec_castles.size(); ++ii)
    {
	xmlcc::Element *castle = castles->addElement("castle");
	//if(vec_castles[ii]) vec_castles[ii]->Save(castle);
    }

    tree.setRoot(root);
    tree.write(fn);
}
#else
void World::Save(const std::string &fn)
{
    Error::Verbose("World::Save: build with DEVELOPMENT=1");
}
#endif
