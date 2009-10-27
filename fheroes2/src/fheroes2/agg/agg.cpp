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

#include <algorithm>
#include <fstream>
#include <iostream>

#include "settings.h"
#include "text.h"
#include "engine.h"
#include "midi_xmi.h"
#include "midi_mid.h"
#include "dir.h"
#include "agg.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

void StoreMemToFile(const std::vector<u8> &, const std::string &);
void StoreFileToMem(std::vector<u8> &, const std::string &);
bool FilePresent(const std::string &);

#define FATSIZENAME	15

/*AGG::File constructor */
AGG::File::File(void) : count_items(0), stream(NULL)
{
}

bool AGG::File::Open(const std::string & fname)
{
    filename = fname;
    stream = new std::fstream(filename.c_str(), std::ios::in | std::ios::binary);

    if(!stream || stream->fail())
    {
	Error::Warning("AGG::File: error read file: " + filename + ", skipping...");
	return false;
    }

    if(Settings::Get().Debug()) Error::Verbose("AGG::File: load: " + filename);

    stream->read(reinterpret_cast<char *>(&count_items), sizeof(u16));
    SwapLE16(count_items);

    if(Settings::Get().Debug()) Error::Verbose("AGG::File: count items: ", count_items);

    char buf[FATSIZENAME + 1];
    buf[FATSIZENAME] = 0;

    for(u16 ii = 0; ii < count_items; ++ii)
    {
            const u32 pos = stream->tellg();

            stream->seekg(-FATSIZENAME * (count_items - ii), std::ios_base::end);
            stream->read(buf, FATSIZENAME);

            const std::string key(buf);

            FAT & f = fat[key];
		
            stream->seekg(pos, std::ios_base::beg);

            stream->read(reinterpret_cast<char *>(&f.crc), sizeof(u32));
            SwapLE32(f.crc);

            stream->read(reinterpret_cast<char *>(&f.offset), sizeof(u32));
            SwapLE32(f.offset);

            stream->read(reinterpret_cast<char *>(&f.size), sizeof(u32));
            SwapLE32(f.size);
    }
    return true;
}

AGG::File::~File()
{
    if(stream)
    {
	stream->close();
	delete stream;
    }
}

bool AGG::File::isGood(void) const
{
    return stream && stream->good() && count_items;
}

/* get AGG file name */
const std::string & AGG::File::Name(void) const
{
    return filename;
}

/* get FAT element */
const AGG::FAT & AGG::File::Fat(const std::string & key)
{
    return fat[key];
}

/* get count elements */
u16 AGG::File::CountItems(void)
{
    return count_items;
}

/* dump FAT */
void AGG::FAT::Dump(const std::string & n) const
{
    printf("%s\t%lX\t%lX\t%lX\n", n.c_str(), static_cast<long unsigned int>(crc), static_cast<long unsigned int>(offset), static_cast<long unsigned int>(size));
}

/* read element to body */
bool AGG::File::Read(const std::string & key, std::vector<char> & body)
{
    const FAT & f = fat[key];

    if(Settings::Get().Debug() > 3) f.Dump(key);
    
    if(f.size)
    {
	if(2 < Settings::Get().Debug()) Error::Verbose("AGG::File::Read: " + key);

	body.resize(f.size);

	stream->seekg(f.offset, std::ios_base::beg);

	stream->read(& body.at(0), f.size);

	return true;
    }

    return false;
}

/* dump AGG file */
void AGG::File::Dump(void) const
{
    std::map<std::string, FAT>::const_iterator it1 = fat.begin();
    std::map<std::string, FAT>::const_iterator it2 = fat.end();

    for(; it1 != it2; ++it1)
    {
	const std::string & n = (*it1).first;
	const FAT & f = (*it1).second;

	f.Dump(n);
    }
}

/* AGG::Cache constructor */
AGG::Cache::Cache()
{
#ifdef WITH_TTF
    Settings & conf = Settings::Get();
    const std::string font1(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "fonts" + SEPARATOR + conf.FontsNormal());
    const std::string font2(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "fonts" + SEPARATOR + conf.FontsSmall());

    if(conf.Unicode())
    {
	if(!font_medium.Open(font1, conf.FontsNormalSize()) ||
	   !font_small.Open(font2, conf.FontsSmallSize())) conf.ResetModes(Settings::USEUNICODE);
    }
#endif
    icn_cache = new icn_cache_t [ICN::UNKNOWN + 1];
    til_cache = new til_cache_t [TIL::UNKNOWN + 1];

    icn_registry_enable = false;
    icn_registry.reserve(250);
}

AGG::Cache::~Cache()
{
    if(icn_cache)
    {
	for(u16 ii = 0; ii < ICN::UNKNOWN + 1; ++ii)
	{
	    if(icn_cache[ii].sprites) delete [] icn_cache[ii].sprites;
	    if(icn_cache[ii].reflect) delete [] icn_cache[ii].reflect;
	}
	delete [] icn_cache;
    }

    if(til_cache)
    {
	for(u16 ii = 0; ii < TIL::UNKNOWN + 1; ++ii)
	{
	    if(til_cache[ii].sprites) delete [] til_cache[ii].sprites;
	}
	delete [] til_cache;
    }
}

