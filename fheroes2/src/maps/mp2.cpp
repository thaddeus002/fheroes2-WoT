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

#include "config.h"
#include "error.h"
#include "mp2.h"

/* return name icn object */
ICN::icn_t MP2::GetICNObject(const u8 type)
{
    switch(type)
    {
	// artifact
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
	    return ICN::OBJNARTI;

	// monster
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	    return ICN::UNKNOWN;

	// castle flags
	case 0x38:
	    return ICN::FLAG32;

	// heroes
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	    break;

	// relief: snow
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	    return ICN::MTNSNOW;

	// relief: swamp
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
	    return ICN::MTNSWMP;

	// relief: lava
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	    return ICN::MTNLAVA;

	// relief: desert
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	    return ICN::MTNDSRT;

	// relief: dirt
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	    return ICN::MTNDIRT;

	// relief: others
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	    return ICN::MTNMULT;

	// mines
	case 0x74:
	    return ICN::EXTRAOVR;

	// road
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	    return ICN::ROAD;

	// relief: crck
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:
	    return ICN::MTNCRCK;

	// relief: gras
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	    return ICN::MTNGRAS;

	// trees jungle
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	    return ICN::TREJNGL;

	// trees evil
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	    return ICN::TREEVIL;

	// castle and tower
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:
	    return ICN::OBJNTOWN;

	// castle lands
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	    return ICN::OBJNTWBA;

	// castle shadow
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	    return ICN::OBJNTWSH;

	// random castle
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	    return ICN::OBJNTWRD;

	// water object
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	    return ICN::OBJNWAT2;

	// object other
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:
	    return ICN::OBJNMUL2;

	// trees snow
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	    return ICN::TRESNOW;

	// trees trefir
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	    return ICN::TREFIR;

	// trees
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	    return ICN::TREFALL;

	// river
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
	    return ICN::STREAM;

	// resource
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	    return ICN::OBJNRSRC;

	// gras object
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	    return ICN::OBJNGRA2;

	// trees tredeci
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:
	    return ICN::TREDECI;

	// sea object
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	    return ICN::OBJNWATR;

	// vegetation gras
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	    return ICN::OBJNGRAS;

	// object on snow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	    return ICN::OBJNSNOW;

	// object on swamp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	    return ICN::OBJNSWMP;

	// object on lava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	    return ICN::OBJNLAVA;

	// object on desert
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
	    return ICN::OBJNDSRT;

	// object on dirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	    return ICN::OBJNDIRT;

	// object on crck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:
	    return ICN::OBJNCRCK;

	// object on lava
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	    return ICN::OBJNLAV3;

	// object on earth
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	    return ICN::OBJNMULT;
	    
	//  object on lava
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	    return ICN::OBJNLAV2;
	    
	default:
	    break;
    }

    if(H2Config::Debug()) Error::Warning("GetICNObject: unknown type: ", type);

    return ICN::UNKNOWN;
}

