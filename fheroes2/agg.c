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
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "draw.h"
#include "tools.h"
#include "config.h"
#include "agg.h"

#pragma pack(1)

typedef struct {
    char                name[AGGSIZENAME];	// оригинальное наименование
    Uint32              offset;			// оригинальный offset в AGG
    Uint32              size;			// оригинальный size в AGG
    AGGTYPE             type;			// тип
    Uint32		life;			// приоритет жизни
    Uint32              count;			// количество внутренних объектов
    void                *data;			// указатель на данные
} AGGDATA;

typedef struct {
    Uint16	count;
    Uint32	size;
} INSIDEICNHEADER;

typedef struct {
    Uint16	offsetX;
    Uint16	offsetY;
    Uint16	width;
    Uint16	height;
    Uint8	unk3;
    Uint32	offset;
} INSIDEICNSPRITE;

typedef struct {
    Uint16      count;
    Uint16      w;
    Uint16      h;
} INSIDETILHEADER;

BOOL		AddICN(void **, INSIDEICNHEADER *, INSIDEICNSPRITE *);
AGGDATA	*	GetHeaderAGGName(const char *);


static AGGDATA		*ptrAGG = NULL;		// указатель на структуру объектов
static FILE		*fd = NULL;		// дескриптор AGG
Uint16			countAGG = 0;		// количество объектов в AGG
static SDL_Color        *colorsPtr = NULL;
static Uint32		sizeMemory = 0;		// текущий размер по памяти

