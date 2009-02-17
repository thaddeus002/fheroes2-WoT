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
	tile2->addAttribute("ii", str);

	str.clear();
	String::AddInt(str, tile.tile_index);
	tile2->addAttribute("tile_index", str);

	str.clear();
	String::AddInt(str, tile.shape);
	tile2->addAttribute("shape", str);

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

	// tiles->tile->addons1
	xmlcc::Element *addons1 = tiles->addElement("addons_level1");

	str.clear();
	String::AddInt(str, tile.addons_level1.size());
	addons1->addAttribute("size", str);

        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level1.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level1.end();
	
	    for(; it1 != it2; ++it1)
	    {
		xmlcc::Element *addon2 = addons1->addElement("addon");
		const Maps::TilesAddon & addon = *it1;

		str.clear();
		String::AddInt(str, addon.level);
		addon2->addAttribute("level", str);

		str.clear();
		String::AddInt(str, addon.uniq);
		addon2->addAttribute("uniq", str);

		str.clear();
		String::AddInt(str, addon.object);
		addon2->addAttribute("object", str);

		str.clear();
		String::AddInt(str, addon.index);
		addon2->addAttribute("index", str);
	    }
	}

	// tiles->tile->addon2
	xmlcc::Element *addons2 = tiles->addElement("addons_level2");

	str.clear();
	String::AddInt(str, tile.addons_level2.size());
	addons2->addAttribute("size", str);

        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level2.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level2.end();
	
	    for(; it1 != it2; ++it1)
	    {
		xmlcc::Element *addon2 = addons1->addElement("addon");
		const Maps::TilesAddon & addon = *it1;

		str.clear();
		String::AddInt(str, addon.level);
		addon2->addAttribute("level", str);

		str.clear();
		String::AddInt(str, addon.uniq);
		addon2->addAttribute("uniq", str);

		str.clear();
		String::AddInt(str, addon.object);
		addon2->addAttribute("object", str);

		str.clear();
		String::AddInt(str, addon.index);
		addon2->addAttribute("index", str);
	    }
	}
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
	kingdom2->addAttribute("ii", str);

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

	// kingdoms->kingdom->visit_object
	xmlcc::Element *visit = kingdom2->addElement("visit_object");

	str.clear();
	String::AddInt(str, kingdom.visit_object.size());
	visit->addAttribute("size", str);

        {
	    std::list<IndexObject>::const_iterator it1 = kingdom.visit_object.begin();
	    std::list<IndexObject>::const_iterator it2 = kingdom.visit_object.end();
	
	    for(; it1 != it2; ++it1)
	    {
		xmlcc::Element *pair = visit->addElement("pair");

		str.clear();
		String::AddInt(str, (*it1).first);
		pair->addAttribute("index", str);

		str.clear();
		String::AddInt(str, (*it1).second);
		pair->addAttribute("object", str);
	    }
	}
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
	hero2->addAttribute("ii", str);

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
	castle2->addAttribute("ii", str);

	str.clear();
	String::AddInt(str, castle.color);
	castle2->addAttribute("color", str);
    }

    tree.setRoot(root);
    tree.setCompression(6);
    tree.write(fn);
}
#else
void Game::SaveXML(const std::string &){};
#endif
