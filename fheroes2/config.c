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

#include <string.h>
#include "SDL.h"
#include "gamedefs.h"
#include "config.h"

typedef struct { char * string; Uint8 value; } S_CONFIG;

void DefaultSettings(S_CONFIG *);

static S_CONFIG *ptrSettings = NULL;

BOOL InitConfig(const char * configFile){

    FILE	*fd;
    char	str[1024], c, *s1, *s2, *e;
    Uint32	value = 0;

    if(NULL == (ptrSettings = (S_CONFIG *) malloc(sizeof(S_CONFIG) * CONFIGEND))){
        fprintf(stderr, "InitConfig: error malloc: %d\n", sizeof(S_CONFIG) * CONFIGEND);
        return FALSE;
    }

    DefaultSettings(ptrSettings);

    // открытие файла конфигурации
    if(NULL == (fd = fopen(configFile, "r"))){
	fprintf(stderr, "InitConfig: %s not found\n", configFile);
	return FALSE;
    }
    
    fprintf(stderr, "InitConfig: read %s\n", configFile);

    // чтение по строкам
    while(fgets(str, sizeof(str), fd)){

	// trim whitespace from the left side
	for(s1 = str; s1[0] == ' ' || s1[0] == '\t'; s1++);

	// trim whitespace from right side.
	e = s1 + strlen(s1);
        while (e > s1 && ((c = e[-1]) == '\n' || c == '\r' || c == ' ' || c == '\t')) e--;
        *e = 0;

        // skip comments and empty lines
        if(*s1 == '#' || *s1 == 0) continue;
	
	//skip if '=' not present
	if(NULL == ( e = strchr(s1, '='))) continue;
	
	// trim whitespace from the left side value s2
	for(s2 = e + 1; s2[0] == ' ' || s2[0] == '\t'; s2++);
	*e = 0;

	// trim whitespace from right side key s1
	e = s1 + strlen(s1);
        while (e > s1 && ((c = e[-1]) == '\n' || c == '\r' || c == ' ' || c == '\t')) e--;
        *e = 0;

	// и заполняем структуру

	if( 0 == strcmp(s1, "aggfile"))
	    SetStrValue(AGGFILE, s2);

	else if( 0 == strcmp(s1, "directorymaps"))
	    SetStrValue(DIRECTORYMAPS, s2);

	else if( 0 == strcmp(s1, "debug") && 0 == strcmp(s2, "on"))
	    SetIntValue(DEBUG, TRUE);
	
	else if(0 == strcmp(s1, "sound") && 0 == strcmp(s2, "on"))
	    SetIntValue(SOUND, TRUE);
	
	else if(0 == strcmp(s1, "music") && 0 == strcmp(s2, "on"))
	    SetIntValue(MUSIC, TRUE);
	
	else if(0 == strcmp(s1, "fullscreen") && 0 == strcmp(s2, "on"))
	    SetIntValue(FULLSCREEN, TRUE);
	
	else if(0 == strcmp(s1, "evilinterface") && 0 == strcmp(s2, "on"))
	    SetIntValue(EVILINTERFACE, TRUE);
	
	else if(0 == strcmp(s1, "animation") && 0 == strcmp(s2, "on"))
	    SetIntValue(ANIMATION, TRUE);

	else if(0 == strcmp(s1, "uniqueartifact") && 0 == strcmp(s2, "on"))
	    SetIntValue(UNIQUEARTIFACT, TRUE);

	else if(0 == strcmp(s1, "videomode")){

	    if(0 == strcmp(s2, "800x600"))
		SetIntValue(VIDEOMODE, 1);

	    else if(0 == strcmp(s2, "1024x768"))
		SetIntValue(VIDEOMODE, 2);

	    else if(0 == strcmp(s2, "1280x1024"))
		SetIntValue(VIDEOMODE, 3);

	    else
	    	SetIntValue(VIDEOMODE, 0);
	
	}else if(0 == strcmp(s1, "limitmemory")){
	    sscanf(s2, "%u", &value);
	    SetIntValue(LIMITMEMORY, value);

	}else if(0 == strcmp(s1, "animationdelay")){
	    sscanf(s2, "%u", &value);
	    SetIntValue(ANIMATIONDELAY, value);

	}
    }

    // закрытие файла
    fclose(fd);
    
    return TRUE;
}

