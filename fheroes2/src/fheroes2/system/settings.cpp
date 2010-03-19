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

#include <fstream>
#include <bitset>
#include "maps.h"
#include "settings.h"

#define DEFAULT_PORT	5154
#define DEFAULT_DEBUG	(DBG_ENGINE | DBG_GAME | DBG_BATTLE | DBG_AI | DBG_NETWORK | DBG_WARN)

namespace
{
    enum ns_t { NS_UNKNOWN, NS_BATTLE, NS_NETWORK, NS_POCKETPC, NS_GLOBAL, NS_FHEROES2 };

    struct ModeSetting
    {
	const ns_t ns;    // Name space
        const char *name; // Name of config setting
        u32 value;        // Mode value to set
    } modeSettings[] =
    {
        { NS_GLOBAL,   "sound",                Settings::SOUND             },
        { NS_GLOBAL,   "music",                Settings::MUSIC_MIDI        },
        { NS_GLOBAL,   "fullscreen",           Settings::FULLSCREEN        },
        { NS_GLOBAL,   "full screen",          Settings::FULLSCREEN        },
        { NS_GLOBAL,   "hide interface",       Settings::HIDEINTERFACE     },
        { NS_GLOBAL,   "evil interface",       Settings::EVILINTERFACE     },
        { NS_GLOBAL,   "evilinterface",        Settings::EVILINTERFACE     },
        { NS_GLOBAL,   "fade",                 Settings::FADE              },
        { NS_GLOBAL,   "logo",                 Settings::LOGO              },
        { NS_GLOBAL,   "unicode",              Settings::USEUNICODE        },
        { NS_GLOBAL,   "autosave",             Settings::AUTOSAVE          },
        { NS_GLOBAL,   "alt resource",         Settings::ALTRESOURCE       },
        { NS_GLOBAL,   "alternative resource", Settings::ALTRESOURCE       },
        { NS_GLOBAL,   "hide ai move",         Settings::HIDEAIMOVE        },
        { NS_GLOBAL,   "system info",          Settings::SHOWSYSTEM        },

        { NS_BATTLE,   "battleauto",           Settings::BATTLEAUTO        },
        { NS_BATTLE,   "battle auto",          Settings::BATTLEAUTO        },
        { NS_BATTLE,   "battlegrid",           Settings::BATTLEGRID        },
        { NS_BATTLE,   "battle grid",          Settings::BATTLEGRID        },
        { NS_BATTLE,   "battlemoveshadow",     Settings::BATTLEMOVESHADOW  },
        { NS_BATTLE,   "battle move shadow",   Settings::BATTLEMOVESHADOW  },
        { NS_BATTLE,   "battlemouseshadow",    Settings::BATTLEMOUSESHADOW },
        { NS_BATTLE,   "battle mouse shadow",  Settings::BATTLEMOUSESHADOW },

        { NS_POCKETPC, "pocketpc",             Settings::POCKETPC          },
        { NS_POCKETPC, "pocket pc",            Settings::POCKETPC          },
        { NS_POCKETPC, "lowmemory",            Settings::LOWMEMORY         },
        { NS_POCKETPC, "low memory",           Settings::LOWMEMORY         },
        { NS_POCKETPC, "tapmode",              Settings::TAPMODE           },
        { NS_POCKETPC, "tap mode",             Settings::TAPMODE           },

	{ NS_FHEROES2, "allow buy from well",	Settings::ALLOW_BUY_FROM_WELL },
	{ NS_FHEROES2, "show visited content",	Settings::SHOW_VISITED_CONTENT },
	{ NS_FHEROES2, "fast load game dialog",	Settings::FAST_LOAD_GAME_DIALOG },
	{ NS_FHEROES2, "remember last focus",	Settings::REMEMBER_LAST_FOCUS },
	{ NS_FHEROES2, "abandoned mine random",	Settings::ABANDONED_MINE_RANDOM },
	{ NS_FHEROES2, "save monster after battle", Settings::SAVE_MONSTER_BATTLE },
	{ NS_FHEROES2, "allow set guardian (future)", Settings::ALLOW_SET_GUARDIAN },
	{ NS_FHEROES2, "learn spells with day",  Settings::LEARN_SPELLS_WITH_DAY },
	{ NS_FHEROES2, "battle show damage",    Settings::BATTLE_SHOW_DAMAGE },
	{ NS_FHEROES2, "battle troop direction",Settings::BATTLE_TROOP_DIRECTION },

        { NS_UNKNOWN,  NULL,                   0                           },
    };
};