SDL_Surface *GetICNSprite(AGGSPRITE *object){

    AGGDATA *ptrAGGData = NULL;

    if(1 > strlen(object->name)) return NULL;

    if(NULL == (ptrAGGData = GetHeaderAGGName(object->name))){
	fprintf(stderr, "GetICNSprite object not found: %s, %d\n", object->name, object->number);
	return NULL;
    }

    if(ptrAGGData->type != ICN){
	fprintf(stderr, "GetICNSprite error: type not ICN, name %s\n", object->name);
	return NULL;
    }

    // загружаем
    if(ptrAGGData->data == NULL){

	char *icnData = (char *) malloc(ptrAGGData->size);

	if(icnData == NULL){
	    fprintf(stderr, "error malloc: %d\n", ptrAGGData->size);
	    return NULL;
	}
	
	// read
	fseek(fd, ptrAGGData->offset, SEEK_SET);
	fread(icnData, 1, ptrAGGData->size, fd);

	// add
	INSIDEICNHEADER	*header = (INSIDEICNHEADER *) icnData;
	INSIDEICNSPRITE	*sprite = (INSIDEICNSPRITE *) (icnData + sizeof(INSIDEICNHEADER));

	ptrAGGData->count = header->count;

	if(ptrAGGData->count)
	    AddICN(&ptrAGGData->data, header, sprite);

	free(icnData);
	
	if(ptrAGGData->data)
	    sizeMemory += GetSizeSurface(((ICNHEADER *) ptrAGGData->data)->surface);
	sizeMemory += sizeof(ICNHEADER);
    }

    // если существуют данные
    if(ptrAGGData->data){

	if(object->number > ptrAGGData->count){
	    fprintf(stderr, "GetICNSprite error: name %s, number %d, > count %d\n", object->name, object->number, ptrAGGData->count);
	    object->number = ptrAGGData->count;
	}

	// увеличиваем приоритет
	++ptrAGGData->life;

	// возвращаем результат
	ICNHEADER *ptr = (ICNHEADER *) ptrAGGData->data;

	Uint16 number = object->number;

	while(number--)
	    if(ptr->next) ptr = (ICNHEADER *) ptr->next;

	return ptr->surface;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetICNSprite error: return NULL\n");
    return NULL;
}

ICNHEADER * GetICNHeader(AGGSPRITE *object){

    AGGDATA *ptrAGGData = NULL;

    if(1 > strlen(object->name)) return NULL;

    if(NULL == (ptrAGGData = GetHeaderAGGName(object->name))){
	if(GetIntValue(DEBUG)) fprintf(stderr, "GetICNHeader object not found: %s, %d\n", object->name, object->number);
	return NULL;
    }

    if(ptrAGGData->type != ICN){
	if(GetIntValue(DEBUG)) fprintf(stderr, "GetICNHeader error: type not ICN, name %s\n", object->name);
	return NULL;
    }

    // загружаем
    if(ptrAGGData->data == NULL){

	char *icnData = (char *) malloc(ptrAGGData->size);

	if(icnData == NULL){
	    fprintf(stderr, "GetICNHeader error malloc: %d\n", ptrAGGData->size);
	    return NULL;
	}
	
	// read
	fseek(fd, ptrAGGData->offset, SEEK_SET);
	fread(icnData, 1, ptrAGGData->size, fd);

	// add
	INSIDEICNHEADER	*header = (INSIDEICNHEADER *) icnData;
	INSIDEICNSPRITE	*sprite = (INSIDEICNSPRITE *) (icnData + sizeof(INSIDEICNHEADER));

	ptrAGGData->count = header->count;

	if(ptrAGGData->count)
	    AddICN(&ptrAGGData->data, header, sprite);

	free(icnData);
	
	if(ptrAGGData->data)
	    sizeMemory += GetSizeSurface(((ICNHEADER *) ptrAGGData->data)->surface);
	sizeMemory += sizeof(ICNHEADER);
    }

    // если существуют данные
    if(ptrAGGData->data){

	if(object->number > ptrAGGData->count){
	    if(GetIntValue(DEBUG)) fprintf(stderr, "GetICNHeader error: name %s, number %d, > count %d\n", object->name, object->number, ptrAGGData->count);
	    object->number = ptrAGGData->count;
	}

	// увеличиваем приоритет
	++ptrAGGData->life;

	// возвращаем результат
	ICNHEADER *ptr = (ICNHEADER *) ptrAGGData->data;

	Uint16 number = object->number;

	while(number--)
	    if(ptr->next) ptr = (ICNHEADER *) ptr->next;

	return ptr;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetICNHeader error: return NULL\n");
    return NULL;
}

/* Функция добавляем цепочку из surface в структуру ICNHEADER */
BOOL AddICN(void **ptr, INSIDEICNHEADER *hdr, INSIDEICNSPRITE *hdrspr){

    if(*ptr != NULL) return FALSE;

    ICNHEADER *head = NULL;

    int i;
    // цикл по спрайтам
    for(i = 0; i < hdr->count; ++i){

	// если первый элемент инициализируем указатель
	if(i == 0){

	    head = (ICNHEADER *) malloc(sizeof(ICNHEADER));

	    if(head == NULL){
		fprintf(stderr, "error malloc: %d\n", sizeof(ICNHEADER));
		return FALSE;
	    }

	    *ptr = head;

	// иначе все в хвост
	}else{

	    head->next = malloc(sizeof(ICNHEADER));

	    if(head->next == NULL){
		fprintf(stderr, "error malloc: %d\n", sizeof(ICNHEADER));
		return FALSE;
	    }

	    head = (ICNHEADER *) head->next;
	}
	
	head->next = NULL;
	head->offsetX = hdrspr[i].offsetX;
	head->offsetY = hdrspr[i].offsetY;
	head->reflect = NULL;

	Uint16 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xF000;
	gmask = 0x0F00;
	bmask = 0x00F0;
	amask = 0x000F;
#else
	rmask = 0x000F;
	gmask = 0x00F0;
	bmask = 0x0F00;
	amask = 0xF000;
#endif
	SDL_Surface *formatSurface = NULL;
	if(NULL == (formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, hdrspr[i].width, hdrspr[i].height, 16, rmask, gmask, bmask, amask))){
	    fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), hdrspr[i].width, hdrspr[i].height);
	    return FALSE;
	}

	Uint32 sizesprite = 0;
        if(i < hdr->count - 1)
	    sizesprite = (Uint32) hdrspr[i + 1].offset - hdrspr[i].offset;
	else
	    sizesprite = (Uint32) hdr->size - hdrspr[i].offset;

	SDL_FillRect(formatSurface, NULL, COLORKEY);
	DrawSceneVar2(formatSurface, (Uint8 *) hdrspr + hdrspr[i].offset, sizesprite);
	SDL_SetColorKey(formatSurface, SDL_SRCCOLORKEY|SDL_RLEACCEL, COLORKEY);
	head->surface = SDL_DisplayFormatAlpha(formatSurface);
	SDL_FreeSurface(formatSurface);
    }

    return TRUE;
}

