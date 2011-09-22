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
#include <iostream>

#include "settings.h"
#include "text.h"
#include "engine.h"
#include "midi_xmi.h"
#include "midi_mid.h"
#include "artifact.h"
#include "dir.h"
#include "agg.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

#define FATSIZENAME	15

/*AGG::File constructor */
AGG::File::File(void) : count_items(0), stream(NULL)
{
}

bool AGG::File::Open(const std::string & fname)
{
    filename = fname;
    stream = new std::ifstream(filename.c_str(), std::ios::binary);

    if(!stream || !stream->is_open())
    {
	DEBUG(DBG_ENGINE, DBG_WARN, "error read file: " << filename << ", skipping...");
	return false;
    }

    stream->read(reinterpret_cast<char *>(&count_items), sizeof(u16));
    SwapLE16(count_items);

    DEBUG(DBG_ENGINE, DBG_INFO, "load: " << filename << ", count items: " << count_items);

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
std::string AGG::FAT::Info(void) const
{
    std::ostringstream os;

    os << "crc: " << crc << ", offset: " << offset << ", size: " << size;
    return os.str();
}

/* read element to body */
bool AGG::File::Read(const std::string & key, std::vector<u8> & body)
{
    const FAT & f = fat[key];

    if(f.size)
    {
	if(last_key != key)
	{
	    DEBUG(DBG_ENGINE, DBG_TRACE, key << ":\t" << f.Info());

	    last_key = key;
	    last_body.resize(f.size);

	    stream->seekg(f.offset, std::ios_base::beg);
	    stream->read(reinterpret_cast<char*>(&last_body[0]), f.size);
	}

	body = last_body;

	return true;
    }

    return false;
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
	   !font_small.Open(font2, conf.FontsSmallSize())) conf.SetUnicode(false);
    }
#endif
    icn_cache = new icn_cache_t [ICN::UNKNOWN + 1];
    til_cache = new til_cache_t [TIL::UNKNOWN + 1];

    icn_cache[ICN::UNKNOWN].count = 1;
    icn_cache[ICN::UNKNOWN].sprites = new Sprite [1];
    icn_cache[ICN::UNKNOWN].reflect = new Sprite [1];

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

void AGG::Cache::ClearAllICN(void)
{
    if(icn_cache)
    {
	for(u16 ii = 0; ii < ICN::UNKNOWN; ++ii)
	{
	    if(icn_cache[ii].sprites) delete [] icn_cache[ii].sprites;
	    icn_cache[ii].sprites = NULL;
	    if(icn_cache[ii].reflect) delete [] icn_cache[ii].reflect;
	    icn_cache[ii].reflect = NULL;
	}
    }
}

void AGG::Cache::ClearAllWAV(void)
{
    loop_sounds.clear();

    std::map<M82::m82_t, std::vector<u8> >::iterator it1 = wav_cache.begin();
    std::map<M82::m82_t, std::vector<u8> >::iterator it2 = wav_cache.end();

    for(; it1 != it2; ++it1) if((*it1).second.size()) (*it1).second.clear();
}

void AGG::Cache::ClearAllMID(void)
{
    std::map<XMI::xmi_t, std::vector<u8> >::iterator it1 = mid_cache.begin();
    std::map<XMI::xmi_t, std::vector<u8> >::iterator it2 = mid_cache.end();

    for(; it1 != it2; ++it1) if((*it1).second.size()) (*it1).second.clear();
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
	std::string lower = String::Lower(*itd);
	if(std::string::npos != lower.find("heroes2.agg") && !heroes2_agg.isGood()) heroes2_agg.Open(*itd);
	if(std::string::npos != lower.find("heroes2x.agg") && !heroes2x_agg.isGood()) heroes2x_agg.Open(*itd);
    }

    if(heroes2x_agg.isGood()) Settings::Get().SetPriceLoyaltyVersion();

    return heroes2_agg.isGood();
}

bool AGG::Cache::ReadChunk(const std::string & key, std::vector<u8> & body)
{
    if(heroes2x_agg.isGood() && heroes2x_agg.Read(key, body)) return true;

    return heroes2_agg.isGood() && heroes2_agg.Read(key, body);
}

