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
#ifndef H2MP2_H
#define H2MP2_H

#include <vector>
#include "gamedefs.h"

#define MP2OFFSETDATA	428
#define SIZEOFMP2TILE	20
#define SIZEOFMP2ADDON	15

namespace MP2
{
    // origin mp2
    typedef struct {
        u16  tileIndex;		// tile (ocean, grass, snow, swamp, lava, desert, dirt, wasteland, beach)
        u8   objectName1;	// level 1.0
        u8   indexName1;	// index level 1.0 or 0xFF
        u8   quantity1;		// count
        u8   quantity2;		// count
        u8   objectName2;	// level 2.0
        u8   indexName2;	// index level 2.0 or 0xFF
        u8   shape;		// shape reflect % 4, 0 none, 1 vertical, 2 horizontal, 3 any
        u8   generalObject;	// zero or object
	u16  indexAddon;	// zero or index addons_t
	u32  uniqNumber1;	// level 1.0
	u32  uniqNumber2;	// level 2.0
    } mp2tile_t;

    // origin mp2
    typedef struct {
	u16      indexAddon;	// zero or next addons_t
	u8       objectNameN1;	// level 1.N
	u8       indexNameN1;	// level 1.N or 0xFF
	u8       quantityN;	//
	u8       objectNameN2;	// level 2.N
	u8       indexNameN2;	// level 1.N or 0xFF
	u32      uniqNumberN1;	// level 1.N
	u32      uniqNumberN2;	// level 2.N
    } mp2addon_t;

    typedef struct {
        u8      object1;
        u8      index1;
        u8      quantity1;
        u8      object2;
        u8      index2;
    } addon_t;

    typedef struct {
        u16     tile;
        std::vector<addon_t> addons;
        u8      quantity2;
        u8      shape;
        u8      general;
    } tile_t;

