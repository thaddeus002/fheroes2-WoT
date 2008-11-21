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

#include "agg.h"
#include "localevent.h"
#include "text.h"
#include "maps.h"
#include "army.h"
#include "heroes.h"
#include "castle.h"
#include "tools.h"
#include "monster.h"
#include "gamearea.h"
#include "background.h"
#include "cursor.h"
#include "error.h"
#include "display.h"
#include "sprite.h"
#include "portrait.h"
#include "game_focus.h"
#include "spell.h"
#include "skill.h"
#include "world.h"
#include "settings.h"
#include "dialog.h"

void Dialog::QuickInfo(const Maps::Tiles & tile)
{
    Display & display = Display::Get();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    // preload
    const ICN::icn_t qwikinfo = ICN::QWIKINFO;

    AGG::PreloadObject(qwikinfo);

    // image box
    const Sprite &box = AGG::GetICN(qwikinfo, 0);

    const Rect ar(BORDERWIDTH, BORDERWIDTH, GameArea::w() * TILEWIDTH, GameArea::h() * TILEWIDTH);
    const Point & mp = LocalEvent::MouseCursor();
    
    Rect pos; 
    u16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    u16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
    my *= TILEWIDTH;

    // top left
    if(mx <= ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	pos = Rect(mx + TILEWIDTH, my + TILEWIDTH, box.w(), box.h());
    else
    // top right
    if(mx > ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	pos = Rect(mx - box.w(), my + TILEWIDTH, box.w(), box.h());
    else
    // bottom left
    if(mx <= ar.x + ar.w / 2 && my > ar.y + ar.h / 2)
	pos = Rect(mx + TILEWIDTH, my - box.h(), box.w(), box.h());
    else
    // bottom right
	pos = Rect(mx - box.w(), my - box.h(), box.w(), box.h());
    
    Background back(pos);
    back.Save();

    display.Blit(box, pos.x, pos.y);

    std::string name_object(MP2::StringObject(tile.GetObject()));
    std::string visit_status;
    const Settings & settings = Settings::Get();

    switch(tile.GetObject())
    {
        case MP2::OBJ_MONSTER:
    	{
    	    switch(Army::GetSize(Monster::GetSize(tile)))
    	    {
		case Army::FEW:		name_object = "A few "; break;
		case Army::SEVERAL:	name_object = "Several "; break;
		case Army::PACK:	name_object = "A pack of "; break;
		case Army::LOTS:	name_object = "Lots of "; break;
		case Army::HORDE:	name_object = "A horde of "; break;
		case Army::THRONG:	name_object = "A throng of "; break;
		case Army::SWARM:	name_object = "A swarm of "; break;
		case Army::ZOUNDS:	name_object = "Zounds of "; break;
		case Army::LEGION:	name_object = "A legion of "; break;
            }

            name_object += Monster::String(Monster::Monster(tile)) + "s";
    	}
    	    break;

        case MP2::OBJ_EVENT:
        case MP2::OBJ_ZERO:
    	    name_object = Maps::Ground::String(tile.GetGround());
    	    break;

	case MP2::OBJ_GAZEBO:
	case MP2::OBJ_FOUNTAIN:
	case MP2::OBJ_FAERIERING:
	case MP2::OBJ_IDOL:
	case MP2::OBJ_FORT:
	case MP2::OBJ_MERCENARYCAMP:
	case MP2::OBJ_DOCTORHUT:
	case MP2::OBJ_STANDINGSTONES:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_TEMPLE:
	case MP2::OBJ_BUOY:
	    // check visited
	    if(Game::Focus::HEROES == Game::Focus::Get().Type())
		visit_status = Game::Focus::Get().GetHeroes().isVisited(tile) ? "(already visited)" : "(not visited)";
	    break;

	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	    // addons pack
	    if(!settings.Original() && world.GetKingdom(settings.MyColor()).isVisited(tile))
		visit_status = "(" + Spell::String(world.SpellFromShrine(tile.GetIndex())) + ")";
	    break;

	case MP2::OBJ_WITCHSHUT:
	    // addons pack
	    if(!settings.Original() && world.GetKingdom(settings.MyColor()).isVisited(tile))
		visit_status = "(" + Skill::Secondary::String(world.SkillFromWitchsHut(tile.GetIndex())) + ")";
	    break;

        default: break;
    }

    if(visit_status.empty())
    {
	u16 tx = pos.x + BORDERWIDTH + (pos.w - BORDERWIDTH - Text::width(name_object, Font::SMALL)) / 2;
	u16 ty = pos.y + (pos.h - BORDERWIDTH - Text::height(name_object, Font::SMALL)) / 2;
	Text(name_object, Font::SMALL, tx, ty);
    }
    else
    {
    	u16 tx = pos.x + BORDERWIDTH + (pos.w - BORDERWIDTH - Text::width(name_object, Font::SMALL)) / 2;
	u16 ty = pos.y + (pos.h - BORDERWIDTH - Text::height(name_object, Font::SMALL)) / 2 - 7;
	Text(name_object, Font::SMALL, tx, ty);

	tx = pos.x + BORDERWIDTH + (pos.w - BORDERWIDTH - Text::width(visit_status, Font::SMALL)) / 2;
	ty += 15;
	Text(visit_status, Font::SMALL, tx, ty);
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MouseRight());

    // restore background
    cursor.Hide();
    back.Restore();
    cursor.Show();
    display.Flip();
}

void Dialog::QuickInfo(const Castle & castle)
{
    Display & display = Display::Get();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    const ICN::icn_t qwiktown = ICN::QWIKTOWN;
    AGG::PreloadObject(qwiktown);

    // image box
    const Sprite &box = AGG::GetICN(qwiktown, 0);

    const Rect ar(BORDERWIDTH, BORDERWIDTH, GameArea::w() * TILEWIDTH, GameArea::h() * TILEWIDTH);
    const Point & mp = LocalEvent::MouseCursor();
    
    Rect cur_rt; 
    u16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    u16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
    my *= TILEWIDTH;

    // top left
    if(mx <= ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my + TILEWIDTH, box.w(), box.h());
    else
    // top right
    if(mx > ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx - box.w(), my + TILEWIDTH, box.w(), box.h());
    else
    // bottom left
    if(mx <= ar.x + ar.w / 2 && my > ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my - box.h(), box.w(), box.h());
    else
    // bottom right
	cur_rt = Rect(mx - box.w(), my - box.h(), box.w(), box.h());
    
    Background back(cur_rt);
    back.Save();
    display.Blit(box, cur_rt.x, cur_rt.y);

    cur_rt = Rect(back.GetRect().x + 28 , back.GetRect().y + 12, 178, 140);
    Point dst_pt;
    std::string message;

    // castle name
    message = castle.GetName();
    dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
    dst_pt.y = cur_rt.y + 5;
    Text(message, Font::SMALL, dst_pt);

    u8 index = 0;

    switch(castle.GetRace())
    {
	case Race::KNGT: index = (castle.isCastle() ?  9 : 15); break;
	case Race::BARB: index = (castle.isCastle() ? 10 : 16); break;
	case Race::SORC: index = (castle.isCastle() ? 11 : 17); break;
	case Race::WRLK: index = (castle.isCastle() ? 12 : 18); break;
	case Race::WZRD: index = (castle.isCastle() ? 13 : 19); break;
	case Race::NECR: index = (castle.isCastle() ? 14 : 20); break;
	default: Error::Warning(": unknown race."); return;
    }
    
    // castle icon
    const Sprite & sprite = AGG::GetICN(ICN::LOCATORS, index);

    dst_pt.x = cur_rt.x + (cur_rt.w - sprite.w()) / 2;
    dst_pt.y += 18;
    display.Blit(sprite, dst_pt);

    // color flags
    switch(castle.GetColor())
    {
	case Color::BLUE:	index = 0; break;
	case Color::GREEN:	index = 2; break;
	case Color::RED:	index = 4; break;
	case Color::YELLOW:	index = 6; break;
	case Color::ORANGE:	index = 8; break;
	case Color::PURPLE:	index = 10; break;
	case Color::GRAY:	index = 12; break;
    }

    const Sprite & l_flag = AGG::GetICN(ICN::FLAG32, index);
    dst_pt.x = cur_rt.x + (cur_rt.w - 60) / 2 - l_flag.w();
    display.Blit(l_flag, dst_pt);

    const Sprite & r_flag = AGG::GetICN(ICN::FLAG32, index + 1);
    dst_pt.x = cur_rt.x + (cur_rt.w + 60) / 2;
    display.Blit(r_flag, dst_pt);

    // info
    message = std::string("Defenders:");
    dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
    dst_pt.y += sprite.h() + 5;
    Text(message, Font::SMALL, dst_pt);

    u8 count = Army::GetCountTroops(castle.GetArmy());

    if(! count)
    {
	message = std::string("None");
	dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
	dst_pt.y += 45;
	Text(message, Font::SMALL, dst_pt);
    }
    else
    {
        // draw in one string
        u8 current = 0;
	const Army::army_t & army = castle.GetArmy();

	for(u8 ii = 0; ii < ARMYMAXTROOPS; ++ii)
        {
	    if(Army::isValid(army.at(ii)))
	    {
		const Sprite & monster = AGG::GetICN(ICN::MONS32, army.at(ii).Monster());

                // align from count
		dst_pt.x = (cur_rt.w / ARMYMAXTROOPS - monster.w()) / 2 + cur_rt.x + current * cur_rt.w / count + ((cur_rt.w / ARMYMAXTROOPS) * (ARMYMAXTROOPS - count) / (2 * count));
		dst_pt.y = cur_rt.y + 85;
		// alignt from height sprite
		dst_pt.y += (monster.h() > 32 ? -(monster.h() - 32) : 32 - monster.h());
                display.Blit(monster, dst_pt);

		// count message
                message.clear();
		String::AddInt(message, army.at(ii).Count());
		dst_pt.x += (monster.w() - Text::width(message, Font::SMALL)) / 2;
		dst_pt.y = cur_rt.y + 118;
		Text(message, Font::SMALL, dst_pt);

        	current++;
            }
	}
    }																																																																																			
    LocalEvent & le = LocalEvent::GetLocalEvent();

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MouseRight());

    // restore background
    cursor.Hide();
    back.Restore();
    cursor.Show();
    display.Flip();
}

void Dialog::QuickInfo(const Heroes & hero)
{
    Display & display = Display::Get();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    const ICN::icn_t qwikhero = ICN::QWIKHERO;
    AGG::PreloadObject(qwikhero);

    // image box
    const Sprite &box = AGG::GetICN(qwikhero, 0);

    const Rect ar(BORDERWIDTH, BORDERWIDTH, GameArea::w() * TILEWIDTH, GameArea::h() * TILEWIDTH);
    const Point & mp = LocalEvent::MouseCursor();
    
    Rect cur_rt; 
    u16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    u16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
    my *= TILEWIDTH;

    // top left
    if(mx <= ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my + TILEWIDTH, box.w(), box.h());
    else
    // top right
    if(mx > ar.x + ar.w / 2 && my <= ar.y + ar.h / 2)
	cur_rt = Rect(mx - box.w(), my + TILEWIDTH, box.w(), box.h());
    else
    // bottom left
    if(mx <= ar.x + ar.w / 2 && my > ar.y + ar.h / 2)
	cur_rt = Rect(mx + TILEWIDTH, my - box.h(), box.w(), box.h());
    else
    // bottom right
	cur_rt = Rect(mx - box.w(), my - box.h(), box.w(), box.h());
    
    Background back(cur_rt);
    back.Save();

    display.Blit(box, cur_rt.x, cur_rt.y);

    cur_rt = Rect(back.GetRect().x + 28 , back.GetRect().y + 10, 146, 144);
    Point dst_pt;
    std::string message;

    // heroes name
    message = hero.GetName();
    dst_pt.x = cur_rt.x + (cur_rt.w - Text::width(message, Font::SMALL)) / 2;
    dst_pt.y = cur_rt.y + 2;
    Text(message, Font::SMALL, dst_pt);

    // mini port heroes
    const Surface & port = Portrait::Hero(hero, Portrait::SMALL);
    dst_pt.x = cur_rt.x + (cur_rt.w - port.w()) / 2;
    dst_pt.y += 15;
    display.Blit(port, dst_pt);

    // color flags
    u8 index = 0;

    switch(hero.GetColor())
    {
	case Color::BLUE:	index = 0; break;
	case Color::GREEN:	index = 2; break;
	case Color::RED:	index = 4; break;
	case Color::YELLOW:	index = 6; break;
	case Color::ORANGE:	index = 8; break;
	case Color::PURPLE:	index = 10; break;
	case Color::GRAY:	index = 12; break;
    }

    const Sprite & l_flag = AGG::GetICN(ICN::FLAG32, index);
    dst_pt.x = cur_rt.x + (cur_rt.w - 40) / 2 - l_flag.w();
    display.Blit(l_flag, dst_pt);

    const Sprite & r_flag = AGG::GetICN(ICN::FLAG32, index + 1);
    dst_pt.x = cur_rt.x + (cur_rt.w + 40) / 2;
    display.Blit(r_flag, dst_pt);

    // attack
    message = "Attack:";
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += port.h() + 4;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, hero.GetAttack());
    dst_pt.x += 75;
    Text(message, Font::SMALL, dst_pt);

    // defense
    message = "Defense:";
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, hero.GetDefense());
    dst_pt.x += 75;
    Text(message, Font::SMALL, dst_pt);

    // power
    message = "Spell Power:";
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, hero.GetPower());
    dst_pt.x += 75;
    Text(message, Font::SMALL, dst_pt);

    // knowledge
    message = "Knowledge:     ";
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, hero.GetKnowledge());
    dst_pt.x += 75;
    Text(message, Font::SMALL, dst_pt);
    
    // spell point
    message = "Spell Points:  ";
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    Text(message, Font::SMALL, dst_pt);

    message.clear();
    String::AddInt(message, hero.GetSpellPoints());
    message += "/";
    String::AddInt(message, hero.GetMaxSpellPoints());
    dst_pt.x += 75;
    Text(message, Font::SMALL, dst_pt);

    // get valid count army
    u8 count = Army::GetCountTroops(hero.GetArmy());

    // draw monster sprite in one string
    u8 current = 0;
    const Army::army_t & army = hero.GetArmy();

    Army::army_t::const_iterator it1 = army.begin();
    Army::army_t::const_iterator it2 = army.end();
    for(; it1 != it2; ++it1)
    {
    	if((*it1).isValid())
	{
	    const Sprite & monster = AGG::GetICN(ICN::MONS32, (*it1).Monster());

            // align from count
	    dst_pt.x = (cur_rt.w / ARMYMAXTROOPS - monster.w()) / 2 + cur_rt.x + current * cur_rt.w / count + ((cur_rt.w / ARMYMAXTROOPS) * (ARMYMAXTROOPS - count) / (2 * count));
	    dst_pt.y = cur_rt.y + 112;
	    // alignt from height sprite
	    dst_pt.y += (monster.h() > 32 ? -(monster.h() - 32) : 32 - monster.h());
            display.Blit(monster, dst_pt);

	    // count message
            message.clear();
	    String::AddInt(message, (*it1).Count());
	    dst_pt.x += (monster.w() - Text::width(message, Font::SMALL)) / 2;
	    dst_pt.y = cur_rt.y + 142;
	    Text(message, Font::SMALL, dst_pt);

    	    current++;
        }
    }
    
    LocalEvent & le = LocalEvent::GetLocalEvent();

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MouseRight());

    // restore background
    cursor.Hide();
    back.Restore();
    cursor.Show();
    display.Flip();
}