/* constructor */
Settings::Settings() : major_version(MAJOR_VERSION), minor_version(MINOR_VERSION),
#ifdef SVN_REVISION
    svn_version(SVN_REVISION),
#endif
    debug(DEFAULT_DEBUG), video_mode(640, 480), game_difficulty(Difficulty::NORMAL),
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

    opt_global.SetModes(FADE);
    opt_global.SetModes(LOGO);

    opt_global.SetModes(SHOWRADAR);
    opt_global.SetModes(SHOWICONS);
    opt_global.SetModes(SHOWBUTTONS);
    opt_global.SetModes(SHOWSTATUS);
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
	    std::string left(str.substr(0, pos));
	    std::string right(str.substr(pos + 1, str.length() - pos - 1));

	    String::Trim(left);
	    String::Trim(right);

	    std::string lower_right(right);

	    String::Lower(left);
	    String::Lower(lower_right);
	    
	    if(lower_right == "on")
		SetStrModes(left);
	    else
	    if(lower_right == "off")
		ResetStrModes(left);
	    else
		Parse(left, right);
	}
    }

    file.close();

    // remove dublicate dir
    list_maps_directory.sort();
    list_maps_directory.unique();

#ifndef WITH_TTF
    opt_global.ResetModes(USEUNICODE);
#endif

    if(font_normal.empty() || font_small.empty()) opt_global.ResetModes(USEUNICODE);

#ifdef BUILD_RELEASE
    debug &= 0x000F;
    debug |= (DBG_ENGINE | DBG_GAME | DBG_BATTLE | DBG_AI | DBG_NETWORK);