/* load manual ICN object */
void AGG::Cache::LoadExtICN(icn_cache_t & v, const ICN::icn_t icn, const u16 index, bool reflect)
{
    DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(icn) << ", " << index);

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
	case ICN::BATTLESKIP:
	case ICN::BATTLEWAIT:
	case ICN::BUYMAX:
	case ICN::BTNBATTLEONLY:
	case ICN::BTNGIFT:
	case ICN::BTNCONFIG:  count = 2; break;
	case ICN::BOAT12:  count = 1; break;

	default: break;
    }

    if(NULL == v.sprites)
    {
	v.sprites = new Sprite [count];
	v.reflect = new Sprite [count];
	v.count = count;
    }

    // simple modify
    switch(icn)
    {
	case ICN::BTNBATTLEONLY:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::BTNNEWGM, 2 + index, false);
	    // clean
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 15, 13, sprite);
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 70, 13, sprite);
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 42, 28, sprite);
	    // ba
	    GetICN(ICN::BTNCMPGN, index).Blit(Rect(41, 28, 28, 14), 30, 13, sprite);
	    // tt
	    GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 57, 13, sprite);
	    GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 70, 13, sprite);
	    // le
	    GetICN(ICN::BTNNEWGM, 6 + index).Blit(Rect(97, 21, 13, 14), 83, 13, sprite);
	    GetICN(ICN::BTNNEWGM, 6 + index).Blit(Rect(86, 21, 13, 14), 96, 13, sprite);
	    // on
	    GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(44, 21, 31, 14), 40, 28, sprite);
	    // ly
	    GetICN(ICN::BTNHOTST, index).Blit(Rect(47, 21, 13, 13), 71, 28, sprite);
	    GetICN(ICN::BTNHOTST, index).Blit(Rect(72, 21, 13, 13), 84, 28, sprite);
	}
	break;

	case ICN::BTNCONFIG:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::SYSTEM, 11 + index, false);
	    // config
	    GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(30, 20, 80, 16), 8, 5, sprite);
	}
	break;

	case ICN::BTNGIFT:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite,
			(Settings::Get().EvilInterface() ? ICN::TRADPOSE : ICN::TRADPOST),
			17 + index, false);
	    // clean
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 72, 15), 6, 4, sprite);
	    // G
	    GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(94, 20, 15, 15), 20, 4, sprite);
	    // I
	    GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(86, 20, 9, 15), 36, 4, sprite);
	    // F
	    GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(74, 20, 13, 15), 46, 4, sprite);
	    // T
	    GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 60, 5, sprite);
	}
	break;

	case ICN::BUYMAX:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::WELLXTRA, index, false);
	    // clean
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 52, 14), 6, 2, sprite);
	    // max
	    GetICN(ICN::RECRUIT, 4 + index).Blit(Rect(12, 6, 50, 12), 7, 3, sprite);
	}
	break;

	case ICN::BATTLESKIP:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::TEXTBAR, 4 + index, false);
	    // clean
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(3, 8, 43, 14), 3, 1, sprite);
	    // skip
	    GetICN(ICN::TEXTBAR, index).Blit(Rect(3, 8, 43, 14), 3, 0, sprite);
	}
	break;

	case ICN::BATTLEWAIT:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::TEXTBAR, 4 + index, false);
	    // clean
	    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(3, 8, 43, 14), 3, 1, sprite);
	    // wait
	    Surface src, dst;
	    src.Set(28, 28);
	    GetICN(ICN::ADVBTNS, 8 + index).Blit(Rect(5, 4, 28, 28), 0, 0, src);
	    Surface::ScaleMinifyByTwo(dst, src);
	    dst.Blit((sprite.w() - dst.w()) / 2, 2, sprite);
	}
	break;

	case ICN::BOAT12:
	if(index < count)
	{
	    Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];
	    LoadOrgICN(sprite, ICN::ADVMCO, 28 + index, false);
	    Surface dst;
	    Surface::ScaleMinifyByTwo(dst, sprite);
	    Surface::Swap(sprite, dst);
	}
	default: break;
    }


    // change color
    for(u8 ii = 0; ii < count; ++ii)
    {
	Sprite & sprite = reflect ? v.reflect[ii] : v.sprites[ii];

	switch(icn)
	{
	    case ICN::TELEPORT1:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 116, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii / 2);
		break;

	    case ICN::TELEPORT2:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 119, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::TELEPORT3:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 122, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::FOUNTAIN:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 15, false);
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
		LoadOrgICN(sprite, ICN::OBJNRSRC, 19, false);
		sprite.ChangeColorIndex(0x0A, ii ? 0x00 : 0x0A);
		sprite.ChangeColorIndex(0xC2, ii ? 0xD6 : 0xC2);
		sprite.ChangeColorIndex(0x64, ii ? 0xDA : 0x64);
		break;

	    case ICN::ROUTERED:
		LoadOrgICN(sprite, ICN::ROUTE, ii, false);
		sprite.ChangeColorIndex(0x55, 0xB0);
		sprite.ChangeColorIndex(0x5C, 0xB7);
		sprite.ChangeColorIndex(0x60, 0xBB);
		break;

	    case ICN::YELLOW_FONT:
		LoadOrgICN(sprite, ICN::FONT, ii, false);
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
		LoadOrgICN(sprite, ICN::SMALFONT, ii, false);
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

