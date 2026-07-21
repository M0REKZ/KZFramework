

#include <SDL3/SDL.h>

#include "window.h"

int main(int argc, char ** args)
{
    KZWindow.Init();

    SDL_Event e;
    bool running = true;
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

        KZWindow.UpdateWindow();
    }

    KZWindow.Destroy();
}
