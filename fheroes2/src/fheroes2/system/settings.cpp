/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include <fstream>
#include "settings.h"

namespace
{
    struct ModeSetting
    {
        const char *name;           //Name of config setting
        Settings::settings_t set;   //Mode value to set for "on"
        Settings::settings_t reset; //Mode value to reset for "off" if different than setting value (ie. music)
    } modeSettings[] =
    {
        { "sound",             Settings::SOUND                       },
        { "music",             Settings::MUSIC_MIDI, Settings::MUSIC },
        { "fullscreen",        Settings::FULLSCREEN                  },
        { "evilinterface",     Settings::EVILINTERFACE               },
        { "shadow",            Settings::SHADOW                      },
        { "original",          Settings::ORIGINAL                    },
        { "logo",              Settings::LOGO                        },
        { "battlegrid",        Settings::BATTLEGRID                  },
        { "battlemoveshadow",  Settings::BATTLEMOVESHADOW            },
        { "battlemouseshadow", Settings::BATTLEMOUSESHADOW           },
        { "unicode",           Settings::UNICODE                     },
    };
}

/* constructor */
Settings::Settings() : major_version(MAJOR_VERSION), minor_version(MINOR_VERSION), build_date(BUILD_DATE),
    debug(0), video_mode(640, 480), game_difficulty(Difficulty::NORMAL),
    my_color(Color::GRAY), cur_color(Color::GRAY), path_cache_directory(""), path_data_directory("data"), path_maps_directory("maps"),
    translationFile("english.str"),
    font_normal("files/fonts/dejavusans.ttf"), font_small("files/fonts/dejavusans.ttf"), size_normal(15), size_small(10),
    sound_volume(6), music_volume(6), animation(6), game(0), players(0), preferably_count_players(0)
{
    SetModes(SHADOW);
    SetModes(ORIGINAL);
    SetModes(LOGO);
}

Settings & Settings::Get(void)
{
    static Settings conf;
    
    return conf;
}

bool Settings::Read(const std::string & filename)
{
    if(filename.empty()) return false;

    // read
    std::fstream file(filename.c_str(), std::ios::in);

    if(! file || file.fail()) return false;

    std::string str;

    while(std::getline(file, str))
    {
	if(String::Comment(str) || 0 == str.size()) continue;

	const size_t pos = str.find('=');

	if(std::string::npos != pos)
	{
	    std::string left(str.substr(0, pos - 1));
	    std::string right(str.substr(pos + 1, str.length() - pos - 1));

	    String::Trim(left);
	    String::Trim(right);

	    String::Lower(left);
	    
	    if(right == "on")
		SetStrModes(left);
	    else
	    if(right == "off")
		ResetStrModes(left);
	    else
		Parse(left, right);
	}
    }

    file.close();

#ifndef WITH_TTF
    ResetModes(UNICODE);
#endif

    if(font_normal.empty() || font_small.empty()) ResetModes(UNICODE);

    return true;
}

bool Settings::Save(const std::string & filename) const
{
    if(filename.empty()) return false;
    
    std::fstream file(filename.c_str(), std::ios::out);

    if(! file || file.fail()) return false;
    
    Dump(file);

    file.close();

    return true;
}

void Settings::Dump(std::ostream & stream) const
{
    std::string str;

    String::AddInt(str, major_version);
    str += ".";
    String::AddInt(str, minor_version);
    
    stream << "# fheroes2 dump config, version " << str;

    str.clear();
    String::AddInt(str, build_date);
    stream <<", build " << str << std::endl;

    stream << "cache = " << path_cache_directory << std::endl;
    stream << "data = " << path_data_directory << std::endl;
    stream << "maps = " << path_maps_directory << std::endl;
    stream << "translation = " << translationFile << std::endl;
    stream << "fonts normal = " << font_normal << std::endl;
    stream << "fonts small = " << font_small << std::endl;
    stream << "fonts normal size = " << static_cast<int>(size_normal) << std::endl;
    stream << "fonts small size = " << static_cast<int>(size_small) << std::endl;

    str.clear();
    String::AddInt(str, video_mode.w);
    str += "x";
    String::AddInt(str, video_mode.h);

    stream << "videomode = " << str << std::endl;
    stream << "sound = " << (Modes(SOUND) ? "on"  : "off") << std::endl;
    stream << "music = " << (Modes(MUSIC_CD) ? "cd" : ( Modes(MUSIC_MIDI) ? "midi" : ( Modes(MUSIC_EXT) ? "ext" : "off"))) << std::endl;
    stream << "sound volume = " << static_cast<int>(sound_volume) << std::endl;
    stream << "music volume = " << static_cast<int>(music_volume) << std::endl;
    stream << "animation = " << static_cast<int>(animation) << std::endl;
    stream << "fullscreen = " << (Modes(FULLSCREEN) ? "on"  : "off") << std::endl;
    stream << "evilinterface = " << (Modes(EVILINTERFACE) ? "on"  : "off") << std::endl;
    stream << "shadow = " << (Modes(SHADOW) ? "on"  : "off") << std::endl;
    stream << "original = " << (Modes(ORIGINAL) ? "on"  : "off") << std::endl;
    stream << "debug = " << (debug ? "on"  : "off") << std::endl;
    stream << "battle grid = " << (Modes(BATTLEGRID) ? "on" : "off") << std::endl;
    stream << "battle movement shadow = " << (Modes(BATTLEMOVESHADOW) ? "on" : "off") << std::endl;
    stream << "battle mouse shadow = " << (Modes(BATTLEMOUSESHADOW) ? "on" : "off") << std::endl;
    stream << "unicode = " << (Modes(UNICODE) ? "on" : "off") << std::endl;

    stream << std::endl;
}