bool AGG::Cache::LoadAltICN(icn_cache_t & v, const std::string & spec, const u16 index, bool reflect)
{
#ifdef WITH_XML
    // parse spec.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_icn = NULL;

    if(doc.LoadFile(spec.c_str()) &&
	NULL != (xml_icn = doc.FirstChildElement("icn")))
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
	const TiXmlElement *xml_sprite = xml_icn->FirstChildElement("sprite");
	u16 ii = 0;
	for(; ii != index && xml_sprite; xml_sprite = xml_sprite->NextSiblingElement("sprite"), ++ii);

	if(xml_sprite && ii == index)
	{
	    xml_sprite->Attribute("ox", &ox);
	    xml_sprite->Attribute("oy", &oy);
	    std::string name(spec);
	    String::Replace(name, "spec.xml", xml_sprite->Attribute("name"));
	    Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];
	    // good load
	    if(sp.Load(name.c_str()) && sp.isValid())
	    {
		sp.SetOffset(ox, oy);
		DEBUG(DBG_ENGINE, DBG_TRACE, spec << ", " << index);

		return true;
	    }
	}
	DEBUG(DBG_ENGINE, DBG_WARN, "broken xml file: " <<  spec);
    }
#endif

    return false;
}

void AGG::Cache::LoadOrgICN(Sprite & sp, const ICN::icn_t icn, const u16 index, bool reflect)
{
    std::vector<u8> body;

    if(ReadChunk(ICN::GetString(icn), body))
    {
	// hard fix artifact "ultimate stuff" sprite for loyalty version
	if(Settings::Get().PriceLoyaltyVersion() &&
	    ICN::ARTIFACT == icn &&
	    Artifact(Artifact::ULTIMATE_STAFF).IndexSprite64() == index)
	{
	    body.clear();
	    heroes2_agg.Read(ICN::GetString(icn), body);
	}

	// loading original
	DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(icn) << ", " << index);

	const u16 count = ReadLE16(&body[0]);
	ICN::Header header1, header2;

	header1.Load(&body[6 + index * ICN::Header::SizeOf()]);
	if(index + 1 != count) header2.Load(&body[6 + (index + 1) * ICN::Header::SizeOf()]);

	const u32 size_data = (index + 1 != count ? header2.OffsetData() - header1.OffsetData() :
				    // total size
				    ReadLE32(&body[2]) - header1.OffsetData());

	sp.Set(header1.Width(), header1.Height(), false);
	sp.SetOffset(header1.OffsetX(), header1.OffsetY());
	Sprite::DrawICN(icn, sp, &body[6 + header1.OffsetData()], size_data, reflect);
	Sprite::AddonExtensionModify(sp, icn, index);
    }
    else
    {
	DEBUG(DBG_ENGINE, DBG_WARN, "error: " << ICN::GetString(icn));
	Error::Except(__FUNCTION__, "load icn");
    }
}

