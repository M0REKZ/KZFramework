
#include <string>
#include <cmath>
#include "images.h"
#include "window.h"
#include "files.h"

#include <SDL3_rotozoom.h>

CKZImages KZImages;

unsigned int CKZImages::LoadImage(unsigned int img)
{
    if(LoadedImages.find(img) != LoadedImages.end())
        return img;

    std::string filename = KZFiles.GetBasePath() + "data/images/" + std::to_string(img) + ".png";

    SDL_Surface * pImg = SDL_LoadPNG(filename.c_str());

    if(pImg)
    {
        LoadedImages[img] = pImg;
        return img;
    }

    SDL_Log("ERROR: Could not load image %d %s!\n", img, filename.c_str());

    //0 on error
    return 0;
}

bool CKZImages::IsImageSlotBusy(unsigned int img)
{
    return LoadedImages.find(img) != LoadedImages.end();
}

void CKZImages::RenderImage(unsigned int img, int x_from, int y_from, int w_from, int h_from, int x_to, int y_to, int w_to, int h_to)
{
    if(LoadedImages.find(img) == LoadedImages.end())
    {
        SDL_Log("ERROR: Cant render image %d because it is not loaded!\n", img);
        return;
    }
    SDL_Rect srcrect = {x_from, y_from, w_from, h_from};
    SDL_Rect dstrect = {x_to, y_to, w_to, h_to};

    SDL_BlitSurfaceScaled(LoadedImages.at(img), &srcrect, KZWindow.pMainSurface, &dstrect, SDL_SCALEMODE_PIXELART);
}

void CKZImages::RenderRotoZoom(unsigned int img, int x_from, int y_from, int w_from, int h_from, int cx_to, int cy_to, double zoomx, double zoomy, double angle)
{
    if(LoadedImages.find(img) == LoadedImages.end())
    {
        SDL_Log("ERROR: Cant render image %d because it is not loaded!\n", img);
        return;
    }
    SDL_Surface *srcimg = LoadedImages.at(img);

    //SDL3_gfx does zoom weird, lets do it separately
    SDL_Surface *clipsurf = SDL_CreateSurface(w_from * std::abs(zoomx), h_from * std::abs(zoomy), srcimg->format);

    if(!clipsurf)
    {
        return;
    }

    SDL_Rect srcrect = {x_from, y_from, w_from, h_from};
    SDL_Rect desttemprect = {0, 0, clipsurf->w, clipsurf->h};
    SDL_BlitSurfaceScaled(srcimg, &srcrect, clipsurf, &desttemprect, SDL_SCALEMODE_PIXELART);

    SDL_Surface *newimg = rotozoomSurfaceXY(clipsurf, angle, zoomx > 0 ? 1 : -1, zoomy > 0 ? 1 : -1, SMOOTHING_OFF);

    if(newimg)
    {
        
        SDL_Rect dstrect = {cx_to - newimg->w / 2, cy_to - newimg->h / 2, 0, 0};

        SDL_BlitSurface(newimg, nullptr, KZWindow.pMainSurface, &dstrect);
        SDL_DestroySurface(newimg);
    }
    SDL_DestroySurface(clipsurf);
}

void CKZImages::KeepAreaAsImage(int x_from, int y_from, int w_from, int h_from, unsigned int destimg)
{
    SDL_Surface *clipsurf = SDL_CreateSurface(w_from, h_from, KZWindow.pMainSurface->format);
    if(!clipsurf)
    {
        return;
    }

    SDL_Rect srcrect = {x_from, y_from, w_from, h_from};
    SDL_BlitSurface(KZWindow.pMainSurface, &srcrect, clipsurf, nullptr);

    UnloadImage(destimg);
    LoadedImages[destimg] = clipsurf;
}

void CKZImages::UnloadAllImages()
{
    for(auto &image : LoadedImages)
    {
        SDL_DestroySurface(image.second);
    }
    LoadedImages.clear();
}

void CKZImages::UnloadImage(unsigned int img)
{
    if(LoadedImages.find(img) == LoadedImages.end())
    {
        return;
    }
    SDL_DestroySurface(LoadedImages[img]);
    LoadedImages.erase(img);
}
