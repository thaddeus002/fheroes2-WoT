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
#include <list>
#include <string>
#include "dir.h"
#include "agg.h"
#include "gamedefs.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "maps_fileinfo.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "kingdom.h"
#include "splitter.h"
#include "world.h"
#include "game.h"

namespace Scenario
{
    void DrawInfo(std::vector<Rect> & coordColors,  std::vector< std::pair<Rect, TextSprite> > & coordClass);
    void RedrawOpponentColors(const std::vector<Rect> & coordColors);
    u8   GetAllowChangeRaces(const Maps::FileInfo &maps);
}

Game::menu_t Game::ScenarioInfo(void)
{
    AGG::PlayMusic(MUS::MAINMENU);

    std::list<Maps::FileInfo *> info_maps;

    Settings & conf = Settings::Get();

    conf.SetPlayers(Color::BLUE);
    // read maps directory
    Dir dir;

    dir.Read(conf.MapsDirectory(), ".mp2", false);

    // loyality version
    if(conf.Modes(Settings::PRICELOYALTY)) dir.Read(conf.MapsDirectory(), ".mx2", false);

    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd)
    {
	Maps::FileInfo *mi = new Maps::FileInfo(*itd);
	const std::bitset<8> colors(mi->AllowColors());
	
	// multi map filter
	if(Game::MULTI & conf.GameType() && 2 > colors.count())
	{
	    delete mi;
	    continue;
	}

	info_maps.push_back(mi);
    }

    // empty maps
    if(info_maps.empty())
    {
	Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
	return MAINMENU;
    }

    // sort list
    info_maps.sort(Maps::FileInfo::PredicateForSorting);

    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::NGEXTRA);
    AGG::PreloadObject(ICN::NGHSBKG);

    menu_t result = QUITGAME;
    LocalEvent & le = LocalEvent::GetLocalEvent();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    // set first maps settings
    if(conf.FileInfo().FileMaps().empty()) conf.LoadFileMaps(info_maps.front()->FileMaps());

    Button buttonSelectMaps(513, 77, ICN::NGEXTRA, 64, 65);
    Button buttonOk(234, 413, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(491, 413, ICN::NGEXTRA, 68, 69);

    // vector coord colors opponent
    std::vector<Rect> coordColors(KINGDOMMAX);
    // vector coord class
    std::vector< std::pair<Rect, TextSprite> > coordClass(KINGDOMMAX);

    // first allow color
    conf.SetPlayers(0);
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(color & conf.FileInfo().AllowColors()){ conf.SetPlayers(color); conf.SetMyColor(color); break; }

    // set levelCursor (default normal)
    const Point pointDifficultyNormal(302, 124);
    Scenario::DrawInfo(coordColors, coordClass);
    Scenario::RedrawOpponentColors(coordColors);

    SpriteCursor levelCursor(AGG::GetICN(ICN::NGEXTRA, 62), pointDifficultyNormal);
    levelCursor.Show(pointDifficultyNormal);
    conf.SetGameDifficulty(Difficulty::NORMAL);

    const Rect rectDifficultyEs(225, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyNr(pointDifficultyNormal.x, pointDifficultyNormal.y, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyHd(378, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyEx(455, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyIm(532, 124, levelCursor.w(), levelCursor.h());

    u8 rnd_color = Scenario::GetAllowChangeRaces(conf.FileInfo());

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
	    const u8 players = conf.Players();
	    switch(conf.GameType())
	    {
		//case Game::NETWORK:
		case Game::HOTSEAT:
	    	    conf.SetPlayers(players & color ? players & ~color : players | color);
		    break;
		default:
		    conf.SetPlayers(0);
	    	    conf.SetPlayers(color);
		    conf.SetMyColor(color);
	    	    break;
	    }
	    Scenario::RedrawOpponentColors(coordColors);
	    cursor.Show();
	    display.Flip();
	}

	// select class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if((conf.FileInfo().KingdomColors() & rnd_color & color) &&
		le.MouseClickLeft(coordClass[Color::GetIndex(color)].first))
	    {
		cursor.Hide();
		u8 index = 0;
		const Rect & rt = coordClass[Color::GetIndex(color)].first;
		TextSprite & textsprite = coordClass[Color::GetIndex(color)].second;
		Race::race_t race = conf.FileInfo().KingdomRace(color);
		switch(race)
		{
		    case Race::KNGT: index = 52; race = Race::BARB; break;
		    case Race::BARB: index = 53; race = Race::SORC; break;
		    case Race::SORC: index = 54; race = Race::WRLK; break;
		    case Race::WRLK: index = 55; race = Race::WZRD; break;
		    case Race::WZRD: index = 56; race = Race::NECR; break;
		    case Race::NECR: index = 58; race = Race::RAND; break;
		    case Race::RAND: index = 51; race = Race::KNGT; break;
		    default: break;
		}
		conf.FileInfo().SetKingdomRace(color, race);
		display.Blit(AGG::GetICN(ICN::NGEXTRA, index), rt.x, rt.y);

		const std::string & name = (Race::NECR == race ? _("Necroman") : Race::String(race));
		textsprite.Hide();
		textsprite.SetText(name);
		textsprite.SetPos(rt.x + (rt.w - textsprite.w()) / 2, rt.y + rt.h + 2);
		textsprite.Show();

		cursor.Show();
		display.Flip();
	    }

	// press button
	le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.PressDraw() : buttonSelectMaps.ReleaseDraw();
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	// click select
	if(le.KeyPress(KEY_s) || le.MouseClickLeft(buttonSelectMaps))
	{
	    const Maps::FileInfo * finfo = Dialog::SelectFileInfo(info_maps);
	    if(NULL != finfo) conf.LoadFileMaps(finfo->FileMaps());

	    cursor.Hide();
	    levelCursor.Hide();
	    // set first allow color
	    conf.SetPlayers(0);
	    for(Color::color_t col = Color::BLUE; col < Color::GRAY; ++col)
		if(conf.FileInfo().AllowColors() & col){ conf.SetPlayers(col); conf.SetMyColor(col); break; }

	    Scenario::DrawInfo(coordColors, coordClass);
	    Scenario::RedrawOpponentColors(coordColors);
	    levelCursor.Move(pointDifficultyNormal);
	    levelCursor.Show();
	    cursor.Show();
	    display.Flip();
	    rnd_color = Scenario::GetAllowChangeRaces(conf.FileInfo());
	}

	// click cancel
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
	{
	    Settings::Get().SetGameType(Game::UNKNOWN);
	    result = MAINMENU;
	    break;
	}

	// click ok
	if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk))
	{
	    if(Settings::Get().Debug()) Error::Verbose("select maps: " + conf.FileInfo().FileMaps());
	    if(Settings::Get().Debug()) Error::Verbose("difficulty: " + Difficulty::String(conf.GameDifficulty()));
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
		if(conf.FileInfo().KingdomColors() & color)
		{
		    if(Race::RAND == conf.FileInfo().KingdomRace(color)) conf.FileInfo().SetKingdomRace(color, Race::Rand());
		    if(Settings::Get().Debug()) Error::Verbose(Color::String(color) + ": " + Race::String(conf.FileInfo().KingdomRace(color)));
		}
	    if(Settings::Get().Debug()) Error::Verbose("select color: " + Color::String(conf.MyColor()));

	    result = STARTGAME;
	    break;
	}
	
	// right info
	if(le.MousePressRight(buttonSelectMaps)) Dialog::Message(_("Scenario"), _("Click here to select which scenario to play."), Font::BIG);

	// difficulty
	if(le.MousePressRight(rectDifficultyEs) ||
	    le.MousePressRight(rectDifficultyNr) ||
	    le.MousePressRight(rectDifficultyHd) ||
	    le.MousePressRight(rectDifficultyEx) ||
	    le.MousePressRight(rectDifficultyIm))
		Dialog::Message(_("Game Difficulty"), _("This lets you change the starting difficulty at which you will play. Higher difficulty levels start you of with fewer resources, and at the higher settings, give extra resources to the computer."), Font::BIG);

	// color
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.FileInfo().KingdomColors() & color &&
		le.MousePressRight(coordColors[Color::GetIndex(color)]))
		    Dialog::Message(_("Opponents"), _("This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player."), Font::BIG);

	// class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.FileInfo().KingdomColors() & color &&
		le.MousePressRight(coordClass[Color::GetIndex(color)].first))
		    Dialog::Message(_("Class"), _("This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alingment."), Font::BIG);

	//if(le.MousePressRight(?)) Dialog::Message(_("Difficulty Rating"), _("The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score."), Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message(_("OK"), _("Click to accept these settings and start a new game."), Font::BIG);
	if(le.MousePressRight(buttonCancel)) Dialog::Message(_("Cancel"), _("Click to return to the main menu."), Font::BIG);
    }

    std::list<Maps::FileInfo *>::const_iterator it1 = info_maps.begin();
    std::list<Maps::FileInfo *>::const_iterator it2 = info_maps.end();
    for(; it1 != it2; ++it1) if(*it1) delete *it1;

    cursor.Hide();

    if(result == STARTGAME)
    {
	display.Fade();
	// Load maps
	world.LoadMaps(conf.FileInfo().FileMaps());
    }

    return result;
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
		(conf.Players() & color ?
		    *colorHumanSprite[color] : *colorAllowSprite[color]) : *colorOpponentSprite[color], rect.x, rect.y);
    }
}