BOOL InitAGG(){

    char *filename = GetStrValue(AGGFILE);

    // открываем файл
    if(NULL == (fd = fopen(filename, "rb"))){
	fprintf(stderr, "InitAGG: error fopen: %s\n", filename);
	return FALSE;
    }

    fprintf(stderr, "read: %s\n", filename);

    // читаем количество файлов
    fread(&countAGG, sizeof(countAGG), 1, fd);

    // выделяем память для буффера AGGDATA
    ptrAGG = (AGGDATA *) malloc(countAGG * sizeof(AGGDATA));
    if(ptrAGG == NULL){
	fprintf(stderr, "InitAGG: error malloc: %d\n", countAGG * sizeof(AGGDATA));
	return FALSE;
    }
    sizeMemory += countAGG * sizeof(AGGDATA);

    int i;
    // читаем FAT
    for(i = 0; i < countAGG; ++i){
	fseek(fd, sizeof(Uint32), SEEK_CUR);
	fread(&ptrAGG[i].offset, sizeof(ptrAGG[i].offset), 1, fd);
	fread(&ptrAGG[i].size, sizeof(ptrAGG[i].size), 1, fd);
	
	// и заодно значения по умолчанию
	ptrAGG[i].count = 0;
	ptrAGG[i].data = NULL;
    }

    // устанавливаем смещение на начало блока имен
    fseek(fd, 0, SEEK_END);
    fseek(fd,  ftell(fd) - countAGG * AGGSIZENAME, SEEK_SET);

    char *ext = NULL;
    // читаем список файлов AGG
    for(i = 0; i < countAGG; ++i){
	fread(&ptrAGG[i].name, sizeof(char), AGGSIZENAME, fd);

	if(NULL != (ext = strrchr(ptrAGG[i].name, '.'))){
	    ++ext;

	    if(0 == strncmp(ext, "ICN", 3))
		ptrAGG[i].type = ICN;

	    else if(0 == strncmp(ext, "82M", 3))
		ptrAGG[i].type = WAV;

	    else if(0 == strncmp(ext, "BIN", 3))
		ptrAGG[i].type = BIN;

	    else if(0 == strncmp(ext, "BMP", 3))
		ptrAGG[i].type = BMP;

	    else if(0 == strncmp(ext, "FNT", 3))
		ptrAGG[i].type = FNT;

	    else if(0 == strncmp(ext, "PAL", 3))
		ptrAGG[i].type = PAL;

	    else if(0 == strncmp(ext, "TIL", 3))
		ptrAGG[i].type = TIL;
	
	    else if(0 == strncmp(ext, "XMI", 3))
		ptrAGG[i].type = XMI;

	    else
		ptrAGG[i].type = UNK;
	}else
	    ptrAGG[i].type = UNK;

	ptrAGG[i].count = 0;

    }

    // загружаем палитру
    AGGDATA *ptr = NULL;
    if(NULL != (ptr = GetHeaderAGGName("KB.PAL"))){

	// память для палитры
	colorsPtr = (SDL_Color *) malloc(256 * sizeof(SDL_Color));
	if(colorsPtr == NULL){
	    fprintf(stderr, "InitAGG: error malloc: %d\n", 256 * sizeof(SDL_Color));
	    return FALSE;
	}

	// buffer для файла
	char *icnFileData = (char *) malloc(ptr->size);
	if(icnFileData == NULL){
	    fprintf(stderr, "InitAGG: error malloc: %d\n", ptr->size);
	    return FALSE;
	}
		    
	// read
	fseek(fd, ptr->offset, SEEK_SET);
	fread(icnFileData, 1, ptr->size, fd);

	// Fill colors with color information
	char *palette = icnFileData;
	int j;
	for(j = 0; j < 256; ++j){
	    colorsPtr[j].r = *palette << 2;
	    ++palette;
	    colorsPtr[j].g = *palette << 2;
	    ++palette;
	    colorsPtr[j].b = *palette << 2;
	    ++palette;
	}
	// определяем COLORKEY в палитре как фиолетовый 0xFF00FF
	colorsPtr[COLORKEY].r = 0xFF;
	colorsPtr[COLORKEY].g = 0x00;
	colorsPtr[COLORKEY].b = 0xFF;
	free(icnFileData);
	if(GetIntValue(DEBUG)) fprintf(stderr, "load palette: kb.pal\n");
    }

    return TRUE;	    
}

void FreeAGG(){

    ICNHEADER		*hi = NULL, *ti = NULL;
    SDL_AudioCVT	*ptrCVT = NULL;

    int i;

    if(colorsPtr) free(colorsPtr);

    if(ptrAGG)
	for(i = 0; i < countAGG; ++i)
	    switch(ptrAGG[i].type){

		case WAV:
		    if(ptrAGG[i].data){
			ptrCVT = (SDL_AudioCVT *) ptrAGG[i].data;

			if(ptrCVT->buf) free(ptrCVT->buf);

			free(ptrAGG[i].data);

			ptrAGG[i].data = NULL;
		    }
		    break;

		case ICN:
		    // освобождаем ICN	
		    if(NULL == ptrAGG[i].data) break;
		    
		    ti = (ICNHEADER *) ptrAGG[i].data;
		    
		    while(ti != NULL){
			hi = ti;
			ti = (ICNHEADER *) ti->next;
			if(hi->surface) SDL_FreeSurface(hi->surface);
			if(hi->reflect) SDL_FreeSurface(hi->reflect);
			free(hi);
		    }

		    ptrAGG[i].data = NULL;
		    break;

		case BMP:
		case BIN:
		case FNT:
		case PAL:
		    break;

		case TIL:
		    if(ptrAGG[i].data) free(ptrAGG[i].data);
		    ptrAGG[i].data = NULL;
		    break;

		case XMI:
		    break;

		case UNK:
		    if(ptrAGG[i].data) free(ptrAGG[i].data);
		    ptrAGG[i].data = NULL;
		    break;
	    }

    free(ptrAGG);
    fclose(fd);
}

