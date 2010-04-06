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
#include <fstream>
#include <bitset>
#include "maps.h"
#include "settings.h"

#define DEFAULT_PORT	5154
#define DEFAULT_DEBUG	(DBG_ENGINE | DBG_GAME | DBG_BATTLE | DBG_AI | DBG_NETWORK | DBG_WARN)

enum
{
    GLOBAL_LOADGAME          = 0x00000001,
    GLOBAL_PRICELOYALTY      = 0x00000004,

    GLOBAL_POCKETPC          = 0x00000010,
    GLOBAL_DEDICATEDSERVER   = 0x00000020,
    GLOBAL_LOCALCLIENT       = 0x00000040,

    GLOBAL_SHOWCPANEL        = 0x00000100,
    GLOBAL_SHOWRADAR         = 0x00000200,
    GLOBAL_SHOWICONS         = 0x00000400,
    GLOBAL_SHOWBUTTONS       = 0x00000800,
    GLOBAL_SHOWSTATUS        = 0x00001000,

    GLOBAL_AUTOBATTLE        = 0x00010000,

    GLOBAL_FONTRENDERBLENDED = 0x00020000,
    GLOBAL_FULLSCREEN        = 0x00400000,

    GLOBAL_SOUND             = 0x01000000,
    GLOBAL_MUSIC_EXT         = 0x02000000,
    GLOBAL_MUSIC_CD          = 0x04000000,
    GLOBAL_MUSIC_MIDI        = 0x08000000,

    GLOBAL_EDITOR            = 0x20000000,
    GLOBAL_USEUNICODE        = 0x40000000,
    GLOBAL_ALTRESOURCE       = 0x80000000,

    GLOBAL_MUSIC           = GLOBAL_MUSIC_CD | GLOBAL_MUSIC_EXT | GLOBAL_MUSIC_MIDI
};

struct settings_t
{
    u32 id;
    const char* str;

    bool operator== (const std::string & s) const { return str && s == str; };
    bool operator== (u32 i) const { return id && id == i; };
};

// external settings
static const settings_t settingsGeneral[] =
{
    { GLOBAL_SOUND,       "sound",        },
    { GLOBAL_MUSIC_MIDI,  "music",        },
    { GLOBAL_FULLSCREEN,  "fullscreen",   },
    { GLOBAL_FULLSCREEN,  "full screen",  },
    { GLOBAL_USEUNICODE,  "unicode",      },
    { GLOBAL_ALTRESOURCE, "alt resource", },
    { GLOBAL_POCKETPC,    "pocketpc",     },
    { GLOBAL_POCKETPC,    "pocket pc",    },

    { 0, NULL, },
};

