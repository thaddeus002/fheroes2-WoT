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

#include <cstring>
#include <ctime>
#include "settings.h"
#include "kingdom.h"
#include "heroes.h"
#include "castle.h"
#include "army.h"
#include "world.h"
#include "gameevent.h"
#include "xmlccwrap.h"

extern char *basename(const char *path);

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
    String::AddInt(str, conf.major_version);
    str += ".";
    String::AddInt(str, conf.minor_version);

    root->SetAttribute("version", str.c_str());
    root->SetAttribute("build", conf.build_date);
    root->SetAttribute("time", std::time(NULL));

    doc.LinkEndChild(root);

    // maps
    TiXmlElement* maps = new TiXmlElement("maps");
    root->LinkEndChild(maps);
    maps->SetAttribute("width", conf.current_maps_file.size_w);
    maps->SetAttribute("height", conf.current_maps_file.size_h);
    maps->SetAttribute("file", basename(conf.current_maps_file.file.c_str()));
    maps->SetAttribute("difficulty", conf.current_maps_file.difficulty);
    maps->SetAttribute("kingdom_colors", conf.current_maps_file.kingdom_colors);
    maps->SetAttribute("allow_colors", conf.current_maps_file.allow_colors);
    maps->SetAttribute("rnd_colors", conf.current_maps_file.rnd_colors);
    maps->SetAttribute("rnd_races", conf.current_maps_file.rnd_races);
    maps->SetAttribute("conditions_wins", conf.current_maps_file.conditions_wins);
    maps->SetAttribute("wins1", conf.current_maps_file.wins1);
    maps->SetAttribute("wins2", conf.current_maps_file.wins2);
    maps->SetAttribute("wins3", conf.current_maps_file.wins3);
    maps->SetAttribute("wins4", conf.current_maps_file.wins4);
    maps->SetAttribute("conditions_loss", conf.current_maps_file.conditions_loss);
    maps->SetAttribute("loss1", conf.current_maps_file.loss1);
    maps->SetAttribute("loss2", conf.current_maps_file.loss2);
    node = new TiXmlElement("races");
    maps->LinkEndChild(node);
    node->SetAttribute("blue", conf.current_maps_file.races[0]);
    node->SetAttribute("green", conf.current_maps_file.races[1]);
    node->SetAttribute("red", conf.current_maps_file.races[2]);
    node->SetAttribute("yellow", conf.current_maps_file.races[3]);
    node->SetAttribute("orange", conf.current_maps_file.races[4]);
    node->SetAttribute("purple", conf.current_maps_file.races[5]);
    node = new TiXmlElement("name");
    maps->LinkEndChild(node);
    node->LinkEndChild(new TiXmlText(conf.current_maps_file.name.c_str()));
    node = new TiXmlElement("description");
    maps->LinkEndChild(node);
    node->LinkEndChild(new TiXmlText(conf.current_maps_file.description.c_str()));
    
    // game
    TiXmlElement* game = new TiXmlElement("game");
    root->LinkEndChild(game);
    game->SetAttribute("game_difficulty", conf.game_difficulty);
    game->SetAttribute("my_color", conf.my_color);
    game->SetAttribute("cur_color", conf.cur_color);
    game->SetAttribute("game_type", conf.game_type);
    game->SetAttribute("players_colors", conf.players_colors);
    game->SetAttribute("preferably_count_players", conf.preferably_count_players);
    game->SetAttribute("debug", conf.debug);

    // world
    TiXmlElement* wrld = new TiXmlElement("world");
    root->LinkEndChild(wrld);
    wrld->SetAttribute("width", world.width);
    wrld->SetAttribute("height", world.height);
    wrld->SetAttribute("ultimate", world.ultimate_artifact);
    wrld->SetAttribute("uniq", world.uniq0);
    wrld->SetAttribute("week", world.week_name);
    wrld->SetAttribute("heroes_cond_wins", world.heroes_cond_wins);
    wrld->SetAttribute("heroes_cond_loss", world.heroes_cond_loss);
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

	tile2->SetAttribute("maps_index", ii);
	tile2->SetAttribute("tile_index", tile.tile_index);
	tile2->SetAttribute("shape", tile.shape);
	tile2->SetAttribute("general", tile.general);
	tile2->SetAttribute("quantity1", tile.quantity1);
	tile2->SetAttribute("quantity2", tile.quantity2);
	tile2->SetAttribute("fogs", tile.fogs);
	tile2->SetAttribute("flags", tile.flags);

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

	hero2->SetAttribute("modes", hero.modes);
	hero2->SetAttribute("color", hero.color);
	hero2->SetAttribute("name", hero.name.c_str());
	hero2->SetAttribute("portrait", hero.portrait);
	hero2->SetAttribute("race", hero.race);
	hero2->SetAttribute("attack", hero.attack);
	hero2->SetAttribute("defense", hero.defence);
	hero2->SetAttribute("knowledge", hero.knowledge);
	hero2->SetAttribute("power", hero.power);
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
	    node->SetAttribute("id", hero.artifacts[ii].GetID());
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
	book->SetAttribute("enable", hero.spell_book.active ? "true" : "false");
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
	mage->SetAttribute("level", castle.mageguild.level);
	if(castle.mageguild.upgrade) mage->SetAttribute("upgrade", "true");
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

	kingdom2->SetAttribute("modes", kingdom.flags);
	kingdom2->SetAttribute("lost_town_days", kingdom.lost_town_days);
	kingdom2->SetAttribute("color", kingdom.color);
	kingdom2->SetAttribute("control", kingdom.control);
	kingdom2->SetAttribute("capital", (kingdom.ai_capital ? kingdom.ai_capital->GetIndex() : 0));

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

	// kingdoms->kingdom->recruits
	TiXmlElement* recruits = new TiXmlElement("recruits");
	kingdom2->LinkEndChild(recruits);
	recruits->SetAttribute("hero1", kingdom.recruits.GetID1());
	recruits->SetAttribute("hero2", kingdom.recruits.GetID2());

	// kingdoms->kingdom->puzzle
	TiXmlElement* puzzle = new TiXmlElement("puzzle");
	kingdom2->LinkEndChild(puzzle);
	puzzle->SetAttribute("value", kingdom.puzzle_maps.to_string<char,std::char_traits<char>,std::allocator<char> >().c_str());
    }

    // signs
    {
        TiXmlElement* signs = new TiXmlElement("signs");
	wrld->LinkEndChild(signs);
        std::map<u16, std::string>::const_iterator it1 = world.map_sign.begin();
	std::map<u16, std::string>::const_iterator it2 = world.map_sign.end();
	for(; it1 != it2; ++it1)
	{
	    node = new TiXmlElement("sign");
	    signs->LinkEndChild(node);
	    node->SetAttribute("index", (*it1).first);
	    node->LinkEndChild(new TiXmlText((*it1).second.c_str()));
	}
    }

    // captured object
    {
	TiXmlElement* captured = new TiXmlElement("captured");
	wrld->LinkEndChild(captured);
	std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it1 = world.map_captureobj.begin();
	std::map<u16, std::pair<MP2::object_t, Color::color_t> >::const_iterator it2 = world.map_captureobj.end();
	for(; it1 != it2; ++it1)
	{
	    node = new TiXmlElement("object");
	    captured->LinkEndChild(node);
	    node->SetAttribute("index", (*it1).first);
	    node->SetAttribute("id", (*it1).second.first);
	    node->SetAttribute("color", (*it1).second.second);
	}
    }

    // rumors
    {
	TiXmlElement* rumors = new TiXmlElement("rumors");
	wrld->LinkEndChild(rumors);

	std::vector<std::string>::const_iterator it1 = world.vec_rumors.begin();
	std::vector<std::string>::const_iterator it2 = world.vec_rumors.end();
	for(; it1 != it2; ++it1)
	{
	    node = new TiXmlElement("message");
	    rumors->LinkEndChild(node);
	    node->LinkEndChild(new TiXmlText((*it1).c_str()));
	}
    }

    // day events
    {
	TiXmlElement* events = new TiXmlElement("day_events");
	wrld->LinkEndChild(events);

	std::vector<GameEvent::Day *>::const_iterator it1 = world.vec_eventsday.begin();
	std::vector<GameEvent::Day *>::const_iterator it2 = world.vec_eventsday.end();
	for(; it1 != it2; ++it1) if(*it1)
	{
	    node = new TiXmlElement("event");
	    events->LinkEndChild(node);
	    node->SetAttribute("wood", (*it1)->resource.wood);
	    node->SetAttribute("mercury", (*it1)->resource.mercury);
	    node->SetAttribute("ore", (*it1)->resource.ore);
	    node->SetAttribute("sulfur", (*it1)->resource.sulfur);
	    node->SetAttribute("crystal", (*it1)->resource.crystal);
	    node->SetAttribute("gems", (*it1)->resource.gems);
	    node->SetAttribute("gold", (*it1)->resource.gold);
	    if((*it1)->computer) node->SetAttribute("computer", "true");
	    node->SetAttribute("first", (*it1)->first);
	    node->SetAttribute("subsequent", (*it1)->subsequent);
	    node->SetAttribute("colors", (*it1)->colors);
	    node->LinkEndChild(new TiXmlText((*it1)->message.c_str()));
	}
    }
    
    // coord events
    {
	TiXmlElement* events = new TiXmlElement("coord_events");
	wrld->LinkEndChild(events);

	std::vector<GameEvent::Coord *>::const_iterator it1 = world.vec_eventsmap.begin();
	std::vector<GameEvent::Coord *>::const_iterator it2 = world.vec_eventsmap.end();
	for(; it1 != it2; ++it1) if(*it1)
	{
	    node = new TiXmlElement("event");
	    events->LinkEndChild(node);
	    node->SetAttribute("index", (*it1)->index_map);
	    node->SetAttribute("wood", (*it1)->resource.wood);
	    node->SetAttribute("mercury", (*it1)->resource.mercury);
	    node->SetAttribute("ore", (*it1)->resource.ore);
	    node->SetAttribute("sulfur", (*it1)->resource.sulfur);
	    node->SetAttribute("crystal", (*it1)->resource.crystal);
	    node->SetAttribute("gems", (*it1)->resource.gems);
	    node->SetAttribute("gold", (*it1)->resource.gold);
	    node->SetAttribute("artifact", (*it1)->artifact);
	    if((*it1)->computer) node->SetAttribute("computer", "true");
	    if((*it1)->cancel) node->SetAttribute("cancel", "true");
	    node->SetAttribute("colors", (*it1)->colors);
	    node->LinkEndChild(new TiXmlText((*it1)->message.c_str()));
	}
    }
    
    // sphinx riddles
    {
	TiXmlElement* sphinxes = new TiXmlElement("sphinxes");
	wrld->LinkEndChild(sphinxes);

	std::vector<GameEvent::Riddle *>::const_iterator it1 = world.vec_riddles.begin();
	std::vector<GameEvent::Riddle *>::const_iterator it2 = world.vec_riddles.end();
	for(; it1 != it2; ++it1) if(*it1)
	{
	    TiXmlElement* riddle = new TiXmlElement("riddle");
	    sphinxes->LinkEndChild(riddle);
	    riddle->SetAttribute("index", (*it1)->index_map);
	    riddle->SetAttribute("wood", (*it1)->resource.wood);
	    riddle->SetAttribute("mercury", (*it1)->resource.mercury);
	    riddle->SetAttribute("ore", (*it1)->resource.ore);
	    riddle->SetAttribute("sulfur", (*it1)->resource.sulfur);
	    riddle->SetAttribute("crystal", (*it1)->resource.crystal);
	    riddle->SetAttribute("gems", (*it1)->resource.gems);
	    riddle->SetAttribute("gold", (*it1)->resource.gold);
	    riddle->SetAttribute("artifact", (*it1)->artifact);
	    if((*it1)->quiet) riddle->SetAttribute("quiet", "true");


	    TiXmlElement* answers = new TiXmlElement("answers");
	    riddle->LinkEndChild(answers);

	    std::vector<std::string>::const_iterator ita1 = (*it1)->answers.begin();
	    std::vector<std::string>::const_iterator ita2 = (*it1)->answers.end();

	    for(; ita1 != ita2; ++ita1)
	    {
		node = new TiXmlElement("answer");
		answers->LinkEndChild(node);
		node->LinkEndChild(new TiXmlText((*ita1).c_str()));
	    }

	    node = new TiXmlElement("message");
	    riddle->LinkEndChild(node);
	    node->LinkEndChild(new TiXmlText((*it1)->message.c_str()));
	}
    }

    doc.SaveFile(fn.c_str());
}

