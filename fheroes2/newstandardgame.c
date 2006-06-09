/*
    new select standard game
*/

#include <stdlib.h>
#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "sound.h"
#include "cursor.h"
#include "config.h"
#include "newstandardgame.h"
#include "newselectmaps.h"

void	ShowNewStandardGame(void);
void	SelectCursorRestoreBackground(void);
void	SelectCursorSaveBackground(SDL_Rect *);

ACTION	ActionPressNewStandardSelect(void);
ACTION	ActionPressNewStandardOkay(void);
ACTION	ActionPressNewStandardCancel(void);

ACTION	ActionSelectDifficultyEasy(void);
ACTION	ActionSelectDifficultyNormal(void);
ACTION	ActionSelectDifficultyHard(void);
ACTION	ActionSelectDifficultyExpert(void);
ACTION	ActionSelectDifficultyImpossible(void);


INTERFACEACTION *stpenewstandard = NULL;

struct {
    SDL_Rect		rect;
    SDL_Surface		*surface;
    BOOL		use;
} backCursor;

ACTION DrawSelectNewStandardGame(void){

    SDL_Rect dest;
    SDL_Surface *image;

    INTERFACEACTION action;
    AGGSPRITE sprite;

    stpenewstandard = NULL;
    
    // инициализируем backgroundCursor
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);

    backCursor.surface = NULL;
    backCursor.use = FALSE;
    backCursor.rect.x = 0;
    backCursor.rect.y = 0;
    backCursor.rect.w = image->w;
    backCursor.rect.h = image->h;

    if(NULL == (backCursor.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "DrawSelectNewStandardGame: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), image->w, image->h);
            return EXIT;
    }

    // регистрируем события

    // кнопка select file
    FillSPRITE(&sprite, "NGEXTRA.ICN", 64);
    image = GetICNSprite(&sprite);
    dest.x = 513;
    dest.y = 77;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 64);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 65);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardSelect;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // кнопка okey select
    FillSPRITE(&sprite, "NGEXTRA.ICN", 66);
    image = GetICNSprite(&sprite);
    dest.x = 234;
    dest.y = 413;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 66);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 67);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardOkay;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // кнопка cancel select
    FillSPRITE(&sprite, "NGEXTRA.ICN", 68);
    image = GetICNSprite(&sprite);
    dest.x = 491;
    dest.y = 413;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    FillSPRITE(&action.objectUp, "NGEXTRA.ICN", 68);
    FillSPRITE(&action.objectPush, "NGEXTRA.ICN", 69);
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionPressNewStandardCancel;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty easy
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 225;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyEasy;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty normal
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 302;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyNormal;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty hard
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 378;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyHard;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty expert
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 455;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyExpert;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);

    // select difficulty impossible
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    dest.x = 532;
    dest.y = 124;
    dest.w = image->w;
    dest.h = image->h;
    // обнуляем структуру
    ZeroINTERFACEACTION(&action);
    // заполняем
    action.rect = dest;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionSelectDifficultyImpossible;
    // регистрируем
    AddActionEvent(&stpenewstandard, &action);



                        
    // отображаем всю картинку
    ShowNewStandardGame();

    // цикл событий
    ACTION result = ActionCycle(stpenewstandard);

    // освобождаем данные
    FreeActionEvent(stpenewstandard);
    if(backCursor.surface) SDL_FreeSurface(backCursor.surface);

    return result;
}

