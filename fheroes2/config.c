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

#define MAXSTRLEN 255

typedef struct {
    char 	valueStr[MAXSTRLEN];
    Uint8	valueInt;
} CONFIGDESC;

static CONFIGDESC settings[CONFIGEND];

void InitConfig(const char * configFile){

    FILE	*fd;
    char	str[1024], c, *s1, *s2, *e;
    Uint32	value = 0;

    // изменяемые настройки игры
    strcpy(settings[AGGFILE].valueStr, "heroes2.agg");
    settings[AGGFILE].valueInt	= 0;

    strcpy(settings[DIRECTORYMAPS].valueStr, "maps");
    settings[DIRECTORYMAPS].valueInt = 0;

    strcpy(settings[DEBUG].valueStr, "off");
    settings[DEBUG].valueInt = FALSE;

    strcpy(settings[SOUND].valueStr, "off");
    settings[SOUND].valueInt = FALSE;

    strcpy(settings[MUSIC].valueStr, "off");
    settings[MUSIC].valueInt = FALSE;

    strcpy(settings[ANIMATION].valueStr, "off");
    settings[ANIMATION].valueInt = FALSE;

    strcpy(settings[ANIMATIONDELAY].valueStr, "");
    settings[ANIMATIONDELAY].valueInt = 100;

    strcpy(settings[FULLSCREEN].valueStr, "off");
    settings[FULLSCREEN].valueInt = FALSE;

    strcpy(settings[EVILINTERFACE].valueStr, "off");
    settings[EVILINTERFACE].valueInt = FALSE;

    strcpy(settings[VIDEOMODE].valueStr, "640x480");
    settings[VIDEOMODE].valueInt = 0;

    strcpy(settings[LIMITMEMORY].valueStr, "");
    settings[LIMITMEMORY].valueInt = 0;

    // служебные параметры сценария карты
    strcpy(settings[FILEMAPSPATH].valueStr, "");// здесь хранится полный путь к выбранному файлу карты игры
    settings[FILEMAPSPATH].valueInt = 0;	// здесь хранится полный путь к выбранному файлу карты игры

    strcpy(settings[MAPSLONGNAME].valueStr, "");// название карты
    settings[MAPSLONGNAME].valueInt = 0;	// название карты

    strcpy(settings[MAPSDESCRIPTION].valueStr, "");// описание сценария карты
    settings[MAPSDESCRIPTION].valueInt = 0;	   // описание сценария карты

    strcpy(settings[MAPSDIFFICULTY].valueStr, "");// сложность карты
    settings[MAPSDIFFICULTY].valueInt = 0;	  // сложность карты

    strcpy(settings[GAMEDIFFICULTY].valueStr, "");// сложность игры
    settings[GAMEDIFFICULTY].valueInt = 0;	  // сложность игры

    strcpy(settings[VICTORYCONDITIONS].valueStr, "");// условия победы
    settings[VICTORYCONDITIONS].valueInt = 0;	     // условия победы

    strcpy(settings[LOSSCONDITIONS].valueStr, "");   // условия поражения
    settings[LOSSCONDITIONS].valueInt = 0;	     // условия поражения

    // открытие файла конфигурации
    if(NULL == (fd = fopen(configFile, "r"))){
	fprintf(stderr, "InitConfig: %s not found\n", configFile);
	return;
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
}

char *GetStrValue(E_CONFIG key){

    return settings[key].valueStr;
}

Uint8 GetIntValue(E_CONFIG key){

    return settings[key].valueInt;
}

void SetStrValue(E_CONFIG key, const char *value){

    if(strlen(value) < MAXSTRLEN){

	settings[key].valueInt = 0;
	strcpy(settings[key].valueStr, value);

    }else
	fprintf(stderr, "SetStrValue: error, key not found or string limit!\n");
}

void SetIntValue(E_CONFIG key, Uint8 value){

    switch(key){
    
	case DEBUG:
	case SOUND:
	case MUSIC:
	case FULLSCREEN:
	case EVILINTERFACE:
	case ANIMATION:

	    if(FALSE == value){
		settings[key].valueInt = FALSE;
		strcpy(settings[key].valueStr, "off");
	    }else{
		settings[key].valueInt = TRUE;
		strcpy(settings[key].valueStr, "on");
	    }
	    break;

	case VIDEOMODE:

	    switch(value){
	
		default:
		case 0:
		    strcpy(settings[key].valueStr, "640x480");
		    settings[key].valueInt = 0;
		    break;
	
		case 1:
		    strcpy(settings[key].valueStr, "800x600");
		    settings[key].valueInt = 1;
		    break;
		
		case 2:
		    strcpy(settings[key].valueStr, "1024x768");
		    settings[key].valueInt = 2;
		    break;

		case 3:
		    strcpy(settings[key].valueStr, "1280x1024");
		    settings[key].valueInt = 3;
		    break;
	    }
	    break;
	
	case MAPSDIFFICULTY:
	case GAMEDIFFICULTY:
	case VICTORYCONDITIONS:
	case LOSSCONDITIONS:
	case ANIMATIONDELAY:
	case LIMITMEMORY:

	    settings[key].valueInt = value;
	    break;
	
	case AGGFILE:
	case DIRECTORYMAPS:
	case FILEMAPSPATH:
	case MAPSLONGNAME:
	case MAPSDESCRIPTION:
	case CONFIGEND:
	    break;
    }
}