/* get AGG::Cache object */
AGG::Cache & AGG::Cache::Get(void)
{
    static Cache agg_cache;

    return agg_cache;
}

/* read data directory */
bool AGG::Cache::ReadDataDir(void)
{
    const Settings & conf = Settings::Get();
    Dir dir;

    dir.Read(conf.DataDirectory(), ".agg", false);
    dir.Read(conf.LocalPrefix() + SEPARATOR + conf.DataDirectory(), ".agg", false);

    // not found agg, exit
    if(0 == dir.size()) return false;

    // attach agg files
    for(Dir::const_iterator itd = dir.begin(); itd != dir.end(); ++itd)
    {
	std::string lower = *itd;
	String::Lower(lower);
	if(std::string::npos != lower.find("heroes2.agg") && !heroes2_agg.isGood()) heroes2_agg.Open(*itd);
	if(std::string::npos != lower.find("heroes2x.agg") && !heroes2x_agg.isGood()) heroes2x_agg.Open(*itd);
    }

    if(heroes2x_agg.isGood()) Settings::Get().SetModes(Settings::PRICELOYALTY);

    return heroes2_agg.isGood();
}

bool AGG::Cache::ReadChunk(const std::string & key, std::vector<char> & body)
{
    if(heroes2x_agg.isGood() && heroes2x_agg.Read(key, body)) return true;

    return heroes2_agg.isGood() && heroes2_agg.Read(key, body);
}

/* load manual ICN object */
void AGG::Cache::LoadExtraICN(const ICN::icn_t icn, u16 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    if(reflect)
    {
	if(v.reflect && (index >= v.count || v.reflect[index].valid())) return;
    }
    else
    {
	if(v.sprites && (index >= v.count || v.sprites[index].valid())) return;
    }

    const Settings & conf = Settings::Get();
    if(1 < conf.Debug()) std::cout << "AGG::Cache::LoadICN: " << ICN::GetString(icn) << ", " << index << std::endl;

    u8 count = 0;		// for animation sprite need update count for ICN::AnimationFrame

    switch(icn)
    {
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3: count = 8; break;
	case ICN::FOUNTAIN:  count = 2; break;
	case ICN::TREASURE:  count = 2; break;
	case ICN::ROUTERED:  count = 145; break;
	case ICN::YELLOW_FONT:
	case ICN::YELLOW_SMALFONT: count = 96; break;

	default: break;
    }

    if(NULL == v.sprites)
    {
	v.sprites = new Sprite [count];
	v.reflect = new Sprite [count];
	v.count = count;
    }

    for(u8 ii = 0; ii < count; ++ii)
    {
	Sprite & sprite = reflect ? v.reflect[ii] : v.sprites[ii];

	switch(icn)
	{
	    case ICN::TELEPORT1:
		sprite = GetICN(ICN::OBJNMUL2, 116);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii / 2);
		break;

	    case ICN::TELEPORT2:
		sprite = GetICN(ICN::OBJNMUL2, 119);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::TELEPORT3:
		sprite = GetICN(ICN::OBJNMUL2, 122);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::FOUNTAIN:
		sprite = GetICN(ICN::OBJNMUL2, 15);
		sprite.ChangeColorIndex(0xE8, 0xE8 - ii);
		sprite.ChangeColorIndex(0xE9, 0xE9 - ii);
		sprite.ChangeColorIndex(0xEA, 0xEA - ii);
		sprite.ChangeColorIndex(0xEB, 0xEB - ii);
		sprite.ChangeColorIndex(0xEC, 0xEC - ii);
		sprite.ChangeColorIndex(0xED, 0xED - ii);
		sprite.ChangeColorIndex(0xEE, 0xEE - ii);
		sprite.ChangeColorIndex(0xEF, 0xEF - ii);
		break;

	    case ICN::TREASURE:
		sprite = GetICN(ICN::OBJNRSRC, 19);
		sprite.ChangeColorIndex(0x0A, ii ? 0x00 : 0x0A);
		sprite.ChangeColorIndex(0xC2, ii ? 0xD6 : 0xC2);
		sprite.ChangeColorIndex(0x64, ii ? 0xDA : 0x64);
		break;

	    case ICN::ROUTERED:
		sprite = GetICN(ICN::ROUTE, ii);
		sprite.ChangeColorIndex(0x55, 0xB0);
		sprite.ChangeColorIndex(0x5C, 0xB7);
		sprite.ChangeColorIndex(0x60, 0xBB);
		break;

	    case ICN::YELLOW_FONT:
		sprite = GetICN(ICN::FONT, ii);
		sprite.ChangeColorIndex(0x0A, 0xDA);
		sprite.ChangeColorIndex(0x0B, 0xDA);
		sprite.ChangeColorIndex(0x0C, 0xDA);
		sprite.ChangeColorIndex(0x0D, 0xDA);
		sprite.ChangeColorIndex(0x0E, 0xDB);
		sprite.ChangeColorIndex(0x0F, 0xDB);
		sprite.ChangeColorIndex(0x10, 0xDB);
		sprite.ChangeColorIndex(0x11, 0xDB);
		sprite.ChangeColorIndex(0x12, 0xDB);
		sprite.ChangeColorIndex(0x13, 0xDB);
		sprite.ChangeColorIndex(0x14, 0xDB);
		break;

	    case ICN::YELLOW_SMALFONT:
		sprite = GetICN(ICN::SMALFONT, ii);
		sprite.ChangeColorIndex(0x0A, 0xDA);
		sprite.ChangeColorIndex(0x0B, 0xDA);
		sprite.ChangeColorIndex(0x0C, 0xDA);
		sprite.ChangeColorIndex(0x0D, 0xDA);
		sprite.ChangeColorIndex(0x0E, 0xDB);
		sprite.ChangeColorIndex(0x0F, 0xDB);
		sprite.ChangeColorIndex(0x10, 0xDB);
		sprite.ChangeColorIndex(0x11, 0xDB);
		sprite.ChangeColorIndex(0x12, 0xDB);
		sprite.ChangeColorIndex(0x13, 0xDB);
		sprite.ChangeColorIndex(0x14, 0xDB);
		break;

	    default: break;
	}
    }
}

