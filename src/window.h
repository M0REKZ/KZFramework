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

#ifndef PLUSKAIZO_WINDOW_H
#define PLUSKAIZO_WINDOW_H

#include <SDL3/SDL.h>

#include "rlsw_include.h"

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

    //3D drawing i think
    void Begin3DDrawing(const char * pdrawmode);
    void Add3DVertex(float x, float y, float z, Uint32 color, float u, float v);
    void End3DDrawing();
    void Clear3DDrawing() { swClear(SW_DEPTH_BUFFER_BIT | SW_COLOR_BUFFER_BIT); }
    void Set3DCamera(float x, float y, float z, float x_look, float y_look, float z_look);

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
