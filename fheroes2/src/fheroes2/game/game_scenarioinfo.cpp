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

#include <algorithm>
#include <vector>
#include <string>
#include "agg.h"
#include "gamedefs.h"
#include "button.h"
#include "cursor.h"
#include "race.h"
#include "settings.h"
#include "maps_fileinfo.h"
#include "dialog_selectscenario.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "kingdom.h"
#include "splitter.h"
#include "world.h"
#include "player.h"
#include "pocketpc.h"
#include "game.h"

u16 GetStepFor(u16, u16, u16);
void RedrawRatingInfo(TextSprite &);
void UpdateCoordOpponentsInfo(const Point &, std::vector<Rect> &);
void UpdateCoordClassInfo(const Point &, std::vector<Rect> &);

Game::menu_t Game::SelectScenario(void)
{
    if(Settings::Get().QVGA()) return PocketPC::SelectScenario();
    return SCENARIOINFO;
}

Game::menu_t Game::ScenarioInfo(void)
{
    Settings & conf = Settings::Get();

    AGG::PlayMusic(MUS::MAINMENU);

    MapsFileInfoList lists;
    if(!PrepareMapsFileInfoList(lists, (conf.GameType(Game::TYPE_MULTI))))
    {
	Dialog::Message(_("Warning"), _("No maps available!"), Font::BIG, Dialog::OK);
        return MAINMENU;
    }

    menu_t result = QUITGAME;
    LocalEvent & le = LocalEvent::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();

    Point top, pointDifficultyInfo, pointOpponentInfo, pointClassInfo;
    Rect rectPanel;
    Button* buttonSelectMaps = NULL;
    Button* buttonOk = NULL;
    Button* buttonCancel = NULL;

    std::vector<Rect>::iterator itr;
    // vector coord difficulty
    std::vector<Rect> coordDifficulty(5);
    // vector coord colors opponent
    std::vector<Rect> coordColors(KINGDOMMAX);
    // vector coord class
    std::vector<Rect> coordClass(KINGDOMMAX);

    const Sprite & ngextra = AGG::GetICN(ICN::NGEXTRA, 62);
    Dialog::FrameBorder* frameborder = NULL;

    // image background
    if(conf.QVGA())
    {
	const u16 window_w = 380;
	const u16 window_h = 224;

	frameborder = new Dialog::FrameBorder();
	frameborder->SetPosition((display.w() - window_w) / 2 - BORDERWIDTH, (display.h() - window_h) / 2 - BORDERWIDTH, window_w, window_h);
	frameborder->Redraw();

	rectPanel = frameborder->GetArea();
	pointDifficultyInfo = Point(rectPanel.x + 4, rectPanel.y + 24);
	pointOpponentInfo = Point(rectPanel.x + 4, rectPanel.y + 94);
	pointClassInfo = Point(rectPanel.x + 4, rectPanel.y + 144);

	coordDifficulty[0] = Rect(rectPanel.x + 1, rectPanel.y + 21,   ngextra.w(), ngextra.h());
	coordDifficulty[1] = Rect(rectPanel.x + 78, rectPanel.y + 21,  ngextra.w(), ngextra.h());
	coordDifficulty[2] = Rect(rectPanel.x + 154, rectPanel.y + 21, ngextra.w(), ngextra.h());
	coordDifficulty[3] = Rect(rectPanel.x + 231, rectPanel.y + 21, ngextra.w(), ngextra.h());
        coordDifficulty[4] = Rect(rectPanel.x + 308, rectPanel.y + 21, ngextra.w(), ngextra.h());

	buttonOk = new Button(rectPanel.x + rectPanel.w / 2 - 160, rectPanel.y + rectPanel.h - 30, ICN::NGEXTRA, 66, 67);
	buttonCancel = new Button(rectPanel.x + rectPanel.w / 2 + 60, rectPanel.y + rectPanel.h - 30, ICN::NGEXTRA, 68, 69);

	const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
	display.Blit(background, Rect(0, 0, window_w, window_h), rectPanel);

	Text text;
	text.Set(conf.CurrentFileInfo().name, Font::BIG);
	text.Blit(rectPanel.x + (rectPanel.w - text.w()) / 2, rectPanel.y + 5);
    }
    else
    {
	const Sprite &panel = AGG::GetICN(ICN::NGHSBKG, 0);
	const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
	const Point top((display.w() - back.w()) / 2, (display.h() - back.h()) / 2);

	rectPanel = Rect(top.x + 204, top.y + 32, panel.w(), panel.h());
	pointDifficultyInfo = Point(rectPanel.x + 24, rectPanel.y + 93);
	pointOpponentInfo = Point(rectPanel.x + 24, rectPanel.y + 202);
	pointClassInfo = Point(rectPanel.x + 24, rectPanel.y + 282);

	coordDifficulty[0] = Rect(rectPanel.x + 21, rectPanel.y + 91,  ngextra.w(), ngextra.h());
	coordDifficulty[1] = Rect(rectPanel.x + 98, rectPanel.y + 91,  ngextra.w(), ngextra.h());
	coordDifficulty[2] = Rect(rectPanel.x + 174, rectPanel.y + 91, ngextra.w(), ngextra.h());
	coordDifficulty[3] = Rect(rectPanel.x + 251, rectPanel.y + 91, ngextra.w(), ngextra.h());
	coordDifficulty[4] = Rect(rectPanel.x + 328, rectPanel.y + 91, ngextra.w(), ngextra.h());

	buttonSelectMaps = new Button(rectPanel.x + 309, rectPanel.y + 45, ICN::NGEXTRA, 64, 65);
	buttonOk = new Button(rectPanel.x + 31, rectPanel.y + 380, ICN::NGEXTRA, 66, 67);
	buttonCancel = new Button(rectPanel.x + 287, rectPanel.y + 380, ICN::NGEXTRA, 68, 69);

	display.Blit(back, top);
    }

    const bool reset_starting_settings = (Color::GRAY == conf.MyColor() || !FilePresent(conf.MapsFile()));

    // set first maps settings
    if(reset_starting_settings)
	conf.LoadFileMapsMP2(lists.front().file);

    // first allow color
    if(reset_starting_settings)
    {
	const Maps::FileInfo & info = conf.CurrentFileInfo();
	conf.SetPlayersColors(Color::GetFirst(info.AllowHumanColors()));
    }

    Scenario::RedrawStaticInfo(rectPanel);
    Scenario::RedrawDifficultyInfo(pointDifficultyInfo, !conf.QVGA());

    UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
    Scenario::RedrawOpponentsInfo(pointOpponentInfo);

    UpdateCoordClassInfo(pointClassInfo, coordClass);
    Scenario::RedrawClassInfo(pointClassInfo, !conf.QVGA());
    
    SpriteCursor levelCursor(ngextra);

    switch(conf.GameDifficulty())
    {
	case Difficulty::EASY:		levelCursor.Move(coordDifficulty[0]); break;
	case Difficulty::NORMAL:	levelCursor.Move(coordDifficulty[1]); break;
	case Difficulty::HARD:		levelCursor.Move(coordDifficulty[2]); break;
	case Difficulty::EXPERT:	levelCursor.Move(coordDifficulty[3]); break;
	case Difficulty::IMPOSSIBLE:	levelCursor.Move(coordDifficulty[4]); break;
    }

    levelCursor.Show();

    TextSprite* rating = conf.QVGA() ? NULL : new TextSprite();

    if(rating)
    {
	rating->SetFont(Font::BIG);
	rating->SetPos(rectPanel.x + 166, rectPanel.y + 383);
	RedrawRatingInfo(*rating);
    }

    if(buttonSelectMaps) buttonSelectMaps->Draw();
    buttonOk->Draw();
    buttonCancel->Draw();

    cursor.Show();
    display.Flip();

    while(le.HandleEvents())
    {
	// press button
	if(buttonSelectMaps)
	le.MousePressLeft(*buttonSelectMaps) ? buttonSelectMaps->PressDraw() : buttonSelectMaps->ReleaseDraw();
	le.MousePressLeft(*buttonOk) ? buttonOk->PressDraw() : buttonOk->ReleaseDraw();
	le.MousePressLeft(*buttonCancel) ? buttonCancel->PressDraw() : buttonCancel->ReleaseDraw();

	// click select
	if(buttonSelectMaps &&
	  (Game::HotKeyPress(Game::EVENT_BUTTON_SELECT) || le.MouseClickLeft(*buttonSelectMaps)))
	{
	    std::string filemaps;
	    if(Dialog::SelectScenario(lists, filemaps) && conf.LoadFileMapsMP2(filemaps))
	    {
		cursor.Hide();
		levelCursor.Hide();
		const Maps::FileInfo & info = conf.CurrentFileInfo();
		conf.SetPlayersColors(Color::GetFirst(info.AllowHumanColors()));
		Scenario::RedrawStaticInfo(rectPanel);
		Scenario::RedrawDifficultyInfo(pointDifficultyInfo, !conf.QVGA());
		UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
		Scenario::RedrawOpponentsInfo(pointOpponentInfo);
		UpdateCoordClassInfo(pointClassInfo, coordClass);
		Scenario::RedrawClassInfo(pointClassInfo, !conf.QVGA());
		if(rating) RedrawRatingInfo(*rating);
		// default difficulty normal
		levelCursor.Move(coordDifficulty[1]);
		levelCursor.Show();
    		conf.SetGameDifficulty(Difficulty::NORMAL);
		buttonOk->Draw();
		buttonCancel->Draw();
	    }
	    cursor.Show();
	    display.Flip();
	}
	else
	// click cancel
	if(HotKeyPress(EVENT_DEFAULT_EXIT) || le.MouseClickLeft(*buttonCancel))
	{
	    result = MAINMENU;
	    break;
	}
	else
	// click ok
	if(HotKeyPress(EVENT_DEFAULT_READY) || le.MouseClickLeft(*buttonOk))
	{
	    DEBUG(DBG_GAME, DBG_INFO, "select maps: " << conf.MapsFile() << \
		    ", difficulty: " << Difficulty::String(conf.GameDifficulty()));
	    conf.FixKingdomRandomRace();
	    //FIXME: dump colors: players and computers
	    //DEBUG(DBG_GAME, DBG_INFO, "select color: " << Color::String(conf.MyColor()));
	    result = STARTGAME;
	    break;
	}
	else
	if(le.MouseClickLeft(rectPanel))
	{
	    // select difficulty
	    if(coordDifficulty.end() != (itr = std::find_if(coordDifficulty.begin(), coordDifficulty.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))))
	    {
		cursor.Hide();
		levelCursor.Move((*itr).x, (*itr).y);
		conf.SetGameDifficulty(itr - coordDifficulty.begin());
		if(rating) RedrawRatingInfo(*rating);
		cursor.Show();
		display.Flip();
	    }
	    else
	    // select color
	    if(coordColors.end() != (itr = std::find_if(coordColors.begin(), coordColors.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))))
	    {
		Color::color_t color = Color::GetFromIndex(itr - coordColors.begin());
		const Maps::FileInfo & info = conf.CurrentFileInfo();

		if(info.AllowHumanColors() & color)
		{
		    cursor.Hide();
		    u8 players = color;
		
		    if(conf.GameType(Game::TYPE_NETWORK | Game::TYPE_HOTSEAT))
		    {
			/* reset color */
		        if(conf.PlayersColors() & color)
			{
			    if(1 < Color::Count(conf.PlayersColors()))
		    		players = conf.PlayersColors() & ~color;
			}
			/* set color */
			else
			    players = conf.PlayersColors() | color;
		    }

		    conf.SetPlayersColors(players);
		    Scenario::RedrawOpponentsInfo(pointOpponentInfo);
		    cursor.Show();
		    display.Flip();
		}
	    }
	    else
	    // select class
	    if(coordClass.end() != (itr = std::find_if(coordClass.begin(), coordClass.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))))
	    {
		Color::color_t color = Color::GetFromIndex(itr - coordClass.begin());
		if(conf.AllowChangeRace(color))
		{
		    cursor.Hide();
		    u8 race = conf.KingdomRace(color);
		    switch(race)
		    {
			case Race::KNGT: race = Race::BARB; break;
			case Race::BARB: race = Race::SORC; break;
			case Race::SORC: race = Race::WRLK; break;
			case Race::WRLK: race = Race::WZRD; break;
			case Race::WZRD: race = Race::NECR; break;
			case Race::NECR: race = Race::RAND; break;
			case Race::RAND: race = Race::KNGT; break;
			default: break;
		    }
		    conf.SetKingdomRace(color, race);
		    Scenario::RedrawStaticInfo(rectPanel);
		    levelCursor.Redraw();
		    Scenario::RedrawDifficultyInfo(pointDifficultyInfo, !conf.QVGA());
		    Scenario::RedrawOpponentsInfo(pointOpponentInfo);
		    Scenario::RedrawClassInfo(pointClassInfo, !conf.QVGA());
		    if(rating) RedrawRatingInfo(*rating);
		    buttonOk->Draw();
		    buttonCancel->Draw();
		    cursor.Show();
		    display.Flip();
		}
	    }
	}
	
	if(le.MousePressRight(rectPanel))
	{
	    // right info
	    if(buttonSelectMaps && le.MousePressRight(*buttonSelectMaps))
		Dialog::Message(_("Scenario"), _("Click here to select which scenario to play."), Font::BIG);
	    else
	    // difficulty
	    if(coordDifficulty.end() != (itr = std::find_if(coordDifficulty.begin(), coordDifficulty.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))))
		Dialog::Message(_("Game Difficulty"), _("This lets you change the starting difficulty at which you will play. Higher difficulty levels start you of with fewer resources, and at the higher settings, give extra resources to the computer."), Font::BIG);
	    else
	    // color
	    if(coordColors.end() != (itr = std::find_if(coordColors.begin(), coordColors.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))) &&
		(Game::GetKingdomColors() & Color::GetFromIndex(itr - coordColors.begin())))
		    Dialog::Message(_("Opponents"), _("This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player."), Font::BIG);
	    else
	    // class
	    if(coordClass.end() != (itr = std::find_if(coordClass.begin(), coordClass.end(), std::bind2nd(RectIncludePoint(), le.GetMouseCursor()))) &&
		(Game::GetKingdomColors() & Color::GetFromIndex(itr - coordClass.begin())))
		    Dialog::Message(_("Class"), _("This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alignment."), Font::BIG);
	    else
	    if(rating && le.MousePressRight(rating->GetRect()))
		Dialog::Message(_("Difficulty Rating"), _("The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score."), Font::BIG);
	    else
	    if(le.MousePressRight(*buttonOk))
		Dialog::Message(_("OK"), _("Click to accept these settings and start a new game."), Font::BIG);
	    else
	    if(le.MousePressRight(*buttonCancel))
		Dialog::Message(_("Cancel"), _("Click to return to the main menu."), Font::BIG);
	}
    }

    cursor.Hide();

    if(result == STARTGAME)
    {
	conf.SetMyColor(Color::GetFirst(conf.PlayersColors()));
	conf.SetCurrentColor(Color::NONE);
	if(conf.ExtUseFade()) display.Fade();
	Game::ShowLoadMapsText();
	// Load maps
	world.LoadMaps(conf.MapsFile());
    }

    if(frameborder) delete frameborder;
    if(rating) delete rating;
    if(buttonSelectMaps) delete buttonSelectMaps;
    delete buttonOk;
    delete buttonCancel;

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
    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, 3);
    u8 current = 0;

    std::fill(rects.begin(), rects.end(), Rect());

    const Colors colors(Game::GetKingdomColors());

    for(Colors::const_iterator
	it = colors.begin(); it != colors.end(); ++it)
	rects[Color::GetIndex(*it)] = Rect(dst.x + GetStepFor(current++, sprite.w(), colors.size()), dst.y, sprite.w(), sprite.h());
}

