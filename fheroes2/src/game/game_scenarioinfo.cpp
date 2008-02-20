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

#include <bitset>
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

namespace Scenario
{
    void DrawInfo(std::vector<Rect> & coordColors,  std::vector<Rect> & coordClass);
    void RedrawOpponentColors(const std::vector<Rect> & coordColors);
    u8   GetAllowChangeRaces(const Maps::FileInfo &maps);
    void SelectMaps(const std::vector<Maps::FileInfo> &allmaps);
    void DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count = LISTMAXITEM);
    void DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current);
}

Game::menu_t Game::ScenarioInfo(void)
{
    std::vector<Maps::FileInfo> info_maps;

    Settings & conf = Settings::Get();
    // read maps directory
    Dir dir;

    dir.Read(conf.MapsDirectory(), "mp2");
    dir.Read(conf.MapsDirectory(), "Mp2");
    dir.Read(conf.MapsDirectory(), "MP2");

    // FIXME: loyality version
    dir.Read(conf.MapsDirectory(), "mx2");
    dir.Read(conf.MapsDirectory(), "Mx2");
    dir.Read(conf.MapsDirectory(), "MX2");

    const u16 count_mp2 = dir.size();
    
    // empty maps dir
    if(0 == count_mp2)
    {
	Dialog::Message("Warning", "No maps available!", Font::BIG, Dialog::OK);
	return MAINMENU;
    }

    info_maps.resize(count_mp2);

    for(u16 ii = 0; ii < count_mp2; ++ii)
	info_maps[ii].Read(dir[ii]);

    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::NGEXTRA);
    AGG::PreloadObject(ICN::NGHSBKG);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(Display::SMALL);

    // vector coord colors opponent
    std::vector<Rect> coordColors(KINGDOMMAX);

    // vector coord class
    std::vector<Rect> coordClass(KINGDOMMAX);

    // set first maps settings
    conf.LoadFileMaps(info_maps.front().FileMaps());

    Scenario::DrawInfo(coordColors, coordClass);
    Scenario::RedrawOpponentColors(coordColors);

    Button buttonSelectMaps(513, 77, ICN::NGEXTRA, 64, 65);
    Button buttonOk(234, 413, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(491, 413, ICN::NGEXTRA, 68, 69);

    // set levelCursor (default normal)
    const Point pointDifficultyNormal(302, 124);
    SpriteCursor levelCursor(AGG::GetICN(ICN::NGEXTRA, 62), pointDifficultyNormal);
    levelCursor.Show(pointDifficultyNormal);
    conf.SetGameDifficulty(Difficulty::NORMAL);

    u8 rnd_color = Scenario::GetAllowChangeRaces(conf.FileInfo());

    const Rect rectDifficultyEs(225, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyNr(pointDifficultyNormal.x, pointDifficultyNormal.y, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyHd(378, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyEx(455, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyIm(532, 124, levelCursor.w(), levelCursor.h());

    buttonSelectMaps.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    // newstandard loop
    while(le.HandleEvents())
    {
	// select level easy
	if(le.MouseClickLeft(rectDifficultyEs) ||
	    le.MouseClickLeft(rectDifficultyNr) ||
	    le.MouseClickLeft(rectDifficultyHd) ||
	    le.MouseClickLeft(rectDifficultyEx) ||
	    le.MouseClickLeft(rectDifficultyIm))
	{
	    cursor.Hide();

	    if(le.MouseCursor(rectDifficultyEs))
	    {
		levelCursor.Move(rectDifficultyEs.x, rectDifficultyEs.y);
		conf.SetGameDifficulty(Difficulty::EASY);
	    }
	    else
	    if(le.MouseCursor(rectDifficultyNr))
	    {
		levelCursor.Move(rectDifficultyNr.x, rectDifficultyNr.y);
		conf.SetGameDifficulty(Difficulty::NORMAL);
	    }
	    else
	    if(le.MouseCursor(rectDifficultyHd))
	    {
		levelCursor.Move(rectDifficultyHd.x, rectDifficultyHd.y);
		conf.SetGameDifficulty(Difficulty::HARD);
	    }
	    else
	    if(le.MouseCursor(rectDifficultyEx))
	    {
		levelCursor.Move(rectDifficultyEx.x, rectDifficultyEx.y);
		conf.SetGameDifficulty(Difficulty::EXPERT);
	    }
	    else
	    if(le.MouseCursor(rectDifficultyIm))
	    {
		levelCursor.Move(rectDifficultyIm.x, rectDifficultyIm.y);
		conf.SetGameDifficulty(Difficulty::IMPOSSIBLE);
	    }
	    if(le.MouseCursor(rectDifficultyEs))
	    {
		levelCursor.Move(rectDifficultyEs.x, rectDifficultyEs.y);
		conf.SetGameDifficulty(Difficulty::EASY);
	    }

	    cursor.Show();
	    display.Flip();
	}

	// select color
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if((conf.FileInfo().KingdomColors() & conf.FileInfo().AllowColors() & color) &&
		le.MouseClickLeft(coordColors[Color::GetIndex(color)]))
	{
	    cursor.Hide();
	    conf.SetMyColor(color);
	    Scenario::RedrawOpponentColors(coordColors);
	    cursor.Show();
	    display.Flip();
	}

	// select class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if((conf.FileInfo().KingdomColors() & rnd_color & color) &&
		le.MouseClickLeft(coordClass[Color::GetIndex(color)]))
	    {
		cursor.Hide();
		u8 index = 0;
		switch(conf.FileInfo().KingdomRace(color))
		{
		    case Race::KNGT: index = 52; conf.FileInfo().SetKingdomRace(color, Race::BARB); break;
		    case Race::BARB: index = 53; conf.FileInfo().SetKingdomRace(color, Race::SORC); break;
		    case Race::SORC: index = 54; conf.FileInfo().SetKingdomRace(color, Race::WRLK); break;
		    case Race::WRLK: index = 55; conf.FileInfo().SetKingdomRace(color, Race::WZRD); break;
		    case Race::WZRD: index = 56; conf.FileInfo().SetKingdomRace(color, Race::NECR); break;
		    case Race::NECR: index = 58; conf.FileInfo().SetKingdomRace(color, Race::RAND); break;
		    case Race::RAND: index = 51; conf.FileInfo().SetKingdomRace(color, Race::KNGT); break;
		    default: break;
		}
    		const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, index);
		display.Blit(sprite, coordClass[Color::GetIndex(color)].x, coordClass[Color::GetIndex(color)].y);
		cursor.Show();
		display.Flip();
	    }

	// press button
	le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.PressDraw() : buttonSelectMaps.ReleaseDraw();
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	// click select
	if(le.MouseClickLeft(buttonSelectMaps))
	{
	    cursor.Hide();
	    levelCursor.Hide();
	    Scenario::SelectMaps(info_maps);
	    Scenario::DrawInfo(coordColors, coordClass);
	    Scenario::RedrawOpponentColors(coordColors);
	    levelCursor.Move(pointDifficultyNormal);
	    levelCursor.Show();
	    cursor.Show();
	    display.Flip();
	    rnd_color = Scenario::GetAllowChangeRaces(conf.FileInfo());
	}

	// click cancel
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(SDLK_ESCAPE)) return MAINMENU;

	// click ok
	if(le.MouseClickLeft(buttonOk))
	{
	    if(H2Config::Debug()) Error::Verbose("select maps: " + conf.FileInfo().FileMaps());
	    if(H2Config::Debug()) Error::Verbose("difficulty: " + Difficulty::String(conf.GameDifficulty()));
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
		if(conf.FileInfo().KingdomColors() & color)
		{
		    if(Race::RAND == conf.FileInfo().KingdomRace(color)) conf.FileInfo().SetKingdomRace(color, Race::Rand());
		    if(H2Config::Debug()) Error::Verbose(Color::String(color) + ": " + Race::String(conf.FileInfo().KingdomRace(color)));
		}
	    if(H2Config::Debug()) Error::Verbose("select color: " + Color::String(conf.MyColor()));

	    return STARTGAME;
	}
	
	// right info
	if(le.MousePressRight(buttonSelectMaps)) Dialog::Message("Scenario", "Click here to select which scenario to play.", Font::BIG);

	// difficulty
	if(le.MousePressRight(rectDifficultyEs) ||
	    le.MousePressRight(rectDifficultyNr) ||
	    le.MousePressRight(rectDifficultyHd) ||
	    le.MousePressRight(rectDifficultyEx) ||
	    le.MousePressRight(rectDifficultyIm))
		Dialog::Message("Game Difficulty", "This lets you change the starting difficulty at which you will play. Higher difficulty levels start you of with fewer resources, and at the higher settings, give extra resources to the computer.", Font::BIG);

	// color
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.FileInfo().KingdomColors() & color &&
		le.MousePressRight(coordColors[Color::GetIndex(color)]))
		    Dialog::Message("Opponents", "This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player.", Font::BIG);

	// class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.FileInfo().KingdomColors() & color &&
		le.MousePressRight(coordClass[Color::GetIndex(color)]))
		    Dialog::Message("Class", "This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alingment.", Font::BIG);

	//if(le.MousePressRight(?)) Dialog::Message("Difficulty Rating", "The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score.", Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message("OK", "Click to accept these settings and start a new game.", Font::BIG);
	if(le.MousePressRight(buttonCancel)) Dialog::Message("Cancel", "Click to return to the main menu.", Font::BIG);
    }

    return QUITGAME;
}

