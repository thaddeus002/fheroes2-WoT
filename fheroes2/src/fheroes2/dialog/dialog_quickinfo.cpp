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
#include "maps.h"
#include "army.h"
#include "heroes.h"
#include "castle.h"
#include "monster.h"
#include "interface_gamearea.h"
#include "cursor.h"
#include "game_focus.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
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
    const Interface::GameArea & gamearea = Interface::GameArea::Get();
    const Rect ar(BORDERWIDTH, BORDERWIDTH, gamearea.GetArea().w, gamearea.GetArea().h);

    LocalEvent & le = LocalEvent::Get();
    const Point & mp = le.GetMouseCursor();
    
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
    const Settings & settings = Settings::Get();

    if(!settings.Debug() && tile.isFog(settings.MyColor()))
	name_object = _("Unchartered Territory");
    else
    switch(tile.GetObject())
    {
        case MP2::OBJ_MONSTER:
    	{
    	    switch(Army::GetSize(tile.GetCountMonster()))
    	    {
		case Army::FEW:		name_object = _("A few\n%{monster}"); break;
		case Army::SEVERAL:	name_object = _("Several\n%{monster}"); break;
		case Army::PACK:	name_object = _("A pack of\n%{monster}"); break;
		case Army::LOTS:	name_object = _("Lots of\n%{monster}"); break;
		case Army::HORDE:	name_object = _("A horde of\n%{monster}"); break;
		case Army::THRONG:	name_object = _("A throng of\n%{monster}"); break;
		case Army::SWARM:	name_object = _("A swarm of\n%{monster}"); break;
		case Army::ZOUNDS:	name_object = _("Zounds of\n%{monster}"); break;
		case Army::LEGION:	name_object = _("A legion of\n%{monster}"); break;
            }

            std::string name = Monster(tile).GetMultiName();
            String::Lower(name);
            String::Replace(name_object, "%{monster}", name);
    	}
    	    break;
        
        case MP2::OBJ_ABANDONEDMINE:
        {
            name_object = _("guarded by lots of %{ghosts}");
            std::string name = Monster(Monster::GHOST).GetMultiName();
            String::Lower(name);
            String::Replace(name_object, "%{ghosts}", name);
            break;
        }

        case MP2::OBJ_EVENT:
        case MP2::OBJ_ZERO:
    	    name_object = Maps::Ground::String(tile.GetGround());
    	    break;

	case MP2::OBJ_GAZEBO:
	case MP2::OBJ_FORT:
	case MP2::OBJ_MERCENARYCAMP:
	case MP2::OBJ_DOCTORHUT:
	case MP2::OBJ_STANDINGSTONES:
	    // check visited
	    if(Game::Focus::HEROES == Game::Focus::Get().Type())
	    {
	    	name_object.append("\n");
		name_object.append(Game::Focus::Get().GetHeroes().isVisited(tile) ? _("(already visited)") : _("(not visited)"));
	    }
	    break;

	case MP2::OBJ_FOUNTAIN:
	case MP2::OBJ_FAERIERING:
	case MP2::OBJ_IDOL:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_TEMPLE:
	case MP2::OBJ_BUOY:
	case MP2::OBJ_WATERINGHOLE:
	case MP2::OBJ_ARENA:
	case MP2::OBJ_STABLES:
	    // check visited
	    if(Game::Focus::HEROES == Game::Focus::Get().Type())
	    {
		name_object.append("\n");
		name_object.append(Game::Focus::Get().GetHeroes().isVisited(tile.GetObject()) ? _("(already visited)") : _("(not visited)"));
	    }
	    break;

	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	    // addons pack
	    if(!settings.Original() && world.GetKingdom(settings.MyColor()).isVisited(tile))
	    {
	    	name_object.append("\n(");
	    	name_object.append(Spell::GetName(Spell::FromInt(tile.GetQuantity1())));
	    	name_object.append(")");
	    }
	    break;

	case MP2::OBJ_WITCHSHUT:
	    // addons pack
	    if(!settings.Original() && world.GetKingdom(settings.MyColor()).isVisited(tile))
	    {
		name_object.append("\n(");
		name_object.append(Skill::Secondary::String(Skill::Secondary::Skill(tile.GetQuantity1())));
		name_object.append(")");
	    }
	    break;

        case MP2::OBJ_OBELISK:
            // check visited
	    name_object.append("\n");
	    name_object.append(world.GetKingdom(settings.MyColor()).isVisited(tile) ? _("(already visited)") : _("(not visited)"));
            break;

        default: break;
    }

    TextBox text(name_object, Font::SMALL, 118);
    text.Blit(pos.x + BORDERWIDTH + (pos.w - BORDERWIDTH - text.w()) / 2, pos.y + (pos.h - BORDERWIDTH - text.h()) / 2);

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MousePressRight());

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
    const Interface::GameArea & gamearea = Interface::GameArea::Get();
    const Rect ar(BORDERWIDTH, BORDERWIDTH, gamearea.GetArea().w, gamearea.GetArea().h);

    LocalEvent & le = LocalEvent::Get();
    const Point & mp = le.GetMouseCursor();
    
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
    Text text;

    // castle name
    text.Set(castle.GetName(), Font::SMALL);
    dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
    dst_pt.y = cur_rt.y + 5;
    text.Blit(dst_pt);

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
    text.Set(_("Defenders:"));
    dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
    dst_pt.y += sprite.h() + 5;
    text.Blit(dst_pt);

    u8 count = castle.GetArmy().GetCount();

    if(! count)
    {
	text.Set(_("None"));
	dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
	dst_pt.y += 45;
	text.Blit(dst_pt);
    }
    else
	castle.GetArmy().DrawMons32Line(cur_rt.x - 5, cur_rt.y + 100, 192);

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MousePressRight());

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
    const Interface::GameArea & gamearea = Interface::GameArea::Get();
    const Rect ar(BORDERWIDTH, BORDERWIDTH, gamearea.GetArea().w, gamearea.GetArea().h);

    LocalEvent & le = LocalEvent::Get();
    const Point & mp = le.GetMouseCursor();
    
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
    Text text;

    // heroes name
    text.Set(hero.GetName(), Font::SMALL);
    dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
    dst_pt.y = cur_rt.y + 2;
    text.Blit(dst_pt);

    // mini port heroes
    const Surface & port = hero.GetPortrait30x22();
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
    std::string message;

    // attack
    message = _("Attack");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += port.h() + 4;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetAttack());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // defense
    message = _("Defense");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetDefense());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // power
    message = _("Spell Power");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetPower());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // knowledge
    message = _("Knowledge");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetKnowledge());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);
    
    // spell point
    message = _("Spell Points");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 35;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetSpellPoints());
    message += "/";
    String::AddInt(message, hero.GetMaxSpellPoints());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // draw monster sprite in one string
    hero.GetArmy().DrawMons32Line(cur_rt.x - 5, cur_rt.y + 114, 160);

    cursor.Show();
    display.Flip();

    // quick info loop
    while(le.HandleEvents() && le.MousePressRight());

    // restore background
    cursor.Hide();
    back.Restore();
    cursor.Show();
    display.Flip();
}