/* read maps info */
void Settings::LoadFileMaps(const std::string & file)
{
    current_maps_file.Read(file);

    // set my color
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(current_maps_file.AllowColors() & color)
	{
	    my_color = color;

	    break;
	}

    // game difficulty
    game_difficulty = Difficulty::NORMAL;
}

/* get file info */
const Maps::FileInfo & Settings::FileInfo(void) const
{
    return current_maps_file;
}

Maps::FileInfo & Settings::FileInfo(void)
{
    return current_maps_file;
}

/* return major version */
u8 Settings::MajorVersion(void) const { return major_version; }

/* return minor version */
u8 Settings::MinorVersion(void) const { return minor_version; }

/* return build date */
u32 Settings::DateBuild(void) const { return build_date; }

/* return debug */
u8 Settings::Debug(void) const { return debug; }

/* return game difficulty */
Difficulty::difficulty_t Settings::GameDifficulty(void) const { return game_difficulty; }

Color::color_t Settings::CurrentColor(void) const { return cur_color; }
Color::color_t Settings::MyColor(void) const { return my_color; }

const std::string & Settings::TranslationFile(void) const { return translationFile; }

/* return fontname */
const std::string & Settings::FontsNormal(void) const { return font_normal; }
const std::string & Settings::FontsSmall(void) const { return font_small; }
u8 Settings::FontsNormalSize(void) const { return size_normal; }
u8 Settings::FontsSmallSize(void) const { return size_small; }
bool Settings::FontsRenderBlended(void) const { return Modes(FONTRENDERBLENDED); }

/* return path to image cache directory */
const std::string & Settings::CacheDirectory(void) const { return path_cache_directory; }

/* return path to data directory */
const std::string & Settings::DataDirectory(void) const { return path_data_directory; }

/* return path to maps directory */
const std::string & Settings::MapsDirectory(void) const { return path_maps_directory; }

/* return editor */
bool Settings::Editor(void) const { return Modes(EDITOR); }

/* return original version */
bool Settings::Original(void) const { return Modes(ORIGINAL); }

/* return sound */
bool Settings::Sound(void) const { return Modes(SOUND); }

/* return music */
bool Settings::Music(void) const { return Modes(MUSIC); }

/* return animation */
u8   Settings::Animation(void) const { return animation; }

/* return full screen */
bool Settings::FullScreen(void) const { return Modes(FULLSCREEN); }

/* return evil interface */
bool Settings::EvilInterface(void) const { return Modes(EVILINTERFACE); }

/* return shadow */
bool Settings::Shadow(void) const { return Modes(SHADOW); }

/* get show logo */
bool Settings::Logo(void) const { return Modes(LOGO); }

/* battle grid */
bool Settings::BattleGrid(void) const { return Modes(BATTLEGRID); }

/* battle shaded grid */
bool Settings::BattleMovementShaded(void) const { return Modes(BATTLEMOVESHADOW); }

/* battle shaded mouse */
bool Settings::BattleMouseShaded(void) const { return Modes(BATTLEMOUSESHADOW); }

/* unicode support */
bool Settings::Unicode(void) const { return Modes(UNICODE); }

/* get video mode */
const Size & Settings::VideoMode(void) const { return video_mode; }