void Scenario::RedrawOpponentColors(const std::vector<Rect> & coordColors)
{
    std::map<Color::color_t, const Sprite *> colorHumanSprite;
    colorHumanSprite[Color::BLUE] = &AGG::GetICN(ICN::NGEXTRA, 9);
    colorHumanSprite[Color::GREEN] = &AGG::GetICN(ICN::NGEXTRA, 10);
    colorHumanSprite[Color::RED] = &AGG::GetICN(ICN::NGEXTRA, 11);
    colorHumanSprite[Color::YELLOW] = &AGG::GetICN(ICN::NGEXTRA, 12);
    colorHumanSprite[Color::ORANGE] = &AGG::GetICN(ICN::NGEXTRA, 13);
    colorHumanSprite[Color::PURPLE] = &AGG::GetICN(ICN::NGEXTRA, 14);

    std::map<Color::color_t, const Sprite *> colorAllowSprite;
    colorAllowSprite[Color::BLUE] = &AGG::GetICN(ICN::NGEXTRA, 3);
    colorAllowSprite[Color::GREEN] = &AGG::GetICN(ICN::NGEXTRA, 4);
    colorAllowSprite[Color::RED] = &AGG::GetICN(ICN::NGEXTRA, 5);
    colorAllowSprite[Color::YELLOW] = &AGG::GetICN(ICN::NGEXTRA, 6);
    colorAllowSprite[Color::ORANGE] = &AGG::GetICN(ICN::NGEXTRA, 7);
    colorAllowSprite[Color::PURPLE] = &AGG::GetICN(ICN::NGEXTRA, 8);

    std::map<Color::color_t, const Sprite *> colorOpponentSprite;
    colorOpponentSprite[Color::BLUE] = &AGG::GetICN(ICN::NGEXTRA, 15);
    colorOpponentSprite[Color::GREEN] = &AGG::GetICN(ICN::NGEXTRA, 16);
    colorOpponentSprite[Color::RED] = &AGG::GetICN(ICN::NGEXTRA, 17);
    colorOpponentSprite[Color::YELLOW] = &AGG::GetICN(ICN::NGEXTRA, 18);
    colorOpponentSprite[Color::ORANGE] = &AGG::GetICN(ICN::NGEXTRA, 19);
    colorOpponentSprite[Color::PURPLE] = &AGG::GetICN(ICN::NGEXTRA, 20);

    const Settings & conf = Settings::Get();

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
    {
	const Rect & rect = coordColors[Color::GetIndex(color)];

	if(conf.FileInfo().KingdomColors() & color)
	    Display::Get().Blit(conf.FileInfo().AllowColors() & color ?
		(conf.MyColor() & color ?
		    *colorHumanSprite[color] : *colorAllowSprite[color]) : *colorOpponentSprite[color], rect.x, rect.y);
    }
}