void ShowNewStandardGame(void){

    SDL_Surface *video; 
    SDL_Surface *image;
    SDL_Rect dest;

    AGGSPRITE sprite;

    SetCursor(CURSOR_POINTER);
    CursorOff();

    // на данной картинке работаем только в 640x480
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

    // загружаем тень панели
    FillSPRITE(&sprite, "NGHSBKG.ICN", 1);
    image = GetICNSprite(&sprite);
    dest.x = 196;
    dest.y = 40;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    // загружаем панель
    FillSPRITE(&sprite, "NGHSBKG.ICN", 0);
    image = GetICNSprite(&sprite);
    dest.x = 204;
    dest.y = 33;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, video, &dest);

    // основные надписи
    
    // Scenario
    dest.x = 360;
    dest.y = 53;
    dest.w = FONT_WIDTHBIG * 10;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Scenario:", FONT_BIG);
    // Game Difficulty
    dest.x = 330;
    dest.y = 108;
    dest.w = FONT_WIDTHBIG * 17;
    dest.h = FONT_HEIGHTBIG;
    PrintText(video, &dest, "Game Difficulty:", FONT_BIG);
    // Easy
    dest.x = 248;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 5;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Easy", FONT_SMALL);
    // Normal
    dest.x = 316;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 9;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Normal", FONT_SMALL);
    // Hard
    dest.x = 395;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 5;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Hard", FONT_SMALL);
    // Expert
    dest.x = 472;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 7;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Expert", FONT_SMALL);
    // Impossible
    dest.x = 536;
    dest.y = 196;
    dest.w = FONT_WIDTHSMALL * 12;
    dest.h = FONT_HEIGHTSMALL;
    PrintText(video, &dest, "Impossible", FONT_SMALL);

    INTERFACEACTION *ptr = stpenewstandard;

    // отображаем динамические элементы
    while(ptr){

	if(NULL != (image = GetICNSprite(&ptr->objectUp)))
	    SDL_BlitSurface(image, NULL, video, &ptr->rect);

	ptr = (INTERFACEACTION *) ptr->next;
    }

    SDL_Flip(video);
    CursorOn();
}

ACTION ActionPressNewStandardSelect(void){

    PreloadObject("REQUESTS.ICN");
    PreloadObject("REQSBKG.ICN");
    
    if(EXIT == DrawNewSelectMaps()) return EXIT;

    FreeObject("REQUESTS.ICN");
    FreeObject("REQSBKG.ICN");

    ShowNewStandardGame();

    return NONE;
}

ACTION ActionPressNewStandardOkay(void){

    fprintf(stderr, "%s\n", GetStrValue("filemapspath"));
    return NONE;
}

ACTION ActionPressNewStandardCancel(void){

    return CANCEL;
}

ACTION ActionSelectDifficultyEasy(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.x = 225;
    rect.y = 124;
    rect.w = image->w;
    rect.h = image->h;
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

ACTION ActionSelectDifficultyNormal(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.x = 302;
    rect.y = 124;
    rect.w = image->w;
    rect.h = image->h;
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

ACTION ActionSelectDifficultyHard(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.x = 378;
    rect.y = 124;
    rect.w = image->w;
    rect.h = image->h;
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

ACTION ActionSelectDifficultyExpert(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.x = 455;
    rect.y = 124;
    rect.w = image->w;
    rect.h = image->h;
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

ACTION ActionSelectDifficultyImpossible(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;

    SDL_Rect rect;
    SDL_Surface *video = SDL_GetVideoSurface();
    
    FillSPRITE(&sprite, "NGEXTRA.ICN", 62);
    image = GetICNSprite(&sprite);
    rect.x = 532;
    rect.y = 124;
    rect.w = image->w;
    rect.h = image->h;
    
    CursorOff();

    SelectCursorRestoreBackground();

    SelectCursorSaveBackground(&rect);
    
    SDL_BlitSurface(image, NULL, video, &rect);

    SDL_Flip(video);

    CursorOn();

    return NONE;
}

void SelectCursorRestoreBackground(void){

    if(! backCursor.use) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_BlitSurface(backCursor.surface, NULL, video, &backCursor.rect);
    SDL_Flip(video);
    
    backCursor.use = FALSE;
}

void SelectCursorSaveBackground(SDL_Rect *rect){

    if(backCursor.use) return;

    SDL_Surface *video = SDL_GetVideoSurface();

    SDL_BlitSurface(video, rect, backCursor.surface, NULL);
    
    backCursor.rect = *rect;
    backCursor.use = TRUE;
}
