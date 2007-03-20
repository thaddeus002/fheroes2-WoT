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

#include <map>
#include <vector>
#include <string>
#include "background.h"
#include "rand.h"
#include "dir.h"
#include "agg.h"
#include "sprite.h"
#include "spritecursor.h"
#include "error.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "maps_fileinfo.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "tools.h"
#include "splitter.h"
#include "game.h"

#define LISTMAXITEM	9
#define LISTHEIGHTROW	19

typedef struct {
    Color::color_t color;
    Rect rect;
} rectcolor_t;

namespace Scenario {
    void DrawInfo(std::vector<rectcolor_t> &vo,  std::vector<rectcolor_t> &vc);
    void RedrawOpponentColors(const std::vector<rectcolor_t> &vo);
    void SelectMaps(const std::vector<Maps::FileInfo> &allmaps);
    void SetCurrentSettings(const Maps::FileInfo &maps);
    void DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count = LISTMAXITEM);
    void DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current);
}

Game::menu_t Game::ScenarioInfo(void){

    std::vector<Maps::FileInfo> info_maps;

    // read maps directory
    Dir dir1(H2Config::GetMapsDirectory(), "mp2");
    Dir dir2(H2Config::GetMapsDirectory(), "Mp2");
    Dir dir3(H2Config::GetMapsDirectory(), "MP2");

    Dir::const_iterator itd1 = dir1.begin();
    while(itd1 != dir1.end()){ info_maps.push_back(Maps::FileInfo(*itd1)); ++itd1; }

    Dir::const_iterator itd2 = dir2.begin();
    while(itd2 != dir2.end()){ info_maps.push_back(Maps::FileInfo(*itd2)); ++itd2; }

    Dir::const_iterator itd3 = dir3.begin();
    while(itd3 != dir3.end()){ info_maps.push_back(Maps::FileInfo(*itd3)); ++itd3; }

    // empty maps dir
    if(!info_maps.size()){
	H2Config::SetInterface(true);
	Dialog::Message("Warning", "None maps available!", Font::BIG, Dialog::OK);
	return QUITGAME;
    }

    // preload
    AGG::PreloadObject("HEROES.ICN");
    AGG::PreloadObject("NGEXTRA.ICN");
    AGG::PreloadObject("NGHSBKG.ICN");

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    // vector coord colors opponent
    std::vector<rectcolor_t> vo;

    // vector coord class
    std::vector<rectcolor_t> vc;

    Scenario::SetCurrentSettings(info_maps.front());
    Scenario::DrawInfo(vo, vc);
    Scenario::RedrawOpponentColors(vo);

    Button buttonSelectMaps(513, 77, "NGEXTRA.ICN", 64, 65);
    Button buttonOk(234, 413, "NGEXTRA.ICN", 66, 67);
    Button buttonCancel(491, 413, "NGEXTRA.ICN", 68, 69);

    // set levelCursor (default normal)
    std::map<Difficulty::difficulty_t, Point> pointDifficulty;
    pointDifficulty[Difficulty::EASY] = Point(225, 124);
    pointDifficulty[Difficulty::NORMAL] = Point(302, 124);
    pointDifficulty[Difficulty::HARD] = Point(378, 124);
    pointDifficulty[Difficulty::EXPERT] = Point(455, 124);
    pointDifficulty[Difficulty::IMPOSSIBLE] = Point(532, 124);
    SpriteCursor levelCursor(AGG::GetICN("NGEXTRA.ICN", 62), pointDifficulty[Difficulty::NORMAL]);
    H2Config::SetGameDifficulty(Difficulty::NORMAL);

    std::vector<rectcolor_t>::const_iterator it;

    Cursor::Show();
    display.Flip();

    // newstandard loop
    while(1){

	le.HandleEvents();

	// select level
	Difficulty::difficulty_t difficulty = Difficulty::EASY;
	do
	    if(le.MousePressLeft(pointDifficulty[difficulty], levelCursor.GetRect().w, levelCursor.GetRect().h)){
		levelCursor.Move(pointDifficulty[difficulty]);
		display.Flip();
		H2Config::SetGameDifficulty(difficulty);
	    }
	while(++difficulty != Difficulty::EASY);

	// select color
	it = vo.begin();
	while(it != vo.end()){
	    if(le.MousePressLeft((*it).rect) && (*it).color & H2Config::GetAllowChangeColors()){
		Cursor::Hide();
		H2Config::SetHumanColor((*it).color);
		Scenario::RedrawOpponentColors(vo);
		display.Flip();
		Cursor::Show();
	    }
	    ++it;
	}

	// select class
	it = vc.begin();
	while(it != vc.end()){
	    if(le.MouseClickLeft((*it).rect) && (H2Config::GetAllowChangeRaces() & (*it).color)){
		Cursor::Hide();
		u8 index = 0;
		switch(H2Config::GetKingdomRace((*it).color)){
		    case Race::KNGT: index = 52; H2Config::SetKingdomRace((*it).color, Race::BARB); break;
		    case Race::BARB: index = 53; H2Config::SetKingdomRace((*it).color, Race::SORC); break;
		    case Race::SORC: index = 54; H2Config::SetKingdomRace((*it).color, Race::WRLK); break;
		    case Race::WRLK: index = 55; H2Config::SetKingdomRace((*it).color, Race::WZRD); break;
		    case Race::WZRD: index = 56; H2Config::SetKingdomRace((*it).color, Race::NECR); break;
		    case Race::NECR: index = 58; H2Config::SetKingdomRace((*it).color, Race::RAND); break;
		    case Race::RAND: index = 51; H2Config::SetKingdomRace((*it).color, Race::KNGT); break;
		    default: break;
		}
    		const Sprite &sprite = AGG::GetICN("NGEXTRA.ICN", index);
		display.Blit(sprite, (*it).rect);
		display.Flip();
		Cursor::Show();
	    }

	    ++it;
	}

	// press button
	le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.Press() : buttonSelectMaps.Release();
	le.MousePressLeft(buttonOk) ? buttonOk.Press() : buttonOk.Release();
	le.MousePressLeft(buttonCancel) ? buttonCancel.Press() : buttonCancel.Release();

	// click select
	if(le.MouseClickLeft(buttonSelectMaps)){
	    Scenario::SelectMaps(info_maps);
	    Scenario::DrawInfo(vo, vc);
	    Scenario::RedrawOpponentColors(vo);
	    levelCursor.Move(pointDifficulty[Difficulty::NORMAL]);
	    display.Flip();
	    H2Config::SetGameDifficulty(Difficulty::NORMAL);
	}

	// click cancel
	if(le.MouseClickLeft(buttonCancel)) return MAINMENU;

	// click ok
	if(le.MouseClickLeft(buttonOk)){
	    Error::Verbose("select maps: " + H2Config::GetFileMaps());
	    Error::Verbose("difficulty: " + Difficulty::String(H2Config::GetGameDifficulty()));
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
		if(H2Config::GetKingdomColors() & color){
		    if(Race::RAND == H2Config::GetKingdomRace(color)) H2Config::SetKingdomRace(color, Race::Rand());
		    Error::Verbose(Color::String(color) + ": " + Race::String(H2Config::GetKingdomRace(color)));
	    }
	    Error::Verbose("select color: " + Color::String(H2Config::GetHumanColor()));

	    return STARTGAME;
	}
	
	// right info
	if(le.MousePressRight(buttonSelectMaps)) Dialog::Message("Scenario", "Click here to select which scenario to play.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Game Difficulty", "This lets you change the starting difficulty at which you will play. Higher difficulty levels start you of with fewer resources, and at the higher settings, give extra resources to the computer.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Opponents", "This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Class", "This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alingment.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Difficulty Rating", "The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score.", Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message("OK", "Click to accept these settings and start a new game.", Font::BIG);
	if(le.MousePressRight(buttonCancel)) Dialog::Message("Cancel", "Click to return to the main menu.", Font::BIG);
    }

    return QUITGAME;
}

void Scenario::RedrawOpponentColors(const std::vector<rectcolor_t> &vo)
{
    std::map<Color::color_t, const Sprite *> colorHumanSprite;
    colorHumanSprite[Color::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 9);
    colorHumanSprite[Color::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 10);
    colorHumanSprite[Color::RED] = &AGG::GetICN("NGEXTRA.ICN", 11);
    colorHumanSprite[Color::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 12);
    colorHumanSprite[Color::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 13);
    colorHumanSprite[Color::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 14);

    std::map<Color::color_t, const Sprite *> colorAllowSprite;
    colorAllowSprite[Color::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 3);
    colorAllowSprite[Color::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 4);
    colorAllowSprite[Color::RED] = &AGG::GetICN("NGEXTRA.ICN", 5);
    colorAllowSprite[Color::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 6);
    colorAllowSprite[Color::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 7);
    colorAllowSprite[Color::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 8);

    std::map<Color::color_t, const Sprite *> colorOpponentSprite;
    colorOpponentSprite[Color::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 15);
    colorOpponentSprite[Color::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 16);
    colorOpponentSprite[Color::RED] = &AGG::GetICN("NGEXTRA.ICN", 17);
    colorOpponentSprite[Color::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 18);
    colorOpponentSprite[Color::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 19);
    colorOpponentSprite[Color::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 20);

    std::vector<rectcolor_t>::const_iterator it = vo.begin();

    while(it != vo.end()){

	if(H2Config::GetAllowChangeColors() & (*it).color)
	    display.Blit((H2Config::GetHumanColor() & (*it).color ? *colorHumanSprite[(*it).color] : *colorAllowSprite[(*it).color]), (*it).rect);
	else
	    display.Blit(*colorOpponentSprite[(*it).color], (*it).rect);
	++it;
    }
}

void Scenario::DrawInfo(std::vector<rectcolor_t> &vo, std::vector<rectcolor_t> &vc)
{
    // image background
    const Sprite &back = AGG::GetICN("HEROES.ICN", 0);
    display.Blit(back);

    // image panel
    const Sprite &shadow = AGG::GetICN("NGHSBKG.ICN", 1);
    display.Blit(shadow, 196, 40);
    const Sprite &panel = AGG::GetICN("NGHSBKG.ICN", 0);
    display.Blit(panel, 204, 33);

    // text scenario
    Text textScenario(376, 53, "Scenario:", Font::BIG, true);

    // maps name
    Text textName(260, 78, H2Config::GetNameMaps(), Font::BIG, true);
    
    // text game difficulty
    Text textDifficulty(330, 107, "Game Difficulty:", Font::BIG, true);

    //
    Text textEasy(248, 196, "Easy", Font::SMALL, true);
    Text textNormal(316, 196, "Normal", Font::SMALL, true);
    Text textHard(395, 196, "Hard", Font::SMALL, true);
    Text textExpert(472, 196, "Expert", Font::SMALL, true);
    Text textImpossible(536, 196, "Impossible", Font::SMALL, true);

    // text opponents
    Text textOpponents(368, 210, "Opponents:", Font::BIG, true);

    // draw opponents
    u8 count = H2Config::GetKingdomCount();
    u8 current = 0;
    vo.clear();

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
        if(H2Config::GetKingdomColors() & color){

            const Sprite &sprite = AGG::GetICN("NGEXTRA.ICN", 3);

	    rectcolor_t op = { color, Rect(228 + current * sprite.w() * 6 / count + (sprite.w() * (6 - count) / (2 * count)), 234, sprite.w(), sprite.h()) };
	    vo.push_back(op);

            ++current;
    }

    // text class
    Text textClass(386, 290, "Class:", Font::BIG, true);

    // draw class
    current = 0;
    vc.clear();

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(H2Config::GetKingdomColors() & color){

	    u8 index = 0;
	    Point pt;

	    switch(H2Config::GetKingdomRace(color)){
		case Race::KNGT: index = 70; break;
	        case Race::BARB: index = 71; break;
	        case Race::SORC: index = 72; break;
		case Race::WRLK: index = 73; break;
	        case Race::WZRD: index = 74; break;
		case Race::NECR: index = 75; break;
	        case Race::MULT: index = 76; break;
		case Race::RAND: index = 58; break;
		default: continue;
	    }

    	    const Sprite &sprite = AGG::GetICN("NGEXTRA.ICN", index);

    	    pt.x = 228 + current * sprite.w() * 6 / count + (sprite.w() * (6 - count) / (2 * count));
    	    pt.y = 314;

	    rectcolor_t op = { color, Rect(pt.x, pt.y, sprite.w(), sprite.h()) };
	    vc.push_back(op);

	    display.Blit(sprite, pt);

    	    ++current;
    }
}

void Scenario::SelectMaps(const std::vector<Maps::FileInfo> &allmaps)
{
    // preload
    AGG::PreloadObject("REQSBKG.ICN");

    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Maps::FileInfo> smallmaps, mediummaps, largemaps, xlargemaps;

    std::vector<Maps::FileInfo>::const_iterator it = allmaps.begin();
    std::vector<Maps::FileInfo>::const_iterator it_end = allmaps.end();

    while(it != it_end){
	switch((*it).GetSizeMaps()){
    	    case Maps::SMALL:
		smallmaps.push_back(*it);
		break;
    	    case Maps::MEDIUM:
		mediummaps.push_back(*it);
		break;
    	    case Maps::LARGE:
		largemaps.push_back(*it);
		break;
    	    case Maps::XLARGE:
		xlargemaps.push_back(*it);
		break;
	    default:
		break;
	}
	++it;
    }

    const std::vector<Maps::FileInfo> *curmaps = &allmaps;

    // cursor
    Cursor::Hide();
    Cursor::Set(Cursor::POINTER);

    Display::SetVideoMode(Display::SMALL);

    Background background(Rect(110, 0, 510, 480));
    background.Save();

    // image panel
    const Sprite &shadow = AGG::GetICN("REQSBKG.ICN", 1);
    display.Blit(shadow, 114, 21);
    const Sprite &panel = AGG::GetICN("REQSBKG.ICN", 0);
    display.Blit(panel, 130, 5);

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));
    
    // Splitter
    Splitter split(AGG::GetICN("ESCROLL.ICN", 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, "REQUESTS.ICN", 1, 2);
    Button buttonPgUp(457, 60, "REQUESTS.ICN", 5, 6);
    Button buttonPgDn(457, 222, "REQUESTS.ICN", 7, 8);

    Button buttonSelectSmall(167, 28, "REQUESTS.ICN", 9, 10);
    Button buttonSelectMedium(229, 28, "REQUESTS.ICN", 11, 12);
    Button buttonSelectLarge(291, 28, "REQUESTS.ICN", 13, 14);
    Button buttonSelectXLarge(353, 28, "REQUESTS.ICN", 15, 16);
    Button buttonSelectAll(415, 28, "REQUESTS.ICN", 17, 18);

    std::vector<Maps::FileInfo>::const_iterator it_list_head = curmaps->begin();
    std::vector<Maps::FileInfo>::const_iterator it_current = it_list_head;

    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
    Scenario::DrawSelectInfo(it_current);

    display.Flip();
    Cursor::Show();

    // newstandard loop
    while(1){

	le.HandleEvents();

	// press button
	le.MousePressLeft(buttonOk) ? buttonOk.Press() : buttonOk.Release();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.Press() : buttonPgUp.Release();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.Press() : buttonPgDn.Release();

	le.MousePressLeft(buttonSelectSmall) ? buttonSelectSmall.Press() : buttonSelectSmall.Release();
	le.MousePressLeft(buttonSelectMedium) ? buttonSelectMedium.Press() : buttonSelectMedium.Release();
	le.MousePressLeft(buttonSelectLarge) ? buttonSelectLarge.Press() : buttonSelectLarge.Release();
	le.MousePressLeft(buttonSelectXLarge) ? buttonSelectXLarge.Press() : buttonSelectXLarge.Release();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.Press() : buttonSelectAll.Release();

	// click small
	if(le.MouseClickLeft(buttonSelectSmall) && smallmaps.size()){
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    display.Flip();
	    Cursor::Show();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium) && mediummaps.size()){
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    display.Flip();
	    Cursor::Show();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge) && largemaps.size()){
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    display.Flip();
	    Cursor::Show();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge) && xlargemaps.size()){
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    display.Flip();
	    Cursor::Show();
	}

	// click all
	if(le.MouseClickLeft(buttonSelectAll)){
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    display.Flip();
	    Cursor::Show();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList)){
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num > curmaps->size()) num = curmaps->size() - 1;
	    Cursor::Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head + num;
	    Scenario::DrawSelectInfo(it_current);
	    split.Move(it_list_head - curmaps->begin());
	    display.Flip();
	    Cursor::Show();
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp)) &&
	    curmaps->size() && it_list_head > curmaps->begin()){
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(--it_list_head);
	    split.Backward();
	    display.Flip();
	    Cursor::Show();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn)) &&
	    LISTMAXITEM < curmaps->size() && it_list_head + LISTMAXITEM < curmaps->end()){
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(++it_list_head);
	    split.Forward();
	    display.Flip();
	    Cursor::Show();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM && le.MouseCursor(split.GetRect())){
	//    Error::Verbose("move on splitter");
	    u16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek > curmaps->size() - LISTMAXITEM) seek = curmaps->size() - LISTMAXITEM;

	    it_list_head = curmaps->begin() + seek;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, LISTMAXITEM);
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.Move(seek);
	    display.Flip();
	    Cursor::Show();
 	}

	// right info
	if(le.MousePressRight(buttonSelectSmall)) Dialog::Message("Small Maps", "View only maps of size small (36x36).", Font::BIG);
	if(le.MousePressRight(buttonSelectMedium)) Dialog::Message("Medium Maps", "View only maps of size medium (72x72).", Font::BIG);
	if(le.MousePressRight(buttonSelectLarge)) Dialog::Message("Large Maps", "View only maps of size large (108x108).", Font::BIG);
	if(le.MousePressRight(buttonSelectXLarge)) Dialog::Message("Extra Large Maps", "View only maps of size extra large (144x144).", Font::BIG);
	if(le.MousePressRight(buttonSelectAll)) Dialog::Message("All Maps", "View all maps, regardless of size.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Players Icon", "Indicates how many players total are in the scenario. Any positions not occupied by humans will be occupied by computer players.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Size Icon", "Indicates whether the maps is small (36x36), medium (72x72), large (108x108), or extra large (144x144).", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Name", "The name of the currently selected map.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Victory Condition Icon", "There are 6 possiblities:.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Loss Condition Icon", "There are 4 possible loss conditions, as indicated by the following icons:.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Map Difficulty", "The map difficulty of the currently selected map.  The map difficulty is determined by the scenario designer. More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player.", Font::BIG);
	//if(le.MousePressRight(?)) Dialog::Message("Selected Description", "The description of the currently selected map.", Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message("OK", "Accept the choice made.", Font::BIG);

	// click ok
	if(le.MouseClickLeft(buttonOk)) break;
    }

    // set current settings
    Scenario::SetCurrentSettings(*it_current);

    Cursor::Hide();
    background.Restore();
    Cursor::Show();
}