// internal settings
static const settings_t settingsFHeroes2[] =
{
    { Settings::GAME_SAVE_REWRITE_CONFIRM,	_("game: always confirm for rewrite savefile"),		},
    { Settings::GAME_ALSO_CONFIRM_AUTOSAVE,	_("game: also confirm autosave"),			},
    { Settings::GAME_FAST_LOAD_GAME_DIALOG,	_("game: fast load game dialog (L hot key)"),		},
    { Settings::GAME_REMEMBER_LAST_FOCUS,	_("game: remember last focus"),				},
    { Settings::GAME_HIDE_AI_MOVE,		_("game: hide AI move"),				},
    { Settings::GAME_REMEMBER_LAST_FILENAME,	_("game: remember last filename"),			},
    { Settings::WORLD_SHOW_VISITED_CONTENT,	_("world: show visited content from objects"),		},
    { Settings::WORLD_ABANDONED_MINE_RANDOM,	_("world: abandoned mine random resource"),		},
    { Settings::WORLD_SAVE_MONSTER_BATTLE,	_("world: save count monster after battle"),		},
    { Settings::WORLD_ALLOW_SET_GUARDIAN,	_("world: allow set guardian to objects"),		},
    { Settings::WORLD_NOREQ_FOR_ARTIFACTS,	_("world: no in-built requirements or guardians for placed artifacts"),	},
    { Settings::CASTLE_ALLOW_BUY_FROM_WELL,	_("castle: allow buy from well"),			},
    { Settings::HEROES_LEARN_SPELLS_WITH_DAY,	_("heroes: learn new spells with day"),  		},
    { Settings::BATTLE_SHOW_DAMAGE,		_("battle: show damage info"),    			},
    { Settings::BATTLE_TROOP_DIRECTION,		_("battle: troop direction to move"),			},
    { Settings::BATTLE_SOFT_WAITING,		_("battle: soft wait troop"),				},
    { Settings::BATTLE_SHOW_GRID,		_("battle: show grid"),					},
    { Settings::BATTLE_SHOW_MOUSE_SHADOW,	_("battle: show mouse shadow"),				},
    { Settings::BATTLE_SHOW_MOVE_SHADOW,	_("battle: show move shadow"),				},
    { Settings::BATTLE_OBJECTS_ARCHERS_PENALTY, _("battle: high objects are an obstacle for archers"),  },
    { Settings::BATTLE_MERGE_ARMIES, 		_("battle: merge armies for hero from castle"),  	},
    { Settings::BATTLE_ARCHMAGE_RESIST_BAD_SPELL,_("battle: archmage can resists (20%) bad spells"),     },
    { Settings::BATTLE_MAGIC_TROOP_RESIST,	_("battle: magical creature resists (20%) the same magic"),},
    { Settings::GAME_SHOW_SYSTEM_INFO,		_("game: show system info"),				},
    { Settings::GAME_AUTOSAVE_ON,		_("game: autosave on"),					},
    { Settings::GAME_USE_FADE,			_("game: use fade"),					},
    { Settings::GAME_SHOW_SDL_LOGO,		_("game: show SDL logo"),				},
    { Settings::GAME_EVIL_INTERFACE,		_("game: use evil interface"),				},
    { Settings::GAME_HIDE_INTERFACE,		_("game: hide interface"),				},
    { Settings::POCKETPC_HIDE_CURSOR,		_("pocketpc: hide cursor"),				},
    { Settings::POCKETPC_TAP_MODE,		_("pocketpc: tap mode"),				},
    { Settings::POCKETPC_LOW_MEMORY,		_("pocketpc: low memory"),				},

    { 0, NULL },
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
    port(DEFAULT_PORT), memory_limit(0)
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

    ExtSetModes(GAME_SHOW_SDL_LOGO);
    ExtSetModes(GAME_AUTOSAVE_ON);

    opt_global.SetModes(GLOBAL_SHOWRADAR);
    opt_global.SetModes(GLOBAL_SHOWICONS);
    opt_global.SetModes(GLOBAL_SHOWBUTTONS);
    opt_global.SetModes(GLOBAL_SHOWSTATUS);
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
	    {
		if(left == "debug") debug = DEFAULT_DEBUG;
		else
		{
		    const settings_t* ptr = std::find(settingsGeneral,
				settingsGeneral + sizeof(settingsGeneral) / sizeof(settings_t) - 1, left);
		    if(ptr->str) opt_global.SetModes(ptr->id);
		}
	    }
	    else
	    if(lower_right == "off")
	    {
		if(left == "debug") debug = DEFAULT_DEBUG;
		else
		{
		    const settings_t* ptr = std::find(settingsGeneral,
				settingsGeneral + sizeof(settingsGeneral) / sizeof(settings_t) - 1, left);
		    if(ptr->str) opt_global.ResetModes(ptr->id);
		}
	    }

	    Parse(left, right);
	}
    }

    file.close();

    // remove dublicate dir
    list_maps_directory.sort();
    list_maps_directory.unique();

#ifndef WITH_TTF
    opt_global.ResetModes(GLOBAL_USEUNICODE);
#endif

    if(font_normal.empty() || font_small.empty()) opt_global.ResetModes(GLOBAL_USEUNICODE);

