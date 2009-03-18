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
#include "xmlccwrap.h"

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

void Game::SaveXML(const std::string &fn)
{
    std::string str;
    const Settings & conf = Settings::Get();
    TiXmlDocument doc;
    TiXmlElement *root, *node;

    doc.LinkEndChild(new TiXmlDeclaration( "1.0", "", ""));
    root = new TiXmlElement("fheroes2");

    str.clear();
    String::AddInt(str, conf.MajorVersion());
    str += ".";
    String::AddInt(str, conf.MinorVersion());

    root->SetAttribute("version", str.c_str());
    root->SetAttribute("build", conf.DateBuild());
    doc.LinkEndChild(root);

    // maps
    TiXmlElement* maps = new TiXmlElement("maps");
    root->LinkEndChild(maps);
    maps->SetAttribute("width", conf.FileInfo().SizeMaps().w);
    maps->SetAttribute("height", conf.FileInfo().SizeMaps().h);
    maps->SetAttribute("file", basename(conf.FileInfo().FileMaps().c_str()));
    //
    node = new TiXmlElement("name");
    maps->LinkEndChild(node);
    node->LinkEndChild(new TiXmlText(conf.FileInfo().Name().c_str()));
    //
    node = new TiXmlElement("description");
    maps->LinkEndChild(node);
    node->LinkEndChild(new TiXmlText(conf.FileInfo().Description().c_str()));
    
    // game
    TiXmlElement* game = new TiXmlElement("game");
    root->LinkEndChild(game);
    game->SetAttribute("wins", conf.FileInfo().ConditionsWins());
    game->SetAttribute("loss", conf.FileInfo().ConditionsLoss());

    // world
    TiXmlElement* wrld = new TiXmlElement("world");
    root->LinkEndChild(wrld);
    wrld->SetAttribute("width", world.width);
    wrld->SetAttribute("height", world.height);
    wrld->SetAttribute("ultimate", world.ultimate_artifact);
    wrld->SetAttribute("uniq", world.uniq0);
    // world->date
    node = new TiXmlElement("date");
    wrld->LinkEndChild(node);
    node->SetAttribute("month", world.month);
    node->SetAttribute("week", world.week);
    node->SetAttribute("day", world.day);

    // world->tiles
    TiXmlElement* tiles = new TiXmlElement("tiles");
    wrld->LinkEndChild(tiles);
    tiles->SetAttribute("size", world.vec_tiles.size());

    for(u16 ii = 0; ii < world.vec_tiles.size(); ++ii) if(world.vec_tiles[ii])
    {
	// tiles->tile
	TiXmlElement* tile2 = new TiXmlElement("tile");
	tiles->LinkEndChild(tile2);
	const Maps::Tiles & tile = *world.vec_tiles[ii];

	tile2->SetAttribute("ii", ii);
	tile2->SetAttribute("tile_index", tile.tile_index);
	tile2->SetAttribute("shape", tile.shape);
	tile2->SetAttribute("general", tile.general);
	tile2->SetAttribute("quantity1", tile.quantity1);
	tile2->SetAttribute("quantity2", tile.quantity2);
	tile2->SetAttribute("fogs", tile.fogs);

	// tiles->tile->addons1
	TiXmlElement* addons = new TiXmlElement("addons_level1");
	tile2->LinkEndChild(addons);
	addons->SetAttribute("size", tile.addons_level1.size());
        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level1.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level1.end();
	
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("addon");
		addons->LinkEndChild(node);
		const Maps::TilesAddon & addon = *it1;
		node->SetAttribute("level", addon.level);
		node->SetAttribute("uniq", addon.uniq);
		node->SetAttribute("object", addon.object);
		node->SetAttribute("index", addon.index);
	    }
	}

	// tiles->tile->addons2
	addons = new TiXmlElement("addons_level2");
	tile2->LinkEndChild(addons);
	addons->SetAttribute("size", tile.addons_level2.size());
        {
	    std::list<Maps::TilesAddon>::const_iterator it1 = tile.addons_level2.begin();
	    std::list<Maps::TilesAddon>::const_iterator it2 = tile.addons_level2.end();
	
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("addon");
		addons->LinkEndChild(node);
		const Maps::TilesAddon & addon = *it1;
		node->SetAttribute("level", addon.level);
		node->SetAttribute("uniq", addon.uniq);
		node->SetAttribute("object", addon.object);
		node->SetAttribute("index", addon.index);
	    }
	}
    }

    // world->kingdoms
    TiXmlElement* kingdoms = new TiXmlElement("kingdoms");
    wrld->LinkEndChild(kingdoms);
    kingdoms->SetAttribute("size", world.vec_kingdoms.size());

    for(u16 ii = 0; ii < world.vec_kingdoms.size(); ++ii) if(world.vec_kingdoms[ii])
    {
	// kingdoms->kingdom
	TiXmlElement* kingdom2 = new TiXmlElement("kingdom");
	kingdoms->LinkEndChild(kingdom2);
	const Kingdom & kingdom = *world.vec_kingdoms[ii];

	kingdom2->SetAttribute("ii", ii);
	kingdom2->SetAttribute("modes", kingdom.flags);
	kingdom2->SetAttribute("lost_town_days", kingdom.lost_town_days);
	kingdom2->SetAttribute("color", kingdom.color);
	kingdom2->SetAttribute("control", kingdom.control);

	if(kingdom.ai_capital)
	    kingdom2->SetAttribute("capital", kingdom.ai_capital->GetIndex());
	else
	    kingdom2->SetAttribute("capital", "null");

	// kingdoms->kingdom->funds
	TiXmlElement* funds = new TiXmlElement("funds");
	kingdom2->LinkEndChild(funds);
	funds->SetAttribute("wood", kingdom.resource.wood);
	funds->SetAttribute("mercury", kingdom.resource.mercury);
	funds->SetAttribute("ore", kingdom.resource.ore);
	funds->SetAttribute("sulfur", kingdom.resource.sulfur);
	funds->SetAttribute("crystal", kingdom.resource.crystal);
	funds->SetAttribute("gems", kingdom.resource.gems);
	funds->SetAttribute("gold", kingdom.resource.gold);

	// kingdoms->kingdom->visit_object
	TiXmlElement* visit = new TiXmlElement("visit_object");
	kingdom2->LinkEndChild(visit);
	visit->SetAttribute("size", kingdom.visit_object.size());
        {
	    std::list<IndexObject>::const_iterator it1 = kingdom.visit_object.begin();
	    std::list<IndexObject>::const_iterator it2 = kingdom.visit_object.end();
	
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("pair");
		visit->LinkEndChild(node);
		node->SetAttribute("index", (*it1).first);
		node->SetAttribute("object", (*it1).second);
	    }
	}
    }

    // world->heroes
    TiXmlElement* heroes = new TiXmlElement("heroes");
    wrld->LinkEndChild(heroes);
    heroes->SetAttribute("size", world.vec_heroes.size());

    for(u16 ii = 0; ii < world.vec_heroes.size(); ++ii) if(world.vec_heroes[ii])
    {
	// heroes->hero
	TiXmlElement* hero2 = new TiXmlElement("hero");
	heroes->LinkEndChild(hero2);
	const Heroes & hero = *world.vec_heroes[ii];

	hero2->SetAttribute("ii", ii);
	hero2->SetAttribute("modes", hero.modes);
	hero2->SetAttribute("color", hero.color);
	hero2->SetAttribute("name", hero.name.c_str());
	hero2->SetAttribute("portrait", hero.portrait);
	hero2->SetAttribute("race", hero.race);
	hero2->SetAttribute("attack", hero.GetAttack());
	hero2->SetAttribute("defense", hero.GetDefense());
	hero2->SetAttribute("knowledge", hero.GetKnowledge());
	hero2->SetAttribute("power", hero.GetPower());
	hero2->SetAttribute("experience", hero.experience);
	hero2->SetAttribute("magic_point", hero.magic_point);
	hero2->SetAttribute("move_point", hero.move_point);
	hero2->SetAttribute("direction", hero.direction);
	hero2->SetAttribute("sprite_index", hero.sprite_index);
	hero2->SetAttribute("save_maps_general", hero.save_maps_general);

	// heroes->hero->secondary_skills
	TiXmlElement* skills = new TiXmlElement("secondary_skills");
	hero2->LinkEndChild(skills);
	skills->SetAttribute("size", hero.secondary_skills.size());
	for(u16 ii = 0; ii < hero.secondary_skills.size(); ++ii)
	{
	    const Skill::Secondary & sec = hero.secondary_skills[ii];
	    node = new TiXmlElement("pair");
	    skills->LinkEndChild(node);
	    node->SetAttribute("skill", sec.Skill());
	    node->SetAttribute("level", sec.Level());
	}

	// heroes->hero->artifacts
	TiXmlElement* artifacts = new TiXmlElement("artifacts");
	hero2->LinkEndChild(artifacts);
	artifacts->SetAttribute("size", hero.artifacts.size());
	for(u16 ii = 0; ii < hero.artifacts.size(); ++ii)
	{
	    node = new TiXmlElement("artifact");
	    artifacts->LinkEndChild(node);
	    node->SetAttribute("id", hero.artifacts[ii]);
	}

	// heroes->hero->armies
	TiXmlElement* armies = new TiXmlElement("armies");
	hero2->LinkEndChild(armies);
	armies->SetAttribute("size", ARMYMAXTROOPS);
	for(u16 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	{
	    const Army::Troop & troop = hero.army.At(ii);
	    node = new TiXmlElement("troop");
	    armies->LinkEndChild(node);
	    node->SetAttribute("monster", troop());
	    node->SetAttribute("count", troop.Count());
	}

	// heroes->hero->spell_book
	TiXmlElement* book = new TiXmlElement("spell_book");
	hero2->LinkEndChild(book);
	book->SetAttribute("enable", hero.spell_book.isActive() ? "true" : "false");
        {
	    std::list<Spell::spell_t>::const_iterator it1, it2;

	    // spell_level1
	    it1 = hero.spell_book.spells_level1.begin();
	    it2 = hero.spell_book.spells_level1.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		book->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level2
	    it1 = hero.spell_book.spells_level2.begin();
	    it2 = hero.spell_book.spells_level2.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		book->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level3
	    it1 = hero.spell_book.spells_level3.begin();
	    it2 = hero.spell_book.spells_level3.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		book->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level4
	    it1 = hero.spell_book.spells_level4.begin();
	    it2 = hero.spell_book.spells_level4.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		book->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level5
	    it1 = hero.spell_book.spells_level5.begin();
	    it2 = hero.spell_book.spells_level5.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		book->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }
	}

	// heroes->hero->center
	node = new TiXmlElement("center");
	hero2->LinkEndChild(node);
	node->SetAttribute("x", hero.mp.x);
	node->SetAttribute("y", hero.mp.y);

	// heroes->hero->visit_object
	TiXmlElement* visit = new TiXmlElement("visit_object");
	hero2->LinkEndChild(visit);
	visit->SetAttribute("size", hero.visit_object.size());
        {
	    std::list<IndexObject>::const_iterator it1 = hero.visit_object.begin();
	    std::list<IndexObject>::const_iterator it2 = hero.visit_object.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("pair");
		visit->LinkEndChild(node);
		node->SetAttribute("index", (*it1).first);
		node->SetAttribute("object", (*it1).second);
	    }
	}
    }

    // world->castles
    TiXmlElement* castles = new TiXmlElement("castles");
    wrld->LinkEndChild(castles);
    castles->SetAttribute("size", world.vec_castles.size());
    for(u16 ii = 0; ii < world.vec_castles.size(); ++ii) if(world.vec_castles[ii])
    {
	// castles->castle
	TiXmlElement* castle2 = new TiXmlElement("castle");
	castles->LinkEndChild(castle2);
	const Castle & castle = *world.vec_castles[ii];

	castle2->SetAttribute("ii", ii);
	castle2->SetAttribute("modes", castle.modes);
	castle2->SetAttribute("color", castle.color);
	castle2->SetAttribute("name", castle.name.c_str());
	castle2->SetAttribute("race", castle.race);
	castle2->SetAttribute("building", castle.building);

	// castles->castle->center
	node = new TiXmlElement("center");
	castle2->LinkEndChild(node);
	node->SetAttribute("x", castle.mp.x);
	node->SetAttribute("y", castle.mp.y);

	// castles->castle->mageguild
	TiXmlElement* mage = new TiXmlElement("mageguild");
	castle2->LinkEndChild(mage);
	mage->SetAttribute("level", castle.mageguild.GetLevel());
        {
	    std::list<Spell::spell_t>::const_iterator it1, it2;

	    // spell_level1
	    it1 = castle.mageguild.spells_level1.begin();
	    it2 = castle.mageguild.spells_level1.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		mage->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level2
	    it1 = castle.mageguild.spells_level2.begin();
	    it2 = castle.mageguild.spells_level2.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		mage->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level3
	    it1 = castle.mageguild.spells_level3.begin();
	    it2 = castle.mageguild.spells_level3.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		mage->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level4
	    it1 = castle.mageguild.spells_level4.begin();
	    it2 = castle.mageguild.spells_level4.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		mage->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }

	    // spell_level5
	    it1 = castle.mageguild.spells_level5.begin();
	    it2 = castle.mageguild.spells_level5.end();
	    for(; it1 != it2; ++it1)
	    {
		node = new TiXmlElement("spell");
		mage->LinkEndChild(node);
		node->SetAttribute("id", *it1);
	    }
	}

	// castles->castle->armies
	TiXmlElement* armies = new TiXmlElement("armies");
	castle2->LinkEndChild(armies);
	armies->SetAttribute("size", ARMYMAXTROOPS);

	for(u16 ii = 0; ii < ARMYMAXTROOPS; ++ii)
	{
	    node = new TiXmlElement("troop");
	    armies->LinkEndChild(node);
	    const Army::Troop & troop = castle.army.At(ii);
	    node->SetAttribute("monster", troop());
	    node->SetAttribute("count", troop.Count());
	}

	// castles->castle->dwelling
	TiXmlElement* dwellings = new TiXmlElement("dwellings");
	castle2->LinkEndChild(dwellings);
	dwellings->SetAttribute("size", castle.dwelling.size());
	for(u16 ii = 0; ii < castle.dwelling.size(); ++ii)
	{
	    node = new TiXmlElement("dwelling");
	    dwellings->LinkEndChild(node);
	    node->SetAttribute("dwelling", castle.dwelling[ii]);
	}
    }

    doc.SaveFile(fn.c_str());
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