/* load ICN object to AGG::Cache */
void AGG::Cache::LoadICN(const ICN::icn_t icn, u16 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    if(reflect)
    {
	if(v.reflect && (index >= v.count || v.reflect[index].valid())) return;
    }
    else
    {
	if(v.sprites && (index >= v.count || v.sprites[index].valid())) return;
    }

    const Settings & conf = Settings::Get();

#ifdef WITH_XML
    // load from image cache dir
    if(conf.Modes(Settings::ALTRESOURCE))
    {
	Dir dir;
	std::string name(ICN::GetString(icn));
	String::Lower(name);
	const std::string icn_folder(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "images" + SEPARATOR + name);
	const std::string xml_spec(icn_folder + SEPARATOR + "spec.xml");

	if(3 < conf.Debug()) Error::Verbose("AGG::Cache::LoadICN: " + icn_folder);

	// parse spec.xml
	TiXmlDocument doc;
	TiXmlElement* xml_icn = NULL;

	if(doc.LoadFile(xml_spec.c_str()) &&
	    NULL != (xml_icn = doc.FirstChildElement()) &&
	    0 == std::strcmp("icn", xml_icn->Value()))
	{
	    int count, ox, oy;
	    xml_icn->Attribute("count", &count);

	    if(NULL == v.sprites)
	    {
		v.count = count;
		v.sprites = new Sprite [v.count];
		v.reflect = new Sprite [v.count];
	    }

	    // find current image
	    TiXmlElement *xml_sprite = xml_icn->FirstChildElement();
	    u16 ii = 0;
	    for(; ii != index && xml_sprite; xml_sprite = xml_sprite->NextSiblingElement(), ++ii)
		if(std::strcmp("sprite", xml_sprite->Value())) --ii;

	    if(xml_sprite && ii == index)
	    {
		xml_sprite->Attribute("ox", &ox);
		xml_sprite->Attribute("oy", &oy);
		name = icn_folder + SEPARATOR + xml_sprite->Attribute("name");
		Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];
		// good load
		if(sp.Load(name.c_str()) && sp.valid())
		{
		    sp.SetOffset(ox, oy);
		    if(1 < conf.Debug()) std::cout << "AGG::Cache::LoadICN: " << xml_spec << ", " << index << std::endl;

		    return;
		}
	    }
	}
	if(conf.Debug()) Error::Verbose("AGG::Cache::LoadICN: broken xml file: " + xml_spec);
    }
#endif

    // loading original
    if(1 < conf.Debug()) std::cout << "AGG::Cache::LoadICN: " << ICN::GetString(icn) << ", " << index << std::endl;

    std::vector<char> body;

    if(ReadChunk(ICN::GetString(icn), body))
    {
	if(NULL == v.sprites)
	{
	    v.count = ReadLE16(reinterpret_cast<const u8*>(&body[0]));
	    v.sprites = new Sprite [v.count];
	    v.reflect = new Sprite [v.count];
	}

	ICN::Header header1, header2;

	header1.Load(&body[6 + index * ICN::Header::SizeOf()]);
	if(index + 1 != v.count) header2.Load(&body[6 + (index + 1) * ICN::Header::SizeOf()]);

	const u32 size_data = (index + 1 != v.count ? header2.OffsetData() - header1.OffsetData() :
				    // total size
				    ReadLE32(reinterpret_cast<const u8*>(&body[2])) - header1.OffsetData());

	Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];
	sp.Set(header1.Width(), header1.Height(), ICN::RequiresAlpha(icn));
	sp.SetOffset(header1.OffsetX(), header1.OffsetY());
	sp.LoadICN(&body[6 + header1.OffsetData()], size_data, reflect);
	
	if(Settings::Get().PocketPC() && ICN::NeedMinify4PocketPC(icn, index)) 
	    sp.ScaleMinifyByTwo();
    }

    // registry icn
    if(icn_registry_enable &&
	icn_registry.end() == std::find(icn_registry.begin(), icn_registry.end(), icn))
	icn_registry.push_back(icn);
}

