

#include <SDL3/SDL.h>

#include "window.h"
#include "images.h"
#include "sounds.h"
#include "scripts.h"
#include "input.h"

bool init_everything()
{
    return KZWindow.Init() && KZSounds.Init() && KZScripts.Init();
}

void destroy_everything()
{
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
    /*int idksound = KZSounds.LoadAudio(1, true);
    KZSounds.PlayAudio(1, true);*/


    int x = 0,y = 0;

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
        /*KZWindow.DrawFillRect(0, 0, KZWindow.Width, KZWindow.Height, KZWindow.ConverToWindowDrawColor(0, 170, 250, 255));

        KZImages.RenderImage(0, 0, 0, 32, 32, x, y, 32, 32);

        if(KZInput.InputState.Up)
            y--;
        if(KZInput.InputState.Down)
            y++;
        if(KZInput.InputState.Left)
            x--;
        if(KZInput.InputState.Right)
            x++;

        KZWindow.DrawLine( 50, 50, 100, 150, KZWindow.ConverToWindowDrawColor(255, 0, 0, 255));
        KZWindow.DrawFillEllipse( KZWindow.Width / 2, KZWindow.Height / 2, KZWindow.Width / 4, KZWindow.Height / 4, KZWindow.ConverToWindowDrawColor(255, 0, 0, 255));
        KZWindow.DrawEllipse( KZWindow.Width / 2, KZWindow.Height / 2, KZWindow.Width / 4, KZWindow.Height / 4, KZWindow.ConverToWindowDrawColor(0, 0, 0, 255));
        KZWindow.DrawRect(100, 100, 200, 200, KZWindow.ConverToWindowDrawColor(255, 0, 0, 255));*/

        if(!KZScripts.RunScript("game/main.lua"))
            break;

        KZWindow.UpdateWindow();

        KZWindow.Delay(16);
    }

    destroy_everything();
}
