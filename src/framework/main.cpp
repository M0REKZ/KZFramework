

#include <SDL3/SDL.h>

#include "window.h"
#include "images.h"
#include "sounds.h"

bool init_everything()
{
    return KZWindow.Init() && KZSounds.Init();
}

void destroy_everything()
{
    KZSounds.Destroy();
    KZImages.UnloadAllImages();
    KZWindow.Destroy();
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
    int idk = KZImages.LoadImage(0);
    int idksound = KZSounds.LoadAudio(1, true);
    KZSounds.PlayAudio(1, true);
    while(running)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        SDL_Rect aaa{0, 0, KZWindow.Width, KZWindow.Height};
        SDL_FillSurfaceRect(KZWindow.pMainSurface, &aaa, KZWindow.ConverToWindowDrawColor(0, 170, 250, 255));

        KZImages.RenderImage(0, 0, 0, 32, 32, 0, 0, 32, 32);

        KZWindow.UpdateWindow();
    }

    destroy_everything();
}
