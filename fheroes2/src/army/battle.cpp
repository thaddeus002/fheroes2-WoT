/***************************************************************************
 *   Copyright (C) 2008 by Vasily Makarov                                  *
 *   drmoriarty@rambler.ru                                                 *
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


#include "battle.h"
#include "localevent.h"
#include "display.h"
#include "agg.h"
#include "sprite.h"
#include "ground.h"
#include "world.h"
#include "m82.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "error.h"
#include "tools.h"
#include "rand.h"

#define CELLW 44
#define CELLH 42
#define BFX 86
#define BFY 108

#define display Display::Get()

#define do_button(b, lf, rt) \
if(LocalEvent::GetLocalEvent().MousePressLeft(b)) b.PressDraw(); \
else b.ReleaseDraw(); \
if(LocalEvent::GetLocalEvent().MouseClickLeft(b)) lf; \
if(LocalEvent::GetLocalEvent().MousePressRight(b)) rt;

#define do_rbutton(b, lf, rt) \
if(LocalEvent::GetLocalEvent().MouseClickLeft(b)) { b.isPressed() ? b.ReleaseDraw() : b.PressDraw(); lf; } \
if(LocalEvent::GetLocalEvent().MousePressRight(b)) rt;

namespace Army {
    bool O_GRID = false, O_SHADM = false, O_SHADC = false;
    void DrawBackground(const Maps::Tiles & tile, const Point & dst_pt);
    void DrawCaptain(const Race::race_t race, const Point & dst_pt, u16 animframe, bool reflect=false);
    Rect DrawHero(const Heroes & hero, const Point & dst_pt, u16 animframe, bool reflect=false);
    void DrawArmy(std::vector<Army::Troops> & army, const Point & dst_pt, bool reflect=false, int animframe=-1);
    //void DrawMonster(const Army::Troops & troop, const Point & dst_pt, bool reflect=false, int animframe=-1);
    void InitArmyPosition(std::vector<Army::Troops> & army, bool compact, bool reflect=false);
    inline Point Scr2Bf(const Point & pt); // translate coordinate to batle field
    inline Point Bf2Scr(const Point & pt); // translate to screen (offset to frame border)
    //void AnimateMonster(Army::Troops & troop, Monster::animstate_t as = Monster::AS_NONE);
    void SettingsDialog();
    void HeroStatus();

    void Temp(int, int, Point);
}

bool Army::Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile)
{
    //CreateBackground(tile);
    LocalEvent & le = LocalEvent::GetLocalEvent();
    // dialog menu loop
    while(le.HandleEvents())
	{
	    // exit
	    if(le.KeyPress(KEY_ESCAPE)) {
		Display::Get().Fade();
		return true;
	    }	
	}
    return false;	
}

bool Army::Battle(Heroes& hero, std::vector<Army::Troops>& army, const Maps::Tiles & tile)
{
    Cursor & cursor = Cursor::Get();
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    //Display & display = Display::Get();
    Dialog::FrameBorder frameborder;

    const Point dst_pt(frameborder.GetArea().x, frameborder.GetArea().y);
    //Point dst_pt(cur_pt);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    DrawBackground(tile, dst_pt);
    const Rect rectHero = DrawHero(hero, dst_pt, 1);
    InitArmyPosition(hero.Army(), false);
    InitArmyPosition(army, false, true);
    DrawArmy(hero.Army(), dst_pt, false, 1);
    DrawArmy(army, dst_pt, true, 1);

    Button buttonSkip(dst_pt.x + 640-48, dst_pt.y + 480-36, ICN::TEXTBAR, 0, 1);
    Button buttonAuto(dst_pt.x, dst_pt.y + 480-36, ICN::TEXTBAR, 4, 5);
    Button buttonSettings(dst_pt.x, dst_pt.y + 480-18, ICN::TEXTBAR, 6, 7);

    Dialog::StatusBar statusBar1(Point(dst_pt.x + 48, dst_pt.y + 480-36), AGG::GetICN(ICN::TEXTBAR, 8), Font::BIG);
    statusBar1.Clear(" ");
    Dialog::StatusBar statusBar2(Point(dst_pt.x + 48, dst_pt.y + 480-18), AGG::GetICN(ICN::TEXTBAR, 9), Font::BIG);
    statusBar2.Clear(" ");

    LocalEvent & le = LocalEvent::GetLocalEvent();
    
    buttonSkip.Draw();
    buttonAuto.Draw();
    buttonSettings.Draw();

    display.Flip();
    cursor.Show();

    int i = 0;
    u16 animat = 0;
    // dialog menu loop
    while(le.HandleEvents()) {
	// exit
	if(le.KeyPress(KEY_ESCAPE)) {
	    Display::Get().Fade();
	    return true;
	}
	if(le.KeyPress(KEY_RETURN)) {
	    DrawBackground(tile, dst_pt);
	    display.Blit(AGG::GetICN(ICN::TEXTBAR, i), dst_pt);
	    army[0].SetCount(i);
	    DrawHero(hero, dst_pt, i);
	    DrawArmy(hero.Army(), dst_pt, false, i);
	    DrawArmy(army, dst_pt, true, i++);
	    cursor.Show();
	    display.Flip();
	}
	if(le.KeyPress(SDLK_SPACE)) {
	    DrawBackground(tile, dst_pt);
	    Temp(0, 1, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_PLUS)) {
	    DrawBackground(tile, dst_pt);
	    Temp(1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_MINUS)) {
	    DrawBackground(tile, dst_pt);
	    Temp(-1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(!(++animat%ANIMATION_LOW) && !i) {
	    cursor.Hide();
	    DrawBackground(tile, dst_pt);
	    DrawHero(hero, dst_pt, 1);
	    //for(unsigned int i=0; i < army.size(); i++) AnimateMonster(army[i]);
	    //for(unsigned int i=0; i < hero.Army().size(); i++) AnimateMonster(hero.Army()[i]);
	    DrawArmy(hero.Army(), dst_pt, false);
	    DrawArmy(army, dst_pt, true);	
	    cursor.Show();
	    display.Flip();
	}
	do_button(buttonSkip, {}, {});
	do_button(buttonAuto, {}, {});
	do_button(buttonSettings, SettingsDialog(), {});
	if(le.MouseClickLeft(rectHero)) HeroStatus();
        if(le.MouseCursor(buttonSkip)) {
	    statusBar2.ShowMessage("Skip this unit");
	    cursor.SetThemes(cursor.WAR_POINTER);
        }else if(le.MouseCursor(buttonAuto)) {
	    statusBar2.ShowMessage("Auto combat");
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(buttonSettings)) {
	    statusBar2.ShowMessage("Customize system options");
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(rectHero)) {
	    statusBar2.ShowMessage("Hero's Options");
	    cursor.SetThemes(cursor.WAR_HELMET);
	} else {
	    statusBar2.ShowMessage(" ");
	    cursor.SetThemes(cursor.WAR_POINTER);
	}
    }
    return false;	
}

bool Army::Battle(std::vector<Army::Troops>& army1, std::vector<Army::Troops> army2, const Maps::Tiles & tile)
{
   //CreateBackground(tile);
	LocalEvent & le = LocalEvent::GetLocalEvent();
	// dialog menu loop
	while(le.HandleEvents())
	   {
	      // exit
	      if(le.KeyPress(KEY_ESCAPE)) {
		 Display::Get().Fade();
		 return true;
	      }
	   }
	return false;	
}

void Army::DrawBackground(const Maps::Tiles & tile, const Point & dst_pt)
{
    AGG::PlaySound(M82::PREBATTL);
    //Display & display = Display::Get();
    ICN::icn_t icn;
    bool trees = false;
    u16 index = tile.GetIndex();
    u16 x = index%world.w(), y = index/world.w();
    if((x < world.w()-1 && world.GetTiles(index+1).GetObject() == MP2::OBJ_TREES) ||
       (x > 0 && world.GetTiles(index-1).GetObject() == MP2::OBJ_TREES) ||
       (y < world.h()-1 && world.GetTiles(index+world.w()).GetObject() == MP2::OBJ_TREES) ||
       (y > 0 && world.GetTiles(index-world.w()).GetObject() == MP2::OBJ_TREES) )
	trees = true;
    if(tile.GetObject() == MP2::OBJN_GRAVEYARD)
	icn = ICN::CBKGGRAV;
    else switch(tile.GetGround()) {
	case Maps::Ground::DESERT: icn = ICN::CBKGDSRT; break;
	case Maps::Ground::SNOW: 
	    icn = trees ? ICN::CBKGSNTR : ICN::CBKGSNMT; break;
	case Maps::Ground::SWAMP: icn = ICN::CBKGSWMP; break;
	case Maps::Ground::WASTELAND: icn = ICN::CBKGCRCK; break;
	case Maps::Ground::BEACH: icn = ICN::CBKGBEAC; break;
	case Maps::Ground::LAVA: icn = ICN::CBKGLAVA; break;
	case Maps::Ground::DIRT: 
	    icn = trees ? ICN::CBKGDITR : ICN::CBKGDIMT; break;
	case Maps::Ground::GRASS:
	    icn = trees ? ICN::CBKGGRTR : ICN::CBKGGRMT; break;
	case Maps::Ground::WATER: icn = ICN::CBKGWATR; break;
	default:
	    return;
	}
    display.Blit(AGG::GetICN(icn, 0), dst_pt);
    // draw grid
    if(O_GRID) {
	// TODO
    }
}

void Army::DrawCaptain(const Race::race_t race, const Point & dst_pt, u16 animframe, bool reflect)
{
    //Display & display = Display::Get();
    ICN::icn_t cap;
    switch(race) {
    case Race::BARB: cap = ICN::CMBTCAPB; break;
    case Race::KNGT: cap = ICN::CMBTCAPK; break;
    case Race::NECR: cap = ICN::CMBTCAPN; break;
    case Race::SORC: cap = ICN::CMBTCAPS; break;
    case Race::WRLK: cap = ICN::CMBTCAPW; break;
    case Race::WZRD: cap = ICN::CMBTCAPZ; break;
    default: return;
    }
    display.Blit(AGG::GetICN(cap, animframe, reflect), dst_pt.x + (reflect?550:0), dst_pt.y + 75);
}

Rect Army::DrawHero(const Heroes & hero, const Point & dst_pt, u16 animframe, bool reflect)
{
    Rect r;
    //Display & display = Display::Get();
    ICN::icn_t cap;
    switch(hero.GetRace()) {
    case Race::BARB: cap = ICN::CMBTHROB; break;
    case Race::KNGT: cap = ICN::CMBTHROK; break;
    case Race::NECR: cap = ICN::CMBTHRON; break;
    case Race::SORC: cap = ICN::CMBTHROS; break;
    case Race::WRLK: cap = ICN::CMBTHROW; break;
    case Race::WZRD: cap = ICN::CMBTHROZ; break;
    default: return r;
    }
    const Sprite & sp = AGG::GetICN(cap, animframe, reflect);
    r.x = dst_pt.x + (reflect?550:0);
    r.y = dst_pt.y + 75;
    r.w = sp.w();
    r.h = sp.h();
    display.Blit(sp, r.x, r.y);
    ICN::icn_t flag;
    switch(hero.GetColor()) {
    case Color::BLUE: flag = ICN::HEROFL00; break;
    case Color::GREEN: flag = ICN::HEROFL01; break;
    case Color::RED: flag = ICN::HEROFL02; break;
    case Color::YELLOW: flag = ICN::HEROFL03; break;
    case Color::ORANGE: flag = ICN::HEROFL04; break;
    case Color::PURPLE: flag = ICN::HEROFL05; break;
    case Color::GRAY: flag = ICN::HEROFL06; break;
    }
    display.Blit(AGG::GetICN(flag, animframe), dst_pt.x + (reflect?550:18), dst_pt.y + 80);
    return r;
}

void Army::DrawArmy(std::vector<Army::Troops> & army, const Point & dst_pt, bool reflect, int animframe)
{
    //Display & display = Display::Get();
    for(unsigned int i=0; i < army.size(); i++) {
	//for(std::vector<Army::Troops>::const_iterator it = army.begin(); it != army.end(); it++) {
	if(army[i].Monster() < Monster::UNKNOWN) {
	    //DrawMonster(*it, dst_pt, reflect, animframe);
	    army[i].Animate();
	    if(army[i].astate == Monster::AS_NONE && !Rand::Get(0, 30))
		army[i].Animate(Monster::AS_IDLE);
	    
	    Point tp = Bf2Scr(army[i].Position()) + dst_pt;
	    army[i].Blit(tp, reflect, animframe);
	    // draw count
	    int offset = Monster::GetStats(army[i].Monster()).wide ? CELLW : 0;
	    tp.x += reflect ? -10 - offset : 10 + offset;
	    tp.y -= 10;
	    // TODO color
	    u16 ind = 10;  // blue
	    // ind = 11;  // blue 2
	    // ind = 12;  // green
	    // ind = 13;  // yellow
	    // ind = 14;  // red
	    display.Blit(AGG::GetICN(ICN::TEXTBAR, ind), tp);
	    std::string str;
	    int count = army[i].Count();
	    if(count < 1000)
		String::AddInt(str, count);
	    else if(count < 1000000)
		String::AddInt(str, count / 1000), str += "K";
	    else 
		String::AddInt(str, count / 1000000), str += "M";
	    tp.x += 10 - Text::width(str, Font::SMALL) / 2;
	    tp.y -= 1;
	    Text(str, Font::SMALL, tp);
	}
    }
}

// void Army::DrawMonster(const Army::Troops & troop, const Point & dst_pt, bool reflect, int animframe)
// {
//     Display & display = Display::Get();
//     const Sprite & sp = AGG::GetICN(Monster::GetStats(troop.Monster()).file_icn, animframe<0?troop.aframe:animframe, reflect);
//     Point tp = Bf2Scr(troop.Position()) + dst_pt;
//     //tp.y -= sp.h();
//     //Point p(dst_pt.x + BFX + tp.x*CELLW - (tp.y % 2 ? CELLW/2 : 0), dst_pt.y + BFY + tp.y*CELLH - sp.h());
//     display.Blit(sp, tp.x + sp.x(), tp.y + sp.y());
//     // draw count
//     tp.x += reflect ? -10 : sp.w() - 10;
//     tp.y += /*sp.h()*/ - 10;
//     // TODO color
//     u16 ind = 10;  // blue
//     // ind = 11;  // blue 2
//     // ind = 12;  // green
//     // ind = 13;  // yellow
//     // ind = 14;  // red
//     display.Blit(AGG::GetICN(ICN::TEXTBAR, ind), tp);
//     std::string str;
//     int count = troop.Count();
//     if(count < 1000)
// 	String::AddInt(str, count);
//     else if(count < 1000000)
// 	String::AddInt(str, count / 1000), str += "K";
//     else 
// 	String::AddInt(str, count / 1000000), str += "M";
//     tp.x += 10 - Text::width(str, Font::SMALL) / 2;
//     tp.y -= 1;
//     Text(str, Font::SMALL, tp);
// }

