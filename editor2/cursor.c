#include "SDL.h"
#include "cursor.h"
#include "tools.h"
#include "agg.h"
#include "gamedefs.h"

typedef struct {
    Uint32	name;
    SDL_Surface *image;
    SDL_Surface *background;
    SDL_Rect pos;		// реальная позициа курсора
    BOOL show;			// флаг рисуем или нет
} CURSORHEADER;

static CURSORHEADER cursor;

void CursorSaveBackground(void);
void CursorRestoreBackground(void);

Uint32  GetCursor(void){

    return cursor.name;
}

void SetCursor(Uint32 name){

    if(CURSOR_NULL == name) return;
    if(cursor.name == name) return;

    AGGSPRITE sprite;

    // востановить old
    CursorOff();

    switch(GetHiUint32(name)){

	case 0x015A:
	    FillSPRITE(&sprite, "ADVMCO.ICN", GetLoUint32(name));
	    break;
	case 0x015B:
	    FillSPRITE(&sprite, "CMSECO.ICN", GetLoUint32(name));
	    break;
	case 0x015C:
	    FillSPRITE(&sprite, "SPELCO.ICN", GetLoUint32(name));
	    break;
	default:
	    FillSPRITE(&sprite, "ADVMCO.ICN", GetLoUint32(CURSOR_POINTER));
	    fprintf(stderr, "SetCursor: unknown name cursor %d\n", name);
	    break;
    }

    cursor.name  = name;
    cursor.image = GetICNSprite(&sprite);

    // рисуем курсор
    CursorOn();
}

void CursorSaveBackground(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_BlitSurface(video, &cursor.pos, cursor.background, NULL);
}

void CursorRestoreBackground(void){

    if(NULL == cursor.background) return;

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rest = cursor.pos;
    rest.x = 0;
    rest.y = 0;
	     
    SDL_BlitSurface(cursor.background, &rest, video, &cursor.pos);
}

void CursorShow(Uint16 x, Uint16 y){

    if(0 == cursor.show) return;

    SDL_Surface *formatSurface;

    // востановить old
    CursorRestoreBackground();

    if(NULL == cursor.background){
	formatSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CURSOR_WIDTH, CURSOR_HEIGHT, 16, 0, 0, 0, 0);
	cursor.background = SDL_DisplayFormat(formatSurface);
	SDL_FreeSurface(formatSurface);
    }

    // сохранить new
    cursor.pos.x = x;
    cursor.pos.y = y;
    cursor.pos.w = cursor.image->w;
    cursor.pos.h = cursor.image->h;
    SDL_Surface *video = SDL_GetVideoSurface();

    // рисуем курсор
    CursorSaveBackground();
    SDL_BlitSurface(cursor.image, NULL, video, &cursor.pos);
    SDL_Flip(video);
    return;
}

void InitCursor(void){

    SDL_ShowCursor(SDL_DISABLE);
    cursor.name = CURSOR_NULL;
    cursor.image = NULL;
    cursor.background = NULL;
    cursor.show = TRUE;

    SetCursor(CURSOR_POINTER);

    fprintf(stderr, "Init cursor.\n");
}

void FreeCursor(void){

    if(cursor.background) SDL_FreeSurface(cursor.background);
    SDL_ShowCursor(SDL_ENABLE);
}


void CursorOff(void){

    if(cursor.show){

	CursorRestoreBackground();
	cursor.show = FALSE;
    }
}

void CursorOn(void){

    if(cursor.show) return;

    cursor.show = TRUE;
    CursorSaveBackground();
    CursorShow(cursor.pos.x, cursor.pos.y);
}