void FreeConfig(void){

    Uint8 i;
    
    if(ptrSettings){
	for(i = 0; i < CONFIGEND; ++i) if(ptrSettings[i].string) free (ptrSettings[i].string);	
	free(ptrSettings);
    }
}

char *GetStrValue(E_CONFIG key){

    return ptrSettings[key].string;
}

Uint8 GetIntValue(E_CONFIG key){

    return ptrSettings[key].value;
}

void SetStrValue(E_CONFIG key, const char *value){


    if(ptrSettings[key].string) free(ptrSettings[key].string);

    ptrSettings[key].string = strdup(value);
}

void SetIntValue(E_CONFIG key, Uint8 value){

    switch(key){
	case VIDEOMODE:
	    switch(value){
		default:
		case 0:
		    SetStrValue(VIDEOMODE, "640x480");
		    ptrSettings[key].value = 0;
		    break;
		case 1:
		    SetStrValue(VIDEOMODE, "800x600");
		    ptrSettings[key].value = 1;
		    break;
		case 2:
		    SetStrValue(VIDEOMODE, "1024x768");
		    ptrSettings[key].value = 2;
		    break;
		case 3:
		    SetStrValue(VIDEOMODE, "1280x1024");
		    ptrSettings[key].value = 3;
		    break;
	    }
	    break;
	
	default:
	    ptrSettings[key].value = value;
	    break;
    }
}

void DefaultSettings(S_CONFIG *ptr){

    if(!ptr) return;

    SetStrValue(AGGFILE, "heroes2.agg");
    SetStrValue(DIRECTORYMAPS, "maps");
    SetIntValue(DEBUG, FALSE);
    SetIntValue(SOUND, FALSE);
    SetIntValue(MUSIC, FALSE);
    SetIntValue(ANIMATION, FALSE);
    SetIntValue(ANIMATIONDELAY, 30);
    SetIntValue(FULLSCREEN, FALSE);
    SetIntValue(EVILINTERFACE, FALSE);
    SetIntValue(VIDEOMODE, 0);
    SetIntValue(LIMITMEMORY, 0);
    SetIntValue(UNIQUEARTIFACT, FALSE);

    SetStrValue(FILEMAPSPATH, "\0");
    SetStrValue(MAPSLONGNAME, "\0");
    SetStrValue(MAPSDESCRIPTION, "\0");
    SetIntValue(MAPSDIFFICULTY, 0);
    SetIntValue(GAMEDIFFICULTY, 0);
    SetIntValue(VICTORYCONDITIONS, 0);
    SetIntValue(LOSSCONDITIONS, 0);
    SetIntValue(KINGDOMCOLORS, 0);
    SetIntValue(ALLOWCOLORS, 0);
    SetIntValue(RNDCOLORS, 0);
    SetIntValue(HUMANCOLORS, 0);
    SetIntValue(RACEBLUE, 0);
    SetIntValue(RACEGREEN, 0);
    SetIntValue(RACERED, 0);
    SetIntValue(RACEYELLOW, 0);
    SetIntValue(RACEORANGE, 0);
    SetIntValue(RACEPURPLE, 0);
    SetIntValue(STARTHEROESCASTLE, FALSE);
    SetIntValue(MONTH, 1);
    SetIntValue(WEEK, 1);
    SetIntValue(DAY, 1);

    // ANIM1 анимация меню
    SetIntValue(ANIM1, TRUE);
    // ANIM2 анимация карта
    SetIntValue(ANIM2, TRUE);
    // ANIM3 анимация замок
    SetIntValue(ANIM3, TRUE);
    SetIntValue(CYCLELOOP, TRUE);
}