void AGG::Cache::LoadOrgICN(icn_cache_t & v, const ICN::icn_t icn, const u16 index, bool reflect)
{
    if(NULL == v.sprites)
    {
	std::vector<u8> body;
	ReadChunk(ICN::GetString(icn), body);

	v.count = ReadLE16(&body[0]);
	v.sprites = new Sprite [v.count];
	v.reflect = new Sprite [v.count];
    }

    Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];

    LoadOrgICN(sp, icn, index, reflect);
}

/* load ICN object to AGG::Cache */
void AGG::Cache::LoadICN(const ICN::icn_t icn, u16 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    if(reflect)
    {
	if(v.reflect && (index >= v.count || v.reflect[index].isValid())) return;
    }
    else
    {
	if(v.sprites && (index >= v.count || v.sprites[index].isValid())) return;
    }

    const Settings & conf = Settings::Get();
    bool skip_origin = false;

    // load modify sprite
    if(ICN::isModifiedSprite(icn))
    {
	LoadExtICN(v, icn, index, reflect);
	skip_origin = true;
    }
    else
    // load from images dir
    if(conf.UseAltResource())
    {
	const std::string xml_spec(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "images" + SEPARATOR +
								String::Lower(ICN::GetString(icn)) + SEPARATOR + "spec.xml");
	if(FilePresent(xml_spec) &&
	    LoadAltICN(v, xml_spec, index, reflect)) skip_origin = true;
    }

    //load origin sprite
    if(!skip_origin) LoadOrgICN(v, icn, index, reflect);

    // pocketpc: scale sprites
    if(Settings::Get().QVGA() && ICN::NeedMinify4PocketPC(icn, index))
    {
	Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];
	sp.ScaleMinifyByTwo();
    }

    // registry icn
    if(icn_registry_enable &&
	icn_registry.end() == std::find(icn_registry.begin(), icn_registry.end(), icn))
	icn_registry.push_back(icn);
}

bool AGG::Cache::LoadAltTIL(til_cache_t & v, const std::string & spec, u16 max)
{
#ifdef WITH_XML
    // parse spec.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_til = NULL;

    if(doc.LoadFile(spec.c_str()) &&
	NULL != (xml_til = doc.FirstChildElement("til")))
    {
	int count, index;
	xml_til->Attribute("count", &count);

	if(NULL == v.sprites)
	{
	    v.count = count;
	    v.sprites = new Surface [v.count];
	}

	index = 0;
	for(const TiXmlElement*
	    xml_sprite = xml_til->FirstChildElement("sprite"); index < count && xml_sprite; ++index, xml_sprite = xml_sprite->NextSiblingElement("sprite"))
	{
	    Surface & sf = v.sprites[index];
	    std::string name(spec);
	    String::Replace(name, "spec.xml", xml_sprite->Attribute("name"));

	    if(sf.Load(name.c_str()) && sf.isValid())
	    {
		DEBUG(DBG_ENGINE, DBG_TRACE, spec << ", " << index);
	    }
	}
	return true;
    }
    else
    DEBUG(DBG_ENGINE, DBG_WARN, "broken xml file: " <<  spec);
#endif

    return false;
}

void AGG::Cache::LoadOrgTIL(til_cache_t & v, const TIL::til_t til, u16 max)
{
    std::vector<u8> body;

    if(ReadChunk(TIL::GetString(til), body))
    {
	const u16 count = ReadLE16(&body.at(0));
	const u16 width = ReadLE16(&body.at(2));
	const u16 height= ReadLE16(&body.at(4));

	const u32 tile_size = width * height;
	const u32 body_size = 6 + count * tile_size;

	// check size
	if(body.size() == body_size && count <= max)
	{
	    for(u16 ii = 0; ii < count; ++ii)
		v.sprites[ii].Set(&body[6 + ii * tile_size], width, height, 1, false);
	}
	else
	    DEBUG(DBG_ENGINE, DBG_WARN, "size mismach" << ", skipping...");
    }
}

