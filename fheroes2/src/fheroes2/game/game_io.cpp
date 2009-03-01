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
#include "army.h"
#include "world.h"

extern char *basename(const char *path);

void Game::Save(void)
{
    const std::string defaults(Settings::Get().LocalDataPrefix() + SEPARATOR + "save" + SEPARATOR + "fheroes2.sav");

    Save(defaults);
}

void Game::Load(void)
{
    const std::string defaults(Settings::Get().LocalDataPrefix() + SEPARATOR + "save" + SEPARATOR + "fheroes2.sav");

    Load(defaults);
}

void Game::Save(const std::string &fn)
{
    Error::Verbose("Game::Save: " + fn);

    SaveXML(fn);
}

void Game::Load(const std::string &fn)
{
    Error::Verbose("Game::Load: " + fn);

    LoadXML(fn);
}

#ifdef WITH_XML
#include "xmlccwrap.h"

void Game::SaveXML(const std::string &fn)
{
    std::string str;
    XML::Writer writer;
    const Settings & conf = Settings::Get();

    if(!writer.open(fn.c_str(), 5))
    {
	Error::Warning("Game::SaveXML: error write " + fn);
	return;
    }

    writer.startElement("fheroes2");

    str.clear();
    String::AddInt(str, conf.MajorVersion());
    str += ".";
    String::AddInt(str, conf.MinorVersion());

    writer.addAttribute("version", str);
    writer.addAttribute("build", conf.DateBuild());

    // maps
    writer.startElement("maps");
    writer.addAttribute("width", conf.FileInfo().SizeMaps().w);
    writer.addAttribute("height", conf.FileInfo().SizeMaps().h);
    writer.addAttribute("file", basename(conf.FileInfo().FileMaps().c_str()));
    writer.addElement("name", conf.FileInfo().Name());
    writer.addElement("description", conf.FileInfo().Description());
    writer.endElement();

    // game
    writer.startElement("game");
    writer.addAttribute("wins", conf.FileInfo().ConditionsWins());
    writer.addAttribute("loss", conf.FileInfo().ConditionsLoss());
    writer.endElement();

    // world
    writer.startElement("world");
    writer.addAttribute("width", world.width);
    writer.addAttribute("height", world.height);
    writer.addAttribute("ultimate", world.ultimate_artifact);
    writer.addAttribute("uniq", world.uniq0);

    // world->date
    writer.startElement("date");
    writer.addAttribute("month", world.month);
    writer.addAttribute("week", world.week);
    writer.addAttribute("day", world.day);
    writer.endElement();

    // world->tiles
    writer.startElement("tiles");
    writer.addAttribute("size", world.vec_tiles.size());

    for(u16 ii = 0; ii < world.vec_tiles.size(); ++ii) if(world.vec_tiles[ii])
    {
	// tiles->tile
	writer.startElement("tile");
	const Maps::Tiles & tile = *world.vec_tiles[ii];

	writer.addAttribute("ii", ii);
	writer.addAttribute("tile_index", tile.tile_index);
	writer.addAttribute("shape", tile.shape);
	writer.addAttribute("general", tile.general);
	writer.addAttribute("quantity1", tile.quantity1);
	writer.addAttribute("quantity2", tile.quantity2);
	writer.addAttribute("fogs", tile.fogs);

	// tiles->tile->addons1
	writer.startElement("addons_level1");
	writer.addAttribute("size", tile.addons_level1.size());
        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level1.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level1.end();
	
	    for(; it1 != it2; ++it1)
	    {
		writer.startElement("addon");
		const Maps::TilesAddon & addon = *it1;
		writer.addAttribute("level", addon.level);
		writer.addAttribute("uniq", addon.uniq);
		writer.addAttribute("object", addon.object);
		writer.addAttribute("index", addon.index);
		writer.endElement();
	    }
	}
	writer.endElement();

	// tiles->tile->addons2
	writer.startElement("addons_level2");
	writer.addAttribute("size", tile.addons_level2.size());
        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level2.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level2.end();
	
	    for(; it1 != it2; ++it1)
	    {
		writer.startElement("addon");
		const Maps::TilesAddon & addon = *it1;
		writer.addAttribute("level", addon.level);
		writer.addAttribute("uniq", addon.uniq);
		writer.addAttribute("object", addon.object);
		writer.addAttribute("index", addon.index);
		writer.endElement();
	    }
	}
	writer.endElement();

	// tiles->tile
	writer.endElement();
    }

    // world->tiles
    writer.endElement();

    // world->kingdoms
    writer.startElement("kingdoms");
    writer.addAttribute("size", world.vec_kingdoms.size());

    for(u16 ii = 0; ii < world.vec_kingdoms.size(); ++ii) if(world.vec_kingdoms[ii])
    {
	// kingdoms->kingdom
	writer.startElement("kingdom");
	const Kingdom & kingdom = *world.vec_kingdoms[ii];

	writer.addAttribute("ii", ii);
	writer.addAttribute("modes", kingdom.flags);
	writer.addAttribute("lost_town_days", kingdom.lost_town_days);
	writer.addAttribute("color", kingdom.color);
	writer.addAttribute("control", kingdom.control);

	if(kingdom.ai_capital)
	writer.addAttribute("capital", kingdom.ai_capital->GetIndex());
	else
	writer.addAttribute("capital", "null");

	// kingdoms->kingdom->funds
	writer.startElement("funds");
	writer.addAttribute("wood", kingdom.resource.wood);
	writer.addAttribute("mercury", kingdom.resource.mercury);
	writer.addAttribute("ore", kingdom.resource.ore);
	writer.addAttribute("sulfur", kingdom.resource.sulfur);
	writer.addAttribute("crystal", kingdom.resource.crystal);
	writer.addAttribute("gems", kingdom.resource.gems);
	writer.addAttribute("gold", kingdom.resource.gold);
	writer.endElement();

	// kingdoms->kingdom->visit_object
	writer.startElement("visit_object");
	writer.addAttribute("size", kingdom.visit_object.size());
        {
	    std::list<IndexObject>::const_iterator it1 = kingdom.visit_object.begin();
	    std::list<IndexObject>::const_iterator it2 = kingdom.visit_object.end();
	
	    for(; it1 != it2; ++it1)
	    {
		writer.startElement("pair");
		writer.addAttribute("index", (*it1).first);
		writer.addAttribute("object", (*it1).second);
		writer.endElement();
	    }
	}
	// kingdoms->kingdom->visit_object
	writer.endElement();

	// kingdoms->kingdom
	writer.endElement();
    }

    // world->kingdoms
    writer.endElement();

    // world->heroes
    writer.startElement("heroes");
    writer.addAttribute("size", world.vec_heroes.size());

    for(u16 ii = 0; ii < world.vec_heroes.size(); ++ii) if(world.vec_heroes[ii])
    {
	// heroes->hero
	writer.startElement("hero");
	const Heroes & hero = *world.vec_heroes[ii];

	writer.addAttribute("ii", ii);
	writer.addAttribute("modes", hero.modes);
	writer.addAttribute("color", hero.color);
	writer.addAttribute("name", hero.name);
	writer.addAttribute("hid", hero.hid);
	writer.addAttribute("race", hero.race);
	writer.addAttribute("attack", hero.GetAttack());
	writer.addAttribute("defense", hero.GetDefense());
	writer.addAttribute("knowledge", hero.GetKnowledge());
	writer.addAttribute("power", hero.GetPower());
	writer.addAttribute("experience", hero.experience);
	writer.addAttribute("magic_point", hero.magic_point);
	writer.addAttribute("move_point", hero.move_point);
	writer.addAttribute("direction", hero.direction);
	writer.addAttribute("sprite_index", hero.sprite_index);
	writer.addAttribute("save_maps_general", hero.save_maps_general);

	// heroes->hero->secondary_skills
	writer.startElement("secondary_skills");
	writer.addAttribute("size", hero.secondary_skills.size());
	for(u16 ii = 0; ii < hero.secondary_skills.size(); ++ii)
	{
	    const Skill::Secondary & sec = hero.secondary_skills[ii];
	    writer.startElement("pair");
	    writer.addAttribute("skill", sec.Skill());
	    writer.addAttribute("level", sec.Level());
	    writer.endElement();
	}
	writer.endElement();

	// heroes->hero->artifacts
	writer.startElement("artifacts");
	writer.addAttribute("size", hero.artifacts.size());
	for(u16 ii = 0; ii < hero.artifacts.size(); ++ii)
	{
	    writer.addElement("artifact", hero.artifacts[ii]);
	}
	writer.endElement();

	// heroes->hero->armies
	writer.startElement("armies");
	writer.addAttribute("size", ARMYMAXTROOPS);
	for(u16 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	{
	    const Army::Troop & troop = hero.army.At(ii);
	    writer.startElement("troop");
	    writer.addAttribute("monster", troop());
	    writer.addAttribute("count", troop.Count());
	    writer.endElement();
	}
	writer.endElement();

	// heroes->hero->spell_book
	writer.startElement("spell_book");
	writer.addAttribute("enable", hero.spell_book.isActive() ? "true" : "false");
        {
	    std::list<Spell::spell_t>::const_iterator it1, it2;

	    // spell_level1
	    it1 = hero.spell_book.spells_level1.begin();
	    it2 = hero.spell_book.spells_level1.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level2
	    it1 = hero.spell_book.spells_level2.begin();
	    it2 = hero.spell_book.spells_level2.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level3
	    it1 = hero.spell_book.spells_level3.begin();
	    it2 = hero.spell_book.spells_level3.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level4
	    it1 = hero.spell_book.spells_level4.begin();
	    it2 = hero.spell_book.spells_level4.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level5
	    it1 = hero.spell_book.spells_level5.begin();
	    it2 = hero.spell_book.spells_level5.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);
	}
	writer.endElement();

	// heroes->hero->center
	writer.startElement("center");
	writer.addAttribute("x", hero.mp.x);
	writer.addAttribute("y", hero.mp.y);
	writer.endElement();

	// heroes->hero->visit_object
	writer.startElement("visit_object");
	writer.addAttribute("size", hero.visit_object.size());
        {
	    std::list<IndexObject>::const_iterator it1 = hero.visit_object.begin();
	    std::list<IndexObject>::const_iterator it2 = hero.visit_object.end();
	    for(; it1 != it2; ++it1)
	    {
		writer.startElement("pair");
		writer.addAttribute("index", (*it1).first);
		writer.addAttribute("object", (*it1).second);
		writer.endElement();
	    }
	}
	writer.endElement();

	// heroes->hero
	writer.endElement();
    }
    // world->heroes
    writer.endElement();

    // world->castles
    writer.startElement("castles");
    writer.addAttribute("size", world.vec_castles.size());
    for(u16 ii = 0; ii < world.vec_castles.size(); ++ii) if(world.vec_castles[ii])
    {
	// castles->castle
	writer.startElement("castle");
	const Castle & castle = *world.vec_castles[ii];

	writer.addAttribute("ii", ii);
	writer.addAttribute("modes", castle.modes);
	writer.addAttribute("color", castle.color);
	writer.addAttribute("name", castle.name);
	writer.addAttribute("race", castle.race);
	writer.addAttribute("building", castle.building);

	// castles->castle->center
	writer.startElement("center");
	writer.addAttribute("x", castle.mp.x);
	writer.addAttribute("y", castle.mp.y);
	writer.endElement();

	// castles->castle->mageguild
	writer.startElement("mageguild");
	writer.addAttribute("level", castle.mageguild.GetLevel());
        {
	    std::list<Spell::spell_t>::const_iterator it1, it2;

	    // spell_level1
	    it1 = castle.mageguild.spells_level1.begin();
	    it2 = castle.mageguild.spells_level1.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level2
	    it1 = castle.mageguild.spells_level2.begin();
	    it2 = castle.mageguild.spells_level2.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level3
	    it1 = castle.mageguild.spells_level3.begin();
	    it2 = castle.mageguild.spells_level3.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level4
	    it1 = castle.mageguild.spells_level4.begin();
	    it2 = castle.mageguild.spells_level4.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);

	    // spell_level5
	    it1 = castle.mageguild.spells_level5.begin();
	    it2 = castle.mageguild.spells_level5.end();
	    for(; it1 != it2; ++it1) writer.addElement("spell", *it1);
	}
	writer.endElement();

	// castles->castle->armies
	writer.startElement("armies");
	writer.addAttribute("size", ARMYMAXTROOPS);

	for(u16 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	{
	    const Army::Troop & troop = castle.army.At(ii);
	    writer.startElement("troop");
	    writer.addAttribute("monster", troop());
	    writer.addAttribute("count", troop.Count());
	    writer.endElement();
	}
	writer.endElement();

	// castles->castle->dwelling
	writer.startElement("dwellings");
	writer.addAttribute("size", castle.dwelling.size());
	for(u16 ii = 0; ii < castle.dwelling.size(); ++ii) writer.addElement("dwelling", castle.dwelling[ii]);
	writer.endElement();

	// castles->castle
	writer.endElement();
    }

    // world->castles
    writer.endElement();

    // world
    writer.endElement();

    // fheroes2
    writer.endElement();

    writer.close();
}

