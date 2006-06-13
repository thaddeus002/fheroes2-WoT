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

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "SDL.h"
#include "agg.h"
#include "actionevent.h"
#include "config.h"
#include "cursor.h"
#include "tools.h"
#include "element.h"
#include "newselectmaps.h"

typedef struct {			// структура заголовка карты с необходимыми полями
    Uint32	magic;			// address: 0x00
    Uint16	level;			// address: 0x04
    Uint8	width;			// address: 0x06
    Uint8	height;			// address: 0x07
    Uint8	count;			// address: 0x1A
    Uint8	conditionsWins;		// address: 0x1D
    Uint8	conditionsLoss;		// address: 0x22
    Uint8	longname[16];		// address: 0x3A
    Uint8	description[143];	// address: 0x76
} LOADMP2HEADER;

typedef struct {
    SDL_Surface	*backSurface;
    SDL_Surface	*cursor;
    SDL_Rect	backRect;
    BOOL	backUse;
    Uint16	maxy;
    Uint16	miny;
    Uint8	step;
} SCROLLCURSOR;

#define MAXLENFILENAME 64
#define MAXNAMES 9			// количество имен в окне выбора

#if defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)
#define	SEPARATOR	"\\"
#else
#define	SEPARATOR	"/"
#endif

typedef struct {				// структура для формирования связанного списка карт
    char	filename[MAXLENFILENAME]; 	// имя файла карты
    LOADMP2HEADER	info;			// информация заголовка
    void	*next;
} FILEMAPINFO;

void ShowSelectLoad(void);		// отображает динамичые элементы с обновлением
void ShowStaticForm(void);		// отображает статичные элементы (без обновления)
void ReadFileMapInfo(void);		// читаем все файлы карт в структуру
void FreeFileMapInfo(void);		// освобождаем память
void ShowListFileMap(void);		// отобразить список карт
void ShowFileMapInfo(FILEMAPINFO *cur);	// отобразить дополнительное инфо карты
void InitSlider(void);
void DrawSlider(Uint16);
void ResetSlider(void);
void FreeSlider(void);
FILEMAPINFO *ExistsMapSizeDown(FILEMAPINFO *, E_SIZEMAP);	// дополнительная функция поиска элемента по критерию (E_SIZEMAP)
FILEMAPINFO *ExistsMapSizeUp(FILEMAPINFO *, E_SIZEMAP);	// дополнительная функция поиска элемента по критерию (E_SIZEMAP)

ACTION ActionPressShowSmall(void);
ACTION ActionPressShowMedium(void);
ACTION ActionPressShowLarge(void);
ACTION ActionPressShowXLarge(void);
ACTION ActionPressShowAll(void);
ACTION ActionPressSelectMap(void);
ACTION ActionPressOKLoad(void);
ACTION ActionPressScrollUp(void);
ACTION ActionPressScrollDown(void);
ACTION ActionPressScrollBar(void);

INTERFACEACTION *stpeload = NULL;
FILEMAPINFO	*header = NULL;			// указатель головы всего списка
FILEMAPINFO	*firstName = NULL;		// указатель текущего первого имени в окне выбора
E_SIZEMAP		showmaps = MAPS_ALL;		// фильтр карты
Uint8		count = 0;			// количество отображенных имен (отсортированных по размеру)
FILEMAPINFO	*currentName = NULL;		// текущий выделенный элемент
SCROLLCURSOR	*slider;

