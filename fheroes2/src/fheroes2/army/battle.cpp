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


#include <math.h>
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
#include "portrait.h"
#include "spell.h"
#include "audio.h"

#define CELLW 44
#define CELLH 42
#define BFX 86
#define BFY 108
#define BFW 11
#define BFH 9
#define MORALEBOUND 10

#define display Display::Get()
#define le LocalEvent::GetLocalEvent()
#define cursor Cursor::Get()
#define world World::GetWorld()

#define do_button(b, lf, rt) \
if(le.MousePressLeft(b)) b.PressDraw();	\
else b.ReleaseDraw(); \
if(le.MouseClickLeft(b) && !b.isDisable()) lf; \
if(le.MousePressRight(b)&& !b.isDisable()) rt;

#define do_rbutton(b, lf, rt) \
if(le.MouseClickLeft(b) && !b.isDisable()) { b.isPressed() ? b.ReleaseDraw() : b.PressDraw(); lf; } \
if(le.MousePressRight(b) && !b.isDisable()) rt;

namespace Army {
    bool O_GRID = false, O_SHADM = false, O_SHADC = false, O_AUTO = false;
    long EXP1, EXP2;
    Point dst_pt;
    std::vector<Point> movePoints;
    Dialog::StatusBar *statusBar1;
    Dialog::StatusBar *statusBar2;
    typedef struct {
	ICN::icn_t icn;
	Point scrpoint, bfpoint;
    } CObj;
    std::vector<Point> blockedCells;
    std::vector<CObj> cobjects;
    Army::army_t bodies1, bodies2;

    battle_t BattleInt(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile);
    battle_t HumanTurn(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack);
    battle_t CompTurn(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack);
    bool AnimateCycle(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, const Point &move, const Point &attack);
    bool MagicSelectTarget(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, bool reflect, Spell::spell_t spell);
    void MagicPreCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::Troops*> &aff1, std::vector<Army::Troops*> &aff2, Army::army_t &army1, Army::army_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile);
    bool MagicCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::Troops*> &aff1, std::vector<Army::Troops*> &aff2, Army::army_t &army1, Army::army_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile);
    void InitBackground(const Maps::Tiles & tile);
    void DrawBackground(const Maps::Tiles & tile);
    void DrawObject(const Point &pt);
    void DrawCaptain(const Race::race_t race, u16 animframe, bool reflect=false);
    Rect DrawHero(const Heroes & hero, u16 animframe, bool reflect=false, int fframe=0);
    void DrawArmy(Army::army_t & army1, Army::army_t & army2, int animframe=-1);
    void DrawTroop(Army::Troops & troop, bool reflect=false, int animframe=-1);
    void InitArmyPosition(Army::army_t & army, bool compact, bool reflect=false);
    inline Point Scr2Bf(const Point & pt); // translate coordinate to battle field
    inline Point Bf2Scr(const Point & pt); // translate to screen (offset to frame border)
    inline bool BfValid(const Point & pt); // check battle field point
    int FindTroop(const Army::army_t &army, const Point &p, bool reflect=false, bool usewide=true);
    int FindTroop(const std::vector<Army::Troops*> &army, const Point &p, bool reflect=false, bool usewide=true);
    bool CellFree(const Point &p, const Army::army_t &army1, const Army::army_t &army2, int skip=99);
    void SettingsDialog();
    battle_t HeroStatus(Heroes &hero, Dialog::StatusBar &statusBar, Spell::spell_t &spell, bool quickshow, bool cansurrender=false, bool locked=false);
    void DrawShadow(const Point &pt);
    void DrawCell(const Point &pt);
    void DrawMark(const Point &point, int animframe=0);
    void PrepMovePoints(int troopN, const Army::army_t &army1, const Army::army_t &army2, bool reflect=false);
    void PrepMovePointsInt(const Point &p, int move, const Army::army_t &army1, const Army::army_t &army2, bool wide, int skip, bool reflect);
    std::vector<Point> *FindPath(const Point& start, const Point &end, int moves, const Army::army_t &army1, const Army::army_t &army2, int skip, bool wide, bool reflect);
    void AttackStatus(const std::string &str);
    int CanAttack(const Army::Troops &myTroop, const std::vector<Point> &moves, const Army::Troops &enemyTroop, const Point &d, bool reflect=false);
    bool GoodMorale(Heroes *hero, const Army::Troops &troop);
    bool BadMorale(Heroes *hero, const Army::Troops &troop);
    int CheckLuck(Heroes *hero, const Army::Troops &troop);

    void Temp(int, int, Point);
}

Army::battle_t Army::Battle(Heroes& hero1, Heroes& hero2, const Maps::Tiles & tile)
{
    return BattleInt(&hero1, &hero2, const_cast<std::vector<Army::Troops>&>(hero1.GetArmy()), const_cast<std::vector<Army::Troops>&>(hero2.GetArmy()), tile);
}

Army::battle_t Army::Battle(Heroes& hero, std::vector<Army::Troops>& army, const Maps::Tiles & tile)
{
    return BattleInt(&hero, 0, const_cast<std::vector<Army::Troops>&>(hero.GetArmy()), army, tile);
}

Army::battle_t Army::Battle(Heroes& hero, Castle& castle, const Maps::Tiles &tile)
{
    return LOSE; // TODO
}

Army::battle_t Army::BattleInt(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile)
{
    Mixer::Reset();
    //FIXME: The music currently plays underneath the sound effect
    int track = ((int)MUS::BATTLE1 + Rand::Get(0, 3));
    AGG::PlayMusic((MUS::mus_t)track);
    
    AGG::PlaySound(M82::PREBATTL);
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    Dialog::FrameBorder frameborder;

    dst_pt = Point(frameborder.GetArea().x, frameborder.GetArea().y);

    display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    InitBackground(tile);
    DrawBackground(tile);
    if(hero1) DrawHero(*hero1, 1);
    if(hero2) DrawHero(*hero2, 1, true);
    InitArmyPosition(army1, false);
    InitArmyPosition(army2, false, true);
    DrawArmy(army1, army2, 1);

    display.Blit(AGG::GetICN(ICN::TEXTBAR, 0), dst_pt.x + 640-48, dst_pt.y + 480-36);
    display.Blit(AGG::GetICN(ICN::TEXTBAR, 4), dst_pt.x, dst_pt.y + 480-36);
    display.Blit(AGG::GetICN(ICN::TEXTBAR, 6), dst_pt.x, dst_pt.y + 480-18);

    statusBar1 = new Dialog::StatusBar(Point(dst_pt.x + 50, dst_pt.y + 480-36), AGG::GetICN(ICN::TEXTBAR, 8), Font::BIG);
    statusBar1->Clear(" ");
    statusBar2 = new Dialog::StatusBar(Point(dst_pt.x + 50, dst_pt.y + 480-16), AGG::GetICN(ICN::TEXTBAR, 9), Font::BIG);
    statusBar2->Clear(" ");

    display.Flip();
    Point move, attack;
    EXP1 = EXP2 = 0;  // experience = damage
    bool goodmorale;

    while(1) {
	if(hero1) hero1->spellCasted = false;
	if(hero2) hero2->spellCasted = false;
	for(u16 i=0; i<army1.size(); i++) army1[i].ProceedMagic();
	for(u16 i=0; i<army2.size(); i++) army2[i].ProceedMagic();
	Speed::speed_t cursp = Speed::INSTANT;
	while(1) {
	    //Dialog::Message("speed", Speed::String(cursp), Font::BIG, Dialog::OK);
	    for(unsigned int i=0; i < army1.size(); i++) {
		goodmorale = false;
		if(Monster::GetStats(army1[i].Monster()).speed == cursp && army1[i].Count() > 0 && !BadMorale(hero1, army1[i])) do {
		    battle_t s;
		    if(hero1 && world.GetKingdom(hero1->GetColor()).Control() == Game::LOCAL && !O_AUTO) {
			s = HumanTurn(hero1, hero2, army1, army2, tile, i, move, attack);
			if( s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLELOSE);
                            return s;
                        }
		    } else {
			s = CompTurn(hero1, hero2, army1, army2, tile, i, move, attack);
			if( s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLELOSE);
                            return s;
                        }
		    }
		    if(AnimateCycle(hero1, hero2, army1, army2, tile, i, move, attack)) {
			if(!AnimateCycle(hero1, hero2, army1, army2, tile, -FindTroop(army2, attack, true)-1, attack, move)) {
			    if(army1[i].Count() <= 0) {
				Army::Troops body = army1[i];
				army1.erase(army1.begin()+i);
				bodies1.push_back(body);
			    }
			}
		    } else {
			int t = FindTroop(army2, attack, true);
			if(t >= 0 && army2[t].Count() <= 0) {
			    Army::Troops body = army2[t];
			    army2.erase(army2.begin()+t);
			    bodies2.push_back(body);
			}
		    }
		} while (!goodmorale && army2.size() && (goodmorale = GoodMorale(hero1, army1[i]), goodmorale));
	    }
	    for(unsigned int i=0; i < army2.size(); i++) {
		goodmorale = false;
		if(Monster::GetStats(army2[i].Monster()).speed == cursp && army2[i].Count() > 0 && !BadMorale(hero2, army2[i])) do {
		    battle_t s;
		    if(hero2 && world.GetKingdom(hero2->GetColor()).Control() == Game::LOCAL && !O_AUTO) {
			s = HumanTurn(hero1, hero2, army1, army2, tile, -i-1, move, attack);
			if(s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLEWIN, false);
                            return WIN;
                        }
		    } else {
			s = CompTurn(hero1, hero2, army1, army2, tile, -i-1, move, attack);
			if(s == RETREAT || s == SURRENDER)
                        {
                            AGG::PlayMusic(MUS::BATTLEWIN, false);
                            return WIN;
                        }
		    }
		    if(AnimateCycle(hero1, hero2, army1, army2, tile, -i-1, move, attack)) {
			if(!AnimateCycle(hero1, hero2, army1, army2, tile, FindTroop(army1, attack, false), attack, move)) {
			    if(army2[i].Count() <= 0) {
				Army::Troops body = army2[i];
				army2.erase(army2.begin()+i);
				bodies2.push_back(body);
			    }
			}
		    } else {
			int t = FindTroop(army1, attack, true);
			if(t >= 0 && army1[t].Count() <= 0) {
			    Army::Troops body = army1[t];
			    army1.erase(army1.begin()+t);
			    bodies1.push_back(body);
			}
		    }
		} while (!goodmorale && army1.size() && (goodmorale = GoodMorale(hero2, army2[i]), goodmorale));
	    }
	    int c1 = 0, c2 = 0;
	    for(unsigned int i=0; i < army1.size(); i++) c1 += army1[i].Count();
	    for(unsigned int i=0; i < army2.size(); i++) c2 += army2[i].Count();
	    if(c1 <= 0)
            {
                AGG::PlayMusic(MUS::BATTLELOSE, false);
                return LOSE;
            }
	    if(c2 <= 0)
            {
                AGG::PlayMusic(MUS::BATTLEWIN, false);
                return WIN;
            }
	    if(cursp == Speed::CRAWLING) break;
	    else --cursp;
	}
    }
    
    AGG::PlayMusic(MUS::BATTLEWIN, false);
    
    return WIN;
    // TODO remove summoned creatures
}

