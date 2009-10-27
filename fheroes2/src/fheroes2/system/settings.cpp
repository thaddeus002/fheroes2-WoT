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
#include <bitset>
#include "maps.h"
#include "settings.h"

#define DEFAULT_PORT	5154

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
        { "full screen",       Settings::FULLSCREEN                  },
        { "hide interface",    Settings::HIDEINTERFACE               },
        { "evil interface",    Settings::EVILINTERFACE               },
        { "evilinterface",     Settings::EVILINTERFACE               },
        { "fade",              Settings::FADE                        },
        { "shadow",            Settings::SHADOW                      },
        { "original",          Settings::ORIGINAL                    },
        { "logo",              Settings::LOGO                        },
        { "battlegrid",        Settings::BATTLEGRID                  },
        { "battlemoveshadow",  Settings::BATTLEMOVESHADOW            },
        { "battlemouseshadow", Settings::BATTLEMOUSESHADOW           },
        { "unicode",           Settings::USEUNICODE                  },
        { "autosave",          Settings::AUTOSAVE                    },
        { "alt resource",      Settings::ALTRESOURCE                 },
        { "alternative resource", Settings::ALTRESOURCE              },
        { "pocketpc",          Settings::POCKETPC                    },
        { "pocket pc",         Settings::POCKETPC                    },
        { "lowmemory",         Settings::LOWMEMORY                   },
        { "low memory",        Settings::LOWMEMORY                   },
        { "tapmode",           Settings::TAPMODE                     },
        { "tap mode",          Settings::TAPMODE                     },
    };
}

/* constructor */
Settings::Settings() : major_version(MAJOR_VERSION), minor_version(MINOR_VERSION),
#ifdef SVN_REVISION
    svn_version(SVN_REVISION),