#ifdef BUILD_RELEASE
    debug &= 0x000F;
    debug |= (DBG_ENGINE | DBG_GAME | DBG_BATTLE | DBG_AI | DBG_NETWORK);
#endif

    BinaryLoad();

    if(QVGA())
    {
	opt_global.SetModes(GLOBAL_POCKETPC);
	ExtSetModes(GAME_HIDE_INTERFACE);
    }

    if(! opt_global.Modes(GLOBAL_POCKETPC))
    {
	ExtResetModes(POCKETPC_HIDE_CURSOR);
	ExtResetModes(POCKETPC_TAP_MODE);
	ExtResetModes(POCKETPC_LOW_MEMORY);
    }

    if(ExtModes(GAME_HIDE_INTERFACE))
    {
       opt_global.SetModes(GLOBAL_SHOWCPANEL);
       opt_global.ResetModes(GLOBAL_SHOWRADAR);
       opt_global.ResetModes(GLOBAL_SHOWICONS);
       opt_global.ResetModes(GLOBAL_SHOWBUTTONS);
       opt_global.ResetModes(GLOBAL_SHOWSTATUS);
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
    stream << "sound = " << (opt_global.Modes(GLOBAL_SOUND) ? "on"  : "off") << std::endl;
    stream << "music = " << (opt_global.Modes(GLOBAL_MUSIC_CD) ? "cd" : (opt_global.Modes(GLOBAL_MUSIC_MIDI) ? "midi" : (opt_global.Modes(GLOBAL_MUSIC_EXT) ? "ext" : "off"))) << std::endl;
    stream << "sound volume = " << static_cast<int>(sound_volume) << std::endl;
    stream << "music volume = " << static_cast<int>(music_volume) << std::endl;
    stream << "animation = " << static_cast<int>(animation) << std::endl;
    stream << "fullscreen = " << (opt_global.Modes(GLOBAL_FULLSCREEN) ? "on"  : "off") << std::endl;
    stream << "alt resource = " << (opt_global.Modes(GLOBAL_ALTRESOURCE) ? "on"  : "off") << std::endl;
    stream << "debug = " << (debug ? "on"  : "off") << std::endl;

#ifdef WITH_TTF
    stream << "fonts normal = " << font_normal << std::endl;
    stream << "fonts small = " << font_small << std::endl;
    if(force_lang.size())
    stream << "force lang = " << force_lang << std::endl;
    stream << "fonts normal size = " << static_cast<int>(size_normal) << std::endl;
    stream << "fonts small size = " << static_cast<int>(size_small) << std::endl;
    stream << "unicode = " << (opt_global.Modes(GLOBAL_USEUNICODE) ? "on" : "off") << std::endl;
#endif

#ifndef WITH_MIXER
    stream << "playmus command = " << playmus_command << std::endl;
#endif

    stream << "port = " << port << std::endl;

    if(opt_global.Modes(GLOBAL_POCKETPC))
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
bool Settings::FontsRenderBlended(void) const { return opt_global.Modes(GLOBAL_FONTRENDERBLENDED); }

const std::string & Settings::BuildVersion(void) const { return build_version; }

/* return path to data directory */
const std::string & Settings::DataDirectory(void) const { return path_data_directory; }

/* return path to maps directory */
const ListMapsDirectory & Settings::GetListMapsDirectory(void) const { return list_maps_directory; }

/* return path to locales directory */
const std::string & Settings::LocalPrefix(void) const { return local_prefix; }
const std::string & Settings::PlayMusCommand(void) const { return playmus_command; }

bool Settings::MusicExt(void) const { return opt_global.Modes(GLOBAL_MUSIC_EXT); }
bool Settings::MusicMIDI(void) const { return opt_global.Modes(GLOBAL_MUSIC_MIDI); }
bool Settings::MusicCD(void) const { return opt_global.Modes(GLOBAL_MUSIC_CD); }

/* return editor */
bool Settings::Editor(void) const { return opt_global.Modes(GLOBAL_EDITOR); }

/* return sound */
bool Settings::Sound(void) const { return opt_global.Modes(GLOBAL_SOUND); }

/* return music */
bool Settings::Music(void) const { return opt_global.Modes(GLOBAL_MUSIC); }

bool Settings::CDMusic(void) const { return opt_global.Modes(GLOBAL_MUSIC_CD | GLOBAL_MUSIC_EXT); }

/* return animation */
u8   Settings::Animation(void) const { return animation; }

/* return full screen */
bool Settings::FullScreen(void) const { return opt_global.Modes(GLOBAL_FULLSCREEN); }

bool Settings::QVGA(void) const { return video_mode.w < 640 || video_mode.h < 480; }

bool Settings::UseAltResource(void) const { return opt_global.Modes(GLOBAL_ALTRESOURCE); }
bool Settings::PriceLoyaltyVersion(void) const { return opt_global.Modes(GLOBAL_PRICELOYALTY); }
bool Settings::LoadedGameVersion(void) const { return opt_global.Modes(GLOBAL_LOADGAME); }

bool Settings::ShowControlPanel(void) const { return opt_global.Modes(GLOBAL_SHOWCPANEL); }
bool Settings::ShowRadar(void) const { return opt_global.Modes(GLOBAL_SHOWRADAR); }
bool Settings::ShowIcons(void) const { return opt_global.Modes(GLOBAL_SHOWICONS); }
bool Settings::ShowButtons(void) const { return opt_global.Modes(GLOBAL_SHOWBUTTONS); }
bool Settings::ShowStatus(void) const { return opt_global.Modes(GLOBAL_SHOWSTATUS); }

/* unicode support */
bool Settings::Unicode(void) const { return opt_global.Modes(GLOBAL_USEUNICODE); }

bool Settings::PocketPC(void) const { return opt_global.Modes(GLOBAL_POCKETPC); }
bool Settings::NetworkDedicatedServer(void) const { return opt_global.Modes(GLOBAL_DEDICATEDSERVER); }
bool Settings::NetworkLocalClient(void) const { return opt_global.Modes(GLOBAL_LOCALCLIENT); }

/* get video mode */
const Size & Settings::VideoMode(void) const { return video_mode; }

void Settings::Parse(const std::string & left, const std::string & right)
{
    LocalEvent & le = LocalEvent::Get();

    // debug
    if(left == "debug") debug = String::ToInt(right);
    else
    //
    if(left == "memory limit") memory_limit = String::ToInt(right);
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
    if(left == "fonts render" && right == "blended") opt_global.SetModes(GLOBAL_FONTRENDERBLENDED);
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
            opt_global.ResetModes(GLOBAL_MUSIC);
            opt_global.SetModes(GLOBAL_MUSIC_MIDI);
        }
        else if(lower == "cd")
        {
            opt_global.ResetModes(GLOBAL_MUSIC);
            opt_global.SetModes(GLOBAL_MUSIC_CD);
        }
        else if(lower == "ext")
        {
            opt_global.ResetModes(GLOBAL_MUSIC);
            opt_global.SetModes(GLOBAL_MUSIC_EXT);
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

#ifdef __SYMBIAN32__
		if(possible_h == 224) possible_h = 240;
		else
		if(possible_h == 352) possible_h = 360;
#endif
		DEBUG(DBG_ENGINE , DBG_INFO, "Settings::Parse: videomode: " << video_mode.w << "x" << video_mode.h << ", current: " << possible_w << "x" << possible_h);
		video_mode.w = possible_w;
		video_mode.h = possible_h;
	    }
	}
	else DEBUG(DBG_ENGINE , DBG_WARN, "Settings::Parse: unknown video mode: " << right);
    }
    // offset mouse
    else
    if(left == "pointer offset x") le.SetMouseOffsetX(String::ToInt(right));
    else
    if(left == "pointer offset y") le.SetMouseOffsetY(String::ToInt(right));
    // tap delay for right click emulation
    else
    if(left == "tap delay") le.SetTapDelayForRightClickEmulation(String::ToInt(right));