SDL_Color * GetPalette(void){

    return colorsPtr;
}

AGGDATA * GetHeaderAGGName(const char *name){

    int i;
    for(i = 0; i < countAGG; ++i)
	if(0 == strncmp(ptrAGG[i].name, name, strlen(name)))
	    return &ptrAGG[i];

    fprintf(stderr, "GetHeaderAGGName error: object not found %s", name);
    return NULL;
}

/* Функция возвращает указатель на SDL_AudioCVT по имени объекта */
SDL_AudioCVT * GetAudioCVT(const char *name){

    AGGDATA *ptr = GetHeaderAGGName(name);
    
    // находимся не в том месте
    if(ptr->type != WAV){
	fprintf(stderr, "GetAudioCVT error: type not WAV, name %s\n", name);
	return NULL;
    }

    // загружаем
    if(NULL == ptr->data){

	ptr->data = malloc(sizeof(SDL_AudioCVT));

	if(ptr->data == NULL){
    	    fprintf(stderr, "error malloc: %d\n", sizeof(SDL_AudioCVT));
	    return NULL;
	}

	SDL_AudioCVT *ptrData = (SDL_AudioCVT *) ptr->data;
	SDL_BuildAudioCVT(ptrData, 8, 1, 22050, AUDIO_S16, 1, 22050);

	ptrData->buf = (Uint8 *) malloc(ptr->size * ptrData->len_mult);

	if(ptrData->buf == NULL){
	    fprintf(stderr, "error malloc: %d\n", ptr->size);
	    return NULL;
	}

	// read
	fseek(fd, ptr->offset, SEEK_SET);
	fread(ptrData->buf, 1, ptr->size, fd);
	ptrData->len = ptr->size;
	SDL_ConvertAudio(ptrData);
	
	ptr->count = 1;
	sizeMemory += ptr->size;
    }

    ++ptr->life;

    return (SDL_AudioCVT *) ptr->data;
}

AGGTYPE ExistsAGGName(const char *name){

    AGGDATA *ptrAGGData = NULL;

    if(NULL != (ptrAGGData = GetHeaderAGGName(name))) return ptrAGGData->type;

    fprintf(stderr, "ExistsAGGName object not found: %s\n", name);
    
    return UNK;
}

Uint8 * GetTILData(const char *name){

    AGGDATA *ptr = GetHeaderAGGName(name);

    // находимся не в том месте
    if(ptr->type != TIL){
	fprintf(stderr, "GetTILData error: type not TIL, name %s\n", name);
	return NULL;
    }

    // загружаем
    if(NULL == ptr->data){
	ptr->data = malloc(ptr->size);

	if(ptr->data == NULL){
	    fprintf(stderr, "error malloc: %d\n", ptr->size);
	    return NULL;
	}

	// read INSIDETILHEADER
	INSIDETILHEADER header;
	fseek(fd, ptr->offset, SEEK_SET);
	fread((char *) &header, 1, sizeof(INSIDETILHEADER), fd);

	ptr->count = header.count;
	// read all
	fread(ptr->data, 1, ptr->size - sizeof(INSIDETILHEADER), fd);
	
	sizeMemory += ptr->size;
    }

    ++ptr->life;

    return ptr->data;
}

/* функция загружает преднамеренно загружает объект */
void PreloadObject(const char *object){

    if(UNK == ExistsAGGName(object)) return;

    char *ext = NULL;
    AGGSPRITE sprite;
    AGGDATA *ptrAGGData = GetHeaderAGGName(object);

    if(NULL != (ext = (char *) strrchr(object, '.'))){
	++ext;

	if(0 == strncmp(ext, "ICN", 3)){

	    FillSPRITE(&sprite, object, 0);
	    GetICNSprite(&sprite);
	    // счетчик в нуль (сборщик мусора его пропускает)
	    ptrAGGData->life = 0;

	}else if(0 == strncmp(ext, "82M", 3)){

	    GetAudioCVT(object);
	    // счетчик в нуль
	    ptrAGGData->life = 0;

	}else
	    fprintf(stderr, "PreloadObject: only ICN and 82M support.\n");
    }else
	fprintf(stderr, "PreloadObject: unknown object.\n");
}

