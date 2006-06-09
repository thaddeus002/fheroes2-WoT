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
    char	*key;
    char 	valueStr[MAXSTRLEN];
    Uint8	valueInt;
} CONFIGDESC;

static CONFIGDESC settings[] = {
    // изменяемые настройки игры
    { "aggfile",	"heroes2.agg",	0 },
    { "directorymaps",	"maps",		0 },
    { "directorysave",	"save",		0 },
    { "debug",		"off",		FALSE },
    { "sound",		"off",		FALSE },
    { "music",		"off",		FALSE },
    { "animation",	"off",		FALSE },
    { "fullscreen",	"off",		FALSE },
    { "videomode",	"640x480",	0 },
    { "limitmemory",	"\0",		0 },

    // служебные параметры игры
    { "filemapspath",	"\0",		0 },	// здесь хранится полный путь к выбранному файлу карты игры

    { NULL,		"\0",		0 }
};

void InitConfig(const char * configFile){

    FILE	*fd;
    char	str[1024], c, *s1, *s2, *e;
    CONFIGDESC *ptr = NULL;
    

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
	ptr = &settings[0];

	if( 0 == strcmp(s1, "aggfile") ||
	    0 == strcmp(s1, "directorymaps") ||
	    0 == strcmp(s1, "directorysave"))
	    while(ptr->key){
		if(0 == strcmp(s1, ptr->key) && strlen(s2) < MAXSTRLEN)
		    strcpy(ptr->valueStr, s2);

		++ptr;
	    }

	else if(0 == strcmp(s1, "mapinfo") ||
		0 == strcmp(s1, "mapcell") ||
		0 == strcmp(s1, "debug") ||
		0 == strcmp(s1, "sound") ||
		0 == strcmp(s1, "music") ||
		0 == strcmp(s1, "fullscreen") ||
		0 == strcmp(s1, "animation"))
	    while(ptr->key){
		if(0 == strcmp(s1, ptr->key)){
		    if(0 == strcmp(s2, "off")){
			ptr->valueInt = FALSE;
			strcpy(ptr->valueStr, "off");
		    }else if(0 == strcmp(s2, "on")){
			ptr->valueInt = TRUE;
			strcpy(ptr->valueStr, "on");
		    }
		}
		++ptr;
	    }

	else if(0 == strcmp(s1, "videomode"))
	    while(ptr->key){
		if(0 == strcmp(s1, ptr->key)){
		    if(0 == strcmp(s2, "640x480")){
			strcpy(ptr->valueStr, "640x480");
			ptr->valueInt = 0;
		    }else if(0 == strcmp(s2, "800x600")){
			strcpy(ptr->valueStr, "800x600");
			ptr->valueInt = 1;
		    }else if(0 == strcmp(s2, "1024x768")){
			strcpy(ptr->valueStr, "1024x768");
			ptr->valueInt = 2;
		    }else if(0 == strcmp(s2, "1280x1024")){
			strcpy(ptr->valueStr, "1280x1024");
			ptr->valueInt = 3;
		    }
		}
		++ptr;
	    }

	else if(0 == strcmp(s1, "limitmemory"))
	    while(ptr->key){
		if(0 == strcmp(s1, ptr->key))
		    sscanf(s2, "%c", (char *) &ptr->valueInt);
		++ptr;
	    }

    }

    // закрытие файла
    fclose(fd);
}

char *GetStrValue(const char *key){

    CONFIGDESC *ptr = &settings[0];

    while(ptr->key){

	if(0 == strcmp(key, ptr->key)) return ptr->valueStr;

	++ptr;
    }

    return NULL;
}

Uint8 GetIntValue(const char *key){

    CONFIGDESC *ptr = &settings[0];

    while(ptr->key){

	if(0 == strcmp(key, ptr->key)) return ptr->valueInt;

	++ptr;
    }

    return 0;
}

void SetStrValue(const char *key, const char *value){

    CONFIGDESC *ptr = &settings[0];

    while(ptr->key && strcmp(key, ptr->key)) ++ptr;

    if(0 == strcmp(ptr->key, key) && strlen(value) < MAXSTRLEN){

	ptr->valueInt = 0;
	strcpy(ptr->valueStr, value);

    }else
	fprintf(stderr, "SetStrValue: error, key not found or string limit!\n");
}

void SetIntValue(const char *key, Uint8 value){

    CONFIGDESC *ptr = &settings[0];

    while(ptr->key && strcmp(key, ptr->key)) ++ptr;

    if(0 == strcmp(ptr->key, "mapinfo") || 0 == strcmp(ptr->key, "mapcell") || 0 == strcmp(ptr->key, "debug") || 0 == strcmp(ptr->key, "sound") || 0 == strcmp(ptr->key, "music") || 0 == strcmp(ptr->key, "fullscreen") || 0 == strcmp(ptr->key, "animation"))
	if(FALSE == value){
	    ptr->valueInt = FALSE;
	    strcpy(ptr->valueStr, "off");
	}else{
	    ptr->valueInt = TRUE;
	    strcpy(ptr->valueStr, "on");
	}

    else if(0 == strcmp(ptr->key, "videomode"))
	switch(value){
	
	    default:
	    case 0:
		strcpy(ptr->valueStr, "640x480");
		ptr->valueInt = 0;
		break;
	
	    case 1:
		strcpy(ptr->valueStr, "800x600");
		ptr->valueInt = 1;
		break;
		
	    case 2:
		strcpy(ptr->valueStr, "1024x768");
		ptr->valueInt = 2;
		break;

	    case 3:
		strcpy(ptr->valueStr, "1280x1024");
		ptr->valueInt = 3;
		break;
	}

    else if(0 == strcmp(ptr->key, "limitmemory"))
	ptr->valueInt = value;
}
