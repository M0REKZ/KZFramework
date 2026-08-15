
#include <string>

#include "sounds.h"

CKZSounds KZSounds;

bool CKZSounds::Init()
{
    if(!MIX_Init())
    {
        printf("ERROR INIT SDL_MIXER: %s\n", SDL_GetError());
        return false;
    }

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = 22050;
    spec.format = SDL_AUDIO_S16;
    spec.channels = MAX_PLAYING_SOUNDS;

    pMainMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

    if(!pMainMixer)
    {
        printf("ERROR INIT SDL_MIXER: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void CKZSounds::Destroy()
{
    BGMusic.Reset();

    for(auto &playingsound : PlayingSounds)
    {
        playingsound.Reset();
    }

    for(auto &sound : LoadedSounds)
    {
        MIX_DestroyAudio(sound.second);
    }
    LoadedSounds.clear();

    MIX_DestroyMixer(pMainMixer);

    MIX_Quit();
}

unsigned int CKZSounds::LoadAudio(unsigned int snd, bool predecode)
{
    if(LoadedSounds.find(snd) != LoadedSounds.end())
        return snd;

    //try mp3
    std::string filename = "game/data/audios/" + std::to_string(snd) + ".mp3";

    MIX_Audio * pSnd = MIX_LoadAudio(pMainMixer, filename.c_str(), predecode);

    if(pSnd)
        goto onsuccess;

    //try wav
    filename = "game/data/audios/" + std::to_string(snd) + ".wav";

    pSnd = MIX_LoadAudio(pMainMixer, filename.c_str(), predecode);

    if(pSnd)
        goto onsuccess;

    printf("ERROR: Could not load audio %d %s!\n", snd, filename.c_str());

    //0 on error
    return 0;

    onsuccess:
    {
        LoadedSounds[snd] = pSnd;
        return snd;
    }
}

bool CKZSounds::IsSoundSlotBusy(unsigned int snd)
{
    return LoadedSounds.find(snd) != LoadedSounds.end();
}

bool CKZSounds::PlayAudio(unsigned int snd, bool loop, float volume)
{
    if(LoadedSounds.find(snd) == LoadedSounds.end())
    {
        printf("ERROR: Audio %d is not loaded!\n", snd);
        return false;
    }

    int bestchannel = -1;

    //dont play if there is the same sound playing in this lapse
    Uint32 Lapse = 30;
    int AllowedInstances = 1;

    Uint64 currenttime = SDL_GetTicks();

    // Try to find free channel
    for (int i = 0; i < MAX_PLAYING_SOUNDS; i++)
    {
        if (!PlayingSounds[i].IsPlaying())
        {
            bestchannel = i;
        }
 
        if (PlayingSounds[i].GetSoundID() == snd)
        {
            if(currenttime < PlayingSounds[i].GetStartTime() + Lapse)
            {
                //Same sound already just started playing, dont overlap
                //but check for allowed instances first
                AllowedInstances--;
                if(!AllowedInstances)
                    return false;
                else
                    continue;
            }

            bestchannel = i;
            break;
        }
    }
    
    if (bestchannel == -1)
        return false;

    PlayingSounds[bestchannel].Reset();

    PlayingSounds[bestchannel].pTrack = MIX_CreateTrack(pMainMixer);
    if(!(PlayingSounds[bestchannel].pTrack && MIX_SetTrackAudio(PlayingSounds[bestchannel].pTrack, LoadedSounds[snd])))
    {
        printf("ERROR: Could not play sound %d!! %s\n", snd, SDL_GetError());
        return false;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0);
    MIX_SetTrackGain(PlayingSounds[bestchannel].pTrack, volume);
    MIX_PlayTrack(PlayingSounds[bestchannel].pTrack, props);
    PlayingSounds[bestchannel].SoundID = snd;
    PlayingSounds[bestchannel].StartTime = currenttime;
    PlayingSounds[bestchannel].Volume = volume;

    return true;
}

void CKZSounds::UnloadAllSounds()
{
    for(auto &playingsound : PlayingSounds)
    {
        playingsound.Reset();
    }

    for(auto &sound : LoadedSounds)
    {
        MIX_DestroyAudio(sound.second);
    }
    LoadedSounds.clear();
}

void CKZSounds::UnloadSound(unsigned int snd)
{
    if(LoadedSounds.find(snd) == LoadedSounds.end())
    {
        return;
    }
    MIX_DestroyAudio(LoadedSounds[snd]);
    LoadedSounds.erase(snd);
}

void CKZSounds::SSoundState::Reset()
{
    if(pTrack)
    {
        MIX_DestroyTrack(pTrack);
        pTrack = nullptr;
    }

    SoundID = 0;
    StartTime = 0;
}