#endif

    if(video_mode.w < 640 || video_mode.h < 480)
    {
	opt_pocket.SetModes(POCKETPC);
    }
    else
    {
        opt_pocket.ResetModes(POCKETPC);
    }

    if(opt_pocket.Modes(POCKETPC))
    {
	opt_global.SetModes(HIDEINTERFACE);
	opt_pocket.SetModes(TAPMODE);
	//opt_pocket.SetModes(LOWMEMORY);
	opt_battle.SetModes(BATTLEGRID);
	opt_battle.SetModes(BATTLEMOUSESHADOW);
	opt_battle.SetModes(BATTLEMOVESHADOW);
    }

    if(opt_global.Modes(HIDEINTERFACE))
    {
       opt_global.SetModes(SHOWCPANEL);
       opt_global.ResetModes(SHOWRADAR);
       opt_global.ResetModes(SHOWICONS);
       opt_global.ResetModes(SHOWBUTTONS);
       opt_global.ResetModes(SHOWSTATUS);
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
    stream << "sound = " << (opt_global.Modes(SOUND) ? "on"  : "off") << std::endl;
    stream << "music = " << (opt_global.Modes(MUSIC_CD) ? "cd" : (opt_global.Modes(MUSIC_MIDI) ? "midi" : (opt_global.Modes(MUSIC_EXT) ? "ext" : "off"))) << std::endl;
    stream << "sound volume = " << static_cast<int>(sound_volume) << std::endl;
    stream << "music volume = " << static_cast<int>(music_volume) << std::endl;
    stream << "animation = " << static_cast<int>(animation) << std::endl;
    stream << "fullscreen = " << (opt_global.Modes(FULLSCREEN) ? "on"  : "off") << std::endl;
    stream << "hide interface = " << (opt_global.Modes(HIDEINTERFACE) ? "on"  : "off") << std::endl;
    stream << "evil interface = " << (opt_global.Modes(EVILINTERFACE) ? "on"  : "off") << std::endl;
    stream << "fade = " << (opt_global.Modes(FADE) ? "on"  : "off") << std::endl;
    stream << "alt resource = " << (opt_global.Modes(ALTRESOURCE) ? "on"  : "off") << std::endl;
    stream << "debug = " << (debug ? "on"  : "off") << std::endl;

    stream << "battle auto = " << (opt_battle.Modes(BATTLEAUTO) ? "on" : "off") << std::endl;
    stream << "battle grid = " << (opt_battle.Modes(BATTLEGRID) ? "on" : "off") << std::endl;
    stream << "battle movement shadow = " << (opt_battle.Modes(BATTLEMOVESHADOW) ? "on" : "off") << std::endl;
    stream << "battle mouse shadow = " << (opt_battle.Modes(BATTLEMOUSESHADOW) ? "on" : "off") << std::endl;

    if(opt_global.Modes(HIDEAIMOVE))
    stream << "hide ai move = on" << std::endl;

#ifdef WITH_TTF
    stream << "fonts normal = " << font_normal << std::endl;
    stream << "fonts small = " << font_small << std::endl;
    if(force_lang.size())
    stream << "force lang = " << force_lang << std::endl;
    stream << "fonts normal size = " << static_cast<int>(size_normal) << std::endl;
    stream << "fonts small size = " << static_cast<int>(size_small) << std::endl;
    stream << "unicode = " << (opt_global.Modes(USEUNICODE) ? "on" : "off") << std::endl;
#endif

#ifndef WITH_MIXER
    stream << "playmus command = " << playmus_command << std::endl;
#endif

    stream << "autosave = " << (opt_global.Modes(AUTOSAVE) ? "on" : "off") << std::endl;
    stream << "port = " << port << std::endl;

    if(opt_pocket.Modes(TAPMODE))
    stream << "tap mode = on" << std::endl;
    if(opt_pocket.Modes(LOWMEMORY))
    stream << "low memory = on" << std::endl;
    if(opt_pocket.Modes(POCKETPC))
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
u16 Settings::Debug(void) const { return debug; }

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
bool Settings::FontsRenderBlended(void) const { return opt_global.Modes(FONTRENDERBLENDED); }

const std::string & Settings::BuildVersion(void) const { return build_version; }

/* return path to data directory */
const std::string & Settings::DataDirectory(void) const { return path_data_directory; }

/* return path to maps directory */
const ListMapsDirectory & Settings::GetListMapsDirectory(void) const { return list_maps_directory; }

/* return path to locales directory */
const std::string & Settings::LocalPrefix(void) const { return local_prefix; }
const std::string & Settings::PlayMusCommand(void) const { return playmus_command; };

/* return editor */
bool Settings::Editor(void) const { return opt_global.Modes(EDITOR); }

/* return sound */
bool Settings::Sound(void) const { return opt_global.Modes(SOUND); }

/* return music */
bool Settings::Music(void) const { return opt_global.Modes(MUSIC); }

bool Settings::CDMusic(void) const { return opt_global.Modes(MUSIC_CD | MUSIC_EXT); }

/* return animation */
u8   Settings::Animation(void) const { return animation; }

/* return full screen */
bool Settings::FullScreen(void) const { return opt_global.Modes(FULLSCREEN); }

/* return evil interface */
bool Settings::EvilInterface(void) const { return opt_global.Modes(EVILINTERFACE); }

bool Settings::HideInterface(void) const { return opt_global.Modes(HIDEINTERFACE); }

bool Settings::UseAltResource(void) const { return opt_global.Modes(ALTRESOURCE); }
bool Settings::PriceLoyaltyVersion(void) const { return opt_global.Modes(PRICELOYALTY); }
bool Settings::LoadedGameVersion(void) const { return opt_global.Modes(LOADGAME); }
bool Settings::UseFade(void) const { return opt_global.Modes(FADE); }
bool Settings::AutoSave(void) const { return opt_global.Modes(AUTOSAVE); }

bool Settings::ShowControlPanel(void) const { return opt_global.Modes(SHOWCPANEL); }
bool Settings::ShowRadar(void) const { return opt_global.Modes(SHOWRADAR); }
bool Settings::ShowIcons(void) const { return opt_global.Modes(SHOWICONS); }
bool Settings::ShowButtons(void) const { return opt_global.Modes(SHOWBUTTONS); }
bool Settings::ShowStatus(void) const { return opt_global.Modes(SHOWSTATUS); }
bool Settings::ShowSystem(void) const { return opt_global.Modes(SHOWSYSTEM); }

/* get show logo */
bool Settings::Logo(void) const { return opt_global.Modes(LOGO); }

bool Settings::BattleAuto(void) const { return opt_battle.Modes(BATTLEAUTO); }

/* battle grid */
bool Settings::BattleGrid(void) const { return opt_battle.Modes(BATTLEGRID); }

/* battle shaded grid */
bool Settings::BattleMovementShaded(void) const { return opt_battle.Modes(BATTLEMOVESHADOW); }

/* battle shaded mouse */
bool Settings::BattleMouseShaded(void) const { return opt_battle.Modes(BATTLEMOUSESHADOW); }

/* unicode support */
bool Settings::Unicode(void) const { return opt_global.Modes(USEUNICODE); }

bool Settings::PocketPC(void) const { return opt_pocket.Modes(POCKETPC); }
bool Settings::LowMemory(void) const { return opt_pocket.Modes(LOWMEMORY); }
bool Settings::TapMode(void) const { return opt_pocket.Modes(TAPMODE); }
bool Settings::HideAIMove(void) const { return opt_global.Modes(HIDEAIMOVE); }

bool Settings::NetworkDedicatedServer(void) const { return opt_global.Modes(DEDICATEDSERVER); }
bool Settings::NetworkLocalClient(void) const { return opt_global.Modes(LOCALCLIENT); }

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
    if(left == "fonts render" && right == "blended") opt_global.SetModes(FONTRENDERBLENDED);
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
            opt_global.ResetModes(MUSIC);
            opt_global.SetModes(MUSIC_MIDI);
        }
        else if(lower == "cd")
        {
            opt_global.ResetModes(MUSIC);
            opt_global.SetModes(MUSIC_CD);
        }
        else if(lower == "ext")
        {
            opt_global.ResetModes(MUSIC);
            opt_global.SetModes(MUSIC_EXT);
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

		DEBUG(DBG_ENGINE , DBG_INFO, "Settings::Parse: videomode: " << video_mode.w << "x" << video_mode.h << ", current: " << possible_w << "x" << possible_h);
		video_mode.w = possible_w;
		video_mode.h = possible_h;
	    }
	}
	else DEBUG(DBG_ENGINE , DBG_WARN, "Settings::Parse: unknown video mode: " << right);
    }
    // offset mouse
    else
    if(left == "pointer offset x") LocalEvent::Get().SetMouseOffsetX(String::ToInt(right));
    else
    if(left == "pointer offset y") LocalEvent::Get().SetMouseOffsetY(String::ToInt(right));
    // tap delay for right click emulation
    else
    if(left == "tap delay") LocalEvent::Get().SetTapDelayForRightClickEmulation(String::ToInt(right));