/* load TIL object to AGG::Cache */
void AGG::Cache::LoadTIL(const TIL::til_t til)
{
    til_cache_t & v = til_cache[til];

    if(v.sprites) return;

    DEBUG(DBG_ENGINE, DBG_INFO, TIL::GetString(til));
    u16 max = 0;

    switch(til)
    {
	case TIL::CLOF32:	max = 4;   break;
        case TIL::GROUND32:	max = 432; break;
        case TIL::STON:		max = 36;  break;
	default: break;
    }

    v.count = max * 4;  // rezerve for rotate sprites
    v.sprites = new Surface [v.count];

    const Settings & conf = Settings::Get();
    bool skip_orig = false;

    // load from images dir
    if(conf.UseAltResource())
    {
	const std::string xml_spec(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "images" + SEPARATOR +
								String::Lower(TIL::GetString(til)) + SEPARATOR + "spec.xml");
	if(FilePresent(xml_spec) &&
	    LoadAltTIL(v, xml_spec, max)) skip_orig = true;
    }

    if(!skip_orig)
	LoadOrgTIL(v, til, max);
}

/* load 82M object to AGG::Cache in Audio::CVT */
void AGG::Cache::LoadWAV(const M82::m82_t m82)
{
    std::vector<u8> & v = wav_cache[m82];

    if(v.size() || !Mixer::isValid()) return;

#ifdef WITH_MIXER
    const Settings & conf = Settings::Get();

    if(conf.UseAltResource())
    {
       std::string name = String::Lower(M82::GetString(m82));
       // ogg
       String::Replace(name, ".82m", ".ogg");
       std::string sound = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "sounds" + SEPARATOR + name;

       if(LoadFileToMem(v, sound))
       {
           DEBUG(DBG_ENGINE, DBG_INFO, sound);
           return;
       }

       // mp3
       String::Replace(name, ".82m", ".mp3");
       sound = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "sounds" + SEPARATOR + name;

       if(LoadFileToMem(v, sound))
       {
           DEBUG(DBG_ENGINE, DBG_INFO, sound);
           return;
       }
    }
#endif

    DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
    std::vector<u8> body;

#ifdef WITH_MIXER
    if(ReadChunk(M82::GetString(m82), body))
    {
	// create WAV format
	v.resize(body.size() + 44);

	WriteLE32(&v[0], 0x46464952);		// RIFF
	WriteLE32(&v[4], body.size() + 0x24);	// size
	WriteLE32(&v[8], 0x45564157);		// WAVE
	WriteLE32(&v[12], 0x20746D66);		// FMT
	WriteLE32(&v[16], 0x10);		// size_t
	WriteLE16(&v[20], 0x01);		// format
	WriteLE16(&v[22], 0x01);		// channels
	WriteLE32(&v[24], 22050);		// samples
	WriteLE32(&v[28], 22050);		// byteper
	WriteLE16(&v[32], 0x01);		// align
	WriteLE16(&v[34], 0x08);		// bitsper
	WriteLE32(&v[36], 0x61746164);		// DATA
	WriteLE32(&v[40], body.size());		// size

	std::copy(body.begin(), body.end(), &v[44]);
    }
#else
    Audio::Spec wav_spec;
    wav_spec.format = AUDIO_U8;
    wav_spec.channels = 1;
    wav_spec.freq = 22050;

    const Audio::Spec & hardware = Audio::GetHardwareSpec();

    Audio::CVT cvt;

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
#endif
}

/* load XMI object to AGG::Cache */
void AGG::Cache::LoadMID(const XMI::xmi_t xmi)
{
    std::vector<u8> & v = mid_cache[xmi];

    if(v.size()) return;

    DEBUG(DBG_ENGINE, DBG_INFO, XMI::GetString(xmi));

    if(! Mixer::isValid()) return;

    std::vector<u8> body;

    if(ReadChunk(XMI::GetString(xmi), body))
    {
	MIDI::Xmi x;
	MIDI::Mid m;
	MIDI::MTrk track;

	x.Read(body);
	track.ImportXmiEVNT(x.EVNT());

	m.AddTrack(track);
	m.SetPPQN(64);

	m.Write(v);
    }
}

void AGG::Cache::LoadPAL(void)
{
}