#ifdef WITHOUT_MOUSE
    else
    if(left == "emulate mouse") le.SetEmulateMouse(right == "on");
    else
    if(left == "emulate mouse up") le.SetEmulateMouseUpKey(String::ToInt(right));
    else
    if(left == "emulate mouse down") le.SetEmulateMouseDownKey(String::ToInt(right));
    else
    if(left == "emulate mouse left") le.SetEmulateMouseLeftKey(String::ToInt(right));
    else
    if(left == "emulate mouse right") le.SetEmulateMouseRightKey(String::ToInt(right));
    else
    if(left == "emulate mouse step") le.SetEmulateMouseStep(String::ToInt(right));
    else
    if(left == "emulate press left") le.SetEmulatePressLeftKey(String::ToInt(right));
    else
    if(left == "emulate press right") le.SetEmulatePressRightKey(String::ToInt(right));
#endif
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

bool Settings::AutoBattle(void) const
{
    return opt_global.Modes(GLOBAL_AUTOBATTLE);
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
    opt_global.SetModes(GLOBAL_EDITOR);
}

void Settings::SetUnicode(bool f)
{
    f ? opt_global.SetModes(GLOBAL_USEUNICODE) : opt_global.ResetModes(GLOBAL_USEUNICODE);
}

void Settings::SetPriceLoyaltyVersion(void)
{
    opt_global.SetModes(GLOBAL_PRICELOYALTY);
}

