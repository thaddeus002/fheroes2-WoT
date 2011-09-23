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

#include <map>
#include <cstdlib>
#include <algorithm>

#include "gamedefs.h"
#include "tinyconfig.h"
#include "settings.h"
#include "game_focus.h"
#include "maps_tiles.h"
#include "ground.h"
#include "world.h"
#include "kingdom.h"
#include "castle.h"
#include "mp2.h"
#include "agg.h"
#include "test.h"
#include "cursor.h"
#include "monster.h"
#include "spell.h"
#include "payment.h"
#include "profit.h"
#include "buildinginfo.h"
#include "skill.h"
#include "battle2.h"
#include "tools.h"
#include "difficulty.h"
#include "game_static.h"
#include "ai.h"
#include "game.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

namespace Game
{
    u8 GetMixerChannelFromObject(const Maps::Tiles &);
    void AnimateDelaysInitialize(void);
    void KeyboardGlobalFilter(int, u16);
    void UpdateGlobalDefines(const std::string &);
    void LoadExternalResource(const Settings &);

    void HotKeysDefaults(void);
    void HotKeysLoad(const std::string &);

    bool	disable_change_music = false;
    u16		reserved_vols[LOOPXX_COUNT];
    char**	argv = NULL;
    u8		current_music = MUS::UNKNOWN;
    u32		castle_animation_frame = 0;
    u32		maps_animation_frame = 0;
}

Game::menu_t Game::Testing(u8 t)
{
#ifndef BUILD_RELEASE
    Test::Run(t);
    return Game::QUITGAME;
#else
    return Game::MAINMENU;
#endif
}

Game::menu_t Game::Credits(void)
{
    const Settings & conf = Settings::Get();

    std::string str;
    str.reserve(200);

    str.append("version: ");
    str.append(conf.GetVersion());
    str.append("\n \n");
    str.append("This program is distributed under the terms of the GPL v2.");
    str.append("\n");
    str.append("AI engine: ");
    str.append(AI::Type());
    str.append(", license: ");
    str.append(AI::License());
    str.append("\n \n");
    str.append("Site project:\n");
    str.append("http://sf.net/projects/fheroes2");
    str.append("\n \n");
    str.append("Authors:\n");
    str.append("Andrey Afletdinov, maintainer\n");
    str.append("email: fheroes2 at gmail.com\n");

    Dialog::Message("Free Heroes II Engine", str, Font::SMALL, Dialog::OK);

    //VERBOSE("Credits: under construction.");

    return Game::MAINMENU;
}

std::string Game::GetARGV(int v)
{
    return std::string(argv[v]);
}

bool Game::ChangeMusicDisabled(void)
{
    return disable_change_music;
}

void Game::DisableChangeMusic(bool f)
{
    disable_change_music = f;
}

void Game::Init(char** ptr)
{
    Settings & conf = Settings::Get();
    LocalEvent & le = LocalEvent::Get();

    argv = ptr;

    // update all global defines
    if(conf.UseAltResource()) LoadExternalResource(conf);

    // default events
    le.SetStateDefaults();

    // set global events
    le.SetGlobalFilterMouseEvents(Cursor::Redraw);
    le.SetGlobalFilterKeysEvents(Game::KeyboardGlobalFilter);
    le.SetGlobalFilter(true);

    le.SetTapMode(conf.ExtTapMode());

    Game::AnimateDelaysInitialize();

    HotKeysDefaults();

    const std::string hotkeys = conf.LocalPrefix() + SEPARATOR + "fheroes2.key";

    if(FilePresent(hotkeys))
	Game::HotKeysLoad(hotkeys);
}

u8 Game::CurrentMusic(void)
{
    return current_music;
}

void Game::SetCurrentMusic(u8 mus)
{
    current_music = mus;
}

u32 & Game::MapsAnimationFrame(void)
{
    return maps_animation_frame;
}

u32 & Game::CastleAnimationFrame(void)
{
    return castle_animation_frame;
}