void Scenario::DrawInfo(std::vector<Rect> & coordColors, std::vector<Rect> & coordClass)
{
    Display & display = Display::Get();

    const Settings & conf = Settings::Get();

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);

    // image panel
    const Sprite &shadow = AGG::GetICN(ICN::NGHSBKG, 1);
    display.Blit(shadow, 196, 40);
    const Sprite &panel = AGG::GetICN(ICN::NGHSBKG, 0);
    display.Blit(panel, 204, 33);

    // text scenario
    Text("Scenario:", Font::BIG, 376, 53);

    // maps name
    Text(conf.FileInfo().Name(), Font::BIG, 260, 78);
    
    // text game difficulty
    Text("Game Difficulty:", Font::BIG, 330, 107);

    //
    Text("Easy", Font::SMALL, 248, 196);
    Text("Normal", Font::SMALL, 316, 196);
    Text("Hard", Font::SMALL, 395, 196);
    Text("Expert", Font::SMALL, 472, 196);
    Text("Impossible", Font::SMALL, 536, 196);

    // text opponents
    Text("Opponents:", Font::BIG, 368, 210);

    // draw opponents
    const std::bitset<8> colors(conf.FileInfo().KingdomColors());
    const u8 count = colors.count();
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
        if(conf.FileInfo().KingdomColors() & color)
        {
            const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, 3);

	    coordColors[Color::GetIndex(color)] = Rect(228 + current * sprite.w() * 6 / count + (sprite.w() * (6 - count) / (2 * count)), 234, sprite.w(), sprite.h());

            ++current;
	}

    // text class
    Text("Class:", Font::BIG, 386, 290);

    // draw class
    current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(conf.FileInfo().KingdomColors() & color)
	{
	    u8 index = 0;
	    Point pt;

	    switch(conf.FileInfo().KingdomRace(color))
	    {
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

    	    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, index);

    	    pt.x = 228 + current * sprite.w() * 6 / count + (sprite.w() * (6 - count) / (2 * count));
    	    pt.y = 314;

	    coordClass[Color::GetIndex(color)] = Rect(pt.x, pt.y, sprite.w(), sprite.h());

	    display.Blit(sprite, pt);

    	    ++current;
    }
}