void Settings::SetAutoBattle(bool f)
{
    f ? opt_global.SetModes(GLOBAL_AUTOBATTLE) : opt_global.ResetModes(GLOBAL_AUTOBATTLE);
}

void Settings::SetEvilInterface(bool f)
{
    f ? ExtSetModes(GAME_EVIL_INTERFACE) : ExtResetModes(GAME_EVIL_INTERFACE);
}

void Settings::SetBattleGrid(bool f)
{
    f ? ExtSetModes(BATTLE_SHOW_GRID) : ExtResetModes(BATTLE_SHOW_GRID);
}

void Settings::SetBattleMovementShaded(bool f)
{
    f ? ExtSetModes(BATTLE_SHOW_MOVE_SHADOW) : ExtResetModes(BATTLE_SHOW_MOVE_SHADOW);
}

void Settings::SetBattleMouseShaded(bool f)
{
    f ? ExtSetModes(BATTLE_SHOW_MOUSE_SHADOW) : ExtResetModes(BATTLE_SHOW_MOUSE_SHADOW);
}

void Settings::ResetSound(void)
{
    opt_global.ResetModes(GLOBAL_SOUND);
}

void Settings::ResetMusic(void)
{
    opt_global.ResetModes(GLOBAL_MUSIC);
}

void Settings::SetLoadedGameVersion(bool f)
{
    f ? opt_global.SetModes(GLOBAL_LOADGAME) : opt_global.ResetModes(GLOBAL_LOADGAME);
}

void Settings::SetShowPanel(bool f)
{
    f ? opt_global.SetModes(GLOBAL_SHOWCPANEL) : opt_global.ResetModes(GLOBAL_SHOWCPANEL);
}

void Settings::SetShowRadar(bool f)
{
    f ? opt_global.SetModes(GLOBAL_SHOWRADAR) : opt_global.ResetModes(GLOBAL_SHOWRADAR);
}

void Settings::SetShowIcons(bool f)
{
    f ? opt_global.SetModes(GLOBAL_SHOWICONS) : opt_global.ResetModes(GLOBAL_SHOWICONS);
}

void Settings::SetShowButtons(bool f)
{
    f ? opt_global.SetModes(GLOBAL_SHOWBUTTONS) : opt_global.ResetModes(GLOBAL_SHOWBUTTONS);
}

