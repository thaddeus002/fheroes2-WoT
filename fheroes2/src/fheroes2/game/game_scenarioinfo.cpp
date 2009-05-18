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

#include <algorithm>
#include <vector>
#include <string>
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

extern bool DialogSelectMapsFileList(MapsFileInfoList &, std::string &);
extern bool PrepareMapsFileInfoList(MapsFileInfoList &);

u16 GetStepFor(u16, u16, u16);
void RedrawStaticInfo(void);
void RedrawRatingInfo(TextSprite &);
void RedrawOpponentsInfo(const Point &);
void RedrawClassInfo(const Point &);
void UpdateCoordOpponentsInfo(const Point &, std::vector<Rect> &);
void UpdateCoordClassInfo(const Point &, std::vector<Rect> &);

Game::menu_t Game::ScenarioInfo(void)
{
    AGG::PlayMusic(MUS::MAINMENU);

    Settings & conf = Settings::Get();

    MapsFileInfoList lists;
    if(!PrepareMapsFileInfoList(lists)) return MAINMENU;

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
    conf.LoadFileMaps(lists.front().file);

    Button buttonSelectMaps(513, 77, ICN::NGEXTRA, 64, 65);
    Button buttonOk(234, 413, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(491, 413, ICN::NGEXTRA, 68, 69);

    const Point pointDifficultyNormal(302, 124);
    const Point pointOpponentInfo(228, 234);
    const Point pointClassInfo(228, 314);

    // vector coord colors opponent
    std::vector<Rect> coordColors(KINGDOMMAX);

    // vector coord class
    std::vector<Rect> coordClass(KINGDOMMAX);

    // first allow color
    conf.SetMyColor(conf.FirstAllowColor());
    conf.SetPlayersColors(conf.MyColor());

    RedrawStaticInfo();

    UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
    RedrawOpponentsInfo(pointOpponentInfo);

    UpdateCoordClassInfo(pointClassInfo, coordClass);
    RedrawClassInfo(pointClassInfo);

    SpriteCursor levelCursor(AGG::GetICN(ICN::NGEXTRA, 62), pointDifficultyNormal);
    levelCursor.Show(pointDifficultyNormal);
    conf.SetGameDifficulty(Difficulty::NORMAL);

    const Rect rectDifficultyEs(225, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyNr(pointDifficultyNormal.x, pointDifficultyNormal.y, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyHd(378, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyEx(455, 124, levelCursor.w(), levelCursor.h());
    const Rect rectDifficultyIm(532, 124, levelCursor.w(), levelCursor.h());

    TextSprite rating;
    rating.SetFont(Font::BIG);
    rating.SetPos(370, 415);
    RedrawRatingInfo(rating);

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

	    RedrawRatingInfo(rating);
	    cursor.Show();
	    display.Flip();
	}

	// select color
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if((conf.KingdomColors(color) && conf.AllowColors(color)) &&
		le.MouseClickLeft(coordColors[Color::GetIndex(color)]))
	{
	    cursor.Hide();
	    switch(conf.GameType())
	    {
		//case Game::NETWORK:
		case Game::HOTSEAT:
		    conf.SetPlayersColors(conf.PlayersColors() & color ? conf.PlayersColors() & ~color : conf.PlayersColors() | color);
		    break;
		default:
		    conf.SetMyColor(color);
		    conf.SetPlayersColors(conf.MyColor());
	    	    break;
	    }
	    RedrawOpponentsInfo(pointOpponentInfo);
	    cursor.Show();
	    display.Flip();
	}

	// select class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.AllowChangeRace(color) &&
		le.MouseClickLeft(coordClass[Color::GetIndex(color)]))
	    {
		cursor.Hide();
		u8 index = 0;
		Race::race_t race = conf.KingdomRace(color);
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
		conf.SetKingdomRace(color, race);

		RedrawStaticInfo();
		levelCursor.Redraw();
		RedrawOpponentsInfo(pointOpponentInfo);
		RedrawClassInfo(pointClassInfo);
		RedrawRatingInfo(rating);

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
	    std::string filemaps;
	    if(DialogSelectMapsFileList(lists, filemaps) && filemaps.size()) conf.LoadFileMaps(filemaps);

	    cursor.Hide();
	    levelCursor.Hide();
	    // set first allow color
	    conf.SetMyColor(conf.FirstAllowColor());
	    conf.SetPlayersColors(conf.MyColor());

	    RedrawStaticInfo();
	    UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
	    RedrawOpponentsInfo(pointOpponentInfo);
	    UpdateCoordClassInfo(pointClassInfo, coordClass);
	    RedrawClassInfo(pointClassInfo);
	    RedrawRatingInfo(rating);
	    levelCursor.Move(pointDifficultyNormal);
	    levelCursor.Show();
	    cursor.Show();
	    display.Flip();
	}
	else
	// click cancel
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
	{
	    Settings::Get().SetGameType(Game::UNKNOWN);
	    result = MAINMENU;
	    break;
	}
	else
	// click ok
	if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk))
	{
	    if(Settings::Get().Debug()) Error::Verbose("select maps: " + conf.MapsFile());
	    if(Settings::Get().Debug()) Error::Verbose("difficulty: " + Difficulty::String(conf.GameDifficulty()));
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
		if(conf.KingdomColors(color))
		{
		    if(Race::RAND == conf.KingdomRace(color)) conf.SetKingdomRace(color, Race::Rand());
		    if(Settings::Get().Debug()) Error::Verbose(Color::String(color) + ": " + Race::String(conf.KingdomRace(color)));
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
	    if(conf.KingdomColors(color) &&
		le.MousePressRight(coordColors[Color::GetIndex(color)]))
		    Dialog::Message(_("Opponents"), _("This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player."), Font::BIG);

	// class
	for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	    if(conf.KingdomColors(color) &&
		le.MousePressRight(coordClass[Color::GetIndex(color)]))
		    Dialog::Message(_("Class"), _("This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alignment."), Font::BIG);

	//if(le.MousePressRight(?)) Dialog::Message(_("Difficulty Rating"), _("The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score."), Font::BIG);
	if(le.MousePressRight(buttonOk)) Dialog::Message(_("OK"), _("Click to accept these settings and start a new game."), Font::BIG);
	if(le.MousePressRight(buttonCancel)) Dialog::Message(_("Cancel"), _("Click to return to the main menu."), Font::BIG);
    }

    cursor.Hide();

    if(result == STARTGAME)
    {
	display.Fade();
	// Load maps
	world.LoadMaps(conf.MapsFile());
    }

    return result;
}

u16 GetStepFor(u16 current, u16 width, u16 count)
{
    return current * width * 6 / count + (width * (6 - count) / (2 * count));
}

void UpdateCoordClassInfo(const Point & dst, std::vector<Rect> & rects)
{
    UpdateCoordOpponentsInfo(dst, rects);
}

void UpdateCoordOpponentsInfo(const Point & dst, std::vector<Rect> & rects)
{
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, 3);
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(conf.KingdomColors(color))
    	{
	    rects[Color::GetIndex(color)] = Rect(dst.x + GetStepFor(current, sprite.w(), count), dst.y, sprite.w(), sprite.h());
    	    ++current;
	}
}

