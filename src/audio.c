#include <stddef.h>

#include "audio.h"

#define MUSIC_LOADED(m) ((m).stream.buffer != NULL)
#define SOUND_LOADED(s) ((s).stream.buffer != NULL)

bool init_audio(GameAudio *a)
{
    if (!a)
        return false;

    InitAudioDevice();

    a->music = LoadMusicStream("assets/music.wav");
    a->mark = LoadSound("assets/mark.mp3");
    a->save = LoadSound("assets/save.wav");

    if (MUSIC_LOADED(a->music))
    {
        SetMusicVolume(a->music, 0.4f);
        PlayMusicStream(a->music);
    }

    return true;
}

void update_audio(GameAudio *a)
{
    if (!a)
        return;
    if (MUSIC_LOADED(a->music))
        UpdateMusicStream(a->music);
}

void play_mark(GameAudio *a)
{
    if (!a)
        return;
    if (SOUND_LOADED(a->mark))
        PlaySound(a->mark);
}

void play_save(GameAudio *a)
{
    if (!a)
        return;
    if (SOUND_LOADED(a->save))
        PlaySound(a->save);
}

void close_audio(GameAudio *a)
{
    if (!a)
        return;
    if (MUSIC_LOADED(a->music))
        UnloadMusicStream(a->music);
    if (SOUND_LOADED(a->mark))
        UnloadSound(a->mark);
    if (SOUND_LOADED(a->save))
        UnloadSound(a->save);
    CloseAudioDevice();
}