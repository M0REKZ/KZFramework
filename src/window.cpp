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

#include <algorithm>
#include <cmath>

#include "window.h"
#include "crossplatform.h"

CKZWindow KZWindow;

bool CKZWindow::Init()
{
    if(!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        SDL_Log("ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    pMainWindow = SDL_CreateWindow("+KZ Framework Game", Width, Height, SDL_WINDOW_RESIZABLE);
    if(!pMainWindow)
    {
        SDL_Log("ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    pMainWindowRenderer = SDL_CreateRenderer(pMainWindow, nullptr);
    if(!pMainWindowRenderer)
    {
        SDL_Log("ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    //rlsw buffer format is ABGR8888
    pMainTexture = SDL_CreateTexture(pMainWindowRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);
    if(!pMainTexture)
    {
        SDL_Log("ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetTextureScaleMode(pMainTexture, SDL_SCALEMODE_NEAREST);


    if(!swInit(Width, Height))
    {
        SDL_Log("ERROR INIT KZWINDOW: Failed to init RLSW\n");
        return false;
    }

    constexpr double near = 0.1;

    double top = near * std::tan(45.0 * M_PI/360);
    double right = top * ((double)Width)/Height;

    swMatrixMode(SW_PROJECTION);
    swLoadIdentity();
    swFrustum(-right, right, -top, top, 0.1, 4096);

    swEnable(SW_DEPTH_TEST);
    swEnable(SW_TEXTURE_2D);
    swClear(SW_COLOR_BUFFER_BIT | SW_DEPTH_BUFFER_BIT);

    SWerrcode error = swGetError();
    if(error != SW_NO_ERROR)
    {
        SDL_Log("ERROR INIT RLSW: %d\n", error);
        return false;
    }
    

    swMatrixMode(SW_MODELVIEW);
    swLoadIdentity();

    error = swGetError();
    if(error != SW_NO_ERROR)
    {
        SDL_Log("ERROR INIT RLSW: %d\n", error);
        return false;
    }

    int outw, outh;
    void * prlswColorBuffer = swGetColorBuffer(&outw, &outh);

    //now im using rlsw pixel buffer
    //pMainSurface = SDL_CreateSurface(Width, Height, SDL_PIXELFORMAT_RGBA8888);
    //rlsw buffer format is ABGR8888
    pMainSurface = SDL_CreateSurfaceFrom(Width, Height, SDL_PIXELFORMAT_ABGR8888, prlswColorBuffer, Width * sizeof(Uint32));
    if(!pMainSurface)
    {
        SDL_Log("ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void CKZWindow::Destroy()
{
    if(pMainTexture)
        SDL_DestroyTexture(pMainTexture);
    if(pMainSurface)
        SDL_DestroySurface(pMainSurface);
    if(pMainWindowRenderer)
        SDL_DestroyRenderer(pMainWindowRenderer);
    if(pMainWindow)
        SDL_DestroyWindow(pMainWindow);
    swClose();
    SDL_Quit();
}

void CKZWindow::UpdateWindow()
{
    int w, h;
    if(SDL_GetWindowSizeInPixels(pMainWindow, &w, &h))
    {
        SDL_FRect srcrect, destrect; 
        srcrect.x = 0; srcrect.y = 0; srcrect.w = Width; srcrect.h = Height;

        float scaleX = (float)w / srcrect.w;
        float scaleY = (float)h / srcrect.h;
        float scale  = (scaleX < scaleY) ? scaleX : scaleY;

        int destW = (int)(srcrect.w * scale);
        int destH = (int)(srcrect.h * scale);

        destrect.w = destW;
        destrect.h = destH;
        destrect.x = (w - destW) / 2;
        destrect.y = (h - destH) / 2;
        SDL_SetRenderDrawColor(pMainWindowRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pMainWindowRenderer);
        SDL_UpdateTexture(pMainTexture, nullptr, pMainSurface->pixels, pMainSurface->pitch);
        SDL_RenderTexture(pMainWindowRenderer, pMainTexture, &srcrect, &destrect);
        KZCrossPlatform.DrawBasicTouchButtons(); //for android :P
        SDL_RenderPresent(pMainWindowRenderer);
    }
}

bool CKZWindow::ChangeWindowTitle(const char *pnewtitle)
{
    if(pnewtitle && SDL_SetWindowTitle(pMainWindow, pnewtitle))
        return true;
    SDL_Log("ERROR CHANGE WINDOW TITLE SDL: %s\n", SDL_GetError());
    return false;
}

void CKZWindow::Delay(int ms)
{
    if (ms <= 0)
        return;

    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (NextTime.tv_sec == 0 && NextTime.tv_nsec == 0)
    {
        NextTime = now;
        return;
    }

    // Advance scheduled frame time.
    NextTime.tv_nsec += (long)ms * 1000000L;
    if (NextTime.tv_nsec >= 1000000000L)
    {
        NextTime.tv_sec++;
        NextTime.tv_nsec -= 1000000000L;
    }

    long long remaining =
        (NextTime.tv_sec - now.tv_sec) * 1000000000LL +
        (NextTime.tv_nsec - now.tv_nsec);

    if (remaining > 1000000LL)
        SDL_Delay((Uint32)(remaining / 1000000LL));

    // If we missed the frame by more than one frame period, resync.
    clock_gettime(CLOCK_MONOTONIC, &now);

    long long late =
        (now.tv_sec - NextTime.tv_sec) * 1000000000LL +
        (now.tv_nsec - NextTime.tv_nsec);

    if (late > (long long)ms * 1000000LL)
        NextTime = now;
}

#define SURFACE_PIXELS ((Uint32 *)(pMainSurface->pixels))

void CKZWindow::DrawLine(int x_from, int y_from, int x_to, int y_to, Uint32 color)
{
    if(!pMainSurface->pixels)
        return;

    const int stride = pMainSurface->pitch / sizeof(Uint32);

    int dx = std::abs(x_to - x_from);
    int sx = (x_from < x_to) ? 1 : -1;
    int dy = -std::abs(y_to - y_from);
    int sy = (y_from < y_to) ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        if (x_from >= 0 && y_from >= 0 &&
            x_from < pMainSurface->w &&
            y_from < pMainSurface->h)
        {
            SURFACE_PIXELS[y_from * stride + x_from] = color;
        }

        if (x_from == x_to && y_from == y_to)
            break;

        int e2 = err * 2;

        if (e2 >= dy)
        {
            err += dy;
            x_from += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y_from += sy;
        }
    }
}

void CKZWindow::DrawRect(int x_from, int y_from, int x_to, int y_to, Uint32 color)
{
    //evil but whatever
    DrawLine(x_from, y_from, x_to, y_from, color);
    DrawLine(x_from, y_to, x_to, y_to, color);
    DrawLine(x_from, y_from, x_from, y_to, color);
    DrawLine(x_to, y_from, x_to, y_to, color);
}

void CKZWindow::DrawFillRect(int x_from, int y_from, int x_to, int y_to, Uint32 color)
{
    SDL_Rect temp {x_from, y_from, x_to, y_to};
    SDL_FillSurfaceRect(pMainSurface, &temp, color);
}

void CKZWindow::DrawEllipse(int cx, int cy, int rx, int ry, Uint32 color)
{
    if(!pMainSurface->pixels)
        return;

    //cx--;
    //rx++;

    const int stride = pMainSurface->pitch / sizeof(Uint32);

    float fwidth = ((float)rx);
    float fheight = ((float)ry);

    float pointX;
    float pointY;
    float pointX2;
    float maxweig = fwidth;
    float maxalt = fheight;

    for(float drawy = -maxalt/1.4f; drawy < fheight/1.4f; drawy++)
    {
        pointX = std::abs(drawy / maxalt);
        pointX = std::sqrt(1.0f - pointX * pointX) * fwidth;
        pointX2 = pointX + cx;
        pointX = -pointX;

        pointX += cx;
        pointY = cy + drawy;

        pointX = std::roundf(pointX);
        pointY = std::roundf(pointY);
        pointX2 = std::roundf(pointX2);

        if ((int)pointX >= 0 && (int)pointY >= 0 &&
            (int)pointX < pMainSurface->w &&
            (int)pointY < pMainSurface->h)
        {
            SURFACE_PIXELS[(int)pointY * stride + (int)pointX] = color;
            SURFACE_PIXELS[(int)pointY * stride + (int)pointX2] = color;
        }
    }

    for(float drawx = -maxweig/1.4f; drawx < fwidth/1.4f; drawx++)
    {
        pointY = std::abs(drawx / maxweig);
        pointY = std::sqrt(1.0f - pointY * pointY) * fheight;
        pointX2 = pointY + cy;
        pointY = -pointY;

        pointY += cy;
        pointX = cx + drawx;

        pointX = std::roundf(pointX);
        pointY = std::roundf(pointY);
        pointX2 = std::roundf(pointX2);

        if ((int)pointX >= 0 && (int)pointY >= 0 &&
            (int)pointX < pMainSurface->w &&
            (int)pointY < pMainSurface->h)
        {
            SURFACE_PIXELS[(int)pointY * stride + (int)pointX] = color;
            SURFACE_PIXELS[(int)pointX2 * stride + (int)pointX] = color;
        }
    }
}

void CKZWindow::DrawFillEllipse(int cx, int cy, int rx, int ry, Uint32 color)
{
    const int stride = pMainSurface->pitch / sizeof(Uint32);

    float fwidth = ((float)rx);
    float fheight = ((float)ry);

    float pointX;
    float pointY;
    int linelength;
    float maxalt = fheight;

    for(float drawy = -maxalt; drawy < fheight; drawy++)
    {
        pointX = std::abs(drawy / maxalt);
        pointX = std::sqrt(1.0f - pointX * pointX) * fwidth;
        linelength = std::roundf(pointX) * 2;
        pointX = -pointX;

        pointX += cx;
        pointY = cy + drawy;

        pointX = std::roundf(pointX);
        pointY = std::roundf(pointY);

        for(int px = 0; px < linelength; px++)
        {
            if ((int)pointX + px >= 0 && (int)pointY >= 0 &&
                (int)pointX + px < pMainSurface->w &&
                (int)pointY < pMainSurface->h)
            {
                SURFACE_PIXELS[(int)pointY * stride + (int)pointX + px] = color;
            }
        }
    }
}

void CKZWindow::Begin3DDrawing(const char *pdrawmode)
{
    if(!pdrawmode)
    {
        return;
    }

    SWdraw mode = SWdraw::SW_DRAW_INVALID;

    switch (pdrawmode[0])
    {
    case 't':
    case 'T':
        mode = SWdraw::SW_TRIANGLES;
        break;
    case 'q':
    case 'Q':
        mode = SWdraw::SW_QUADS;
        break;
    case 'l':
    case 'L':
        mode = SWdraw::SW_LINES;
        break;
    case 'p':
    case 'P':
        mode = SWdraw::SW_POINTS;
        break;
    }

    if(mode != SW_DRAW_INVALID)
    {
        swBegin(mode);
    }
}

void CKZWindow::Add3DVertex(float x, float y, float z, Uint32 color, float u, float v)
{
    swTexCoord2f(u, v);
    swColor4ub((color & 0xff), (color >> 8) & 0xff, (color >> 16) & 0xff, (color >> 24) & 0xff);
    swVertex3f(x, y, z);
}

void CKZWindow::End3DDrawing()
{
    swEnd();
}

void CKZWindow::Set3DCamera(float x, float y, float z, float x_look, float y_look, float z_look)
{
    swLoadIdentity();

    float yaw   = std::atan2(z_look, x_look);
    float pitch = std::atan2(y_look, std::sqrt(x_look * x_look + z_look * z_look));

    // applied to vertices in reverse: translate -> yaw -> pitch
    swRotatef(pitch * (180.0f / M_PI), 1.f, 0.f, 0.f);
    swRotatef(yaw * (180.0f / M_PI) - 90.0f, 0.f, 1.f, 0.f);
    swTranslatef(-x, -y, -z);
}

#undef SURFACE_PIXELS
