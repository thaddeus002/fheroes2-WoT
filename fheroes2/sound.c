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
#include <string.h>
#include "config.h"
#include "agg.h"
#include "gamedefs.h"
#include "sound.h"

typedef struct{
    Uint8	*data;
    Uint32	size;
    Uint32	pos;
} SOUNDHEADER;

void MixAudio(void *, Uint8 *, int);

static SOUNDHEADER *sounds = NULL;

BOOL InitSound(void){

    if(0 == GetIntValue(SOUND)) return TRUE;

    SDL_AudioSpec fmt;

    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 1024;
    fmt.callback = MixAudio;
    fmt.userdata = NULL;

    if(0 > SDL_OpenAudio(&fmt, NULL)){
	fprintf(stderr, "InitSound: error open audio: %s\n", SDL_GetError());
	return FALSE;
    }
    SDL_PauseAudio(0);
    
    sounds = (SOUNDHEADER *) malloc(NUM_SOUNDS * sizeof(SOUNDHEADER));
    if(sounds == NULL){
	fprintf(stderr, "InitSound: error malloc: %d\n", NUM_SOUNDS * sizeof(SOUNDHEADER));
	return FALSE;
    }
    memset(sounds, 0, NUM_SOUNDS * sizeof(SOUNDHEADER));

    fprintf(stderr, "Init sound.\n");

    return TRUE;
}

void FreeSound(void){

    if(0 == GetIntValue(SOUND)) return;

    SDL_CloseAudio();

    if(sounds) free(sounds);
    sounds = NULL;
}

void MixAudio(void *unused, Uint8 *stream, int len){

    int i;
    Uint32 amount;

    for (i = 0; i < NUM_SOUNDS; ++i){
        amount = sounds[i].size - sounds[i].pos;
        if (amount > len) amount = len;

        SDL_MixAudio(stream, &sounds[i].data[sounds[i].pos], amount, SDL_MIX_MAXVOLUME);
        sounds[i].pos += amount;
    }
}

void PlaySound(char *nameSound){

    if(0 == GetIntValue(SOUND)) return;

    if(WAV != ExistsAGGName(nameSound)) return;

    int index;
    for(index = 0; index < NUM_SOUNDS; ++index)
	if(sounds[index].pos == sounds[index].size) break;

    if(index == NUM_SOUNDS) return;
    
    SDL_AudioCVT *header = GetAudioCVT(nameSound);

    SDL_LockAudio();
    sounds[index].data = header->buf;
    sounds[index].size = header->len_cvt;
    sounds[index].pos = 0;
    SDL_UnlockAudio();

    return;
}
