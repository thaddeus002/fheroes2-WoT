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

int InitSound(void){

    if(0 == GetIntValue("sound")) return 1;

    SDL_AudioSpec fmt;

    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 1024;
    fmt.callback = MixAudio;
    fmt.userdata = NULL;

    if(0 > SDL_OpenAudio(&fmt, NULL)){
	fprintf(stderr, "error open audio: %s\n", SDL_GetError());
	return 1;
    }
    SDL_PauseAudio(0);
    
    sounds = (SOUNDHEADER *) malloc(NUM_SOUNDS * sizeof(SOUNDHEADER));
    if(sounds == NULL){
	fprintf(stderr, "error malloc: %d\n", NUM_SOUNDS * sizeof(SOUNDHEADER));
	return 1;
    }
    memset(sounds, 0, NUM_SOUNDS * sizeof(SOUNDHEADER));

    fprintf(stderr, "Init sound.\n");

    return 0;
}

void FreeSound(void){

    if(0 == GetIntValue("sound")) return;

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

    if(0 == GetIntValue("sound")) return;

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