void AGG::Cache::LoadFNT(void)
{
#ifdef WITH_TTF
    const Settings & conf = Settings::Get();

    if(conf.Unicode())
    {
	if(fnt_cache.size()) return;

	const std::string letters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	std::vector<u16> unicode = String::UTF8_to_UNICODE(letters);

	for(std::vector<u16>::const_iterator
	    it = unicode.begin(); it != unicode.end(); ++it)
	    LoadFNT(*it);

	if(fnt_cache.size())
	{
    	    DEBUG(DBG_ENGINE, DBG_INFO, "normal fonts " << conf.FontsNormal());
    	    DEBUG(DBG_ENGINE, DBG_INFO, "small fonts " << conf.FontsSmall());
    	    DEBUG(DBG_ENGINE, DBG_INFO, "preload english charsets");
	}
	else
	    DEBUG(DBG_ENGINE, DBG_INFO, "use bitmap fonts");
    }
    else
#endif
    {
	PreloadObject(ICN::SMALFONT);
	PreloadObject(ICN::FONT);

	DEBUG(DBG_ENGINE, DBG_INFO, "use bitmap fonts");
    }
}

#ifdef WITH_TTF
void AGG::Cache::LoadFNT(u16 ch)
{
    const Settings & conf = Settings::Get();

    if(conf.Unicode())
    {
        const RGBColor white = { 0xFF, 0xFF, 0xFF, 0x00 };
        const RGBColor yellow= { 0xFF, 0xFF, 0x00, 0x00 };

	// small
	font_small.RenderUnicodeChar(fnt_cache[ch].small_white, ch, white, conf.FontSmallRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	font_small.RenderUnicodeChar(fnt_cache[ch].small_yellow, ch, yellow, conf.FontSmallRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);

	// medium
	if(!(conf.QVGA() && !conf.Unicode()))
	{
	    font_medium.RenderUnicodeChar(fnt_cache[ch].medium_white, ch, white, conf.FontNormalRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	    font_medium.RenderUnicodeChar(fnt_cache[ch].medium_yellow, ch, yellow, conf.FontNormalRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	}

	DEBUG(DBG_ENGINE, DBG_TRACE, "0x" << std::hex << static_cast<int>(ch));
    }
}
#endif

/* free ICN object in AGG::Cache */
void AGG::Cache::FreeICN(const ICN::icn_t icn)
{
    DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(icn));
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
	DEBUG(DBG_ENGINE, DBG_WARN, ICN::GetString(icn) << ", " << "out of range: " << index);
	index = 0;
    }

    // need load?
    if(0 == v.count || ((reflect && (!v.reflect || !v.reflect[index].isValid())) || (!v.sprites || !v.sprites[index].isValid())))
	LoadICN(icn, index, reflect);

    // invalid sprite?
    if((reflect && !v.reflect[index].isValid()) || (!reflect && !v.sprites[index].isValid()))
    {
	DEBUG(DBG_ENGINE, DBG_INFO, "invalid sprite: " << ICN::GetString(icn) << ", index: " << index << ", reflect: " << (reflect ? "true" : "false"));
    }

    return reflect ? v.reflect[index] : v.sprites[index];
}

/* return count of sprites from specific ICN */
int AGG::Cache::GetICNCount(const ICN::icn_t icn)
{
    if(icn_cache[icn].count == 0) AGG::GetICN(icn, 0);
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
	DEBUG(DBG_ENGINE, DBG_WARN, TIL::GetString(til) << ", " << "out of range: " << index);
	index2 = 0;
    }

    Surface & surface = v.sprites[index2];

    if(shape && ! surface.isValid())
    {
	const Surface & src = v.sprites[index];

	if(src.isValid())
	{
	    Surface::Reflect(surface, src, shape);
	}
	else
	DEBUG(DBG_ENGINE, DBG_WARN, "is NULL");
    }

    if(! surface.isValid())
    {
	DEBUG(DBG_ENGINE, DBG_WARN, "invalid sprite: " << TIL::GetString(til) << ", index: " << index);
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

#ifdef WITH_TTF
/* return FNT cache */
const Surface & AGG::Cache::GetFNT(u16 c, u8 f)
{
    if(!fnt_cache[c].small_white.isValid()) LoadFNT(c);

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
    for(; it1 != it2; ++it1) if(!ICN::SkipRegistryFree(*it1)) FreeICN(*it1);
}

void AGG::Cache::Dump(void) const
{
    std::ostringstream os;

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
		os << ICN::GetString((ICN::icn_t) ii) << "(" << icn_cache[ii].count << ", " << total2 << "), ";
	    total1 += total2;
        }
	if(total1)
	{
	    DEBUG(DBG_ENGINE, DBG_TRACE, os.str());
	    DEBUG(DBG_ENGINE, DBG_INFO, "ICN" << " total: " << total1 << " bytes");
	    os.clear();
	}
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
		os << TIL::GetString((TIL::til_t) ii) << "(" << til_cache[ii].count << ", " << total2 << "), ";
	    total1 += total2;
        }
	if(total1)
	{
	    DEBUG(DBG_ENGINE, DBG_TRACE, os.str());
	    DEBUG(DBG_ENGINE, DBG_INFO, "TIL" << " total: " << total1 << " bytes");
	    os.clear();
	}
    }

    if(wav_cache.size())
    {
	total1 = 0;
	for(std::map<M82::m82_t, std::vector<u8> >::const_iterator
	    it = wav_cache.begin(); it != wav_cache.end(); ++it)
	{
	    if((*it).second.size())
	    	os << M82::GetString((*it).first) << "(" << (*it).second.size() << ",) ";
	    total1 += (*it).second.size();
	}
	if(total1)
	{
	    DEBUG(DBG_ENGINE, DBG_TRACE, os.str());
	    DEBUG(DBG_ENGINE, DBG_INFO, "WAV" << " total: " << total1 << " bytes");
	    os.clear();
	}
    }

    if(mid_cache.size())
    {
	total1 = 0;
	for(std::map<XMI::xmi_t, std::vector<u8> >::const_iterator
	    it = mid_cache.begin(); it != mid_cache.end(); ++it)
	{
	    if((*it).second.size())
	    	os << XMI::GetString((*it).first) << "(" << (*it).second.size() << "), ";
	    total1 += (*it).second.size();
	}
	if(total1)
	{
	    DEBUG(DBG_ENGINE, DBG_TRACE, os.str());
	    DEBUG(DBG_ENGINE, DBG_INFO, "MID" << " total: " << total1 << " bytes");
	    os.clear();
	}
    }