ACTION DrawNewSelectMaps(void){			// типа майн ;)

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    stpeload = NULL;

    // выбор текущей карты (щелчек  по списку)
    dest.x = 174;
    dest.y = 61;
    dest.w = 260;
    dest.h = 172;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressSelectMap;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // скроллинг карт UP
    dest.x = 174;
    dest.y = 61;
    dest.w = 260;
    dest.h = 172;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_UWHEEL;
    action.pf = ActionPressScrollUp;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // скроллинг карт DOWN
    dest.x = 174;
    dest.y = 61;
    dest.w = 260;
    dest.h = 172;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_DWHEEL;
    action.pf = ActionPressScrollDown;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка small
    FillSPRITE(&sprite, "REQUESTS.ICN", 9);
    image = GetICNSprite(&sprite);
    dest.x = 167;
    dest.y = 28;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 9);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 10);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressShowSmall;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка medium
    FillSPRITE(&sprite, "REQUESTS.ICN", 11);
    image = GetICNSprite(&sprite);
    dest.x = 229;
    dest.y = 28;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 11);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 12);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressShowMedium;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка large
    FillSPRITE(&sprite, "REQUESTS.ICN", 13);
    image = GetICNSprite(&sprite);
    dest.x = 291;
    dest.y = 28;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 13);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 14);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressShowLarge;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка xlarge
    FillSPRITE(&sprite, "REQUESTS.ICN", 15);
    image = GetICNSprite(&sprite);
    dest.x = 353;
    dest.y = 28;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 15);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 16);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressShowXLarge;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка all
    FillSPRITE(&sprite, "REQUESTS.ICN", 17);
    image = GetICNSprite(&sprite);
    dest.x = 415;
    dest.y = 28;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 17);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 18);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressShowAll;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка okay
    FillSPRITE(&sprite, "REQUESTS.ICN", 1);
    image = GetICNSprite(&sprite);
    dest.x = 270;
    dest.y = 415;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp ,"REQUESTS.ICN", 1);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 2);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressOKLoad;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка scroll up
    FillSPRITE(&sprite, "REQUESTS.ICN", 5);
    image = GetICNSprite(&sprite);
    dest.x = 457;
    dest.y = 60;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 5);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 6);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollUp;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // кнопка scroll down
    FillSPRITE(&sprite, "REQUESTS.ICN", 7);
    image = GetICNSprite(&sprite);
    dest.x = 457;
    dest.y = 222;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "REQUESTS.ICN", 7);
    FillSPRITE(&action.objectPush, "REQUESTS.ICN", 8);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollDown;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    // scroll bar
    dest.x = 459;
    dest.y = 76;
    dest.w = 10;
    dest.h = 143;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressScrollBar;
    // регистрируем
    AddActionEvent(&stpeload, &action);

    ReadFileMapInfo();
    InitSlider();

    // отображаем всю картинку
    ShowStaticForm();

    ACTION result = ActionCycle(stpeload);

    FreeActionEvent(stpeload);
    FreeSlider();
    FreeFileMapInfo();

    return result;
}