void Settings::SetShowStatus(bool f)
{
    f ? opt_global.SetModes(GLOBAL_SHOWSTATUS) : opt_global.ResetModes(GLOBAL_SHOWSTATUS);
}

void Settings::SetNetworkLocalClient(bool f)
{
    f ? opt_global.SetModes(GLOBAL_LOCALCLIENT) : opt_global.ResetModes(GLOBAL_LOCALCLIENT);
}

void Settings::SetNetworkDedicatedServer(bool f)
{
    f ? opt_global.SetModes(GLOBAL_DEDICATEDSERVER) : opt_global.ResetModes(GLOBAL_DEDICATEDSERVER);
}

bool Settings::ExtModes(u32 f) const
{
    const u32 mask = 0x0FFFFFFF;
    switch(f >> 28)
    {
	case 0x01: return opt_game.Modes(f & mask);
	case 0x02: return opt_world.Modes(f & mask);
	case 0x04: return opt_battle.Modes(f & mask);
	default: break;
    }
    return false;
}

const char* Settings::ExtName(u32 f) const
{
    const settings_t* ptr = std::find(settingsFHeroes2,
		settingsFHeroes2 + sizeof(settingsFHeroes2) / sizeof(settings_t) - 1, f);

    return ptr ? _(ptr->str) : NULL;
}

void Settings::ExtSetModes(u32 f)
{
    const u32 mask = 0x0FFFFFFF;
    switch(f >> 28)
    {
	case 0x01: opt_game.SetModes(f & mask); break;
	case 0x02: opt_world.SetModes(f & mask); break;
	case 0x04: opt_battle.SetModes(f & mask); break;
	default: break;
    }
}

void Settings::ExtResetModes(u32 f)
{
    const u32 mask = 0x0FFFFFFF;
    switch(f >> 28)
    {
	case 0x01: opt_game.ResetModes(f & mask); break;
	case 0x02: opt_world.ResetModes(f & mask); break;
	case 0x04: opt_battle.ResetModes(f & mask); break;
	default: break;
    }
}

bool Settings::ExtAllowBuyFromWell(void) const
{
    return ExtModes(CASTLE_ALLOW_BUY_FROM_WELL);
}

bool Settings::ExtShowVisitedContent(void) const
{
    return ExtModes(WORLD_SHOW_VISITED_CONTENT);
}

bool Settings::ExtFastLoadGameDialog(void) const
{
    return ExtModes(GAME_FAST_LOAD_GAME_DIALOG);
}

bool Settings::ExtRememberLastFocus(void) const
{
    return ExtModes(GAME_REMEMBER_LAST_FOCUS);
}

bool Settings::ExtAbandonedMineRandom(void) const
{
    return ExtModes(WORLD_ABANDONED_MINE_RANDOM);
}

bool Settings::ExtSaveMonsterBattle(void) const
{
    return ExtModes(WORLD_SAVE_MONSTER_BATTLE);
}

bool Settings::ExtAllowSetGuardian(void) const
{
    return ExtModes(WORLD_ALLOW_SET_GUARDIAN);
}

bool Settings::ExtNoRequirementsForArtifacts(void) const
{
    return ExtModes(WORLD_NOREQ_FOR_ARTIFACTS);
}

bool Settings::ExtLearnSpellsWithDay(void) const
{
    return ExtModes(HEROES_LEARN_SPELLS_WITH_DAY);
}

bool Settings::ExtBattleShowDamage(void) const
{
    return ExtModes(BATTLE_SHOW_DAMAGE);
}

bool Settings::ExtBattleTroopDirection(void) const
{
    return ExtModes(BATTLE_TROOP_DIRECTION);
}

bool Settings::ExtBattleSoftWait(void) const
{
    return ExtModes(BATTLE_SOFT_WAITING);
}

bool Settings::ExtBattleShowGrid(void) const
{
    return ExtModes(BATTLE_SHOW_GRID);
}