#ifdef WITH_TTF
    if(fnt_cache.size())
    {
	total1 = 0;
	for(std::map<u16, fnt_cache_t>::const_iterator
	    it = fnt_cache.begin(); it != fnt_cache.end(); ++it)
	    total1 += ((*it).second.medium_white.GetSize() + (*it).second.medium_yellow.GetSize() +
				(*it).second.small_white.GetSize() + (*it).second.small_yellow.GetSize());
	if(total1)
	{
	    DEBUG(DBG_ENGINE, DBG_INFO, "FNT" << " total: " << total1 << " bytes");
	    os.clear();
	}
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

void AGG::Cache::ResetMixer(void)
{
    Mixer::Reset();
    loop_sounds.clear();
    loop_sounds.reserve(7);
}

void AGG::Cache::LoadLOOPXXSounds(const u16* vols)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound() && vols)
    {
	// set volume loop sounds
	for(u8 channel = 0; channel != LOOPXX_COUNT; ++channel)
	{
	    u16 vol = vols[channel];
	    M82::m82_t m82 = M82::GetLOOP00XX(channel);
	    if(M82::UNKNOWN == m82) continue;

	    // find loops
	    std::vector<loop_sound_t>::iterator it = std::find_if(loop_sounds.begin(), loop_sounds.end(),
		    std::bind2nd(std::mem_fun_ref(&loop_sound_t::isM82), m82));

	    if(it != loop_sounds.end())
	    {
		// unused and free
		if(0 == vol)
		{
		    if(Mixer::isPlaying((*it).channel))
		    {
			Mixer::Pause((*it).channel);
			Mixer::Volume((*it).channel, Mixer::MaxVolume() * conf.SoundVolume() / 10);
			Mixer::Stop((*it).channel);
		    }
		    (*it).sound = M82::UNKNOWN;
		}
		// used and set vols
		else
		if(Mixer::isPlaying((*it).channel))
		{
		    Mixer::Pause((*it).channel);
		    Mixer::Volume((*it).channel, vol * conf.SoundVolume() / 10);
		    Mixer::Resume((*it).channel);
		}
	    }
	    else
	    // new sound
	    if(0 != vol)
	    {
    		const std::vector<u8> & v = AGG::Cache::Get().GetWAV(m82);
		int ch = Mixer::Play(&v[0], v.size(), -1, true);

		if(0 <= ch)
		{
		    Mixer::Pause(ch);
		    Mixer::Volume(ch, vol * conf.SoundVolume() / 10);
		    Mixer::Resume(ch);

		    // find unused
		    std::vector<loop_sound_t>::iterator it = std::find_if(loop_sounds.begin(), loop_sounds.end(),
			    std::bind2nd(std::mem_fun_ref(&loop_sound_t::isM82), M82::UNKNOWN));

		    if(it != loop_sounds.end())
		    {
			(*it).sound = m82;
			(*it).channel = ch;
		    }
		    else
			loop_sounds.push_back(loop_sound_t(m82, ch));

		    DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
		}
	    }
	}
    }
}