/* load TIL object to AGG::Cache */
void AGG::Cache::LoadTIL(const TIL::til_t til)
{
    til_cache_t & v = til_cache[til];

    if(v.sprites) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadTIL: ", TIL::GetString(til));

    std::vector<char> body;

    if(ReadChunk(TIL::GetString(til), body))
    {
	const u16 count = ReadLE16(reinterpret_cast<const u8*>(&body.at(0)));
	const u16 width = ReadLE16(reinterpret_cast<const u8*>(&body.at(2)));
	const u16 height= ReadLE16(reinterpret_cast<const u8*>(&body.at(4)));

	const u32 tile_size = width * height;
	const u32 body_size = 6 + count * tile_size;

	// check size
	if(body.size() != body_size)
	{
	    Error::Warning("AGG::Cache::LoadTIL: size mismach, skipping...");
	    return;
	}

	v.count = count * 4;  // rezerve for rotate sprites
	v.sprites = new Surface [v.count];

	for(u16 ii = 0; ii < count; ++ii)
	    v.sprites[ii].Set(&body[6 + ii * tile_size], width, height, 1, false);
    }
}

/* load 82M object to AGG::Cache in Audio::CVT */
void AGG::Cache::LoadWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size()) return;


    if(! Mixer::isValid()) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadWAV: ", M82::GetString(m82));

    Audio::Spec wav_spec;
    wav_spec.format = AUDIO_U8;
    wav_spec.channels = 1;
    wav_spec.freq = 22050;

    const Audio::Spec & hardware = Audio::GetHardwareSpec();

    Audio::CVT cvt;
    std::vector<char> body;

    if(cvt.Build(wav_spec, hardware) &&
       ReadChunk(M82::GetString(m82), body))
    {
	const u32 size = cvt.len_mult * body.size();

	cvt.buf = new u8[size];
	cvt.len = body.size();

	memcpy(cvt.buf, &body[0], body.size());

	cvt.Convert();

	v.assign(cvt.buf, cvt.buf + size - 1);

	delete [] cvt.buf;
	cvt.buf = NULL;
    }
}

/* load XMI object to AGG::Cache */
void AGG::Cache::LoadMID(const XMI::xmi_t xmi)
{
    std::vector<u8> & v = mid_cache[xmi];

    if(v.size()) return;

    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadMID: ", XMI::GetString(xmi));

    if(! Mixer::isValid()) return;

    std::vector<char> body;

    if(ReadChunk(XMI::GetString(xmi), body))
    {
	MIDI::Xmi x;
	MIDI::Mid m;
	MIDI::MTrk track;

	x.Read(body);
	track.ImportXmiEVNT(x.EVNT());

	m.AddTrack(track);
	m.SetPPQN(64);

	m.Write(reinterpret_cast<std::vector<char> &>(v));
    }
}

void AGG::Cache::LoadPAL(void)
{
    if(Settings::Get().Debug()) Error::Verbose("AGG::Cache::LoadPAL: ", Palette::Get().Size());
}

void AGG::Cache::LoadMUS(const MUS::mus_t mus)
{
    std::vector<u8> & v = mus_cache[mus];
    if(! Mixer::isValid() || v.size()) return;

    const Settings & conf = Settings::Get();
    const std::string musname(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "music" + SEPARATOR + MUS::GetString(mus));
    std::string shortname(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "music" + SEPARATOR + MUS::GetString(mus, true));
    const char* filename = NULL;

    if(FilePresent(musname))   filename = musname.c_str();
    else
    if(FilePresent(shortname)) filename = shortname.c_str();
    else
    {
	String::Replace(shortname, ".ogg", ".mp3");
	if(FilePresent(shortname)) filename = shortname.c_str();
	else
	Error::Warning("AGG::Cache::LoadMUS: error read file: " + musname + ", skipping...");
    }

    std::fstream stream(filename, std::ios::in | std::ios::binary);

    if(stream.good())
    {
	if(conf.Debug()) Error::Verbose("AGG::Cache::LoadMUS: ", filename);

	stream.seekg(0, std::ios_base::end);
	const u32 size = stream.tellg();
	stream.seekg(0, std::ios_base::beg);
	v.resize(size);
    
	stream.read(reinterpret_cast<char *>(&v[0]), size);
	stream.close();
    }
}

