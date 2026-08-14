#ifndef PLUSKAIZO_SOUNDS_H
#define PLUSKAIZO_SOUNDS_H

#include <SDL3/SDL.h>
#include <unordered_map>
#include <SDL3_mixer/SDL_mixer.h>

class CKZSounds
{
public:
    static const uint8_t MAX_PLAYING_SOUNDS = 8;
private:
    std::unordered_map<unsigned int, MIX_Audio *> LoadedSounds;

    MIX_Mixer * pMainMixer = nullptr;

    struct SSoundState
    {
        MIX_Track * pTrack = nullptr;
        unsigned int SoundID = 0;
        Uint64 StartTime = 0;
        float Volume = 0.f;

        void Reset();
        bool IsPlaying() { return pTrack && MIX_TrackPlaying(pTrack); }
        unsigned int GetSoundID() { return SoundID; }
        Uint64 GetStartTime() { return StartTime; }
    };

    SSoundState BGMusic;
    SSoundState PlayingSounds[MAX_PLAYING_SOUNDS];

public:

    bool Init();
    void Destroy();

    unsigned int LoadAudio(unsigned int snd, bool predecode);
    unsigned int LoadMusic(unsigned int snd) { return LoadAudio(snd, false); }
    unsigned int LoadSoundEffect(unsigned int snd) { return LoadAudio(snd, true); }
    bool IsSoundSlotBusy(unsigned int snd);
    bool PlayAudio(unsigned int snd, bool loop, float volume);
    void UnloadAllSounds();
    void UnloadSound(unsigned int snd);
};

extern CKZSounds KZSounds;

#endif
