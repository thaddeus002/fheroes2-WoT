/* 
    showicn.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "tools.h"

#define MODE_X 800
#define MODE_Y 600
#define BACKGROUND 16
#define DELAY 100

#define ICN_DEBUG 1

void FreeAll();

int main(int argc, char **argv){
    	SDL_Surface *surface;
    	SDL_Event event;
	SDL_Color	colors[256];

    	FILE *fd;
    	void 		*ptricn;
    	void 		*ptrpal;
    	icnspriteheader	*ptrheader;
	Uint16		icncount;
	Uint32		icnsize;


    	if(argc != 2){
        	fprintf(stderr, "usage: ./showicn test.icn\n");
		exit(1);
    	}

	// открываем файл icn
	if(NULL == (fd = fopen(argv[1], "r"))){
		fprintf(stderr, "open error: file %s\n", argv[1]);
		exit(1);
    	}

	// загружаем в буффер
	fread(&icncount, sizeof(Uint16), 1, fd);
	if(ICN_DEBUG) fprintf(stderr, "ICN COUNT: %d\n", icncount);
	fread(&icnsize, sizeof(Uint32), 1, fd);
    	if(NULL == (ptricn = malloc(icnsize))){
		fprintf(stderr, "malloc error: size %lu\n", (long) icnsize);
		exit(1);
    	}
    	if(icnsize != fread(ptricn, sizeof(Uint8), icnsize, fd)){
		fprintf(stderr, "read file incorrect count byte!\n");
		exit(1);
    	}
    	fclose(fd);

	// открываем файл pal
	if(NULL == (fd = fopen("kb.pal", "r"))){
		fprintf(stderr, "open error: file kb.pal\n");
		exit(1);
    	}

	// вычисляем его размер
	Uint32 palfilesize;
	fseek(fd, 0, SEEK_END);
    	palfilesize = ftell(fd);
    	fseek(fd, 0, SEEK_SET);

	// загружаем в буффер
    	if(NULL == (ptrpal = malloc(palfilesize))){
		fprintf(stderr, "malloc error: size %lu\n", (long) palfilesize);
		exit(1);
    	}
    	if(palfilesize != fread(ptrpal, sizeof(Uint8), palfilesize, fd)){
		fprintf(stderr, "read file incorrect count byte!\n");
		exit(1);
    	}
    	fclose(fd);

    	/* Fill colors with color information */
    	Uint16 i;
    	Uint8	*palette = ptrpal;
    	for(i = 0; i < 256; i++){
		colors[i].r = *palette << 2;
		++palette;
		colors[i].g = *palette << 2;
		++palette;
		colors[i].b = *palette << 2;
		++palette;
    	}
	ptrheader = ptricn;

    	// инициализация SDL
    	if( 0 > SDL_Init(SDL_INIT_VIDEO)){
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		exit(1); }
    	atexit(FreeAll);

    	SDL_WM_SetCaption("showicn","showicn");
    	if(NULL == (surface = SDL_SetVideoMode(MODE_X, MODE_Y, 8, SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF))){
		fprintf(stderr, "SDL_SetVideoMode: MODE_X=%d, MODE_Y=%d, %s\n", ptrheader->w, ptrheader->h, SDL_GetError());
		exit(1); }
    	// инициализация палитры
    	SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);

    Uint8	done = 0;
    Uint16	count = 0;

    while(!done){
	// опрос событий клавиатуры
    	while(SDL_PollEvent(&event))
	    if(event.type == SDL_QUIT)
		    done = 1;
	    else if(event.type == SDL_KEYDOWN && SDLK_ESCAPE == event.key.keysym.sym)
		    done = 1;
	
        // цикл по спрайтам
	if(count < icncount){
	    if(ICN_DEBUG) fprintf(stderr, "\nHEADER: %d, OFFSET: 0x%hX,", count+1, ptrheader[count].offset + sizeof(icncount) + sizeof(icnsize));
	    if(ICN_DEBUG) fprintf(stderr, " WIDTH: %d, HEIGHT %d,", ptrheader[count].w, ptrheader[count].h);
    	    Uint32 sizesprite;
	    if(count < (icncount - 1))
		sizesprite = (Uint32) ptrheader[count + 1].offset - ptrheader[count].offset;
	    else
		sizesprite = (Uint32) icnsize - ptrheader[count].offset;
    	    if(ICN_DEBUG) fprintf(stderr, " SIZE: %lu\n", (long) sizesprite);

	    // рисуем картинку
	    SDL_FillRect(surface, NULL, BACKGROUND);
    	    DrawScene(surface, ptricn + ptrheader[count].offset, sizesprite);
	    SDL_Flip(surface);
	    ++count;

	}
	SDL_Delay(DELAY);
    }

    free(ptricn);
    free(ptrpal);

    exit(0);
}

void FreeAll(){
    SDL_Quit();
    return;
}