Army::battle_t Army::HumanTurn(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack)
{
    std::vector<Army::Troops> &myArmy = troopN >= 0 ? army1 : army2;
    std::vector<Army::Troops> &enemyArmy = troopN >= 0 ? army2 : army1;
    Army::Troops &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    PrepMovePoints(troopN, army1, army2, troopN < 0);
    move.x = move.y = attack.x = attack.y = -1;
    bool close = false;
    for(u16 j=0; j<enemyArmy.size(); j++) {
	Point tmpoint(0,0);
	std::vector<Point> tmpoints;
	tmpoints.push_back(enemyArmy[j].Position());
	if(CanAttack(enemyArmy[j], tmpoints, myTroop, tmpoint, troopN>=0) >= 0) {
	    close = true;
	    break;
	}
    }
    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    AttackStatus("");

    //Dialog::FrameBorder frameborder;

    //const Point dst_pt(frameborder.GetArea().x, frameborder.GetArea().y);

    //display.FillRect(0, 0, 0, Rect(dst_pt, 640, 480));

    DrawBackground(tile);
    Rect rectHero1, rectHero2;
    if(hero1) rectHero1 = DrawHero(*hero1, 1);
    if(hero2) rectHero2 = DrawHero(*hero2, 1, true);
    DrawArmy(army1, army2);

    Button buttonSkip(dst_pt.x + 640-48, dst_pt.y + 480-36, ICN::TEXTBAR, 0, 1);
    Button buttonAuto(dst_pt.x, dst_pt.y + 480-36, ICN::TEXTBAR, 4, 5);
    Button buttonSettings(dst_pt.x, dst_pt.y + 480-18, ICN::TEXTBAR, 6, 7);

    /*Dialog::StatusBar statusBar1(Point(dst_pt.x + 50, dst_pt.y + 480-36), AGG::GetICN(ICN::TEXTBAR, 8), Font::BIG);
    statusBar1.Clear(" ");
    Dialog::StatusBar statusBar2(Point(dst_pt.x + 50, dst_pt.y + 480-16), AGG::GetICN(ICN::TEXTBAR, 9), Font::BIG);
    statusBar2.Clear(" ");*/

    buttonSkip.Draw();
    buttonAuto.Draw();
    buttonSettings.Draw();

    display.Flip();
    cursor.Show();

    int i = 0;
    u16 animat = 0;
    Point cur_pt(-1,-1);
    Spell::spell_t spell = Spell::NONE;
    // dialog menu loop
    while(le.HandleEvents()) {
	if(le.KeyPress(KEY_RETURN)) {
	    DrawBackground(tile);
	    army1[0].SetCount(i);
	    if(hero1) DrawHero(*hero1, i);
	    if(hero2) DrawHero(*hero2, i, true);
	    DrawArmy(army1, army2, i++);
	    cursor.Show();
	    display.Flip();
	}
// 	if(le.KeyPress(SDLK_SPACE)) return WIN;
	if(le.KeyPress(SDLK_SPACE)) {
	    DrawBackground(tile);
	    Temp(0, 1, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_PLUS)) {
	    DrawBackground(tile);
	    Temp(1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(le.KeyPress(SDLK_KP_MINUS)) {
	    DrawBackground(tile);
	    Temp(-1, 0, dst_pt);
	    i = 1;
	    display.Flip();
	}
	if(Game::ShouldAnimateInfrequent(++animat, 3) && !i) {
	    cursor.Hide();
	    DrawBackground(tile);
	    if(O_SHADM) //DrawMoveShadow(myTroop, army1, army2, troopN < 0);
		for(u16 i=0; i<movePoints.size(); i++)
		    DrawShadow(movePoints[i]);
	    if(hero1) DrawHero(*hero1, 1, false, 1);
	    if(hero2) DrawHero(*hero2, 1, true);
	    if(O_SHADC && BfValid(cur_pt)) DrawShadow(cur_pt);
	    DrawMark(myTroop.Position(), 1);
	    if(myMonster.wide) {
		Point p = myTroop.Position();
		p.x += troopN >= 0 ? 1 : -1;
		DrawMark(p);
	    }
	    //display.Blit(AGG::GetICN(ICN::TEXTBAR, 8), Point(dst_pt.x + 50, dst_pt.y + 480-36));
	    statusBar1->Redraw();
	    DrawArmy(army1, army2);
	    cursor.Show();
	    display.Flip();
	}
	do_button(buttonSkip, return WIN, Dialog::Message("Skip", "Skip the current creature. The current creature loses its turn and does not get to go again until the next round.", Font::BIG));
	do_button(buttonAuto, O_AUTO=true, Dialog::Message("Auto Combat", "Allows the computer to fight out the battle for you.", Font::BIG));
	do_button(buttonSettings, SettingsDialog(), Dialog::Message("System Options", "Allows you to customize the combat screen.", Font::BIG));
	if(le.MouseClickLeft(rectHero1)) {
	    battle_t s = HeroStatus(*hero1, *statusBar2, spell, false, hero2, troopN < 0);
	    if(s == RETREAT) {
		if(Dialog::Message("", "Are you sure you want to retreat?", Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
		    return s;
	    }
	    if(s == SURRENDER) return s;
	    if(spell != Spell::NONE && MagicSelectTarget(hero1, hero2, army1, army2, tile, false, spell))
		hero1->spellCasted = 0;//true;
	}
	if(le.MouseClickLeft(rectHero2)) {
	    battle_t s = HeroStatus(*hero2, *statusBar2, spell, false, hero1, troopN >= 0);
	    if(s == RETREAT) {
		if(Dialog::Message("", "Are you sure you want to retreat?", Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
		    return s;
	    }
	    if(s == SURRENDER) return s;
	    if(spell != Spell::NONE && MagicSelectTarget(hero1, hero2, army1, army2, tile, true, spell))
		hero2->spellCasted = 0;//true;
	}
	if(le.MousePressRight(rectHero1)) HeroStatus(*hero1, *statusBar2, spell, true, false, false);
	if(le.MousePressRight(rectHero2)) HeroStatus(*hero2, *statusBar2, spell, true, false, false);
        if(le.MouseCursor(buttonSkip)) {
	    statusBar2->ShowMessage("Skip this unit");
	    cursor.SetThemes(cursor.WAR_POINTER);
        }else if(le.MouseCursor(buttonAuto)) {
	    statusBar2->ShowMessage("Auto combat");
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(buttonSettings)) {
	    statusBar2->ShowMessage("Customize system options");
	    cursor.SetThemes(cursor.WAR_POINTER);
        } else if(le.MouseCursor(troopN >= 0 ? rectHero1 : rectHero2)) {
	    statusBar2->ShowMessage("Hero's Options");
	    cursor.SetThemes(cursor.WAR_HELMET);
	} else if(le.MouseCursor(troopN >= 0 ? rectHero2 : rectHero1)) {
	    statusBar2->ShowMessage("View Oppositing Hero");
	    cursor.SetThemes(cursor.WAR_INFO);
	} else {
	    int t = -1;
	    bool click;
	    cur_pt = Scr2Bf(le.MouseCursor()-dst_pt);
	    if(!le.MouseLeft() && cur_pt == Scr2Bf(le.MousePressLeft()-dst_pt) && cur_pt == Scr2Bf(le.MouseReleaseLeft()-dst_pt)) {
		click = true;
		le.MouseClickLeft(Rect(0, 0, display.w(), display.h()));
	    } else click = false;
	    if(BfValid(cur_pt)) {
		// cursor on the battle field
		if(t = FindTroop(myArmy, cur_pt, troopN < 0), t >= 0 && myArmy[t].Count() > 0) {
		    // troop from my army
		    statusBar2->ShowMessage("View "+Monster::String(myArmy[t].Monster())+" info.");
		    cursor.SetThemes(cursor.WAR_INFO);
		    if(click) {
			cursor.SetThemes(cursor.POINTER); 
			Dialog::ArmyInfo(myArmy[t], false, false, false, true);
		    }
		    if(le.MouseRight()) {
			Dialog::ArmyInfo(myArmy[t], false, true, false, true);
		    }
		} else if(t = FindTroop(enemyArmy, cur_pt, troopN >= 0), t >= 0 && enemyArmy[t].Count() > 0) {
		    // enemy troop
		    int mp;
		    if(myTroop.shots > 0 && !close) {
			std::string str = "Shoot "+Monster::String(enemyArmy[t].Monster())+" (";
			String::AddInt(str, myTroop.shots);
			str += " shot(s) left)";
			statusBar2->ShowMessage(str);
			cursor.SetThemes(cursor.WAR_ARROW);
			if(click) {
			    attack = cur_pt;
			    return WIN;
			} else if(le.MouseRight()) {
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
			}
		    } else if(mp = CanAttack(myTroop, movePoints, enemyArmy[t], le.MouseCursor()-(Bf2Scr(cur_pt)+dst_pt), troopN<0), mp >= 0) {
			std::string str = "Attack "+Monster::String(enemyArmy[t].Monster());
			statusBar2->ShowMessage(str);
			Point p1 = Bf2Scr(movePoints[mp]), p2 = Bf2Scr(cur_pt);
			if(p1.x > p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPRIGHT);
			else if(p1.x < p2.x && p1.y > p2.y) cursor.SetThemes(cursor.SWORD_TOPLEFT);
			else if(p1.x > p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMRIGHT);
			else if(p1.x < p2.x && p1.y < p2.y) cursor.SetThemes(cursor.SWORD_BOTTOMLEFT);
			else if(p1.x > p2.x) cursor.SetThemes(cursor.SWORD_RIGHT);
			else if(p1.x < p2.x) cursor.SetThemes(cursor.SWORD_LEFT);
			//cursor.SetThemes(cursor.SWORD_TOPLEFT); // TODO
			if(click) {
			    move = movePoints[mp];
			    attack = cur_pt;
			    return WIN;
			} else if(le.MouseRight())
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
		    } else {
			// attack
			statusBar2->ShowMessage("View "+Monster::String(enemyArmy[t].Monster())+" info.");
			cursor.SetThemes(cursor.WAR_INFO);
			if(click) {
			    cursor.SetThemes(cursor.POINTER);
			    Dialog::ArmyInfo(enemyArmy[t], false, false, false, true);
			} else if(le.MouseRight()) {
			    Dialog::ArmyInfo(enemyArmy[t], false, true, false, true);
			}
		    }
		} else {
		    // empty cell
		    bool canmove = false;
		    for(u16 i=0; i<movePoints.size(); i++) if(movePoints[i] == cur_pt) {
			canmove = true; 
			break;
		    }
		    if(canmove) {
			std::string str;
			if(myMonster.fly) cursor.SetThemes(cursor.WAR_FLIGHT), str = "Fly ";
			else cursor.SetThemes(cursor.WAR_MOVE), str = "Move ";
			str += Monster::String(myTroop.Monster())+" here.";
			statusBar2->ShowMessage(str);
			if(click) {
			    move = cur_pt;
			    return WIN;
			}
		    } else {
			statusBar2->ShowMessage(" ");
			cursor.SetThemes(cursor.WAR_NONE);
		    }
		}
	    } else {
		statusBar2->ShowMessage(" ");
		cursor.SetThemes(cursor.WAR_NONE);
	    }
	}
    }
    return WIN;
}

Army::battle_t Army::CompTurn(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, Point &move, Point &attack)
{
    move.x = move.y = attack.x = attack.y = -1;
    return WIN;
}

bool Army::AnimateCycle(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, int troopN, const Point &move, const Point &attack)
{
    bool counterstrike = false;
    cursor.Hide();
    Army::Troops &myTroop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    u16 animat = 0;
    // MOVE
    if(BfValid(move) && move != myTroop.Position()) {
	Point start = Bf2Scr(myTroop.Position()) + dst_pt;
	Point end = Bf2Scr(move) + dst_pt;
	Point tp = start;
	Point step;
	bool reflect = end.x > start.x ? false : true;
	u8 st, len, prep=0, post=0;
	Monster::GetAnimFrames(myTroop.Monster(), Monster::AS_WALK, st, len);
	int frame = 0, curstep = 0, part = 0;
	std::vector<Point> *path=0;
	if(myMonster.fly) {
	    if(len >= 12) prep = post = 4;
	    else if(len >= 8) prep = post = 2;
	    else prep = post = 1;
	    step = end-start;
	    step.x /= 8;
	    step.y /= 8;
	} else {
	    if(path = FindPath(myTroop.Position(), move, Speed::Move(myMonster.speed), army1, army2, troopN, myMonster.wide, reflect),!path) {
		Dialog::Message("Error", "Path not found!", Font::BIG, Dialog::OK);
		return counterstrike;
	    }
	    step.x = ((Bf2Scr(path->back())+dst_pt).x - tp.x) / len;
	    step.y = ((Bf2Scr(path->back())+dst_pt).y - tp.y) / len;
	    reflect = step.x < 0;
	    if(step.y) step.x = -step.x;
	    else step.x = 0;
	}
	myTroop.Animate(Monster::AS_WALK);
	while(le.HandleEvents()) {
	    if(Game::ShouldAnimateInfrequent(++animat, 4)) {
		if(myMonster.fly) {
		    if(frame >= prep && !part) {
			AGG::PlaySound(myMonster.m82_move);
			part = 1;
		    }
		    if(part == 1) {
			tp += step;
			curstep ++;
			if(myTroop.aframe >= st+len-post-1) {
			    AGG::PlaySound(myMonster.m82_move);
			    myTroop.aframe = st+prep;
			}
			if(curstep == 8) {
			    part = 3;
			    tp = end;
			    myTroop.aframe = st+len-post;
			}
		    }
		    if(part == 3 && myTroop.astate != Monster::AS_WALK) break;
		} else {
		    tp += step;
		    if(myTroop.astate != Monster::AS_WALK && path->size()) {
			AGG::PlaySound(myMonster.m82_move);
			myTroop.SetPosition(path->back());
			tp = Bf2Scr(myTroop.Position()) + dst_pt;
			path->pop_back();
			if(!path->size()) break;
			step.x = ((Bf2Scr(path->back())+dst_pt).x - tp.x) / len;
			step.y = ((Bf2Scr(path->back())+dst_pt).y - tp.y) / len;
			reflect = step.x < 0;
			if(step.y) step.x = -step.x;
			else step.x = 0;
			myTroop.Animate(Monster::AS_WALK);
		    }
		}
		DrawBackground(tile);
		if(hero1) DrawHero(*hero1, 1, false, 1);
		if(hero2) DrawHero(*hero2, 1, true);
		Point p;
		int t=-1;
		for(p.y = 0; p.y < BFH; p.y++)
		    for(p.x = 0; p.x < BFW; p.x++) {
			DrawObject(p);
			if(t = FindTroop(army1, p, false, false), t >= 0) {
			    if(&army1[t] == &myTroop) {
				    myTroop.Blit(tp, reflect);
				    myTroop.Animate();
			    } else {
				DrawTroop(army1[t], false);
				army1[t].Animate();
			    }
			}
			if(t = FindTroop(army2, p,  true, false), t >= 0) {
			    if(&army2[t] == &myTroop) {
				myTroop.Blit(tp, reflect);
				myTroop.Animate();
			    } else {
				DrawTroop(army2[t],  true);
				army2[t].Animate();
			    }
			}
		    }
		display.Flip();
		frame ++;
	    }
	}
	myTroop.SetPosition(move);
	if(path) delete path;
    }
    // ATTACK
    if(BfValid(attack)) {
	int t;
	Army::Troops &target = (t = FindTroop(army1, attack, false), t >= 0 ? army1[t] : army2[FindTroop(army2, attack, troopN >= 0)]);
	int sk_a = myMonster.attack + (myTroop.MasterSkill() ? myTroop.MasterSkill()->GetAttack() : 0);
	int sk_d = Monster::GetStats(target.Monster()).defence + (target.MasterSkill() ? target.MasterSkill()->GetDefense() : 0);
	long damage = 0;
	switch(CheckLuck(troopN >= 0 ? hero1 : hero2, myTroop)) {
	case -1:
	    damage = myMonster.damageMin*myTroop.Count();
	    break;
	case 0:
	    damage = Rand::Get(myMonster.damageMin*myTroop.Count(), myMonster.damageMax*myTroop.Count());
	    break;
	case 1:
	    damage = myMonster.damageMax*myTroop.Count();
	    break;
	}
	// TODO bless and curse
	//damage *= sk_a;
	//damage /= sk_d;
	float d = sk_a-sk_d;
	d *= d>0 ? 0.1f : 0.05f;
	damage = (long)(((float)damage) * 1+d); 
	long hp = target.hp + (target.Count()-1)*Monster::GetStats(target.Monster()).hp;	
	int state = 0, missframe = 0, missindex = 0;
	Point miss_start(myTroop.Position().x + (myMonster.wide ? (troopN<0 ? -1 : 1): 0), myTroop.Position().y);
	miss_start = Bf2Scr(miss_start) + dst_pt;
	Point miss_step(Bf2Scr(target.Position()) + dst_pt);
	miss_step -= miss_start;
#define MISSFRAMES 8
	miss_step.x /= MISSFRAMES;
	miss_step.y /= MISSFRAMES;
	miss_start.y -= CELLH;
	{
	    Point tmp(0,0);
	    std::vector<Point> tmpmoves;
	    tmpmoves.push_back(target.Position());
	    if(CanAttack(target, tmpmoves, myTroop, tmp, troopN>=0) >= 0) 
		counterstrike = true;
	}
	bool ranged = myMonster.miss_icn != ICN::UNKNOWN && myTroop.shots > 0 && !counterstrike;
	Monster::animstate_t targetstate = hp > damage ? Monster::AS_PAIN : Monster::AS_DIE;
	Monster::animstate_t mytroopstate;
	if(target.Position().y > myTroop.Position().y)
	    mytroopstate = Monster::AS_ATT3P;
	else if(target.Position().y < myTroop.Position().y)
	    mytroopstate = Monster::AS_ATT1P;
	else
	    mytroopstate = Monster::AS_ATT2P;
	if(!ranged) {
	    if(myMonster.miss_icn != ICN::UNKNOWN && myTroop.Monster() != Monster::MAGE &&
	       myTroop.Monster() != Monster::ARCHMAGE && myTroop.Monster() != Monster::TITAN)
		damage /= 2;
	    AGG::PlaySound(myMonster.m82_attk);
	    u8 st, len, len2, len3;
	    Monster::GetAnimFrames(target.Monster(), targetstate, st, len);
	    Monster::GetAnimFrames(myTroop.Monster(), (Monster::animstate_t)(mytroopstate & ~Monster::AS_ATTPREP), st, len2);
	    Monster::GetAnimFrames(myTroop.Monster(), Monster::AS_ATTPREP, st, len3);
	    missframe = len3 + len2 - len;
	    if(missframe < 0) missframe = 0;
	    if(!missframe) {
		target.Animate(targetstate);
		if(hp <= damage)
		    AGG::PlaySound(Monster::GetStats(target.Monster()).m82_kill);
		else
		    AGG::PlaySound(Monster::GetStats(target.Monster()).m82_wnce);
	    }
	    myTroop.aranged = false;
	} else {
	    AGG::PlaySound(myMonster.m82_shot);
	    myTroop.aranged = true;
	    int maxind = AGG::GetICNCount(myMonster.miss_icn);
	    if(maxind > 1) {
		double angle = M_PI_2 - atan2(-miss_step.y, miss_step.x);
		missindex = (int)(angle/M_PI * maxind);
	    } else missindex = 0;
	}
	myTroop.Animate(mytroopstate);
	while(le.HandleEvents()) {
	    if(Game::ShouldAnimateInfrequent(++animat, 4)) {
		if(!ranged) {
		    if(missframe) {
			missframe --;
			if(!missframe) {
			    target.Animate(targetstate);
			    if(hp <= damage)
				AGG::PlaySound(Monster::GetStats(target.Monster()).m82_kill);
			    else
				AGG::PlaySound(Monster::GetStats(target.Monster()).m82_wnce);
			}
		    }
		    if(myTroop.astate == Monster::AS_NONE) {
			if(hp > damage) {
			    if(target.astate == Monster::AS_NONE) break;
			} else {
			    u8 st, len;
			    Monster::GetAnimFrames(target.Monster(), Monster::AS_DIE, st, len);
			    if(target.aframe >= st+len-1) break;
			}
		    }
		} else {
		    if(myTroop.astate == Monster::AS_NONE && !state) 
			state ++;
		    if(missframe >= MISSFRAMES && state == 1) {
			target.Animate(hp > damage ? Monster::AS_PAIN : Monster::AS_DIE), state++;
			if(hp <= damage)
			    AGG::PlaySound(Monster::GetStats(target.Monster()).m82_kill);
			else
			    AGG::PlaySound(Monster::GetStats(target.Monster()).m82_wnce);
		    }
		    if(target.astate == Monster::AS_NONE && state == 2) break;
		    if(hp <= damage && state == 2) {
			u8 st, len;
			Monster::GetAnimFrames(target.Monster(), Monster::AS_DIE, st, len);
			if(target.aframe >= st+len-1) break;
		    }
		}
		DrawBackground(tile);
		if(hero1) DrawHero(*hero1, 1, false, 1);
		if(hero2) DrawHero(*hero2, 1, true);
		Point p;
		int t=-1;
		for(p.y = 0; p.y < BFH; p.y++)
		    for(p.x = 0; p.x < BFW; p.x++) {
			DrawObject(p);
			if(t = FindTroop(army1, p, false, false), t >= 0) {
			    DrawTroop(army1[t], false);
			    army1[t].Animate();
			}
			if(t = FindTroop(army2, p,  true, false), t >= 0) {
			    DrawTroop(army2[t],  true);
			    army2[t].Animate();
			}
		    }
		if(state == 1) {
		    display.Blit(AGG::GetICN(myMonster.miss_icn, abs(missindex), missindex < 0), miss_start);
		    miss_start += miss_step;
		    missframe ++;
		}
		display.Flip();
	    }
	}
	std::string status = "";
	status += Monster::String(myTroop.Monster());
	if(myTroop.Count() > 1) status += "s";
	status += " do ";
	String::AddInt(status, damage);
	status += " damage. ";
	int oldcount = target.Count();
	if(troopN >= 0) EXP1 += hp > damage ? damage : hp;
	else EXP2 += hp > damage ? damage : hp;
	if(hp > damage) {
	    target.SetCount((hp - damage)/Monster::GetStats(target.Monster()).hp + 1);
	    target.hp = (hp - damage) % Monster::GetStats(target.Monster()).hp;
	    if(!target.hp) {
		target.SetCount(target.Count() - 1);
		target.hp = Monster::GetStats(target.Monster()).hp;
	    }
	} else {
	    target.SetCount(0);
	    target.hp = 0;
	    counterstrike = false;
	}
	oldcount -= target.Count();
	if(oldcount > 0) {
	    String::AddInt(status, oldcount);
	    status += " "+Monster::String(target.Monster());
	    if(oldcount > 1) status += "s";
	    status += " perish.";
	}
	AttackStatus(status);
	if(ranged) myTroop.shots --;
	if(!ranged) {
	    std::vector<Army::Troops*> ta1, ta2;
	    (troopN >= 0 ? ta1 : ta2).push_back(&target);
	    MagicCycle(hero1, hero2, ta1, ta2, army1, army2, troopN<0, Spell::TroopAttack(myTroop.Monster()), tile);
	}
    }
    return counterstrike;
}

bool Army::MagicSelectTarget(Heroes *hero1, Heroes *hero2, Army::army_t &army1, Army::army_t &army2, const Maps::Tiles &tile, bool reflect, Spell::spell_t spell)
{
    std::vector<Army::Troops*> ta1, ta2;
    Army::army_t &myarmy = reflect ? army2 : army1;
    Army::army_t &enarmy = reflect ? army1 : army2;
    switch(Spell::Target(spell)) {
    case Spell::ONEFRIEND:
    case Spell::ONEENEMY:
    case Spell::FREECELL:
	break;
    case Spell::ALLFRIEND:
	for(u16 i=0; i<myarmy.size(); i++) {
	    if(Spell::AllowSpell(spell, myarmy[i]))
		(reflect?ta2:ta1).push_back(&myarmy[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::ALLENEMY:
	for(u16 i=0; i<enarmy.size(); i++) {
	    if(Spell::AllowSpell(spell, enarmy[i]))
		(reflect?ta1:ta2).push_back(&enarmy[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::ALLLIVE:
    case Spell::ALLDEAD:
    case Spell::ALL:
	for(u16 i=0; i<army1.size(); i++) {
	    if(Spell::AllowSpell(spell, army1[i]))
		ta1.push_back(&army1[i]);
	}
	for(u16 i=0; i<army2.size(); i++) {
	    if(Spell::AllowSpell(spell, army2[i]))
		ta1.push_back(&army2[i]);
	}
	return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
    case Spell::NOTARGET:
	return false;
    }
    // select target
    while(le.HandleEvents()) {
	Point cur_pt = Scr2Bf(le.MouseCursor()-dst_pt);
	int t;
	if(Spell::Target(spell) == Spell::ONEFRIEND) {
	    if(t = FindTroop(myarmy, cur_pt, reflect, true), t >= 0 && Spell::AllowSpell(spell, myarmy[t])) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		if(le.MouseClickLeft(Rect(0, 0, display.w(), display.h()))) {
		    (reflect?ta2:ta1).push_back(&myarmy[t]);
		    break;
		}
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else if(Spell::Target(spell) == Spell::ONEENEMY) {
	    if(t = FindTroop(enarmy, cur_pt, reflect, true), t >= 0) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		if(le.MouseClickLeft(Rect(0, 0, display.w(), display.h()))) {
		    (reflect?ta1:ta2).push_back(&enarmy[t]);
		    break;
		}
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else if(Spell::Target(spell) == Spell::FREECELL) {
	    if(CellFree(cur_pt, army1, army2)) {
		cursor.SetThemes((Cursor::themes_t)(cursor.SPELLNONE + Spell::GetIndexSprite(spell)));
		// TODO COLDRING & SUMMON
	    } else 
		cursor.SetThemes(cursor.SPELLNONE);
	} else {
	    cursor.SetThemes(cursor.SPELLNONE);
	}
	// exit
	if(le.KeyPress(KEY_ESCAPE)) return false;
	if(le.MousePressRight(Rect(0, 0, display.w(), display.h()))) return false;
    }    
    return MagicCycle(hero1, hero2, ta1, ta2, army1, army2, reflect, spell, tile);
}

void Army::MagicPreCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::Troops*> &aff1, std::vector<Army::Troops*> &aff2, Army::army_t &army1, Army::army_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile)
{
    Background back(Rect(dst_pt.x, dst_pt.y, 640, 480));
    back.Save();
    ICN::icn_t icn = ICN::UNKNOWN;
    int maxframe = 0, icnframe=-1;
    switch(spell) {
    case Spell::COLDRAY:
    case Spell::DISRUPTINGRAY:
    case Spell::ARROW:
	if(aff1.size() + aff2.size() == 1) {
	    Point start, end;
	    if(aff1.size() && hero2) {
		Rect hrect = DrawHero(*hero2, 1, true);
		start = hrect + Point(hrect.w, hrect.h/2);
		end = Bf2Scr(aff1[0]->Position())+dst_pt;
	    } else if(aff2.size() && hero1) {
		Rect hrect = DrawHero(*hero1, 1);
		start = hrect + Point(0, hrect.h/2);
		end = Bf2Scr(aff2[0]->Position())+dst_pt;
	    } else break;
	    int animat = 0;
	    int frame = 0;
	    Point delta = end - start;
	    switch(spell) {
	    case Spell::COLDRAY:
		icn = ICN::COLDRAY;
		maxframe = AGG::GetICNCount(icn);
		break;
	    case Spell::DISRUPTINGRAY:
		icn = ICN::DISRRAY;
		maxframe = AGG::GetICNCount(icn);
		break;
	    case Spell::ARROW: {
		icn = ICN::KEEP;
		maxframe = 16;
		int maxind = AGG::GetICNCount(icn);
		if(maxind > 1) {
		    double angle = M_PI_2 - atan2(-delta.y, delta.x);
		    icnframe = (int)(angle/M_PI * maxind);
		} else icnframe = 0;
		break;
	    }
	    default:
		break;
	    }
	    delta.x /= maxframe;
	    delta.y /= maxframe;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(animat++, 3)) {
		    display.Blit(AGG::GetICN(icn, icnframe>=0? icnframe : frame, reflect), start);
		    display.Flip();
		    start += delta;
		    frame ++;
		    if(frame >= maxframe) break;
		}
	    }
	}
	break;
    case Spell::LIGHTNINGBOLT:
    case Spell::CHAINLIGHTNING:
    case Spell::HOLYWORD:
    case Spell::HOLYSHOUT:
    case Spell::ARMAGEDDON:
    case Spell::ELEMENTALSTORM:
    case Spell::METEORSHOWER:
    case Spell::DEATHRIPPLE:
    case Spell::DEATHWAVE:
    case Spell::EARTHQUAKE:
	// TODO
    default:
	break;
    }
}

bool Army::MagicCycle(Heroes *hero1, Heroes *hero2, std::vector<Army::Troops*> &aff1, std::vector<Army::Troops*> &aff2, Army::army_t &army1, Army::army_t &army2, bool reflect, Spell::spell_t spell, const Maps::Tiles &tile)
{
    if(spell == Spell::NONE) return false;
    if(!aff1.size() && !aff2.size()) {
	Dialog::Message("", "That spell will affect no one!", Font::BIG, Dialog::OK);
	return false;
    }
    MagicPreCycle(hero1, hero2, aff1, aff2, army1, army2, reflect, spell, tile);
    AGG::PlaySound(Spell::M82(spell));
    int frame = 0, mframe = 0;
    if(Spell::Icn(spell) != ICN::UNKNOWN) mframe = AGG::GetICNCount(Spell::Icn(spell));
    int damage = 0;
    int spellpower = (reflect?hero2:hero1) ? (reflect?hero2:hero1)->GetPower() : 1 ;
    if(Spell::Target(spell) != Spell::ONEFRIEND && Spell::Target(spell) != Spell::ALLFRIEND) {
	if(Spell::Power(spell) > 1) {
	    damage = Spell::Power(spell) * spellpower;
	}
    }
    if(damage) {
	for(u16 i=0; i<aff1.size(); i++) {
	    if(damage < (aff1[i]->Count()-1) * Monster::GetStats(aff1[i]->Monster()).hp + aff1[i]->hp)
		aff1[i]->Animate(Monster::AS_PAIN);
	    else
		aff1[i]->Animate(Monster::AS_DIE);
	}
	for(u16 i=0; i<aff2.size(); i++) {
	    if(damage < (aff2[i]->Count()-1) * Monster::GetStats(aff2[i]->Monster()).hp + aff2[i]->hp)
		aff2[i]->Animate(Monster::AS_PAIN);
	    else
		aff2[i]->Animate(Monster::AS_DIE);
	}
    }
    int animat = 0;
    while(le.HandleEvents()) {
	if(Game::ShouldAnimateInfrequent(animat++, 3)) {
	    // paint
	    DrawBackground(tile);
	    if(hero1) DrawHero(*hero1, 1, false, 1);
	    if(hero2) DrawHero(*hero2, 1, true);
	    Point p;
	    int t=-1;
	    for(p.y = 0; p.y < BFH; p.y++)
		for(p.x = 0; p.x < BFW; p.x++) {
		    DrawObject(p);
		    if(t = FindTroop(army1, p, false, false), t >= 0) {
			DrawTroop(army1[t], false);
			army1[t].Animate();
		    }
		    if(t = FindTroop(army2, p,  true, false), t >= 0) {
			DrawTroop(army2[t],  true);
			army2[t].Animate();
		    }
		}
	    const Sprite& spr = AGG::GetICN(Spell::Icn(spell), frame);
	    for(u16 i=0; i<aff1.size(); i++) display.Blit(spr, Bf2Scr(aff1[i]->Position())+dst_pt+spr);
	    for(u16 i=0; i<aff2.size(); i++) display.Blit(spr, Bf2Scr(aff2[i]->Position())+dst_pt+spr);
	    display.Flip();
	    if(frame >= mframe) {
		if(damage) {
		    bool br = true;
		    for(u16 i=0; i<aff1.size(); i++) if(aff1[i]->astate != Monster::AS_NONE) {
			u8 strt=0, len=0;
			Monster::GetAnimFrames(aff1[i]->Monster(), Monster::AS_DIE, strt, len, false);
			if(aff1[i]->astate != Monster::AS_DIE || aff1[i]->aframe < strt+len-1)
			    br = false;
		    }
		    for(u16 i=0; i<aff2.size(); i++) if(aff2[i]->astate != Monster::AS_NONE) {
			u8 strt=0, len=0;
			Monster::GetAnimFrames(aff2[i]->Monster(), Monster::AS_DIE, strt, len, false);
			if(aff2[i]->astate != Monster::AS_DIE || aff2[i]->aframe < strt+len-1)
			    br = false;
		    }
		    if(br) break;
		} else break;
	    }
	    frame ++;
	}
    }
    for(u16 i=0; i<aff1.size(); i++) {
	if(!reflect || AllowSpell(spell, *aff1[i]))
	    Spell::ApplySpell(spellpower, spell, *aff1[i]);
	else {};// TODO resistance
	if(aff1[i]->Count() <= 0) {
	    Army::Troops body = *aff1[i];
	    for(u16 j=0; j<army1.size(); j++) if(&army1[j] == aff1[i])
		army1.erase(army1.begin()+j);
	    bodies1.push_back(body);
	}
    }
    for(u16 i=0; i<aff2.size(); i++) {
	if(reflect || AllowSpell(spell, *aff2[i]))
	    Spell::ApplySpell(spellpower, spell, *aff2[i]);
	else {};// TODO resistance
	if(aff2[i]->Count() <= 0) {
	    Army::Troops body = *aff2[i];
	    for(u16 j=0; j<army2.size(); j++) if(&army2[j] == aff2[i])
		army2.erase(army2.begin()+j);
	    bodies2.push_back(body);
	}
    }
    if(damage) {
	std::string str = "The " + Spell::String(spell) + " does ";
	String::AddInt(str, damage);
	str += " damage";
	if(aff1.size() + aff2.size() == 1) str += " to the "+Monster::String((aff1.size()?aff1[0]:aff2[0])->Monster()) + "s.";
	else str += ".";
	AttackStatus(str);
	AttackStatus("");
    }
    return true;
}

void Army::InitBackground(const Maps::Tiles & tile)
{
    blockedCells.clear();
    cobjects.clear();
    bodies1.clear();
    bodies2.clear();
    int obj = Rand::Get(1,4);
    for(int i=0; i < obj; i++) {
	Point p;
	p.x = Rand::Get(2, BFW-4);
	p.y = Rand::Get(0, BFH-1);
	blockedCells.push_back(p);
	CObj cobj;
	if(tile.GetObject() == MP2::OBJN_GRAVEYARD) {
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0000;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0001;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0025;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	}
	else switch(tile.GetGround()) {
	case Maps::Ground::DESERT: 
	    switch(Rand::Get(0,1)) {
	    case 0:
		cobj.icn = ICN::COBJ0009;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0024;
		break;
	    }
	    break;
	case Maps::Ground::SNOW: 
	    switch(Rand::Get(0,1)) {
	    case 0:
		cobj.icn = ICN::COBJ0022;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0026;
		break;
	    }
	    break;
	case Maps::Ground::SWAMP: 
	    switch(Rand::Get(0,3)) {
	    case 0:
		cobj.icn = ICN::COBJ0004;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0006;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0015;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0016;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::WASTELAND: 
	    switch(Rand::Get(0,3)) {
	    case 0:
		cobj.icn = ICN::COBJ0013;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0018;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0020;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0021;
		break;
	    }
	    break;
	case Maps::Ground::BEACH: 
	    switch(Rand::Get(0,0)) {
	    case 0:
		cobj.icn = ICN::COBJ0017;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::LAVA: 
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0029;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0030;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0031;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    }
	    break;
	case Maps::Ground::GRASS:
	case Maps::Ground::DIRT: 
	    switch(Rand::Get(0,9)) {
	    case 0:
		cobj.icn = ICN::COBJ0002;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0005;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0007;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 3:
		cobj.icn = ICN::COBJ0011;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 4:
		cobj.icn = ICN::COBJ0012;
		break;
	    case 5:
		cobj.icn = ICN::COBJ0014;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 6:
		cobj.icn = ICN::COBJ0019;
		p.x ++;
		blockedCells.push_back(p);
		p.x --;
		break;
	    case 7:
		cobj.icn = ICN::COBJ0027;
		break;
	    case 8:
		cobj.icn = ICN::COBJ0028;
		break;
	    case 9:
		cobj.icn = ICN::COBJ0008;
		break;
	    }
	    break;
	case Maps::Ground::WATER: 
	    switch(Rand::Get(0,2)) {
	    case 0:
		cobj.icn = ICN::COBJ0003;
		break;
	    case 1:
		cobj.icn = ICN::COBJ0010;
		break;
	    case 2:
		cobj.icn = ICN::COBJ0023;
		break;
	    }
	    break;
	default:
	    cobj.icn = ICN::COBJ0000;
	    break;
	}
	//const Sprite & sp = AGG::GetICN(cobj.icn, 0, false);
	cobj.bfpoint = p;
	cobj.scrpoint = Bf2Scr(p)+dst_pt;
	//cobj.point.y -= sp.h();
	//cobj.point.x -= CELLH * 0.5f;
	cobjects.push_back(cobj);
    }
}

void Army::DrawBackground(const Maps::Tiles & tile)
{
    ICN::icn_t icn = ICN::UNKNOWN;
    ICN::icn_t frng = ICN::UNKNOWN;
    bool trees = false;
    u16 index = tile.GetIndex();
    u16 x = index%world.w(), y = index/world.w();
    if((x < world.w()-1 && world.GetTiles(index+1).GetObject() == MP2::OBJ_TREES) ||
       (x > 0 && world.GetTiles(index-1).GetObject() == MP2::OBJ_TREES) ||
       (y < world.h()-1 && world.GetTiles(index+world.w()).GetObject() == MP2::OBJ_TREES) ||
       (y > 0 && world.GetTiles(index-world.w()).GetObject() == MP2::OBJ_TREES) )
	trees = true;
    if(tile.GetObject() == MP2::OBJN_GRAVEYARD) {
	icn = ICN::CBKGGRAV;
	frng = ICN::FRNG0001;
    }
    else switch(tile.GetGround()) {
    case Maps::Ground::DESERT: 
	icn = ICN::CBKGDSRT;
	frng = ICN::FRNG0003; 
	break;
    case Maps::Ground::SNOW: 
	icn = trees ? ICN::CBKGSNTR : ICN::CBKGSNMT; 
	frng = trees ? ICN::FRNG0006 : ICN::FRNG0007;
	break;
    case Maps::Ground::SWAMP: 
	icn = ICN::CBKGSWMP; 
	frng = ICN::FRNG0008; 
	break;
    case Maps::Ground::WASTELAND: 
	icn = ICN::CBKGCRCK; 
	frng = ICN::FRNG0002; 
	break;
    case Maps::Ground::BEACH: 
	icn = ICN::CBKGBEAC; 
	frng = ICN::FRNG0004; 
	break;
    case Maps::Ground::LAVA: 
	icn = ICN::CBKGLAVA; 
	frng = ICN::FRNG0005; 
	break;
    case Maps::Ground::DIRT: 
	icn = trees ? ICN::CBKGDITR : ICN::CBKGDIMT; 
	frng = trees ? ICN::FRNG0010 : ICN::FRNG0009;
	break;
    case Maps::Ground::GRASS:
	icn = trees ? ICN::CBKGGRTR : ICN::CBKGGRMT; 
	frng = trees ? ICN::FRNG0011 : ICN::FRNG0012;
	break;
    case Maps::Ground::WATER: 
	icn = ICN::CBKGWATR; 
	frng = ICN::FRNG0013; 
	break;
    default:
	return;
    }
    display.Blit(AGG::GetICN(icn, 0), dst_pt);
    Point pt = dst_pt;
    pt.y += 200;
    display.Blit(AGG::GetICN(frng, 0), pt);
    // draw grid
    if(O_GRID) {
	Point p(0,0);
	for(p.x=0; p.x<BFW; p.x++)
	    for(p.y=0; p.y<BFH; p.y++)
		DrawCell(p);
    }
    for(u16 i=0; i<bodies1.size(); i++)
	DrawTroop(bodies1[i], false);
    for(u16 i=0; i<bodies2.size(); i++)
	DrawTroop(bodies2[i], true);
//     for(u16 i=0; i<cobjects.size(); i++) {
// 	const Sprite& spr = AGG::GetICN(cobjects[i].icn, 0);
// 	display.Blit(spr, cobjects[i].point.x + spr.x(), cobjects[i].point.y +spr.y());
//     }
}

void Army::DrawObject(const Point &pt)
{
    for(u16 i=0; i<cobjects.size(); i++) if(cobjects[i].bfpoint == pt) {
	const Sprite& spr = AGG::GetICN(cobjects[i].icn, 0);
	display.Blit(spr, cobjects[i].scrpoint.x + spr.x(), cobjects[i].scrpoint.y + spr.y());
	break;
    }
}

void Army::DrawCaptain(const Race::race_t race, u16 animframe, bool reflect)
{
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

Rect Army::DrawHero(const Heroes & hero, u16 animframe, bool reflect, int fframe)
{
    static int flagframe = 0;
    flagframe += fframe;
    Rect r;
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
    r.x = dst_pt.x + (reflect?640-sp.w():0);
    r.y = dst_pt.y + 75;
    r.w = sp.w();
    r.h = sp.h();
    display.Blit(sp, r.x, r.y);
    ICN::icn_t flag = ICN::UNKNOWN;
    switch(hero.GetColor()) {
    case Color::BLUE: flag = ICN::HEROFL00; break;
    case Color::GREEN: flag = ICN::HEROFL01; break;
    case Color::RED: flag = ICN::HEROFL02; break;
    case Color::YELLOW: flag = ICN::HEROFL03; break;
    case Color::ORANGE: flag = ICN::HEROFL04; break;
    case Color::PURPLE: flag = ICN::HEROFL05; break;
    case Color::GRAY: flag = ICN::HEROFL06; break;
    }
    display.Blit(AGG::GetICN(flag, flagframe%5, reflect), dst_pt.x + (reflect?640-sp.w()-17:17), dst_pt.y + 80);
    return r;
}

void Army::DrawArmy(Army::army_t & army1, Army::army_t & army2, int animframe)
{
    Point p;
    int t=-1;
    for(p.y = 0; p.y < BFH; p.y++) 
	for(p.x = 0; p.x < BFW; p.x++) {
	    DrawObject(p);
	    if(t = FindTroop(army1, p, false, false), t >= 0) {
		DrawTroop(army1[t], false, animframe);
		army1[t].Animate();
		if(army1[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
		    army1[t].Animate(Monster::AS_IDLE);
	    }
	    if(t = FindTroop(army2, p,  true, false), t >= 0) {
		DrawTroop(army2[t],  true, animframe);
		army2[t].Animate();
		if(army2[t].astate == Monster::AS_NONE && !Rand::Get(0, 30))
		    army2[t].Animate(Monster::AS_IDLE);
	    }
	}
}

void Army::DrawTroop(Army::Troops & troop, bool reflect, int animframe)
{
    if(troop.Monster() == Monster::UNKNOWN) return;
    Point tp = Bf2Scr(troop.Position()) + dst_pt;
    troop.Blit(tp, reflect, animframe);
    // draw count
    if(troop.Count() <= 0) return;
    int offset = Monster::GetStats(troop.Monster()).wide ? CELLW : 0;
    tp.x += reflect ? -30 - offset : 10 + offset;
    tp.y -= 10;
    // TODO color
    u16 ind = 10;  // blue
    // ind = 11;  // blue 2
    // ind = 12;  // green
    // ind = 13;  // yellow
    // ind = 14;  // red
    display.Blit(AGG::GetICN(ICN::TEXTBAR, ind), tp);
    std::string str;
    int count = troop.Count();
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

void Army::InitArmyPosition(Army::army_t & army, bool compact, bool reflect)
{
    int x = reflect ? 10 : 0;
    int y = compact ? 2 : 0;
    //for(std::vector<Army::Troops>::iterator it = army.begin(); it != army.end(); it++) {
    for(unsigned int i=0; i < army.size(); i++) {
	if(army[i].Monster() != Monster::UNKNOWN) {
	    const Monster::stats_t &stats = Monster::GetStats(army[i].Monster());
	    AGG::PreloadObject(stats.file_icn);
	    army[i].SetPosition(Point(x, y));
	    army[i].aframe = 1;
	    army[i].astate = Monster::AS_NONE;
	    army[i].shots = stats.shots;
	    army[i].hp = stats.hp;
	    army[i].oldcount = army[i].Count();
	}
	y += compact ? 1 : 2;
    }
}

// translate coordinate to batle field
inline Point Army::Scr2Bf(const Point & pt)
{
    return Point((pt.x - BFX + (((pt.y - BFY + CELLH)/CELLH) % 2 ? CELLW/2 : 0))/CELLW , (pt.y - BFY + CELLH)/CELLH);
}

// translate to screen (offset to frame border)
inline Point Army::Bf2Scr(const Point & pt)
{
    return Point(BFX + pt.x*CELLW + (pt.y % 2 ? 0 : CELLW/2), BFY + pt.y*CELLH);
}

inline bool Army::BfValid(const Point & pt)
{
    return pt.x >= 0 && pt.x < BFW && pt.y >= 0 && pt.y < BFH;
}

// find troop at the position
int Army::FindTroop(const Army::army_t &army, const Point &p, bool reflect, bool usewide)
{
    for(unsigned int i=0; i<army.size(); i++) {
	Point p2(army[i].Position());
	if(Monster::GetStats(army[i].Monster()).wide && usewide) p2.x += (reflect ? -1 : 1);
	if(army[i].Monster() != Monster::UNKNOWN && (p == army[i].Position() || p == p2)) 
	    return i;
	}
    return -1;
}

// find troop at the position
int Army::FindTroop(const std::vector<Army::Troops*> &army, const Point &p, bool reflect, bool usewide)
{
    for(unsigned int i=0; i<army.size(); i++) {
	Point p2(army[i]->Position());
	if(Monster::GetStats(army[i]->Monster()).wide && usewide) p2.x += (reflect ? -1 : 1);
	if(army[i]->Monster() != Monster::UNKNOWN && (p == army[i]->Position() || p == p2)) 
	    return i;
	}
    return -1;
}

bool Army::CellFree(const Point &p, const Army::army_t &army1, const Army::army_t &army2, int skip)
{
    int t;
    if(!BfValid(p)) return false;
    if(t = FindTroop(army1, p), t >= 0 && army1[t].Count() > 0 && (skip < 0 || skip != t)) 
	return false;
    if(t = FindTroop(army2, p, true), t >= 0 && army2[t].Count() > 0 && (skip >= 0 || -skip-1 != t)) 
	return false;
    for(u16 i=0; i<blockedCells.size(); i++) if(p == blockedCells[i]) return false;
    return true;
}

void Army::SettingsDialog()
{ 
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::CSPANBKE : ICN::CSPANBKG;
    const ICN::icn_t butt = H2Config::EvilInterface() ? ICN::CSPANBTE : ICN::CSPANBTN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

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

Army::battle_t Army::HeroStatus(Heroes &hero, Dialog::StatusBar &statusBar, Spell::spell_t &spell, bool quickshow, bool cansurrender, bool locked)
{
    spell = Spell::NONE;
    const ICN::icn_t sett = H2Config::EvilInterface() ? ICN::VGENBKGE : ICN::VGENBKG;
    const ICN::icn_t butt = ICN::VIEWGEN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    display.Blit(Portrait::Hero(hero.GetHeroes(), Portrait::BIG), pos_rt.x + 27, pos_rt.y + 42);
    display.Blit(AGG::GetICN(butt, Color::GetIndex(hero.GetColor())+1), pos_rt.x + 148, pos_rt.y + 36);
    Point tp(pos_rt);
    std::string str;
    str = hero.GetName() + " the " + Race::String(hero.GetRace());
    tp.x += 8 + pos_rt.w/2 - Text::width(str, Font::SMALL)/2;
    tp.y += 10;
    Text(str, Font::SMALL, tp);
    str = "Attack: ";
    String::AddInt(str, hero.GetAttack());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 40;
    Text(str, Font::SMALL, tp);
    str = "Defense: ";
    String::AddInt(str, hero.GetDefense());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 51;
    Text(str, Font::SMALL, tp);
    str = "Spell Power: ";
    String::AddInt(str, hero.GetPower());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 62;
    Text(str, Font::SMALL, tp);
    str = "Knowledge: ";
    String::AddInt(str, hero.GetKnowledge());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 73;
    Text(str, Font::SMALL, tp);
    str = "Morale: " + Morale::String(hero.GetMorale());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 84;
    Text(str, Font::SMALL, tp);
    str = "Luck: " + Luck::String(hero.GetLuck());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 95;
    Text(str, Font::SMALL, tp);
    str = "Spell Points: ";
    String::AddInt(str, hero.GetSpellPoints());
    str += "/";
    String::AddInt(str, hero.GetMaxSpellPoints());
    tp.x = pos_rt.x + 205 - Text::width(str, Font::SMALL)/2;
    tp.y = pos_rt.y + 117;
    Text(str, Font::SMALL, tp);
    
    Button buttonMag(pos_rt.x + 30, pos_rt.y + 148, butt, 9, 10);
    Button buttonRet(pos_rt.x + 89, pos_rt.y + 148, butt, 11, 12);
    Button buttonSur(pos_rt.x + 148, pos_rt.y + 148, butt, 13, 14);
    Button buttonOK(pos_rt.x + 207, pos_rt.y + 148, butt, 15, 16);
    buttonMag.SetDisable(!hero.SpellBook().Active() || hero.spellCasted || locked || quickshow);
    buttonRet.SetDisable(locked || quickshow);
    buttonSur.SetDisable(!cansurrender || locked || quickshow);
    buttonMag.Draw();
    buttonRet.Draw();
    buttonSur.Draw();
    buttonOK.Draw();

    display.Flip();
    cursor.Show();

    // dialog menu loop
    while(le.HandleEvents()) {
        if(quickshow) {
	    if(!le.MouseRight()) break;
        } else {
	    // exit
	    if(le.KeyPress(KEY_ESCAPE)) {
		return WIN;
	    }
	    do_button(buttonMag, {if(spell=hero.SpellBook().Open(Spell::Book::CMBT, true),spell!=Spell::NONE) {back.Restore();return WIN;}}, Dialog::Message("Cast Spell", "Cast a magical spell. You may only cast one spell per combat round. The round is reset when every creature has had a turn.", Font::BIG));
	    do_button(buttonRet, return RETREAT, Dialog::Message("Retreat", "Retreat your hero, abandoning your creatures. Your hero will be available for you to recrut again, however, the hero will have only a novice hero's forces.", Font::BIG));
	    do_button(buttonSur, return SURRENDER, Dialog::Message("Surrender", "Surrendering costs gold. However if you pay the ransom, the hero and all of his or her surviving creatures will be available to recruit again.", Font::BIG));
	    do_button(buttonOK, return WIN, Dialog::Message("Cancel", "Return to the battle.", Font::BIG));
	    if(le.MouseCursor(buttonMag)) {
		statusBar.ShowMessage("Cast Spell");
	    } else if(le.MouseCursor(buttonRet)) {
		statusBar.ShowMessage("Retreat");
	    } else if(le.MouseCursor(buttonSur)) {
		statusBar.ShowMessage("Surrender");
	    } else if(le.MouseCursor(buttonOK)) {
		statusBar.ShowMessage("Cancel");
	    } else {
		statusBar.ShowMessage("Hero's Options");
	    }
	}
    }
    return WIN;
}

void Army::DrawShadow(const Point &pt)
{
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    if(prep) {
	prep = false;
	u32 gr = shadow.MapRGB(0,0,0, 0x30);
	u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	for(int i=1; i<CELLW/2; i+=2) {
	    r.y--, r.h +=2;
	    shadow.FillRect(gr, r);
	    r.x += 2;
	    r.w -= 4;
	}
	//shadow.SetAlpha(0x30);
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

void Army::DrawCell(const Point&pt)
{
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    if(prep) {
	prep = false;
	u32 gr = shadow.MapRGB(0,128,0, 128);
	u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	shadow.FillRect(tr, r);
	for(int i=1; i<CELLW/2; i+=2) {
	    r.y--, r.h +=2;
	    shadow.FillRect(gr, r);
	    r.y++, r.h -=2;
	    shadow.FillRect(tr, r);
	    r.y--, r.h +=2;
	    r.x += 2;
	    r.w -= 4;
	}
	//shadow.SetAlpha(0x80);
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

void Army::DrawMark(const Point&pt, int animframe)
{
    static int frame = 0;
    const int delta = (int)(((double)CELLW)/6.928);
    static Surface shadow(CELLW, CELLH+2*delta, true);
    static bool prep = true;
    frame += animframe;
    /*if(prep)*/ {
	prep = false;
	u32 gr = shadow.MapRGB(255,255,0, abs((frame%21)-10)*20+55);
	//u32 tr = shadow.MapRGB(255,255,255, 1);
	Rect r(0,0,shadow.w(), shadow.h());
	//shadow.FillRect(tr, r);
	r.y += 2*delta-1;
	r.h -= 4*delta-2;
	for(int i=r.y; i<r.y+r.h; i++) shadow.SetPixel(r.x, i, gr), shadow.SetPixel(r.x+r.w-1, i, gr);
	//shadow.FillRect(gr, r);
	r.x ++;
	r.w -= 2;
	//shadow.FillRect(tr, r);
	for(; r.x<CELLW/2; r.x++, r.w-=2) {
	    if(r.x%2) r.y--, r.h +=2;
	    for(int x = r.x; x < r.x+r.w; x += r.w-1)
		for(int y = r.y; y < r.y+r.h; y += r.h-1)
		    shadow.SetPixel(x, y, gr);
	    //shadow.FillRect(gr, r);
	}
    }
    Point p = Bf2Scr(pt) + dst_pt;
    p.y -= CELLH+delta;
    p.x -= CELLW/2;
    display.Blit(shadow, p);
}

void Army::PrepMovePoints(int troopN, const Army::army_t &army1, const Army::army_t &army2, bool reflect)
{
    movePoints.clear();
    Point p;
    const Army::Troops &troop = troopN >= 0 ? army1[troopN] : army2[-troopN-1];
    const Monster::stats_t &stat = Monster::GetStats(troop.Monster());
    if(stat.fly) {
	for(p.x = 0; p.x < BFW; p.x ++) 
	    for(p.y = 0; p.y < BFH; p.y ++) {
		if(stat.wide && !CellFree(Point(p.x+(reflect?-1:1), p.y), army1, army2, troopN)) 
		    continue;
		if(CellFree(p, army1, army2, troopN))
		    movePoints.push_back(p);
	    }
    } else {
	Point p = troop.Position();
	movePoints.push_back(p);
	Point d;
	for(d.x = -1; d.x <= 1; d.x++)
	    for(d.y = -1; d.y <= 1; d.y++) 
		if(d.x || d.y ) {
		    if(p.y%2 && d.y && d.x>0) continue;
		    if(!(p.y%2) && d.y && d.x<0) continue;
		    PrepMovePointsInt(p+d, Speed::Move(stat.speed), army1, army2, stat.wide, troopN, reflect);
		}
    }
}

void Army::PrepMovePointsInt(const Point &p, int move, const Army::army_t &army1, const Army::army_t &army2, bool wide, int skip, bool reflect)
{
    if(BfValid(p) && CellFree(p, army1, army2, skip) && move > 0) {
	if(wide && !CellFree(Point(p.x + (reflect ? -1 : 1), p.y), army1, army2, skip)) return;
	bool exist = false;
	for(u16 i=0; i<movePoints.size(); i++) if(movePoints[i] == p) exist = true;
	if(!exist) movePoints.push_back(p);
	Point d;
	for(d.x = -1; d.x <= 1; d.x++)
	    for(d.y = -1; d.y <= 1; d.y++) 
		if(d.x || d.y ) {
		    if(p.y%2 && d.y && d.x>0) continue;
		    if(!(p.y%2) && d.y && d.x<0) continue;
		    PrepMovePointsInt(p+d, move-1, army1, army2, wide, false, reflect);
		}
    }
}

std::vector<Point> *Army::FindPath(const Point& start, const Point &end, int moves, const Army::army_t &army1, const Army::army_t &army2, int skip, bool wide, bool reflect)
{
    if(moves < 0 || !BfValid(start) || !CellFree(start, army1, army2, skip)) return 0;
    if(start == end) return new std::vector<Point>();
    Point p = start;
    Point d;
    std::vector<Point> *path = 0, *tmpath = 0;
    int length = -1;
    for(d.x = -1; d.x <= 1; d.x++)
	for(d.y = -1; d.y <= 1; d.y++) 
	    if(d.x || d.y ) {
		if(p.y%2 && d.y && d.x>0) continue;
		if(!(p.y%2) && d.y && d.x<0) continue;
		if(tmpath = FindPath(p+d, end, moves-1, army1, army2, skip, wide, reflect),tmpath) {
		    if(length < 0 || length > static_cast<int>(tmpath->size())) {
			length = tmpath->size();
			tmpath->push_back(p+d);
			path = tmpath;
		    } else delete tmpath;
		}
	    }
    return path;
}

void Army::AttackStatus(const std::string &str)
{
    static std::string str1, str2;
    str1 = str2;
    str2 = str;
    statusBar1->ShowMessage(str1);
    statusBar2->ShowMessage(str2);
}

int Army::CanAttack(const Army::Troops &myTroop, const std::vector<Point> &moves, const Army::Troops &enemyTroop, const Point &d, bool reflect)
{
    if(myTroop.Count() <= 0) return -1;
    Point delta = d;
    //delta.x -= CELLW;
    delta.y += CELLH/2;
    const Monster::stats_t &myMonster = Monster::GetStats(myTroop.Monster());
    const Monster::stats_t &enemyMonster = Monster::GetStats(enemyTroop.Monster());
    int mp = -1, min = 9999;
    for(u32 i=0; i<moves.size(); i++) {
	Point d, p1, p2;
	p1 = moves[i];
	p2 = enemyTroop.Position();
	do {
	    for(d.x = -1; d.x <= 1; d.x++)
		for(d.y = -1; d.y <= 1; d.y++) 
		    if(d.x || d.y ) {
			if(p1.y%2 && d.y && d.x>0) continue;
			if(!(p1.y%2) && d.y && d.x<0) continue;
			if(p1 + d == p2) {
			    Point d2 = Bf2Scr(p2)+delta - Bf2Scr(p1);
			    if(abs(d2.x) + abs(d2.y) < min) {
				min = abs(d2.x) + abs(d2.y);
				mp = i;
			    }
			}
		    }
            bool mod = false;
	    if(myMonster.wide && p1.x == moves[i].x) {
		p1.x += reflect ? -1 : 1;
		mod = true;
	    }
	    if(enemyMonster.wide && p2.x == enemyTroop.Position().x) {
		if(!mod) p1 = myTroop.Position();
		p2.x -= reflect ? -1 : 1;
		mod = true;
	    }
            if(mod) continue;
	    else break;
	} while(1);
    }
    return mp;
}

bool Army::GoodMorale(Heroes *hero, const Army::Troops &troop)
{
    if(Monster::GetRace(troop.Monster()) == Race::NECR) return false;
    if(Monster::GetRace(troop.Monster()) == Race::BOMG && troop.Monster() >= Monster::EARTH_ELEMENT) return false;
    int m = Rand::Get(1, MORALEBOUND);
    if(hero && (hero->GetMorale() >= m)) {
	AGG::PlaySound(M82::GOODMRLE);
	for(u16 i=0; i<AGG::GetICNCount(ICN::MORALEG); i++) {
	    const Sprite &sp = AGG::GetICN(ICN::MORALEG, i);
	    Rect pos_rt = sp;
	    pos_rt += Bf2Scr(troop.Position()) + dst_pt;
	    Background back(pos_rt);
	    back.Save();
	    display.Blit(sp, pos_rt);
	    display.Flip();
	    int animat = 0;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
	    }
	    back.Restore();
	}
	AttackStatus("High morale enables the "+Monster::String(troop.Monster())+"s to attack again.");
	return true;
    }
    return false;
}

bool Army::BadMorale(Heroes *hero, const Army::Troops &troop)
{
    if(Monster::GetRace(troop.Monster()) == Race::NECR) return false;
    if(Monster::GetRace(troop.Monster()) == Race::BOMG && troop.Monster() >= Monster::EARTH_ELEMENT) return false;
    int m = -Rand::Get(1, MORALEBOUND);
    if(hero && (hero->GetMorale() <= m)) {
	AGG::PlaySound(M82::BADMRLE);
	for(u16 i=0; i<AGG::GetICNCount(ICN::MORALEB); i++) {
	    const Sprite &sp = AGG::GetICN(ICN::MORALEB, i);
	    Rect pos_rt = sp;
	    pos_rt += Bf2Scr(troop.Position()) + dst_pt;
	    Background back(pos_rt);
	    back.Save();
	    display.Blit(sp, pos_rt);
	    display.Flip();
	    int animat = 0;
	    while(le.HandleEvents()) {
		if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
	    }
	    back.Restore();
	}
	AttackStatus("Low morale causes the "+Monster::String(troop.Monster())+"s to freeze in panic.");
	return true;
    }
    return false;
}

// return 1 for good luck, -1 for bad luck, 0 for none luck
int Army::CheckLuck(Heroes *hero, const Army::Troops &troop)
{
    int luck = Rand::Get(0, MORALEBOUND);
    if(hero) {
	if(hero->GetLuck() < 0 && hero->GetLuck() <= -luck) {
	    AGG::PlaySound(M82::BADLUCK);
	    for(u16 i=0; i<AGG::GetICNCount(ICN::CLOUDLUK); i++) {
		const Sprite &sp = AGG::GetICN(ICN::CLOUDLUK, i);
		Rect pos_rt = sp;
		pos_rt += Bf2Scr(troop.Position()) + dst_pt;
		Background back(pos_rt);
		back.Save();
		display.Blit(sp, pos_rt);
		display.Flip();
		int animat = 0;
		while(le.HandleEvents()) {
		    if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
		}
		back.Restore();
	    }
	    return -1;
	}
	if(hero->GetLuck() > 0 && hero->GetLuck() >= luck) {
	    AGG::PlaySound(M82::GOODLUCK);
	    for(u16 i=0; i<AGG::GetICNCount(ICN::EXPMRL); i++) {
		const Sprite &sp = AGG::GetICN(ICN::EXPMRL, 0);
		Rect pos_rt = sp;
		pos_rt += Bf2Scr(troop.Position()) + dst_pt;
		pos_rt.y -= CELLH*2;
		pos_rt.x -= CELLW/2;
		Background back(pos_rt);
		back.Save();
		display.Blit(sp, pos_rt);
		display.Flip();
		int animat = 0;
		while(le.HandleEvents()) {
		    if(Game::ShouldAnimateInfrequent(++animat, 4)) break;
		}
		back.Restore();
	    }
	    return 1;
	}
    }
    return 0;
}

void Army::Temp(int dicn, int dindex, Point pt)
{
    static int icn=0, index=0;
    if(dicn) {
	icn += dicn;
	index = 0;
    }
    index += dindex;
    if(icn < 0) icn = ICN::UNKNOWN-1;
    if(icn >= ICN::UNKNOWN) icn = 0;
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
	if(index >= AGG::GetICNCount((ICN::icn_t)icn)) break;
    }
    String::AddInt(str, index);
    Text(str, Font::SMALL, pt);
}
