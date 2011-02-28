/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "race.h"
#include "interface_gamearea.h"
#include "cursor.h"
#include "game_focus.h"
#include "world.h"
#include "settings.h"
#include "kingdom.h"
#include "spell.h"
#include "dialog.h"

std::string CountScoute(u32 count, u8 scoute)
{
    std::string str;
    switch(scoute)
    {
	case Skill::Level::BASIC:
	    String::AddInt(str, Rand::Get(count - count * 30 / 100, count));
	    str.append("-");
	    String::AddInt(str, Rand::Get(count, count + count * 30 / 100));
	    break;
	case Skill::Level::ADVANCED:
	    String::AddInt(str, Rand::Get(count - count * 14 / 100, count));
	    str.append("-");
	    String::AddInt(str, Rand::Get(count, count + count * 15 / 100));
	    break;
	case Skill::Level::EXPERT:
	    String::AddInt(str, count);
	    break;
	default:
	    str = Army::String(count);
	    break;
    }

    return str;
}

std::string ShowGuardiansInfo(const Maps::Tiles & tile, u8 scoute)
{
    std::string str;
    const Army::Troop troop(tile);

    if(MP2::OBJ_MINES == tile.GetObject())
        str = Maps::GetMinesName(tile.GetMinesType());
    else
	str = MP2::StringObject(tile.GetObject());

    str.append("\n");
    str.append(_("guarded by %{count} of %{monster}"));

    String::Replace(str, "%{monster}", String::Lower(troop.GetMultiName()));
    String::Replace(str, "%{count}", CountScoute(troop.GetCount(), scoute));

    return str;
}

std::string ShowMonsterInfo(const Maps::Tiles & tile, u8 scoute)
{
    std::string str;
    const Army::Troop troop(tile);

    if(scoute)
    {
        str = "%{count} %{monster}";
	String::Replace(str, "%{count}", CountScoute(troop.GetCount(), scoute));
    }
    else
    switch(Army::GetSize(troop.GetCount()))
    {
        default: break;
        case Army::FEW:     str = _("A few\n%{monster}"); break;
        case Army::SEVERAL: str = _("Several\n%{monster}"); break;
        case Army::PACK:    str = _("A pack of\n%{monster}"); break;
        case Army::LOTS:    str = _("Lots of\n%{monster}"); break;
        case Army::HORDE:   str = _("A horde of\n%{monster}"); break;
        case Army::THRONG:  str = _("A throng of\n%{monster}"); break;
        case Army::SWARM:   str = _("A swarm of\n%{monster}"); break;
        case Army::ZOUNDS:  str = _("Zounds of\n%{monster}"); break;
        case Army::LEGION:  str = _("A legion of\n%{monster}"); break;
    }
    String::Replace(str, "%{monster}", String::Lower(troop.GetMultiName()));

    return str;
}

std::string ShowArtifactInfo(const Maps::Tiles & tile, bool show)
{
    std::string str = MP2::StringObject(tile.GetObject());

    if(show)
    {
	str.append("\n(");
	str.append(Artifact(tile.GetQuantity1()).GetName());
	str.append(")");
    }

    return str;
}

std::string ShowResourceInfo(const Maps::Tiles & tile, bool show, u8 scoute)
{
    std::string str = MP2::StringObject(tile.GetObject());

    if(show)
    {
	str.append("\n(");
	str.append(Resource::String(tile.GetQuantity1()));

	if(scoute)
	{
	    str.append(": ");
	    str.append(CountScoute(tile.GetQuantity2(), scoute));
	}
	str.append(")");
    }

    return str;
}

std::string ShowDwellingInfo(const Maps::Tiles & tile, u8 scoute)
{
    std::string str = MP2::StringObject(tile.GetObject());

    if(scoute)
    {
	str.append("\n");
	if(tile.GetCountMonster())
	{
	    str.append(_("(available: %{count})"));
	    String::Replace(str, "%{count}", CountScoute(tile.GetCountMonster(), scoute));
	}
	else
	    str.append("(empty)");
    }

    return str;
}