void AGG::Cache::LoadFNT(void)
{
    const Settings & conf = Settings::Get();

#ifdef WITH_TTF
    if(conf.Unicode())
    {
	if(fnt_cache.size()) return;

	const char *letters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	const u8 letters_size = std::strlen(letters);
	u16 *unicode = new u16[letters_size + 1];

	String::UTF8_to_UNICODE(unicode, letters, letters_size);

	for(u8 ii = 0; ii < letters_size; ++ii) LoadFNT(unicode[ii]);

	delete [] unicode;

	if(conf.Debug())
	{
	    if(fnt_cache.size())
	    {
    		Error::Verbose("AGG::LoadFonts: normal fonts " + conf.FontsNormal());
    	        Error::Verbose("AGG::LoadFonts:  small fonts " + conf.FontsSmall());
    		Error::Verbose("AGG::LoadFonts: preload english charsets");
	    }
	    else
		Error::Verbose("AGG::LoadFonts: internal.");
	}
    }
    else
#endif
    {
	PreloadObject(ICN::SMALFONT);
	PreloadObject(ICN::FONT);

	if(conf.Debug()) Error::Verbose("AGG::LoadFonts: internal.");
    }
}

#ifdef WITH_TTF
void AGG::Cache::LoadFNT(u16 ch)
{
    const Settings & conf = Settings::Get();

    if(conf.Unicode())
    {
        const Colors white = { 0xFF, 0xFF, 0xFF, 0x00 };
        const Colors yellow= { 0xFF, 0xFF, 0x00, 0x00 };

	// small
	font_small.RenderUnicodeChar(fnt_cache[ch].small_white, ch, white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	font_small.RenderUnicodeChar(fnt_cache[ch].small_yellow, ch, yellow, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);

	// medium
	if(!(conf.PocketPC() && !conf.Unicode()))
	{
	    font_medium.RenderUnicodeChar(fnt_cache[ch].medium_white, ch, white, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	    font_medium.RenderUnicodeChar(fnt_cache[ch].medium_yellow, ch, yellow, conf.FontsRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	}

	if(conf.Debug()) Error::Verbose("AGG::LoadChar: ", static_cast<int>(ch));
    }
}
#endif

/* free ICN object in AGG::Cache */
void AGG::Cache::FreeICN(const ICN::icn_t icn)
{
    if(1 < Settings::Get().Debug()) Error::Verbose("AGG::Cache::FreeICN: ", ICN::GetString(icn));
    if(icn_cache[icn].sprites){ delete [] icn_cache[icn].sprites; icn_cache[icn].sprites = NULL; }
    if(icn_cache[icn].reflect){ delete [] icn_cache[icn].reflect; icn_cache[icn].reflect = NULL; }
    icn_cache[icn].count = 0;
}

/* free TIL object in AGG::Cache */
void AGG::Cache::FreeTIL(const TIL::til_t til)
{
    if(til_cache[til].sprites){ delete [] til_cache[til].sprites; til_cache[til].sprites = NULL; }
    til_cache[til].count = 0;
}

/* free 82M object in AGG::Cache */
void AGG::Cache::FreeWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size()) v.clear();
}

/* free music object in AGG::Cache */
void AGG::Cache::FreeMUS(const MUS::mus_t mus)
{
    std::vector<u8> & v = mus_cache[mus];

    if(v.size()) v.clear();
}

/* free XMI object in AGG::Cache */
void AGG::Cache::FreeMID(const XMI::xmi_t xmi)
{
    std::vector<u8> & v = mid_cache[xmi];

    if(v.size()) v.clear();
}

/* return ICN sprite from AGG::Cache */
const Sprite & AGG::Cache::GetICN(const ICN::icn_t icn, u16 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    // out of range?
    if(v.count && index >= v.count)
    {
	Error::Warning("AGG::GetICN: " + std::string(ICN::GetString(icn)) + ", index out of range: ", index);
	index = 0;
    }

    // need load?
    if(0 == v.count || ((reflect && !v.reflect[index].valid()) || !v.sprites[index].valid()))
    {
	switch(icn)
	{
	    case ICN::YELLOW_FONT:
	    case ICN::YELLOW_SMALFONT:
	    case ICN::ROUTERED:
	    case ICN::TELEPORT1:
	    case ICN::TELEPORT2:
	    case ICN::TELEPORT3:
	    case ICN::FOUNTAIN:
	    case ICN::TREASURE:	LoadExtraICN(icn, index, reflect); break;
	    default:		LoadICN(icn, index, reflect); break;
	}
    }

    // invalid sprite?
    if(Settings::Get().Debug() &&  ((reflect && !v.reflect[index].valid()) || (!reflect && !v.sprites[index].valid())))
    {
	Error::Verbose("AGG::Cache::GetICN: return invalid sprite: ", ICN::GetString(icn));
	Error::Verbose("AGG::Cache::GetICN: index: ", index);
	Error::Verbose("AGG::Cache::GetICN: reflect: ", (reflect ? "true" : "false"));
    }

    return reflect ? v.reflect[index] : v.sprites[index];
}

/* return count of sprites from specific ICN */
int AGG::Cache::GetICNCount(const ICN::icn_t icn)
{
    return icn_cache[icn].count;
}

/* return TIL surface from AGG::Cache */
const Surface & AGG::Cache::GetTIL(const TIL::til_t til, u16 index, u8 shape)
{
    til_cache_t & v = til_cache[til];

    if(0 == v.count) LoadTIL(til);

    u16 index2 = index;

    if(shape)
    {
	switch(til)
	{
	    case TIL::STON:     index2 += 36 * (shape % 4); break;
	    case TIL::CLOF32:   index2 += 4 * (shape % 4); break;
	    case TIL::GROUND32: index2 += 432 * (shape % 4); break;
	    default: break;
	}
    }

    if(index2 >= v.count)
    {
	Error::Warning("AGG::GetTIL: " + std::string(TIL::GetString(til)) + ", index out of range: ", index);
	index2 = 0;
    }

    Surface & surface = v.sprites[index2];

    if(shape && ! surface.valid())
    {
	const Surface & src = v.sprites[index];

	if(src.valid())
	{
	    surface.Set(src.w(), src.h(), 8, SDL_SWSURFACE);
	    TIL::Reflect(surface, src, shape);
	}
	else
	Error::Warning("AGG::GetTIL: src surface is NULL");
    }

    if(! surface.valid())
    {
	Error::Warning("AGG::GetTIL: invalid sprite: ", TIL::GetString(til));
	Error::Warning("AGG::GetTIL: index: ", index);
    }

    return surface;
}

/* return CVT from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetWAV(const M82::m82_t m82)
{
    const std::vector<u8> & v = wav_cache[m82];

    if(v.empty()) LoadWAV(m82);

    return v;
}

/* return MID from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetMID(const XMI::xmi_t xmi)
{
    const std::vector<u8> & v = mid_cache[xmi];

    if(v.empty()) LoadMID(xmi);

    return v;
}

/* return CVT from AGG::Cache */
const std::vector<u8> & AGG::Cache::GetMUS(const MUS::mus_t mus)
{
    const std::vector<u8> & v = mus_cache[mus];

    if(v.empty()) LoadMUS(mus);

    return v;
}

#ifdef WITH_TTF
/* return FNT cache */
const Surface & AGG::Cache::GetFNT(u16 c, u8 f)
{
    if(!fnt_cache[c].small_white.valid()) LoadFNT(c);

    switch(f)
    {
	case Font::YELLOW_SMALL: return fnt_cache[c].small_yellow;
	case Font::BIG:		 return fnt_cache[c].medium_white;
	case Font::YELLOW_BIG:	 return fnt_cache[c].medium_yellow;
	default: break;
    }

    return fnt_cache[c].small_white;
}

const SDL::Font & AGG::Cache::GetMediumFont(void) const
{
    return font_medium;
}

const SDL::Font & AGG::Cache::GetSmallFont(void) const
{
    return font_small;
}
#endif

bool AGG::Cache::isValidFonts(void) const
{
#ifdef WITH_TTF
    return Settings::Get().Unicode() ? font_small.isValid() && font_medium.isValid() : false;
#endif
    return false;
}

void AGG::Cache::ICNRegistryEnable(bool f)
{
    icn_registry_enable = f;
}

void AGG::Cache::ICNRegistryFreeObjects(void)
{
    std::vector<ICN::icn_t>::const_iterator it1 = icn_registry.begin();
    std::vector<ICN::icn_t>::const_iterator it2 = icn_registry.end();
    for(; it1 != it2; ++it1) FreeICN(*it1);
}

void AGG::Cache::Dump(void) const
{
    std::cout << "AGG::Cache::Dump:" << std::endl; 
    u32 total1 = 0;
    u32 total2 = 0;

    if(icn_cache)
    {
	total1 = 0;
        for(u16 ii = 0; ii < ICN::UNKNOWN; ++ii)
        {
	    total2 = 0;
            if(icn_cache[ii].sprites)
        	for(u16 jj = 0; jj < icn_cache[ii].count; ++jj)
		    total2 += (icn_cache[ii].sprites[jj].GetSize() + icn_cache[ii].reflect[jj].GetSize());
	    if(icn_cache[ii].count)
		std::cout << "ICN: " << ICN::GetString((ICN::icn_t) ii) << ", count: " << icn_cache[ii].count << ", total: " << total2 << std::endl;
	    total1 += total2;
        }
	std::cout << "ICN: " << " total: " << total1 << std::endl;
    }

    if(til_cache)
    {
	total1 = 0;
        for(u16 ii = 0; ii < TIL::UNKNOWN; ++ii)
        {
	    total2 = 0;
	    if(til_cache[ii].sprites)
        	for(u16 jj = 0; jj < til_cache[ii].count; ++jj)
        	    total2 += til_cache[ii].sprites[jj].GetSize();
	    if(til_cache[ii].count)
		std::cout << "TIL: " << TIL::GetString((TIL::til_t) ii) << ", count: " << til_cache[ii].count << ", total: " << total2 << std::endl;
	    total1 += total2;
        }
	std::cout << "TIL: " << " total: " << total1 << std::endl;
    }

    if(wav_cache.size())
    {
	total1 = 0;
	std::map<M82::m82_t, std::vector<u8> >::const_iterator it1 = wav_cache.begin();
	std::map<M82::m82_t, std::vector<u8> >::const_iterator it2 = wav_cache.end();
	for(; it1 != it2; ++it1)
	{
	    if((*it1).second.size())
	    	std::cout << "M82: " << M82::GetString((*it1).first) << ", size: " << (*it1).second.size() << std::endl;
	    total1 += (*it1).second.size();
	}
	std::cout << "M82: " << " total: " << total1 << std::endl;
    }

    if(mus_cache.size())
    {
	total1 = 0;
	std::map<MUS::mus_t, std::vector<u8> >::const_iterator it1 = mus_cache.begin();
	std::map<MUS::mus_t, std::vector<u8> >::const_iterator it2 = mus_cache.end();
	for(; it1 != it2; ++it1)
	{
	    if((*it1).second.size())
	    	std::cout << "MUS: " << MUS::GetString((*it1).first) << ", size: " << (*it1).second.size() << std::endl;
	    total1 += (*it1).second.size();
	}
	std::cout << "MUS: " << " total: " << total1 << std::endl;
    }

    if(mid_cache.size())
    {
	total1 = 0;
	std::map<XMI::xmi_t, std::vector<u8> >::const_iterator it1 = mid_cache.begin();
	std::map<XMI::xmi_t, std::vector<u8> >::const_iterator it2 = mid_cache.end();
	for(; it1 != it2; ++it1)
	{
	    if((*it1).second.size())
	    	std::cout << "XMI: " << XMI::GetString((*it1).first) << ", size: " << (*it1).second.size() << std::endl;
	    total1 += (*it1).second.size();
	}
	std::cout << "XMI: " << " total: " << total1 << std::endl;
    }

#ifdef WITH_TTF
    if(fnt_cache.size())
    {
	total1 = 0;
	std::map<u16, fnt_cache_t>::const_iterator it1 = fnt_cache.begin();
	std::map<u16, fnt_cache_t>::const_iterator it2 = fnt_cache.end();
	for(; it1 != it2; ++it1)
	    total1 += ((*it1).second.medium_white.GetSize() + (*it1).second.medium_yellow.GetSize() + (*it1).second.small_white.GetSize() + (*it1).second.small_yellow.GetSize());

	std::cout << "FNT: " << " total: " << total1 << std::endl;
    }
#endif
}

// wrapper AGG::PreloadObject
void AGG::PreloadObject(const ICN::icn_t icn, bool reflect)
{
    // deprecated
    // or loading all sprites? AGG::Cache::Get().LoadICN(icn, index, reflect);
}

void AGG::PreloadObject(const TIL::til_t til)
{
    return AGG::Cache::Get().LoadTIL(til);
}

void AGG::ICNRegistryEnable(bool f)
{
    AGG::Cache::Get().ICNRegistryEnable(f);
}

void AGG::ICNRegistryFreeObjects(void)
{
    AGG::Cache::Get().ICNRegistryFreeObjects();
}

// wrapper AGG::FreeObject
void AGG::FreeObject(const ICN::icn_t icn)
{
    return AGG::Cache::Get().FreeICN(icn);
}

void AGG::FreeObject(const TIL::til_t til)
{
    return AGG::Cache::Get().FreeTIL(til);
}

// wrapper AGG::GetXXX
int AGG::GetICNCount(const ICN::icn_t icn)
{
    return AGG::Cache::Get().GetICNCount(icn);
}

const Sprite & AGG::GetICN(const ICN::icn_t icn, const u16 index, bool reflect)
{
    return AGG::Cache::Get().GetICN(icn, index, reflect);
}

const Surface & AGG::GetTIL(const TIL::til_t til, const u16 index, const u8 shape)
{
    return AGG::Cache::Get().GetTIL(til, index, shape);
}

const std::vector<u8> & AGG::GetWAV(const M82::m82_t m82)
{
    return AGG::Cache::Get().GetWAV(m82);
}

const std::vector<u8> & AGG::GetMUS(const MUS::mus_t mus)
{
    return AGG::Cache::Get().GetMUS(mus);
}

const std::vector<u8> & AGG::GetMID(const XMI::xmi_t xmi)
{
    return AGG::Cache::Get().GetMID(xmi);
}

void AGG::LoadLoopSound(const M82::m82_t m82, u8 ch)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
#ifdef WITH_MIXER
	if(conf.Modes(Settings::ALTRESOURCE))
	{
	    std::string name(M82::GetString(m82));
	    String::Lower(name);
	    String::Replace(name, ".82m", ".ogg");
	    const std::string sound(Settings::Get().LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "sounds" + SEPARATOR + name);

	    if(FilePresent(sound))
	    {
		if(conf.Debug()) Error::Verbose("AGG::LoadRAW: ", sound.c_str());
		Mixer::LoadRAW(sound.c_str(), true, ch);
		return;
	    }
	}
#endif
	if(conf.Debug()) Error::Verbose("AGG::LoadRAW: ", M82::GetString(m82));
	Mixer::LoadRAW(AGG::Cache::Get().GetWAV(m82), true, ch);
    }
}