const char * MP2::StringObject(u8 object)
{
    switch(object){
        case MP2::OBJ_ZERO:			return "OBJ_ZERO";
        case MP2::OBJN_ALCHEMYTOWER:
        case MP2::OBJ_ALCHEMYTOWER:		return "Alchemy Tower";
        case MP2::OBJN_DAEMONCAVE:
        case MP2::OBJ_DAEMONCAVE:		return "Daemon Cave";
        case MP2::OBJN_FAERIERING:
        case MP2::OBJ_FAERIERING:		return "Faerie Ring";
        case MP2::OBJN_GRAVEYARD:
        case MP2::OBJ_GRAVEYARD:		return "Graveyard";
        case MP2::OBJN_DRAGONCITY:
        case MP2::OBJ_DRAGONCITY:		return "Dragon City";
        case MP2::OBJN_LIGHTHOUSE:
        case MP2::OBJ_LIGHTHOUSE:		return "Lighthouse";
        case MP2::OBJN_WATERMILL:
        case MP2::OBJ_WATERMILL:		return "Watermill";
        case MP2::OBJN_MINES:
        case MP2::OBJ_MINES:			return "Mines";
        case MP2::OBJN_OBELISK:
        case MP2::OBJ_OBELISK:			return "Obelisk";
        case MP2::OBJN_OASIS:
        case MP2::OBJ_OASIS:			return "Oasis";
        case MP2::OBJN_SAWMILL:
        case MP2::OBJ_SAWMILL:			return "Sawmill";
        case MP2::OBJN_ORACLE:
        case MP2::OBJ_ORACLE:			return "Oracle";
        case MP2::OBJN_SHIPWRECK:
        case MP2::OBJ_SHIPWRECK:		return "Ship Wreck";
        case MP2::OBJN_DESERTTENT:
        case MP2::OBJ_DESERTTENT:		return "Desert Tent";
        case MP2::OBJN_CASTLE:
        case MP2::OBJ_CASTLE:			return "Castle";
        case MP2::OBJN_WAGONCAMP:
        case MP2::OBJ_WAGONCAMP:		return "Wagon Camp";
        case MP2::OBJN_WINDMILL:
        case MP2::OBJ_WINDMILL:			return "Windmill";
        case MP2::OBJN_RNDTOWN:
        case MP2::OBJ_RNDTOWN:			return "Random Town";
        case MP2::OBJN_RNDCASTLE:
        case MP2::OBJ_RNDCASTLE:		return "Random Castle";
        case MP2::OBJN_WATCHTOWER:
        case MP2::OBJ_WATCHTOWER:            	return "Watch Tower";
        case MP2::OBJN_TREECITY:
        case MP2::OBJ_TREECITY:			return "Tree City";
        case MP2::OBJN_TREEHOUSE:
        case MP2::OBJ_TREEHOUSE:             	return "Tree House";
        case MP2::OBJN_RUINS:
        case MP2::OBJ_RUINS:			return "Ruins";
        case MP2::OBJN_FORT:
        case MP2::OBJ_FORT:			return "Fort";
        case MP2::OBJN_TRADINGPOST:
        case MP2::OBJ_TRADINGPOST:		return "Trading Post";
        case MP2::OBJN_ABANDONEDMINE:
        case MP2::OBJ_ABANDONEDMINE:		return "Abandone Mine";
        case MP2::OBJN_TREEKNOWLEDGE:
        case MP2::OBJ_TREEKNOWLEDGE:		return "Tree Knowledge";
        case MP2::OBJN_DOCTORHUNT:
        case MP2::OBJ_DOCTORHUNT:		return "Doctor Hunt";
        case MP2::OBJN_TEMPLE:
        case MP2::OBJ_TEMPLE:			return "Temple";
        case MP2::OBJN_HILLFORT:
        case MP2::OBJ_HILLFORT:			return "Hill Fort";
        case MP2::OBJN_HALFLINGHOLE:
        case MP2::OBJ_HALFLINGHOLE:		return "Halfling Hole";
        case MP2::OBJN_MERCENARYCAMP:
        case MP2::OBJ_MERCENARYCAMP:		return "Mercenary Camp";
        case MP2::OBJN_PIRAMID:
        case MP2::OBJ_PIRAMID:			return "Piramid";
        case MP2::OBJN_CITYDEAD:
        case MP2::OBJ_CITYDEAD:			return "City Dead";
        case MP2::OBJN_EXCAVATION:
        case MP2::OBJ_EXCAVATION:		return "Excavation";
        case MP2::OBJN_SPHINX:
        case MP2::OBJ_SPHINX:			return "Sphinx";
        case MP2::OBJN_TROLLBRIDGE:
        case MP2::OBJ_TROLLBRIDGE:		return "Troll Bridge";
        case MP2::OBJN_WITCHHUNT:
        case MP2::OBJ_WITCHHUNT:		return "Witch's Hunt";
        case MP2::OBJN_XANADU:
        case MP2::OBJ_XANADU:			return "Xanadu";
        case MP2::OBJN_CAVE:
        case MP2::OBJ_CAVE:			return "Cave";
        case MP2::OBJN_MAGELLANMAPS:
        case MP2::OBJ_MAGELLANMAPS:		return "Magellan Maps";
        case MP2::OBJN_DERELICTSHIP:
        case MP2::OBJ_DERELICTSHIP:		return "Derelict Ship";
        case MP2::OBJN_OBSERVATIONTOWER:
        case MP2::OBJ_OBSERVATIONTOWER:		return "Observation Tower";
        case MP2::OBJN_FREEMANFOUNDRY:
        case MP2::OBJ_FREEMANFOUNDRY:		return "Freeman Foundry";
        case MP2::OBJN_CRAKEDLAKE:
        case MP2::OBJ_CRAKEDLAKE:            	return "Watering Hole";
        case MP2::OBJN_ARTESIANSPRING:
        case MP2::OBJ_ARTESIANSPRING:        	return "Artesian Spring";
        case MP2::OBJN_GAZEBO:
        case MP2::OBJ_GAZEBO:			return "Gazebo";
        case MP2::OBJN_ARCHERHOUSE:
        case MP2::OBJ_ARCHERHOUSE:		return "Archer House";
        case MP2::OBJN_PEASANTHUNT:
        case MP2::OBJ_PEASANTHUNT:		return "Peasant Hunt";
        case MP2::OBJN_DWARFCOTT:
        case MP2::OBJ_DWARFCOTT:		return "Dwarf Cott";
        case MP2::OBJN_STONELIGHTS:
        case MP2::OBJ_STONELIGHTS:		return "Stonelights";
        case MP2::OBJN_MAGICWELL:
        case MP2::OBJ_MAGICWELL:             	return "Magic Well";
        case MP2::OBJ_HEROES:			return "Heroes";
        case MP2::OBJ_SIGN:			return "Sign";
        case MP2::OBJ_SHRUB2:			return "Shrub";
        case MP2::OBJ_NOTHINGSPECIAL:		return "Nothing Special";
        case MP2::OBJ_OILLAKE:			return "Tar Pit";
        case MP2::OBJ_COAST:			return "Coast";
        case MP2::OBJ_MOUND:			return "Mound";
        case MP2::OBJ_DUNE:			return "Dune";
	case MP2::OBJ_STUMP:			return "Stump";
	case MP2::OBJ_CACTUS:			return "Cactus";
        case MP2::OBJ_TREES:			return "Trees";
        case MP2::OBJ_DEADTREE:			return "Dead Tree";
        case MP2::OBJ_MOUNTS:			return "Mountains";
        case MP2::OBJ_VOLCANO:			return "Volcano";
        case MP2::OBJ_STONES:			return "Stones";
        case MP2::OBJ_FLOWERS:			return "Flowers";
        case MP2::OBJ_WATERLAKE:		return "Water Lake";
        case MP2::OBJ_VEGETATION2:		return "Vegetation";
        case MP2::OBJ_CRATER:			return "Crater";
        case MP2::OBJ_LAVALAKE:			return "Lava Lake";
        case MP2::OBJ_SHRUB:			return "Shrub";
        case MP2::OBJ_BUOY:			return "Buoy";
        case MP2::OBJ_SKELETON:			return "Skeleton";
        case MP2::OBJ_TREASURECHEST:		return "Treasure Chest";
        case MP2::OBJ_CAMPFIRE:			return "Camp Fire";
        case MP2::OBJ_FOUNTAIN:			return "Fountain";
        case MP2::OBJ_ANCIENTLAMP:		return "Ancient Lamp";
        case MP2::OBJ_GOBLINHUNT:		return "Goblin Hunt";
        case MP2::OBJ_PEASANTHUNT2:		return "Peasant Hunt";
        case MP2::OBJ_MONSTER:			return "Monster";
        case MP2::OBJ_RESOURCE:			return "Resource";
        case MP2::OBJ_WHIRLPOOL:		return "Whirlpool";
        case MP2::OBJ_ARTIFACT:			return "Artifact";
        case MP2::OBJ_BOAT:			return "Boat";
        case MP2::OBJ_RNDARTIFACT:		return "Random Artifact";
        case MP2::OBJ_RNDRESOURCE:		return "Random Resource";
        case MP2::OBJ_RNDMONSTER1:           	return "OBJ_RNDMONSTER1";
        case MP2::OBJ_RNDMONSTER2:           	return "OBJ_RNDMONSTER2";
        case MP2::OBJ_RNDMONSTER3:           	return "OBJ_RNDMONSTER3";
        case MP2::OBJ_RNDMONSTER4:           	return "OBJ_RNDMONSTER4";
        case MP2::OBJ_STANDINGSTONES:        	return "Standing Stones";
        case MP2::OBJ_EVENT:                 	return "OBJ_EVENT";
        case MP2::OBJ_RNDMONSTER:            	return "OBJ_RNDMONSTER";
        case MP2::OBJ_RNDULTIMATEARTIFACT:   	return "OBJ_RNDULTIMATEARTIFACT";
        case MP2::OBJ_IDOL:                  	return "Idol";
        case MP2::OBJ_SHRINE1:               	return "Shrine of Level 1";
        case MP2::OBJ_SHRINE2:               	return "Shrine of Level 2";
        case MP2::OBJ_SHRINE3:               	return "Shrine of Level 3";
        case MP2::OBJ_WAGON:                 	return "Wagon";
        case MP2::OBJ_LEANTO:                	return "Leanto";
        case MP2::OBJ_FLOTSAM:               	return "Flotsam";
        case MP2::OBJ_SHIPWRECKSURVIROR:     	return "Shipwreck Surviror";
        case MP2::OBJ_BOTTLE:                	return "Bottle";
        case MP2::OBJ_MAGICGARDEN:           	return "Magic Garden";
        case MP2::OBJ_RNDARTIFACT1:          	return "OBJ_RNDARTIFACT1";
        case MP2::OBJ_RNDARTIFACT2:          	return "OBJ_RNDARTIFACT2";
        case MP2::OBJ_RNDARTIFACT3:          	return "OBJ_RNDARTIFACT3";
	
	default:
	    Error::Warning("MP2::Object: unknown, ", object);
	    break;
    }
    
    return "";
}

