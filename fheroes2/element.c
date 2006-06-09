#include "SDL.h"
#include "agg.h"
#include "tools.h"
#include "actionevent.h"
#include "cursor.h"
#include "element.h"

#define BOXWIDTH 306
#define BOXHEIGHT 226

ACTION DialogPressYES(void);
ACTION DialogPressNO(void);

ACTION MessageBox(const char *message, ENUMFONT font){

    CursorOff();
    
    Uint32 cursor = GetCursor();
    
    // отрисовка диалога по центру экрана
    SDL_Surface *format, *back, *elem, *video;
    SDL_Rect rectBack, rectCur;
    ACTION result = NONE;
    AGGSPRITE sprite;
            
    video = SDL_GetVideoSurface();
    rectBack.x = (video->w - BOXWIDTH) / 2;
    rectBack.y = (video->h - BOXHEIGHT) / 2;
    rectBack.w = BOXWIDTH;
    rectBack.h = BOXHEIGHT;

    // сохраняем бакгроунд
    format = SDL_CreateRGBSurface(SDL_SWSURFACE, BOXWIDTH, BOXHEIGHT, 16, 0, 0, 0, 0);
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // получаем левый верхний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 4);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 1;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый средний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x;
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем левый нижний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 6);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый верхний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 0);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + rectCur.w;
    rectCur.y = rectBack.y;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый средний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 1);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // получаем правый нижний спрайт
    FillSPRITE(&sprite, "BUYBUILD.ICN", 2);
    elem = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // рисуем текст
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + 70;
    rectCur.w = BOXWIDTH - 70;
    rectCur.h = BOXHEIGHT - 150;
    PrintText(video, &rectCur, message, font);

    // рисуем кнопки
    INTERFACEACTION action;
    INTERFACEACTION *dialog = NULL;        

    // кнопка YES
    FillSPRITE(&sprite, "SYSTEM.ICN", 5);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + 40;
    rectCur.y = rectBack.y + BOXHEIGHT - 70;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SYSTEM.ICN", 5);
    FillSPRITE(&action.objectPush, "SYSTEM.ICN", 6);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = DialogPressYES;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // кнопка NO
    FillSPRITE(&sprite, "SYSTEM.ICN", 7);
    elem = GetICNSprite(&sprite);
    rectCur.x = rectBack.x + BOXWIDTH - elem->w - 25;
    rectCur.y = rectBack.y + BOXHEIGHT - 70;
    rectCur.w = elem->w;
    rectCur.h = elem->h;
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SYSTEM.ICN", 7);
    FillSPRITE(&action.objectPush, "SYSTEM.ICN", 8);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = DialogPressNO;
    AddActionEvent(&dialog, &action);
    SDL_BlitSurface(elem, NULL, video, &rectCur);

    // Отрисовка диалога
    SDL_Flip(video);
    
    SetCursor(CURSOR_POINTER);
    
    CursorOn();

    // цикл событий
    result = ActionCycle(dialog);

    // востанавливаем бакгроунд
    CursorOff();
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_Flip(video);

    FreeActionEvent(dialog);
    SDL_FreeSurface(back);
    
    SetCursor(cursor);
    
    CursorOn();

    return result;
}

ACTION DialogPressYES(){

    return YES;
}

ACTION DialogPressNO(){

    return NO;
}