/* wrapper Audio::Play */
void AGG::PlaySound(const M82::m82_t m82)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
#ifdef WITH_MIXER
	if(conf.Modes(Settings::ALTRESOURCE))
	{
	    std::string name(M82::GetString(m82));
	    String::Lower(name);
	    String::Replace(name, ".82m", ".ogg");
	    const std::string sound(Settings::Get().LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "sounds" + SEPARATOR + name);

	    if(FilePresent(sound))
	    {
		if(conf.Debug()) Error::Verbose("AGG::PlaySound: ", sound.c_str());
		Mixer::PlayRAW(sound.c_str());
		return;
	    }
	}
#endif
	if(conf.Debug()) Error::Verbose("AGG::PlaySound: ", M82::GetString(m82));
	Mixer::PlayRAW(AGG::Cache::Get().GetWAV(m82));
    }
}

/* wrapper Audio::Play */
void AGG::PlayMusic(const MUS::mus_t mus, bool loop)
{
    const Settings & conf = Settings::Get();

    if(!conf.Music()) return;


    if(conf.Modes(Settings::MUSIC_EXT))
    {
	if(MUS::UNUSED != mus && MUS::UNKNOWN != mus)
	{
#ifdef WITH_MIXER
	    Music::Play(AGG::Cache::Get().GetMUS(mus), loop);
#else
	    const std::string file = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "music" + SEPARATOR + MUS::GetString(mus);
	    if(FilePresent(file))
	    {
		const std::string run = conf.PlayMusCommand() + " " + file;
		Music::Play(run, loop);
	    }
#endif
	}
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: " + MUS::GetString(mus));
    }
    else
    if(conf.Modes(Settings::MUSIC_CD) && Cdrom::isValid())
    {
	if(MUS::UNUSED != mus && MUS::UNKNOWN != mus) Cdrom::Play(mus, loop);
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: cd track ", mus);
    }
    else
    {
	XMI::xmi_t xmi = XMI::FromMUS(mus);
	if(XMI::UNKNOWN != xmi)
	{
#ifdef WITH_MIXER
	    Music::Play(AGG::Cache::Get().GetMID(xmi), loop);
#else
	    if(conf.PlayMusCommand().size())
	    {
		const std::string file = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "cache" + SEPARATOR + XMI::GetString(xmi);
		if(!FilePresent(file))
		    StoreMemToFile(AGG::Cache::Get().GetMID(xmi), file);
		else
		{
		    const std::string run = conf.PlayMusCommand() + " " + file;
		    Music::Play(run, loop);
		}
	    }
#endif
	}
        else Music::Reset();
	if(conf.Debug()) Error::Verbose("AGG::PlayMusic: ", XMI::GetString(xmi));
    }
}