#endif
    debug(0), video_mode(640, 480), game_difficulty(Difficulty::NORMAL),
    my_color(Color::GRAY), cur_color(Color::GRAY), path_data_directory("data"),
    font_normal("dejavusans.ttf"), font_small("dejavusans.ttf"), size_normal(15), size_small(10),
    sound_volume(6), music_volume(6), animation(6), game_type(0), players_colors(0), preferably_count_players(0),
    port(DEFAULT_PORT)
{
    build_version = "version: ";
    String::AddInt(build_version, MAJOR_VERSION);
    build_version += ".";
    String::AddInt(build_version, MINOR_VERSION);
    if(svn_version.size())
    {
	build_version += "." + svn_version;
    }

    // default maps dir
    list_maps_directory.push_back("maps");

    SetModes(SHADOW);
    SetModes(ORIGINAL);
    SetModes(FADE);
    SetModes(LOGO);

    SetModes(SHOWRADAR);
    SetModes(SHOWICONS);
    SetModes(SHOWBUTTONS);
    SetModes(SHOWSTATUS);
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
    ResetModes(USEUNICODE);
#endif

    if(font_normal.empty() || font_small.empty()) ResetModes(USEUNICODE);

#ifdef BUILD_RELEASE
    debug = 0;
#endif

    if(video_mode.w < 640 || video_mode.h < 480)
    {
	SetModes(POCKETPC);
    }
    else
    {
        ResetModes(POCKETPC);
    }

    if(Modes(POCKETPC))
    {
	SetModes(HIDEINTERFACE);
	SetModes(LOWMEMORY);
    }

    if(Modes(HIDEINTERFACE))
    {
       SetModes(SHOWCPANEL);
       ResetModes(SHOWRADAR);
       ResetModes(SHOWICONS);
       ResetModes(SHOWBUTTONS);
       ResetModes(SHOWSTATUS);
    }

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
    str += "." + svn_version;
    
    stream << std::endl;
    stream << "# fheroes2 dump config, version " << str << std::endl;

    if(path_data_directory.size()) stream << "data = " << path_data_directory << std::endl;

    ListMapsDirectory::const_iterator it1 = list_maps_directory.begin();
    ListMapsDirectory::const_iterator it2 = list_maps_directory.end();

    for(; it1 != it2; ++it1)
    stream << "maps = " << *it1 << std::endl;

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
    stream << "hide interface = " << (Modes(HIDEINTERFACE) ? "on"  : "off") << std::endl;
    stream << "evil interface = " << (Modes(EVILINTERFACE) ? "on"  : "off") << std::endl;
    stream << "fade = " << (Modes(FADE) ? "on"  : "off") << std::endl;
    stream << "shadow = " << (Modes(SHADOW) ? "on"  : "off") << std::endl;
    stream << "original = " << (Modes(ORIGINAL) ? "on"  : "off") << std::endl;
    stream << "alt resource = " << (Modes(ALTRESOURCE) ? "on"  : "off") << std::endl;
    stream << "debug = " << (debug ? "on"  : "off") << std::endl;

    stream << "battle grid = " << (Modes(BATTLEGRID) ? "on" : "off") << std::endl;
    stream << "battle movement shadow = " << (Modes(BATTLEMOVESHADOW) ? "on" : "off") << std::endl;
    stream << "battle mouse shadow = " << (Modes(BATTLEMOUSESHADOW) ? "on" : "off") << std::endl;

#ifdef WITH_TTF
    stream << "fonts normal = " << font_normal << std::endl;
    stream << "fonts small = " << font_small << std::endl;
    if(force_lang.size())
    stream << "force lang = " << force_lang << std::endl;
    stream << "fonts normal size = " << static_cast<int>(size_normal) << std::endl;
    stream << "fonts small size = " << static_cast<int>(size_small) << std::endl;
    stream << "unicode = " << (Modes(USEUNICODE) ? "on" : "off") << std::endl;
#endif

#ifndef WITH_MIXER
    stream << "playmus command = " << playmus_command << std::endl;
#endif

    stream << "autosave = " << (Modes(AUTOSAVE) ? "on" : "off") << std::endl;
    stream << "port = " << port << std::endl;

    if(Modes(TAPMODE))
    stream << "tap mode = on" << std::endl;
    if(Modes(LOWMEMORY))
    stream << "low memory = on" << std::endl;
    if(Modes(POCKETPC))
    stream << "pocket pc = on" << std::endl;

    stream << std::endl;
}

/* read maps info */
bool Settings::LoadFileMapsMP2(const std::string & file)
{
    if(! current_maps_file.ReadMP2(file)) return false;

    // get first color
    my_color = Color::Get(Color::GetFirst(current_maps_file.allow_colors));

    // game difficulty
    game_difficulty = Difficulty::NORMAL;

    std::bitset<8> b(current_maps_file.allow_colors);
    preferably_count_players = b.count();
    
    return true;
}

Maps::FileInfo & Settings::CurrentFileInfo(void)
{
    return current_maps_file;
}

/* return major version */
u8 Settings::MajorVersion(void) const { return major_version; }

/* return minor version */
u8 Settings::MinorVersion(void) const { return minor_version; }

/* return debug */
u8 Settings::Debug(void) const { return debug; }

/* return game difficulty */
Difficulty::difficulty_t Settings::GameDifficulty(void) const { return game_difficulty; }

Color::color_t Settings::CurrentColor(void) const { return cur_color; }
Color::color_t Settings::MyColor(void) const { return my_color; }

/* return fontname */
const std::string & Settings::FontsNormal(void) const { return font_normal; }
const std::string & Settings::FontsSmall(void) const { return font_small; }
const std::string & Settings::ForceLang(void) const { return force_lang; }
u8 Settings::FontsNormalSize(void) const { return size_normal; }
u8 Settings::FontsSmallSize(void) const { return size_small; }
bool Settings::FontsRenderBlended(void) const { return Modes(FONTRENDERBLENDED); }

const std::string & Settings::BuildVersion(void) const { return build_version; }

/* return path to data directory */
const std::string & Settings::DataDirectory(void) const { return path_data_directory; }