u8 MP2::GetAnimationFrame(u8 object, u8 index, u32 frame)
{
    switch(object)
    {
	// minimon
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:

	    return index % 9 ? 0 : index + (frame % 6) + 1;

	// objnwatr
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:

	    switch(index)
	    {
		// buttle
		case 0x00:
		    return index + (frame % 11) + 1;

		// shadow
		case 0x0C:
		// chest
		case 0x13:
		// shadow
		case 0x26:
		// flotsam
		case 0x2D:
		// unkn
		case 0x37:
		// boat
		case 0x3E:
		// waves
		case 0x45:
		// seaweed
		case 0x4C:
		case 0x53:
		case 0x5A:
		case 0x61:
		case 0x68:
		// sailor-man
		case 0x6F:
		// shadow
		case 0xBC:
		// buoy
		case 0xC3:
		// broken ship (right)
		case 0xE2:
		case 0xE9:
		case 0xF1:
		case 0xF8:
		    return index + (frame % 6) + 1;

		// seagull on stones
		case 0x76:
		case 0x86:
		case 0x96:
		    return index + (frame % 15) + 1;

		// whirlpool
		case 0xCA:
		case 0xCE:
		case 0xD2:
		case 0xD6:
		case 0xDA:
		case 0xDE:
		    return index + (frame % 3) + 1;

		default:
		    return 0;
	    }
	    break;

	// objnwtr2
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:

	    switch(index)
	    {
		// sail broken ship (left)
		case 0x03:
		case 0x0C:
		    return index + (frame % 6) + 1;

		default:
		    return 0;
	    }

	// objncrck
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:

	    switch(index)
	    {
		// pool of oil
		case 0x50:
		case 0x5B:
		case 0x66:
		case 0x71:
		case 0x7C:
		case 0x89:
		case 0x94:
		case 0x9F:
		case 0xAA:
		// smoke from chimney
		case 0xBE:
		// shadow smoke
		case 0xCA:
		    return index + (frame % 10) + 1;

		default:
		    return 0;
	    }

	// objndirt
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:

	    switch(index)
	    {
		// mill
		case 0x99:
		case 0x9D:
		case 0xA1:
		case 0xA5:
		case 0xA9:
		case 0xAD:
		case 0xB1:
		case 0xB5:
		case 0xB9:
		case 0xBD:
		    return index + (frame % 3) + 1;

		default:
		    return 0;
	    }

	// objndsrt
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:

	    switch(index)
	    {
		// campfire
		case 0x36:
		case 0x3D:
		    return index + (frame % 6) + 1;

		default:
		    return 0;
	    }

	// objngras
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:

	    switch(index)
	    {
		// mill
		case 0x17:
		case 0x1B:
		case 0x1F:
		case 0x23:
		case 0x27:
		case 0x2B:
		case 0x2F:
		case 0x33:
		case 0x37:
		case 0x3B:
		    return index + (frame % 3) + 1;

		// smoke from chimney
		case 0x3F:
		case 0x46:
		case 0x4D:
		// archerhouse
		case 0x54:
		// smoke from chimney
		case 0x5D:
		case 0x64:
		// shadow smoke
		case 0x6B:
		// peasanthunt
		case 0x72:
		    return index + (frame % 6) + 1;

		default:
		    return 0;
		    break;
	    }

	// objnlav2
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:

	    switch(index)
	    {
		// middle volcano
		case 0x00:
		// shadow
		case 0x07:
		case 0x0E:
		// lava
		case 0x15:
		    return index + (frame % 6) + 1;

		// small volcano
		// shadow
		case 0x21:
		case 0x2C:
		// lava
		case 0x37:
		case 0x43:
		    return index + (frame % 10) + 1;

		default:
		    return 0;
	    }

	// objnlav3
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:

	    // big volcano
	    switch(index)
	    {
		// smoke
		case 0x00:
		case 0x0F:
		case 0x1E:
		case 0x2D:
		case 0x3C:
		case 0x4B:
		case 0x5A:
		case 0x69:
		case 0x87:
		case 0x96:
		case 0xA5:
		// shadow
		case 0x78:
		case 0xB4:
		case 0xC3:
		case 0xD2:
		case 0xE1:
		    return index + (frame % 14) + 1;

		default:
		    return 0;
	    }

	// objnlava
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:

	    switch(index)
	    {
		// shadow of lava
		case 0x4F:
		case 0x58:
		case 0x62:
		    return index + (frame % 9) + 1;

		default:
		    return 0;
	    }

	// objnmul2
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:

	    switch(index)
	    {
		// lighthouse
		case 0x3D:
		    return index + (frame % 9) + 1;

		// alchemytower
		case 0x1B:
		// watermill
		case 0x53:
		case 0x5A:
		case 0x62:
		case 0x69:
		// fire in wagoncamp
		case 0x81:
		// smoke smithy (2 chimney)
		case 0xA6:
		// smoke smithy (1 chimney)
		case 0xAD:
		// shadow smoke
		case 0xB4:
		// magic garden
		case 0xBE:

		    return index + (frame % 6) + 1;

		default:
		    return 0;
	    }
	    

	// objnmult
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:

	    switch(index)
	    {
		// smoke
		case 0x05:
		// shadow
		case 0x0F:
		case 0x19:
		    return index + (frame % 9) + 1;

		// smoke
		case 0x24:
		// shadow
		case 0x2D:
		    return index + (frame % 8) + 1;

		// smoke
		case 0x5A:
		// shadow
		case 0x61:
		case 0x68:
		case 0x7C:
		// campfire
		case 0x83:
		    return index + (frame % 6) + 1;

		default:
		    return 0;
	    }

	// objnsnow
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:

	    switch(index)
	    {
		// firecamp
		case 0x04:
		// alchemytower
		case 0x97:
		// watermill
		case 0xA2:
		case 0xA9:
		case 0xB1:
		case 0xB8:
		    return index + (frame % 6) + 1;

		// mill
		case 0x60:
		case 0x64:
		case 0x68:
		case 0x6C:
		case 0x70:
		case 0x74:
		case 0x78:
		case 0x7C:
		case 0x80:
		case 0x84:
		    return index + (frame % 3) + 1;

		default:
		    return 0;
	    }

	// objnswamp
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:

	    switch(index)
	    {
		// shadow
		case 0x00:
		case 0x0E:
		case 0x2B:
		// smoke
		case 0x07:
		case 0x22:
		case 0x33:
		// light in window
		case 0x16:
		case 0x3A:
		case 0x43:
		case 0x4A:
		    return index + (frame % 6) + 1;
    
		default:
		    return 0;
	    }

	default:
	    break;
    }

    return 0;
}
