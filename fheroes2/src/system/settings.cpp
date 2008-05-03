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
#include "tools.h"
#include "config.h"

/* constructor */
Settings::Settings() : major_version(MAJOR_VERSION), minor_version(MINOR_VERSION), build_date(BUILD_DATE),
    modes(ANIMATION | SHADOW | ORIGINAL | LOGO), debug(0), video_mode(640, 480), game_difficulty(Difficulty::NORMAL),
    my_color(Color::GRAY), path_data_directory("data"), path_maps_directory("maps"), sound_volume(10), music_volume(10),
    animation_speed(10), hotseat(false), players(Color::BLUE)
{
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
		SetModes(left);
	    else
	    if(right == "off")
		ResetModes(left);
	    else
		Parse(left, right);
	}
    }

    file.close();

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

    stream << "data = " << path_data_directory << std::endl;
    stream << "maps = " << path_maps_directory << std::endl;

    str.clear();
    String::AddInt(str, video_mode.w);
    str += "x";
    String::AddInt(str, video_mode.h);

    stream << "videomode = " << str << std::endl;
    stream << "sound = " << (modes & SOUND ? "on"  : "off") << std::endl;
    stream << "music = " << (modes & MUSIC ? "on"  : "off") << std::endl;
    stream << "fullscreen = " << (modes & FULLSCREEN ? "on"  : "off") << std::endl;
    stream << "evilinterface = " << (modes & EVILINTERFACE ? "on"  : "off") << std::endl;
    stream << "shadow = " << (modes & SHADOW ? "on"  : "off") << std::endl;
    stream << "original = " << (modes & ORIGINAL ? "on"  : "off") << std::endl;
    stream << "debug = " << (debug ? "on"  : "off") << std::endl;

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

Color::color_t Settings::MyColor(void) const { return my_color; }

bool Settings::Modes(const settings_t s) const { return modes & s; }

/* return path to data directory */
const std::string & Settings::DataDirectory(void) const { return path_data_directory; }

/* return path to maps directory */
const std::string & Settings::MapsDirectory(void) const { return path_maps_directory; }

/* return editor */
bool Settings::Editor(void) const { return modes & EDITOR; }

/* return original version */
bool Settings::Original(void) const { return modes & ORIGINAL; }

/* return sound */
bool Settings::Sound(void) const { return modes & SOUND; }

/* return music */
bool Settings::Music(void) const { return modes & MUSIC; }

/* return animation */
bool Settings::Animation(void) const { return modes & ANIMATION; }

/* return full screen */
bool Settings::FullScreen(void) const { return modes & FULLSCREEN; }

/* return evil interface */
bool Settings::EvilInterface(void) const { return modes & EVILINTERFACE; }

/* return shadow */
bool Settings::Shadow(void) const { return modes & SHADOW; }

/* get show logo */
bool Settings::Logo(void) const { return modes & LOGO; }

/* set modes */
void Settings::SetModes(const settings_t s) { modes |= s; }

/* reset modes */
void Settings::ResetModes(const settings_t s) { modes &= ~s; }

/* get video mode */
Size & Settings::VideoMode(void) { return video_mode; }

void Settings::Parse(const std::string & left, const std::string & right)
{
    // debug
    if(left == "debug") debug = String::ToInt(right);
    else
    // data directory
    if(left == "data") path_data_directory = right;
    else
    // maps directory
    if(left == "maps") path_maps_directory = right;
    else
    // value
    if(left == "videomode")
    {
	if(right == "640x480")
	{
	    video_mode.w = 640;
	    video_mode.h = 480;
	}
	else
	if(right == "800x600")
	{
	    video_mode.w = 800;
	    video_mode.h = 600;
	}
	else
	if(right == "1024x768")
	{
	    video_mode.w = 1024;
	    video_mode.h = 768;
	}
	else
	if(right == "1280x1024")
	{
	    video_mode.w = 1280;
	    video_mode.h = 1024;
	}
	else
	{
	    std::string str(right);

	    String::Lower(str);

	    const size_t pos = str.find('x');

	    if(std::string::npos != pos)
	    {
		std::string left2(str.substr(0, pos - 1));
		std::string right2(str.substr(pos + 1, str.length() - pos - 1));

		String::Trim(left2);
		String::Trim(right2);
		
		video_mode.w = String::ToInt(left2);
		video_mode.h = String::ToInt(right2);
	    }
	}
    }
}

/* set level debug */
void Settings::SetDebug(const u8 d) { debug = d; }

/**/
void Settings::SetGameDifficulty(const Difficulty::difficulty_t d) { game_difficulty = d; }

void Settings::SetMyColor(const Color::color_t c) { my_color = c; }
/**/
void Settings::SetModes(const std::string & key)
{
    // debug
    if(key == "debug")		debug = 1;
    else
    // sound
    if(key == "sound")		SetModes(SOUND);
    else
    // music
    if(key == "music")		SetModes(MUSIC);
    else
    // animation
    if(key == "animation")	SetModes(ANIMATION);
    else
    // fullscreen
    if(key == "fullscreen")	SetModes(FULLSCREEN);
    else
    // interface
    if(key == "evilinterface")	SetModes(EVILINTERFACE);
    else
    // shadow
    if(key == "shadow")		SetModes(SHADOW);
    else
    // origin version
    if(key == "original")	SetModes(ORIGINAL);
    else
    // show logo
    if(key == "logo")		SetModes(LOGO);
}

void Settings::ResetModes(const std::string & key)
{
    // debug
    if(key == "debug")		debug = 0;
    else
    // sound
    if(key == "sound")		ResetModes(SOUND);
    else
    // music
    if(key == "music")		ResetModes(MUSIC);
    else
    // animation
    if(key == "animation")	ResetModes(ANIMATION);
    else
    // fullscreen
    if(key == "fullscreen")	ResetModes(FULLSCREEN);
    else
    // interface
    if(key == "evilinterface") ResetModes(EVILINTERFACE);
    else
    // shadow
    if(key == "shadow")		ResetModes(SHADOW);
    else
    // origin version
    if(key == "original")	ResetModes(ORIGINAL);
    else
    // show logo
    if(key == "logo")		ResetModes(LOGO);
}

u8   Settings::SoundVolume(void) const
{
    return sound_volume;
}

u8   Settings::MusicVolume(void) const
{
    return music_volume;
}

u8   Settings::AnimationSpeed(void) const
{
    return animation_speed;
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
void Settings::SetAnimationSpeed(const u8 s)
{
    animation_speed = 10 <= s ? 10 : (s ? s : 1);
}
