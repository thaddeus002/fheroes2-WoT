/*
 * 82m -> wav
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    typedef struct __attribute__((packed)){
	unsigned int riff;		// "RIFF"
	unsigned int size;		// size_d + 0x22
	unsigned int wave;		// "WAVE"
	unsigned int fmt;		// "FMT "
	unsigned int size_t;		// 0x10
	unsigned short int format;	// 0x01 pcm
	unsigned short int channels;	// 0x01 mono
	unsigned int samples;		// 22050
	unsigned int byteper;		// 22050 = channels * samples * bitsper / 8
	unsigned short int align;	// 1     = channels * bitsper / 8
	unsigned short int bitsper;	// 8
	unsigned int data;		// "DATA"
	unsigned int size_d;		//
    } wavetable;

    FILE *fd;
    void *pfile;
    long size;
    wavetable *zag;

    // нет параметров
    if(argc != 3){
	fprintf(stderr, "./82m2wav file.82m file.wav\n");
	exit(1);
    }

    // открываем файл
    if(NULL == (fd = fopen(argv[1], "r"))){
	fprintf(stderr, "error fopen(r): %s\n", argv[1]);
	exit(1);
    }

    // вычисляем размер файла
    if(0 > fseek(fd, 0, SEEK_END)){
	fprintf(stderr, "error fseek\n");
	exit(1);
    }
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // выделяем память для буффера pfile
    if(NULL == (pfile = malloc(size))){
	fprintf(stderr, "error malloc: %lu\n", size);
        exit(1);
    }
    // читаем весь pfat
    fread(pfile, size, 1, fd);
    fclose(fd);

    // выделяем память для буффера zag
    if(NULL == (zag = malloc(sizeof(wavetable)))){
	fprintf(stderr, "error malloc: %lu\n", size);
        exit(1);
    }

    // заполняем заголовок wav
    zag->riff = 0x46464952;		// "RIFF"
    zag->wave = 0x45564157;		// "WAVE"
    zag->fmt = 0x20746D66;		// "fmt "
    zag->size_t = 0x10;
    zag->format = 0x01;
    zag->channels = 0x01;
    zag->samples = 22050;
    zag->bitsper = 0x08;
    zag->data = 0x61746164;		// "data"
    zag->size_d = size;
    zag->size = zag->size_d + 0x22;
    zag->byteper = zag->channels * zag->samples * zag->bitsper / 8;
    zag->align = zag->channels * zag->bitsper / 8;

    // открываем файл
    if(NULL == (fd = fopen(argv[2], "w"))){
	fprintf(stderr, "error fopen(w): %s\n", argv[1]);
	exit(1);
    }
    // пишем заголовок
    fwrite(zag, sizeof(wavetable), 1, fd);
    
    // пишем данные
    fwrite(pfile, size, 1, fd);

    fclose(fd);

    free(zag);
    free(pfile);
    exit(0);
}