void Game::LoadXML(const std::string &fn)
{
    TiXmlDocument doc;
    if (!doc.LoadFile(fn.c_str())) return;

    TiXmlElement* root = doc.FirstChildElement();

    if(!root || std::strcmp("fheroes2", root->Value()))
    {
	Error::Verbose("Game::LoadXML: broken file " + fn);
	return;
    }

    TiXmlElement *node;
    TiXmlElement *maps = root->FirstChildElement("maps");
    TiXmlElement *game = root->FirstChildElement("game");
    TiXmlElement *wrld = root->FirstChildElement("world");

    if(!maps || !game || !wrld)
    {
	Error::Verbose("Game::LoadXML: broken file " + fn);
	return;
    }

    TiXmlElement *date = wrld->FirstChildElement("date");
    TiXmlElement *tiles = wrld->FirstChildElement("tiles");
    TiXmlElement *kingdoms = wrld->FirstChildElement("kingdoms");
    TiXmlElement *heroes = wrld->FirstChildElement("heroes");
    TiXmlElement *castles = wrld->FirstChildElement("castles");

    if(!date || !tiles || !kingdoms || !heroes || !castles)
    {
	Error::Verbose("Game::LoadXML: broken file " + fn);
	return;
    }

    // loading info
    Display & display = Display::Get();
    display.Fill(0, 0, 0);
    TextBox(_("Maps Loading..."), Font::BIG, Rect(0, display.h()/2, display.w(), display.h()/2));
    display.Flip();

    // prepare World
    world.Reset();

    int res;
    const char *str;
    Settings & conf = Settings::Get();
    conf.SetModes(Settings::LOADGAME);

    // fheroes2 version
    str = root->Attribute("version");
    // fheroes2 build
    root->Attribute("build", &res);

    // maps
    maps->Attribute("width", &res);
    conf.current_maps_file.size_w = res;
    maps->Attribute("height", &res);
    conf.current_maps_file.size_h = res;
    conf.current_maps_file.file = maps->Attribute("file");
    //
    node = maps->FirstChildElement("name");
    if(node && node->GetText()) conf.current_maps_file.name = node->GetText();
    //
    node = maps->FirstChildElement("description");
    if(node && node->GetText()) conf.current_maps_file.description = node->GetText();
    //
    node = maps->FirstChildElement("races");
    if(node)
    {
	node->Attribute("blue", &res);
	conf.current_maps_file.races[0] = res;
	node->Attribute("green", &res);
	conf.current_maps_file.races[1] = res;
	node->Attribute("red", &res);
	conf.current_maps_file.races[2] = res;
	node->Attribute("yellow", &res);
	conf.current_maps_file.races[3] = res;
	node->Attribute("orange", &res);
	conf.current_maps_file.races[4] = res;
	node->Attribute("purple", &res);
	conf.current_maps_file.races[5] = res;
    }

    //
    maps->Attribute("difficulty", &res);
    conf.current_maps_file.difficulty = Difficulty::Get(res);
    maps->Attribute("kingdom_colors", &res);
    conf.current_maps_file.kingdom_colors = res;
    maps->Attribute("allow_colors", &res);
    conf.current_maps_file.allow_colors = res;
    maps->Attribute("rnd_colors", &res);
    conf.current_maps_file.rnd_colors = res;
    maps->Attribute("rnd_races", &res);
    conf.current_maps_file.rnd_races = res;
    maps->Attribute("conditions_wins", &res);
    conf.current_maps_file.conditions_wins = res;
    maps->Attribute("wins1", &res);
    conf.current_maps_file.wins1 = res;
    maps->Attribute("wins2", &res);
    conf.current_maps_file.wins2 = res;
    maps->Attribute("wins3", &res);
    conf.current_maps_file.wins3 = res;
    maps->Attribute("wins4", &res);
    conf.current_maps_file.wins4 = res;
    maps->Attribute("conditions_loss", &res);
    conf.current_maps_file.conditions_loss = res;
    maps->Attribute("loss1", &res);
    conf.current_maps_file.loss1 = res;
    maps->Attribute("loss2", &res);
    conf.current_maps_file.loss2 = res;

    // game
    game->Attribute("game_difficulty", &res);
    conf.game_difficulty = Difficulty::Get(res);
    game->Attribute("my_color", &res);
    conf.my_color = Color::Get(res);
    game->Attribute("cur_color", &res);
    conf.cur_color = Color::Get(res);
    game->Attribute("game_type", &res);
    conf.game_type = Game::GetControl(res);
    game->Attribute("players_colors", &res);
    conf.players_colors = res;
    game->Attribute("preferably_count_players", &res);
    conf.preferably_count_players = res;
    game->Attribute("debug", &res);
    conf.debug = res;

    // world
    wrld->Attribute("width", &res);
    world.width = res;
    wrld->Attribute("height", &res);
    world.height = res;
    wrld->Attribute("ultimate", &res);
    world.ultimate_artifact = res;
    wrld->Attribute("uniq", &res);
    world.uniq0 = res;
    wrld->Attribute("week", &res);
    world.week_name = Week::Get(res);
    wrld->Attribute("heroes_cond_wins", &res);
    world.heroes_cond_wins = Heroes::ConvertID(res);
    wrld->Attribute("heroes_cond_loss", &res);
    world.heroes_cond_loss = Heroes::ConvertID(res);

    // world date
    date->Attribute("month", &res);
    world.month = res;
    date->Attribute("week", &res);
    world.week = res;
    date->Attribute("day", &res);
    world.day = res;

    // tiles
    tiles->Attribute("size", &res);
    world.vec_tiles.reserve(res);

    TiXmlElement *tile2 = tiles->FirstChildElement();
    u16 maps_index = 0;
    for(; tile2; tile2 = tile2->NextSiblingElement(), ++maps_index)
    {
	// load tile
	Maps::Tiles *tile = new Maps::Tiles(maps_index);

	tile2->Attribute("tile_index", &res);
	tile->tile_index = res;
	tile2->Attribute("shape", &res);
	tile->shape = res;
	tile2->Attribute("general", &res);
	tile->general = res;
	tile2->Attribute("quantity1", &res);
	tile->quantity1 = res;
	tile2->Attribute("quantity2", &res);
	tile->quantity2 = res;
	tile2->Attribute("fogs", &res);
	tile->fogs = res;
	tile2->Attribute("flags", &res);
	tile->flags = res;

	// tiles->tile->addons1
	TiXmlElement *addons = tile2->FirstChildElement("addons_level1");
	if(addons)
	{
	    Maps::TilesAddon ta;
	    node = addons->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		node->Attribute("level", &res);
		ta.level = res;
		node->Attribute("uniq", &res);
		ta.uniq = res;
		node->Attribute("object", &res);
		ta.object = res;
		node->Attribute("index", &res);
		ta.index = res;

		tile->addons_level1.push_back(ta);
	    }
	}

	// tiles->tile->addons2
	addons = tile2->FirstChildElement("addons_level2");
	if(addons)
	{
	    Maps::TilesAddon ta;
	    node = addons->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		node->Attribute("level", &res);
		ta.level = res;
		node->Attribute("uniq", &res);
		ta.uniq = res;
		node->Attribute("object", &res);
		ta.object = res;
		node->Attribute("index", &res);
		ta.index = res;

		tile->addons_level2.push_back(ta);
	    }
	}

	world.vec_tiles.push_back(tile);
    }

    // heroes size
    heroes->Attribute("size", &res);
    world.vec_heroes.reserve(res);

    TiXmlElement *hero2 = heroes->FirstChildElement();
    for(; hero2; hero2 = hero2->NextSiblingElement())
    {
	// load hero
	hero2->Attribute("portrait", &res);
	const Heroes::heroes_t id = Heroes::ConvertID(res);
	hero2->Attribute("race", &res);
	Heroes* hero = new Heroes(id, Race::Get(res));

	hero2->Attribute("modes", &res);
	hero->modes = res;
	hero2->Attribute("color", &res);
	hero->color = Color::Get(res);
	hero->name = hero2->Attribute("name");
	hero2->Attribute("attack", &res);
	hero->attack = res;
	hero2->Attribute("defense", &res);
	hero->defence = res;
	hero2->Attribute("power", &res);
	hero->power = res;
	hero2->Attribute("knowledge", &res);
	hero->knowledge = res;
	hero2->Attribute("experience", &res);
	hero->experience = res;
	hero2->Attribute("magic_point", &res);
	hero->magic_point = res;
	hero2->Attribute("move_point", &res);
	hero->move_point = res;
	hero2->Attribute("direction", &res);
	hero->direction = Direction::FromInt(res);
	hero2->Attribute("sprite_index", &res);
	hero->sprite_index = res;
	hero2->Attribute("save_maps_general", &res);
	hero->save_maps_general = static_cast<MP2::object_t>(static_cast<u8>(res));

	node = hero2->FirstChildElement("center");
	if(node)
	{
	    node->Attribute("x", &res);
	    hero->mp.x = res;
	    node->Attribute("y", &res);
	    hero->mp.y = res;
	}

	hero->secondary_skills.clear();
	TiXmlElement* skills = hero2->FirstChildElement("secondary_skills");
	if(skills)
	{
	    size_t ii = 0;
	    node = skills->FirstChildElement();
	    for(; node && ii < HEROESMAXSKILL; node = node->NextSiblingElement(), ++ii)
	    {
		Skill::Secondary skill;
		node->Attribute("skill", &res);
		skill.SetSkill(Skill::Secondary::Skill(res));
		node->Attribute("level", &res);
		skill.SetLevel(res);
		hero->secondary_skills.push_back(skill);
	    }
	}

	std::fill(hero->artifacts.begin(), hero->artifacts.end(), Artifact::UNKNOWN);
	TiXmlElement* artifacts = hero2->FirstChildElement("artifacts");
	if(artifacts)
	{
	    size_t ii = 0;
	    node = artifacts->FirstChildElement();
	    for(; node && ii < hero->artifacts.size(); node = node->NextSiblingElement(), ++ii)
	    {
	        node->Attribute("id", &res);
		hero->artifacts[ii].Set(Artifact::FromInt(res));
	    }
	}

	TiXmlElement* armies = hero2->FirstChildElement("armies");
	if(armies)
	{
	    size_t ii = 0;
	    node = armies->FirstChildElement();
	    for(; node && ii < ARMYMAXTROOPS; node = node->NextSiblingElement(), ++ii)
	    {
		node->Attribute("monster", &res);
		hero->army.At(ii).SetMonster(Monster::FromInt(res));
		node->Attribute("count", &res);
		hero->army.At(ii).SetCount(res);
	    }
	}

	TiXmlElement* book = hero2->FirstChildElement("spell_book");
	if(book)
	{
	    str = book->Attribute("enable");
	    hero->spell_book.spells_level1.clear();
	    hero->spell_book.spells_level2.clear();
	    hero->spell_book.spells_level3.clear();
	    hero->spell_book.spells_level4.clear();
	    hero->spell_book.spells_level5.clear();
	    hero->spell_book.active = (str && 0 == std::strcmp(str, "true"));

	    // load spells
	    node = book->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		node->Attribute("id", &res);
		const Spell::spell_t spell = Spell::FromInt(res);
		switch(Spell::Level(spell))
		{
		    case 1:	hero->spell_book.spells_level1.push_back(spell); break;
		    case 2:	hero->spell_book.spells_level2.push_back(spell); break;
		    case 3:	hero->spell_book.spells_level3.push_back(spell); break;
		    case 4:	hero->spell_book.spells_level4.push_back(spell); break;
		    case 5:	hero->spell_book.spells_level5.push_back(spell); break;
		    default: break;
		}
	    }
	}

	TiXmlElement *visit = hero2->FirstChildElement("visit_object");
	if(visit)
	{
	    node = visit->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		IndexObject io;
		node->Attribute("index", &res);
		io.first = res;
		node->Attribute("object", &res);
		io.second = static_cast<MP2::object_t>(static_cast<u8>(res));

		hero->visit_object.push_back(io);
	    }
	}

	world.vec_heroes.push_back(hero);
    }

    // castles size
    castles->Attribute("size", &res);
    world.vec_castles.reserve(res);

    TiXmlElement *castle2 = castles->FirstChildElement();
    for(; castle2; castle2 = castle2->NextSiblingElement())
    {
	Point center;
	node = castle2->FirstChildElement("center");
	if(node)
	{
	    node->Attribute("x", &res);
	    center.x = res;
	    node->Attribute("y", &res);
	    center.y = res;
	}

	castle2->Attribute("race", &res);
	// load castle
	Castle* castle = new Castle(center.x, center.y, Race::Get(res));

	castle2->Attribute("modes", &res);
	castle->modes = res;
	castle2->Attribute("color", &res);
	castle->color = Color::Get(res);
	castle->name = castle2->Attribute("name");
	castle2->Attribute("building", &res);
	castle->building = res;

	TiXmlElement* mage = castle2->FirstChildElement("mageguild");
	if(mage)
	{
	    mage->Attribute("level", &res);
	    castle->mageguild.level = res;
	    str = mage->Attribute("upgrade");
	    castle->mageguild.upgrade = (str && 0 == std::strcmp(str, "true"));

	    // load spells
	    node = mage->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		node->Attribute("id", &res);
		const Spell::spell_t spell = Spell::FromInt(res);
		switch(Spell::Level(spell))
		{
		    case 1:	castle->mageguild.spells_level1.push_back(spell); break;
		    case 2:	castle->mageguild.spells_level2.push_back(spell); break;
		    case 3:	castle->mageguild.spells_level3.push_back(spell); break;
		    case 4:	castle->mageguild.spells_level4.push_back(spell); break;
		    case 5:	castle->mageguild.spells_level5.push_back(spell); break;
		    default: break;
		}
	    }
	}

	TiXmlElement* armies = castle2->FirstChildElement("armies");
	if(armies)
	{
	    size_t ii = 0;
	    node = armies->FirstChildElement();
	    for(; node && ii < ARMYMAXTROOPS; node = node->NextSiblingElement(), ++ii)
	    {
		node->Attribute("monster", &res);
		castle->army.At(ii).SetMonster(Monster::FromInt(res));
		node->Attribute("count", &res);
		castle->army.At(ii).SetCount(res);
	    }
	}

	TiXmlElement* dwellings = castle2->FirstChildElement("dwellings");
	if(dwellings)
	{
	    size_t ii = 0;
	    node = dwellings->FirstChildElement();
	    for(; node && ii < castle->dwelling.size(); node = node->NextSiblingElement(), ++ii)
	    {
		node->Attribute("dwelling", &res);
		castle->dwelling[ii] = res;
	    }
	}

	world.vec_castles.push_back(castle);
    }

    // kingdoms size
    kingdoms->Attribute("size", &res);
    world.vec_kingdoms.reserve(res);

    TiXmlElement *kingdom2 = kingdoms->FirstChildElement();
    for(; kingdom2; kingdom2 = kingdom2->NextSiblingElement())
    {
	kingdom2->Attribute("color", &res);
	const Color::color_t color = Color::Get(res);

	kingdom2->Attribute("control", &res);
	// load kingdom
	Kingdom* kingdom = new Kingdom(color, Game::GetControl(res));

	kingdom2->Attribute("modes", &res);
	kingdom->flags = res;
	kingdom2->Attribute("lost_town_days", &res);
	kingdom->lost_town_days = res;
	kingdom2->Attribute("capital", &res);
	kingdom->ai_capital = (res ? world.GetCastle(res) : NULL);

	TiXmlElement *funds = kingdom2->FirstChildElement("funds");
	if(funds)
	{
	    funds->Attribute("wood", &res);
	    kingdom->resource.wood = res;
	    funds->Attribute("mercury", &res);
	    kingdom->resource.mercury = res;
	    funds->Attribute("ore", &res);
	    kingdom->resource.ore = res;
	    funds->Attribute("sulfur", &res);
	    kingdom->resource.sulfur = res;
	    funds->Attribute("crystal", &res);
	    kingdom->resource.crystal = res;
	    funds->Attribute("gems", &res);
	    kingdom->resource.gems = res;
	    funds->Attribute("gold", &res);
	    kingdom->resource.gold = res;
	}

	TiXmlElement *visit = kingdom2->FirstChildElement("visit_object");
	if(visit)
	{
	    node = visit->FirstChildElement();
	    for(; node; node = node->NextSiblingElement())
	    {
		IndexObject io;
		node->Attribute("index", &res);
		io.first = res;
		node->Attribute("object", &res);
		io.second = static_cast<MP2::object_t>(static_cast<u8>(res));

		kingdom->visit_object.push_back(io);
	    }
	}

	// kingdoms->kingdom->recruits
	TiXmlElement* recruits = kingdom2->FirstChildElement("recruits");
	if(recruits)
	{
	    recruits->Attribute("hero1", &res);
	    kingdom->recruits.SetID1(res);
	    recruits->Attribute("hero2", &res);
	    kingdom->recruits.SetID2(res);
	}

	// kingdoms->kingdom->puzzle
	TiXmlElement* puzzle = kingdom2->FirstChildElement("puzzle");
	if(puzzle)
	{
	    kingdom->puzzle_maps = puzzle->Attribute("value");
	}

	world.vec_kingdoms.push_back(kingdom);
    }

    // signs
    TiXmlElement* signs = wrld->FirstChildElement("signs");
    if(signs)
    {
	node = signs->FirstChildElement();
	for(; node; node = node->NextSiblingElement())
	{
	    node->Attribute("index", &res);
	    if(node->GetText()) world.map_sign[res] = node->GetText();
	}
    }

    // captured objects
    TiXmlElement* captured = wrld->FirstChildElement("captured");
    if(captured)
    {
	node = captured->FirstChildElement();
	for(; node; node = node->NextSiblingElement())
	{
	    node->Attribute("index", &res);
	    std::pair<MP2::object_t, Color::color_t> & value = world.map_captureobj[res];
	    node->Attribute("id", &res);
	    value.first = static_cast<MP2::object_t>(static_cast<u8>(res));
	    node->Attribute("color", &res);
	    value.second = Color::Get(res);
	}
    }

    // rumors
    TiXmlElement* rumors = wrld->FirstChildElement("rumors");
    if(rumors)
    {
	node = rumors->FirstChildElement();
	for(; node; node = node->NextSiblingElement()) if(node->GetText())
	{
	    world.vec_rumors.push_back(node->GetText());
	}
    }

    // day events
    TiXmlElement* devents = wrld->FirstChildElement("day_events");
    if(devents)
    {
	node = devents->FirstChildElement();
	for(; node; node = node->NextSiblingElement())
	{
	    GameEvent::Day *event = new GameEvent::Day();

	    node->Attribute("wood", &res);
	    event->resource.wood = res;
	    node->Attribute("mercury", &res);
	    event->resource.mercury = res;
	    node->Attribute("ore", &res);
	    event->resource.ore = res;
	    node->Attribute("sulfur", &res);
	    event->resource.sulfur = res;
	    node->Attribute("crystal", &res);
	    event->resource.crystal = res;
	    node->Attribute("gems", &res);
	    event->resource.gems = res;
	    node->Attribute("gold", &res);
	    event->resource.gold = res;
	    str = node->Attribute("computer");
	    event->computer = str && 0 == std::strcmp(str, "true");
	    node->Attribute("first", &res);
	    event->first = res;
	    node->Attribute("subsequent", &res);
	    event->subsequent = res;
	    node->Attribute("colors", &res);
	    event->colors = res;
	    if(node->GetText()) event->message = node->GetText();
	    
	    world.vec_eventsday.push_back(event);
	}
    }

    // coord events
    TiXmlElement* cevents = wrld->FirstChildElement("coord_events");
    if(cevents)
    {
	node = cevents->FirstChildElement();
	for(; node; node = node->NextSiblingElement())
	{
	    GameEvent::Coord *event = new GameEvent::Coord();

	    node->Attribute("index", &res);
	    event->index_map = res;
	    node->Attribute("wood", &res);
	    event->resource.wood = res;
	    node->Attribute("mercury", &res);
	    event->resource.mercury = res;
	    node->Attribute("ore", &res);
	    event->resource.ore = res;
	    node->Attribute("sulfur", &res);
	    event->resource.sulfur = res;
	    node->Attribute("crystal", &res);
	    event->resource.crystal = res;
	    node->Attribute("gems", &res);
	    event->resource.gems = res;
	    node->Attribute("gold", &res);
	    event->resource.gold = res;
	    node->Attribute("artifact", &res);
	    event->artifact = Artifact::FromInt(res);
	    str = node->Attribute("computer");
	    event->computer = str && 0 == std::strcmp(str, "true");
	    str = node->Attribute("cancel");
	    event->cancel = str && 0 == std::strcmp(str, "true");
	    node->Attribute("colors", &res);
	    event->colors = res;
	    if(node->GetText()) event->message = node->GetText();
	    
	    world.vec_eventsmap.push_back(event);
	}
    }

    // sphinx riddles
    TiXmlElement* sphinxes = wrld->FirstChildElement("sphinxes");
    if(sphinxes)
    {
	TiXmlElement *riddle2 = sphinxes->FirstChildElement();
	for(; riddle2; riddle2 = riddle2->NextSiblingElement())
	{
	    GameEvent::Riddle *riddle = new GameEvent::Riddle();

	    riddle2->Attribute("index", &res);
	    riddle->index_map = res;
	    riddle2->Attribute("wood", &res);
	    riddle->resource.wood = res;
	    riddle2->Attribute("mercury", &res);
	    riddle->resource.mercury = res;
	    riddle2->Attribute("ore", &res);
	    riddle->resource.ore = res;
	    riddle2->Attribute("sulfur", &res);
	    riddle->resource.sulfur = res;
	    riddle2->Attribute("crystal", &res);
	    riddle->resource.crystal = res;
	    riddle2->Attribute("gems", &res);
	    riddle->resource.gems = res;
	    riddle2->Attribute("gold", &res);
	    riddle->resource.gold = res;
	    riddle2->Attribute("artifact", &res);
	    riddle->artifact = Artifact::FromInt(res);
	    str = riddle2->Attribute("quiet");
	    riddle->quiet = str && 0 == std::strcmp(str, "true");

	    TiXmlElement *answers = riddle2->FirstChildElement("answers");
	    if(answers)
	    {
		node = answers->FirstChildElement();
		for(; node; node = node->NextSiblingElement()) if(node->GetText())
		{
		    riddle->answers.push_back(node->GetText());
		}
	    }
	    
	    node = riddle2->FirstChildElement("message");
	    if(node && node->GetText()) riddle->message = node->GetText();

	    world.vec_riddles.push_back(riddle);
	}
    }

    // sort castles to kingdoms
    std::vector<Castle *>::const_iterator itc1 = world.vec_castles.begin();
    std::vector<Castle *>::const_iterator itc2 = world.vec_castles.end();
    for(; itc1 != itc2; ++itc1)
        if(*itc1) world.GetKingdom((*itc1)->GetColor()).AddCastle(*itc1);

    // sort heroes to kingdoms
    std::vector<Heroes *>::const_iterator ith1 = world.vec_heroes.begin();
    std::vector<Heroes *>::const_iterator ith2 = world.vec_heroes.end();
    for(; ith1 != ith2; ++ith1)
        if(*ith1) world.GetKingdom((*ith1)->GetColor()).AddHeroes(*ith1);
}
