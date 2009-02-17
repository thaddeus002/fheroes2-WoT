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

#include "settings.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"

extern char *basename(const char *path);

void Game::Save(const std::string &fn)
{
    Error::Verbose("Game::Save: to " + fn);

    SaveXML(fn);
}

#ifdef WITH_XML
#include "xmlccwrap.h"

void Game::SaveXML(const std::string &fn)
{
    xmlcc::XMLTree tree;
    std::string str;
    const Settings & conf = Settings::Get();

    xmlcc::Element *root =  new xmlcc::Element("fheroes2");

    str.clear();
    String::AddInt(str, conf.MajorVersion());
    str += ".";
    String::AddInt(str, conf.MinorVersion());
    root->addAttribute("version", str);

    str.clear();
    String::AddInt(str, conf.DateBuild());
    root->addAttribute("build", str);

    // maps
    xmlcc::Element *maps = root->addElement("maps");

    // maps->file
    str = basename(conf.FileInfo().FileMaps().c_str());
    xmlcc::Element *file = maps->addElement("file");
    file->addContent(str);
    // maps->name
    xmlcc::Element *name = maps->addElement("name");
    name->addContent(conf.FileInfo().Name());
    // maps->description
    xmlcc::Element *desc = maps->addElement("description");
    desc->addContent(conf.FileInfo().Description());

    // world
    xmlcc::Element *world2 = root->addElement("world");

    str.clear();
    String::AddInt(str, world.width);
    world2->addAttribute("width", str);

    str.clear();
    String::AddInt(str, world.height);
    world2->addAttribute("height", str);

    str.clear();
    String::AddInt(str, world.ultimate_artifact);
    world2->addAttribute("ultimate", str);

    str.clear();
    String::AddInt(str, world.uniq0);
    world2->addAttribute("uniq", str);

    // world->date
    xmlcc::Element *date = world2->addElement("date");

    str.clear();
    String::AddInt(str, world.month);
    date->addAttribute("month", str);

    str.clear();
    String::AddInt(str, world.week);
    date->addAttribute("week", str);

    str.clear();
    String::AddInt(str, world.day);
    date->addAttribute("day", str);

    // world->tiles
    xmlcc::Element *tiles = world2->addElement("tiles");

    str.clear();
    String::AddInt(str, world.vec_tiles.size());
    tiles->addAttribute("size", str);

    for(u16 ii = 0; ii < world.vec_tiles.size(); ++ii) if(world.vec_tiles[ii])
    {
	// tiles->tile
	xmlcc::Element *tile2 = tiles->addElement("tile");
	const Maps::Tiles & tile = *world.vec_tiles[ii];

	str.clear();
	String::AddInt(str, ii);
	tile2->addAttribute("index", str);

	str.clear();
	String::AddInt(str, tile.general);
	tile2->addAttribute("general", str);

	str.clear();
	String::AddInt(str, tile.quantity1);
	tile2->addAttribute("quantity1", str);

	str.clear();
	String::AddInt(str, tile.quantity2);
	tile2->addAttribute("quantity2", str);

	str.clear();
	String::AddInt(str, tile.fogs);
	tile2->addAttribute("fogs", str);
/*
        Surface tile_sprite;
        std::list<TilesAddon> addons_level1;
        std::list<TilesAddon> addons_level2;
*/
    }

    // world->kingdoms
    xmlcc::Element *kingdoms = world2->addElement("kingdoms");

    str.clear();
    String::AddInt(str, world.vec_kingdoms.size());
    kingdoms->addAttribute("size", str);

    for(u16 ii = 0; ii < world.vec_kingdoms.size(); ++ii) if(world.vec_kingdoms[ii])
    {
	// kingdoms->kingdom
	xmlcc::Element *kingdom2 = kingdoms->addElement("kingdom");
	const Kingdom & kingdom = *world.vec_kingdoms[ii];

	str.clear();
	String::AddInt(str, ii);
	kingdom2->addAttribute("index", str);

	str.clear();
	String::AddInt(str, kingdom.color);
	kingdom2->addAttribute("color", str);

	str.clear();
	String::AddInt(str, kingdom.flags);
	kingdom2->addAttribute("flags", str);

	str.clear();
	String::AddInt(str, kingdom.control);
	kingdom2->addAttribute("control", str);

	if(kingdom.ai_capital)
	{
	    str.clear();
	    String::AddInt(str, kingdom.ai_capital->GetIndex());
	    kingdom2->addAttribute("capital", str);
	}
	else
	    kingdom2->addAttribute("capital", "null");

	// kingdoms->kingdom->funds
	xmlcc::Element *funds = kingdom2->addElement("funds");

	str.clear();
	String::AddInt(str, kingdom.resource.wood);
	funds->addAttribute("wood", str);

	str.clear();
	String::AddInt(str, kingdom.resource.mercury);
	funds->addAttribute("mercury", str);

	str.clear();
	String::AddInt(str, kingdom.resource.ore);
	funds->addAttribute("ore", str);

	str.clear();
	String::AddInt(str, kingdom.resource.sulfur);
	funds->addAttribute("sulfur", str);

	str.clear();
	String::AddInt(str, kingdom.resource.crystal);
	funds->addAttribute("crystal", str);

	str.clear();
	String::AddInt(str, kingdom.resource.gems);
	funds->addAttribute("gems", str);

	str.clear();
	String::AddInt(str, kingdom.resource.gold);
	funds->addAttribute("gold", str);

/*
    std::map<u16, MP2::object_t> ai_objects;
    std::list<IndexObject> visit_object;
*/
    }

    // world->heroes
    xmlcc::Element *heroes = world2->addElement("heroes");

    str.clear();
    String::AddInt(str, world.vec_heroes.size());
    heroes->addAttribute("size", str);

    for(u16 ii = 0; ii < world.vec_heroes.size(); ++ii) if(world.vec_heroes[ii])
    {
	xmlcc::Element *hero2 = heroes->addElement("hero");
	const Heroes & hero = *world.vec_heroes[ii];

	str.clear();
	String::AddInt(str, ii);
	hero2->addAttribute("index", str);

	str.clear();
	String::AddInt(str, hero.color);
	hero2->addAttribute("color", str);
    }

    // world->castles
    xmlcc::Element *castles = world2->addElement("castles");

    str.clear();
    String::AddInt(str, world.vec_castles.size());
    castles->addAttribute("size", str);

    for(u16 ii = 0; ii < world.vec_castles.size(); ++ii) if(world.vec_castles[ii])
    {
	xmlcc::Element *castle2 = castles->addElement("castle");
	const Castle & castle = *world.vec_castles[ii];

	str.clear();
	String::AddInt(str, ii);
	castle2->addAttribute("index", str);

	str.clear();
	String::AddInt(str, castle.color);
	castle2->addAttribute("color", str);
    }

    tree.setRoot(root);
    tree.write(fn);
}
#else
void Game::SaveXML(const std::string &){};
#endif
