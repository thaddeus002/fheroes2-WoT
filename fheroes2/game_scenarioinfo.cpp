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
#include "event.h"
#include "button.h"
#include "config.h"
#include "maps_fileinfo.h"
#include "text.h"
#include "tools.h"
#include "game.h"

#define LISTMAXITEM	9
#define LISTHEIGHTROW	19

typedef struct {
    Kingdom::color_t color;
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
    Dir dir(H2Config::GetMapsDirectory(), "mp2");

    Dir::const_iterator itd = dir.begin();
    Dir::const_iterator itd_end = dir.end();

    while(itd != itd_end){ info_maps.push_back(Maps::FileInfo(*itd)); ++itd; }

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
    std::map<Maps::difficulty_t, Point> pointDifficulty;
    pointDifficulty[Maps::EASY] = Point(225, 124);
    pointDifficulty[Maps::NORMAL] = Point(302, 124);
    pointDifficulty[Maps::HARD] = Point(378, 124);
    pointDifficulty[Maps::EXPERT] = Point(455, 124);
    pointDifficulty[Maps::IMPOSSIBLE] = Point(532, 124);
    SpriteCursor levelCursor(AGG::GetICN("NGEXTRA.ICN", 62), pointDifficulty[Maps::NORMAL]);
    H2Config::SetGameDifficulty(Maps::NORMAL);

    std::vector<rectcolor_t>::const_iterator it;

    Cursor::Show();
    display.Flip();

    // newstandard loop
    while(1){

	le.HandleEvents();

	// select level
	Maps::difficulty_t difficulty = Maps::EASY;
	do
	    if(le.MousePressLeft(pointDifficulty[difficulty], levelCursor.w(), levelCursor.h())){
		levelCursor.Move(pointDifficulty[difficulty]);
		display.Flip();
		H2Config::SetGameDifficulty(difficulty);
	    }
	while(++difficulty != Maps::EASY);

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
		    case Kingdom::KNGT: index = 52; H2Config::SetKingdomRace((*it).color, Kingdom::BARB); break;
		    case Kingdom::BARB: index = 53; H2Config::SetKingdomRace((*it).color, Kingdom::SORC); break;
		    case Kingdom::SORC: index = 54; H2Config::SetKingdomRace((*it).color, Kingdom::WRLK); break;
		    case Kingdom::WRLK: index = 55; H2Config::SetKingdomRace((*it).color, Kingdom::WZRD); break;
		    case Kingdom::WZRD: index = 56; H2Config::SetKingdomRace((*it).color, Kingdom::NECR); break;
		    case Kingdom::NECR: index = 58; H2Config::SetKingdomRace((*it).color, Kingdom::RAND); break;
		    case Kingdom::RAND: index = 51; H2Config::SetKingdomRace((*it).color, Kingdom::KNGT); break;
		    default: break;
		}
    		const Sprite &sprite = AGG::GetICN("NGEXTRA.ICN", index);
		display.Blit(sprite, Point((*it).rect.x, (*it).rect.y));
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
	    levelCursor.Move(pointDifficulty[Maps::NORMAL]);
	    display.Flip();
	    H2Config::SetGameDifficulty(Maps::NORMAL);
	}

	// click cancel
	if(le.MouseClickLeft(buttonCancel)) return MAINMENU;

	// click ok
	if(le.MouseClickLeft(buttonOk)){
	    Error::Verbose("select maps: " + H2Config::GetFileMaps());
	    Error::Verbose("difficulty: " + String::Difficulty(H2Config::GetGameDifficulty()));
	    for(Kingdom::color_t color = Kingdom::BLUE; color != Kingdom::GRAY; ++color)
		if(H2Config::GetKingdomColors() & color){
		    if(Kingdom::RAND == H2Config::GetKingdomRace(color)) H2Config::SetKingdomRace(color, Rand::Race());
		    Error::Verbose(String::Color(color) + ": " + String::Race(H2Config::GetKingdomRace(color)));
	    }
	    Error::Verbose("select color: " + String::Color(H2Config::GetHumanColor()));

	    return STARTGAME;
	}
    }

    return QUITGAME;
}