void RedrawStaticInfo(void)
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
    text.Set(conf.MapsName());
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

    // text class
    text.Set(_("Class:"), Font::BIG);
    text.Blit(414 - text.w()/2, 290);
}


void RedrawOpponentsInfo(const Point & dst)
{
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
    {
	if(conf.KingdomColors(color))
	{
	    u8 index = 0;

	    if(!(conf.AllowColors(color)))
	    {
		switch(color)
		{
		    case Color::BLUE:	index = 15; break;
		    case Color::GREEN:	index = 16; break;
		    case Color::RED:	index = 17; break;
		    case Color::YELLOW:	index = 18; break;
		    case Color::ORANGE:	index = 19; break;
		    case Color::PURPLE:	index = 20; break;
		    default: break;
		}
	    }
	    else
	    if(conf.PlayersColors() & color)
	    {
		switch(color)
		{
		    case Color::BLUE:	index =  9; break;
		    case Color::GREEN:	index = 10; break;
		    case Color::RED:	index = 11; break;
		    case Color::YELLOW:	index = 12; break;
		    case Color::ORANGE:	index = 13; break;
		    case Color::PURPLE:	index = 14; break;
		    default: break;
		}
	    }
	    else
	    {
		switch(color)
		{
		    case Color::BLUE:	index = 3; break;
		    case Color::GREEN:	index = 4; break;
		    case Color::RED:	index = 5; break;
		    case Color::YELLOW:	index = 6; break;
		    case Color::ORANGE:	index = 7; break;
		    case Color::PURPLE:	index = 8; break;
		    default: break;
		}
	    }

	    if(index)
	    {
		const Sprite & sprite = AGG::GetICN(ICN::NGEXTRA, index);
		Display::Get().Blit(sprite, dst.x + GetStepFor(current, sprite.w(), count), dst.y);
		++current;
	    }
	}
    }
}

void RedrawClassInfo(const Point & dst)
{
    Display & display = Display::Get();
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(conf.KingdomColors(color))
    {
	    u8 index = 0;
	    const Race::race_t race = conf.KingdomRace(color);
	    switch(race)
	    {
		case Race::KNGT: index = conf.AllowChangeRace(color) ? 51 : 70; break;
	        case Race::BARB: index = conf.AllowChangeRace(color) ? 52 : 71; break;
	        case Race::SORC: index = conf.AllowChangeRace(color) ? 53 : 72; break;
		case Race::WRLK: index = conf.AllowChangeRace(color) ? 54 : 73; break;
	        case Race::WZRD: index = conf.AllowChangeRace(color) ? 55 : 74; break;
		case Race::NECR: index = conf.AllowChangeRace(color) ? 56 : 75; break;
	        case Race::MULT: index = 76; break;
		case Race::RAND: index = 58; break;
		default: continue;
	    }

    	    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, index);
	    display.Blit(sprite, dst.x + GetStepFor(current, sprite.w(), count), dst.y);

	    const std::string & name = (Race::NECR == race ? _("Necroman") : Race::String(race));
	    Text label(name, Font::SMALL);
	    label.Blit(dst.x + GetStepFor(current, sprite.w(), count) + (sprite.w() - label.w()) / 2, dst.y + sprite.h() + 2);

    	    ++current;
    }
}

void RedrawRatingInfo(TextSprite & sprite)
{
    sprite.Hide();
    std::string str(_("Rating %{rating}%"));
    String::Replace(str, "%{rating}", Game::GetRating());
    sprite.SetText(str);
    sprite.Show();
}