    typedef enum {
	OBJ_ZERO		= 0x00,
	OBJN_ALCHEMYTOWER	= 0x01,
	OBJN_DAEMONCAVE		= 0x05,
	OBJN_FAERIERING		= 0x07,
	OBJN_GRAVEYARD		= 0x0C,
			 
	OBJN_DRAGONCITY		= 0x14,
	OBJN_LIGHTHOUSE		= 0x15,
	OBJN_WATERMILL		= 0x16,
	OBJN_MINES		= 0x17,
	OBJN_OBELISK		= 0x19,
	OBJN_OASIS		= 0x1A,
	OBJ_COAST		= 0x1C,
	OBJN_SAWMILL		= 0x1D,
	OBJN_ORACLE		= 0x1E,
			 
	OBJN_DERELICTSHIP	= 0x20,
	OBJN_DESERTTENT		= 0x22,
	OBJN_CASTLE		= 0x23,
	OBJN_WAGONCAMP		= 0x25,
	OBJN_WINDMILL		= 0x28,
			 
	OBJN_RNDTOWN		= 0x30,
	OBJN_RNDCASTLE		= 0x31,
	OBJ_SMALLCRACK		= 0x38,
	OBJN_TREECITY		= 0x3C,
	OBJN_RUINS		= 0x3D,
	OBJN_FORT		= 0x3E,
	OBJN_TRADINGPOST	= 0x3F,
			 
	OBJN_ABANDONEDMINE	= 0x40,
	OBJN_TREEKNOWLEDGE	= 0x44,
	OBJN_DOCTORHUNT		= 0x45,
	OBJN_TEMPLE		= 0x46,
	OBJN_HILLFORT		= 0x47,
	OBJN_HALFLINGHOLE	= 0x48,
	OBJN_MERCENARYCAMP	= 0x49,
	OBJN_PIRAMID		= 0x4C,
	OBJN_CITYDEAD		= 0x4D,
	OBJN_EXCAVATION		= 0x4E,
	OBJN_SPHINX		= 0x4F,
			 
	OBJ_OILLAKE		= 0x51,
	OBJN_TROLLBRIDGE	= 0x53,
	OBJN_WITCHHUNT		= 0x55,
	OBJN_XANADU		= 0x56,
	OBJN_CAVE		= 0x57,
	OBJN_MAGELLANMAPS	= 0x59,
	OBJN_SHIPWRECK		= 0x5B,
			 
	OBJN_OBSERVATIONTOWER	= 0x60,
	OBJN_FREEMANFOUNDRY	= 0x61,
	OBJ_TREES		= 0x63,
	OBJ_MOUNTS		= 0x64,
	OBJ_VOLCANO		= 0x65,
	OBJ_STONES		= 0x67,
	OBJ_VEGETATION1		= 0x66,
	OBJ_WATERLAKE		= 0x68,
	OBJ_VEGETATION2		= 0x69,
	OBJ_FIRTREES		= 0x6A,
	OBJ_BIGCRACK		= 0x6C,
			 
	OBJ_LAVALAKE		= 0x70,
	OBJ_VEGETATION3		= 0x71,
			 
	OBJ_ALCHEMYTOWER	= 0x81,
	OBJ_SIGN		= 0x82,
	OBJ_BUOY		= 0x83,
	OBJ_SKELETON		= 0x84,
	OBJ_DAEMONCAVE		= 0x85,
	OBJ_TREASURECHEST	= 0x86,
	OBJ_FAERIERING		= 0x87,
	OBJ_CAMPFIRE		= 0x88,
	OBJ_FOUNTAIN		= 0x89,
	OBJ_GAZEBO		= 0x8A,
	OBJ_ANCIENTLAMP		= 0x8B,
	OBJ_GRAVEYARD		= 0x8C,
	OBJ_ARCHERHOUSE		= 0x8D,
	OBJ_GOBLINHUNT		= 0x8E,
	OBJ_DWARFCOTT		= 0x8F,
			 
	OBJ_PEASANTHUNT		= 0x90,
	OBJ_EVENT		= 0x93,
	OBJ_DRAGONCITY		= 0x94,
	OBJ_LIGHTHOUSE		= 0x95,
	OBJ_WATERMILL		= 0x96,
	OBJ_MINES		= 0x97,
	OBJ_MONSTER		= 0x98,
	OBJ_OBELISK		= 0x99,
	OBJ_OASIS		= 0x9A,
	OBJ_RESOURCE		= 0x9B,
	OBJ_SAWMILL		= 0x9D,
	OBJ_ORACLE		= 0x9E,
	OBJ_SHRINE1		= 0x9F,
			 
	OBJ_DERELICTSHIP	= 0xA0,
	OBJ_DESERTTENT		= 0xA2,
	OBJ_CASTLE		= 0xA3,
	OBJ_STONELITHS		= 0xA4,
	OBJ_WAGONCAMP		= 0xA5,
	OBJ_WHIRLPOOL		= 0xA7,
	OBJ_WINDMILL		= 0xA8,
	OBJ_ARTIFACT		= 0xA9,
	OBJ_BOAT		= 0xAB,
	OBJ_RNDULTIMATEARTIFACT	= 0xAC,
	OBJ_RNDARTIFACT		= 0xAD,
	OBJ_RNDRESOURCE		= 0xAE,
	OBJ_RNDMONSTER		= 0xAF,
			 
	OBJ_RNDTOWN		= 0xB0,
	OBJ_RNDCASTLE		= 0xB1,
	OBJ_RNDMONSTER1		= 0xB3,
	OBJ_RNDMONSTER2		= 0xB4,
	OBJ_RNDMONSTER3		= 0xB5,
	OBJ_RNDMONSTER4		= 0xB6,
	OBJ_HEROES		= 0xB7,
	OBJ_WATCHTOWER		= 0xBA,
	OBJ_TREEHOUSE		= 0xBB,
	OBJ_TREECITY		= 0xBC,
	OBJ_RUINS		= 0xBD,
	OBJ_FORT		= 0xBE,
	OBJ_TRADINGPOST		= 0xBF,
			 
	OBJ_ABANDONEDMINE	= 0xC0,
	OBJ_PEASANTHUNT2	= 0xC1,
	OBJ_STANDINGSTONES	= 0xC2,
	OBJ_IDOL		= 0xC3,
	OBJ_TREEKNOWLEDGE	= 0xC4,
	OBJ_DOCTORHUNT		= 0xC5,
	OBJ_TEMPLE		= 0xC6,
	OBJ_HILLFORT		= 0xC7,
	OBJ_HALFLINGHOLE	= 0xC8,
	OBJ_MERCENARYCAMP	= 0xC9,
	OBJ_SHRINE2		= 0xCA,
	OBJ_SHRINE3		= 0xCB,
	OBJ_PIRAMID		= 0xCC,
	OBJ_CITYDEAD		= 0xCD,
	OBJ_EXCAVATION		= 0xCE,
	OBJ_SPHINX		= 0xCF,
			 
	OBJ_WAGON		= 0xD0,
	OBJ_ARTESIANSPRING	= 0xD2,
	OBJ_TROLLBRIDGE		= 0xD3,
	OBJ_CRAKEDLAKE		= 0xD4,
	OBJ_WITCHHUNT		= 0xD5,
	OBJ_XANADU		= 0xD6,
	OBJ_CAVE		= 0xD7,
	OBJ_LEANTO		= 0xD8,
	OBJ_MAGELLANMAPS	= 0xD9,
	OBJ_FLOTSAM		= 0xDA,
	OBJ_SHIPWRECK		= 0xDB,
	OBJ_SHIPWRECKSURVIROR	= 0xDC,
	OBJ_BOTTLE		= 0xDD,
	OBJ_MAGICWELL		= 0xDE,
	OBJ_MAGICGARDEN		= 0xDF,
			 
	OBJ_OBSERVATIONTOWER	= 0xE0,
	OBJ_FREEMANFOUNDRY	= 0xE1,
			 
	OBJ_RNDARTIFACT1	= 0xF4,
	OBJ_RNDARTIFACT2	= 0xF5,
	OBJ_RNDARTIFACT3	= 0xF6,

    } object_t;

    const char *GetICNObject(u8 type);
    bool StaticObject(u8 type, u8 index);
    Maps::ground_t GetTypeGrounds(const MP2::tile_t &info);
    bool VectorAddonSort(const MP2::addon_t & lhs, const MP2::addon_t & rhs);
};

#endif