void ShowStaticForm(void){

    SDL_Surface *video; 
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    // only 640x480
    Uint32 flag = SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    if(TRUE == GetIntValue("fullscreen")) flag |= SDL_FULLSCREEN;
    if(NULL == (video = SDL_GetVideoSurface()) || video->w != 640 || video->h !=480)
        video = SDL_SetVideoMode(640, 480, 16, flag);

    if(NULL == video){
	fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
	return;
    }

    // загружаем фон
    FillSPRITE(&sprite, "HEROES.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // загружаем тень ;)
    FillSPRITE(&sprite, "REQSBKG.ICN", 1);
    image = GetICNSprite(&sprite);
    dest.x = 114;
    dest.y = 21;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    CursorOn();

    // дополнительно
    ShowSelectLoad();
}

void ShowSelectLoad(void){

    SDL_Surface *video; 
    SDL_Surface *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    video = SDL_GetVideoSurface();

    CursorOff();
    // загружаем панель
    FillSPRITE(&sprite, "REQSBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 130;
    dest.y = 5;
    dest.w = image->w;
    dest.h = image->h;
    
    SDL_BlitSurface(image, NULL, video, &dest);

    INTERFACEACTION *ptr = stpeload;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }


    ShowListFileMap();
    ShowFileMapInfo(currentName);

    SDL_Flip(video);
    CursorOn();

    // slider
    DrawSlider(slider->backRect.y);
}

ACTION ActionPressSelectMap(){

    SDL_Rect dest;
    int x, y, i, j;
    
    dest.x = 174;
    dest.y = 61;
    dest.w = 260;
    dest.h = 19;

    SDL_GetMouseState(&x, &y);

    // поиск currentName
    FILEMAPINFO *ptr = firstName;

    for(i = 0; i < MAXNAMES; ++i){

	if(ValidPoint(&dest, (Uint16) x, (Uint16) y)){
	    if(0 != i)
		for(j = 0; j < i; ++j)
		    ptr = ExistsMapSizeDown(ptr, showmaps);
	    
	    currentName = ptr;

	    break;
	}

	dest.y += dest.h;
    }

    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressShowSmall(){

    showmaps = MAPS_SMALL;
    if(ExistsMapSizeDown(header, showmaps))
	firstName = ExistsMapSizeDown(header, showmaps);

    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressShowMedium(){

    showmaps = MAPS_MEDIUM;
    if(ExistsMapSizeDown(header, showmaps))
	firstName = ExistsMapSizeDown(header, showmaps);
    
    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressShowLarge(){

    showmaps = MAPS_LARGE;
    if(ExistsMapSizeDown(header, showmaps))
	firstName = ExistsMapSizeDown(header, showmaps);
    
    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressShowXLarge(){

    showmaps = MAPS_XLARGE;
    if(ExistsMapSizeDown(header, showmaps))
	firstName = ExistsMapSizeDown(header, showmaps);
    
    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressShowAll(){

    showmaps = MAPS_ALL;
    firstName = NULL;

    ResetSlider();
    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressOKLoad(){

    if(firstName == NULL){
	fprintf(stderr, "ActionPressOKLoad: error open maps file.\n");
	return EXIT;
    }

    SetStrValue("filemapspath", currentName->filename);
    SetStrValue("mapslongname", (char *) currentName->info.longname);
    SetStrValue("mapsdescription", (char *) currentName->info.description);

    return OK;
}

ACTION ActionPressScrollUp(){

    // если первый элемент
    if(firstName == header) return NONE;

    if(ExistsMapSizeUp(firstName, showmaps))
	firstName = ExistsMapSizeUp(firstName, showmaps);

    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressScrollDown(){

    // если последний элемент
    if(NULL == firstName->next) return NONE;
    
    // если число отображенных меньше максимально возможного в списке
    if(count < MAXNAMES) return NONE;

    if(ExistsMapSizeDown(firstName, showmaps))
	firstName = ExistsMapSizeDown(firstName, showmaps);

    ShowSelectLoad();

    return NONE;
}

ACTION ActionPressScrollBar(){

    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);

    DrawSlider(mousey);

    return NONE;
}

void ReadFileMapInfo(){

    DIR *dp;
    struct dirent *ep;

    char 	*ext;

    FILEMAPINFO	*ptr = NULL;
    FILE	*fd = NULL;

    // читаем
    dp = opendir(GetStrValue("directorymaps"));
    
    if(dp == NULL){
	fprintf(stderr, "ReadFileMapInfo: error open directory %s\n", GetStrValue("directorymaps"));
	return;
    }

    while(NULL != (ep = readdir(dp))){

	if(NULL == (ext = strrchr(ep->d_name, '.'))) continue;
	if(MAXLENFILENAME - 1 < strlen(ep->d_name)) continue;

	if(0 == strcmp(++ext, "mp2") || 0 == strcmp(++ext, "MP2")){

	    // выделяем память для цепочки структуры информации файлов карты
	    if(NULL == header){
		// голова
		header = malloc(sizeof(FILEMAPINFO));
		if(NULL == header){
		    fprintf(stderr, "ReadFileMapInfo: error malloc: %d\n", sizeof(FILEMAPINFO));
		    return;
		}
		ptr = header;
	    }else{
		// хвост
		ptr->next = malloc(sizeof(FILEMAPINFO));
		if(NULL == ptr->next){
		    fprintf(stderr, "ReadFileMapInfo: error malloc: %d\n", sizeof(FILEMAPINFO));
		    return;
		}
		ptr = ptr->next;
	    }

	    // заполняем
	    ptr->next = NULL;
	    memset(ptr->filename, 0, MAXLENFILENAME);
	    char *pf = ptr->filename;
	    strncat(pf, (const char *) GetStrValue("directorymaps"), strlen(GetStrValue("directorymaps")));
	    pf += strlen(GetStrValue("directorymaps"));
	    strncat(pf, SEPARATOR, 1);
	    ++pf;
	    strncat(pf, (const char *) ep->d_name, strlen(ep->d_name) + 1);

	    // открываем файл
	    if(NULL == (fd = fopen(ptr->filename, "rb"))){
        	fprintf(stderr, "ReadFileMapInfo: error fopen: %s\n", ptr->filename);
	        return;
    	    }
	    // читаем заголовок
	    fread(&ptr->info.magic, sizeof(Uint32), 1, fd);

	    // проверяем magic
	    if(ptr->info.magic != 0x0000005C){
        	fprintf(stderr, "ReadFileMapInfo: error magic: %s\n", ptr->filename);
		// освобождаем блок
		free(ptr);
		ptr = NULL;
		ptr = header;
		// ищем хвост
		while(ptr->next) ptr = ptr->next;
		// следующий файл
		continue;
	    }

	    fread(&ptr->info.level, sizeof(Uint16), 1, fd);
	    fread(&ptr->info.width, sizeof(Uint8), 1, fd);
	    fread(&ptr->info.height, sizeof(Uint8), 1, fd);
	    fseek(fd, 0x1A, SEEK_SET);
	    fread(&ptr->info.count, sizeof(Uint8), 1, fd);
	    fseek(fd, 0x1D, SEEK_SET);
	    fread(&ptr->info.conditionsWins, sizeof(Uint8), 1, fd);
	    fseek(fd, 0x22, SEEK_SET);
	    fread(&ptr->info.conditionsLoss, sizeof(Uint8), 1, fd);
	    fseek(fd, 0x3A, SEEK_SET);
	    fread(&ptr->info.longname, sizeof(Uint8), 16, fd);
	    fseek(fd, 0x76, SEEK_SET);
	    fread(&ptr->info.description, sizeof(Uint8), 143, fd);

	    // закрываем файл
	    fclose(fd);
	    ptr->next = NULL;
	}
    }

    closedir(dp);
}

void FreeFileMapInfo(void){

    if(NULL == header) return;

    FILEMAPINFO *ptr = header;

    while(ptr){
	ptr = header->next;
	free(header);
	header = ptr;
    }

    header = NULL;
    firstName = NULL;
    currentName = NULL;
}

void ShowListFileMap(void){

    SDL_Surface *video, *image;
    SDL_Rect dest;
    AGGSPRITE sprite;

    video = SDL_GetVideoSurface();

    int i;
    
    // инициализация первого элемента
    count = 0;
    if(NULL == firstName)
	switch(showmaps){
	    case MAPS_ALL:
		firstName = header;
		break;
	    case MAPS_SMALL:
	    case MAPS_MEDIUM:
	    case MAPS_LARGE:
	    case MAPS_XLARGE:
		firstName = ExistsMapSizeDown(header, showmaps);
		break;
	}

    if(NULL == currentName) currentName = firstName;

    FILEMAPINFO *ptr = firstName;

    dest.y = 64;

    for(i = 0; i < MAXNAMES; ++i){

	if(NULL != ptr){

	    // отображаем значек количества героев
	    FillSPRITE(&sprite, "REQUESTS.ICN", 19 + ptr->info.count);
	    image = GetICNSprite(&sprite);
	    dest.x = 176;
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &dest);

	    // отображаем значек размера карты
	    switch(ptr->info.width){
		case MAPS_SMALL:
		    FillSPRITE(&sprite, "REQUESTS.ICN", 26);
		    break;
		case MAPS_MEDIUM:
		    FillSPRITE(&sprite, "REQUESTS.ICN", 27);
		    break;
		case MAPS_LARGE:
		    FillSPRITE(&sprite, "REQUESTS.ICN", 28);
		    break;
		case MAPS_XLARGE:
		    FillSPRITE(&sprite, "REQUESTS.ICN", 29);
		    break;
		default:
		    FillSPRITE(&sprite, "REQUESTS.ICN", 30);
		    break;
	    }
	    image = GetICNSprite(&sprite);
	    dest.x += dest.w + 2;
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &dest);
	    
	    // отображаем имя карты
	    dest.x += dest.w + 14;
	    dest.w = FONT_WIDTHBIG * 10;
	    dest.h = FONT_HEIGHTBIG / 2;
	    PrintText(video, &dest, (const char *) ptr->info.longname, FONT_BIG);

	    // отображаем условия победы
	    FillSPRITE(&sprite, "REQUESTS.ICN", 30 + ptr->info.conditionsWins);
	    image = GetICNSprite(&sprite);
	    dest.x = 400;
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &dest);

	    // отображаем условия проигрыша
	    FillSPRITE(&sprite, "REQUESTS.ICN", 36 + ptr->info.conditionsLoss);
	    image = GetICNSprite(&sprite);
	    dest.x += dest.w + 2;
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &dest);

	    dest.y += 19;
	    count++;
	    ptr  = ExistsMapSizeDown(ptr, showmaps);
	}
    }
}

void ShowFileMapInfo(FILEMAPINFO *cur){

    if(cur == NULL) return;

    SDL_Rect dest;
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    AGGSPRITE sprite;

    // отображаем значек количества героев
    FillSPRITE(&sprite, "REQUESTS.ICN", 19 + cur->info.count);
    image = GetICNSprite(&sprite);
    dest.x = 196;
    dest.y = 269;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);
    // отображаем значек размера карты
    switch(cur->info.width){
	case MAPS_SMALL:
	    FillSPRITE(&sprite, "REQUESTS.ICN", 26);
	    break;
	case MAPS_MEDIUM:
	    FillSPRITE(&sprite, "REQUESTS.ICN", 27);
	    break;
	case MAPS_LARGE:
	    FillSPRITE(&sprite, "REQUESTS.ICN", 28);
	    break;
	case MAPS_XLARGE:
	    FillSPRITE(&sprite, "REQUESTS.ICN", 29);
	    break;
	default:
	    FillSPRITE(&sprite, "REQUESTS.ICN", 30);
	    break;
    }
    image = GetICNSprite(&sprite);
    dest.x += dest.w + 2;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // отображаем имя карты
    dest.x = 240;
    dest.w = FONT_WIDTHBIG * 10;
    dest.h = FONT_HEIGHTBIG / 2;
    PrintText(video, &dest, (const char *) cur->info.longname, FONT_BIG);

    // отображаем условия победы
    FillSPRITE(&sprite, "REQUESTS.ICN", 30 + cur->info.conditionsWins);
    image = GetICNSprite(&sprite);
    dest.x = 405;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // отображаем условия проигрыша
    FillSPRITE(&sprite, "REQUESTS.ICN", 36 + cur->info.conditionsLoss);
    image = GetICNSprite(&sprite);
    dest.x += dest.w + 2;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, video, &dest);

    // level
    dest.x = 360;
    dest.y = 295;
    dest.w = 100;
    dest.h = 15;
    switch(cur->info.level){
	case 0:
	    PrintText(video, &dest, "Easy", FONT_BIG);
	    break;
	case 1:
	    PrintText(video, &dest, "Normal", FONT_BIG);
	    break;
	case 2:
	    PrintText(video, &dest, "Hard", FONT_BIG);
	    break;
	case 3:
	    PrintText(video, &dest, "Expert", FONT_BIG);
	    break;
	default:
	    PrintText(video, &dest, "Unknown", FONT_BIG);
	    break;
    }
    // description
    dest.x = 180;
    dest.y = 323;
    dest.w = 265;
    dest.h = 86;

    PrintText(video, &dest, (const char *) cur->info.description, FONT_BIG);
}

FILEMAPINFO *ExistsMapSizeDown(FILEMAPINFO *cur, E_SIZEMAP size){

    while(cur->next){

	cur = cur->next;

	switch(size){
	    case MAPS_ALL:
		return cur;
		break;
	    
	    case MAPS_SMALL:
		if(cur->info.width == MAPS_SMALL) return cur;
		break;

	    case MAPS_MEDIUM:
		if(cur->info.width == MAPS_MEDIUM) return cur;
		break;

	    case MAPS_LARGE:
		if(cur->info.width == MAPS_LARGE) return cur;
		break;

	    case MAPS_XLARGE:
		if(cur->info.width == MAPS_XLARGE) return cur;
		break;
	}
    }

    return NULL;
}

FILEMAPINFO *ExistsMapSizeUp(FILEMAPINFO *cur, E_SIZEMAP size){

    if(cur == header) return NULL;

    FILEMAPINFO	*ptr = header;
    FILEMAPINFO	*res = NULL;

    switch(size){
	case MAPS_ALL:
	    while(ptr->next != cur) ptr = ptr->next;
	    return ptr;
	    break;

	case MAPS_SMALL:
	case MAPS_MEDIUM:
	case MAPS_LARGE:
	case MAPS_XLARGE:
	    while(cur != (ptr = ExistsMapSizeDown(ptr, size)))
		res = ptr;
	    return res;
    	    break;
    }

    return NULL;
}

void InitSlider(void){

    if(NULL == (slider = (SCROLLCURSOR *) malloc(sizeof(SCROLLCURSOR)))){
	fprintf(stderr, "InitSlider: error malloc: %d\n", sizeof(SCROLLCURSOR));
	return;
    }

    AGGSPRITE sprite;

    FillSPRITE(&sprite, "ESCROLL.ICN", 3);
    slider->cursor = GetICNSprite(&sprite);
    slider->backSurface = NULL;
    if(NULL == (slider->backSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, slider->cursor->w, slider->cursor->h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "DrawSelectNewStandardGame: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), slider->cursor->w, slider->cursor->w);
        return;
    }
    slider->maxy = 218;
    slider->miny = 78;
    slider->backUse = FALSE;
    slider->backRect.x = 460;
    slider->backRect.y = 0;
    slider->backRect.w = slider->cursor->w;
    slider->backRect.h = slider->cursor->h;
    slider->step = 0;
}

void FreeSlider(void){

    if(slider->backSurface) SDL_FreeSurface(slider->backSurface);
    if(slider) free(slider);
}

void DrawSlider(Uint16 y){

    CursorOff();
        
    SDL_Surface *video = SDL_GetVideoSurface();

    if(slider->backUse)
	SDL_BlitSurface(slider->backSurface, NULL, video, &slider->backRect);

    if(y < slider->miny)
	slider->backRect.y = slider->miny;
    else if(y > (slider->maxy - slider->backRect.h))
	slider->backRect.y = slider->maxy - slider->backRect.h;
    else
	slider->backRect.y = y;
    
    SDL_BlitSurface(video,  &slider->backRect, slider->backSurface, NULL);
    slider->backUse = TRUE;

    SDL_BlitSurface(slider->cursor, NULL, video, &slider->backRect);

    CursorOn();
}

void ResetSlider(void){
    slider->backRect.y = slider->miny;
}