#ifdef WITH_KEYMAPPING
    else
    // load virtual key map
    if(4 < left.size() && 1 < right.size() && left.substr(0, 4) == "key_")
    {
	int code = String::ToInt(left.substr(4));
	DEBUG(DBG_ENGINE, DBG_INFO, "Settings::Parse: " << left << ", set virtual key: " << code << ", to: " << KeySymFromChar(right[1]));
	LocalEvent::Get().SetVirtualKey(code, KeySymFromChar(right[1]));
    }
#endif
}

/* set level debug */
void Settings::SetDebug(const u16 d)
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
    if(key == "debug") debug = DEFAULT_DEBUG;
    else
    {
	ModeSetting* ptr = modeSettings;
	while(NS_UNKNOWN != ptr->ns && key != ptr->name) ++ptr;

	switch(ptr->ns)
        {
	    case NS_BATTLE:   opt_battle.SetModes(ptr->value); break;
	    case NS_NETWORK:  opt_global.SetModes(ptr->value); break;
	    case NS_POCKETPC: opt_pocket.SetModes(ptr->value); break;
	    case NS_GLOBAL:   opt_global.SetModes(ptr->value); break;
	    case NS_FHEROES2: opt_fheroes2.SetModes(ptr->value); break;
	    default: break;
	}
    }
}

