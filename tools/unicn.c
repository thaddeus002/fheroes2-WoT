/*
    unicn.c
*/

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
       
int main(int argc, char **argv){

    	FILE 	*fd = NULL;
    	void 	*ptricn = NULL;
	char	dirname[255];
	char	filename[255];
	unsigned short int	icncount = 0;
	unsigned int		icnsize = 0;
    	icnspriteheader		*ptrheader = NULL;
	char	*type;

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

	if(0 > chdir(dirname)){
		printf("chdir: %s\n", dirname);
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

	ptrheader = ptricn;

	unsigned short int	count = 0;
	unsigned short int	countsprite = 1;
    	unsigned int 		sizesprite = 0;
	unsigned int 		temp = 0;

        // цикл по спрайтам
	while(count < icncount){

    	    sizesprite = 0;

	    if(count < (icncount - 1))
		sizesprite = ptrheader[count + 1].offset - ptrheader[count].offset;
	    else
		sizesprite = icnsize - ptrheader[count].offset;

	    sprintf(filename, "%s_%d.icn", (const char *) dirname, count);

	    // открываем файл icn
	    if(NULL == (fd = fopen(filename, "w"))){
		printf("open error: file %s\n", filename);
		exit(1);
    	    }

	    // сохраняем в файл
	    temp = sizesprite + sizeof(icnspriteheader);
	    fwrite(&countsprite, sizeof(short int), 1, fd);
	    fwrite(&temp, sizeof(int), 1, fd);

	    temp = sizeof(icnspriteheader);
	    fwrite(&ptrheader[count].unknown1, sizeof(short int), 1, fd);
	    fwrite(&ptrheader[count].unknown2, sizeof(short int), 1, fd);
	    fwrite(&ptrheader[count].w, sizeof(short int), 1, fd);
	    fwrite(&ptrheader[count].h, sizeof(short int), 1, fd);
	    fwrite(&ptrheader[count].unknown3, sizeof(char), 1, fd);
	    fwrite(&temp, sizeof(int), 1, fd);

	    fwrite((char *) ptricn + ptrheader[count].offset, sizesprite, 1, fd);

	    fclose(fd);

	    ++count;
	}

    free(ptricn);

    exit(0);
}