std::string ShowShrineInfo(const Maps::Tiles & tile, const Heroes* hero, u8 scoute)
{
    std::string str = MP2::StringObject(tile.GetObject());
    bool show = false;

    switch(tile.GetObject())
    {
	case MP2::OBJ_SHRINE1: show = scoute >= Skill::Level::BASIC; break;
	case MP2::OBJ_SHRINE2: show = scoute >= Skill::Level::ADVANCED; break;
	case MP2::OBJ_SHRINE3: show = scoute == Skill::Level::EXPERT; break;
	default: break;
    }

    if(show)
    {
	const Spell spell(tile.GetQuantity1());
	str.append("\n(");
	str.append(spell.GetName());
	str.append(")");
	if(hero && hero->HaveSpell(spell))
	{
	    str.append("\n(");
	    str.append(_("already learned"));
	    str.append(")");
	}
    }

    return str;
}

std::string ShowWitchHutInfo(const Maps::Tiles & tile, const Heroes* hero, bool show)
{
    std::string str = MP2::StringObject(tile.GetObject());

    if(show)
    {
	const Skill::Secondary::skill_t skill = Skill::Secondary::Skill(tile.GetQuantity1());
	str.append("\n(");
	str.append(Skill::Secondary::String(skill));
	str.append(")");

	if(hero)
	{
	    if(hero->HasSecondarySkill(skill))
	    {
		str.append("\n(");
		str.append(_("already knows this skill"));
		str.append(")");
	    }
	    else
	    if(hero->HasMaxSecondarySkill())
	    {
		str.append("\n(");
		str.append(_("already has max skills"));
		str.append(")");
	    }
	}
    }

    return str;
}

std::string ShowLocalVisitTileInfo(const Maps::Tiles & tile, const Heroes* hero)
{
    std::string str = MP2::StringObject(tile.GetObject());
    if(hero)
    {
	str.append("\n");
	str.append(hero->isVisited(tile) ? _("(already visited)") : _("(not visited)"));
    }

    return str;
}

std::string ShowLocalVisitObjectInfo(const Maps::Tiles & tile, const Heroes* hero)
{
    std::string str = MP2::StringObject(tile.GetObject());
    if(hero)
    {
	str.append("\n");
	str.append(hero->isVisited(tile.GetObject()) ? _("(already visited)") : _("(not visited)"));
    }

    return str;
}

std::string ShowGlobalVisitInfo(const Maps::Tiles & tile, const Kingdom & kingdom)
{
    std::string str = MP2::StringObject(tile.GetObject());
    str.append("\n");
    str.append(kingdom.isVisited(tile) ? _("(already visited)") : _("(not visited)"));

    return str;
}

std::string ShowGlobalVisitInfo(const Maps::Tiles & tile, const Kingdom & kingdom, bool ext)
{
    std::string str = MP2::StringObject(tile.GetObject());
    if(ext && kingdom.isVisited(tile))
    {
	str.append("\n");
	str.append(_("(already visited)"));
    }

    return str;
}

std::string ShowBarrierTentInfo(const Maps::Tiles & tile, const Kingdom & kingdom)
{
    std::string str = BarrierColor::String(tile.GetQuantity1());
    str.append(" ");
    str.append(MP2::StringObject(tile.GetObject()));

    if(MP2::OBJ_TRAVELLERTENT == tile.GetObject() &&
	kingdom.IsVisitTravelersTent(tile.GetQuantity1()))
    {
	str.append("\n");
	str.append(_("(already visited)"));
    }

    return str;
}

std::string ShowTreasureChestInfo(const Maps::Tiles & tile)
{
    std::string str = Maps::Ground::WATER == tile.GetGround() ? 
		    _("Sea Chest") : MP2::StringObject(MP2::OBJ_TREASURECHEST);

    return str;
}

