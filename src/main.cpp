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
