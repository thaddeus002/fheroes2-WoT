/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include <stdlib.h>
#include "SDL.h"

#include "config.h"
#include "resource.h"


E_RESOURCE CheckValidResource(Uint8 type){

    E_RESOURCE a;
        
    for(a = WOOD; a < RES_NONE; ++a)
        if(a == type)
            return a;

    if(GetIntValue(DEBUG))
        fprintf(stderr, "CheckValidResource: unknown object: 0x%hhX\n", type);

    return RES_NONE;
}


E_RESOURCE GetRNDResource(){

    switch(rand() % 7){
    
	case 0:
	    return WOOD;
	    break;
	
	case 1:
	    return MERCURY;
	    break;
	    
	case 2:
	    return ORE;
	    break;
	    
	case 3:
	    return SULFUR;
	    break;
	
	case 4:
	    return CRYSTAL;
	    break;
	
	case 5:
	    return GEMS;
	    break;
	
	case 6:
	    return GOLDS;
	    break;

	default:
	    break;
    }
    
    return RES_NONE;
}

Uint16 GetCountResource(E_RESOURCE res){

    if(GOLDS == res)
	return 100 * (rand() % (RESOURCEMAXCOUNT - RESOURCEMINCOUNT) + RESOURCEMINCOUNT);
    else
	return rand() % (RESOURCEMAXCOUNT - RESOURCEMINCOUNT) + RESOURCEMINCOUNT;
}

const char * GetDescriptionsResource(E_RESOURCE res){

    const char * string;

    switch(res){
    
	case WOOD:
	    string = "Wood";
	    break;
	
	case MERCURY:
	    string = "Mercury";
	    break;
	    
	case ORE:
	    string = "Ore";
	    break;
	    
	case SULFUR:
	    string = "Sulfur";
	    break;
	
	case CRYSTAL:
	    string = "Crystal";
	    break;
	
	case GEMS:
	    string = "Gems";
	    break;
	
	case GOLDS:
	    string = "Gold";
	    break;

	case LAMP:
	    string = "Lamp";
	    break;

	case RNDRES:
	    string = "Random resource";
	    break;

	case CHEST:
	    string = "Treasure chest";
	    break;

	default:
	    string = "";
	    break;
    }

    return string;
}