void Game::SetFixVideoMode(void)
{
    const Settings & conf = Settings::Get();

    Size fixsize(conf.VideoMode());

    u32 max = (6 + conf.MapsWidth()) * TILEWIDTH;

    if(conf.VideoMode().w > max) fixsize.w = max;
    if(conf.VideoMode().h > max) fixsize.h = max;

    Display::Get().SetVideoMode(fixsize.w, fixsize.h, conf.DisplayFlags());
}

/* play all sound from focus area game */
void Game::EnvironmentSoundMixer(void)
{
    const Point abs_pt(GameFocus::GetCenter());
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	std::fill(reserved_vols, reserved_vols + LOOPXX_COUNT, 0);

        // scan 4x4 square from focus
        for(s16 yy = abs_pt.y - 3; yy <= abs_pt.y + 3; ++yy)
    	{
    	    for(s16 xx = abs_pt.x - 3; xx <= abs_pt.x + 3; ++xx)
	    {
		if(Maps::isValidAbsPoint(xx, yy))
		{
		    const u8 channel = GetMixerChannelFromObject(world.GetTiles(xx, yy));
    		    if(channel < LOOPXX_COUNT)
		    {
			// calculation volume
    			const u8 length = std::max(std::abs(xx - abs_pt.x), std::abs(yy - abs_pt.y));
			const u16 volume = (2 < length ? 4 : (1 < length ? 8 : (0 < length ? 12 : 16))) * Mixer::MaxVolume() / 16;

			if(volume > reserved_vols[channel]) reserved_vols[channel] = volume;
		    }
		}
	    }
	}

	AGG::Cache::Get().LoadLOOPXXSounds(reserved_vols);
    }
}

u8 Game::GetMixerChannelFromObject(const Maps::Tiles & tile)
{
    // force: check stream
    if(tile.isStream()) return 13;

    return M82::GetIndexLOOP00XXFromObject(tile.GetObject(false));
}

u8 Game::GetRating(void)
{
    Settings & conf = Settings::Get();
    u8 rating = 50;

    switch(conf.MapsDifficulty())
    {
        case Difficulty::NORMAL:     rating += 20; break;
        case Difficulty::HARD:       rating += 40; break;
        case Difficulty::EXPERT:
        case Difficulty::IMPOSSIBLE:	rating += 80; break;
	default: break;
    }

    switch(conf.GameDifficulty())
    {
        case Difficulty::NORMAL:     rating += 30; break;
        case Difficulty::HARD:       rating += 50; break;
        case Difficulty::EXPERT:	rating += 70; break;
        case Difficulty::IMPOSSIBLE:	rating += 90; break;
	default: break;
    }

    return rating;
}

u16 Game::GetGameOverScores(void)
{
    Settings & conf = Settings::Get();

    u8 k_size = 0;

    switch(conf.MapsWidth())
    {
	case Maps::SMALL:	k_size = 140; break;
	case Maps::MEDIUM:	k_size = 100; break;
	case Maps::LARGE:	k_size =  80; break;
	case Maps::XLARGE:	k_size =  60; break;
	default: break;
    }

    u8 flag = 0;
    u8 nk = 0;
    u16 end_days = world.CountDay();

    for(u16 ii = 1; ii <= end_days; ++ii)
    {
	nk = ii * k_size / 100;

	if(0 == flag && nk > 60){ end_days = ii + (world.CountDay() - ii) / 2; flag = 1; }
	else
	if(1 == flag && nk > 120) end_days = ii + (world.CountDay() - ii) / 2;
	else
	if(nk > 180) break;
    }

    return GetRating() * (200 - nk) / 100;
}

void Game::ShowLoadMapsText(void)
{
    Display & display = Display::Get();
    const Rect pos(0, display.h() / 2, display.w(), display.h() / 2);
    TextBox text(_("Maps Loading..."), Font::BIG, pos.w);

    // blit test
    display.Fill(0, 0, 0);
    text.Blit(pos, display);
    display.Flip();
}

u8 Game::GetLostTownDays(void)
{
    return GameStatic::GetGameOverLostDays();
}

u8 Game::GetViewDistance(u8 d)
{
    return GameStatic::GetOverViewDistance(d);
}

