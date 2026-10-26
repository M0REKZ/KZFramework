/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This file is part of KZFramework.

    KZFramework is free software: you can redistribute it and/or modify it under the terms of
    the GNU Lesser General Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    KZFramework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with KZFramework.
    If not, see <https://www.gnu.org/licenses/>.
*/

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
    void StopAudio(unsigned int snd);
    void UnloadAllSounds();
    void UnloadSound(unsigned int snd);
};

extern CKZSounds KZSounds;

#endif