void Scenario::RedrawOpponentColors(const std::vector<rectcolor_t> &vo)
{
    std::map<Kingdom::color_t, const Sprite *> colorHumanSprite;
    colorHumanSprite[Kingdom::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 9);
    colorHumanSprite[Kingdom::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 10);
    colorHumanSprite[Kingdom::RED] = &AGG::GetICN("NGEXTRA.ICN", 11);
    colorHumanSprite[Kingdom::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 12);
    colorHumanSprite[Kingdom::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 13);
    colorHumanSprite[Kingdom::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 14);

    std::map<Kingdom::color_t, const Sprite *> colorAllowSprite;
    colorAllowSprite[Kingdom::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 3);
    colorAllowSprite[Kingdom::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 4);
    colorAllowSprite[Kingdom::RED] = &AGG::GetICN("NGEXTRA.ICN", 5);
    colorAllowSprite[Kingdom::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 6);
    colorAllowSprite[Kingdom::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 7);
    colorAllowSprite[Kingdom::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 8);

    std::map<Kingdom::color_t, const Sprite *> colorOpponentSprite;
    colorOpponentSprite[Kingdom::BLUE] = &AGG::GetICN("NGEXTRA.ICN", 15);
    colorOpponentSprite[Kingdom::GREEN] = &AGG::GetICN("NGEXTRA.ICN", 16);
    colorOpponentSprite[Kingdom::RED] = &AGG::GetICN("NGEXTRA.ICN", 17);
    colorOpponentSprite[Kingdom::YELLOW] = &AGG::GetICN("NGEXTRA.ICN", 18);
    colorOpponentSprite[Kingdom::ORANGE] = &AGG::GetICN("NGEXTRA.ICN", 19);
    colorOpponentSprite[Kingdom::PURPLE] = &AGG::GetICN("NGEXTRA.ICN", 20);

    std::vector<rectcolor_t>::const_iterator it = vo.begin();

    while(it != vo.end()){

	if(H2Config::GetAllowChangeColors() & (*it).color)
	    display.Blit((H2Config::GetHumanColor() & (*it).color ? *colorHumanSprite[(*it).color] : *colorAllowSprite[(*it).color]), Point((*it).rect.x, (*it).rect.y));
	else
	    display.Blit(*colorOpponentSprite[(*it).color], Point((*it).rect.x, (*it).rect.y));
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
    display.Blit(shadow, Point(196, 40));
    const Sprite &panel = AGG::GetICN("NGHSBKG.ICN", 0);
    display.Blit(panel, Point(204, 33));

    // text scenario
    Text textScenario(Point(376, 53), "Scenario:", Font::BIG, true);

    // maps name
    Text textName(Point(260, 78), H2Config::GetNameMaps(), Font::BIG, true);
    
    // text game difficulty
    Text textDifficulty(Point(330, 107), "Game Difficulty:", Font::BIG, true);

    //
    Text textEasy(Point(248, 196), "Easy", Font::SMALL, true);
    Text textNormal(Point(316, 196), "Normal", Font::SMALL, true);
    Text textHard(Point(395, 196), "Hard", Font::SMALL, true);
    Text textExpert(Point(472, 196), "Expert", Font::SMALL, true);
    Text textImpossible(Point(536, 196), "Impossible", Font::SMALL, true);

    // text opponents
    Text textOpponents(Point(368, 210), "Opponents:", Font::BIG, true);

    // draw opponents
    u8 count = H2Config::GetKingdomCount();
    u8 current = 0;
    vo.clear();

    for(Kingdom::color_t color = Kingdom::BLUE; color != Kingdom::GRAY; ++color)
        if(H2Config::GetKingdomColors() & color){

            const Sprite &sprite = AGG::GetICN("NGEXTRA.ICN", 3);

	    rectcolor_t op = { color, Rect(228 + current * sprite.w() * 6 / count + (sprite.w() * (6 - count) / (2 * count)), 234, sprite.w(), sprite.h()) };
	    vo.push_back(op);

            ++current;
    }

    // text class
    Text textClass(Point(386, 290), "Class:", Font::BIG, true);

    // draw class
    current = 0;
    vc.clear();

    for(Kingdom::color_t color = Kingdom::BLUE; color != Kingdom::GRAY; ++color)
	if(H2Config::GetKingdomColors() & color){

	    u8 index = 0;
	    Point pt;

	    switch(H2Config::GetKingdomRace(color)){
		case Kingdom::KNGT: index = 70; break;
	        case Kingdom::BARB: index = 71; break;
	        case Kingdom::SORC: index = 72; break;
		case Kingdom::WRLK: index = 73; break;
	        case Kingdom::WZRD: index = 74; break;
		case Kingdom::NECR: index = 75; break;
	        case Kingdom::MULT: index = 76; break;
		case Kingdom::RAND: index = 58; break;
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
    display.Blit(shadow, Point(114, 21));
    const Sprite &panel = AGG::GetICN("REQSBKG.ICN", 0);
    display.Blit(panel, Point(130, 5));

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));

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
	if(le.MouseClickLeft(buttonSelectSmall)){
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    display.Flip();
	    Cursor::Show();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium)){
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    display.Flip();
	    Cursor::Show();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge)){
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    display.Flip();
	    Cursor::Show();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge)){
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
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
	    display.Flip();
	    Cursor::Show();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList)){
	    int num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    Cursor::Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head + num;
	    Scenario::DrawSelectInfo(it_current);
	    display.Flip();
	    Cursor::Show();
	}

	// click up
	if(le.MouseWheelUp(rectAreaList) ||
	   (le.MouseClickLeft(buttonPgUp) && curmaps->size() && it_list_head > curmaps->begin())){
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(--it_list_head);
	    display.Flip();
	    Cursor::Show();
	}

	// click down
	if(le.MouseWheelDn(rectAreaList) ||
	   (le.MouseClickLeft(buttonPgDn) && LISTMAXITEM < curmaps->size() && it_list_head + LISTMAXITEM < curmaps->end())){
	    Cursor::Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(++it_list_head);
	    display.Flip();
	    Cursor::Show();
	}

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
    for(Kingdom::color_t color = Kingdom::BLUE; color != Kingdom::GRAY; ++color)
        switch(maps.GetKingdomRace(color)){
            case 0x00:
                H2Config::SetKingdomRace(color, Kingdom::KNGT);
                break;
            case 0x01:
                H2Config::SetKingdomRace(color, Kingdom::BARB);
                break;
            case 0x02:
                H2Config::SetKingdomRace(color, Kingdom::SORC);
                break;
            case 0x03:
                H2Config::SetKingdomRace(color, Kingdom::WRLK);
                break;
            case 0x04:
                H2Config::SetKingdomRace(color, Kingdom::WZRD);
                break;
            case 0x05:
                H2Config::SetKingdomRace(color, Kingdom::NECR);
                break;
            case 0x06:
                H2Config::SetKingdomRace(color, Kingdom::MULT);
                break;
            case 0x07:
                H2Config::SetKingdomRace(color, Kingdom::RAND);
                H2Config::SetAllowChangeRaces(color | H2Config::GetAllowChangeRaces());
                break;
            default:
                H2Config::SetKingdomRace(color, Kingdom::BOMG);
                break;
	}
    for(Kingdom::color_t color = Kingdom::BLUE; color != Kingdom::GRAY; ++color)
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
	display.Blit(spriteCount, Point(x, y));

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
	display.Blit(spriteSize, Point(x + spriteCount.w() + 2, y));

	// text longname
	Text textLongName(Point(x + spriteCount.w() + spriteSize.w() + 18, y), (*it_head).GetName(), Font::BIG, true);

	// sprite wins
	index = 30 + (*it_head).GetConditionsWins();
	const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteWins, Point(x + 224, y));

	// sprite loss
	index = 36 + (*it_head).GetConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
	display.Blit(spriteLoss, Point(x + 226 + spriteWins.w(), y));

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
    display.Blit(spriteCount, Point(x, y));

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
    display.Blit(spriteSize, Point(x + spriteCount.w() + 2, y));

    // text longname
    Text textLongName(Point(x + spriteCount.w() + spriteSize.w() + 22, y), (*it_current).GetName(), Font::BIG, true);

    // sprite wins
    index = 30 + (*it_current).GetConditionsWins();
    const Sprite &spriteWins = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteWins, Point(x + 209, y));

    // sprite loss
    index = 36 + (*it_current).GetConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN("REQUESTS.ICN", index);
    display.Blit(spriteLoss, Point(x + 211 + spriteWins.w(), y));

    Text textLabel(Point(200, 295), "Maps difficulty:", Font::BIG, true);
    Text textDifficulty(Point(360, 295), String::Difficulty((*it_current).GetDifficulty()), Font::BIG, true);
    
    TextBox textDescription(Rect(175, 322, 282, 90), (*it_current).GetDescription(), Font::BIG, true);
}