/* return path to maps directory */
const ListMapsDirectory & Settings::GetListMapsDirectory(void) const { return list_maps_directory; }

/* return path to locales directory */
const std::string & Settings::LocalPrefix(void) const { return local_prefix; }
const std::string & Settings::PlayMusCommand(void) const { return playmus_command; };

/* return editor */
bool Settings::Editor(void) const { return Modes(EDITOR); }

/* return original version */
bool Settings::Original(void) const { return Modes(ORIGINAL); }

/* return sound */
bool Settings::Sound(void) const { return Modes(SOUND); }

/* return music */
bool Settings::Music(void) const { return Modes(MUSIC); }

bool Settings::CDMusic(void) const { return Modes(MUSIC_CD | MUSIC_EXT); }

/* return animation */
u8   Settings::Animation(void) const { return animation; }

/* return full screen */
bool Settings::FullScreen(void) const { return Modes(FULLSCREEN); }

/* return evil interface */
bool Settings::EvilInterface(void) const { return Modes(EVILINTERFACE); }

bool Settings::HideInterface(void) const { return Modes(HIDEINTERFACE); }

bool Settings::ShowControlPanel(void) const { return Modes(SHOWCPANEL); }
bool Settings::ShowRadar(void) const { return Modes(SHOWRADAR); }
bool Settings::ShowIcons(void) const { return Modes(SHOWICONS); }
bool Settings::ShowButtons(void) const { return Modes(SHOWBUTTONS); }
bool Settings::ShowStatus(void) const { return Modes(SHOWSTATUS); }

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
bool Settings::Unicode(void) const { return Modes(USEUNICODE); }

bool Settings::PocketPC(void) const { return Modes(POCKETPC); }
bool Settings::LowMemory(void) const { return Modes(LOWMEMORY); }
bool Settings::TapMode(void) const { return Modes(TAPMODE); }

/* get video mode */
const Size & Settings::VideoMode(void) const { return video_mode; }

void Settings::Parse(const std::string & left, const std::string & right)
{
    // debug
    if(left == "debug") debug = String::ToInt(right);
    else
    // default depth
    if(left == "default depth") Surface::SetDefaultDepth(String::ToInt(right));
    else
    // animation
    if(left == "animation") animation = String::ToInt(right);
    else
    // playmus command
    if(left == "playmus command") playmus_command = right;
    // port
    if(left == "port"){ port = String::ToInt(right); if(!port) port = DEFAULT_PORT; }
    else
    // font name
    if(left == "lang") force_lang = right;
    else
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
    // data directory
    if(left == "data") path_data_directory = right;
    else
    // maps directory
    if(left == "maps") list_maps_directory.push_back(right);
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

		std::cout << "Settings: videomode: " << video_mode.w << "x" << video_mode.h << ", current: " << possible_w << "x" << possible_h << std::endl;
		video_mode.w = possible_w;
		video_mode.h = possible_h;
	    }
	}
	else Error::Warning("Settings: unknown video mode: " + right);
    }
    // offset mouse
    else
    if(left == "pointer offset x") LocalEvent::Get().SetMouseOffsetX(String::ToInt(right));
    else
    if(left == "pointer offset y") LocalEvent::Get().SetMouseOffsetY(String::ToInt(right));
    // tap delay for right click emulation
    else
    if(left == "tap delay") LocalEvent::Get().SetTapDelayForRightClickEmulation(String::ToInt(right));
}

/* set level debug */
void Settings::SetDebug(const u8 d)
{
    debug = d;
}

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
    return game_type;
}

/* set game type */
void Settings::SetGameType(const Game::type_t type)
{
    game_type = type;
}

/* get color players */
u8 Settings::PlayersColors(void) const
{
    return players_colors;
}

/* set game color players */
void Settings::SetPlayersColors(u8 c)
{
    players_colors = c;
}

void Settings::SetPreferablyCountPlayers(u8 c)
{
    preferably_count_players = 6 < c ? 6 : c;
}

u8 Settings::PreferablyCountPlayers(void) const
{
    return preferably_count_players;
}