#ifdef WITH_TTF
/* return letter sprite */
const Surface & AGG::GetUnicodeLetter(u16 ch, u8 ft)
{
    if(AGG::Cache::Get().isValidFonts())
	return AGG::Cache::Get().GetFNT(ch, ft);
    else
    return AGG::GetLetter(ch, ft);
}
#endif

const Surface & AGG::GetLetter(char ch, u8 ft)
{
    if(ch < 0x21) Error::Warning("AGG::GetLetter: unknown letter");

    switch(ft)
    {
	case Font::YELLOW_BIG:	return AGG::GetICN(ICN::YELLOW_FONT, ch - 0x20);
	case Font::YELLOW_SMALL:return AGG::GetICN(ICN::YELLOW_SMALFONT, ch - 0x20);
	case Font::BIG:		return AGG::GetICN(ICN::FONT, ch - 0x20);
	case Font::SMALL:	return AGG::GetICN(ICN::SMALFONT, ch - 0x20);

	default: break;
    }

    return AGG::GetICN(ICN::SMALFONT, ch - 0x20);
}

bool FilePresent(const std::string & file)
{
    std::fstream fs;
    // check file
    fs.open(file.c_str(), std::ios::in | std::ios::binary);
    if(fs.good())
    {
	fs.close();
	return true;
    }
    return false;
}

void StoreMemToFile(const std::vector<u8> & data, const std::string & file)
{
    if(FilePresent(file)) return;
    std::fstream fs;
    fs.open(file.c_str(), std::ios::out | std::ios::binary);
    if(fs.good() && data.size())
    {
	fs.write(reinterpret_cast<const char*>(&data[0]), data.size());
	fs.close();
    }
}

void StoreFileToMem(std::vector<u8> & data, const std::string & file)
{
    std::fstream fs;
    fs.open(file.c_str(), std::ios::in | std::ios::binary);
    if(fs.good())
    {
	fs.seekg(0, std::ios_base::end);
	data.resize(fs.tellg());
	fs.seekg(0, std::ios_base::beg);
	fs.read(reinterpret_cast<char*>(&data[0]), data.size());
	fs.close();
    }
}
