/***************************************************************************
 *   Copyright (C) 2008 by Josh Matthews                                   *
 *   josh@joshmatthews.net                                                 *
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

#include <sstream>
#include <iomanip>
#include "race.h"
#include "ground.h"
#include "mus.h"

namespace MUS
{
    static const musmap_t musmap[] = 
    {
        { UNUSED,       ""                    },
        { DATATRACK,    ""                    },
        { BATTLE1,      "Battle (1)"          },
        { BATTLE2,      "Battle (2)"          },
        { BATTLE3,      "Battle (3)"          },
        { BARBARIAN,    "Barbarian Castle"    },
        { SORCERESS,    "Sorceress Castle"    },
        { WARLOCK,      "Warlock Castle"      },
        { KNIGHT,       "Knight Castle"       },
        { NECROMANCER,  "Necromancer Castle"  },
        { WIZARD,       "Wizard Castle"       },
        { LAVA,         "Lava Theme"          },
        { WASTELAND,    "Wasteland Theme"     },
        { DESERT,       "Desert Theme"        },
        { SNOW,         "Snow Theme"          },
        { SWAMP,        "Swamp Theme"         },
        { BEACH,        "Beach Theme"         },
        { DIRT,         "Dirt Theme"          },
        { GRASS,        "Grass Theme"         },
        { LOSTGAME,     "Lost Game"           },
        { WEEK1,        "Week (1)"            },
        { WEEK2_MONTH1, "Week (2) Month (1)"  },
        { MONTH2,       "Month (2)"           },
        { PUZZLE,       "Map Puzzle"          },
        { ROLAND,       "Roland's Campaign"   },
        { EVENT1,       "25"                  },
        { EVENT2,       "26"                  },
        { EVENT3,       "27"                  },
        { COMPUTER,     "28"                  },
        { BATTLEWIN,    "29"                  },
        { BATTLELOSE,   "30"                  },
        { EVENT6,       "31"                  },
        { EVENT7,       "32"                  },
        { EVENT8,       "33"                  },
        { EVENT9,       "34"                  },
        { EVENT10,      "35"                  },
        { EVENT11,      "36"                  },
        { EVENT12,      "37"                  },
        { EVENT13,      "38"                  },
        { EVENT14,      "39"                  },
        { EVENT15,      "40"                  },
        { EVENT16,      "41"                  },
        { MAINMENU,     "Main Menu"           },
        { VICTORY,      "Scenario Victory"    },
        { UNKNOWN,      "???"                 }
    };
    
    const std::string GetString(const mus_t mus)
    {
      std::stringstream sstream;
      sstream << std::setw(2) << std::setfill('0') << (int)mus << " " << musmap[mus].string << ".ogg";
      return sstream.str();
    }
}

MUS::mus_t MUS::FromGround(const u16 ground)
{
    switch(ground)
    {
        case Maps::Ground::DESERT:	return DESERT;
        case Maps::Ground::SNOW:	return SNOW;
        case Maps::Ground::SWAMP:	return SWAMP;
        case Maps::Ground::WASTELAND:	return WASTELAND;
        case Maps::Ground::BEACH:	return BEACH;
        case Maps::Ground::LAVA:	return LAVA;
        case Maps::Ground::DIRT:	return DIRT;
        case Maps::Ground::GRASS:	return GRASS;
        default: break;
    }

    return UNKNOWN;
}

MUS::mus_t MUS::FromRace(const u8 race)
{
    switch(race)
    {
        case Race::KNGT:	return KNIGHT;
        case Race::BARB:	return BARBARIAN;
        case Race::SORC:	return SORCERESS;
        case Race::WRLK:	return WARLOCK;
        case Race::WZRD:	return WIZARD;
        case Race::NECR:	return NECROMANCER;
        default: break;
    }

    return UNKNOWN;
}