void Army::InitArmyPosition(std::vector<Army::Troops> & army, bool compact, bool reflect)
{
    int x = reflect ? 10 : 0;
    int y = compact ? 2 : 0;
    for(unsigned int i=0; i < army.size(); i++) {
    //for(std::vector<Army::Troops>::iterator it = army.begin(); it != army.end(); it++) {
	army[i].SetPosition(Point(x, y));
	army[i].aframe = 1;
	army[i].astate = Monster::AS_NONE;
	y += compact ? 1 : 2;
    }
}

// translate coordinate to batle field
inline Point Army::Scr2Bf(const Point & pt)
{
    return Point((pt.x - BFX + (pt.y % 2 ? CELLW/2 : 0))/CELLW , (pt.y - BFY)/CELLH);
}

// translate to screen (offset to frame border)
inline Point Army::Bf2Scr(const Point & pt)
{
    return Point(BFX + pt.x*CELLW - (pt.y % 2 ? CELLW/2 : 0), BFY + pt.y*CELLH);
}

// void Army::AnimateMonster(Army::Troops & troop, Monster::animstate_t as)
// {
//     u8 start, count;
//     if(as != Monster::AS_NONE) {
// 	troop.astate = as;
// 	Monster::GetAnimFrames(troop.Monster(), as & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : as, start, count);
// 	troop.aframe = start;
//     } else {
// 	Monster::GetAnimFrames(troop.Monster(), troop.astate & Monster::AS_ATTPREP ? Monster::AS_ATTPREP : troop.astate, start, count);
// 	troop.aframe++;
// 	if(troop.aframe >= start+count) {
// 	    if(troop.astate & Monster::AS_ATTPREP) 
// 		troop.astate = (Monster::animstate_t)(troop.astate & ~Monster::AS_ATTPREP);
// 	    else
// 		troop.astate = Rand::Get(0, 30) ? Monster::AS_NONE : Monster::AS_IDLE;
// 	    Monster::GetAnimFrames(troop.Monster(), troop.astate, start, count);
// 	    troop.aframe = start;
// 	}
//     }
// }

