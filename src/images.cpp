
#include <string>
#include "images.h"
#include "window.h"

CKZImages KZImages;

unsigned int CKZImages::LoadImage(unsigned int img)
{
    if(LoadedImages.find(img) != LoadedImages.end())
        return img;

    std::string filename = "game/data/images/" + std::to_string(img) + ".png";

    SDL_Surface * pImg = SDL_LoadPNG(filename.c_str());

    if(pImg)
    {
        LoadedImages[img] = pImg;
        return img;
    }

    printf("ERROR: Could not load image %d %s!\n", img, filename.c_str());

    //0 on error
    return 0;
}

void CKZImages::RenderImage(unsigned int img, int x_from, int y_from, int w_from, int h_from, int x_to, int y_to, int w_to, int h_to)
{
    if(LoadedImages.find(img) == LoadedImages.end())
    {
        printf("ERROR: Cant render image %d because it is not loaded!\n", img);
        return;
    }
    SDL_Rect srcrect = {x_from, y_from, w_from, h_from};
    SDL_Rect dstrect = {x_to, y_to, w_to, h_to};

    SDL_BlitSurfaceScaled(LoadedImages.at(img), &srcrect, KZWindow.pMainSurface, &dstrect, SDL_SCALEMODE_PIXELART);
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