void Game::Scenario::RedrawStaticInfo(const Rect & rt)
{
    Display & display = Display::Get();
    Settings & conf = Settings::Get();

    if(conf.QVGA())
    {
	const Sprite & background = AGG::GetICN(ICN::STONEBAK, 0);
	display.Blit(background, Rect(0, 0, rt.w, rt.h), rt);

	Text text;
	text.Set(conf.CurrentFileInfo().name, Font::BIG);
	text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 5);
    }
    else
    {
	// image panel
	const Sprite &panel = AGG::GetICN(ICN::NGHSBKG, 0);
	display.Blit(panel, rt);

	// text scenario
	Text text(_("Scenario:"), Font::BIG);
        text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 20);

	// maps name
	text.Set(conf.MapsName());
	text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 46);
    
	// text game difficulty
	text.Set(_("Game Difficulty:"));
	text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 75);

	// text opponents
	text.Set(_("Opponents:"), Font::BIG);
	text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 181);

	// text class
	text.Set(_("Class:"), Font::BIG);
	text.Blit(rt.x + (rt.w - text.w()) / 2, rt.y + 262);
    }
}

void Game::Scenario::RedrawOpponentsInfo(const Point & dst, const std::vector<Player> *players)
{
    const Settings & conf = Settings::Get();
    const Maps::FileInfo & fi = conf.CurrentFileInfo();

    u8 current = 0;
    const Colors colors(Game::GetKingdomColors());

    for(Colors::const_iterator
	color = colors.begin(); color != colors.end(); ++color)
    {
	u8 index = 0;

	// current human
	if(conf.PlayersColors() & *color)
	    index = 9 + Color::GetIndex(*color);
	else
	// comp only
	if(fi.ComputerOnlyColors() & *color)
	    index = 15 + Color::GetIndex(*color);
	else
	// comp/human
	    index = 3 + Color::GetIndex(*color);

	// multiplay sprite offset
        if(players) index += 24;

	if(index)
	{
		const Sprite & sprite = AGG::GetICN(ICN::NGEXTRA, index);
		Display::Get().Blit(sprite, dst.x + GetStepFor(current, sprite.w(), colors.size()), dst.y);
		
		// draw name
		if(players)
		{
		    std::vector<Player>::const_iterator itp = std::find_if(players->begin(), players->end(),
							std::bind2nd(std::mem_fun_ref(&Player::isColor), *color));
		    if(players->end() != itp)
		    {
			Text name((*itp).player_name, Font::SMALL);
			name.Blit(dst.x + GetStepFor(current, sprite.w(), colors.size()) + (sprite.w() - name.w()) / 2,
											dst.y + sprite.h() - 14);
		    }
		}

		++current;
	}
    }
}

