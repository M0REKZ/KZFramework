#include "window.h"

CKZWindow KZWindow;

bool CKZWindow::Init()
{
    pMainWindow = SDL_CreateWindow("+KAIZO", Width, Height, SDL_WINDOW_RESIZABLE);
    if(!pMainWindow)
    {
        fprintf(stderr, "ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    pMainWindowRenderer = SDL_CreateRenderer(pMainWindow, nullptr);
    if(!pMainWindowRenderer)
    {
        fprintf(stderr, "ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    pMainSurface = SDL_CreateSurface(Width, Height, SDL_GetWindowPixelFormat(pMainWindow));
    if(!pMainSurface)
    {
        fprintf(stderr, "ERROR INIT SDL: %s\n", SDL_GetError());
        return false;
    }

    pMainTexture = SDL_CreateTexture(pMainWindowRenderer, SDL_GetWindowPixelFormat(pMainWindow), SDL_TEXTUREACCESS_STREAMING, Width, Height);
    if(!pMainTexture)
    {
        fprintf(stderr, "ERROR INIT SDL: %s\n", SDL_GetError());
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
        //
        SDL_RenderPresent(pMainWindowRenderer);
    }
}