void Game::LoadXML(const std::string &fn)
{
/*
    xmlcc::XMLTree tree;

    if(!tree.read("fheroes2.sav"))
    {
	Error::Warning("Game::LoadXML: parser error: " + fn);
	return;
    }

    xmlcc::Element *fheroes2 = tree.getRoot();

    if(!fheroes2 || fheroes2->name() != "fheroes2")
    {
	Error::Warning("Game::LoadXML: parser error: fheroes2");
	return;
    }

    xmlcc::Element *maps = fheroes2->getElement("maps");
    if(!maps)
    {
	Error::Warning("Game::LoadXML: parser error: maps");
	return;
    }

    xmlcc::Element *mapsw = maps->getElement("width");
    xmlcc::Element *mapsh = maps->getElement("height");
    if(!mapsw || !mapsh)
    {
	Error::Warning("Game::LoadXML: parser error: width or height");
	return;
    }
*/

//
//    Error::Verbose(mapsw->name());
//    Error::Verbose(mapsh->name());
//    Error::Verbose("w: " + mapsw->getContent());
//    Error::Verbose("h: " + mapsh->getContent());
//    Error::Verbose("w: ", String::ToInt(mapsw->getContent()));
//    Error::Verbose("h: ", String::ToInt(mapsh->getContent()));

//    xmlcc::Element *file = maps->getElement("file");
//    xmlcc::Element *name = maps->getElement("name");
//    xmlcc::Element *desc = maps->getElement("description");

}
#else
void Game::SaveXML(const std::string &){};
void Game::LoadXML(const std::string &){};
#endif