void Dialog::QuickInfo(const Maps::Tiles & tile)
{
    // check 
    switch(tile.GetObject())
    {
	case MP2::OBJN_MINES:
	case MP2::OBJN_ABANDONEDMINE:
	case MP2::OBJN_SAWMILL:
	case MP2::OBJN_ALCHEMYLAB:
	{
	    const Maps::Tiles & left  = world.GetTiles(tile.GetIndex() - 1);
	    const Maps::Tiles & right = world.GetTiles(tile.GetIndex() + 1);
	    const Maps::Tiles* center = NULL;

	    if(MP2::isGroundObject(left.GetObject()))  center = &left;
	    else
	    if(MP2::isGroundObject(right.GetObject())) center = &right;

	    if(center)
	    {
		QuickInfo(*center);
		return;
	    }
	}
	break;

	default: break;
    }

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
    s16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    s16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
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

    std::string name_object;
    const Settings & settings = Settings::Get();

    const Heroes* from_hero = (Game::Focus::HEROES == Game::Focus::Get().Type() ? &Game::Focus::Get().GetHeroes() : NULL);
    const Kingdom & kingdom = world.GetKingdom(settings.MyColor());
    u8 scoute = from_hero ? from_hero->CanScouteTile(tile.GetIndex()) : 0;
    const bool & show = settings.ExtShowVisitedContent();

    if(tile.isFog(settings.MyColor()))
	name_object = _("Unchartered Territory");
    else
    // check guardians mine
    if(MP2::OBJ_ABANDONEDMINE == tile.GetObject() ||
	tile.CaptureObjectIsProtection(settings.MyColor()))
    {
	name_object = ShowGuardiansInfo(tile,
		(settings.MyColor() == world.ColorCapturedObject(tile.GetIndex()) ? Skill::Level::EXPERT : scoute));
    }
    else
    switch(tile.GetObject())
    {
        case MP2::OBJ_MONSTER:
	    name_object = ShowMonsterInfo(tile, scoute);
    	    break;

        case MP2::OBJ_EVENT:
        case MP2::OBJ_ZERO:
    	    name_object = Maps::Ground::String(tile.GetGround());
    	    break;

	case MP2::OBJ_DERELICTSHIP:
	case MP2::OBJ_SHIPWRECK:
	case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DAEMONCAVE:
	case MP2::OBJ_PYRAMID:
	case MP2::OBJ_WAGON:
	case MP2::OBJ_SKELETON:
	case MP2::OBJ_WINDMILL:
	case MP2::OBJ_WATERWHEEL:
	case MP2::OBJ_LEANTO:
	case MP2::OBJ_MAGICGARDEN:
	    name_object = ShowGlobalVisitInfo(tile, kingdom, show);
	    break;

	case MP2::OBJ_CAMPFIRE:
	    name_object = ShowResourceInfo(tile, scoute, scoute);
	    break;

	case MP2::OBJ_RESOURCE:
	    name_object = ShowResourceInfo(tile, show || scoute, scoute);
	    break;

	case MP2::OBJ_ARTIFACT:
	    name_object = ShowArtifactInfo(tile, show || scoute);
	    break;

	case MP2::OBJ_MINES:
            name_object = Maps::GetMinesName(tile.GetMinesType());
	    break;

        // join army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
        // recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_DESERTTENT:
        // battle and recruit army
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:
	    name_object = ShowDwellingInfo(tile, (kingdom.isVisited(tile) ? Skill::Level::EXPERT : scoute));
	    break;

	case MP2::OBJ_GAZEBO:
	case MP2::OBJ_FORT:
	case MP2::OBJ_XANADU:
	case MP2::OBJ_MERCENARYCAMP:
	case MP2::OBJ_DOCTORHUT:
	case MP2::OBJ_STANDINGSTONES:
	case MP2::OBJ_ARTESIANSPRING:
	case MP2::OBJ_TREEKNOWLEDGE:
	    name_object = ShowLocalVisitTileInfo(tile, from_hero);
	    break;

	case MP2::OBJ_MAGICWELL:
	case MP2::OBJ_FOUNTAIN:
	case MP2::OBJ_FAERIERING:
	case MP2::OBJ_IDOL:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_TEMPLE:
	case MP2::OBJ_BUOY:
	case MP2::OBJ_WATERINGHOLE:
	case MP2::OBJ_ARENA:
	case MP2::OBJ_STABLES:
	    name_object = ShowLocalVisitObjectInfo(tile, from_hero);
	    break;

	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
	case MP2::OBJ_SHRINE3:
	    name_object = ShowShrineInfo(tile, from_hero,
			    (show && kingdom.isVisited(tile) ? Skill::Level::EXPERT : scoute));
	    break;

	case MP2::OBJ_WITCHSHUT:
	    name_object = ShowWitchHutInfo(tile, from_hero,
			    ((show && kingdom.isVisited(tile)) || scoute == Skill::Level::EXPERT));
	    break;

        case MP2::OBJ_OBELISK:
	    name_object = ShowGlobalVisitInfo(tile, kingdom);
            break;

        case MP2::OBJ_BARRIER:
        case MP2::OBJ_TRAVELLERTENT:
	    name_object = ShowBarrierTentInfo(tile, kingdom);
	    break;

	case MP2::OBJ_TREASURECHEST:
	    name_object = ShowTreasureChestInfo(tile);
	    break;

        default: 
	    name_object = MP2::StringObject(tile.GetObject());
	    break;
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
    s16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    s16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
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

    if(Settings::Get().QVGA())
    {
	cur_rt = Rect((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    }

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
	default: DEBUG(DBG_GAME, DBG_WARN, "unknown race"); return;
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
	default: break;
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

    //
    u8 count = castle.GetArmy().GetCount();
    const Settings & conf = Settings::Get();

    bool hide_count, hide_guardians;

    hide_count = hide_guardians = conf.MyColor() != castle.GetColor() &&
		    !conf.IsUnions(conf.MyColor(), castle.GetColor());

    const Heroes* from_hero = Game::Focus::HEROES == Game::Focus::Get().Type() ?
			    &Game::Focus::Get().GetHeroes() : NULL;

    // show guardians (scouting: advanced)
    if(hide_guardians &&
	from_hero &&
	Skill::Level::ADVANCED <= from_hero->GetSecondaryValues(Skill::Secondary::SCOUTING))
		hide_guardians = false;

    if(hide_count && from_hero && from_hero->CanScouteTile(castle.GetIndex()))
	hide_count = false;

    // draw guardian portrait
    const Heroes* guardian = castle.GetHeroes().Guard();
    if(guardian &&
	! hide_guardians)
    {
	// heroes name
	text.Set(guardian->GetName(), Font::SMALL);
	dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
	dst_pt.y += 10;
	text.Blit(dst_pt);

	// mini port heroes
	const Surface & port = guardian->GetPortrait30x22();
	dst_pt.x = cur_rt.x + (cur_rt.w - port.w()) / 2;
	dst_pt.y += 15;
	display.Blit(port, dst_pt);
    }

    // draw defenders
    if(! count)
    {
	text.Set(_("None"));
	dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
	dst_pt.y += 45;
	text.Blit(dst_pt);
    }
    else
	castle.GetArmy().DrawMons32Line(cur_rt.x - 5, cur_rt.y + 100, 192, 0, 0, hide_count);

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
    s16 mx = (mp.x - BORDERWIDTH) / TILEWIDTH;
    mx *= TILEWIDTH;
    s16 my = (mp.y - BORDERWIDTH) / TILEWIDTH;
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

    if(Settings::Get().QVGA())
    {
	cur_rt = Rect((display.w() - box.w()) / 2, (display.h() - box.h()) / 2, box.w(), box.h());
    }
    
    Background back(cur_rt);
    back.Save();

    display.Blit(box, cur_rt.x, cur_rt.y);

    cur_rt = Rect(back.GetRect().x + 28 , back.GetRect().y + 10, 146, 144);
    Point dst_pt;
    Text text;

    // heroes name
    text.Set(hero.GetName(), Font::SMALL);
    dst_pt.x = cur_rt.x + (cur_rt.w - text.w()) / 2;
    dst_pt.y = cur_rt.y;
    text.Blit(dst_pt);

    // mini port heroes
    const Surface & port = hero.GetPortrait30x22();
    dst_pt.x = cur_rt.x + (cur_rt.w - port.w()) / 2;
    dst_pt.y += 13;
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
	default: break;
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
    dst_pt.x = cur_rt.x + 20;
    dst_pt.y += port.h();
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
    dst_pt.x = cur_rt.x + 20;
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
    dst_pt.x = cur_rt.x + 20;
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
    dst_pt.x = cur_rt.x + 20;
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
    dst_pt.x = cur_rt.x + 20;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetSpellPoints());
    message += "/";
    String::AddInt(message, hero.GetMaxSpellPoints());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // move point
    message = _("Move Points");
    message += ":";
    text.Set(message);
    dst_pt.x = cur_rt.x + 20;
    dst_pt.y += 12;
    text.Blit(dst_pt);

    message.clear();
    String::AddInt(message, hero.GetMovePoints());
    message += "/";
    String::AddInt(message, hero.GetMaxMovePoints());
    text.Set(message);
    dst_pt.x += 75;
    text.Blit(dst_pt);

    // draw monster sprite in one string
    const Settings & conf = Settings::Get();
    bool hide_count = conf.MyColor() != hero.GetColor() && !conf.IsUnions(conf.MyColor(), hero.GetColor());

    const Heroes* from_hero = Game::Focus::HEROES == Game::Focus::Get().Type() ?
			    &Game::Focus::Get().GetHeroes() : NULL;
    if(hide_count && from_hero && from_hero->CanScouteTile(hero.GetIndex()))
	hide_count = false;

    hero.GetArmy().DrawMons32Line(cur_rt.x - 5, cur_rt.y + 114, 160, 0, 0, hide_count);

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