/* функция освобождает определенный объект */
void FreeObject(const char *name){

    AGGDATA *ptr = GetHeaderAGGName(name);
    ICNHEADER *hi = NULL, *ti = NULL;
    SDL_AudioCVT *ptrCVT = NULL;

    switch(ptr->type){
    
	case WAV:
	    if(NULL == ptr->data) break;
	    ptrCVT = (SDL_AudioCVT *) ptr->data;
	    if(ptrCVT->buf) free(ptrCVT->buf);
	    free(ptr->data);
	    sizeMemory -= ptr->size;
	    ptr->data = NULL;
	    break;

	case ICN:
	    if(NULL == ptr->data) break;
	    ti = (ICNHEADER *) ptr->data;
	    while(ti != NULL){
		hi = ti;
		ti = (ICNHEADER *) ti->next;
		if(hi->surface) SDL_FreeSurface(hi->surface);
		if(hi->reflect) SDL_FreeSurface(hi->reflect);
		free(hi);
	    }
	    ptr->data = NULL;
	    sizeMemory -= ptr->size;
	    break;

	case TIL:
	    if(ptr->data) free(ptr->data);
	    ptr->data = NULL;
	    sizeMemory -= ptr->size;
	    break;

	default:
	    break;
    }
}

/* функция возвращает общее значение занимаемой памяти объектами */
Uint32 GetCurrentSizeMemory(void){

    return sizeMemory;
}

SDL_Surface *GetReflectICNSprite(AGGSPRITE *object){

    AGGDATA *ptrAGGData = NULL;
    Sint16 y, x, y2, x2;
    Uint16 *ptrPixel;

    if(1 > strlen(object->name)) return NULL;

    if(NULL == (ptrAGGData = GetHeaderAGGName(object->name))){
	fprintf(stderr, "GetReflectICNSprite object not found: %s, %d\n", object->name, object->number);
	return NULL;
    }

    if(ptrAGGData->type != ICN){
	fprintf(stderr, "GetReflectICNSprite error: type not ICN, name %s\n", object->name);
	return NULL;
    }

    // загружаем
    if(ptrAGGData->data == NULL) PreloadObject(object->name);

    // если существуют данные
    if(ptrAGGData->data){

	if(object->number > ptrAGGData->count){
	    fprintf(stderr, "GetReflectICNSprite error: name %s, number %d, > count %d\n", object->name, object->number, ptrAGGData->count);
	    object->number = ptrAGGData->count;
	}

	// увеличиваем приоритет
	++ptrAGGData->life;

	// возвращаем результат
	ICNHEADER *ptr = (ICNHEADER *) ptrAGGData->data;

	Uint16 number = object->number;

	while(number--)
	    if(ptr->next) ptr = (ICNHEADER *) ptr->next;

	if(ptr->reflect) return ptr->reflect;
	
	// reflect surface
	SDL_Surface *src = NULL;
	if(NULL == (src = SDL_CreateRGBSurface(SDL_SWSURFACE, ptr->surface->w, ptr->surface->h, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptr->surface->w, ptr->surface->h);
	    return NULL;
	}

	SDL_FillRect(src, NULL, COLORKEY);
	SDL_BlitSurface(ptr->surface, NULL, src, NULL);

	SDL_Surface *dst = NULL;
	if(NULL == (dst = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 16, 0, 0, 0, 0))){
	    fprintf(stderr, "CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), src->w, src->h);
	    return NULL;
	}

	LockSurface(src);
	LockSurface(dst);
	y2 = 0;
        for(y = 0; y < dst->h; ++y){
            x2 = 0;
            for(x = dst->w - 1; x >= 0; --x){
                ptrPixel = (Uint16 *) dst->pixels + y * dst->pitch / 2 + x;
                *ptrPixel = *((Uint16 *) src->pixels + y2 * src->pitch / 2 + x2);
                ++x2;
            }
    	    ++y2;
        }
	UnlockSurface(src);
	UnlockSurface(dst);
	SDL_FreeSurface(src);

	SDL_SetColorKey(dst, SDL_SRCCOLORKEY|SDL_RLEACCEL, COLORKEY);
	ptr->reflect = SDL_DisplayFormatAlpha(dst);
	SDL_FreeSurface(dst);

	return ptr->reflect;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetReflectICNSprite error: return NULL\n");
    return NULL;
}