void Settings::ResetStrModes(const std::string & key)
{
    // debug
    if(key == "debug")		debug = 0;
    else
    {
	ModeSetting* ptr = modeSettings;
	while(NS_UNKNOWN != ptr->ns && key != ptr->name) ++ptr;

	switch(ptr->ns)
        {
	    case NS_BATTLE:   opt_battle.ResetModes(ptr->value); break;
	    case NS_NETWORK:  opt_global.ResetModes(ptr->value); break;
	    case NS_POCKETPC: opt_pocket.ResetModes(ptr->value); break;
	    case NS_GLOBAL:   opt_global.ResetModes(ptr->value); break;
	    default: break;
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

bool Settings::GameStartWithHeroes(void) const
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
        DEBUG(DBG_GAME , DBG_INFO, "Settings::FixKingdomRandomRace: " << Color::String(color) << ": " << Race::String(KingdomRace(color)));
    }
}

void Settings::SetEditor(void)
{
    opt_global.SetModes(EDITOR);
}

void Settings::SetUnicode(bool f)
{
    f ? opt_global.SetModes(USEUNICODE) : opt_global.ResetModes(USEUNICODE);
}

void Settings::SetPriceLoyaltyVersion(void)
{
    opt_global.SetModes(PRICELOYALTY);
}

u32 Settings::GetMusicType(void) const
{
    if(opt_global.Modes(MUSIC_EXT)) return MUSIC_EXT;
    else
    if(opt_global.Modes(MUSIC_CD)) return MUSIC_CD;
    else
    if(opt_global.Modes(MUSIC_MIDI)) return MUSIC_MIDI;

    return 0;
}

void Settings::SetEvilInterface(bool f)
{
    f ? opt_global.SetModes(EVILINTERFACE) : opt_global.ResetModes(EVILINTERFACE);
}

void Settings::SetBattleAuto(bool f)
{
    f ? opt_battle.SetModes(BATTLEAUTO) : opt_battle.ResetModes(BATTLEAUTO);
}

void Settings::SetBattleGrid(bool f)
{
    f ? opt_battle.SetModes(BATTLEGRID) : opt_battle.ResetModes(BATTLEGRID);
}

void Settings::SetBattleMovementShaded(bool f)
{
    f ? opt_battle.SetModes(BATTLEMOVESHADOW) : opt_battle.ResetModes(BATTLEMOVESHADOW);
}

void Settings::SetBattleMouseShaded(bool f)
{
    f ? opt_battle.SetModes(BATTLEMOUSESHADOW) : opt_battle.ResetModes(BATTLEMOUSESHADOW);
}

void Settings::ResetSound(void)
{
    opt_global.ResetModes(SOUND);
}

void Settings::ResetMusic(void)
{
    opt_global.ResetModes(MUSIC);
}

void Settings::SetLoadedGameVersion(bool f)
{
    f ? opt_global.SetModes(LOADGAME) : opt_global.ResetModes(LOADGAME);
}

void Settings::SetShowPanel(bool f)
{
    f ? opt_global.SetModes(SHOWCPANEL) : opt_global.ResetModes(SHOWCPANEL);
}

void Settings::SetShowRadar(bool f)
{
    f ? opt_global.SetModes(SHOWRADAR) : opt_global.ResetModes(SHOWRADAR);
}

void Settings::SetShowIcons(bool f)
{
    f ? opt_global.SetModes(SHOWICONS) : opt_global.ResetModes(SHOWICONS);
}

void Settings::SetShowButtons(bool f)
{
    f ? opt_global.SetModes(SHOWBUTTONS) : opt_global.ResetModes(SHOWBUTTONS);
}

void Settings::SetShowStatus(bool f)
{
    f ? opt_global.SetModes(SHOWSTATUS) : opt_global.ResetModes(SHOWSTATUS);
}

void Settings::SetNetworkLocalClient(bool f)
{
    f ? opt_global.SetModes(LOCALCLIENT) : opt_global.ResetModes(LOCALCLIENT);
}

void Settings::SetNetworkDedicatedServer(bool f)
{
    f ? opt_global.SetModes(DEDICATEDSERVER) : opt_global.ResetModes(DEDICATEDSERVER);
}

bool Settings::ExtModes(u32 f) const
{
    return opt_fheroes2.Modes(f);
}

const char* Settings::ExtName(u32 f) const
{
    ModeSetting* ptr = modeSettings;
    while(NS_UNKNOWN != ptr->ns) if(NS_FHEROES2 == ptr->ns && ptr->value == f) return ptr->name; else ++ptr;
    return NULL;
}

void Settings::ExtSetModes(u32 f)
{
    opt_fheroes2.SetModes(f);
}

void Settings::ExtResetModes(u32 f)
{
    opt_fheroes2.ResetModes(f);
}

bool Settings::ExtAllowBuyFromWell(void) const
{
    return opt_fheroes2.Modes(ALLOW_BUY_FROM_WELL);
}

bool Settings::ExtShowVisitedContent(void) const
{
    return opt_fheroes2.Modes(SHOW_VISITED_CONTENT);
}

bool Settings::ExtFastLoadGameDialog(void) const
{
    return opt_fheroes2.Modes(FAST_LOAD_GAME_DIALOG);
}

bool Settings::ExtRememberLastFocus(void) const
{
    return opt_fheroes2.Modes(REMEMBER_LAST_FOCUS);
}

bool Settings::ExtAbandonedMineRandom(void) const
{
    return opt_fheroes2.Modes(ABANDONED_MINE_RANDOM);
}

bool Settings::ExtSaveMonsterBattle(void) const
{
    return opt_fheroes2.Modes(SAVE_MONSTER_BATTLE);
}

bool Settings::ExtAllowSetGuardian(void) const
{
    return opt_fheroes2.Modes(ALLOW_SET_GUARDIAN);
}

bool Settings::ExtLearnSpellsWithDay(void) const
{
    return opt_fheroes2.Modes(LEARN_SPELLS_WITH_DAY);
}

bool Settings::ExtBattleShowDamage(void) const
{
    return opt_fheroes2.Modes(BATTLE_SHOW_DAMAGE);
}

bool Settings::ExtBattleTroopDirection(void) const
{
    return opt_fheroes2.Modes(BATTLE_TROOP_DIRECTION);
}
