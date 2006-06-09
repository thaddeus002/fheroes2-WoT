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
		printf("  -c\tpatch to config file\n");
		printf("  -f\tpatch to heroes2.agg\n");
		printf("  -h\tprint this help and exit\n");
		exit(0);
		break;

	    default:
		break;
	}

    // инициализация переопределенных конфигурационных парамертов
    if(NULL == config_file) config_file = "heroes2.cfg";
    InitConfig(config_file);
    SetIntValue("debug", debug);
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