void Settings::SetLocalPrefix(const char* str)
{
    if(str) local_prefix = str;
}

u16 Settings::GetPort(void) const
{
    return port;
}

Race::race_t Settings::KingdomRace(u8 color) const
{
    return Race::Get(current_maps_file.KingdomRace(color));
}

void Settings::SetKingdomRace(u8 color, u8 race)
{
    switch(color)
    {
        case Color::BLUE:       current_maps_file.races[0] = race; break;
        case Color::GREEN:      current_maps_file.races[1] = race; break;
        case Color::RED:        current_maps_file.races[2] = race; break;
        case Color::YELLOW:     current_maps_file.races[3] = race; break;
        case Color::ORANGE:     current_maps_file.races[4] = race; break;
        case Color::PURPLE:     current_maps_file.races[5] = race; break;
        default: break;
    }
}

const std::string & Settings::MapsFile(void) const
{
    return current_maps_file.file;
}

const std::string & Settings::MapsName(void) const
{
    return current_maps_file.name;
}

const std::string & Settings::MapsDescription(void) const
{
    return current_maps_file.description;
}

Difficulty::difficulty_t Settings::MapsDifficulty(void) const
{
    return Difficulty::Get(current_maps_file.difficulty);
}

u8 Settings::MapsWidth(void) const
{
    return current_maps_file.size_w;
}

bool Settings::AllowColors(u8 f) const
{
    return current_maps_file.allow_colors & f;
}

Color::color_t Settings::FirstAllowColor(void) const
{
    return Color::Get(Color::GetFirst(current_maps_file.allow_colors));
}

bool Settings::AllowChangeRace(u8 f) const
{
    return current_maps_file.rnd_races & f;
}

void Settings::SetKingdomColors(u8 c)
{
    current_maps_file.kingdom_colors = c;
}

u8 Settings::KingdomColors(void) const
{
    return current_maps_file.kingdom_colors;
}

bool Settings::KingdomColors(u8 f) const
{
    return current_maps_file.kingdom_colors & f;
}

u8 Settings::AllowColorsCount(void) const
{
    return current_maps_file.AllowColorsCount();
}

u8 Settings::KingdomColorsCount(void) const
{
    return current_maps_file.KingdomColorsCount();
}

bool Settings::MapsWithHeroes(void) const
{
    return current_maps_file.with_heroes;
}

GameOver::conditions_t Settings::ConditionWins(void) const
{
    return current_maps_file.ConditionWins();
}

GameOver::conditions_t Settings::ConditionLoss(void) const
{
    return current_maps_file.ConditionLoss();
}

bool Settings::WinsCompAlsoWins(void) const
{
    return current_maps_file.WinsCompAlsoWins();
}

bool Settings::WinsAllowNormalVictory(void) const
{
    return current_maps_file.WinsAllowNormalVictory();
}

Artifact::artifact_t Settings::WinsFindArtifact(void) const
{
    return current_maps_file.WinsFindArtifact();
}

bool Settings::WinsFindUltimateArtifact(void) const
{
    return current_maps_file.WinsFindUltimateArtifact();
}

u16 Settings::WinsSidePart(void) const
{
    return current_maps_file.WinsSidePart();
}

u32 Settings::WinsAccumulateGold(void) const
{
    return current_maps_file.WinsAccumulateGold();
}

u32 Settings::WinsMapsIndexObject(void) const
{
    return current_maps_file.WinsMapsIndexObject();
}

u32 Settings::LossMapsIndexObject(void) const
{
    return current_maps_file.LossMapsIndexObject();
}

u16 Settings::LossCountDays(void) const
{
    return current_maps_file.LossCountDays();
}

void Settings::FixKingdomRandomRace(void)
{
    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(KingdomColors(color))
    {
        if(Race::RAND == KingdomRace(color)) SetKingdomRace(color, Race::Rand());
        if(Debug()) Error::Verbose(Color::String(color) + ": " + Race::String(KingdomRace(color)));
    }
}