void Army::SettingsDialog()
{ 
    //Display & display = Display::Get();
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::CSPANBKE : ICN::CSPANBKG;
    const ICN::icn_t butt = H2Config::EvilInterface() ? ICN::CSPANBTE : ICN::CSPANBTN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    
    // 0-2 speed
    // 3-5 monster info
    // 6,7 auto spell
    // 8,9 grid
    // 10,11 shadow movement
    // 12,13 shadow cursor
    Button buttonOK(pos_rt.x + 113, pos_rt.y + 252, butt, 0, 1);
    Button optGrid(pos_rt.x + 36, pos_rt.y + 157, ICN::CSPANEL, 8, 9);
    Button optSM(pos_rt.x + 128, pos_rt.y + 157, ICN::CSPANEL, 10, 11);
    Button optSC(pos_rt.x + 220, pos_rt.y + 157, ICN::CSPANEL, 12, 13);
    buttonOK.Draw();
    optGrid.Draw();
    optSM.Draw();
    optSC.Draw();
    if(O_GRID) optGrid.PressDraw();
    if(O_SHADM) optSM.PressDraw();
    if(O_SHADC) optSC.PressDraw();

    display.Flip();
    cursor.Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
    // dialog menu loop
    while(le.HandleEvents()) {
	// exit
	if(le.KeyPress(KEY_ESCAPE)) {
	    return;
	}	
	do_button(buttonOK, return, {});
	do_rbutton(optGrid, O_GRID = !O_GRID, {});
	do_rbutton(optSM, O_SHADM = !O_SHADM, {});
	do_rbutton(optSC, O_SHADC = !O_SHADC, {});
    }
}

