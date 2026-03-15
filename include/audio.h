#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include <stdbool.h>

typedef struct
{
    Music music;
    Sound mark;
    Sound save;
} GameAudio;

bool init_audio(GameAudio *a);
void update_audio(GameAudio *a);
void play_mark(GameAudio *a);
void play_save(GameAudio *a);
void close_audio(GameAudio *a);

#endif