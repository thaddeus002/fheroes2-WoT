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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "SDL.h"
#include "gamedefs.h"
#include "stpemain.h"
#include "config.h"
#include "agg.h"
#include "cursor.h"
#include "sound.h"

int main(int argc, char **argv){

    char *heroes2_agg = NULL;
    char *config_file = NULL;
    
    SDL_Surface *video = NULL;

    opterr = 0;
    BOOL debug = FALSE;
    int c;

    char caption[64];
    sprintf(caption, "Free Heroes II, version: %d, (maps viewer only)", VERSION);
    fprintf(stderr, "Free Heroes II, version: %d, (maps viewer only)\n", VERSION);

    while((c = getopt(argc, argv, "dhc:f:")) != -1)
	switch (c){

	    case 'f':
		heroes2_agg = optarg;
		break;

	    case 'c':
		config_file = optarg;
		break;

	    case 'd':
		debug = TRUE;
		break;

	    case '?':
	    case 'h':
		printf("Usage: %s [OPTIONS]\n", argv[0]);
		printf("  -d\tdebug mode\n");
		printf("  -c\tpatch to config file (default: fheroes2.cfg)\n");
		printf("  -f\tpatch to heroes2.agg\n");
		printf("  -h\tprint this help and exit\n");
		exit(0);
		break;

	    default:
		break;
	}

    // инициализация переопределенных конфигурационных парамертов
    if(NULL == config_file) config_file = "fheroes2.cfg";
    InitConfig(config_file);
    if(debug) SetIntValue("debug", debug);
    if(NULL == heroes2_agg) heroes2_agg = GetStrValue("aggfile");

    Uint32 flags;
    // инициализация SDL
    if(GetIntValue("sound"))
	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    else
	flags = SDL_INIT_VIDEO;

    if( 0 > SDL_Init(flags)){
	fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
	exit(1);
    }

    if(GetIntValue("sound")) atexit(SDL_CloseAudio);
    atexit(SDL_Quit);

    Uint16 xres = 0;
    Uint16 yres = 0;
    switch(GetIntValue("videomode")){

        default:
        case 0:
	    xres = 640;
	    yres = 480;
            break;

        case 1:
	    xres = 800;
	    yres = 576;
            break;

        case 2:
	    xres = 1024;
	    yres = 768;
            break;
    }

    // check mode
    fprintf(stderr, "Checking mode %dx%d@16bpp. (SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF)\n", xres, yres);
    if(16 != SDL_VideoModeOK(xres, yres, 16, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF)){
	fprintf(stderr, "Mode not available.");
	exit(1);
    }

    SDL_WM_SetCaption(caption, NULL);

    // only 640x480
    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue("fullscreen")) flag |= SDL_FULLSCREEN;

    if(NULL == (video = SDL_SetVideoMode(640, 480, 16, flag))){
        fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
        exit(1);
    }

    // инициализация игровых данных
    if(! InitAGG(heroes2_agg)) exit(1);
    InitSound();
    InitCursor();

    // переходим на первый экран
    PreloadObject("EDITOR.ICN");
    PreloadObject("BTNEMAIN.ICN");
    PreloadObject("REDBACK.ICN");

    DrawNewLoadQuit();

    // освобождаем данные
    FreeCursor();
    FreeSound();
    FreeAGG();

    fprintf(stderr, "Free all resource.\nBye!\n");
    exit(0);
}