void Game::UpdateGlobalDefines(const std::string & spec)
{
#ifdef WITH_XML
    // parse profits.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_globals = NULL;

    if(doc.LoadFile(spec.c_str()) &&
	NULL != (xml_globals = doc.FirstChildElement("globals")))
    {
	// starting_resource
	KingdomUpdateStartingResource(xml_globals->FirstChildElement("starting_resource"));
	// view_distance
	OverViewUpdateStatic(xml_globals->FirstChildElement("view_distance"));
	// kingdom
	KingdomUpdateStatic(xml_globals->FirstChildElement("kingdom"));
	// game_over
	GameOverUpdateStatic(xml_globals->FirstChildElement("game_over"));
	// whirlpool
	WhirlpoolUpdateStatic(xml_globals->FirstChildElement("whirlpool"));
	// heroes
	HeroesUpdateStatic(xml_globals->FirstChildElement("heroes"));
	// castle_extra_growth 
	CastleUpdateGrowth(xml_globals->FirstChildElement("castle_extra_growth"));
	// monster upgrade ratio
	MonsterUpdateStatic(xml_globals->FirstChildElement("monster_upgrade"));
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

u8 Game::GetWhirlpoolPercent(void)
{
    return GameStatic::GetLostOnWhirlpoolPercent();
}

void Game::LoadExternalResource(const Settings & conf)
{
    std::string spec;

    // globals.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "globals.xml";

    if(FilePresent(spec))
	Game::UpdateGlobalDefines(spec);

    // animations.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "animations.xml";

    if(FilePresent(spec))
	Battle2::UpdateMonsterInfoAnimation(spec);

    // battle.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "battle.xml";

    if(FilePresent(spec))
	Battle2::UpdateMonsterAttributes(spec);

    // monsters.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "monsters.xml";

    if(FilePresent(spec))
	Monster::UpdateStats(spec);

    // spells.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "spells.xml";

    if(FilePresent(spec))
	Spell::UpdateStats(spec);

    // artifacts.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "artifacts.xml";

    if(FilePresent(spec))
	Artifact::UpdateStats(spec);

    // buildings.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "buildings.xml";

    if(FilePresent(spec))
	BuildingInfo::UpdateCosts(spec);

    // payments.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "payments.xml";

    if(FilePresent(spec))
	PaymentConditions::UpdateCosts(spec);

    // profits.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "profits.xml";

    if(FilePresent(spec))
	ProfitConditions::UpdateCosts(spec);

    // skills.xml
    spec = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "stats" + SEPARATOR + "skills.xml";

    if(FilePresent(spec))
	Skill::UpdateStats(spec);
}

std::string Game::GetEncodeString(const char* str)
{
    const Settings & conf = Settings::Get();
    std::string str1(str);
    std::string str2(_(str));

    // encode name
    if(str1 == str2 && str1.size() &&
       conf.Unicode() && conf.MapsCharset().size())
	str2 = EncodeString(str1, conf.MapsCharset().c_str());

    return str2;
}

u8 Game::GetKingdomColors(void)
{
    return Settings::Get().GetPlayers().GetColors();
}

#include <cmath>
std::string Game::CountScoute(u32 count, u8 scoute)
{
    float infelicity = 0;
    std::string res;

    switch(scoute)
    {
        case Skill::Level::BASIC:
            infelicity = count * 30 / 100.0;
            break;

        case Skill::Level::ADVANCED:
            infelicity = count * 15 / 100.0;
            break;

        case Skill::Level::EXPERT:
            String::AddInt(res, count);
            break;

        default:
            return Army::SizeString(count);
    }

    if(res.empty())
    {
        u32 min = Rand::Get(static_cast<u32>(std::floor(count - infelicity + 0.5)),
			    static_cast<u32>(std::floor(count + infelicity + 0.5)));
        u32 max = 0;

        if(min > count)
        {
            max = min;
            min = static_cast<u32>(std::floor(count - infelicity + 0.5));
        }
        else
            max = static_cast<u32>(std::floor(count + infelicity + 0.5));

        String::AddInt(res, min);

        if(min != max)
        {
            res.append("-");
            String::AddInt(res, max);
        }
    }

    return res;
}