void Settings::Parse(const std::string & left, const std::string & right)
{
    // debug
    if(left == "debug") debug = String::ToInt(right);
    else
    // animation
    if(left == "animation") animation = String::ToInt(right);
    else
    // translation strings
    if(left == "translation") translationFile = right;
    else
    // font name
    if(left == "fonts normal") font_normal = right;
    else
    if(left == "fonts small") font_small = right;
    else
    if(left == "fonts normal size") size_normal = String::ToInt(right);
    else
    if(left == "fonts small size") size_small = String::ToInt(right);
    else
    if(left == "fonts render" && right == "blended") SetModes(FONTRENDERBLENDED);
    else
    // cache directory
    if(left == "cache") path_cache_directory = right;
    else
    // data directory
    if(left == "data") path_data_directory = right;
    else
    // maps directory
    if(left == "maps") path_maps_directory = right;
    else
    if(left == "music")
    {
	std::string lower(right);
	String::Lower(lower);

        if(lower == "midi")
        {
            ResetModes(MUSIC);
            SetModes(MUSIC_MIDI);
        }
        else if(lower == "cd")
        {
            ResetModes(MUSIC);
            SetModes(MUSIC_CD);
        }
        else if(lower == "ext")
        {
            ResetModes(MUSIC);
            SetModes(MUSIC_EXT);
        }
    }
    else
    // volume
    if(left == "sound volume")
    {
	sound_volume = String::ToInt(right);
	if(sound_volume > 10) sound_volume = 10;
    }
    else
    // volume
    if(left == "music volume")
    {
	music_volume = String::ToInt(right);
	if(music_volume > 10) music_volume = 10;
    }
    else
    // value
    if(left == "videomode")
    {
	// default
	video_mode.w = 640;
	video_mode.h = 480;

	std::string str(right);

	String::Lower(str);

	const size_t pos = str.find('x');

	if("800x600" == right)
	{
	    video_mode.w = 800;
	    video_mode.h = 576;
	}
	else
	if(std::string::npos != pos)
	{
	    std::string left2(str.substr(0, pos));
	    std::string right2(str.substr(pos + 1, str.length() - pos - 1));

	    String::Trim(left2);
	    String::Trim(right2);
		
	    video_mode.w = String::ToInt(left2);
	    video_mode.h = String::ToInt(right2);

	    if((video_mode.w % TILEWIDTH) || (video_mode.h % TILEWIDTH))
	    {
		u16 possible_w = video_mode.w / TILEWIDTH;
		u16 possible_h = video_mode.h / TILEWIDTH;

		possible_w *= TILEWIDTH;
		possible_h *= TILEWIDTH;

		if(possible_w < 640) possible_w = 640;
		if(possible_h < 480) possible_h = 480;

		std::cout << "Settings: unknown videomode: " << video_mode.w << "x" << video_mode.h << ", approximate load: " << possible_w << "x" << possible_h << std::endl;

		video_mode.w = possible_w;
		video_mode.h = possible_h;
	    }
	}
	else Error::Warning("Settings: unknown video mode: " + right);
    }
}

/* set level debug */
void Settings::SetDebug(const u8 d) { debug = d; }

/**/
void Settings::SetGameDifficulty(const Difficulty::difficulty_t d) { game_difficulty = d; }

void Settings::SetCurrentColor(const Color::color_t c) { cur_color = c; }
void Settings::SetMyColor(const Color::color_t c) { my_color = c; }

/**/
void Settings::SetStrModes(const std::string & key)
{
    // debug
    if(key == "debug")		debug = 1;
    else
    {
        for(u16 i = 0; i < sizeof(modeSettings) / sizeof(modeSettings[0]); i++)
        {
            if(key == modeSettings[i].name)
            {
                SetModes(modeSettings[i].set);
                break;
            }
        }
    }
}

void Settings::ResetStrModes(const std::string & key)
{
    // debug
    if(key == "debug")		debug = 0;
    else
    {
        for(u16 i = 0; i < sizeof(modeSettings) / sizeof(modeSettings[0]); i++)
        {
            if(key == modeSettings[i].name)
            {
                if(modeSettings[i].reset != Settings::NONE)
                    ResetModes(modeSettings[i].reset);
                else ResetModes(modeSettings[i].set);
                break;
            }
        }
    }
}

u8   Settings::SoundVolume(void) const
{
    return sound_volume;
}

u8   Settings::MusicVolume(void) const
{
    return music_volume;
}

/* sound volume: 0 - 10 */
void Settings::SetSoundVolume(const u8 v)
{
    sound_volume = 10 <= v ? 10 : v;
}

/* music volume: 0 - 10 */
void Settings::SetMusicVolume(const u8 v)
{
    music_volume = 10 <= v ? 10 : v;
}

/* animation speed: 1 - 10 */
void Settings::SetAnimation(const u8 s)
{
    animation = 10 <= s ? 10 : s;
}

/* check game type */
u8 Settings::GameType(void) const
{
    return game;
}

/* set game type */
void Settings::SetGameType(const Game::type_t type)
{
    game = type;
}

/* get color players */
u8 Settings::Players(void) const
{
    return players;
}

/* set game color players */
void Settings::SetPlayers(u8 c)
{
    players = c;
}

void Settings::SetPreferablyCountPlayers(u8 c)
{
    if(2 > c) preferably_count_players = 2;
    else
    if(6 < c) preferably_count_players = 6;
    else      preferably_count_players = c;
}

u8 Settings::PreferablyCountPlayers(void) const
{
    return preferably_count_players;
}