void Game::Scenario::RedrawClassInfo(const Point & dst, bool label)
{
    Display & display = Display::Get();
    const Settings & conf = Settings::Get();
    u8 current = 0;

    const Colors colors(Game::GetKingdomColors());

    for(Colors::const_iterator
	color = colors.begin(); color != colors.end(); ++color)
    {
	    u8 index = 0;
	    const u8 race = conf.KingdomRace(*color);
	    switch(race)
	    {
		case Race::KNGT: index = conf.AllowChangeRace(*color) ? 51 : 70; break;
	        case Race::BARB: index = conf.AllowChangeRace(*color) ? 52 : 71; break;
	        case Race::SORC: index = conf.AllowChangeRace(*color) ? 53 : 72; break;
		case Race::WRLK: index = conf.AllowChangeRace(*color) ? 54 : 73; break;
	        case Race::WZRD: index = conf.AllowChangeRace(*color) ? 55 : 74; break;
		case Race::NECR: index = conf.AllowChangeRace(*color) ? 56 : 75; break;
	        case Race::MULT: index = 76; break;
		case Race::RAND: index = 58; break;
		default: continue;
	    }

    	    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, index);
	    display.Blit(sprite, dst.x + GetStepFor(current, sprite.w(), colors.size()), dst.y);

	    if(label)
	    {
		const std::string & name = (Race::NECR == race ? _("Necroman") : Race::String(race));
		Text text(name, Font::SMALL);
		text.Blit(dst.x + GetStepFor(current, sprite.w(), colors.size()) + (sprite.w() - text.w()) / 2, dst.y + sprite.h() + 2);
	    }

    	    ++current;
    }
}

void Game::Scenario::RedrawDifficultyInfo(const Point & dst, bool label)
{
    Display & display = Display::Get();

    for(u8 current = Difficulty::EASY; current <= Difficulty::IMPOSSIBLE; ++current)
    {
    	const Sprite & sprite = AGG::GetICN(ICN::NGHSBKG, 0);
    	Rect src_rt(24, 94, 65, 65);
    	u16 offset = current * (src_rt.w + 12);
    	src_rt.x = src_rt.x + offset;
	display.Blit(sprite, src_rt, dst.x + offset, dst.y);

	if(label)
	{
	    Text text(Difficulty::String(current), Font::SMALL);
	    text.Blit(dst.x + offset + (src_rt.w - text.w()) / 2, dst.y + src_rt.h + 5);
	}
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