bool Settings::ExtBattleShowMouseShadow(void) const
{
    return ExtModes(BATTLE_SHOW_MOUSE_SHADOW);
}

bool Settings::ExtBattleShowMoveShadow(void) const
{
    return ExtModes(BATTLE_SHOW_MOVE_SHADOW);
}

bool Settings::ExtBattleObjectsArchersPenalty(void) const
{
    return ExtModes(BATTLE_OBJECTS_ARCHERS_PENALTY);
}

bool Settings::ExtBattleMergeArmies(void) const
{
    return ExtModes(BATTLE_MERGE_ARMIES);
}

bool Settings::ExtBattleArchmageCanResistBadMagic(void) const
{
    return ExtModes(BATTLE_ARCHMAGE_RESIST_BAD_SPELL);
}

bool Settings::ExtBattleMagicTroopCanResist(void) const
{
    return ExtModes(BATTLE_MAGIC_TROOP_RESIST);
}

bool Settings::ExtRewriteConfirm(void) const
{
    return ExtModes(GAME_SAVE_REWRITE_CONFIRM);
}

bool Settings::ExtAutosaveConfirm(void) const
{
    return ExtModes(GAME_ALSO_CONFIRM_AUTOSAVE);
}

bool Settings::ExtHideCursor(void) const
{
    return ExtModes(POCKETPC_HIDE_CURSOR);
}

bool Settings::ExtHideAIMove(void) const
{
    return ExtModes(GAME_HIDE_AI_MOVE);
}

bool Settings::ExtShowSystemInfo(void) const
{
    return ExtModes(GAME_SHOW_SYSTEM_INFO);
}

bool Settings::ExtAutoSaveOn(void) const
{
    return ExtModes(GAME_AUTOSAVE_ON);
}

bool Settings::ExtRememberLastFilename(void) const
{
    return ExtModes(GAME_REMEMBER_LAST_FILENAME);
}

bool Settings::ExtUseFade(void) const
{
    return ExtModes(GAME_USE_FADE);
}

bool Settings::ExtShowSDL(void) const
{
    return ExtModes(GAME_SHOW_SDL_LOGO);
}

bool Settings::EvilInterface(void) const
{
    return ExtModes(GAME_EVIL_INTERFACE);
}

bool Settings::HideInterface(void) const
{
    return ExtModes(GAME_HIDE_INTERFACE);
}

bool Settings::ExtLowMemory(void) const
{
    return ExtModes(POCKETPC_LOW_MEMORY);
}

bool Settings::ExtTapMode(void) const
{
    return ExtModes(POCKETPC_TAP_MODE);
}

void Settings::BinarySave(void) const
{
    const std::string binary = local_prefix + SEPARATOR + "fheroes2.bin";
    QueueMessage msg;

    msg.Push(static_cast<u16>(CURRENT_FORMAT_VERSION));
    msg.Push(opt_game());
    msg.Push(opt_world());
    msg.Push(opt_battle());

    msg.Save(binary.c_str());
}

void Settings::BinaryLoad(void)
{
    const std::string binary = local_prefix + SEPARATOR + "fheroes2.bin";

    if(FilePresent(binary))
    {
	QueueMessage msg;
	u32 byte32;
	u16 version;

	msg.Load(binary.c_str());

	opt_game.ResetModes(MODES_ALL);
	opt_world.ResetModes(MODES_ALL);
	opt_battle.ResetModes(MODES_ALL);

	msg.Pop(version);

	msg.Pop(byte32);
	opt_game.SetModes(byte32);

	msg.Pop(byte32);
	opt_world.SetModes(byte32);

	msg.Pop(byte32);
	opt_battle.SetModes(byte32);
    }
}

void Settings::SetMemoryLimit(u32 limit)
{
    memory_limit = limit;
}

u32 Settings::MemoryLimit(void) const
{
    return memory_limit;
}
