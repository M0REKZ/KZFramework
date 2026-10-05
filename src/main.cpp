

#include <SDL3/SDL.h>

#include "window.h"
#include "images.h"
#include "sounds.h"
#include "scripts.h"
#include "input.h"
#include "files.h"
#include "text.h"
#include "globals.h"
#include "crossplatform.h"

bool running = true;

bool init_everything()
{
    if(!KZWindow.Init())
    {
        SDL_Log("KZWindow init failed!\n");
        return false;
    }
    
    if(!KZSounds.Init())
    {
        SDL_Log("KZSounds init failed\n");
        return false;
    }
    
    if(!KZFiles.Init())
    {
        SDL_Log("KZFiles init failed\n");
        return false;
    }
    
    if(!KZText.Init())
    {
        SDL_Log("KZText init failed\n");
        return false;
    }
    
    if(!KZScripts.Init())
    {
        SDL_Log("KZScripts (lua) init failed\n");
        return false;
    }

    return true;
}

void destroy_everything()
{
    KZText.Destroy();
    KZSounds.Destroy();
    KZImages.UnloadAllImages();
    KZWindow.Destroy();
    KZScripts.Close();
}

#ifdef __ANDROID__
extern "C" int SDL_main(int argc, char *argv[])
#else
int main(int argc, char ** args)
#endif
{
    int ret = 0;
    if(init_everything())
    {
        KZImages.LoadImage(0);

        #ifdef KZFRAMEWORK_USE_GAME_DIR
        if(KZScripts.RunScript("game/main.lua"))
        #else
        if(KZScripts.RunScript("main.lua"))
        #endif
        {
            KZGlobals.Lock();

            KZCrossPlatform.DoMainLoop();
        }
        else
        {
            SDL_Log("Failed to run main.lua");
        }
    }
    else
    {
        SDL_Log("ERROR: Could not init SDL!!!\n");
        ret = -1;
    }

    destroy_everything();

    return ret;
}
