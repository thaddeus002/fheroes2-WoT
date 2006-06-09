#include <stdlib.h>
#include "SDL.h"
#include "gamedefs.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "stpesize.h"
#include "cursor.h"
#include "config.h"
#include "element.h"
#include "stpemaps.h"

void ShowSelectSize(void);
ACTION ActionPressSmall();
ACTION ActionPressMedium();
ACTION ActionPressLarge();
ACTION ActionPressXLarge();

INTERFACEACTION *stpesize = NULL;

ACTION DrawSelectSize(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;
    stpesize = NULL;

    // кнопка small
    FillSPRITE(&sprite, "BTNESIZE.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 45;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNESIZE.ICN", 0);
    FillSPRITE(&action.objectPush, "BTNESIZE.ICN", 1);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressSmall;
    // регистрируем
    AddActionEvent(&stpesize, &action);

    // кнопка medium
    FillSPRITE(&sprite, "BTNESIZE.ICN", 2);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 110;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNESIZE.ICN", 2);
    FillSPRITE(&action.objectPush, "BTNESIZE.ICN", 3);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressMedium;
    // регистрируем
    AddActionEvent(&stpesize, &action);

    // кнопка large
    FillSPRITE(&sprite, "BTNESIZE.ICN", 4);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 175;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNESIZE.ICN", 4);
    FillSPRITE(&action.objectPush, "BTNESIZE.ICN", 5);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressLarge;
    // регистрируем
    AddActionEvent(&stpesize, &action);

    // кнопка xlarge
    FillSPRITE(&sprite, "BTNESIZE.ICN", 6);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 240;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNESIZE.ICN", 6);
    FillSPRITE(&action.objectPush, "BTNESIZE.ICN", 7);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressXLarge;
    // регистрируем
    AddActionEvent(&stpesize, &action);

    // кнопка cancel
    FillSPRITE(&sprite, "BTNESIZE.ICN", 8);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 375;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "BTNESIZE.ICN", 8);
    FillSPRITE(&action.objectPush, "BTNESIZE.ICN", 9);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressCancel;
    // регистрируем
    AddActionEvent(&stpesize, &action);

    // отображаем всю картинку
    ShowSelectSize();

    ACTION result = ActionCycle(stpesize);

    FreeActionEvent(stpesize);
    
    return result;
}

void ShowSelectSize(void){

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
    FillSPRITE(&sprite, "EDITOR.ICN", 0);
    image = GetICNSprite(&sprite);

    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    // загружаем панель
    FillSPRITE(&sprite, "REDBACK.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 405;
    dest.y = 5;
    dest.w = image->w;
    dest.h = image->h;
    
    SDL_BlitSurface(image, NULL, video, &dest);

    INTERFACEACTION *ptr = stpesize;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressSmall(){

    return NewMaps(MAPS_SMALL);
}

ACTION ActionPressMedium(){

    return NewMaps(MAPS_MEDIUM);
}

ACTION ActionPressLarge(){

    return NewMaps(MAPS_LARGE);
}

ACTION ActionPressXLarge(){

    return NewMaps(MAPS_XLARGE);
}
