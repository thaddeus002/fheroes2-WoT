/*
    unicn2bmp.c 
*/

#define BACKGROUND 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "SDL.h"
#include "tools.h"

extern int errno;

void FreeAll();

int main(int argc, char **argv){

    	FILE 	*fd = NULL;
    	void 	*ptricn = NULL;
	void    *ptrpal = NULL;
    	char	dirname[255];
	char	filename[255];
	unsigned short int	icncount = 0;
	unsigned int		icnsize = 0;
    	icnspriteheader		*ptrheader = NULL;
	char	*type;
	SDL_Color       colors[256];
	SDL_Surface	*surface = NULL;

	bzero(filename, 255);
	bzero(dirname, 255);

    	if((argc != 2) || (NULL == (type = strstr((const char *) argv[1], ".icn")))){
        	fprintf(stderr, "usage: %s test.icn\n", argv[0]);
		exit(1);
    	}

	strncpy( dirname, (char const *) argv[1], type - argv[1]);
	// создаем каталог для результата
	if(0 > mkdir(dirname, 0755)){
	    if(EEXIST != errno){
		printf("create: directory %s\n", dirname);
		exit(1);
	    }
	}

	// открываем файл icn
	if(NULL == (fd = fopen(argv[1], "r"))){
		printf("open error: file %s\n", argv[1]);
		exit(1);
    	}

	// загружаем в буффер
	fread(&icncount, sizeof(short int), 1, fd);
	fread(&icnsize, sizeof(int), 1, fd);
    	if(NULL == (ptricn = malloc(icnsize))){
		printf("malloc: error size %lu\n", (long) icnsize);
		exit(1);
    	}
    	if(icnsize != fread(ptricn, sizeof(char), icnsize, fd)){
		printf("read: incorrect icn file!\n");
		exit(1);
    	}
    	fclose(fd);

	// открываем файл pal
	if(NULL == (fd = fopen("kb.pal", "r"))){
		printf("open error: file kb.pal\n");
		exit(1);
    	}

	// вычисляем его размер
	Uint32 palfilesize;
	fseek(fd, 0, SEEK_END);
    	palfilesize = ftell(fd);
    	fseek(fd, 0, SEEK_SET);

	// загружаем в буффер
    	if(NULL == (ptrpal = malloc(palfilesize))){
		printf("malloc error: size %lu\n", (long) palfilesize);
		exit(1);
    	}
    	if(palfilesize != fread(ptrpal, sizeof(Uint8), palfilesize, fd)){
		printf("read file incorrect count byte!\n");
		exit(1);
    	}
    	fclose(fd);

	if(0 > chdir(dirname)){
		printf("chdir: %s\n", dirname);
		exit(1);
	}

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

	unsigned short int	count = 0;
    	Uint32 			sizesprite = 0;
	char number[6];

        // цикл по спрайтам
	while(count < icncount){

    	    sizesprite = 0;

	    if(count < (icncount - 1))
		sizesprite = ptrheader[count + 1].offset - ptrheader[count].offset;
	    else
		sizesprite = icnsize - ptrheader[count].offset;
	    
	    sprintf(number, "%4d", count + 1);
	    for(i = 0; i < 6; i++)
		if(0 == strncmp(&number[i], " ", 1))
		    number[i] = '0';
	    sprintf(filename, "%s_%4s.bmp", (const char *) dirname, number);

    	    if(NULL == (surface = SDL_CreateRGBSurface(SDL_SWSURFACE, ptrheader[count].w, ptrheader[count].h, 8, 0, 0, 0, 0))){
        	fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptrheader[count].w, ptrheader[count].h);
        	exit(1);
    	    }
    	    // инициализация палитры
    	    SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);

            SDL_FillRect(surface, NULL, BACKGROUND);
	    DrawScene(surface, (Uint8 *) ptricn + ptrheader[count].offset, sizesprite);
	    SDL_SaveBMP(surface, filename);
	    SDL_FreeSurface(surface);

	    ++count;
	}

    free(ptricn);

    exit(0);
}

void FreeAll(){
    SDL_Quit();
    return;
}