void Scenario::DrawInfo(std::vector<Rect> & coordColors,  std::vector< std::pair<Rect, TextSprite> > & coordClass)
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
    Text text(_("Scenario:"), Font::BIG);
    text.Blit(414 - text.w()/2, 53);

    // maps name
    text.Set(conf.FileInfo().Name());
    text.Blit(374 - text.w()/2, 78);
    
    // text game difficulty
    text.Set(_("Game Difficulty:"));
    text.Blit(414 - text.w()/2, 107);

    //
    text.Set(_("Easy"), Font::SMALL);
    text.Blit(260 - text.w()/2, 196);
    text.Set(_("Normal"));
    text.Blit(336 - text.w()/2, 196);
    text.Set(_("Hard"));
    text.Blit(412 - text.w()/2, 196);
    text.Set(_("Expert"));
    text.Blit(490 - text.w()/2, 196);
    text.Set(_("Impossible"));
    text.Blit(566 - text.w()/2, 196);

    // text opponents
    text.Set(_("Opponents:"), Font::BIG);
    text.Blit(414 - text.w()/2, 213);

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
    text.Set(_("Class:"), Font::BIG);
    text.Blit(414 - text.w()/2, 290);

    // draw class
    current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
    {
	TextSprite & textsprite = coordClass[Color::GetIndex(color)].second;
	textsprite.Hide();
    }

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(conf.FileInfo().KingdomColors() & color)
    {
	    u8 index = 0;
	    Point pt;
	    const Race::race_t race = conf.FileInfo().KingdomRace(color);

	    switch(race)
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

	    coordClass[Color::GetIndex(color)].first = Rect(pt.x, pt.y, sprite.w(), sprite.h());

	    display.Blit(sprite, pt);

	    const Rect & rt = coordClass[Color::GetIndex(color)].first;
	    TextSprite & textsprite = coordClass[Color::GetIndex(color)].second;

	    const std::string & name = (Race::NECR == race ? _("Necroman") : Race::String(race));
	    textsprite.SetFont(Font::SMALL);
	    textsprite.SetText(name);
	    textsprite.SetPos(rt.x + (rt.w - textsprite.w()) / 2, rt.y + rt.h + 2);
	    textsprite.Show();

    	    ++current;
    }
}

u8 Scenario::GetAllowChangeRaces(const Maps::FileInfo &maps)
{
    u8 result = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(Race::RAND == maps.KingdomRace(color)) result |= color;

    return result;
}