void Scenario::SelectMaps(const std::vector<Maps::FileInfo> &allmaps)
{
    // preload
    AGG::PreloadObject(ICN::REQSBKG);

    LocalEvent & le = LocalEvent::GetLocalEvent();

    std::vector<Maps::FileInfo> smallmaps, mediummaps, largemaps, xlargemaps;

    std::vector<Maps::FileInfo>::const_iterator it = allmaps.begin();
    std::vector<Maps::FileInfo>::const_iterator it_end = allmaps.end();

    while(it != it_end)
    {
	switch((*it).SizeMaps())
	{
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
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(Display::SMALL);

    Background background(Rect(110, 0, 510, 480));
    background.Save();

    // image panel
    const Sprite &shadow = AGG::GetICN(ICN::REQSBKG, 1);
    display.Blit(shadow, 114, 21);
    const Sprite &panel = AGG::GetICN(ICN::REQSBKG, 0);
    display.Blit(panel, 130, 5);

    Background backgroundList(Rect(170, 60, 270, 175));
    backgroundList.Save();
    Background backgroundInfo(Rect(165, 265, 305, 145));
    backgroundInfo.Save();

    // area list
    Rect rectAreaList(Rect(170, 60, 270, 175));
    
    // Splitter
    Splitter split(AGG::GetICN(ICN::ESCROLL, 3), Rect(460, 78, 8, 141), Splitter::VERTICAL);
    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));

    // button
    Button buttonOk(270, 415, ICN::REQUESTS, 1, 2);
    Button buttonPgUp(457, 60, ICN::REQUESTS, 5, 6);
    Button buttonPgDn(457, 222, ICN::REQUESTS, 7, 8);

    Button buttonSelectSmall(167, 28, ICN::REQUESTS, 9, 10);
    Button buttonSelectMedium(229, 28, ICN::REQUESTS, 11, 12);
    Button buttonSelectLarge(291, 28, ICN::REQUESTS, 13, 14);
    Button buttonSelectXLarge(353, 28, ICN::REQUESTS, 15, 16);
    Button buttonSelectAll(415, 28, ICN::REQUESTS, 17, 18);

    buttonOk.Draw();
    buttonPgUp.Draw();
    buttonPgDn.Draw();
    buttonSelectSmall.Draw();
    buttonSelectMedium.Draw();
    buttonSelectLarge.Draw();
    buttonSelectXLarge.Draw();
    buttonSelectAll.Draw();

    std::vector<Maps::FileInfo>::const_iterator it_list_head = curmaps->begin();
    std::vector<Maps::FileInfo>::const_iterator it_current = it_list_head;

    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
    Scenario::DrawSelectInfo(it_current);

    cursor.Show();
    display.Flip();

    u16 num_select = 0xFFFF;

    // newstandard loop
    while(le.HandleEvents())
    {
	// press button
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonPgUp) ? buttonPgUp.PressDraw() : buttonPgUp.ReleaseDraw();
	le.MousePressLeft(buttonPgDn) ? buttonPgDn.PressDraw() : buttonPgDn.ReleaseDraw();

	le.MousePressLeft(buttonSelectSmall) ? buttonSelectSmall.PressDraw() : buttonSelectSmall.ReleaseDraw();
	le.MousePressLeft(buttonSelectMedium) ? buttonSelectMedium.PressDraw() : buttonSelectMedium.ReleaseDraw();
	le.MousePressLeft(buttonSelectLarge) ? buttonSelectLarge.PressDraw() : buttonSelectLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectXLarge) ? buttonSelectXLarge.PressDraw() : buttonSelectXLarge.ReleaseDraw();
	le.MousePressLeft(buttonSelectAll) ? buttonSelectAll.PressDraw() : buttonSelectAll.ReleaseDraw();

	// click small
	if(le.MouseClickLeft(buttonSelectSmall) && smallmaps.size()){
	    curmaps = &smallmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click medium
	if(le.MouseClickLeft(buttonSelectMedium) && mediummaps.size()){
	    curmaps = &mediummaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click large
	if(le.MouseClickLeft(buttonSelectLarge) && largemaps.size()){
	    curmaps = &largemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click xlarge
	if(le.MouseClickLeft(buttonSelectXLarge) && xlargemaps.size()){
	    curmaps = &xlargemaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click all
	if(le.MouseClickLeft(buttonSelectAll)){
	    curmaps = &allmaps;
	    it_list_head = curmaps->begin();
	    it_current = it_list_head;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, (LISTMAXITEM > curmaps->size() ? curmaps->size() : LISTMAXITEM));
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.SetRange(0, (LISTMAXITEM < curmaps->size() ? curmaps->size() - LISTMAXITEM : 0));
	    cursor.Show();
	    display.Flip();
	}

	// click list
	if(le.MouseClickLeft(rectAreaList)){
	    u16 num = (le.MouseReleaseLeft().y - rectAreaList.y) / LISTHEIGHTROW;
	    if(num >= curmaps->size()) num = curmaps->size() - 1;
	    if(num_select == num) break;
	    num_select = num;
	    cursor.Hide();
	    backgroundInfo.Restore();
	    it_current = it_list_head + num;
	    Scenario::DrawSelectInfo(it_current);
	    split.Move(it_list_head - curmaps->begin());
	    cursor.Show();
	    display.Flip();
	    num_select = num;
	}

	// click up
	if((le.MouseWheelUp(rectAreaList) || le.MouseWheelUp(split.GetRect()) || le.MouseClickLeft(buttonPgUp)) &&
	    curmaps->size() && it_list_head > curmaps->begin()){
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(--it_list_head);
	    split.Backward();
	    cursor.Show();
	    display.Flip();
	}

	// click down
	if((le.MouseWheelDn(rectAreaList) || le.MouseWheelDn(split.GetRect()) || le.MouseClickLeft(buttonPgDn)) &&
	    LISTMAXITEM < curmaps->size() && it_list_head + LISTMAXITEM < curmaps->end()){
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(++it_list_head);
	    split.Forward();
	    cursor.Show();
	    display.Flip();
	}

	// move cursor splitter
	if(le.MousePressLeft(split.GetRect()) && curmaps->size() > LISTMAXITEM && le.MouseCursor(split.GetRect()))
	{
	    u16 seek = (le.MouseCursor().y - split.GetRect().y) * 100 / split.GetStep();
	    if(seek > curmaps->size() - LISTMAXITEM) seek = curmaps->size() - LISTMAXITEM;

	    it_list_head = curmaps->begin() + seek;
	    cursor.Hide();
	    backgroundList.Restore();
	    Scenario::DrawList(it_list_head, LISTMAXITEM);
	    backgroundInfo.Restore();
	    Scenario::DrawSelectInfo(it_current);
	    split.Move(seek);
	    cursor.Show();
	    display.Flip();
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
	if(le.MouseClickLeft(buttonOk) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)) break;
    }

    // set current settings
    Settings::Get().LoadFileMaps((*it_current).FileMaps());

    cursor.Hide();
    background.Restore();
}

u8 Scenario::GetAllowChangeRaces(const Maps::FileInfo &maps)
{
    u8 result = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(Race::RAND == maps.KingdomRace(color)) result |= color;

    return result;
}


void Scenario::DrawList(std::vector<Maps::FileInfo>::const_iterator &it_top, u8 count)
{
    Display & display = Display::Get();

    u16 y = 64;
    u16 x = 176;
    u8 index = 0;
    std::vector<Maps::FileInfo>::const_iterator it_head = it_top;

    for(int ii = 0; ii < count; ++ii)
    {
	// sprite count
	const std::bitset<8> colors((*it_head).KingdomColors());
	index = 19 + colors.count();
	const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteCount, x, y);

        // sprite size
	switch((*it_head).SizeMaps())
	{
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
	const Sprite &spriteSize = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteSize, x + spriteCount.w() + 2, y);

	// text longname
	Text((*it_head).Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 18, y);

	// sprite wins
	index = 30 + (*it_head).ConditionsWins();
	const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteWins, x + 224, y);

	// sprite loss
	index = 36 + (*it_head).ConditionsLoss();
	const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
	display.Blit(spriteLoss, x + 226 + spriteWins.w(), y);

	y += LISTHEIGHTROW;
	++it_head;
    }
}