void Scenario::SetCurrentSettings(const Maps::FileInfo &maps)
{
    H2Config::SetKingdomCount(maps.GetKingdomCount());
    H2Config::SetKingdomColors(maps.GetKingdomColors());
    H2Config::SetAllowChangeColors(maps.GetAllowColors());
    H2Config::SetAllowChangeRaces(0);
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
        switch(maps.GetKingdomRace(color)){
            case 0x00:
                H2Config::SetKingdomRace(color, Race::KNGT);
                break;
            case 0x01:
                H2Config::SetKingdomRace(color, Race::BARB);
                break;
            case 0x02:
                H2Config::SetKingdomRace(color, Race::SORC);
                break;
            case 0x03:
                H2Config::SetKingdomRace(color, Race::WRLK);
                break;
            case 0x04:
                H2Config::SetKingdomRace(color, Race::WZRD);
                break;
            case 0x05:
                H2Config::SetKingdomRace(color, Race::NECR);
                break;
            case 0x06:
                H2Config::SetKingdomRace(color, Race::MULT);
                break;
            case 0x07:
                H2Config::SetKingdomRace(color, Race::RAND);
                H2Config::SetAllowChangeRaces(color | H2Config::GetAllowChangeRaces());
                break;
            default:
                H2Config::SetKingdomRace(color, Race::BOMG);
                break;
	}
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	// set first allow color
	if(maps.GetAllowColors() & color){ H2Config::SetHumanColor(color); break; }
    H2Config::SetSizeMaps(maps.GetSizeMaps());
    H2Config::SetFileMaps(maps.GetFileMaps());
    H2Config::SetNameMaps(maps.GetName());
    H2Config::SetDescriptionMaps(maps.GetDescription());
}

