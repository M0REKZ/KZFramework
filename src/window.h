/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This program is shared under the PLUSKAIZO LICENSE, you should have received
    a copy of the license along with the program.
*/

#ifndef PLUSKAIZO_WINDOW_H
#define PLUSKAIZO_WINDOW_H

#include <SDL3/SDL.h>

#include <time.h>

class CKZWindow
{
public:
    //the window
    SDL_Window * pMainWindow = nullptr;

    //render texture to the window
    SDL_Renderer * pMainWindowRenderer = nullptr;

    //the surface where we want to draw
    SDL_Surface * pMainSurface = nullptr;

    //we need to convert the surface to texture before drawing
    SDL_Texture * pMainTexture = nullptr;

    const int Width = 256 * 2;
    const int Height = 244 * 2;

    unsigned int TargetDelay = 16; // 60 FPS default

    bool Init();
    void Destroy();

    void UpdateWindow();

    //Window

    bool ChangeWindowTitle(const char * pnewtitle);
    void SetTargetDelay(unsigned int delay) { TargetDelay = delay; }

    //Drawing

    inline Uint32 ConverToWindowDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return SDL_MapSurfaceRGBA(pMainSurface, r, g, b, a);
    }

    void DrawLine(int x_from, int y_from, int x_to, int y_to, Uint32 color);
    void DrawRect(int x_from, int y_from, int x_to, int y_to, Uint32 color);
    void DrawFillRect(int x_from, int y_from, int x_to, int y_to, Uint32 color);
    void DrawEllipse(int cx, int cy, int rx, int ry, Uint32 color);
    void DrawFillEllipse(int cx, int cy, int rx, int ry, Uint32 color);

    void Delay(int ms);
    void DoTargetDelay()
    {
        Delay(TargetDelay);
    }
    
private:
    timespec NextTime = {0,0};
};

extern CKZWindow KZWindow;

#endif