void Scenario::DrawSelectInfo(std::vector<Maps::FileInfo>::const_iterator &it_current)
{
    Display & display = Display::Get();

    u16 x = 196;
    u16 y = 269;
    u8 index = 0;

    // sprite count
    const std::bitset<8> colors((*it_current).KingdomColors());
    index = 19 + colors.count();
    const Sprite &spriteCount = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteCount, x, y);

    // sprite size
    switch((*it_current).SizeMaps())
    {
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
    const Sprite &spriteSize = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteSize, x + spriteCount.w() + 2, y);

    // text longname
    Text((*it_current).Name(), Font::BIG, x + spriteCount.w() + spriteSize.w() + 22, y);

    // sprite wins
    index = 30 + (*it_current).ConditionsWins();
    const Sprite &spriteWins = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteWins, x + 209, y);

    // sprite loss
    index = 36 + (*it_current).ConditionsLoss();
    const Sprite &spriteLoss = AGG::GetICN(ICN::REQUESTS, index);
    display.Blit(spriteLoss, x + 211 + spriteWins.w(), y);

    Text("Maps difficulty:", Font::BIG, 200, 295);
    Text(Difficulty::String((*it_current).Difficulty()), Font::BIG, 360, 295);
    
    TextBox((*it_current).Description(), Font::BIG, Rect(175, 322, 282, 90));
}
