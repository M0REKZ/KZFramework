

#include <SDL3/SDL.h>

#include "window.h"
#include "images.h"
#include "sounds.h"
#include "scripts.h"
#include "input.h"
#include "files.h"
#include "text.h"

bool init_everything()
{
    return KZWindow.Init() && KZSounds.Init() && KZFiles.Init() && KZText.Init() && KZScripts.Init();
}

void destroy_everything()
{
    KZText.Destroy();
    KZSounds.Destroy();
    KZImages.UnloadAllImages();
    KZWindow.Destroy();
    KZScripts.Close();
}

int main(int argc, char ** args)
{
    if(!init_everything())
    {
        printf("ERROR: Could not init SDL!!!\n");
        return -1;
    }

    SDL_Event e;
    bool running = true;
    KZImages.LoadImage(0);

    if(!KZScripts.RunScript("game/main.lua"))
    {
        goto epic_quit;
    }

    while(running)
    {
        KZInput.UpdateInput();
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            KZInput.HandleInput(e);
        }

        if(!KZScripts.RunMain())
            break;

        KZWindow.UpdateWindow();

        KZWindow.Delay(16);
    }

    epic_quit:

    destroy_everything();
}