void Army::HeroStatus()
{
    //Display & display = Display::Get();
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::VGENBKGE : ICN::VGENBKG;
    const ICN::icn_t butt = ICN::VIEWGEN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    Cursor & cursor = Cursor::Get();
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    
    Button buttonOK(pos_rt.x + 113, pos_rt.y + 252, butt, 0, 1);
    buttonOK.Draw();

    display.Flip();
    cursor.Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
    // dialog menu loop
    while(le.HandleEvents()) {
	// exit
	if(le.KeyPress(KEY_ESCAPE)) {
	    return;
	}	
	do_button(buttonOK, return, {});
    }
}

void Army::Temp(int dicn, int dindex, Point pt)
{
    static int icn=0, index=0;
    if(dicn) {
	icn += dicn;
	index = 0;
    }
    index += dindex;
    if(icn < 0) icn = 0;
    if(icn >= ICN::UNKNOWN) icn = ICN::UNKNOWN-1;
    if(index < 0) index = 0;
    std::string str = ICN::GetString((ICN::icn_t)icn);
    str += " ";
    String::AddInt(str, index);
    str += "-";
    int dh = 0, x = 0, y = 0;
    while(1) {
	const Sprite &sp = AGG::GetICN((ICN::icn_t)icn, index);
	if(sp.w() == 0 || sp.h() == 0) {
	    index --;
	    break;
	}
	if(dh < sp.h()) dh = sp.h();
	if(x + sp.w() > 640) {
	    x = 0;
	    y += dh;
	    if(y + sp.h() > 480) {
		index --;
		break;
	    }
	    dh = sp.h();
	}
	display.Blit(sp, pt.x + x, pt.y + y);
	x += sp.w();
	index ++;
    }
    String::AddInt(str, index);
    Text(str, Font::SMALL, pt);
}