/* wrapper Audio::Play */
void AGG::PlaySound(const M82::m82_t m82)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
	const std::vector<u8> & v = AGG::Cache::Get().GetWAV(m82);
	int ch = Mixer::Play(&v[0], v.size(), -1, false);
	Mixer::Pause(ch);
	Mixer::Volume(ch, Mixer::MaxVolume() * conf.SoundVolume() / 10);
	Mixer::Resume(ch);
    }
}

/* wrapper Audio::Play */
void AGG::PlayMusic(const MUS::mus_t mus, bool loop)
{
    const Settings & conf = Settings::Get();

    if(!conf.Music() || MUS::UNUSED == mus || MUS::UNKNOWN == mus || (Game::CurrentMusic() == mus && Music::isPlaying())) return;

    Game::SetCurrentMusic(mus);
    
    if(conf.MusicExt())
    {
	const std::string musname(conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "music" + SEPARATOR + MUS::GetString(mus));
#ifdef WITH_MIXER
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
		DEBUG(DBG_ENGINE, DBG_WARN, "error read file: " << musname << ", skipping...");
	}

	if(filename) Music::Play(filename, loop);
#else
	if(FilePresent(musname) && conf.PlayMusCommand().size())
	{
	    const std::string run = conf.PlayMusCommand() + " " + musname;
	    Music::Play(run.c_str(), loop);
	}
#endif
	DEBUG(DBG_ENGINE, DBG_INFO, MUS::GetString(mus));
    }
    else
#ifdef WITH_AUDIOCD
    if(conf.MusicCD() && Cdrom::isValid())
    {
	Cdrom::Play(mus, loop);
	DEBUG(DBG_ENGINE, DBG_INFO, "cd track " << static_cast<int>(mus));
    }
    else
#endif
    if(conf.MusicMIDI())
    {
	XMI::xmi_t xmi = XMI::FromMUS(mus);
	if(XMI::UNKNOWN != xmi)
	{
#ifdef WITH_MIXER
	    const std::vector<u8> & v = AGG::Cache::Get().GetMID(xmi);
	    if(v.size()) Music::Play(&v[0], v.size(), loop);
#else
	    if(conf.PlayMusCommand().size())
	    {
		const std::string file = conf.LocalPrefix() + SEPARATOR + "files" + SEPARATOR + "music" + SEPARATOR + XMI::GetString(xmi);
		if(FilePresent(file))
		{
		    const std::string run = conf.PlayMusCommand() + " " + file;
		    Music::Play(run.c_str(), loop);
		}
		else
		    SaveMemToFile(AGG::Cache::Get().GetMID(xmi), file);
	    }
#endif
	}
	DEBUG(DBG_ENGINE, DBG_INFO, XMI::GetString(xmi));
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
    if(ch < 0x21) DEBUG(DBG_ENGINE, DBG_WARN, "unknown letter");

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

void AGG::ResetMixer(void)
{
    AGG::Cache::Get().ResetMixer();
}