void Scenario::DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count)
{
    u16 y = 64;
    u16 x = 176;
    u8 index = 0;
    std::vector<Maps::FileInfo>::const_iterator it_head = it_top;

    for(int ii = 0; ii < count; ++ii){

	// sprite count
	index = 19 + (*it_head).GetKingdomCount();
	const Sprite &spriteCount = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteCount, x, y);

        // sprite size
	switch((*it_head).GetSizeMaps()){
            case Maps::SMALL:
    		index = 26;
                break;
            case Maps::MEDIUM:
    		index = 27;
                break;
            case Maps::LARGE:
    		index = 28;
                break;
            case Maps::XLARGE:
    		index = 29;
                break;
            default:
    		index = 30;
                break;
        }
	const Sprite &spriteSize = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteSize, x + spriteCount.w() + 2, y);

	// text longname
	Text textLongName(x + spriteCount.w() + spriteSize.w() + 18, y, (*it_head).GetName(), Font::BIG, true);

	// sprite wins
	index = 30 + (*it_head).GetConditionsWins();
	const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteWins, x + 224, y);

	// sprite loss
	index = 36 + (*it_head).GetConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteLoss, x + 226 + spriteWins.w(), y);

	y += LISTHEIGHTROW;
	++it_head;
    }
}

void Scenario::DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current)
{
    u16 x = 196;
    u16 y = 269;
    u8 index = 0;

    // sprite count
    index = 19 + (*it_current).GetKingdomCount();
    const Sprite &spriteCount = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteCount, x, y);

    // sprite size
    switch((*it_current).GetSizeMaps()){
        case Maps::SMALL:
    	    index = 26;
            break;
        case Maps::MEDIUM:
    	    index = 27;
            break;
        case Maps::LARGE:
    	    index = 28;
            break;
        case Maps::XLARGE:
    	    index = 29;
            break;
        default:
    	    index = 30;
            break;
    }
    const Sprite &spriteSize = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteSize, x + spriteCount.w() + 2, y);

    // text longname
    Text textLongName(x + spriteCount.w() + spriteSize.w() + 22, y, (*it_current).GetName(), Font::BIG, true);

    // sprite wins
    index = 30 + (*it_current).GetConditionsWins();
    const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteWins, x + 209, y);

    // sprite loss
    index = 36 + (*it_current).GetConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteLoss, x + 211 + spriteWins.w(), y);

    Text textLabel(200, 295, "Maps difficulty:", Font::BIG, true);
    Text textDifficulty(360, 295, Difficulty::String((*it_current).GetDifficulty()), Font::BIG, true);
    
    TextBox textDescription(Rect(175, 322, 282, 90), (*it_current).GetDescription(), Font::BIG, true);
}
