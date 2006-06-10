/*
    unpackage heroes2.agg
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

void strtolower(char *str);

int main(int argc, char *argv[]){

    typedef struct{
	char junk[4];
	unsigned int offset;
	unsigned int size;
    } fat_element;

    typedef struct{
	char name[15];
    } name_element;

    int i, fdin, fdout;
    unsigned short int count;
    fat_element *pfat;
    name_element *pname;
    
    // нет параметров
    if(argc != 2){
	printf("./extractor heroes2.agg\n");
	exit(1);
    }
    // открываем файл
    if(0 > (fdin = open(argv[1], O_RDONLY))){
	printf("error open(O_RDONLY): %s\n", argv[1]);
	exit(1);
    }
    // читаем количество файлов
    read(fdin, &count, 2);
    
    // выделяем память для буффера pfat
    pfat = calloc(count, sizeof(*pfat));
    if(pfat == NULL){
	printf("error calloc: %d, %d\n", count, sizeof(*pfat));
	exit(1);
    }
    // выделяем память для буффера pname
    pname = calloc(count, sizeof(*pname));
    if(pname == NULL){
	printf("error calloc: %d, %d\n", count, sizeof(*pname));
	exit(1);
    }
    // читаем весь pfat
    read(fdin, pfat, count * sizeof(*pfat));
    
    // устанавливаем смещение на начало блока имен
    lseek(fdin, lseek(fdin, 0, SEEK_END) - count * 15, SEEK_SET);
    
    // читаем весь pname
    read(fdin, pname, count * sizeof(*pname));

    // цикл по файлам
    for(i = 0; i < count; ++i){

	// создаем буффер обмена
	void *buf = malloc(pfat[i].size);
	if(buf == NULL){
	    printf("error malloc: %d\n", pfat[i].size);
	    exit(1);
	}
	// устанавливаем смещение на файл
	lseek(fdin, pfat[i].offset, SEEK_SET);
	
	// читаем файл в буффер
	read(fdin, buf, pfat[i].size);
	
	// создаем файл в текущем каталоге
	strtolower(pname[i].name);
        printf("extracting %14s ... ", pname[i].name);
	if(0 > (fdout =	open(pname[i].name, O_WRONLY|O_TRUNC|O_CREAT, 0644))){
	    printf("error open(O_WRONLY|O_TRUNC|O_CREAT, 0644): %s\n", pname[i].name);
	    exit(1);
	}
	// сохраняем буффер
	if(0 > write(fdout, buf, pfat[i].size)){
	    printf("error write: %s, %d\n", pname[i].name, pfat[i].size);
	    exit(1);
	}
	// закрываем файл
	close(fdout);
	printf("ok\n");
	// освобождаем буффер
	free(buf);
    }
    // завершаем работу
    free(pname);
    free(pfat);
    close(fdin);
    //
    exit(0);
}

void strtolower(char *str){
    for( ; *str; ++str){ *str = tolower(*str); }
    return;
}
